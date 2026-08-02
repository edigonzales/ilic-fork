#pragma once

#include <cstdint>
#include <string>

namespace ilic::detail {

struct CompilerFingerprint final {
   std::string compilerVersion;
   std::uint32_t abiVersion = 1;
   std::string grammarFingerprint;
   std::string builtinFingerprint;
   std::string semanticRulesFingerprint;

   std::string canonicalString() const;
};

CompilerFingerprint currentCompilerFingerprint();

} // namespace ilic::detail
