#pragma once

#include "../SnapshotPipeline.h"

#include "../../../include/ilic/Incremental.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace ilic::detail {

struct ParsedSourceKey final {
   std::string uri;
   std::uint64_t contentRevision = 0;
   std::string contentHash;
   std::size_t byteLength = 0;
   ParseMode mode = ParseMode::StrictCompiler;
   std::string grammarFingerprint;

   std::string canonicalString() const;
};

struct ParsedSourceCacheEntry final {
   ParsedSourceKey key;
   std::string sourceBytes;
   ParsedSourceArtifactPtr artifact;
   SyntaxSnapshot syntax;
   EditorSnapshot editor;
   std::size_t retainedBytes = 0;
   mutable std::uint64_t lastUsedTick = 0;
};

struct ParsedSourceCacheMetrics final {
   std::uint64_t strictBuilds = 0;
   std::uint64_t tolerantBuilds = 0;
   std::uint64_t strictHits = 0;
   std::uint64_t tolerantHits = 0;
   std::uint64_t evictions = 0;
   std::size_t entries = 0;
   std::size_t retainedBytes = 0;
};

class ParsedSourceCache final {
public:
   using Builder = std::function<SnapshotBundle(const SourceBuffer &,ParseMode)>;

   explicit ParsedSourceCache(ParsedSourceCacheOptions options = {});

   std::shared_ptr<const ParsedSourceCacheEntry> getOrBuild(
      const SourceBuffer &source,const SourceIdentity &identity,ParseMode mode,
      const std::string &grammarFingerprint,const Builder &builder);

   std::size_t invalidateUri(std::string_view uri);
   std::size_t clear();
   void resetMetrics() noexcept;
   ParsedSourceCacheMetrics metrics() const noexcept;
   bool checkInvariants(std::string *error = nullptr) const;

private:
   using EntryPtr = std::shared_ptr<ParsedSourceCacheEntry>;
   std::size_t erase(std::map<std::string,EntryPtr>::iterator iterator);
   void evictUntilWithinBudget();

   ParsedSourceCacheOptions options_;
   std::map<std::string,EntryPtr> entries_;
   std::size_t retainedBytes_ = 0;
   std::uint64_t tick_ = 0;
   ParsedSourceCacheMetrics metrics_;
};

} // namespace ilic::detail
