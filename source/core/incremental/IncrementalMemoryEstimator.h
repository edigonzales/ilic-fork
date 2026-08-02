#pragma once

#include "../ParsedSourceArtifact.h"

#include "../../../include/ilic/Editor.h"
#include "../../../include/ilic/Semantic.h"
#include "../../../include/ilic/Syntax.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ilic::detail {

class IncrementalMemoryEstimator final {
public:
   static std::size_t sourceBytes(const std::string &value) noexcept;
   static std::size_t diagnostics(const std::vector<Diagnostic> &values) noexcept;
   static std::size_t syntax(const SyntaxSnapshot &value) noexcept;
   static std::size_t editor(const EditorSnapshot &value) noexcept;
   static std::size_t artifact(const ParsedSourceArtifact &value) noexcept;
   static std::size_t compilation(const CompilationAnalysisResult &value) noexcept;
};

} // namespace ilic::detail
