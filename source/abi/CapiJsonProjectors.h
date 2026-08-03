#pragma once

#include "../../include/ilic/Compiler.h"
#include "../../include/ilic/Editor.h"
#include "../../include/ilic/Formatter.h"
#include "../../include/ilic/Semantic.h"
#include "../../include/ilic/Syntax.h"
#include "Json.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ilic::capi {

using json::Value;

Value errorResult(const char *kind,const std::string &message,
   const std::string &uri = {},std::uint64_t documentVersion = 0);
Value compileResult(const CompilationResult &result);
Value syntaxResult(const SyntaxSnapshot &result);
Value editorResult(const EditorSnapshot &result);
Value semanticResult(const SemanticSnapshot &result);
Value compilationAnalysisResult(const CompilationAnalysisResult &result);
Value incrementalStatsResult(const IncrementalStats &stats);
Value incrementalTraceResult(const IncrementalTrace &trace);
Value incrementalCacheSnapshotResult(const IncrementalCacheSnapshot &snapshot);
Value formattingResult(const FormatResult &result);
Value diagnostics(const std::vector<Diagnostic> &values);

} // namespace ilic::capi
