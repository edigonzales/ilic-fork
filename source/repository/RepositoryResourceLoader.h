#pragma once

#include "RepositoryCache.h"
#include "RepositoryTransport.h"
#include "RepositoryUri.h"
#include "ilic/Repository.h"

#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace ilic::repository {

struct RepositoryResource {
   bool success = false;
   bool fromCache = false;
   bool stale = false;
   std::string uri;
   std::string content;
   std::filesystem::path localPath;
   std::string error;
   std::vector<std::string> warnings;
};

struct FetchOptions {
   std::chrono::seconds ttl{};
   bool optional = false;
};

class RepositoryResourceLoader {
public:
   RepositoryResourceLoader(const RepositoryOptions &options,
      RepositoryTransport &transport,RepositoryCache &cache,
      const repository::ports::RepositoryChecksum *checksum = nullptr);

   RepositoryResource load(const RepositoryUri &uri,const FetchOptions &options,
      RepositoryResourceKind kind = RepositoryResourceKind::ModelIndex);
   RepositoryResource loadModel(const RepositoryUri &uri,std::string_view expectedMd5,
      bool materialize = true);

private:
   RepositoryResource download(const RepositoryUri &uri,bool optional,RepositoryResourceKind kind);
   bool materializeFallback(RepositoryResource &resource);
   std::string checksum(std::string_view content) const;

   const RepositoryOptions &options_;
   RepositoryTransport &transport_;
   RepositoryCache &cache_;
   const repository::ports::RepositoryChecksum *checksum_ = nullptr;
   TemporaryModelStore temporaryModels_;
};

} // namespace ilic::repository
