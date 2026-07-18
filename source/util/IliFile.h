#pragma once

#include <string>
#include <list>

#include "antlr4-runtime.h"

#include "../input/parser/generated/IliFileParserBaseVisitor.h"

using namespace std;

namespace util {

   class IliFile : public parser::IliFileParserBaseVisitor {
   public:

      IliFile(string filepath);

      antlrcpp::Any visitIliFile(parser::IliFileParser::IliFileContext *context) override;
      antlrcpp::Any visitVersion(parser::IliFileParser::VersionContext *context) override;
      antlrcpp::Any visitModelName(parser::IliFileParser::ModelNameContext *context) override;
      antlrcpp::Any visitTranslationOf(parser::IliFileParser::TranslationOfContext *context) override;
      antlrcpp::Any visitModelImport(parser::IliFileParser::ModelImportContext *context) override;

      string getFilePath();
      string getIliVersion();
      list<string> getImports();
      list<string> getTranslationModels();
      list<string> getRequiredModels();
      list<string> getModels();
      void setAutoSearch(bool autosearch);
      bool getAutoSearch();

   private:

      string filepath = "";
      string iliversion = "2.3";
      list<string> translation_of;
      list<string> imports;
      list<string> models;      
      bool auto_search = false;

   };

   extern list<IliFile *> all_ilifiles;
   void reset_ilifiles();
   void set_autosearch(bool auto_search);
   void set_ilidirs(string ilidirs);
   IliFile* load_ilifiles_by_file(string filepath);
   IliFile* load_ilifiles_by_model(string modelname,string iliversion);

};
