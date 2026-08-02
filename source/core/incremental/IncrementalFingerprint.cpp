#include "IncrementalFingerprint.h"

#include "../../../include/ilic/Compiler.h"
#include "../../input/ili2/InterlisModel.h"

#include <iomanip>
#include <sstream>

namespace ilic::detail {
namespace {

std::string hashText(const std::string &text)
{
   std::uint64_t hash = 1469598103934665603ULL;
   for (const unsigned char byte : text) {
      hash ^= byte;
      hash *= 1099511628211ULL;
   }
   std::ostringstream result;
   result << std::hex << std::setw(16) << std::setfill('0') << hash;
   return result.str();
}

} // namespace

std::string CompilerFingerprint::canonicalString() const
{
   std::ostringstream value;
   value << "compiler=" << compilerVersion << "|abi=" << abiVersion
      << "|grammar=" << grammarFingerprint << "|builtin=" << builtinFingerprint
      << "|semantic=" << semanticRulesFingerprint;
   return value.str();
}

CompilerFingerprint currentCompilerFingerprint()
{
   const std::string builtin = input::getInterlisModel23();
   return {ilic::version(),1,"generated-antlr-v1-recovery-v1",hashText(builtin),
      "semantic-checker-v1-translation-checker-v1"};
}

} // namespace ilic::detail
