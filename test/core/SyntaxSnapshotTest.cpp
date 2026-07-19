#include "ilic/Compiler.h"

#include <cassert>
#include <string>

int main()
{
   ilic::CompilerSession session;
   const std::string uri = "memory:///Unicode.ili";
   session.putSource(uri,
      "INTERLIS 2.4;\nMODEL Unicode (en) AT \"https://example.invalid\" VERSION \"1\" =\n"
      "  IMPORTS UNQUALIFIED Geometry, Units;\nEND Unicode.\n",42);

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

   session.putSource(uri,"INTERLIS 2.4;\nMODEL Broken =\n",43);
   const ilic::SyntaxSnapshot broken = session.parse(uri);
   assert(!broken.success);
   assert(!broken.diagnostics.empty());
   assert(broken.diagnostics.front().range.valid);
   return 0;
}
