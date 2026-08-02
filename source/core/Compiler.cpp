#include "../../include/ilic/Compiler.h"
#include "../../include/ilic/Semantic.h"

#include "CompilerContext.h"
#include "SnapshotPipeline.h"
#include "ilic/DiagnosticPipeline.h"
#include "incremental/ParsedSourceCache.h"
#include "incremental/RootAnalysisCache.h"
#include "incremental/SourceDependencyIndex.h"
#include "incremental/SourceModelIndex.h"
#include "incremental/SourceImpactAnalyzer.h"
#include "incremental/IncrementalMemoryEstimator.h"
#include "incremental/IncrementalFingerprint.h"
#include "../input/ili1/Ili1Input.h"
#include "../input/ili2/Ili2Input.h"
#include "../input/ili2/InterlisModel.h"
#include "../metamodel/SemanticChecker.h"
#include "../metamodel/TranslationChecker.h"
#include "../util/Logger.h"
#include "../util/StringUtil.h"

#include <chrono>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <set>
#include <sstream>

namespace ilic {
namespace {

metamodel::MetaElement *findExternalMetaTarget(
   metamodel::Package *package,const std::string &path,const std::string &prefix)
{
   if (package == nullptr) return nullptr;
   const std::string packagePath = prefix.empty() ? package->Name : prefix + "." + package->Name;
   if (packagePath == path) return package;
   for (auto *element : package->Element) {
      if (element == nullptr) continue;
      const std::string elementPath = packagePath + "." + element->Name;
      if (elementPath == path) return element;
      if (auto *childPackage = dynamic_cast<metamodel::Package *>(element)) {
         if (auto *found = findExternalMetaTarget(childPackage,path,packagePath)) return found;
      }
      if (auto *viewable = dynamic_cast<metamodel::Class *>(element)) {
         for (auto *attribute : viewable->ClassAttribute)
            if (attribute != nullptr && elementPath + "." + attribute->Name == path) return attribute;
         for (auto *parameter : viewable->ClassParameter)
            if (parameter != nullptr && elementPath + "." + parameter->Name == path) return parameter;
         for (auto *role : viewable->Role)
            if (role != nullptr && elementPath + "." + role->Name == path) return role;
         for (auto *constraint : viewable->Constraint)
            if (constraint != nullptr && elementPath + "." + constraint->Name == path) return constraint;
      }
      if (auto *function = dynamic_cast<metamodel::FunctionDef *>(element)) {
         for (auto *argument : function->Argument)
            if (argument != nullptr && elementPath + "." + argument->Name == path) return argument;
      }
   }
   return nullptr;
}

void applyExternalMetaAttributes(const CompilationRequest &request,
   const metamodel::MetaModelStore &store,util::Logger &logger)
{
   for (const auto &external : request.externalMetaAttributes) {
      metamodel::MetaElement *target = nullptr;
      for (auto *model : store.models()) {
         target = findExternalMetaTarget(model,external.element,"");
         if (target != nullptr) break;
      }
      if (target == nullptr) {
         logger.error(util::DiagnosticId::MetaTarget,
            "external meta attribute target " + external.element + " not found",-1);
         continue;
      }
      if (external.name == "ili2c.translationOf") {
         if (auto *model = dynamic_cast<metamodel::Model *>(target))
            model->_translationOfName = external.value;
         else
            logger.error(util::DiagnosticId::MetaTarget,
               "ili2c.translationOf may only target a model",-1);
      }
   }
}

std::string joinDirectories(const std::vector<std::string> &directories)
{
   std::ostringstream value;
   for (std::size_t i = 0; i < directories.size(); ++i) {
      if (i != 0) value << ';';
      value << directories[i];
   }
   return value.str();
}

std::string diagnosticTranscriptLine(const Diagnostic &diagnostic)
{
   const bool error = diagnostic.treatedAsError ||
      diagnostic.severity == DiagnosticSeverity::Error;
   std::string prefix = error ? "err:" :
      (diagnostic.severity == DiagnosticSeverity::Warning ? "wrn:" : "inf:");
   std::string line = prefix + (error || diagnostic.severity == DiagnosticSeverity::Warning ?
      "    " : " ");
   if (diagnostic.range.valid) {
      line += diagnostic.range.uri + ":" +
         std::to_string(diagnostic.range.start.line + 1) + ":" +
         std::to_string(diagnostic.range.start.character + 1) + ": ";
   }
   line += diagnostic.message;
   return line;
}

void appendNewEvents(std::vector<std::string> &transcript,
   std::size_t &diagnosticIndex,std::size_t &logIndex,const util::Logger &logger)
{
   const auto &diagnostics = logger.getDiagnostics();
   while (diagnosticIndex < diagnostics.size())
      transcript.push_back(diagnosticTranscriptLine(diagnostics[diagnosticIndex++]));

   const auto &events = logger.getLogEvents();
   while (logIndex < events.size()) {
      const auto &event = events[logIndex++];
      const std::string prefix = event.level == LogLevel::Error ? "err:" :
         (event.level == LogLevel::Warning ? "wrn:" :
         (event.level == LogLevel::Debug ? "dbg:" : "inf:"));
      transcript.push_back(prefix + "    " + event.message);
   }
}

std::string completionTranscriptLine(int errorCount,int warningCount,
   const std::string &timestamp)
{
   std::string line = "inf: ilic completed with";
   if (errorCount == 0) line += " no errors";
   else if (errorCount == 1) line += " 1 error";
   else line += " " + std::to_string(errorCount) + " errors";
   if (warningCount == 0) line += ", no warnings";
   else if (warningCount == 1) line += ", 1 warning";
   else line += ", " + std::to_string(warningCount) + " warnings";
   return line + " " + timestamp;
}

std::string compilerRunTimestamp()
{
   const std::time_t now = std::chrono::system_clock::to_time_t(
      std::chrono::system_clock::now());
   std::tm local{};
#if defined(_WIN32)
   localtime_s(&local,&now);
#else
   localtime_r(&now,&local);
#endif
   char timestamp[20]{};
   if (std::strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",&local) == 0)
      return "0000-00-00 00:00:00";
   return timestamp;
}

std::string incrementalHash(const std::string &bytes)
{
   std::uint64_t hash = 1469598103934665603ULL;
   for (const unsigned char byte : bytes) {
      hash ^= byte;
      hash *= 1099511628211ULL;
   }
   std::ostringstream result;
   result << std::hex << std::setw(16) << std::setfill('0') << hash;
   return result.str();
}

ilic::SourceIdentity identityFor(const ilic::SourceManager &sources,
   const std::string &uri)
{
   if (const auto *identity = sources.identity(uri)) return *identity;
   const auto *source = sources.get(uri);
   if (source == nullptr) return {uri,0,0,0,"",0};
   return {uri,source->version,0,0,incrementalHash(source->text),source->text.size()};
}

ilic::SourceIdentity identityFor(const ilic::SourceManager &sources,
   const ilic::SourceBuffer &source)
{
   if (const auto *identity = sources.identity(source.uri)) return *identity;
   return {source.uri,source.version,0,0,incrementalHash(source.text),source.text.size()};
}

std::string requestKey(const CompilationRequest &request)
{
   std::ostringstream key;
   key << (request.options.autoSearch ? '1' : '0') << ':'
      << (request.options.warningsAsErrors ? '1' : '0') << ':';
   for (const auto &root : request.roots) key << root.size() << ':' << root << ';';
   key << '|';
   for (const auto &directory : request.options.modelDirectories)
      key << directory.size() << ':' << directory << ';';
   key << '|';
   for (const auto &attribute : request.externalMetaAttributes)
      key << attribute.element.size() << ':' << attribute.element << ':'
         << attribute.name.size() << ':' << attribute.name << ':'
         << attribute.value.size() << ':' << attribute.value << ';';
   return key.str();
}

std::string rootKey(const std::string &base,
   const std::vector<std::string> &uris,const ilic::SourceManager &sources)
{
   std::ostringstream key;
   key << base << "|fingerprint=" << detail::currentCompilerFingerprint().canonicalString() << ';';
   for (const auto &uri : uris) {
      const auto identity = identityFor(sources,uri);
      key << uri.size() << ':' << uri << ':' << identity.contentRevision << ':'
         << identity.contentHash << ':' << identity.byteLength << ';';
   }
   return key.str();
}

std::size_t parsedRetainedBytes(const std::string &text,
   const detail::SnapshotBundle &bundle)
{
   return text.size()
      + bundle.syntax.tokens.size() * sizeof(SyntaxToken)
      + bundle.syntax.nodes.size() * sizeof(SyntaxNode)
      + bundle.editor.declarations.size() * sizeof(EditorDeclaration)
      + bundle.editor.references.size() * sizeof(EditorReference);
}

void appendInputFileTranscript(std::vector<std::string> &transcript,
   util::IliFile *file)
{
   if (file == nullptr || file->getFilePath() == "INTERLIS") return;
   std::string models;
   for (const auto &model : file->getModels()) {
      if (!models.empty()) models += ",";
      models += model;
   }
   transcript.push_back("inf:    " + file->getFilePath() + ", iliversion=" +
      file->getIliVersion() + ", models=" + models + ", " +
      (file->getAutoSearch() ? "auto search" : "command line"));
}

ilic::SourceBuffer sourceFor(detail::CompilerContext &context,util::IliFile *file)
{
   const std::string uri = file->getFilePath();
   if (const auto *source = context.compilationSources().get(uri)) return *source;
   if (uri == "INTERLIS") {
      const auto &builtin = context.compilationSources().rememberBuiltinSource(
         uri,input::getInterlisModel23());
      return builtin;
   }
   return {uri,util::load_filtered_string_from_file(uri),0};
}

bool compileFile(
   detail::CompilerContext &context,
   util::IliFile *file,
   std::vector<util::IliFile *> &compiledFiles,
   std::set<std::string> &compiledModels,
   std::vector<std::string> &transcript,
   std::size_t &diagnosticIndex,
   std::size_t &logIndex)
{
   if (file == nullptr) return false;
   util::Logger &logger = context.logger();
   if (file->getIliVersion() != "1.0" && file->getIliVersion() != "2.3"
       && file->getIliVersion() != "2.4") {
      logger.setCurrentSource(file->getFilePath());
      logger.error(util::DiagnosticId::InputUnsupportedVersion,
         "unsupported INTERLIS version " + file->getIliVersion()
            + "; supported versions are 1.0, 2.3, and 2.4",1);
      appendNewEvents(transcript,diagnosticIndex,logIndex,logger);
      return true;
   }
   for (auto *compiled : compiledFiles) if (compiled == file) return true;

   for (const auto &required : file->getRequiredModels()) {
      bool found = compiledModels.find(required) != compiledModels.end();
      for (const auto &local : file->getModels()) found = found || local == required;
      if (!found) return false;
   }

   transcript.push_back("inf: compiling " + file->getFilePath() + " ...");
   const int errorsBefore = logger.getErrorCount();
   const ilic::SourceBuffer source = sourceFor(context,file);
   if (auto artifact = context.parsedSourceArtifact(source)) {
      artifact->reportParserDiagnostics(logger);
      if (artifact->parserDiagnostics().empty() && artifact->supportsMetaModelBuild())
         artifact->buildMetaModel(context.builder(),logger);
      else if (artifact->parserDiagnostics().empty())
         logger.error(util::DiagnosticId::CompilerInternal,
            "tolerant parser artifact cannot be used for compilation",-1);
   }
   else if (file->getIliVersion() == "1.0")
      input::parseIli1(source,context.builder(),logger);
   else
      input::parseIli2(source,context.builder(),logger);
   appendNewEvents(transcript,diagnosticIndex,logIndex,logger);
   compiledFiles.push_back(file);
   if (file->getFilePath() == "INTERLIS") compiledModels.insert("INTERLIS");
   for (const auto &model : file->getModels()) compiledModels.insert(model);
   transcript.push_back("inf: " + file->getFilePath() +
      (logger.getErrorCount() > errorsBefore ? " compiled with errors." : " compiled."));
   return true;
}

} // namespace

CompilationResult compileRun(detail::CompilerContext &context,
   const CompilationRequest &request,std::vector<std::string> &lastCompilationSourceUris);

struct CompilerSession::Impl {
   SourceManager sources;
   std::mutex mutex;
   detail::ParsedSourceCache parsedCache;
   detail::RootAnalysisCache rootCache;
   detail::SourceModelIndex modelIndex;
   detail::SourceDependencyIndex dependencyIndex;
   IncrementalStats stats;
   IncrementalTrace trace;

