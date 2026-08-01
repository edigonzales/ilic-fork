#include <filesystem>
#include <memory>

#include "../../include/ilic/Compiler.h"
#include "../input/ili2/InterlisModel.h"
#include "../input/parser/generated/IliFileLexer.cpp"
#include "../input/parser/generated/IliFileParser.cpp"
#include "../input/parser/IliParserErrorListener.h"
#include "StringUtil.h"
#include "IliFile.h"

using namespace parser;

namespace util {

antlrcpp::Any IliFileHeaderVisitor::visitIliFile(
   IliFileParser::IliFileContext *context)
{
   logger_.debug(">>> visitIliFile(" + target_.getFilePath() + ")");
   auto indent = logger_.indentScope();
   antlrcpp::Any result = visitChildren(context);
   logger_.debug("<<< visitIliFile(" + target_.getFilePath() + ")");
   return result;
}

antlrcpp::Any IliFileHeaderVisitor::visitVersion(
   IliFileParser::VersionContext *context)
{
   if (context->majorversion != nullptr)
      target_.iliversion_ = context->majorversion->getText() + "." +
         context->minorversion->getText();
   else
      target_.iliversion_ = "1.0";
   return visitChildren(context);
}

antlrcpp::Any IliFileHeaderVisitor::visitModelName(
   IliFileParser::ModelNameContext *context)
{
   target_.models_.push_back(context->modelname->getText());
   return visitChildren(context);
}

antlrcpp::Any IliFileHeaderVisitor::visitTranslationOf(
   IliFileParser::TranslationOfContext *context)
{
   const string model = context->modelname->getText();
   target_.translation_of_.push_back(model);
   target_.required_models_.push_back(model);
   return visitChildren(context);
}

antlrcpp::Any IliFileHeaderVisitor::visitModelImport(
   IliFileParser::ModelImportContext *context)
{
   const string model = context->modelname->getText();
   target_.imports_.push_back(model);
   target_.required_models_.push_back(model);
   return visitChildren(context);
}

IliFileCatalog::IliFileCatalog(ilic::detail::CompilationSourceStore &sources,
   Logger &logger)
   : sources_(sources),logger_(logger)
{
}

IliFileCatalog::IliFileCatalog(ilic::detail::CompilationSourceStore &sources,
   Logger &logger,const ilic::CompilerOptions &options,ParsedSourceHeaderProvider headerProvider)
   : sources_(sources),logger_(logger),options_(&options),
     headerProvider_(std::move(headerProvider))
{
   setAutoSearch(options.autoSearch);
   setModelDirectories(options.modelDirectories);
}

void IliFileCatalog::setModelDirectories(vector<string> directories)
{
   modelDirectories_.clear();
   for (const string &directory : directories) {
      if (!directory.empty() && !std::filesystem::is_directory(directory)) {
         logger_.error("ilidirs: " + directory + " is not a directory");
         continue;
      }
      if (!directory.empty()) modelDirectories_.push_back(directory);
   }
}

void IliFileCatalog::clear()
{
   files_.clear();
   models_.clear();
   ownedFiles_.clear();
}

IliFile *IliFileCatalog::findByUri(const string &uri) const noexcept
{
   for (const auto &file : ownedFiles_)
      if (compare_case_insensitive(file->getFilePath(),uri)) return file.get();
   return nullptr;
}

IliFile *IliFileCatalog::findByModel(const string &name,const string &version) const noexcept
{
   const auto found = models_.find(name);
   if (found == models_.end() || found->second == nullptr ||
       found->second->getIliVersion() != version) return nullptr;
   return found->second;
}

IliFile *IliFileCatalog::load(const string &filepath,bool autoSearched)
{
   if (IliFile *known = findByUri(filepath)) return known;

   const ilic::SourceBuffer *registered = sources_.get(filepath);
   string source;
   if (registered != nullptr) {
      source = registered->text;
   } else {
      source = load_string(filepath);
      if (source.empty() && !std::filesystem::exists(filepath)) {
         logger_.error("unable to open " + filepath);
         return nullptr;
      }
      registered = &sources_.rememberFileSystemSource(filepath,source);
   }

   auto sourceScope = logger_.sourceScope(filepath);
   auto categoryScope = logger_.categoryScope("parser");
   std::unique_ptr<IliFile> file(new IliFile(filepath,logger_));
   file->source_ = registered;
   file->auto_search_ = autoSearched;
   if (headerProvider_) {
      file->setParsedHeader(headerProvider_(*registered));
   }
   else {
      antlr4::ANTLRInputStream input(registered->text);
      lexer::IliFileLexer lexer(&input);
      antlr4::CommonTokenStream tokens(&lexer);
      parser::IliFileParser parser(&tokens);
      parser::IliFileParser::IliFileContext *context = parser.iliFile();
      if (context == nullptr) {
         logger_.error("unable to parse ili file structure " + filepath);
         return nullptr;
      }
      IliFileHeaderVisitor visitor(logger_,*file);
      visitor.visitIliFile(context);
   }
   IliFile *result = file.get();
   ownedFiles_.push_back(std::move(file));
   for (const string &model : result->getModels()) models_[model] = result;
   return result;
}

void IliFileCatalog::add(IliFile *file)
{
   if (file == nullptr) return;
   for (IliFile *existing : files_)
      if (compare_case_insensitive(existing->getFilePath(),file->getFilePath())) return;
   files_.push_back(file);
}

IliFile *IliFileCatalog::loadByFile(const string &filepath)
{
   IliFile *file = load(filepath,false);
   add(file);
   return file;
}

IliFile *IliFileCatalog::loadByRepositorySource(const string &uri,const string &source)
{
   sources_.rememberRepositorySource(uri,source);
   return loadByFile(uri);
}

IliFile *IliFileCatalog::loadByModel(const string &modelname,const string &iliVersion)
{
   if (IliFile *known = findByModel(modelname,iliVersion)) {
      add(known);
      return known;
   }

   if (modelname == "INTERLIS") {
      if (IliFile *known = findByUri("INTERLIS")) {
         add(known);
         return known;
      }
      const ilic::SourceBuffer &builtin = sources_.rememberBuiltinSource(
         "INTERLIS",input::getInterlisModel23());
      std::unique_ptr<IliFile> file(new IliFile("INTERLIS",logger_));
      file->source_ = &builtin;
      IliFile *result = file.get();
      ownedFiles_.push_back(std::move(file));
      files_.insert(files_.begin(),result);
      return result;
   }

   for (const string &uri : sources_.uris()) {
      IliFile *file = load(uri,true);
      if (file == nullptr || file->getIliVersion() != iliVersion) continue;
      for (const string &model : file->getModels()) {
         if (model == modelname) {
            file->setAutoSearch(true);
            add(file);
            return file;
         }
      }
   }

   if (!auto_search_) return nullptr;

   for (const string &directory : modelDirectories_) {
      const string filepath = (std::filesystem::path(directory) /
         (modelname + ".ili")).string();
      if (!std::filesystem::exists(filepath)) continue;
      IliFile *file = load(filepath,true);
      if (file != nullptr && file->getIliVersion() == iliVersion) {
         add(file);
         return file;
      }
   }

   return nullptr;
}

} // namespace util
