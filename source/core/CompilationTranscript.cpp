#include "CompilationTranscript.h"

#include "../util/IliFile.h"
#include "../util/Logger.h"

#include <chrono>
#include <ctime>

namespace ilic::detail {
namespace {

std::string diagnosticLine(const Diagnostic &diagnostic)
{
   const bool error = diagnostic.treatedAsError ||
      diagnostic.severity == DiagnosticSeverity::Error;
   const std::string prefix = error ? "err:" :
      (diagnostic.severity == DiagnosticSeverity::Warning ? "wrn:" : "inf:");
   std::string line = prefix + (error || diagnostic.severity == DiagnosticSeverity::Warning
      ? "    " : " ");
   if (diagnostic.range.valid)
      line += diagnostic.range.uri + ":" + std::to_string(diagnostic.range.start.line + 1)
         + ":" + std::to_string(diagnostic.range.start.character + 1) + ": ";
   line += diagnostic.message;
   return line;
}

} // namespace

void CompilationTranscript::appendNewEvents(std::vector<std::string> &transcript,
   std::size_t &diagnosticIndex,std::size_t &logIndex,const util::Logger &logger)
{
   const auto &diagnostics = logger.getDiagnostics();
   while (diagnosticIndex < diagnostics.size())
      transcript.push_back(diagnosticLine(diagnostics[diagnosticIndex++]));

   const auto &events = logger.getLogEvents();
   while (logIndex < events.size()) {
      const auto &event = events[logIndex++];
      const std::string prefix = event.level == LogLevel::Error ? "err:" :
         (event.level == LogLevel::Warning ? "wrn:" :
         (event.level == LogLevel::Debug ? "dbg:" : "inf:"));
      transcript.push_back(prefix + "    " + event.message);
   }
}

std::string CompilationTranscript::timestamp()
{
   const std::time_t now = std::chrono::system_clock::to_time_t(
      std::chrono::system_clock::now());
   std::tm local{};
#if defined(_WIN32)
   localtime_s(&local,&now);
#else
   localtime_r(&now,&local);
#endif
   char value[20]{};
   if (std::strftime(value,sizeof(value),"%Y-%m-%d %H:%M:%S",&local) == 0)
      return "0000-00-00 00:00:00";
   return value;
}

std::string CompilationTranscript::completionLine(int errorCount,int warningCount)
{
   std::string line = "inf: ilic completed with";
   if (errorCount == 0) line += " no errors";
   else if (errorCount == 1) line += " 1 error";
   else line += " " + std::to_string(errorCount) + " errors";
   if (warningCount == 0) line += ", no warnings";
   else if (warningCount == 1) line += ", 1 warning";
   else line += ", " + std::to_string(warningCount) + " warnings";
   return line + " " + timestamp();
}

void CompilationTranscript::appendInputFile(std::vector<std::string> &transcript,
   util::IliFile *file)
{
   if (file == nullptr || file->getFilePath() == "INTERLIS") return;
   std::string models;
   for (const auto &model : file->getModels()) {
      if (!models.empty()) models += ",";
      models += model;
   }
   transcript.push_back("inf:    " + file->getFilePath() + ", iliversion=" +
      file->getIliVersion() + ", models=" + models + ", " +
      (file->getAutoSearch() ? "auto search" : "command line"));
}

} // namespace ilic::detail
