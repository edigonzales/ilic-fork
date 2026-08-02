#pragma once

#include "RootAnalysisCache.h"
#include "SourceModelIndex.h"

#include "../../../include/ilic/SourceManager.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ilic::detail {

struct SourceModelContribution final {
   std::string uri;
   std::string iliVersion;
   std::vector<std::string> definedModels;
   std::vector<std::string> requiredModels;
};

struct SourceImpact final {
   bool invalidateParser = false;
   bool replaceSourceIndex = false;
   bool invalidateAllRoots = false;
   bool mayCreateAmbiguity = false;
   std::size_t observedRootEntries = 0;
   std::vector<std::string> affectedModels;
   std::vector<std::string> missingModelsResolved;
};

class SourceImpactAnalyzer final {
public:
   SourceImpact analyze(const SourceUpdateResult &update,
      const SourceModelRecord *before,const SourceModelRecord *after,
      const RootAnalysisCache &roots) const;
};

} // namespace ilic::detail
