#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "antlr4-runtime.h"
#include "../core/CompilationSourceStore.h"
#include "../input/parser/generated/IliFileParserBaseVisitor.h"
#include "Logger.h"

using namespace std;

namespace ilic {
struct CompilerOptions;
}

namespace util {

class IliFileHeaderVisitor;

// Header metadata only. The parser visitor is deliberately separate so an
// IliFile is a stable, catalog-owned data record rather than parser state.
class IliFile final {
public:
   const string &getFilePath() const noexcept { return filepath_; }
   const string &getIliVersion() const noexcept { return iliversion_; }
   const vector<string> &getImports() const noexcept { return imports_; }
   const vector<string> &getTranslationModels() const noexcept { return translation_of_; }
   const vector<string> &getRequiredModels() const noexcept { return required_models_; }
   const vector<string> &getModels() const noexcept { return models_; }
   void setAutoSearch(bool value) noexcept { auto_search_ = value; }
   bool getAutoSearch() const noexcept { return auto_search_; }
   const ilic::SourceBuffer &source() const noexcept { return *source_; }

   // Const-correct names used by the context-oriented API. The get* methods
   // remain as source-compatible aliases for the legacy compiler frontends.
   const string &uri() const noexcept { return filepath_; }
   const string &iliVersion() const noexcept { return iliversion_; }
   const vector<string> &imports() const noexcept { return imports_; }
   const vector<string> &translationModels() const noexcept { return translation_of_; }
   const vector<string> &requiredModels() const noexcept { return required_models_; }
   const vector<string> &models() const noexcept { return models_; }
   bool autoSearched() const noexcept { return auto_search_; }

private:
   friend class IliFileCatalog;
   friend class IliFileHeaderVisitor;

   explicit IliFile(string filepath,Logger &logger)
      : filepath_(std::move(filepath)),logger_(logger) {}

   string filepath_;
   string iliversion_ = "2.3";
   vector<string> translation_of_;
   vector<string> imports_;
   vector<string> required_models_;
   vector<string> models_;
   bool auto_search_ = false;
   const ilic::SourceBuffer *source_ = nullptr;
   Logger &logger_;
};

class IliFileHeaderVisitor final : public parser::IliFileParserBaseVisitor {
public:
   IliFileHeaderVisitor(Logger &logger,IliFile &target) : logger_(logger),target_(target) {}

   antlrcpp::Any visitIliFile(parser::IliFileParser::IliFileContext *context) override;
   antlrcpp::Any visitVersion(parser::IliFileParser::VersionContext *context) override;
   antlrcpp::Any visitModelName(parser::IliFileParser::ModelNameContext *context) override;
   antlrcpp::Any visitTranslationOf(parser::IliFileParser::TranslationOfContext *context) override;
   antlrcpp::Any visitModelImport(parser::IliFileParser::ModelImportContext *context) override;

private:
   Logger &logger_;
   IliFile &target_;
};

class IliFileCatalog final {
public:
   IliFileCatalog(ilic::detail::CompilationSourceStore &sources,Logger &logger);
   IliFileCatalog(ilic::detail::CompilationSourceStore &sources,Logger &logger,
      const ilic::CompilerOptions &options);
   IliFileCatalog(const IliFileCatalog &) = delete;
   IliFileCatalog &operator=(const IliFileCatalog &) = delete;

   void setAutoSearch(bool enabled) noexcept { auto_search_ = enabled; }
   void setModelDirectories(vector<string> directories);
   void clear();
   IliFile *loadByFile(const string &filepath);
   IliFile *loadByRepositorySource(const string &uri,const string &source);
   IliFile *loadByModel(const string &modelname,const string &iliVersion);
   IliFile *loadRoot(const string &uri) { return loadByFile(uri); }
   IliFile *resolveModel(const string &name,const string &version) {
      return loadByModel(name,version);
   }
   IliFile *findByUri(const string &uri) const noexcept;
   IliFile *findByModel(const string &name,const string &version) const noexcept;
   void select(IliFile &file) { add(&file); }
   size_t fileCount() const noexcept { return ownedFiles_.size(); }
   const vector<IliFile *> &files() const noexcept { return files_; }
   const vector<IliFile *> &selectedFiles() const noexcept { return files_; }
   const vector<unique_ptr<IliFile>> &allFiles() const noexcept { return ownedFiles_; }

private:
   IliFile *load(const string &filepath,bool autoSearched = false);
   void add(IliFile *file);

   ilic::detail::CompilationSourceStore &sources_;
   Logger &logger_;
   const ilic::CompilerOptions *options_ = nullptr;
   vector<unique_ptr<IliFile>> ownedFiles_;
   vector<IliFile *> files_;
   map<string,IliFile *> models_;
   vector<string> modelDirectories_;
   bool auto_search_ = true;
};

} // namespace util
