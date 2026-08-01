#include "../../include/ilic/Compiler.h"
#include "../../include/ilic/Semantic.h"

#include "CompilerContext.h"
#include "SnapshotPipeline.h"
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
   key << base << "|grammar=0.9.9|abi=1|builtin=INTERLIS-2.3;";
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
      if (artifact->parserDiagnostics().empty())
         artifact->buildMetaModel(context.builder(),logger);
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
   struct ParsedCacheEntry {
      std::string uri;
      std::string hash;
      std::string text;
      detail::SnapshotBundle bundle;
      std::size_t retainedBytes = 0;
      std::uint64_t lastUsed = 0;
   };

   struct RootCacheEntry {
      std::string baseKey;
      std::string key;
      std::vector<std::string> closure;
      std::vector<SourceIdentity> identities;
      bool hadMissingModels = false;
      CompilationAnalysisResult result;
      std::size_t retainedBytes = 0;
      std::uint64_t lastUsed = 0;
   };

   SourceManager sources;
   std::mutex mutex;
   std::map<std::string,ParsedCacheEntry> parsedCache;
   std::map<std::string,RootCacheEntry> rootCache;
   std::size_t parsedBytes = 0;
   std::size_t rootBytes = 0;
   std::uint64_t cacheClock = 0;
   ParsedSourceCacheOptions parsedOptions;
   RootAnalysisCacheOptions rootOptions;
   IncrementalStats stats;
   IncrementalTrace trace;

   detail::SnapshotBundle ensureParsed(const SourceBuffer &source)
   {
      const auto identity = identityFor(sources,source);
      const std::string key = source.uri + "\n" + identity.contentHash + ":"
         + std::to_string(identity.byteLength);
      auto found = parsedCache.find(key);
      if (found != parsedCache.end() && found->second.text == source.text) {
         ++stats.parserHits;
         found->second.lastUsed = ++cacheClock;
         return found->second.bundle;
      }
      auto bundle = detail::SnapshotPipeline(sources).build(source,true);
      ParsedCacheEntry entry{source.uri,identity.contentHash,source.text,bundle,
         parsedRetainedBytes(source.text,bundle),++cacheClock};
      retainParsed(key,std::move(entry));
      ++stats.parserBuilds;
      return bundle;
   }

   detail::ParsedSourceArtifactPtr parsedArtifact(const SourceBuffer &source)
   {
      return ensureParsed(source).artifact;
   }

   void retainParsed(std::string key,ParsedCacheEntry entry)
   {
      auto old = parsedCache.find(key);
      if (old != parsedCache.end()) {
         parsedBytes -= old->second.retainedBytes;
         parsedCache.erase(old);
      }
      parsedBytes += entry.retainedBytes;
      parsedCache.emplace(std::move(key),std::move(entry));
      while (!parsedCache.empty() && (parsedCache.size() > parsedOptions.maxEntries
         || parsedBytes > parsedOptions.maxRetainedBytes)) {
         auto victim = std::min_element(parsedCache.begin(),parsedCache.end(),
            [](const auto &left,const auto &right) {
               return left.second.lastUsed < right.second.lastUsed;
            });
         parsedBytes -= victim->second.retainedBytes;
         parsedCache.erase(victim);
         ++stats.parserEvictions;
      }
      stats.parserBytes = parsedBytes;
   }

   void retainRoot(std::string key,RootCacheEntry entry)
   {
      auto old = rootCache.find(key);
      if (old != rootCache.end()) {
         rootBytes -= old->second.retainedBytes;
         rootCache.erase(old);
      }
      rootBytes += entry.retainedBytes;
      rootCache.emplace(std::move(key),std::move(entry));
      while (!rootCache.empty() && (rootCache.size() > rootOptions.maxEntries
         || rootBytes > rootOptions.maxRetainedBytes)) {
         auto victim = std::min_element(rootCache.begin(),rootCache.end(),
            [](const auto &left,const auto &right) {
               return left.second.lastUsed < right.second.lastUsed;
            });
         rootBytes -= victim->second.retainedBytes;
         rootCache.erase(victim);
         ++stats.rootAnalysisEvictions;
      }
   }
};

CompilerSession::CompilerSession() : CompilerSession(IncrementalCacheOptions{}) {}

