#pragma once

#include "../../include/ilic/Compiler.h"

#include <string>
#include <vector>

namespace ilic::detail {

class CompilerContext;

class CompilationRun final {
public:
   explicit CompilationRun(CompilerContext &context) noexcept : context_(context) {}
   CompilationResult run(const CompilationRequest &request,
      std::vector<std::string> &sourceUris);

private:
   CompilerContext &context_;
};

} // namespace ilic::detail