   Impl(const IncrementalCacheOptions &options)
      : parsedCache(options.parsed),rootCache(options.root) {}

   std::shared_ptr<const detail::ParsedSourceCacheEntry> ensureParsed(
      const SourceBuffer &source,detail::ParseMode mode)
   {
      const auto identity = identityFor(sources,source);
      const auto before = parsedCache.metrics();
      auto result = parsedCache.getOrBuild(source,identity,mode,"generated-grammar-v1",
         [this](const SourceBuffer &value,detail::ParseMode requested) {
            return detail::SnapshotPipeline(sources).build(value,requested);
         });
      const auto after = parsedCache.metrics();
      const bool built = mode == detail::ParseMode::StrictCompiler
         ? after.strictBuilds > before.strictBuilds
         : after.tolerantBuilds > before.tolerantBuilds;
      const bool hit = mode == detail::ParseMode::StrictCompiler
         ? after.strictHits > before.strictHits
         : after.tolerantHits > before.tolerantHits;
      if (built) {
         if (mode == detail::ParseMode::StrictCompiler) trace.strictParserBuilds.push_back(source.uri);
         else trace.tolerantParserBuilds.push_back(source.uri);
         trace.parserMisses.push_back(source.uri);
      } else if (hit) {
         if (mode == detail::ParseMode::StrictCompiler) trace.strictParserHits.push_back(source.uri);
         else trace.tolerantParserHits.push_back(source.uri);
         trace.parserHits.push_back(source.uri);
      }
      return result;
   }

