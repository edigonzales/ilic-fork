#include "RepositoryVersion.h"

#include "ilic/test/TestHarness.h"
#include <string>
#include <vector>

namespace {

ilic::ModelMetadata model(std::string version,std::string precursor = {},
   std::string language = "ili2_4",bool browseOnly = false)
{
   ilic::ModelMetadata value;
   value.name = "Example";
   value.schemaLanguage = std::move(language);
   value.file = "Example.ili";
   value.version = std::move(version);
   value.precursorVersion = std::move(precursor);
   value.browseOnly = browseOnly;
   return value;
}

}

int main()
{
   using ilic::repository::selectLatestVersion;
   {
      std::vector<ilic::ModelMetadata> models = {model("1")};
      auto result = selectLatestVersion(models,"Example","ili2_4");
      ILIC_REQUIRE(result.model != nullptr);
      ILIC_REQUIRE(result.model->version == "1");
   }
   {
      std::vector<ilic::ModelMetadata> models = {model("1"),model("2","1"),model("3","2")};
      auto result = selectLatestVersion(models,"Example","ili2_4");
      ILIC_REQUIRE(result.model != nullptr);
      ILIC_REQUIRE(result.model->version == "3");
   }
   {
      std::vector<ilic::ModelMetadata> models = {model("2.9"),model("2.10","2.9")};
      auto result = selectLatestVersion(models,"Example","ili2_4");
      ILIC_REQUIRE(result.model != nullptr);
      ILIC_REQUIRE(result.model->version == "2.10");
   }
   {
      std::vector<ilic::ModelMetadata> models = {model("A"),model("B")};
      auto result = selectLatestVersion(models,"Example","ili2_4");
      ILIC_REQUIRE(result.model != nullptr);
      ILIC_REQUIRE(result.model->version == "A");
      ILIC_REQUIRE(result.diagnostics.size() == 1);
   }
   {
      std::vector<ilic::ModelMetadata> models = {model("A"),model("B","A"),model("C","A")};
      auto result = selectLatestVersion(models,"Example","ili2_4");
      ILIC_REQUIRE(result.model != nullptr);
      ILIC_REQUIRE(result.model->version == "B");
      ILIC_REQUIRE(result.diagnostics.size() == 1);
   }
   {
      std::vector<ilic::ModelMetadata> models = {model("A"),model("B","A"),model("X","UNKNOWN")};
      auto result = selectLatestVersion(models,"Example","ili2_4");
      ILIC_REQUIRE(result.model != nullptr);
      ILIC_REQUIRE(result.model->version == "B");
      ILIC_REQUIRE(result.diagnostics.size() == 1);
   }
   {
      std::vector<ilic::ModelMetadata> models = {model("B","A")};
      auto result = selectLatestVersion(models,"Example","ili2_4");
      ILIC_REQUIRE(result.model == nullptr);
      ILIC_REQUIRE(result.diagnostics.size() == 1);
   }
   {
      std::vector<ilic::ModelMetadata> models = {model("1"),model("2","1","ili2_4",true)};
      auto result = selectLatestVersion(models,"Example","ili2_4");
      ILIC_REQUIRE(result.model != nullptr);
      ILIC_REQUIRE(result.model->version == "1");
   }
   {
      std::vector<ilic::ModelMetadata> models = {
         model("23",{},"ili2_3"),model("1",{},"ili1"),model("24",{},"ili2_4")};
      auto latest = selectLatestVersion(models,"Example","");
      ILIC_REQUIRE(latest.model != nullptr);
      ILIC_REQUIRE(latest.model->schemaLanguage == "ili2_4");
      auto ili23 = selectLatestVersion(models,"Example","ili2_3");
      ILIC_REQUIRE(ili23.model != nullptr);
      ILIC_REQUIRE(ili23.model->version == "23");
   }
   {
      std::vector<ilic::ModelMetadata> models = {
         model("broken","missing","ili2_4"),model("23",{},"ili2_3")};
      auto result = selectLatestVersion(models,"Example","");
      ILIC_REQUIRE(result.model != nullptr);
      ILIC_REQUIRE(result.model->schemaLanguage == "ili2_3");
      ILIC_REQUIRE(result.diagnostics.size() == 1);
   }
   ILIC_REQUIRE((ilic::repository::supportedSchemaLanguagePreference()
      == std::vector<std::string>{"ili2_4","ili2_3","ili1"}));
   return 0;
}
