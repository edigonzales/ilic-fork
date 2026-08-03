#include "CompilationRun.h"

#include "CompilationTranscript.h"
#include "CompilerContext.h"
#include "ExternalMetaAttributeApplier.h"
#include "../../include/ilic/DiagnosticPipeline.h"
#include "../input/ili1/Ili1Input.h"
#include "../input/ili2/Ili2Input.h"
#include "../input/ili2/InterlisModel.h"
#include "../metamodel/SemanticChecker.h"
#include "../metamodel/TranslationChecker.h"
#include "../util/IliFile.h"
#include "../util/Logger.h"
#include "../util/StringUtil.h"

#include <map>
#include <set>

namespace ilic::detail {
namespace {

SourceBuffer sourceFor(CompilerContext &context,util::IliFile *file)
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

bool compileFile(CompilerContext &context,util::IliFile *file,
   std::vector<util::IliFile *> &compiledFiles,std::set<std::string> &compiledModels,
   std::vector<std::string> &transcript,std::size_t &diagnosticIndex,std::size_t &logIndex)
{
   if (file == nullptr) return false;
   util::Logger &logger = context.logger();
   if (file->getIliVersion() != "1.0" && file->getIliVersion() != "2.3" &&
      file->getIliVersion() != "2.4") {
      logger.setCurrentSource(file->getFilePath());
      logger.error(util::DiagnosticId::InputUnsupportedVersion,
         "unsupported INTERLIS version " + file->getIliVersion() +
         "; supported versions are 1.0, 2.3, and 2.4",1);
      CompilationTranscript::appendNewEvents(transcript,diagnosticIndex,logIndex,logger);
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
   const SourceBuffer source = sourceFor(context,file);
   if (auto artifact = context.parsedSourceArtifact(source)) {
      artifact->reportParserDiagnostics(logger);
      if (artifact->parserDiagnostics().empty() && artifact->supportsMetaModelBuild())
         artifact->buildMetaModel(context.builder(),logger);
      else if (artifact->parserDiagnostics().empty())
         logger.error(util::DiagnosticId::CompilerInternal,
            "tolerant parser artifact cannot be used for compilation",-1);
   }
   else if (file->getIliVersion() == "1.0") input::parseIli1(source,context.builder(),logger);
   else input::parseIli2(source,context.builder(),logger);
   CompilationTranscript::appendNewEvents(transcript,diagnosticIndex,logIndex,logger);
   compiledFiles.push_back(file);
   if (file->getFilePath() == "INTERLIS") compiledModels.insert("INTERLIS");
   for (const auto &model : file->getModels()) compiledModels.insert(model);
   transcript.push_back("inf: " + file->getFilePath() +
      (logger.getErrorCount() > errorsBefore ? " compiled with errors." : " compiled."));
   return true;
}

} // namespace

CompilationResult CompilationRun::run(const CompilationRequest &request,
   std::vector<std::string> &sourceUris)
{
   sourceUris.clear();
   util::Logger &logger = context_.logger();
   logger.setSilent(true);
   logger.setCategory("imports");
   CompilationResult result;
   std::vector<std::string> transcript{
      "inf: ilic " + std::string(version()),"inf:",
      "inf: loading ili files from command line ..."};
   std::size_t diagnosticIndex = 0;
   std::size_t logIndex = 0;
   auto finish = [&]() {
      result.errorCount = logger.getErrorCount();
      result.warningCount = logger.getWarningCount();
      result.success = result.errorCount == 0 && !result.cancelled;
      CompilationTranscript::appendNewEvents(transcript,diagnosticIndex,logIndex,logger);
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
      transcript.push_back(CompilationTranscript::completionLine(result.errorCount,
         result.warningCount));
      result.transcript = std::move(transcript);
      return result;
   };

   try {
      for (const auto &root : request.roots) {
         transcript.push_back("inf:    loading " + root + " ...");
         if (context_.files().loadByFile(root) == nullptr) {
            logger.error(util::DiagnosticId::InputLoad,
               "unable to load root source " + root,-1);
            CompilationTranscript::appendNewEvents(transcript,diagnosticIndex,logIndex,logger);
            transcript.push_back("inf:    not done.");
         }
         else transcript.push_back("inf:    done.");
      }
      transcript.push_back("inf: done.");
      if (context_.files().files().empty()) return finish();

      const std::string iliVersion = context_.files().files().back()->getIliVersion();
      transcript.push_back("inf:");
      transcript.push_back("inf: loading imported models ...");
      std::map<std::string,bool> loaded;
      std::set<std::string> reportedMissing;
      bool progress = true;
      while (progress) {
         progress = false;
         const std::size_t before = context_.files().files().size();
         for (std::size_t index = 0; index < context_.files().files().size(); ++index) {
            auto *file = context_.files().files()[index];
            for (const auto &model : file->getRequiredModels()) {
               if (model == "INTERLIS" || loaded[model]) continue;
               util::IliFile *resolved = context_.files().loadByModel(model,iliVersion);
               if (resolved == nullptr) {
                  if (reportedMissing.insert(model).second) {
                     result.missingModels.push_back(model);
                     logger.error(util::DiagnosticId::NameModelNotFound,
                        "model " + model + " not found.",-1);
                     CompilationTranscript::appendNewEvents(transcript,diagnosticIndex,logIndex,logger);
                     transcript.push_back("inf:    model " + model + " not found.");
                  }
               }
               else {
                  loaded[model] = true;
                  transcript.push_back("inf:    found in " + resolved->getFilePath() + ".");
               }
            }
         }
         progress = context_.files().files().size() > before;
      }
      transcript.push_back("inf: done.");
      for (auto *file : context_.files().files())
         if (file != nullptr) sourceUris.push_back(file->getFilePath());
      transcript.push_back("inf:");
      transcript.push_back("inf: all input files are:");
      for (auto *file : context_.files().files()) CompilationTranscript::appendInputFile(transcript,file);
      transcript.push_back("inf: done.");

      std::vector<util::IliFile *> compiledFiles;
      std::set<std::string> compiledModels;
      compileFile(context_,context_.files().loadByModel("INTERLIS",iliVersion),compiledFiles,
         compiledModels,transcript,diagnosticIndex,logIndex);
      for (std::size_t pass = 0; pass <= context_.files().files().size(); ++pass) {
         bool allCompiled = true;
         for (auto *file : context_.files().files())
            allCompiled = compileFile(context_,file,compiledFiles,compiledModels,transcript,
               diagnosticIndex,logIndex) && allCompiled;
         if (allCompiled) break;
         if (pass == context_.files().files().size()) {
            logger.error(util::DiagnosticId::ModelDependency,
               "unable to order model dependencies",-1);
            CompilationTranscript::appendNewEvents(transcript,diagnosticIndex,logIndex,logger);
         }
      }
      ExternalMetaAttributeApplier::apply(request,context_.models(),logger);
      metamodel::check_model_semantics(context_.models(),logger);
      metamodel::check_model_translations(context_.models(),logger);
      CompilationTranscript::appendNewEvents(transcript,diagnosticIndex,logIndex,logger);
      for (auto *model : context_.models().models()) {
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

} // namespace ilic::detail