   detail::ParsedSourceArtifactPtr parsedArtifact(const SourceBuffer &source)
   {
      const auto entry = ensureParsed(source,detail::ParseMode::StrictCompiler);
      return entry == nullptr ? detail::ParsedSourceArtifactPtr{} : entry->artifact;
   }

   void syncCacheStats()
   {
      const auto parsed = parsedCache.metrics();
      const auto root = rootCache.metrics();
      stats.strictParserBuilds = parsed.strictBuilds;
      stats.tolerantParserBuilds = parsed.tolerantBuilds;
      stats.strictParserHits = parsed.strictHits;
      stats.tolerantParserHits = parsed.tolerantHits;
      stats.parserBuilds = parsed.strictBuilds + parsed.tolerantBuilds;
      stats.parserHits = parsed.strictHits + parsed.tolerantHits;
      stats.parserEvictions = parsed.evictions;
      stats.parserEntries = parsed.entries;
      stats.parserRetainedBytes = parsed.retainedBytes;
      stats.parserBytes = parsed.retainedBytes;
      stats.rootEntries = root.entries;
      stats.rootRetainedBytes = root.retainedBytes;
      stats.rootAnalysisEvictions = root.evictions;
      trace.bytesRetained = parsed.retainedBytes + root.retainedBytes;
   }
};

CompilerSession::CompilerSession() : CompilerSession(IncrementalCacheOptions{}) {}

CompilerSession::CompilerSession(IncrementalCacheOptions cacheOptions)
   : impl_(std::make_unique<Impl>(cacheOptions)) {}
CompilerSession::~CompilerSession() = default;

void CompilerSession::putSource(std::string uri,std::string utf8,std::uint64_t version)
{
   (void)updateSource(std::move(uri),std::move(utf8),version);
}

SourceUpdateResult CompilerSession::updateSource(std::string uri,std::string utf8,
   std::uint64_t version)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   SourceUpdateResult result = impl_->sources.update(std::move(uri),std::move(utf8),version);
   impl_->trace = {};
   impl_->trace.operation = "updateSource";
   impl_->trace.reasons.push_back(result.current.uri.empty() ? "rejected" : result.current.uri);
   switch (result.kind) {
      case SourceUpdateKind::Added: ++impl_->stats.sourceAdds; break;
      case SourceUpdateKind::Reintroduced: ++impl_->stats.sourceReintroductions; break;
      case SourceUpdateKind::Unchanged: ++impl_->stats.sourceNoOps; break;
      case SourceUpdateKind::VersionOnly: ++impl_->stats.versionOnlyUpdates; break;
      case SourceUpdateKind::ContentChanged: ++impl_->stats.contentChanges; break;
      case SourceUpdateKind::Rejected: ++impl_->stats.rejectedUpdates; break;
      case SourceUpdateKind::Removed: break;
   }
   if (result.accepted) {
      std::size_t parserReleased = 0;
      if (result.kind == SourceUpdateKind::ContentChanged
         || result.kind == SourceUpdateKind::Reintroduced)
         parserReleased = impl_->parsedCache.invalidateUri(result.current.uri);

      const auto *source = impl_->sources.get(result.current.uri);
      const auto *beforeEntry = impl_->modelIndex.byUri(result.current.uri);
      const std::optional<detail::SourceModelRecord> before = beforeEntry == nullptr
         ? std::optional<detail::SourceModelRecord>{}
         : std::optional<detail::SourceModelRecord>(*beforeEntry);
      detail::SourceImpactAnalyzer impactAnalyzer;
      if (source != nullptr && result.kind != SourceUpdateKind::Unchanged
         && result.kind != SourceUpdateKind::VersionOnly) {
         const auto entry = impl_->ensureParsed(*source,detail::ParseMode::StrictCompiler);
         if (entry != nullptr && entry->artifact != nullptr) {
            const auto modelUpdate = impl_->modelIndex.replace(result.current,
               entry->artifact->header(),entry->artifact->parserDiagnostics().empty());
            impl_->dependencyIndex.replaceSourceDependencies(result.current.uri,
               modelUpdate.current.requiredModels);
         }
      }
      const auto *after = impl_->modelIndex.byUri(result.current.uri);
      const auto impact = impactAnalyzer.analyze(result,
         before == std::nullopt ? nullptr : &*before,after,impl_->rootCache);
      detail::RootInvalidationResult invalidated;
      if (result.kind == SourceUpdateKind::ContentChanged
         || result.kind == SourceUpdateKind::Reintroduced)
         invalidated = impl_->rootCache.invalidateBySource(result.current.uri);
      else if (result.kind == SourceUpdateKind::Added) {
         const auto *record = after;
         if (impact.invalidateAllRoots || record == nullptr || !record->syntacticallyUsable
            || record->declaredModels.empty()) {
            invalidated = impl_->rootCache.invalidateIf(
               [](const detail::RootAnalysisCacheEntry &) { return true; });
         }
         else {
            invalidated = impl_->rootCache.invalidateIf([&](const detail::RootAnalysisCacheEntry &entry) {
               for (const auto &model : record->declaredModels) {
                  if (std::find(entry.missingModels.begin(),entry.missingModels.end(),model)
                     != entry.missingModels.end()) return true;
                  if (std::find(entry.resolvedModels.begin(),entry.resolvedModels.end(),model)
                     != entry.resolvedModels.end()) return true;
               }
               return false;
            });
         }
      }
      impl_->stats.invalidatedRootEntries += invalidated.invalidatedKeyIds.size();
      impl_->trace.bytesReleased += parserReleased + invalidated.releasedBytes;
      impl_->trace.invalidatedRoots = invalidated.invalidatedRoots;
   }
   impl_->syncCacheStats();
   return result;
}

bool CompilerSession::removeSource(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   impl_->trace = {};
   impl_->trace.operation = "removeSource";
   impl_->trace.reasons.push_back(uri);
   const bool removed = impl_->sources.remove(uri);
   if (!removed) return false;
   ++impl_->stats.sourceRemoves;
   const std::size_t parserReleased = impl_->parsedCache.invalidateUri(uri);
   impl_->modelIndex.remove(uri);
   impl_->dependencyIndex.removeSource(uri);
   const auto invalidated = impl_->rootCache.invalidateIf(
      [](const detail::RootAnalysisCacheEntry &) { return true; });
   impl_->stats.invalidatedRootEntries += invalidated.invalidatedKeyIds.size();
   impl_->trace.bytesReleased = parserReleased + invalidated.releasedBytes;
   impl_->trace.invalidatedRoots = invalidated.invalidatedRoots;
   impl_->syncCacheStats();
   return true;
}

SourceManager &CompilerSession::sources() { return impl_->sources; }
const SourceManager &CompilerSession::sources() const { return impl_->sources; }

SyntaxSnapshot CompilerSession::parse(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   impl_->trace = {};
   impl_->trace.operation = "parse";
   impl_->trace.roots.push_back(uri);
   ++impl_->stats.parseRequests;
   const auto *source = impl_->sources.get(uri);
   if (source == nullptr) return detail::SnapshotPipeline(impl_->sources).syntax(uri);
   const auto entry = impl_->ensureParsed(*source,detail::ParseMode::StrictCompiler);
   ++impl_->stats.syntaxMaterializations;
   impl_->syncCacheStats();
   if (entry == nullptr) return detail::SnapshotPipeline(impl_->sources).syntax(uri);
   SyntaxSnapshot syntax = entry->syntax;
   syntax.documentVersion = source->version;
   return syntax;
}

EditorSnapshot CompilerSession::editorSnapshot(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   impl_->trace = {};
   impl_->trace.operation = "editorSnapshot";
   impl_->trace.roots.push_back(uri);
   ++impl_->stats.editorSnapshotRequests;
   const auto *source = impl_->sources.get(uri);
   if (source == nullptr) return detail::SnapshotPipeline(impl_->sources).editor(uri);
   const auto entry = impl_->ensureParsed(*source,detail::ParseMode::TolerantEditor);
   ++impl_->stats.editorMaterializations;
   impl_->syncCacheStats();
   if (entry == nullptr) return detail::SnapshotPipeline(impl_->sources).editor(uri);
   EditorSnapshot editor = entry->editor;
   editor.documentVersion = source->version;
   return editor;
}

SemanticSnapshot CompilerSession::analyze(const CompilationRequest &request)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return compileAndAnalyzeUnlocked(request).semantic;
}

