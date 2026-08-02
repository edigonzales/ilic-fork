#include "RootAnalysisCache.h"

#include "IncrementalMemoryEstimator.h"

#include <algorithm>
#include <sstream>

namespace ilic::detail {

RootAnalysisCache::RootAnalysisCache(RootAnalysisCacheOptions options)
   : options_(options) {}

std::shared_ptr<const RootAnalysisCacheEntry> RootAnalysisCache::findExact(
   const std::string &baseKey,const SourceManager &sources)
{
   for (auto &pair : entries_) {
      auto &entry = pair.second;
      if (entry->baseKey != baseKey) continue;
      if (entry->hadMissingModels && entry->identities.size() != sources.uris().size()) continue;
      bool current = true;
      for (std::size_t index = 0; index < entry->closureUris.size(); ++index) {
         const auto *identity = sources.identity(entry->closureUris[index]);
         if (identity == nullptr || index >= entry->identities.size()
            || identity->contentRevision != entry->identities[index].contentRevision
            || identity->contentHash != entry->identities[index].contentHash
            || identity->byteLength != entry->identities[index].byteLength) {
            current = false;
            break;
         }
      }
      if (!current) continue;
      entry->lastUsedTick = ++tick_;
      ++metrics_.hits;
      return entry;
   }
   ++metrics_.misses;
   return nullptr;
}

void RootAnalysisCache::insert(RootAnalysisCacheEntry entry)
{
   entry.retainedBytes = sizeof(entry) + entry.baseKey.capacity() + entry.key.capacity()
      + entry.closureUris.capacity() * sizeof(std::string)
      + entry.identities.capacity() * sizeof(SourceIdentity)
      + entry.resolvedModels.capacity() * sizeof(std::string)
      + entry.missingModels.capacity() * sizeof(std::string)
      + IncrementalMemoryEstimator::compilation(entry.result);
   for (const auto &uri : entry.closureUris) entry.retainedBytes += uri.capacity();
   for (const auto &identity : entry.identities)
      entry.retainedBytes += identity.uri.capacity() + identity.contentHash.capacity();
   for (const auto &model : entry.resolvedModels) entry.retainedBytes += model.capacity();
   for (const auto &model : entry.missingModels) entry.retainedBytes += model.capacity();
   entry.lastUsedTick = ++tick_;

   auto old = entries_.find(entry.key);
   if (old != entries_.end()) erase(old);
   auto stored = std::make_shared<RootAnalysisCacheEntry>(std::move(entry));
   retainedBytes_ += stored->retainedBytes;
   entries_.emplace(stored->key,stored);
   evictUntilWithinBudget();
   metrics_.entries = entries_.size();
   metrics_.retainedBytes = retainedBytes_;
}

std::size_t RootAnalysisCache::erase(std::map<std::string,EntryPtr>::iterator iterator,
   RootInvalidationResult *result)
{
   if (iterator == entries_.end()) return 0;
   const auto &entry = iterator->second;
   const std::size_t released = entry->retainedBytes;
   if (result != nullptr) {
      result->releasedBytes += released;
      result->invalidatedKeyIds.push_back(entry->key);
      result->invalidatedRoots.insert(result->invalidatedRoots.end(),
         entry->closureUris.begin(),entry->closureUris.end());
   }
   retainedBytes_ -= std::min(retainedBytes_,released);
   entries_.erase(iterator);
   metrics_.entries = entries_.size();
   metrics_.retainedBytes = retainedBytes_;
   return released;
}

void RootAnalysisCache::evictUntilWithinBudget()
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

RootInvalidationResult RootAnalysisCache::invalidateBySource(std::string_view uri)
{
   return invalidateIf([&](const RootAnalysisCacheEntry &entry) {
      return std::find(entry.closureUris.begin(),entry.closureUris.end(),uri)
         != entry.closureUris.end();
   });
}

RootInvalidationResult RootAnalysisCache::invalidateIf(
   const std::function<bool(const RootAnalysisCacheEntry &)> &predicate)
{
   RootInvalidationResult result;
   for (auto iterator = entries_.begin(); iterator != entries_.end();) {
      if (!predicate(*iterator->second)) {
         ++iterator;
         continue;
      }
      auto current = iterator++;
      erase(current,&result);
   }
   std::sort(result.invalidatedKeyIds.begin(),result.invalidatedKeyIds.end());
   result.invalidatedKeyIds.erase(std::unique(result.invalidatedKeyIds.begin(),
      result.invalidatedKeyIds.end()),result.invalidatedKeyIds.end());
   std::sort(result.invalidatedRoots.begin(),result.invalidatedRoots.end());
   result.invalidatedRoots.erase(std::unique(result.invalidatedRoots.begin(),
      result.invalidatedRoots.end()),result.invalidatedRoots.end());
   return result;
}

RootInvalidationResult RootAnalysisCache::clear()
{
   RootInvalidationResult result;
   for (const auto &entry : entries_) {
      result.releasedBytes += entry.second->retainedBytes;
      result.invalidatedKeyIds.push_back(entry.second->key);
   }
   entries_.clear();
   retainedBytes_ = 0;
   metrics_.entries = 0;
   metrics_.retainedBytes = 0;
   return result;
}

void RootAnalysisCache::resetMetrics() noexcept
{
   const std::size_t entries = entries_.size();
   const std::size_t bytes = retainedBytes_;
   metrics_ = {};
   metrics_.entries = entries;
   metrics_.retainedBytes = bytes;
}

RootAnalysisCacheMetrics RootAnalysisCache::metrics() const noexcept
{
   RootAnalysisCacheMetrics result = metrics_;
   result.entries = entries_.size();
   result.retainedBytes = retainedBytes_;
   return result;
}

bool RootAnalysisCache::checkInvariants(std::string *error) const
{
   std::size_t sum = 0;
   for (const auto &entry : entries_) sum += entry.second->retainedBytes;
   if (sum == retainedBytes_ && entries_.size() == metrics_.entries) return true;
   if (error != nullptr) {
      std::ostringstream message;
      message << "root cache accounting mismatch: sum=" << sum
         << " retained=" << retainedBytes_ << " entries=" << entries_.size()
         << " metrics=" << metrics_.entries;
      *error = message.str();
   }
   return false;
}

} // namespace ilic::detail
