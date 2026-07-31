#include "../../include/ilic/Compiler.h"
#include "../../include/ilic/Semantic.h"

#include "CompilerContext.h"
#include "../input/ili1/Ili1Input.h"
#include "../input/ili2/Ili2Input.h"
#include "../input/ili2/InterlisModel.h"
#include "../metamodel/SemanticChecker.h"
#include "../metamodel/TranslationChecker.h"
#include "../util/Logger.h"
#include "../util/StringUtil.h"

#include <chrono>
#include <ctime>
#include <list>
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
   if (file->getIliVersion() == "1.0")
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
};

CompilerSession::CompilerSession() : impl_(std::make_unique<Impl>()) {}
CompilerSession::~CompilerSession() = default;

void CompilerSession::putSource(std::string uri,std::string utf8,std::uint64_t version)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   impl_->sources.put(std::move(uri),std::move(utf8),version);
}

bool CompilerSession::removeSource(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return impl_->sources.remove(uri);
}

SourceManager &CompilerSession::sources() { return impl_->sources; }
const SourceManager &CompilerSession::sources() const { return impl_->sources; }

SyntaxSnapshot CompilerSession::parse(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return parseSyntax(impl_->sources,uri);
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
   detail::CompilerContext context(impl_->sources,request.options);
   ++compileInvocationCount_;
   CompilationAnalysisResult result;
   std::vector<std::string> compilationSourceUris;
   result.compilation = compileRun(context,request,compilationSourceUris);
   result.semantic = buildSemanticSnapshot(impl_->sources,request,
      result.compilation,compilationSourceUris,&result.syntax,
      &context.models());
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
   detail::CompilerContext context(impl_->sources,request.options);
   std::vector<std::string> compilationSourceUris;
   return compileRun(context,request,compilationSourceUris);
}

const char *version() { return "0.9.9"; }

} // namespace ilic
