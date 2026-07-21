#include "RepositoryCache.h"

#include "Md5.h"

#include <atomic>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <random>
#include <system_error>

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace ilic::repository {
namespace {

std::uint64_t processId()
{
#ifdef _WIN32
   return static_cast<std::uint64_t>(_getpid());
#else
   return static_cast<std::uint64_t>(getpid());
#endif
}

std::string uniqueSuffix()
{
   static std::atomic<std::uint64_t> sequence{0};
   static thread_local std::mt19937_64 random(std::random_device{}());
   return std::to_string(processId()) + '.' + std::to_string(random()) + '.'
      + std::to_string(sequence.fetch_add(1,std::memory_order_relaxed));
}

std::string readFile(const std::filesystem::path &path,std::string *error)
{
   std::ifstream input(path,std::ios::binary);
   if (!input) {
      if (error != nullptr) *error = "unable to open " + path.string();
      return {};
   }
   std::string content((std::istreambuf_iterator<char>(input)),std::istreambuf_iterator<char>());
   if (input.bad() && error != nullptr) *error = "unable to read " + path.string();
   return content;
}

std::string extensionFor(std::string_view uri)
{
   const std::size_t suffix = uri.find_first_of("?#");
   const std::string path(uri.substr(0,suffix));
   const std::string extension = std::filesystem::path(path).extension().string();
   if (extension.size() > 16 || extension.find_first_of("/\\") != std::string::npos) return {};
   return extension;
}

CacheStoreResult writeAtomically(const std::filesystem::path &target,
   const std::filesystem::path &temporary,std::string_view content)
{
   CacheStoreResult result;
   result.path = target;
   std::error_code filesystemError;
   std::filesystem::create_directories(target.parent_path(),filesystemError);
   if (filesystemError) {
      result.error = "unable to create cache directory " + target.parent_path().string()
         + ": " + filesystemError.message();
      return result;
   }
   {
      std::ofstream output(temporary,std::ios::binary | std::ios::trunc);
      if (!output) {
         result.error = "unable to open temporary cache file " + temporary.string();
         return result;
      }
      output.write(content.data(),static_cast<std::streamsize>(content.size()));
      if (!output) {
         result.error = "unable to write temporary cache file " + temporary.string();
         output.close();
         std::filesystem::remove(temporary,filesystemError);
         return result;
      }
      output.flush();
      if (!output) {
         result.error = "unable to flush temporary cache file " + temporary.string();
         output.close();
         std::filesystem::remove(temporary,filesystemError);
         return result;
      }
      output.close();
      if (output.fail()) {
         result.error = "unable to close temporary cache file " + temporary.string();
         std::filesystem::remove(temporary,filesystemError);
         return result;
      }
   }
#ifdef _WIN32
   bool published = false;
   DWORD publishError = ERROR_SUCCESS;
   for (int attempt = 0; attempt < 32 && !published; ++attempt) {
      if (std::filesystem::exists(target)) {
         published = ReplaceFileW(target.c_str(),temporary.c_str(),nullptr,
            REPLACEFILE_WRITE_THROUGH,nullptr,nullptr) != 0;
      }
      else {
         published = MoveFileExW(temporary.c_str(),target.c_str(),MOVEFILE_WRITE_THROUGH) != 0;
      }
      if (published) break;
      publishError = GetLastError();
      // Another process can publish between exists() and MoveFileExW(), or
      // briefly hold the file while ReplaceFileW() completes.
      if (publishError != ERROR_ALREADY_EXISTS && publishError != ERROR_FILE_EXISTS
         && publishError != ERROR_SHARING_VIOLATION && publishError != ERROR_ACCESS_DENIED)
         break;
      SwitchToThread();
   }
   if (!published) result.error = "unable to publish cache file " + target.string()
      + ": Windows error " + std::to_string(publishError);
#else
   std::filesystem::rename(temporary,target,filesystemError);
   if (filesystemError) result.error = "unable to publish cache file " + target.string()
      + ": " + filesystemError.message();
#endif
   if (!result.error.empty()) {
      std::filesystem::remove(temporary,filesystemError);
      return result;
   }
   result.success = true;
   return result;
}

} // namespace

std::filesystem::path defaultCacheDirectory()
{
   if (const char *configured = std::getenv("ILIC_CACHE"))
      if (*configured != '\0') return std::filesystem::path(configured);
   if (const char *legacy = std::getenv("ILI_CACHE"))
      if (*legacy != '\0') return std::filesystem::path(legacy) / "ilic-v1";
   if (const char *home = std::getenv("HOME"))
      if (*home != '\0') return std::filesystem::path(home) / ".ilicache" / "ilic-v1";
#ifdef _WIN32
   if (const char *profile = std::getenv("USERPROFILE"))
      if (*profile != '\0') return std::filesystem::path(profile) / ".ilicache" / "ilic-v1";
#endif
   return std::filesystem::temp_directory_path() / "ilic-cache-v1";
}

RepositoryCache::RepositoryCache(std::filesystem::path root) : root_(std::move(root)) {}

std::filesystem::path RepositoryCache::pathFor(std::string_view uri) const
{
   return root_ / (md5(std::string(uri)) + extensionFor(uri));
}

std::filesystem::path RepositoryCache::makeTemporaryPath(
   const std::filesystem::path &target) const
{
   return std::filesystem::path(target.string() + '.' + uniqueSuffix() + ".tmp");
}

CacheLookupResult RepositoryCache::lookup(std::string_view uri,std::chrono::seconds ttl) const
{
   CacheLookupResult result;
   result.path = pathFor(uri);
   std::error_code error;
   result.exists = std::filesystem::is_regular_file(result.path,error);
   if (!result.exists || error) return result;
   result.content = readFile(result.path,&result.error);
   if (!result.error.empty()) {
      result.exists = false;
      return result;
   }
   const auto written = std::filesystem::last_write_time(result.path,error);
   if (!error) result.fresh = std::filesystem::file_time_type::clock::now() - written <= ttl;
   return result;
}

CacheStoreResult RepositoryCache::store(std::string_view uri,std::string_view content)
{
   const std::filesystem::path target = pathFor(uri);
   return writeAtomically(target,makeTemporaryPath(target),content);
}

void RepositoryCache::invalidate(std::string_view uri)
{
   std::error_code error;
   std::filesystem::remove(pathFor(uri),error);
}

TemporaryModelStore::TemporaryModelStore()
{
   root_ = std::filesystem::temp_directory_path()
      / ("ilic-" + uniqueSuffix());
}

TemporaryModelStore::~TemporaryModelStore()
{
   std::error_code error;
   std::filesystem::remove_all(root_,error);
}

CacheStoreResult TemporaryModelStore::materialize(
   std::string_view suggestedName,std::string_view content)
{
   const std::string extension = extensionFor(suggestedName);
   const std::filesystem::path target = root_ / (md5(std::string(suggestedName)) + extension);
   const std::filesystem::path temporary(target.string() + '.' + uniqueSuffix() + ".tmp");
   return writeAtomically(target,temporary,content);
}

} // namespace ilic::repository
