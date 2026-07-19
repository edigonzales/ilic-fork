#include "ilic/Compiler.h"
#include "ilic/Semantic.h"

#include <algorithm>
#include <cassert>
#include <string>

int main()
{
   ilic::CompilerSession session;
   const std::string uri = "memory:///Semantic.ili";
   session.putSource(uri,R"ili(INTERLIS 2.4;
MODEL Semantic (en) AT "https://example.invalid" VERSION "1" =
  TOPIC Data =
    CLASS Base (ABSTRACT) =
      Name : TEXT * 80;
    END Base;
    CLASS Item EXTENDS Base =
      Code : MANDATORY TEXT * 20;
    END Item;
    ASSOCIATION Link =
      left -- {1} Item;
      right -- {*} Base;
    END Link;
  END Data;
END Semantic.
)ili",9);

   ilic::CompilationRequest request;
   request.roots = {uri};
   const ilic::SemanticSnapshot snapshot = session.analyze(request);
   assert(snapshot.success);
   assert(snapshot.documentVersions.at(uri) == 9);
   assert(std::any_of(snapshot.symbols.begin(),snapshot.symbols.end(),[](const auto &symbol) {
      return symbol.qualifiedName == "Semantic.Data.Item" && symbol.kind == "class";
   }));
   assert(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == "inheritance";
   }));
   assert(std::any_of(snapshot.diagram.edges.begin(),snapshot.diagram.edges.end(),[](const auto &edge) {
      return edge.kind == "association" && edge.label == "Link";
   }));
   assert(snapshot.documentation.title == "Semantic");
   return 0;
}
