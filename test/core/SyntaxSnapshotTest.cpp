#include "ilic/Compiler.h"

#include <cassert>
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
   assert(snapshot.success);
   assert(snapshot.uri == uri);
   assert(snapshot.documentVersion == 42);
   assert(snapshot.iliVersion == "2.4");
   assert(!snapshot.tokens.empty());
   assert(!snapshot.nodes.empty());
   assert(snapshot.imports.size() == 2);
   assert(snapshot.imports[0] == "Geometry");
   assert(snapshot.imports[1] == "Units");
   assert(snapshot.importReferences.size() == 2);
   assert(snapshot.importReferences[0].model == "Geometry");
   assert(snapshot.importReferences[0].unqualified);
   assert(snapshot.importReferences[0].range.valid);
   assert(snapshot.importReferences[0].range.start.line == 2);
   assert(snapshot.importReferences[0].range.start.character == 22);
   assert(snapshot.importReferences[0].range.end.character == 30);
   assert(snapshot.importReferences[1].model == "Units");
   assert(!snapshot.importReferences[1].unqualified);
   assert(snapshot.importReferences[1].range.start.character == 32);
   assert(snapshot.importReferences[1].range.end.character == 37);
   // The non-ASCII URL above exercises UTF-8/UTF-16 accounting before the
   // parser token while the byte offset still points at its exact source byte.
   assert(snapshot.importReferences[0].range.start.byteOffset == source.find("Geometry"));

   session.putSource(uri,"INTERLIS 2.4;\nMODEL Broken =\n",43);
   const ilic::SyntaxSnapshot broken = session.parse(uri);
   assert(!broken.success);
   assert(!broken.diagnostics.empty());
   assert(broken.diagnostics.front().range.valid);
   return 0;
}
