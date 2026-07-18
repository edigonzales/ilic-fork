#include "stdio.h"
#include <iostream>
#include <filesystem>

// antlr4
#include "Exceptions.h"

#include "../input/parser/generated/IliFileLexer.cpp"
#include "../input/parser/generated/IliFileParser.cpp"

#include "StringUtil.h"
#include "IliFile.h"
#include "Logger.h"
#include "../../include/ilic/SourceManager.h"

using namespace std;
using namespace parser;

namespace util {

   // global variables

   list <IliFile*> all_ilifiles;
   static list <IliFile*> all_ilifiles_full;
   static map <string, IliFile*> all_ilimodels;
   static bool auto_search = true;
   static list<string> ilidirs;

   void reset_ilifiles()
   {
      all_ilifiles.clear();
      all_ilifiles_full.clear();
      all_ilimodels.clear();
      ilidirs.clear();
      auto_search = true;
   }

   // constructor

   IliFile::IliFile(string ilifile)
   {
      this->filepath = ilifile;
   }

   // visitor interface

   antlrcpp::Any IliFile::visitIliFile(IliFileParser::IliFileContext *context)
   {
      // root method
      Log.debug(">>> visitIliFile(" + filepath + ")");
      Log.incNestLevel();
      antlrcpp::Any result = visitChildren(context);
      Log.decNestLevel();
      Log.debug("<<< visitILiFile(" + filepath + ")");
      return result;
   }

   antlrcpp::Any IliFile::visitVersion(IliFileParser::VersionContext *context)
   {
      Log.debug(">>> visitVersion()");
      if (context->majorversion != nullptr) {
         this->iliversion = context->majorversion->getText() + "." + context->minorversion->getText();
      }
      else {
         this->iliversion = "1.0";
      }
      antlrcpp::Any result = visitChildren(context);
      Log.debug("<<< visitVersion(" + this->iliversion + ")");
      return result;
   }

   antlrcpp::Any IliFile::visitModelName(IliFileParser::ModelNameContext *context)
   {
      Log.debug(">>> visitModelName()");
      string name = context->modelname->getText();
      this->models.push_back(name);
      all_ilimodels[name] = this;
      antlrcpp::Any result = visitChildren(context);
      Log.debug("<<< visitModelName(" + name + ")");
      return result;
   }

   antlrcpp::Any IliFile::visitTranslationOf(IliFileParser::TranslationOfContext *context)
   {
      Log.debug(">>> vistTranslationOf()");
      string modelname = context->modelname->getText();
      translation_of.push_back(modelname);
      antlrcpp::Any result = visitChildren(context);
      Log.debug("<<< vistTranslationOf(" + modelname + ")");
      return result;
   }

   antlrcpp::Any IliFile::visitModelImport(IliFileParser::ModelImportContext *context)
   {
      Log.debug(">>> vistModelImport()");
      string modelname = context->modelname->getText();
      this->imports.push_back(modelname);
      antlrcpp::Any result = visitChildren(context);
      Log.debug("<<< vistModelImport(" + modelname + ")");
      return result;
   }
   
   // public interface

   string IliFile::getFilePath()
   {
      return filepath;
   }

   string IliFile::getIliVersion()
   {
      return iliversion;
   }

   list<string> IliFile::getImports()
   {
      return imports;
   }

   list<string> IliFile::getTranslationModels()
   {
      return translation_of;
   }

   list<string> IliFile::getRequiredModels()
   {
      list<string> required = imports;
      required.insert(required.end(), translation_of.begin(), translation_of.end());
      return required;
   }

   list<string> IliFile::getModels()
   {
      return models;
   }
   
   void IliFile::setAutoSearch(bool auto_search)
   {
      this->auto_search = auto_search;
   }
   
   bool IliFile::getAutoSearch()
   {
      return auto_search;
   }
   
   void set_autosearch(bool auto_search)
   {
      util::auto_search = auto_search;
   }
   
   void set_ilidirs(string dirs)
   {
      while (true) {
         int pos = dirs.find(";"); // for ili2c compatiblity
         if (pos < 0) {
            pos = dirs.find(",");
         }
         string dir;
         if (pos > 0) {
            dir = dirs.substr(0,pos);
            dirs = dirs.substr(pos+1);
         }
         else {
            dir = dirs;
         }
         if (dir != "" && !filesystem::is_directory(dir)) {
            Log.error("ilidirs: " + dir + " is not a directory");
            exit(1);
         }
         ilidirs.push_back(dir);
         if (pos < 0) {
            break;
         }
      }
   }
   
