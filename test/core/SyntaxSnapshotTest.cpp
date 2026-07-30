#include "ilic/Compiler.h"

#include "ilic/test/TestHarness.h"
#include <string>

int main()
{
   ilic::CompilerSession session;
   const std::string uri = "memory:///Unicode.ili";
   const std::string source =
      "INTERLIS 2.4;\nMODEL Unicode (en) AT \"https://example.invalid/\xF0\x9F\x98\x80\" VERSION \"1\" =\n"
      "  IMPORTS UNQUALIFIED Geometry, Units;\nEND Unicode.\n";
   session.putSource(uri,source,42);

   const ilic::SyntaxSnapshot snapshot = session.parse(uri);
   ILIC_REQUIRE(snapshot.success);
   ILIC_REQUIRE(snapshot.uri == uri);
   ILIC_REQUIRE(snapshot.documentVersion == 42);
   ILIC_REQUIRE(snapshot.iliVersion == "2.4");
   ILIC_REQUIRE(!snapshot.tokens.empty());
   ILIC_REQUIRE(!snapshot.nodes.empty());
   ILIC_REQUIRE(snapshot.imports.size() == 2);
   ILIC_REQUIRE(snapshot.imports[0] == "Geometry");
   ILIC_REQUIRE(snapshot.imports[1] == "Units");
   ILIC_REQUIRE(snapshot.importReferences.size() == 2);
   ILIC_REQUIRE(snapshot.importReferences[0].model == "Geometry");
   ILIC_REQUIRE(snapshot.importReferences[0].unqualified);
   ILIC_REQUIRE(snapshot.importReferences[0].range.valid);
   ILIC_REQUIRE(snapshot.importReferences[0].range.start.line == 2);
   ILIC_REQUIRE(snapshot.importReferences[0].range.start.character == 22);
   ILIC_REQUIRE(snapshot.importReferences[0].range.end.character == 30);
   ILIC_REQUIRE(snapshot.importReferences[1].model == "Units");
   ILIC_REQUIRE(!snapshot.importReferences[1].unqualified);
   ILIC_REQUIRE(snapshot.importReferences[1].range.start.character == 32);
   ILIC_REQUIRE(snapshot.importReferences[1].range.end.character == 37);
   // The non-ASCII URL above exercises UTF-8/UTF-16 accounting before the
   // parser token while the byte offset still points at its exact source byte.
   ILIC_REQUIRE(snapshot.importReferences[0].range.start.byteOffset == source.find("Geometry"));

   session.putSource(uri,"INTERLIS 2.4;\nMODEL Broken =\n",43);
   const ilic::SyntaxSnapshot broken = session.parse(uri);
   ILIC_REQUIRE(!broken.success);
   ILIC_REQUIRE(!broken.diagnostics.empty());
   ILIC_REQUIRE(broken.diagnostics.front().range.valid);
   return 0;
}
