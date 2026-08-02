#include "ParsedSourceCache.h"

#include "IncrementalMemoryEstimator.h"

#include <algorithm>
#include <sstream>

namespace ilic::detail {

std::string ParsedSourceKey::canonicalString() const
{
   std::ostringstream value;
   value << uri.size() << ':' << uri << '|' << contentRevision << '|'
      << contentHash.size() << ':' << contentHash << '|' << byteLength << '|'
      << static_cast<unsigned int>(mode) << '|' << grammarFingerprint.size() << ':'
      << grammarFingerprint;
   return value.str();
}

ParsedSourceCache::ParsedSourceCache(ParsedSourceCacheOptions options)
   : options_(options) {}

std::shared_ptr<const ParsedSourceCacheEntry> ParsedSourceCache::getOrBuild(
   const SourceBuffer &source,const SourceIdentity &identity,ParseMode mode,
   const std::string &grammarFingerprint,const Builder &builder)
{
   ParsedSourceKey key{source.uri,identity.contentRevision,identity.contentHash,
      source.text.size(),mode,grammarFingerprint};
   const std::string canonical = key.canonicalString();
   auto found = entries_.find(canonical);
   if (found != entries_.end() && found->second->sourceBytes == source.text) {
      found->second->lastUsedTick = ++tick_;
      if (mode == ParseMode::StrictCompiler) ++metrics_.strictHits;
      else ++metrics_.tolerantHits;
      return found->second;
   }

   SnapshotBundle bundle = builder(source,mode);
   auto entry = std::make_shared<ParsedSourceCacheEntry>();
   entry->key = std::move(key);
   entry->sourceBytes = source.text;
   entry->artifact = std::move(bundle.artifact);
   entry->syntax = std::move(bundle.syntax);
   entry->editor = std::move(bundle.editor);
   entry->retainedBytes = IncrementalMemoryEstimator::sourceBytes(entry->sourceBytes)
      + (entry->artifact == nullptr ? 0 : IncrementalMemoryEstimator::artifact(*entry->artifact))
      + IncrementalMemoryEstimator::syntax(entry->syntax)
      + IncrementalMemoryEstimator::editor(entry->editor);
   entry->lastUsedTick = ++tick_;

   auto old = entries_.find(canonical);
   if (old != entries_.end()) erase(old);
   if (mode == ParseMode::StrictCompiler) ++metrics_.strictBuilds;
   else ++metrics_.tolerantBuilds;

   if (entry->retainedBytes <= options_.maxRetainedBytes || options_.retainOversizedEntry) {
      retainedBytes_ += entry->retainedBytes;
      entries_.emplace(canonical,entry);
      evictUntilWithinBudget();
   }
   else {
      // The entry served the current request but was deliberately not
      // retained. Keep the legacy eviction counter meaningful for callers
      // that use a tiny budget as a pressure probe.
      ++metrics_.evictions;
   }
   metrics_.entries = entries_.size();
   metrics_.retainedBytes = retainedBytes_;
   return entry;
}

std::size_t ParsedSourceCache::erase(std::map<std::string,EntryPtr>::iterator iterator)
{
   if (iterator == entries_.end()) return 0;
   const std::size_t released = iterator->second->retainedBytes;
   retainedBytes_ -= std::min(retainedBytes_,released);
   entries_.erase(iterator);
   metrics_.entries = entries_.size();
   metrics_.retainedBytes = retainedBytes_;
   return released;
}

void ParsedSourceCache::evictUntilWithinBudget()
{
   while (!entries_.empty() && (entries_.size() > options_.maxEntries
      || retainedBytes_ > options_.maxRetainedBytes)) {
      auto victim = std::min_element(entries_.begin(),entries_.end(),
         [](const auto &left,const auto &right) {
            if (left.second->lastUsedTick != right.second->lastUsedTick)
               return left.second->lastUsedTick < right.second->lastUsedTick;
            return left.first < right.first;
         });
      erase(victim);
      ++metrics_.evictions;
   }
}

std::size_t ParsedSourceCache::invalidateUri(std::string_view uri)
{
   std::size_t released = 0;
   for (auto iterator = entries_.begin(); iterator != entries_.end();) {
      if (iterator->second->key.uri != uri) {
         ++iterator;
         continue;
      }
      auto current = iterator++;
      released += erase(current);
   }
   return released;
}

std::size_t ParsedSourceCache::clear()
{
   const std::size_t released = retainedBytes_;
   entries_.clear();
   retainedBytes_ = 0;
   metrics_.entries = 0;
   metrics_.retainedBytes = 0;
   return released;
}

void ParsedSourceCache::resetMetrics() noexcept
{
   const std::size_t entries = entries_.size();
   const std::size_t bytes = retainedBytes_;
   metrics_ = {};
   metrics_.entries = entries;
   metrics_.retainedBytes = bytes;
}

ParsedSourceCacheMetrics ParsedSourceCache::metrics() const noexcept
{
   ParsedSourceCacheMetrics result = metrics_;
   result.entries = entries_.size();
   result.retainedBytes = retainedBytes_;
   return result;
}

bool ParsedSourceCache::checkInvariants(std::string *error) const
{
   std::size_t sum = 0;
   for (const auto &entry : entries_) sum += entry.second->retainedBytes;
   if (sum == retainedBytes_ && entries_.size() == metrics_.entries) return true;
   if (error != nullptr) {
      std::ostringstream message;
      message << "parsed cache accounting mismatch: sum=" << sum
         << " retained=" << retainedBytes_ << " entries=" << entries_.size()
         << " metrics=" << metrics_.entries;
      *error = message.str();
   }
   return false;
}

} // namespace ilic::detail