CompilationAnalysisResult CompilerSession::compileAndAnalyze(const CompilationRequest &request)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return compileAndAnalyzeUnlocked(request);
}

CompilationAnalysisResult CompilerSession::compileAndAnalyzeUnlocked(
   const CompilationRequest &request)
{
   impl_->trace = {};
   impl_->trace.operation = "compileAndAnalyze";
   impl_->trace.roots = request.roots;
   ++impl_->stats.compileRequests;
   ++compileInvocationCount_;
   ++impl_->stats.compilationInvocations;
   const std::string baseKey = requestKey(request);
   if (auto cached = impl_->rootCache.findExact(baseKey,impl_->sources)) {
      ++impl_->stats.rootAnalysisHits;
      impl_->stats.reusedClosureSources += cached->closureUris.size();
      CompilationAnalysisResult result = cached->result;
      for (auto &version : result.semantic.documentVersions)
         if (const auto *source = impl_->sources.get(version.first)) version.second = source->version;
      for (auto &syntax : result.syntax)
         if (const auto *source = impl_->sources.get(syntax.uri)) syntax.documentVersion = source->version;
      for (auto iterator = result.compilation.transcript.rbegin();
         iterator != result.compilation.transcript.rend(); ++iterator) {
         if (iterator->rfind("inf: ilic completed with ",0) == 0) {
            const std::size_t timestamp = iterator->rfind(' ');
            if (timestamp != std::string::npos)
               *iterator = iterator->substr(0,timestamp + 1) + compilerRunTimestamp();
            break;
         }
      }
      impl_->trace.planKind = "ExactCacheHit";
      impl_->trace.closure = cached->closureUris;
      impl_->trace.reasons.push_back("root cache hit");
      impl_->syncCacheStats();
      return result;
   }
   ++impl_->stats.rootAnalysisMisses;
   ++impl_->stats.compileExecutions;
   const auto parserBefore = impl_->parsedCache.metrics();
   detail::CompilerContext context(impl_->sources,request.options,
      [this](const SourceBuffer &source) { return impl_->parsedArtifact(source); });
   CompilationAnalysisResult result;
   std::vector<std::string> compilationSourceUris;
   result.compilation = compileRun(context,request,compilationSourceUris);
   for (const auto &uri : compilationSourceUris) {
      if (impl_->sources.get(uri) == nullptr) continue;
      const auto entry = impl_->ensureParsed(*impl_->sources.get(uri),detail::ParseMode::StrictCompiler);
      if (entry == nullptr) continue;
      ++impl_->stats.syntaxMaterializations;
      SyntaxSnapshot syntax = entry->syntax;
      syntax.documentVersion = impl_->sources.get(uri)->version;
      result.syntax.push_back(std::move(syntax));
   }
   result.semantic = buildSemanticSnapshot(impl_->sources,request,
      result.compilation,compilationSourceUris,&result.syntax,
      &context.models());
   const std::string key = rootKey(baseKey,compilationSourceUris,impl_->sources);
   detail::RootAnalysisCacheEntry cached;
   cached.baseKey = baseKey;
   cached.key = key;
   cached.closureUris = compilationSourceUris;
   for (const auto &uri : compilationSourceUris)
      cached.identities.push_back(identityFor(impl_->sources,uri));
   cached.hadMissingModels = !result.compilation.missingModels.empty();
   cached.missingModels = result.compilation.missingModels;
   for (const auto &model : result.compilation.models) cached.resolvedModels.push_back(model.name);
   cached.result = result;
   impl_->rootCache.insert(std::move(cached));
   ++impl_->stats.rootAnalysisBuilds;
   const auto parserAfter = impl_->parsedCache.metrics();
   const std::size_t reparsed = static_cast<std::size_t>(
      parserAfter.strictBuilds - parserBefore.strictBuilds);
   impl_->stats.reparsedClosureSources += reparsed;
   impl_->stats.reusedClosureSources += compilationSourceUris.size() > reparsed
      ? compilationSourceUris.size() - reparsed : 0;
   impl_->trace.planKind = "RebuildWithParseReuse";
   impl_->trace.closure = compilationSourceUris;
   impl_->trace.reasons.push_back("root cache miss");
   impl_->syncCacheStats();
   return result;
}

