#pragma once

#include "Diagnostic.h"

#include <string>
#include <vector>

namespace ilic {

class DiagnosticPipeline final {
public:
   PublishedDiagnostics publish(std::vector<DiagnosticCandidate> candidates) const;

private:
   static void normalize(Diagnostic &diagnostic,DiagnosticPipelineStats &stats);
   static void deduplicate(std::vector<DiagnosticCandidate> &candidates,
      DiagnosticPipelineStats &stats);
   static void suppressCascades(std::vector<DiagnosticCandidate> &candidates,
      DiagnosticPipelineStats &stats);
   static void sort(std::vector<DiagnosticCandidate> &candidates);
};

} // namespace ilic
