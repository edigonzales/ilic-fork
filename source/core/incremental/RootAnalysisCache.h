#pragma once

#include "../../../include/ilic/Incremental.h"
#include "../../../include/ilic/Semantic.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace ilic::detail {

struct RootAnalysisCacheEntry final {
   std::string baseKey;
   std::string key;
   std::vector<std::string> closureUris;
   std::vector<SourceIdentity> identities;
   std::vector<std::string> resolvedModels;
   std::vector<std::string> missingModels;
   bool hadMissingModels = false;
   CompilationAnalysisResult result;
   std::size_t retainedBytes = 0;
   mutable std::uint64_t lastUsedTick = 0;
};

struct RootInvalidationResult final {
   std::vector<std::string> invalidatedKeyIds;
   std::vector<std::string> invalidatedRoots;
   std::size_t releasedBytes = 0;
};

struct RootAnalysisCacheMetrics final {
   std::uint64_t hits = 0;
   std::uint64_t misses = 0;
   std::uint64_t evictions = 0;
   std::size_t entries = 0;
   std::size_t retainedBytes = 0;
};

class RootAnalysisCache final {
public:
   explicit RootAnalysisCache(RootAnalysisCacheOptions options = {});

   std::shared_ptr<const RootAnalysisCacheEntry> findExact(
      const std::string &baseKey,const SourceManager &sources);
   void insert(RootAnalysisCacheEntry entry);
   RootInvalidationResult invalidateBySource(std::string_view uri);
   RootInvalidationResult invalidateIf(
      const std::function<bool(const RootAnalysisCacheEntry &)> &predicate);
   RootInvalidationResult clear();
   void resetMetrics() noexcept;
   RootAnalysisCacheMetrics metrics() const noexcept;
   bool checkInvariants(std::string *error = nullptr) const;

private:
   using EntryPtr = std::shared_ptr<RootAnalysisCacheEntry>;
   std::size_t erase(std::map<std::string,EntryPtr>::iterator iterator,
      RootInvalidationResult *result = nullptr);
   void evictUntilWithinBudget();

   RootAnalysisCacheOptions options_;
   std::map<std::string,EntryPtr> entries_;
   std::size_t retainedBytes_ = 0;
   std::uint64_t tick_ = 0;
   RootAnalysisCacheMetrics metrics_;
};

} // namespace ilic::detail