   static IliFile* load_ilifile(string filepath)
   {
      
      for (auto f : all_ilifiles_full) {
         if (f->getFilePath() == filepath) {
            return f;
         }
      }

      Log.debug(">>> load_ilifile(" + filepath + ")");
      Log.incNestLevel();

      Log.debug("opening source " + filepath);
      string source = util::load_string(filepath);
      bool registered = ilic::activeSourceManager() != nullptr
         && ilic::activeSourceManager()->contains(filepath);
      if (source.empty() && !registered && !filesystem::exists(filepath)) {
         Log.error("unable to open " + filepath);
         return nullptr;
      }

      // create lexer
      antlr4::ANTLRInputStream input(source);
      Log.debug("creating ilifile lexer (1) ...");
      lexer::IliFileLexer lexer(&input);
      Log.debug("creating ilifile lexer (2) ...");
      antlr4::CommonTokenStream tokens(&lexer);
      lexer.removeErrorListeners();

      // create parser
      Log.debug("creating ilifile parser ...");
      parser::IliFileParser parser(&tokens);
      parser::IliFileParser::IliFileContext *context = parser.iliFile();
      if (context == nullptr) {
         Log.error("unable to parse ili file structure " + filepath);
         return nullptr;
      }

      // run parser
      Log.debug("running parser");
      util::IliFile *ilifile = new IliFile(filepath);
      ilifile->visitIliFile(context);
      
      all_ilifiles_full.push_back(ilifile);

      Log.decNestLevel();
      Log.debug("<<< load_ilifile(" + filepath + ")");

      return ilifile;

   }

   static void add_ilifile(IliFile *f)
   {
      for (auto ff : all_ilifiles) {
         if (util::compare_case_insensitive(ff->getFilePath(),f->getFilePath())) {
            return;
         }
      }
      all_ilifiles.push_back(f);
   }

   IliFile* load_ilifiles_by_file(string filepath)
   {
      IliFile *f = load_ilifile(filepath);
      if (f != nullptr) {
         add_ilifile(f);
         return f;
      }
      else {
         return nullptr;
      }
   }
   
   IliFile* load_ilifiles_by_model(string modelname,string iliversion)
   {
      
      // model already loaded?
      if (all_ilimodels.find(modelname) != all_ilimodels.end()) {
         IliFile* f = all_ilimodels[modelname];
         if (f->getIliVersion() == iliversion) {
            add_ilifile(f);
            return f;
         }
      }
      
      if (modelname == "INTERLIS") {
         IliFile *interlis = new IliFile("INTERLIS");
         all_ilifiles.push_front(interlis);
         return interlis;
      }

      // Virtual sources supplied by CompilerSession are searched before the
      // filesystem. This is the import path used by WASM and editor buffers.
      if (ilic::activeSourceManager() != nullptr) {
         for (const string &uri : ilic::activeSourceManager()->uris()) {
            IliFile *ff = load_ilifile(uri);
            if (ff == nullptr || ff->getIliVersion() != iliversion) continue;
            for (const string &model : ff->getModels()) {
               if (model == modelname) {
                  ff->setAutoSearch(true);
                  add_ilifile(ff);
                  return ff;
               }
            }
         }
      }

      if (!auto_search) {
         Log.error("model " + modelname + " not found (no_auto).");
         return nullptr;
      }

      // 1. try: search <modelname>.ili in ilidirs
      for (string ilidir : ilidirs) {
         string filepath = "";
         if (ilidir != "") {
            filepath = (filesystem::path(ilidir) / (modelname + ".ili")).string();
         }
         else {
            filepath = modelname + ".ili";
         }
         ifstream stream;
         stream.open(filepath);
         if (!stream.is_open()) {
            continue;
         }
         stream.close();
         IliFile *ff = load_ilifile(filepath);
         if (ff->getIliVersion() == iliversion) {
            add_ilifile(ff);
            return ff;
         }
      }

      // 2. try: search in any .ili in ilidirs
      for (string ilidir : ilidirs) {
         if (ilidir == "") {
            ilidir = "./"; // clang does not like empty directory names
         }
         Log.debug("loading .ili files in directory " + ilidir + " ...");
         for (filesystem::directory_entry entry : filesystem::directory_iterator(ilidir)) {
            string filepath = entry.path().string();
            if (!ends_with(filepath, ".ili")) {
               continue;
            }
            if (filepath.substr(0,2) == "./") {
               filepath = filepath.substr(2);
            }
            IliFile *ff = load_ilifile(filepath);
            for (string model : ff->getModels()) {
               // search for model in .ili file
               if (model == modelname) {
                  ff->setAutoSearch(true);
                  if (ff->getIliVersion() == iliversion) {
                     add_ilifile(ff);
                     return ff;
                  }
               }
            }
         }
      }

      return nullptr;

   }

}
