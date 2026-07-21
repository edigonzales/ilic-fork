#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>

namespace ilic::repository {

struct CacheLookupResult {
   bool exists = false;
   bool fresh = false;
   std::string content;
   std::filesystem::path path;
   std::string error;
};

struct CacheStoreResult {
   bool success = false;
   std::filesystem::path path;
   std::string error;
};

std::filesystem::path defaultCacheDirectory();

class RepositoryCache {
public:
   explicit RepositoryCache(std::filesystem::path root);

   const std::filesystem::path &root() const { return root_; }
   CacheLookupResult lookup(std::string_view uri,std::chrono::seconds ttl) const;
   CacheStoreResult store(std::string_view uri,std::string_view content);
   void invalidate(std::string_view uri);

private:
   std::filesystem::path pathFor(std::string_view uri) const;
   std::filesystem::path makeTemporaryPath(const std::filesystem::path &target) const;
   std::filesystem::path root_;
};

class TemporaryModelStore {
public:
   TemporaryModelStore();
   ~TemporaryModelStore();

   TemporaryModelStore(const TemporaryModelStore &) = delete;
   TemporaryModelStore &operator=(const TemporaryModelStore &) = delete;

   CacheStoreResult materialize(std::string_view suggestedName,std::string_view content);

private:
   std::filesystem::path root_;
};

} // namespace ilic::repository
