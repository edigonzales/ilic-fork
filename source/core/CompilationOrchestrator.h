#pragma once

#include "../../include/ilic/Compiler.h"

namespace ilic::detail {

class CompilerSessionState;

class CompilationOrchestrator final {
public:
   static CompilationAnalysisResult compileAndAnalyze(CompilerSessionState &state,
      const CompilationRequest &request,std::uint64_t &invocationCount);
   static CompilationResult compile(CompilerSessionState &state,
      const CompilationRequest &request);
};

} // namespace ilic::detail