CompilerSession::CompilerSession(IncrementalCacheOptions cacheOptions)
   : impl_(std::make_unique<Impl>())
{
   impl_->parsedOptions = cacheOptions.parsed;
   impl_->rootOptions = cacheOptions.root;
}
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
   switch (result.kind) {
      case SourceUpdateKind::Added: ++impl_->stats.sourceAdds; break;
      case SourceUpdateKind::Reintroduced: ++impl_->stats.sourceReintroductions; break;
      case SourceUpdateKind::Unchanged: ++impl_->stats.sourceNoOps; break;
      case SourceUpdateKind::VersionOnly: ++impl_->stats.versionOnlyUpdates; break;
      case SourceUpdateKind::ContentChanged: ++impl_->stats.contentChanges; break;
      case SourceUpdateKind::Rejected: ++impl_->stats.rejectedUpdates; break;
      case SourceUpdateKind::Removed: break;
   }
   if (result.accepted && (result.kind == SourceUpdateKind::ContentChanged
      || result.kind == SourceUpdateKind::Reintroduced)) {
      for (auto iterator = impl_->rootCache.begin(); iterator != impl_->rootCache.end();) {
         const auto &closure = iterator->second.closure;
         if (std::find(closure.begin(),closure.end(),result.current.uri) == closure.end()) {
            ++iterator;
            continue;
         }
         ++impl_->stats.invalidatedRootEntries;
         iterator = impl_->rootCache.erase(iterator);
      }
   }
   impl_->trace = {};
   impl_->trace.operation = "updateSource";
   impl_->trace.reasons.push_back(result.current.uri.empty() ? "rejected" : result.current.uri);
   return result;
}

bool CompilerSession::removeSource(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   const bool removed = impl_->sources.remove(uri);
   if (!removed) return false;
   ++impl_->stats.sourceRemoves;
   for (auto iterator = impl_->rootCache.begin(); iterator != impl_->rootCache.end();) {
      const auto &closure = iterator->second.closure;
      if (std::find(closure.begin(),closure.end(),uri) == closure.end()) {
         ++iterator;
         continue;
      }
      ++impl_->stats.invalidatedRootEntries;
      iterator = impl_->rootCache.erase(iterator);
   }
   impl_->trace = {};
   impl_->trace.operation = "removeSource";
   impl_->trace.reasons.push_back(uri);
   return true;
}

SourceManager &CompilerSession::sources() { return impl_->sources; }
const SourceManager &CompilerSession::sources() const { return impl_->sources; }

SyntaxSnapshot CompilerSession::parse(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   const auto *source = impl_->sources.get(uri);
   if (source == nullptr) return detail::SnapshotPipeline(impl_->sources).syntax(uri);
   const auto identity = identityFor(impl_->sources,uri);
   const std::string key = uri + "\n" + identity.contentHash + ":" +
      std::to_string(identity.byteLength);
   auto found = impl_->parsedCache.find(key);
   if (found != impl_->parsedCache.end() && found->second.text == source->text) {
      ++impl_->stats.parserHits;
      found->second.lastUsed = ++impl_->cacheClock;
      ++impl_->stats.syntaxMaterializations;
      return found->second.bundle.syntax;
   }
   auto bundle = detail::SnapshotPipeline(impl_->sources).build(uri,true);
   const SyntaxSnapshot syntax = bundle.syntax;
   CompilerSession::Impl::ParsedCacheEntry entry{uri,identity.contentHash,source->text,
      bundle,parsedRetainedBytes(source->text,bundle),++impl_->cacheClock};
   impl_->retainParsed(key,std::move(entry));
   ++impl_->stats.parserBuilds;
   ++impl_->stats.syntaxMaterializations;
   return syntax;
}

