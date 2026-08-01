#pragma once

#include "ilic/RepositoryContract.h"

#include <chrono>

namespace ilic::repository::core {

enum class CacheFreshness {
   Missing,
   Fresh,
   Stale
};

CacheFreshness classifyCacheEntry(const repository::ports::RepositoryCacheEntry *entry,
   std::chrono::system_clock::time_point now,std::chrono::milliseconds ttl);

bool isTransientTransportFailure(const repository::ports::RepositoryTransportResponse &response);

} // namespace ilic::repository::core
