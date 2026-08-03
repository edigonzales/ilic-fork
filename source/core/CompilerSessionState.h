#pragma once

#include "../../include/ilic/Compiler.h"
#include "incremental/ParsedSourceCache.h"
#include "incremental/RootAnalysisCache.h"
#include "incremental/SourceDependencyIndex.h"
#include "incremental/SourceModelIndex.h"

#include <memory>
#include <mutex>

namespace ilic::detail {

// Session-owned mutable state. Services operate on this aggregate so parsing,
// invalidation and compilation share one explicit lifetime and lock domain.
class CompilerSessionState final {
public:
   explicit CompilerSessionState(const IncrementalCacheOptions &options);

   std::shared_ptr<const ParsedSourceCacheEntry> ensureParsed(
      const SourceBuffer &source,ParseMode mode);
   ParsedSourceArtifactPtr parsedArtifact(const SourceBuffer &source);
   void syncCacheStats();

   SourceManager sources;
   std::mutex mutex;
   ParsedSourceCache parsedCache;
   RootAnalysisCache rootCache;
   SourceModelIndex modelIndex;
   SourceDependencyIndex dependencyIndex;
   IncrementalStats stats;
   IncrementalTrace trace;
};

} // namespace ilic::detail
