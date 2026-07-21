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
      RepositoryTransport &transport,RepositoryCache &cache);

   RepositoryResource load(const RepositoryUri &uri,const FetchOptions &options);
   RepositoryResource loadModel(const RepositoryUri &uri,std::string_view expectedMd5);

private:
   RepositoryResource download(const RepositoryUri &uri,bool optional);
   bool materializeFallback(RepositoryResource &resource);

   const RepositoryOptions &options_;
   RepositoryTransport &transport_;
   RepositoryCache &cache_;
   TemporaryModelStore temporaryModels_;
};

} // namespace ilic::repository
