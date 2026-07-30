#include "RepositoryCli.h"

#include <algorithm>
#include "ilic/test/TestHarness.h"
#include <string>

namespace {

util::StringMap arguments(std::initializer_list<std::pair<std::string,std::string>> values)
{
   util::StringMap result;
   for (const auto &[key,value] : values) result.put(key,value);
   return result;
}

}

int main()
{
   ILIC_REQUIRE(ilic::cli::schemaLanguageFromIliVersion("1.0") == "ili1");
   ILIC_REQUIRE(ilic::cli::schemaLanguageFromIliVersion("2.3") == "ili2_3");
   ILIC_REQUIRE(ilic::cli::schemaLanguageFromIliVersion("2.4") == "ili2_4");
   ILIC_REQUIRE(!ilic::cli::schemaLanguageFromIliVersion("2.2"));

   auto rootModels = arguments({{"models","DatasetIdx16"}});
   auto config = ilic::cli::buildRepositoryConfig(rootModels);
   ILIC_REQUIRE(config.effectiveRepositories == ilic::RepositoryManager::defaultRepositories());
   ILIC_REQUIRE(!ilic::cli::validateRepositoryCliConfig(rootModels,config,false));

   auto empty = arguments({{"repositories","https://models.interlis.ch"}});
   auto emptyConfig = ilic::cli::buildRepositoryConfig(empty);
   ILIC_REQUIRE(ilic::cli::validateRepositoryCliConfig(empty,emptyConfig,false)
      == "no input .ili files or root models specified");

   auto versionOnly = arguments({{"ili-version","2.4"}});
   auto versionOnlyConfig = ilic::cli::buildRepositoryConfig(versionOnly);
   ILIC_REQUIRE(ilic::cli::validateRepositoryCliConfig(versionOnly,versionOnlyConfig,false)
      == "no input .ili files or root models specified");

   auto invalid = arguments({{"ili-version","ili2_4"},{"ilifile1","Root.ili"}});
   auto invalidConfig = ilic::cli::buildRepositoryConfig(invalid);
   auto invalidMessage = ilic::cli::validateRepositoryCliConfig(invalid,invalidConfig,true);
   ILIC_REQUIRE(invalidMessage.has_value());
   ILIC_REQUIRE(invalidMessage->find("expected") != std::string::npos);

   auto noDefaults = arguments({{"models","DatasetIdx16"},{"no-default-repositories",""}});
   auto noDefaultsConfig = ilic::cli::buildRepositoryConfig(noDefaults);
   ILIC_REQUIRE(noDefaultsConfig.effectiveRepositories.empty());
   ILIC_REQUIRE(ilic::cli::validateRepositoryCliConfig(noDefaults,noDefaultsConfig,false)
      == "no repositories configured");

   auto explicitFile = arguments({{"repositories","https://example.org/models/"},
      {"ilifile1","Root.ili"}});
   auto explicitConfig = ilic::cli::buildRepositoryConfig(explicitFile);
   ILIC_REQUIRE(!ilic::cli::validateRepositoryCliConfig(explicitFile,explicitConfig,true));
   ILIC_REQUIRE(explicitConfig.effectiveRepositories.size() == 2);
   ILIC_REQUIRE(explicitConfig.effectiveRepositories.front() == "https://example.org/models");
   ILIC_REQUIRE(explicitConfig.effectiveRepositories.back() == "https://models.interlis.ch");

   auto duplicate = arguments({{"repositories",
      "https://models.interlis.ch/;https://models.interlis.ch"},{"models","A"}});
   ILIC_REQUIRE(ilic::cli::buildRepositoryConfig(duplicate).effectiveRepositories.size() == 1);
   return 0;
}
