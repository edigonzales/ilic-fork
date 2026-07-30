#include "RepositoryXml.h"

#include "ilic/test/TestHarness.h"
#include <string>
#include <vector>

int main()
{
   const std::string xml = R"xml(<?xml version="1.0"?>
<TRANSFER xmlns:r="urn:interlis:repository">
 <DATASECTION>
  <r:IliRepository09.RepositoryIndex.ModelMetadata>
   <r:Name>Old</r:Name><r:SchemaLanguage>ili2_3</r:SchemaLanguage>
   <r:File>old/Old.ili</r:File><r:Version>2.10</r:Version>
   <r:publishingDate>2026-01-01</r:publishingDate><r:precursorVersion>2.9</r:precursorVersion>
   <r:md5>ABCDEF</r:md5><r:browseOnly>true</r:browseOnly>
   <r:dependsOnModel><r:ModelName_><r:value>Base</r:value></r:ModelName_></r:dependsOnModel>
   <r:futureElement>ignored</r:futureElement>
  </r:IliRepository09.RepositoryIndex.ModelMetadata>
  <IliRepository20.RepositoryIndex.ModelMetadata>
   <Name>Invalid</Name><File>Invalid.ili</File>
  </IliRepository20.RepositoryIndex.ModelMetadata>
 </DATASECTION>
</TRANSFER>)xml";
   std::vector<ilic::Diagnostic> diagnostics;
   auto index = ilic::repository::RepositoryXml::parseModelIndex(xml,"fixture",&diagnostics);
   ILIC_REQUIRE(index.models.size() == 1);
   const auto &model = index.models.front();
   ILIC_REQUIRE(model.name == "Old");
   ILIC_REQUIRE(model.schemaLanguage == "ili2_3");
   ILIC_REQUIRE(model.file == "old/Old.ili");
   ILIC_REQUIRE(model.version == "2.10");
   ILIC_REQUIRE(model.precursorVersion == "2.9");
   ILIC_REQUIRE(model.md5 == "ABCDEF");
   ILIC_REQUIRE(model.browseOnly);
   ILIC_REQUIRE(model.dependencies.size() == 1);
   ILIC_REQUIRE(model.dependencies[0] == "Base");
   ILIC_REQUIRE(diagnostics.size() == 1);
   ILIC_REQUIRE(diagnostics[0].severity == ilic::DiagnosticSeverity::Warning);

   const std::string siteXml = R"xml(<TRANSFER><DATASECTION><Site>
     <parentSite><value>https://parent-1</value><value>https://parent-2</value></parentSite>
     <subsidiarySite><value>https://child-1</value></subsidiarySite>
   </Site></DATASECTION></TRANSFER>)xml";
   auto site = ilic::repository::RepositoryXml::parseSite(siteXml,"fixture",&diagnostics);
   ILIC_REQUIRE((site.parentSites == std::vector<std::string>{"https://parent-1","https://parent-2"}));
   ILIC_REQUIRE((site.subsidiarySites == std::vector<std::string>{"https://child-1"}));

   diagnostics.clear();
   auto invalid = ilic::repository::RepositoryXml::parseModelIndex("<broken>","fixture",&diagnostics);
   ILIC_REQUIRE(invalid.models.empty());
   ILIC_REQUIRE(diagnostics.size() == 1);
   ILIC_REQUIRE(diagnostics[0].severity == ilic::DiagnosticSeverity::Error);
   return 0;
}