CompilationResult compileRun(detail::CompilerContext &context,
   const CompilationRequest &request,std::vector<std::string> &lastCompilationSourceUris)
{
   lastCompilationSourceUris.clear();
   util::Logger &logger = context.logger();
   logger.setSilent(true);
   logger.setCategory("imports");

   CompilationResult result;
   std::vector<std::string> transcript{
      "inf: ilic " + std::string(version()),"inf:",
      "inf: loading ili files from command line ..."};
   std::size_t transcriptedDiagnostics = 0;
   std::size_t transcriptedLogs = 0;

   auto finish = [&]() {
      result.errorCount = logger.getErrorCount();
      result.warningCount = logger.getWarningCount();
      result.success = result.errorCount == 0 && !result.cancelled;
      appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs,logger);
      std::vector<DiagnosticCandidate> candidates;
      candidates.reserve(logger.getDiagnostics().size());
      for (const auto &diagnostic : logger.getDiagnostics()) {
         DiagnosticCandidate candidate;
         candidate.diagnostic = diagnostic;
         candidate.producer = "logger";
         candidates.push_back(std::move(candidate));
      }
      result.diagnostics = DiagnosticPipeline{}.publish(std::move(candidates)).values;
      result.logs = logger.getLogEvents();
      transcript.push_back("inf:");
      transcript.push_back(completionTranscriptLine(result.errorCount,result.warningCount,
         compilerRunTimestamp()));
      result.transcript = std::move(transcript);
      return result;
   };

   try {
      for (const auto &root : request.roots) {
         transcript.push_back("inf:    loading " + root + " ...");
         if (context.files().loadByFile(root) == nullptr) {
            logger.error(util::DiagnosticId::InputLoad,
               "unable to load root source " + root,-1);
            appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs,logger);
            transcript.push_back("inf:    not done.");
         }
         else transcript.push_back("inf:    done.");
      }
      transcript.push_back("inf: done.");
      if (context.files().files().empty()) return finish();

      std::string iliVersion = context.files().files().back()->getIliVersion();
      transcript.push_back("inf:");
      transcript.push_back("inf: loading imported models ...");
      std::map<std::string,bool> loaded;
      std::set<std::string> reportedMissing;
      bool progress = true;
      while (progress) {
         progress = false;
         const std::size_t before = context.files().files().size();
         for (std::size_t index = 0; index < context.files().files().size(); ++index) {
            auto *file = context.files().files()[index];
            for (const auto &model : file->getRequiredModels()) {
               if (model == "INTERLIS" || loaded[model]) continue;
               util::IliFile *resolved = context.files().loadByModel(model,iliVersion);
               if (resolved == nullptr) {
                  if (reportedMissing.insert(model).second) {
                     result.missingModels.push_back(model);
                     logger.error(util::DiagnosticId::NameModelNotFound,
                        "model " + model + " not found.",-1);
                     appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs,logger);
                     transcript.push_back("inf:    model " + model + " not found.");
                  }
               }
               else {
                  loaded[model] = true;
                  transcript.push_back("inf:    found in " + resolved->getFilePath() + ".");
               }
            }
         }
         progress = context.files().files().size() > before;
      }
      transcript.push_back("inf: done.");

      for (auto *file : context.files().files())
         if (file != nullptr) lastCompilationSourceUris.push_back(file->getFilePath());
      transcript.push_back("inf:");
      transcript.push_back("inf: all input files are:");
      for (auto *file : context.files().files()) appendInputFileTranscript(transcript,file);
      transcript.push_back("inf: done.");

      std::vector<util::IliFile *> compiledFiles;
      std::set<std::string> compiledModels;
      compileFile(context,context.files().loadByModel("INTERLIS",iliVersion),compiledFiles,
         compiledModels,transcript,transcriptedDiagnostics,transcriptedLogs);
      for (std::size_t pass = 0; pass <= context.files().files().size(); ++pass) {
         bool allCompiled = true;
         for (auto *file : context.files().files())
            allCompiled = compileFile(context,file,compiledFiles,compiledModels,transcript,
               transcriptedDiagnostics,transcriptedLogs) && allCompiled;
         if (allCompiled) break;
         if (pass == context.files().files().size()) {
            logger.error(util::DiagnosticId::ModelDependency,
               "unable to order model dependencies",-1);
            appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs,logger);
         }
      }

      applyExternalMetaAttributes(request,context.models(),logger);
      metamodel::check_model_semantics(context.models(),logger);
      metamodel::check_model_translations(context.models(),logger);
      appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs,logger);
      for (auto *model : context.models().models()) {
         CompiledModel compiled{model->Name,model->iliVersion,model->_ilifile,{}};
         for (auto *attribute : model->MetaAttribute)
            if (attribute != nullptr) compiled.metaAttributes.push_back({attribute->Name,attribute->Value});
         result.models.push_back(std::move(compiled));
      }
      transcript.push_back("inf:");
      transcript.push_back("inf: all models are:");
      for (const auto &model : result.models)
         transcript.push_back("inf:    model " + model.name + ", iliversion=" +
            model.iliVersion + ", file=" + model.uri);
      transcript.push_back("inf: done.");
      return finish();
   }
   catch (const util::CompilerAbort &error) {
      logger.error(util::DiagnosticId::CompilerInternal,
         std::string("internal compiler failure: ") + error.what(),-1);
      return finish();
   }
}

