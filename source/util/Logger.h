/*

to do !!!

*/

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <stdexcept>

#include "../../include/ilic/Diagnostic.h"
#include "DiagnosticCode.h"

using namespace std;

namespace util {

   class CompilerAbort : public runtime_error {
   public:
      CompilerAbort(string message,int code) : runtime_error(std::move(message)), code_(code) {}
      int code() const noexcept { return code_; }
   private:
      int code_;
   };

   class Logger {
      public:

         class SourceScope final {
         public:
            SourceScope(Logger &logger,string source);
            ~SourceScope() noexcept;
            SourceScope(const SourceScope &) = delete;
            SourceScope &operator=(const SourceScope &) = delete;
            SourceScope(SourceScope &&other) noexcept;
            SourceScope &operator=(SourceScope &&) = delete;
         private:
            Logger *logger_;
            string previous_;
         };

         class CategoryScope final {
         public:
            CategoryScope(Logger &logger,string category);
            ~CategoryScope() noexcept;
            CategoryScope(const CategoryScope &) = delete;
            CategoryScope &operator=(const CategoryScope &) = delete;
            CategoryScope(CategoryScope &&other) noexcept;
            CategoryScope &operator=(CategoryScope &&) = delete;
         private:
            Logger *logger_;
            string previous_;
         };

         class IndentScope final {
         public:
            explicit IndentScope(Logger &logger);
            ~IndentScope() noexcept;
            IndentScope(const IndentScope &) = delete;
            IndentScope &operator=(const IndentScope &) = delete;
            IndentScope(IndentScope &&other) noexcept;
            IndentScope &operator=(IndentScope &&) = delete;
         private:
            Logger *logger_;
         };

         Logger() = default;
         ~Logger() = default;
         Logger(const Logger &) = delete;
         Logger &operator=(const Logger &) = delete;
         Logger(Logger &&) = delete;
         Logger &operator=(Logger &&) = delete;

         // general
         void openFile(string log_file);
         void closeFile();
         void setLevel(int level);
         void incNestLevel();
         void decNestLevel();
         void setSilent(bool state);
         void setIdentSpaces(int spaces);
         void message(string message);
         void messageNoNL(string message);
         void messageNoIdent(string message);
         void warningsAsErrors();
         void reset();
         void setCurrentSource(string uri);
         void setCategory(string category);
         SourceScope sourceScope(string uri) { return SourceScope(*this,std::move(uri)); }
         CategoryScope categoryScope(string category) { return CategoryScope(*this,std::move(category)); }
         IndentScope indentScope() { return IndentScope(*this); }
         // Kept as a source-compatible no-op. Core errors always become
         // CompilerAbort; process termination belongs exclusively to the CLI.
         void setAbortWithException(bool state);
         const string &getCurrentSource() const;
         void setLogSink(ilic::LogSink sink);
         void setDiagnosticSink(ilic::DiagnosticSink sink);
         const vector<ilic::Diagnostic> &getDiagnostics() const;
         const vector<ilic::LogEvent> &getLogEvents() const;
         void log(ilic::LogLevel level,string category,string message,
            map<string,string> context = {});

         // info
         void displayInfo(bool state);
         void info(string message);
         void info(int nestlevel,string message);
         void infoNoNL(string message);
         void infoAppend(string message);

         // debug
         void displayDebug(bool state);
         void debug(string message);

         // warning
         void displayWarnings(bool state);
         void warning(string message);
         void warning(string message,int line);
         void warning(string message,int line,int column,string code);
         int getWarningCount();

         // error
         void displayErrors(bool state);
         void error(string message);
         void errorNoIdent(string message);
         void error(string message,int line);
         void error(string message,int line,int column,string code = "",
            vector<ilic::RelatedInformation> relatedInformation = {},
            vector<string> notes = {});
         void error(string message,const ilic::SourceRange &range,string code = "",
            vector<ilic::RelatedInformation> relatedInformation = {},
            vector<string> notes = {});
         void error(DiagnosticId id,string message,int line,int column = 0,
            vector<ilic::RelatedInformation> relatedInformation = {},
            vector<string> notes = {});
         void error(DiagnosticId id,string message,const ilic::SourceRange &range,
            vector<ilic::RelatedInformation> relatedInformation = {},
            vector<string> notes = {});
         int getErrorCount();
         
         // internal_error
         void internal_error(string message);
         void internal_error(string message,int error_code);
         void internal_error(string message,exception e,int error_code);

      private:
         ofstream out;
         int nestLevel = 0;
         int errorcount = 0;
         int warningcount = 0;
         string ident();
         string ident(int nestlevel);
         bool silent = false;
         bool display_info = true;
         bool display_warning = true;
         bool display_error = true;
         bool display_debug = false;
         int ident_spaces = 3;
         bool warnings_as_errors = false;
         string current_source;
         string current_category = "compiler";
         vector<ilic::Diagnostic> diagnostics;
         vector<ilic::LogEvent> events;
         ilic::LogSink log_sink;
         ilic::DiagnosticSink diagnostic_sink;
         void recordDiagnostic(ilic::Diagnostic diagnostic);
};

};
