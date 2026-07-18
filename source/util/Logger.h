/*

to do !!!

*/

#pragma once

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

namespace util {

   class Logger {
      public:

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
         int getWarningCount();

         // error
         void displayErrors(bool state);
         void error(string message);
         void errorNoIdent(string message);
         void error(string message,int line);
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
   };

};

extern util::Logger Log;
