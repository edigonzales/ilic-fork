/*

to do !!!

*/

#include <string>
#include <iostream>
#include <fstream>

#include "Logger.h"

using namespace util;

//-------------------------------------------------------------
// private interface
//-------------------------------------------------------------

void Logger::setIdentSpaces(int spaces)
{
   ident_spaces = spaces;
}

string Logger::ident(int nestlevel)
{

   string spaces = "";
   for (int i=0;i<ident_spaces;i++) {
      spaces += " ";
   }

   string ident = "";
   for (int i=0;i<nestlevel;i++) {
      ident += spaces;
   }

   return ident;

};

string Logger::ident()
{
   return ident(nestLevel);
};

//-------------------------------------------------------------
// public interface
//-------------------------------------------------------------

Logger Log;

// general

void Logger::reset()
{
   closeFile();
   nestLevel = 0;
   errorcount = 0;
   warningcount = 0;
   silent = false;
   display_info = true;
   display_warning = true;
   display_error = true;
   display_debug = false;
   warnings_as_errors = false;
}

void Logger::setLevel(int level)
{
   nestLevel = level;
};

void Logger::incNestLevel()
{
   nestLevel++;
};

void Logger::decNestLevel()
{
   nestLevel--;
};

void Logger::openFile(string log_file)
{
   out.open(log_file);
   nestLevel = 0;
};

void Logger::closeFile()
{
   out.close();
};

void Logger::setSilent(bool state)
{
   silent = state;
}

// message

void Logger::messageNoNL(string message)
{
   out << message;
   if (silent) return;
   std::cout << message;
};

void Logger::messageNoIdent(string message)
{
   messageNoNL(message + "\n");
};

void Logger::message(string message)
{
   this->messageNoIdent(ident() + message);
};

// info

void Logger::displayInfo(bool state)
{
   display_info = state;
}

void Logger::info(string message)
{
   if (!display_info) return;
   this->messageNoIdent("inf: " + ident() + message);
};

void Logger::infoNoNL(string message)
{
   if (!display_info) return;
   this->messageNoNL("inf: " + ident() + message);
};

void Logger::infoAppend(string message)
{
   if (!display_info) return;
   this->messageNoIdent(message);
};

void Logger::info(int nestlevel,string message)
{
   if (!display_info) return;
   this->messageNoIdent("inf: " + ident(nestlevel) + message);
};

// debug

void Logger::displayDebug(bool state)
{
   display_debug = state;
}

void Logger::debug(string message)
{
   if (!display_debug) return;
   this->messageNoIdent("dbg: " + ident() + message);
};

// warning

void Logger::warningsAsErrors()
{
   warnings_as_errors = true;
}

void Logger::displayWarnings(bool state)
{
   display_warning = state;
}

void Logger::warning(string message)
{
   if (warnings_as_errors) {
      error(message);
   }
   else {
      if (!display_warning) return;
      this->messageNoIdent("wrn: " + ident() + message);
      warningcount++;
   }
};

void Logger::warning(string message, int line)
{
   if (warnings_as_errors) {
      error(message,line);
   }
   else {
      if (!display_warning) return;
      this->messageNoIdent("wrn:    line=" + to_string(line) + ", " + message);
      warningcount++;
   }
}

int Logger::getWarningCount()
{
   return warningcount;
}

// error

void Logger::displayErrors(bool state)
{
   display_error = state;
}

void Logger::error(string message)
{
   if (!display_error) return;
   this->messageNoIdent("err:    " + message);
   errorcount++;
}

void Logger::errorNoIdent(string message)
{
   if (!display_error) return;
   this->messageNoIdent("err: " + message);
   errorcount++;
}

void Logger::error(string message,int line)
{
   if (!display_error) return;
   this->messageNoIdent("err:    line=" + to_string(line) + ", " + message);
   errorcount++;
}

int Logger::getErrorCount()
{
   return errorcount;
}

// internal error

void Logger::internal_error(string message)
{
   this->messageNoIdent("int: " + message);
   exit(1);
}

void Logger::internal_error(string message,int error_code)
{
   this->messageNoIdent("int: " + message);
   exit(error_code);
}

void Logger::internal_error(string message,exception e,int error_code)
{
   this->messageNoIdent("int: " + message + " (" + string(e.what()) + ")");
   exit(error_code);
}
