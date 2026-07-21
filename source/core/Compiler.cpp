#include "../../include/ilic/Compiler.h"
#include "../../include/ilic/Semantic.h"

#include "../input/ili1/Ili1Input.h"
#include "../input/ili2/Ili2Input.h"
#include "../metamodel/MetaModel.h"
#include "../metamodel/MetaModelInput.h"
#include "../metamodel/SemanticChecker.h"
#include "../metamodel/TranslationChecker.h"
#include "../util/IliFile.h"
#include "../util/Logger.h"

#include <list>
#include <map>
#include <mutex>
#include <set>
#include <sstream>

namespace ilic {
namespace {

std::mutex compilerMutex;

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

void applyExternalMetaAttributes(const CompilationRequest &request)
{
   for (const auto &external : request.externalMetaAttributes) {
      metamodel::MetaElement *target = nullptr;
      for (auto *model : metamodel::get_all_models()) {
         target = findExternalMetaTarget(model,external.element,"");
         if (target != nullptr) break;
      }
      if (target == nullptr) {
         Log.error("external meta attribute target " + external.element + " not found",-1,0,
            "ILIC-META-TARGET");
         continue;
      }
      if (external.name == "ili2c.translationOf") {
         if (auto *model = dynamic_cast<metamodel::Model *>(target)) {
            model->_translationOfName = external.value;
         }
         else {
            Log.error("ili2c.translationOf may only target a model",-1,0,"ILIC-META-TARGET");
         }
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
   std::size_t &diagnosticIndex,std::size_t &logIndex)
{
   const auto &diagnostics = Log.getDiagnostics();
   while (diagnosticIndex < diagnostics.size())
      transcript.push_back(diagnosticTranscriptLine(diagnostics[diagnosticIndex++]));

   const auto &events = Log.getLogEvents();
   while (logIndex < events.size()) {
      const auto &event = events[logIndex++];
      const std::string prefix = event.level == LogLevel::Error ? "err:" :
         (event.level == LogLevel::Warning ? "wrn:" :
         (event.level == LogLevel::Debug ? "dbg:" : "inf:"));
      transcript.push_back(prefix + "    " + event.message);
   }
}

std::string completionTranscriptLine(int errorCount,int warningCount)
{
   std::string line = "inf: ilic completed with";
   if (errorCount == 0) line += " no errors";
   else if (errorCount == 1) line += " 1 error";
   else line += " " + std::to_string(errorCount) + " errors";
   if (warningCount == 0) line += ", no warnings";
   else if (warningCount == 1) line += ", 1 warning";
   else line += ", " + std::to_string(warningCount) + " warnings";
   return line + ".";
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

bool compileFile(
   util::IliFile *file,
   std::list<util::IliFile *> &compiledFiles,
   std::set<std::string> &compiledModels,
   std::vector<std::string> &transcript,
   std::size_t &diagnosticIndex,
   std::size_t &logIndex)
{
   if (file == nullptr) return false;
   if (file->getIliVersion() != "1.0" && file->getIliVersion() != "2.3"
       && file->getIliVersion() != "2.4") {
      Log.error(file->getFilePath() + ": unsupported iliversion " + file->getIliVersion());
      appendNewEvents(transcript,diagnosticIndex,logIndex);
      return true;
   }
   for (auto *compiled : compiledFiles) if (compiled == file) return true;

   for (const auto &required : file->getRequiredModels()) {
      bool found = compiledModels.find(required) != compiledModels.end();
      for (const auto &local : file->getModels()) found = found || local == required;
      if (!found) return false;
   }

   transcript.push_back("inf: compiling " + file->getFilePath() + " ...");
   const int errorsBefore = Log.getErrorCount();
   if (file->getIliVersion() == "1.0") input::parseIli1(file->getFilePath());
   else input::parseIli2(file->getFilePath());
   appendNewEvents(transcript,diagnosticIndex,logIndex);
   compiledFiles.push_back(file);
   if (file->getFilePath() == "INTERLIS") compiledModels.insert("INTERLIS");
   for (const auto &model : file->getModels()) compiledModels.insert(model);
   transcript.push_back("inf: " + file->getFilePath() +
      (Log.getErrorCount() > errorsBefore ? " compiled with errors." : " compiled."));
   return true;
}

} // namespace

CompilerSession::CompilerSession() = default;
CompilerSession::~CompilerSession() = default;

void CompilerSession::putSource(std::string uri, std::string utf8, std::uint64_t version)
{
   sources_.put(std::move(uri), std::move(utf8), version);
}

bool CompilerSession::removeSource(const std::string &uri) { return sources_.remove(uri); }
SourceManager &CompilerSession::sources() { return sources_; }
const SourceManager &CompilerSession::sources() const { return sources_; }

SyntaxSnapshot CompilerSession::parse(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(compilerMutex);
   return parseSyntax(sources_,uri);
}

CompilationResult CompilerSession::compile(const CompilationRequest &request)
{
   std::lock_guard<std::mutex> lock(compilerMutex);
   return compileUnlocked(request);
}

SemanticSnapshot CompilerSession::analyze(const CompilationRequest &request)
{
   std::lock_guard<std::mutex> lock(compilerMutex);
   return compileAndAnalyzeUnlocked(request).semantic;
}

CompilationAnalysisResult CompilerSession::compileAndAnalyze(const CompilationRequest &request)
{
   std::lock_guard<std::mutex> lock(compilerMutex);
   return compileAndAnalyzeUnlocked(request);
}

CompilationAnalysisResult CompilerSession::compileAndAnalyzeUnlocked(
   const CompilationRequest &request)
{
   CompilationAnalysisResult result;
   result.compilation = compileUnlocked(request);
   result.semantic = buildSemanticSnapshot(sources_,request,result.compilation,
      lastCompilationSourceUris_,&result.syntax);
   return result;
}

CompilationResult CompilerSession::compileUnlocked(const CompilationRequest &request)
{
   ++compileInvocationCount_;
   lastCompilationSourceUris_.clear();
   ActiveSourceManagerScope sourceScope(&sources_);
   CompilationResult result;
   std::vector<std::string> transcript{
      "inf: ilic " + std::string(version()),
      "inf:",
      "inf: loading ili files from command line ...",
   };
   std::size_t transcriptedDiagnostics = 0;
   std::size_t transcriptedLogs = 0;

   auto finish = [&]() {
      appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs);
      result.errorCount = Log.getErrorCount();
      result.warningCount = Log.getWarningCount();
      result.diagnostics = Log.getDiagnostics();
      result.logs = Log.getLogEvents();
      result.success = result.errorCount == 0;
      transcript.push_back("inf:");
      transcript.push_back(completionTranscriptLine(result.errorCount,result.warningCount));
      result.transcript = std::move(transcript);
      return result;
   };

   try {
   Log.reset();
   Log.setSilent(true);
   Log.setAbortWithException(true);
   Log.setCategory("imports");
   if (request.options.warningsAsErrors) Log.warningsAsErrors();
   util::reset_ilifiles();
   metamodel::reset();
   metamodel::reset_input_state();
   util::set_autosearch(request.options.autoSearch);
   util::set_ilidirs(joinDirectories(request.options.modelDirectories));

   for (const auto &root : request.roots) {
      transcript.push_back("inf:    loading " + root + " ...");
      if (util::load_ilifiles_by_file(root) == nullptr) {
         Log.error("unable to load root source " + root);
         appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs);
         transcript.push_back("inf:    not done.");
      }
      else {
         transcript.push_back("inf:    done.");
      }
   }
   transcript.push_back("inf: done.");
   if (util::all_ilifiles.empty()) {
      return finish();
   }

   const std::string iliVersion = util::all_ilifiles.back()->getIliVersion();
   metamodel::init(iliVersion);
   transcript.push_back("inf:");
   transcript.push_back("inf: loading imported models ...");
   std::map<std::string, bool> loaded;
   std::set<std::string> reportedMissing;
   bool progress = true;
   while (progress) {
      progress = false;
      const std::size_t before = util::all_ilifiles.size();
      for (auto *file : util::all_ilifiles) {
         for (const auto &model : file->getRequiredModels()) {
            if (model == "INTERLIS" || loaded[model]) continue;
            util::IliFile *resolved = util::load_ilifiles_by_model(model, iliVersion);
            if (resolved == nullptr) {
               if (reportedMissing.insert(model).second) {
                  result.missingModels.push_back(model);
                  Log.error("model " + model + " not found.");
                  appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs);
                  transcript.push_back("inf:    model " + model + " not found.");
               }
            }
            else {
               loaded[model] = true;
               transcript.push_back("inf:    found in " + resolved->getFilePath() + ".");
            }
         }
      }
      progress = util::all_ilifiles.size() > before;
   }
   transcript.push_back("inf: done.");

   for (auto *file : util::all_ilifiles)
      if (file != nullptr) lastCompilationSourceUris_.push_back(file->getFilePath());

   transcript.push_back("inf:");
   transcript.push_back("inf: all input files are:");
   for (auto *file : util::all_ilifiles) appendInputFileTranscript(transcript,file);
   transcript.push_back("inf: done.");

   std::list<util::IliFile *> compiledFiles;
   std::set<std::string> compiledModels;
   compileFile(util::load_ilifiles_by_model("INTERLIS", iliVersion), compiledFiles,
      compiledModels,transcript,transcriptedDiagnostics,transcriptedLogs);
   for (std::size_t pass = 0; pass <= util::all_ilifiles.size(); ++pass) {
      bool allCompiled = true;
      for (auto *file : util::all_ilifiles) {
         allCompiled = compileFile(file,compiledFiles,compiledModels,transcript,
            transcriptedDiagnostics,transcriptedLogs) && allCompiled;
      }
      if (allCompiled) break;
      if (pass == util::all_ilifiles.size()) {
         Log.error("unable to order model dependencies");
         appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs);
      }
   }

   applyExternalMetaAttributes(request);
   metamodel::check_model_semantics();
   metamodel::check_model_translations();
   appendNewEvents(transcript,transcriptedDiagnostics,transcriptedLogs);
   for (auto *model : metamodel::get_all_models()) {
      CompiledModel compiled{model->Name, model->iliVersion, model->_ilifile, {}};
      for (auto *attribute : model->MetaAttribute) {
         if (attribute != nullptr) compiled.metaAttributes.push_back({attribute->Name,attribute->Value});
      }
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
      Log.error(std::string("internal compiler failure: ") + error.what());
      return finish();
   }
}

const char *version() { return "0.9.9"; }

} // namespace ilic
