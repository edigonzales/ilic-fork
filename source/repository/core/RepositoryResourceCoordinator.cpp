#include "RepositoryResourceCoordinator.h"

#include <limits>

namespace ilic::repository::core {

CacheFreshness classifyCacheEntry(const repository::ports::RepositoryCacheEntry *entry,
   std::chrono::system_clock::time_point now,std::chrono::milliseconds ttl)
{
   if (entry == nullptr) return CacheFreshness::Missing;
   if (ttl.count() <= 0 || entry->storedAt > now) return CacheFreshness::Stale;
   const auto remaining = now - entry->storedAt;
   return remaining < ttl ? CacheFreshness::Fresh : CacheFreshness::Stale;
}

bool isTransientTransportFailure(const repository::ports::RepositoryTransportResponse &response)
{
   return response.retryable || response.statusCode == 408 || response.statusCode == 429
      || response.statusCode >= 500;
}

} // namespace ilic::repository::core
