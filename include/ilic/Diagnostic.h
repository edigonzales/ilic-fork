#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace ilic {

enum class DiagnosticSeverity { Error, Warning, Information, Hint };

struct Position {
   std::size_t line = 0;
   std::size_t character = 0;
   std::size_t byteOffset = 0;
};

struct SourceRange {
   std::string uri;
   Position start;
   Position end;
   bool valid = false;
};

struct RelatedInformation {
   SourceRange range;
   std::string message;
};

struct Diagnostic {
   DiagnosticSeverity severity = DiagnosticSeverity::Error;
   std::string code;
   std::string message;
   SourceRange range;
   std::vector<RelatedInformation> relatedInformation;
   std::vector<std::string> notes;
   bool treatedAsError = false;
};

enum class LogLevel { Trace, Debug, Information, Warning, Error };

struct LogEvent {
   LogLevel level = LogLevel::Information;
   std::string category;
   std::string message;
   std::map<std::string, std::string> context;
};

using LogSink = std::function<void(const LogEvent &)>;
using DiagnosticSink = std::function<void(const Diagnostic &)>;

} // namespace ilic
