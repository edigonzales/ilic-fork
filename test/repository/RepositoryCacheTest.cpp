#include "Md5.h"
#include "RepositoryCache.h"
#include "RepositoryResourceLoader.h"

#include "ilic/test/TestHarness.h"
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

ilic::repository::RepositoryUri parseUri(const std::string &uri)
{
   auto parsed = ilic::repository::RepositoryUri::parse(uri);
   ILIC_REQUIRE_MSG(parsed.has_value(),"test URI must be valid");
   return *parsed;
}

}

int main()
{
   const auto root = temporaryDirectory();
   const std::string uri = "https://example.invalid/Model.ili?token=x";
   ilic::repository::RepositoryCache cache(root / "cache");
   auto stored = cache.store(uri,"complete-content");
   ILIC_REQUIRE(stored.success);
   ILIC_REQUIRE(stored.path.extension() == ".ili");
   auto cached = cache.lookup(uri,std::chrono::hours(1));
   ILIC_REQUIRE(cached.exists);
   ILIC_REQUIRE(cached.fresh);
   ILIC_REQUIRE(cached.content == "complete-content");

   std::vector<std::thread> writers;
   for (int index = 0; index < 8; ++index) writers.emplace_back([&,index] {
      const std::string content(10000,static_cast<char>('A' + index));
      const auto result = cache.store(uri,content);
      ILIC_REQUIRE(result.success);
   });
   for (auto &writer : writers) writer.join();
   cached = cache.lookup(uri,std::chrono::hours(1));
   ILIC_REQUIRE(cached.content.size() == 10000);
   ILIC_REQUIRE(!cached.content.empty());
   for (const char character : cached.content) {
      ILIC_REQUIRE(character == cached.content.front());
   }
   for (const auto &entry : std::filesystem::directory_iterator(cache.root()))
      ILIC_REQUIRE(entry.path().extension() != ".tmp");

   ilic::RepositoryOptions options;
   options.cacheDirectory = root / "loader-cache";
   ilic::repository::RepositoryCache loaderCache(options.cacheDirectory);
   FakeTransport transport;
   transport.responses[uri] = {true,200,"correct",{}};
   ilic::repository::RepositoryResourceLoader loader(options,transport,loaderCache);
   const auto corrupt = loaderCache.store(uri,"corrupt");
   ILIC_REQUIRE(corrupt.success);
   auto model = loader.loadModel(parseUri(uri),
      ilic::repository::md5("correct"));
   ILIC_REQUIRE(model.success);
   ILIC_REQUIRE(model.content == "correct");
   ILIC_REQUIRE(transport.requests[uri] == 1);
   ILIC_REQUIRE(std::filesystem::is_regular_file(model.localPath));
   auto warm = loader.loadModel(parseUri(uri),
      ilic::repository::md5("correct"));
   ILIC_REQUIRE(warm.success);
   ILIC_REQUIRE(warm.fromCache);
   ILIC_REQUIRE(transport.requests[uri] == 1);

   for (const auto &[name,response] : std::vector<std::pair<std::string,
      ilic::repository::TransportResponse>>{
         {"not-found",{false,404,{},"not found (HTTP 404)"}},
         {"server-error",{false,500,{},"server error (HTTP 500)"}},
         {"timeout",{false,0,{},"operation timed out"}}}) {
      const std::string failureUri = "https://example.invalid/" + name;
      transport.responses[failureUri] = response;
      auto failed = loader.load(parseUri(failureUri),
         {std::chrono::seconds(0),false});
      ILIC_REQUIRE(!failed.success);
      ILIC_REQUIRE(!failed.error.empty());
      ILIC_REQUIRE(transport.requests[failureUri] == 1);
   }
   const std::string redirectedUri = "https://example.invalid/redirected";
   transport.responses[redirectedUri] = {true,200,"final response",{}};
   auto redirected = loader.load(parseUri(redirectedUri),
      {std::chrono::hours(1),false});
   ILIC_REQUIRE(redirected.success);
   ILIC_REQUIRE(redirected.content == "final response");
   auto redirectedWarm = loader.load(parseUri(redirectedUri),
      {std::chrono::hours(1),false});
   ILIC_REQUIRE(redirectedWarm.fromCache);
   ILIC_REQUIRE(transport.requests[redirectedUri] == 1);

   const std::string staleUri = "https://example.invalid/stale";
   const auto staleStore = loaderCache.store(staleUri,"stale content");
   ILIC_REQUIRE(staleStore.success);
   transport.responses[staleUri] = {false,500,{},"server error"};
   auto stale = loader.load(parseUri(staleUri),
      {std::chrono::seconds(-1),false});
   ILIC_REQUIRE(stale.success);
   ILIC_REQUIRE(stale.fromCache);
   ILIC_REQUIRE(stale.stale);
   ILIC_REQUIRE(!stale.warnings.empty());

   ilic::RepositoryOptions offline = options;
   offline.offline = true;
   const auto brokenStore = loaderCache.store(uri,"broken-again");
   ILIC_REQUIRE(brokenStore.success);
   ilic::repository::RepositoryResourceLoader offlineLoader(offline,transport,loaderCache);
   auto broken = offlineLoader.loadModel(parseUri(uri),
      ilic::repository::md5("correct"));
   ILIC_REQUIRE(!broken.success);
   ILIC_REQUIRE(broken.error.find("expected") != std::string::npos);

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
      parseUri(fallbackUri),ilic::repository::md5("fallback"));
   ILIC_REQUIRE(fallback.success);
   ILIC_REQUIRE(!fallback.warnings.empty());
   ILIC_REQUIRE(std::filesystem::is_regular_file(fallback.localPath));

   std::error_code error;
   std::filesystem::remove_all(root,error);
   return 0;
}