CompilationResult CompilerSession::compile(const CompilationRequest &request)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   ++compileInvocationCount_;
   ++impl_->stats.compileRequests;
   ++impl_->stats.compileExecutions;
   ++impl_->stats.compilationInvocations;
   detail::CompilerContext context(impl_->sources,request.options,
      [this](const SourceBuffer &source) { return impl_->parsedArtifact(source); });
   std::vector<std::string> compilationSourceUris;
   auto result = compileRun(context,request,compilationSourceUris);
   impl_->syncCacheStats();
   return result;
}

IncrementalStats CompilerSession::incrementalStats() const
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   impl_->syncCacheStats();
   return impl_->stats;
}

IncrementalTrace CompilerSession::lastIncrementalTrace() const
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return impl_->trace;
}

IncrementalCacheSnapshot CompilerSession::incrementalCacheSnapshot() const
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   const auto parsed = impl_->parsedCache.metrics();
   const auto root = impl_->rootCache.metrics();
   IncrementalCacheSnapshot snapshot;
   snapshot.parserEntries = parsed.entries;
   snapshot.parserRetainedBytes = parsed.retainedBytes;
   snapshot.parserEvictions = parsed.evictions;
   snapshot.rootEntries = root.entries;
   snapshot.rootRetainedBytes = root.retainedBytes;
   snapshot.rootEvictions = root.evictions;
   snapshot.parserInvariants = impl_->parsedCache.checkInvariants();
   snapshot.rootInvariants = impl_->rootCache.checkInvariants();
   return snapshot;
}

void CompilerSession::resetIncrementalStats()
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   impl_->parsedCache.resetMetrics();
   impl_->rootCache.resetMetrics();
   impl_->stats.reset();
   impl_->syncCacheStats();
}

void CompilerSession::clearIncrementalCaches()
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   const std::size_t parsedReleased = impl_->parsedCache.clear();
   const auto rootReleased = impl_->rootCache.clear();
   impl_->trace = {};
   impl_->trace.operation = "clearIncrementalCaches";
   impl_->trace.bytesReleased = parsedReleased + rootReleased.releasedBytes;
   impl_->syncCacheStats();
}

const char *version() { return "0.9.9"; }

} // namespace ilic
