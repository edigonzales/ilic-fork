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
   current_source.clear();
   current_category = "compiler";
   abort_with_exception = false;
   diagnostics.clear();
   events.clear();
}

void Logger::setCurrentSource(string uri) { current_source = std::move(uri); }
void Logger::setCategory(string category) { current_category = std::move(category); }
void Logger::setAbortWithException(bool state) { abort_with_exception = state; }
const string &Logger::getCurrentSource() const { return current_source; }
void Logger::setLogSink(ilic::LogSink sink) { log_sink = std::move(sink); }
void Logger::setDiagnosticSink(ilic::DiagnosticSink sink) { diagnostic_sink = std::move(sink); }
const vector<ilic::Diagnostic> &Logger::getDiagnostics() const { return diagnostics; }
const vector<ilic::LogEvent> &Logger::getLogEvents() const { return events; }

void Logger::log(ilic::LogLevel level,string category,string message,map<string,string> context)
{
   ilic::LogEvent event{level,std::move(category),std::move(message),std::move(context)};
   events.push_back(event);
   if (log_sink) log_sink(event);
}

void Logger::recordDiagnostic(ilic::Diagnostic diagnostic)
{
   diagnostics.push_back(diagnostic);
   if (diagnostic_sink) diagnostic_sink(diagnostic);
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
   log(ilic::LogLevel::Information,current_category,message);
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
   log(ilic::LogLevel::Debug,current_category,message);
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
   ilic::Diagnostic diagnostic;
   diagnostic.severity = ilic::DiagnosticSeverity::Warning;
   diagnostic.code = "ILIC-WARNING";
   diagnostic.message = message;
   diagnostic.treatedAsError = warnings_as_errors;
   recordDiagnostic(diagnostic);
   warningcount++;
   if (warnings_as_errors) {
      errorcount++;
      if (display_error) this->messageNoIdent("err:    " + ident() + message);
   }
   else {
      if (display_warning) this->messageNoIdent("wrn: " + ident() + message);
   }
};

void Logger::warning(string message, int line)
{
   warning(std::move(message),line,0,"ILIC-WARNING");
}

void Logger::warning(string message,int line,int column,string code)
{
   ilic::Diagnostic diagnostic;
   diagnostic.severity = ilic::DiagnosticSeverity::Warning;
   diagnostic.code = std::move(code);
   diagnostic.message = message;
   diagnostic.treatedAsError = warnings_as_errors;
   if (line > 0 && !current_source.empty()) {
      diagnostic.range.valid = true;
      diagnostic.range.uri = current_source;
      diagnostic.range.start.line = static_cast<size_t>(line - 1);
      diagnostic.range.start.character = static_cast<size_t>(column);
      diagnostic.range.end = diagnostic.range.start;
      diagnostic.range.end.character++;
   }
   recordDiagnostic(diagnostic);
   warningcount++;
   if (warnings_as_errors) {
      errorcount++;
      if (display_error) this->messageNoIdent("err:    " + current_source + ":" +
         to_string(line) + ":" + to_string(column + 1) + ": " + message);
   }
   else {
      if (display_warning) this->messageNoIdent("wrn:    " + current_source + ":" +
         to_string(line) + ":" + to_string(column + 1) + ": " + message);
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
   ilic::Diagnostic diagnostic;
   diagnostic.code = "ILIC-COMPILER";
   diagnostic.message = message;
   recordDiagnostic(diagnostic);
   errorcount++;
   if (display_error) this->messageNoIdent("err:    " + message);
}

void Logger::errorNoIdent(string message)
{
   ilic::Diagnostic diagnostic;
   diagnostic.code = "ILIC-COMPILER";
   diagnostic.message = message;
   recordDiagnostic(diagnostic);
   errorcount++;
   if (display_error) this->messageNoIdent("err: " + message);
}

void Logger::error(string message,int line)
{
   error(std::move(message),line,0,"ILIC-SEMANTIC");
}

void Logger::error(string message,int line,int column,string code)
{
   ilic::Diagnostic diagnostic;
   diagnostic.code = std::move(code);
   diagnostic.message = message;
   if (line > 0 && !current_source.empty()) {
      diagnostic.range.valid = true;
      diagnostic.range.uri = current_source;
      diagnostic.range.start.line = static_cast<size_t>(line - 1);
      diagnostic.range.start.character = static_cast<size_t>(column);
      diagnostic.range.end = diagnostic.range.start;
      diagnostic.range.end.character++;
   }
   recordDiagnostic(diagnostic);
   errorcount++;
   if (display_error) {
      if (!current_source.empty() && line > 0) {
         this->messageNoIdent("err:    " + current_source + ":" + to_string(line) + ":" +
            to_string(column + 1) + ": " + message);
      }
      else this->messageNoIdent("err:    " + message);
   }
}

int Logger::getErrorCount()
{
   return errorcount;
}

// internal error

void Logger::internal_error(string message)
{
   this->messageNoIdent("int: " + message);
   if (abort_with_exception) throw CompilerAbort(message,1);
   exit(1);
}

void Logger::internal_error(string message,int error_code)
{
   this->messageNoIdent("int: " + message);
   if (abort_with_exception) throw CompilerAbort(message,error_code);
   exit(error_code);
}

void Logger::internal_error(string message,exception e,int error_code)
{
   this->messageNoIdent("int: " + message + " (" + string(e.what()) + ")");
   if (abort_with_exception) throw CompilerAbort(message + " (" + string(e.what()) + ")",error_code);
   exit(error_code);
}
