#pragma once

#include "ilic/Repository.h"

#include <cstddef>
#include <vector>

namespace ilic::repository::core {

struct RepositoryPolicy {
   std::chrono::milliseconds metadataTtl{24 * 60 * 60 * 1000};
   std::chrono::milliseconds modelTtl{7 * 24 * 60 * 60 * 1000};
   bool offline = false;
   bool allowStaleOnError = true;
   bool followSiteLinks = true;
   bool validateChecksums = true;
   std::size_t maxMetadataBytes = 32 * 1024 * 1024;
   std::size_t maxModelBytes = 64 * 1024 * 1024;
   std::size_t maxRepositoriesVisited = 4096;
   std::size_t maxDependencyDepth = 1024;
   std::size_t maxModelsResolved = 10000;
   std::size_t maxRedirects = 10;
};

RepositoryPolicy normalizeRepositoryPolicy(const RepositoryOptions &options,
   std::vector<Diagnostic> &diagnostics);

} // namespace ilic::repository::core
