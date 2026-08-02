#include "ilic/DiagnosticBuilder.h"
#include "ilic/DiagnosticCatalog.h"
#include "ilic/DiagnosticPipeline.h"
#include "ilic/DiagnosticRangeResolver.h"
#include "ilic/SourceManager.h"

#include "ilic/test/TestHarness.h"

#include <string>

namespace {

ilic::SourceRange range(const std::string &uri,std::size_t start,std::size_t end)
{
   ilic::SourceRange value;
   value.valid = true;
   value.uri = uri;
   value.start.byteOffset = start;
   value.end.byteOffset = end;
   return value;
}

} // namespace

int main()
{
   ILIC_REQUIRE(ilic::DiagnosticCatalog::isRegistered("ILIC-SYN-MISSING-TOKEN"));
   ILIC_REQUIRE(!ilic::DiagnosticCatalog::isRegistered("ILIC-SYN-NOT-REGISTERED"));

   ilic::SourceManager sources;
   const std::string uri = "memory:///ranges.ili";
   sources.put(uri,"INTERLIS 2.4;\r\nMODEL Zürich 😀 =\r\n",7);
   const ilic::DiagnosticRangeResolver ranges(sources);
   const auto unicode = ranges.byteRange(uri,sources.get(uri)->text.find("Zürich"),
      sources.get(uri)->text.find("Zürich") + 7);
   ILIC_REQUIRE(unicode.valid);
   ILIC_REQUIRE(unicode.start.line == 1);
   ILIC_REQUIRE(unicode.start.character == 6);
   ILIC_REQUIRE(unicode.end.character == 12);
   ILIC_REQUIRE(unicode.end.byteOffset > unicode.start.byteOffset);
   const auto eof = ranges.insertionPoint(uri,sources.get(uri)->text.size());
   ILIC_REQUIRE(eof.valid);
   ILIC_REQUIRE(eof.start.line == eof.end.line);
   ILIC_REQUIRE(eof.start.character == eof.end.character);
   ILIC_REQUIRE(eof.start.byteOffset == eof.end.byteOffset);
   ILIC_REQUIRE(eof.start.line == 2);

   const auto primary = ilic::DiagnosticBuilder("ILIC-SYN-MISSING-TOKEN")
      .message("missing\nsemicolon")
      .primaryRange(range(uri,20,20))
      .source("compiler")
      .note("")
      .producer("test")
      .cause({42})
      .build();
   const auto duplicate = ilic::DiagnosticBuilder("ILIC-SYN-MISSING-TOKEN")
      .message("missing semicolon")
      .primaryRange(range(uri,20,20))
      .source("compiler")
      .producer("test")
      .cause({42})
      .build();
   auto cascaded = ilic::DiagnosticBuilder("ILIC-NAME-TYPE-NOT-FOUND")
      .message("follow-up")
      .primaryRange(range(uri,21,22))
      .source("compiler")
      .publicationClass(ilic::DiagnosticPublicationClass::Cascaded)
      .dependsOn({42})
      .producer("test")
      .build();
   auto independent = ilic::DiagnosticBuilder("ILIC-NAME-TYPE-NOT-FOUND")
      .message("independent")
      .primaryRange(range(uri,21,22))
      .source("compiler")
      .publicationClass(ilic::DiagnosticPublicationClass::Independent)
      .producer("test")
      .build();
   const auto published = ilic::DiagnosticPipeline{}.publish({
      duplicate,primary,std::move(cascaded),std::move(independent)});
   ILIC_REQUIRE(published.stats.candidates == 4);
   ILIC_REQUIRE(published.stats.exactDuplicatesRemoved == 1);
   ILIC_REQUIRE(published.stats.cascadesSuppressed == 1);
   ILIC_REQUIRE(published.values.size() == 2);
   ILIC_REQUIRE(published.values[0].message == "missing semicolon");
   ILIC_REQUIRE(published.values[0].phase == ilic::DiagnosticPhase::Syntax);
   ILIC_REQUIRE(published.values[1].message == "independent");
   ILIC_REQUIRE(published.values[0].fingerprint != published.values[1].fingerprint);
   return 0;
}