EditorSnapshot CompilerSession::editorSnapshot(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   const auto *source = impl_->sources.get(uri);
   if (source == nullptr) return detail::SnapshotPipeline(impl_->sources).editor(uri);
   const auto identity = identityFor(impl_->sources,uri);
   const std::string key = uri + "\n" + identity.contentHash + ":" +
      std::to_string(identity.byteLength);
   auto found = impl_->parsedCache.find(key);
   if (found != impl_->parsedCache.end() && found->second.text == source->text) {
      ++impl_->stats.parserHits;
      found->second.lastUsed = ++impl_->cacheClock;
      ++impl_->stats.editorMaterializations;
      return found->second.bundle.editor;
   }
   auto bundle = detail::SnapshotPipeline(impl_->sources).build(uri,true);
   const EditorSnapshot editor = bundle.editor;
   CompilerSession::Impl::ParsedCacheEntry entry{uri,identity.contentHash,source->text,
      bundle,parsedRetainedBytes(source->text,bundle),++impl_->cacheClock};
   impl_->retainParsed(key,std::move(entry));
   ++impl_->stats.parserBuilds;
   ++impl_->stats.editorMaterializations;
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
   const std::string baseKey = requestKey(request);
   for (auto &entry : impl_->rootCache) {
      auto &cached = entry.second;
      if (cached.baseKey != baseKey) continue;
      if (cached.hadMissingModels && cached.identities.size() != impl_->sources.uris().size()) continue;
      bool current = true;
      for (std::size_t index = 0; index < cached.closure.size(); ++index) {
         const auto *identity = impl_->sources.identity(cached.closure[index]);
         if (identity == nullptr || index >= cached.identities.size()
            || identity->contentRevision != cached.identities[index].contentRevision
            || identity->contentHash != cached.identities[index].contentHash
            || identity->byteLength != cached.identities[index].byteLength) {
            current = false;
            break;
         }
      }
      if (!current) continue;
      ++impl_->stats.rootAnalysisHits;
      ++impl_->stats.reusedClosureSources;
      cached.lastUsed = ++impl_->cacheClock;
      CompilationAnalysisResult result = cached.result;
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
      impl_->trace = {"compileAndAnalyze","ExactCacheHit",request.roots,cached.closure,{}, {},{}, {"root cache hit"}};
      ++compileInvocationCount_;
      ++impl_->stats.compilationInvocations;
      return result;
   }
   ++impl_->stats.rootAnalysisMisses;
   detail::CompilerContext context(impl_->sources,request.options,
      [this](const SourceBuffer &source) { return impl_->parsedArtifact(source); });
   ++compileInvocationCount_;
   ++impl_->stats.compilationInvocations;
   CompilationAnalysisResult result;
   std::vector<std::string> compilationSourceUris;
   result.compilation = compileRun(context,request,compilationSourceUris);
   for (const auto &uri : compilationSourceUris) {
      if (impl_->sources.get(uri) == nullptr) continue;
      const auto identity = identityFor(impl_->sources,uri);
      const std::string cacheKey = uri + "\n" + identity.contentHash + ":" +
         std::to_string(identity.byteLength);
      auto found = impl_->parsedCache.find(cacheKey);
      if (found != impl_->parsedCache.end() && found->second.text == impl_->sources.get(uri)->text) {
         ++impl_->stats.parserHits;
         found->second.lastUsed = ++impl_->cacheClock;
         ++impl_->stats.syntaxMaterializations;
         result.syntax.push_back(found->second.bundle.syntax);
      }
      else {
         auto bundle = detail::SnapshotPipeline(impl_->sources).build(uri,true);
         const SyntaxSnapshot syntax = bundle.syntax;
         const std::size_t retainedBytes = parsedRetainedBytes(
            impl_->sources.get(uri)->text,bundle);
         Impl::ParsedCacheEntry entry{uri,identity.contentHash,impl_->sources.get(uri)->text,
            std::move(bundle),retainedBytes,++impl_->cacheClock};
         impl_->retainParsed(cacheKey,std::move(entry));
         ++impl_->stats.parserBuilds;
         ++impl_->stats.syntaxMaterializations;
         result.syntax.push_back(syntax);
      }
   }
   result.semantic = buildSemanticSnapshot(impl_->sources,request,
      result.compilation,compilationSourceUris,&result.syntax,
      &context.models());
   const std::string key = rootKey(baseKey,compilationSourceUris,impl_->sources);
   Impl::RootCacheEntry cached;
   cached.baseKey = baseKey;
   cached.key = key;
   cached.closure = compilationSourceUris;
   for (const auto &uri : compilationSourceUris)
      cached.identities.push_back(identityFor(impl_->sources,uri));
   cached.hadMissingModels = !result.compilation.missingModels.empty();
   cached.result = result;
   cached.retainedBytes = sizeof(cached) + result.syntax.size() * sizeof(SyntaxSnapshot);
   cached.lastUsed = ++impl_->cacheClock;
   impl_->retainRoot(key,std::move(cached));
   ++impl_->stats.rootAnalysisBuilds;
   impl_->stats.reparsedClosureSources += result.compilation.success ? 1 : 0;
   impl_->trace = {"compileAndAnalyze","RebuildWithParseReuse",request.roots,compilationSourceUris,{},compilationSourceUris,{}, {"root cache miss"}};
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
      result.diagnostics = logger.getDiagnostics();
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
   ++impl_->stats.compilationInvocations;
   detail::CompilerContext context(impl_->sources,request.options,
      [this](const SourceBuffer &source) { return impl_->parsedArtifact(source); });
   std::vector<std::string> compilationSourceUris;
   return compileRun(context,request,compilationSourceUris);
}

IncrementalStats CompilerSession::incrementalStats() const
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   IncrementalStats result = impl_->stats;
   result.parserBytes = impl_->parsedBytes;
   return result;
}

IncrementalTrace CompilerSession::lastIncrementalTrace() const
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return impl_->trace;
}

void CompilerSession::clearIncrementalCaches()
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   impl_->parsedCache.clear();
   impl_->rootCache.clear();
   impl_->parsedBytes = 0;
   impl_->rootBytes = 0;
   impl_->stats.parserBytes = 0;
   impl_->trace = {};
}

const char *version() { return "0.9.9"; }

} // namespace ilic
