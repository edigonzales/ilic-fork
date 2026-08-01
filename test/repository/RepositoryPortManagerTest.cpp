#include "ilic/Repository.h"

#include "ilic/test/TestHarness.h"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <map>
#include <memory>
#include <string>

namespace {

class FakeTransport final : public ilic::repository::ports::RepositoryTransportPort {
public:
   std::map<std::string,std::string> bodies;

   ilic::repository::ports::RepositoryTransportResponse get(
      const ilic::repository::ports::RepositoryTransportRequest &request) override
   {
      ilic::repository::ports::RepositoryTransportResponse response;
      response.finalUri = request.uri;
      const auto found = bodies.find(request.uri);
      if (found == bodies.end()) {
         response.statusCode = 404;
         response.notFound = true;
         response.error = "not found";
         return response;
      }
      response.success = true;
      response.statusCode = 200;
      response.body = found->second;
      return response;
   }
};

class FakeCache final : public ilic::repository::ports::RepositoryCachePort {
public:
   std::map<std::string,ilic::repository::ports::RepositoryCacheEntry> entries;

   ilic::repository::ports::RepositoryCacheLookup get(std::string_view key) override
   {
      const auto found = entries.find(std::string(key));
      if (found == entries.end()) return {};
      return {true,found->second,{}};
   }

   ilic::repository::ports::RepositoryCacheStoreResult put(std::string_view key,
      std::string_view value,std::chrono::system_clock::time_point storedAt) override
   {
      entries[std::string(key)] = {std::string(value),storedAt};
      return {true,{}, {}};
   }

   void remove(std::string_view key) override { entries.erase(std::string(key)); }
   void clear() override { entries.clear(); }
};

class FakeDecoder final : public ilic::repository::ports::RepositoryMetadataDecoder {
public:
   bool decodeModelIndex(std::string_view,std::string_view repository,
      std::vector<ilic::ModelMetadata> &models,std::vector<ilic::Diagnostic> &) override
   {
      ilic::ModelMetadata model;
      model.name = "PortModel";
      model.schemaLanguage = "ili2_4";
      model.file = "PortModel.ili";
      model.version = "1";
      model.repository = std::string(repository);
      models.push_back(std::move(model));
      return true;
   }

   bool decodeSite(std::string_view,std::string_view,
      std::vector<std::string> &,std::vector<std::string> &,
      std::vector<ilic::Diagnostic> &) override { return true; }
};

class FakeChecksum final : public ilic::repository::ports::RepositoryChecksum {
public:
   std::string md5(std::string_view) const override { return "injected-checksum"; }
};

}

int main()
{
   const std::string root = "https://ports.invalid/repository";
   const std::string modelUri = root + "/PortModel.ili";
   auto transport = std::make_unique<FakeTransport>();
   transport->bodies[root + "/ilimodels.xml"] =
      "<TRANSFER><DATASECTION><IliRepository20.RepositoryIndex.ModelMetadata>"
      "<Name>PortModel</Name><File>PortModel.ili</File><SchemaLanguage>ili2_4</SchemaLanguage>"
      "<Version>1</Version></IliRepository20.RepositoryIndex.ModelMetadata>"
      "</DATASECTION></TRANSFER>";
   transport->bodies[modelUri] = "INTERLIS 2.4; MODEL PortModel = END PortModel.";

   const auto cacheRoot = std::filesystem::temp_directory_path() / "ilic-port-manager-test-cache";
   const auto blocker = cacheRoot / "not-a-directory";
   std::error_code ignored;
   std::filesystem::remove_all(cacheRoot,ignored);
   std::filesystem::create_directories(cacheRoot);
   { std::ofstream output(blocker); output << "cache blocker"; }
   ilic::RepositoryOptions options;
   options.repositories = {root};
   options.cacheDirectory = blocker / "cache";
   options.followSiteLinks = false;
   ilic::RepositoryPorts ports;
   ports.transport = std::move(transport);
   ports.cache = std::make_unique<FakeCache>();
   ports.clock = std::make_unique<ilic::repository::ports::ManualRepositoryClock>(
      std::chrono::system_clock::time_point(std::chrono::milliseconds(1000)));
   ports.metadataDecoder = std::make_unique<FakeDecoder>();
   ports.checksum = std::make_unique<FakeChecksum>();
   ilic::RepositoryManager manager(std::move(options),std::move(ports));
   const auto result = manager.resolve("PortModel","ili2_4");
   ILIC_REQUIRE(result.success);
   ILIC_REQUIRE(result.models.size() == 1);
   ILIC_REQUIRE(result.models.front().uri == modelUri);
   ILIC_REQUIRE(result.models.front().source.find("MODEL PortModel") != std::string::npos);
   ILIC_REQUIRE(result.models.front().localPath.empty());
   std::filesystem::remove_all(cacheRoot,ignored);
   return 0;
}
