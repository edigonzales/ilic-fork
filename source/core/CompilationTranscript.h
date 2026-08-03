#pragma once

#include "../../include/ilic/Diagnostic.h"

#include <cstddef>
#include <string>
#include <vector>

namespace util {
class IliFile;
class Logger;
}

namespace ilic::detail {

class CompilationTranscript final {
public:
   static void appendNewEvents(std::vector<std::string> &transcript,
      std::size_t &diagnosticIndex,std::size_t &logIndex,const util::Logger &logger);
   static std::string completionLine(int errorCount,int warningCount);
   static std::string timestamp();
   static void appendInputFile(std::vector<std::string> &transcript,util::IliFile *file);
};

} // namespace ilic::detail
