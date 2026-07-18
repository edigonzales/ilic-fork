#pragma once

#include "Diagnostic.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ilic {

struct FormatOptions {
   std::size_t indentSize = 2;
   bool requireValidSyntax = true;
};

struct FormatResult {
   bool success = false;
   bool applicable = false;
   bool changed = false;
   std::string text;
   std::vector<Diagnostic> diagnostics;
};

class Formatter {
public:
   FormatResult format(const std::string &uri,const std::string &utf8,
      const FormatOptions &options = {}) const;
};

} // namespace ilic
