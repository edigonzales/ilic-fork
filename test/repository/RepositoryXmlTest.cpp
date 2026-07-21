#include "RepositoryXml.h"

#include <cassert>
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
   assert(index.models.size() == 1);
   const auto &model = index.models.front();
   assert(model.name == "Old" && model.schemaLanguage == "ili2_3");
   assert(model.file == "old/Old.ili" && model.version == "2.10");
   assert(model.precursorVersion == "2.9" && model.md5 == "ABCDEF");
   assert(model.browseOnly && model.dependencies.size() == 1 && model.dependencies[0] == "Base");
   assert(diagnostics.size() == 1);
   assert(diagnostics[0].severity == ilic::DiagnosticSeverity::Warning);

   const std::string siteXml = R"xml(<TRANSFER><DATASECTION><Site>
     <parentSite><value>https://parent-1</value><value>https://parent-2</value></parentSite>
     <subsidiarySite><value>https://child-1</value></subsidiarySite>
   </Site></DATASECTION></TRANSFER>)xml";
   auto site = ilic::repository::RepositoryXml::parseSite(siteXml,"fixture",&diagnostics);
   assert((site.parentSites == std::vector<std::string>{"https://parent-1","https://parent-2"}));
   assert((site.subsidiarySites == std::vector<std::string>{"https://child-1"}));

   diagnostics.clear();
   auto invalid = ilic::repository::RepositoryXml::parseModelIndex("<broken>","fixture",&diagnostics);
   assert(invalid.models.empty() && diagnostics.size() == 1);
   assert(diagnostics[0].severity == ilic::DiagnosticSeverity::Error);
   return 0;
}
