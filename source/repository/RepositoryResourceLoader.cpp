#include "RepositoryResourceLoader.h"

#include "Md5.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>

namespace ilic::repository {
namespace {

std::string readFile(const std::filesystem::path &path,std::string *error)
{
   std::ifstream input(path,std::ios::binary);
   if (!input) {
      if (error != nullptr) *error = "unable to open " + path.string();
      return {};
   }
   std::string value((std::istreambuf_iterator<char>(input)),std::istreambuf_iterator<char>());
   if (input.bad() && error != nullptr) *error = "unable to read " + path.string();
   return value;
}

std::string lower(std::string value)
{
   std::transform(value.begin(),value.end(),value.begin(),[](unsigned char character) {
      return static_cast<char>(std::tolower(character));
   });
   return value;
}

} // namespace

RepositoryResourceLoader::RepositoryResourceLoader(const RepositoryOptions &options,
   RepositoryTransport &transport,RepositoryCache &cache)
   : options_(options),transport_(transport),cache_(cache)
{
}

RepositoryResource RepositoryResourceLoader::download(const RepositoryUri &uri,bool optional)
{
   RepositoryResource resource;
   resource.uri = uri.normalized();
   const auto response = transport_.get(resource.uri);
   if (!response.success) {
      if (!optional) resource.error = response.error.empty() ? "request failed" : response.error;
      return resource;
   }
   resource.success = true;
   resource.content = response.body;
   auto stored = cache_.store(uri.cacheKey(),resource.content);
   if (stored.success) resource.localPath = stored.path;
   else resource.warnings.push_back(stored.error);
   return resource;
}

RepositoryResource RepositoryResourceLoader::load(const RepositoryUri &uri,
   const FetchOptions &fetchOptions)
{
   RepositoryResource resource;
   resource.uri = uri.normalized();
   if (uri.isLocal()) {
      resource.localPath = uri.toLocalPath();
      resource.content = readFile(resource.localPath,&resource.error);
      resource.success = resource.error.empty();
      if (!resource.success && fetchOptions.optional) resource.error.clear();
      return resource;
   }

   const auto cached = cache_.lookup(uri.cacheKey(),fetchOptions.ttl);
   if (cached.exists && (cached.fresh || options_.offline)) {
      resource.success = true;
      resource.fromCache = true;
      resource.stale = !cached.fresh;
      resource.content = cached.content;
      resource.localPath = cached.path;
      return resource;
   }
   if (options_.offline) {
      if (!fetchOptions.optional) resource.error = "offline and no cached copy of " + resource.uri;
      return resource;
   }
   resource = download(uri,fetchOptions.optional);
   if (!resource.success && cached.exists && options_.allowStaleOnError) {
      const std::string downloadError = resource.error;
      resource.success = true;
      resource.fromCache = true;
      resource.stale = true;
      resource.content = cached.content;
      resource.localPath = cached.path;
      resource.error.clear();
      resource.warnings.push_back("using stale cache entry for " + uri.normalized()
         + " after download error: " + downloadError);
   }
   return resource;
}

bool RepositoryResourceLoader::materializeFallback(RepositoryResource &resource)
{
   if (!resource.localPath.empty() && std::filesystem::is_regular_file(resource.localPath)) return true;
   auto stored = temporaryModels_.materialize(resource.uri,resource.content);
   if (!stored.success) {
      resource.error = stored.error;
      resource.success = false;
      return false;
   }
   resource.localPath = stored.path;
   return true;
}

RepositoryResource RepositoryResourceLoader::loadModel(const RepositoryUri &uri,
   std::string_view expectedMd5)
{
   RepositoryResource resource = load(uri,{options_.modelTtl,false});
   if (!resource.success) return resource;
   if (options_.validateChecksums && !expectedMd5.empty()) {
      std::string actual = md5(resource.content);
      if (lower(actual) != lower(std::string(expectedMd5))) {
         if (resource.fromCache && !options_.offline && uri.isRemote()) {
            cache_.invalidate(uri.cacheKey());
            auto retry = download(uri,false);
            retry.warnings.insert(retry.warnings.begin(),resource.warnings.begin(),resource.warnings.end());
            if (retry.success) {
               actual = md5(retry.content);
               resource = std::move(retry);
            }
         }
         if (!resource.success || lower(actual) != lower(std::string(expectedMd5))) {
            resource.success = false;
            resource.error = "MD5 mismatch for " + uri.normalized() + ": expected "
               + std::string(expectedMd5) + ", actual " + actual;
            return resource;
         }
      }
   }
   materializeFallback(resource);
   return resource;
}

} // namespace ilic::repository
