#include "Md5.h"
#include "RepositoryCache.h"
#include "RepositoryResourceLoader.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <thread>
#include <vector>

namespace {

class FakeTransport final : public ilic::repository::RepositoryTransport {
public:
   std::map<std::string,ilic::repository::TransportResponse> responses;
   std::map<std::string,int> requests;
   ilic::repository::TransportResponse get(const std::string &uri) override
   {
      ++requests[uri];
      auto found = responses.find(uri);
      return found == responses.end()
         ? ilic::repository::TransportResponse{false,404,{},"not found"} : found->second;
   }
};

std::filesystem::path temporaryDirectory()
{
   const auto path = std::filesystem::temp_directory_path()
      / ("ilic-cache-test-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
   std::filesystem::create_directories(path);
   return path;
}

}

int main()
{
   const auto root = temporaryDirectory();
   const std::string uri = "https://example.invalid/Model.ili?token=x";
   ilic::repository::RepositoryCache cache(root / "cache");
   auto stored = cache.store(uri,"complete-content");
   assert(stored.success && stored.path.extension() == ".ili");
   auto cached = cache.lookup(uri,std::chrono::hours(1));
   assert(cached.exists && cached.fresh && cached.content == "complete-content");

   std::vector<std::thread> writers;
   for (int index = 0; index < 8; ++index) writers.emplace_back([&,index] {
      const std::string content(10000,static_cast<char>('A' + index));
      assert(cache.store(uri,content).success);
   });
   for (auto &writer : writers) writer.join();
   cached = cache.lookup(uri,std::chrono::hours(1));
   assert(cached.content.size() == 10000);
   for (const char character : cached.content) assert(character == cached.content.front());
   for (const auto &entry : std::filesystem::directory_iterator(cache.root()))
      assert(entry.path().extension() != ".tmp");

   ilic::RepositoryOptions options;
   options.cacheDirectory = root / "loader-cache";
   ilic::repository::RepositoryCache loaderCache(options.cacheDirectory);
   FakeTransport transport;
   transport.responses[uri] = {true,200,"correct",{}};
   ilic::repository::RepositoryResourceLoader loader(options,transport,loaderCache);
   assert(loaderCache.store(uri,"corrupt").success);
   auto model = loader.loadModel(*ilic::repository::RepositoryUri::parse(uri),
      ilic::repository::md5("correct"));
   assert(model.success && model.content == "correct" && transport.requests[uri] == 1);
   assert(std::filesystem::is_regular_file(model.localPath));
   auto warm = loader.loadModel(*ilic::repository::RepositoryUri::parse(uri),
      ilic::repository::md5("correct"));
   assert(warm.success && warm.fromCache && transport.requests[uri] == 1);

   for (const auto &[name,response] : std::vector<std::pair<std::string,
      ilic::repository::TransportResponse>>{
         {"not-found",{false,404,{},"not found (HTTP 404)"}},
         {"server-error",{false,500,{},"server error (HTTP 500)"}},
         {"timeout",{false,0,{},"operation timed out"}}}) {
      const std::string failureUri = "https://example.invalid/" + name;
      transport.responses[failureUri] = response;
      auto failed = loader.load(*ilic::repository::RepositoryUri::parse(failureUri),
         {std::chrono::seconds(0),false});
      assert(!failed.success && !failed.error.empty() && transport.requests[failureUri] == 1);
   }
   const std::string redirectedUri = "https://example.invalid/redirected";
   transport.responses[redirectedUri] = {true,200,"final response",{}};
   auto redirected = loader.load(*ilic::repository::RepositoryUri::parse(redirectedUri),
      {std::chrono::hours(1),false});
   assert(redirected.success && redirected.content == "final response");
   auto redirectedWarm = loader.load(*ilic::repository::RepositoryUri::parse(redirectedUri),
      {std::chrono::hours(1),false});
   assert(redirectedWarm.fromCache && transport.requests[redirectedUri] == 1);

   const std::string staleUri = "https://example.invalid/stale";
   assert(loaderCache.store(staleUri,"stale content").success);
   transport.responses[staleUri] = {false,500,{},"server error"};
   auto stale = loader.load(*ilic::repository::RepositoryUri::parse(staleUri),
      {std::chrono::seconds(-1),false});
   assert(stale.success && stale.fromCache && stale.stale && !stale.warnings.empty());

   ilic::RepositoryOptions offline = options;
   offline.offline = true;
   assert(loaderCache.store(uri,"broken-again").success);
   ilic::repository::RepositoryResourceLoader offlineLoader(offline,transport,loaderCache);
   auto broken = offlineLoader.loadModel(*ilic::repository::RepositoryUri::parse(uri),
      ilic::repository::md5("correct"));
   assert(!broken.success && broken.error.find("expected") != std::string::npos);

   const auto blocker = root / "not-a-directory";
   { std::ofstream output(blocker); output << "block"; }
   ilic::RepositoryOptions fallbackOptions;
   fallbackOptions.cacheDirectory = blocker / "cache";
   ilic::repository::RepositoryCache unwritable(fallbackOptions.cacheDirectory);
   FakeTransport fallbackTransport;
   const std::string fallbackUri = "https://example.invalid/Fallback.ili";
   fallbackTransport.responses[fallbackUri] = {true,200,"fallback",{}};
   ilic::repository::RepositoryResourceLoader fallbackLoader(
      fallbackOptions,fallbackTransport,unwritable);
   auto fallback = fallbackLoader.loadModel(
      *ilic::repository::RepositoryUri::parse(fallbackUri),ilic::repository::md5("fallback"));
   assert(fallback.success && !fallback.warnings.empty());
   assert(std::filesystem::is_regular_file(fallback.localPath));

   std::error_code error;
   std::filesystem::remove_all(root,error);
   return 0;
}
