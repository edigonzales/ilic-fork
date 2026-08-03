#include "SnapshotParserSupport.h"

#include <algorithm>

namespace ilic::detail {

void sortDiagnostics(std::vector<Diagnostic> &diagnostics)
{
   std::stable_sort(diagnostics.begin(),diagnostics.end(),[](const Diagnostic &left,const Diagnostic &right) {
      const std::size_t leftStart = left.range.valid ? left.range.start.byteOffset : 0;
      const std::size_t rightStart = right.range.valid ? right.range.start.byteOffset : 0;
      const std::size_t leftEnd = left.range.valid ? left.range.end.byteOffset : 0;
      const std::size_t rightEnd = right.range.valid ? right.range.end.byteOffset : 0;
      if (left.range.uri != right.range.uri) return left.range.uri < right.range.uri;
      if (leftStart != rightStart) return leftStart < rightStart;
      if (leftEnd != rightEnd) return leftEnd < rightEnd;
      if (left.severity != right.severity) return static_cast<int>(left.severity) < static_cast<int>(right.severity);
      if (left.code != right.code) return left.code < right.code;
      return left.message < right.message;
   });
   diagnostics.erase(std::unique(diagnostics.begin(),diagnostics.end(),[](const Diagnostic &left,const Diagnostic &right) {
      return left.code == right.code && left.severity == right.severity
         && left.message == right.message && left.range.valid == right.range.valid
         && (!left.range.valid || (left.range.uri == right.range.uri
            && left.range.start.byteOffset == right.range.start.byteOffset
            && left.range.end.byteOffset == right.range.end.byteOffset));
   }),diagnostics.end());
}



} // namespace ilic::detail

