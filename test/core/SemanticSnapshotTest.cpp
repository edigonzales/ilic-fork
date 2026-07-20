#include "ilic/Compiler.h"
#include "ilic/Semantic.h"

#include <algorithm>
#include <cassert>
#include <string>

int main()
{
   ilic::CompilerSession session;
   const std::string uri = "memory:///Semantic.ili";
   const std::string baseUri = "memory:///BaseLibrary.ili";
   session.putSource(baseUri,R"ili(INTERLIS 2.4;
MODEL BaseLibrary (en) AT "https://example.invalid" VERSION "1" =
  DOMAIN Remote = TEXT * 80;
END BaseLibrary.
)ili",3);
   const std::string semanticSource = R"ili(INTERLIS 2.4;
MODEL Semantic (en) AT "https://example.invalid" VERSION "1" =
  IMPORTS BaseLibrary;
  TOPIC Data =
    CLASS Base (ABSTRACT) =
      /* 😀 */ Name : BaseLibrary.Remote;
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
)ili";
   session.putSource(uri,semanticSource,9);

   ilic::CompilationRequest request;
   request.roots = {uri};
   const ilic::SemanticSnapshot snapshot = session.analyze(request);
   assert(snapshot.success);
   assert(snapshot.documentVersions.at(uri) == 9);
   assert(snapshot.missingModels.empty());
   assert(std::any_of(snapshot.symbols.begin(),snapshot.symbols.end(),[](const auto &symbol) {
      return symbol.qualifiedName == "Semantic.Data.Item" && symbol.kind == "class" &&
         symbol.selectionRange.valid && symbol.selectionRange.start.line == 7 &&
         symbol.selectionRange.start.character == 10 && symbol.selectionRange.end.character == 14;
   }));
   assert(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == "inheritance" && reference.range.valid &&
         reference.range.start.line == 7 && reference.range.start.character == 23 &&
         reference.range.end.character == 27;
   }));
   assert(std::any_of(snapshot.references.begin(),snapshot.references.end(),
      [&semanticSource](const auto &reference) {
      return reference.kind == "type" && reference.range.valid &&
         reference.range.start.line == 5 && reference.range.start.character == 34 &&
         reference.range.end.character == 40 &&
         reference.range.start.byteOffset == semanticSource.find("Remote;");
   }));
   assert(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == "qualifier" && reference.range.valid &&
         reference.range.start.line == 5 && reference.range.start.character == 22 &&
         reference.range.end.character == 33;
   }));
   assert(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == "role" && reference.range.valid &&
         reference.range.start.line == 11 && reference.range.start.character == 18 &&
         reference.range.end.character == 22;
   }));
   assert(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == "import" && reference.range.valid &&
         reference.range.start.line == 2 && reference.range.start.character == 10 &&
         reference.range.end.character == 21;
   }));
   assert(std::any_of(snapshot.dependencies.begin(),snapshot.dependencies.end(),
      [&uri,&baseUri](const auto &dependency) {
      return dependency.sourceUri == uri && dependency.targetUri == baseUri &&
         dependency.model == "BaseLibrary" && dependency.range.valid &&
         dependency.range.start.line == 2 && dependency.range.start.character == 10;
   }));
   assert(std::any_of(snapshot.diagram.edges.begin(),snapshot.diagram.edges.end(),[](const auto &edge) {
      return edge.kind == "association" && edge.label == "Link";
   }));
   assert(!snapshot.documentation.title.empty());

   ilic::CompilerSession ili23Session;
   const std::string ili23LibraryUri = "memory:///Library23.ili";
   const std::string ili23Uri = "memory:///Root23.ili";
   ili23Session.putSource(ili23LibraryUri,R"ili(INTERLIS 2.3;
MODEL Library23 (en) AT "https://example.invalid" VERSION "1" =
  DOMAIN Code = TEXT * 8;
END Library23.
)ili",1);
   ili23Session.putSource(ili23Uri,R"ili(INTERLIS 2.3;
MODEL Root23 (en) AT "https://example.invalid" VERSION "1" =
  IMPORTS Library23;
  TOPIC Data =
    CLASS Item =
      Value : Library23.Code;
    END Item;
  END Data;
END Root23.
)ili",2);
   ilic::CompilationRequest ili23Request;
   ili23Request.roots = {ili23Uri};
   const ilic::SemanticSnapshot ili23 = ili23Session.analyze(ili23Request);
   assert(ili23.success);
   assert(std::any_of(ili23.references.begin(),ili23.references.end(),[](const auto &reference) {
      return reference.kind == "type" && reference.range.valid &&
         reference.range.start.line == 5 && reference.range.start.character == 24 &&
         reference.range.end.character == 28;
   }));
   assert(std::any_of(ili23.references.begin(),ili23.references.end(),[](const auto &reference) {
      return reference.kind == "qualifier" && reference.range.valid &&
         reference.range.start.line == 5 && reference.range.start.character == 14 &&
         reference.range.end.character == 23;
   }));

   ilic::CompilerSession missingSession;
   const std::string missingUri = "memory:///Missing.ili";
   missingSession.putSource(missingUri,R"ili(INTERLIS 2.4;
MODEL Missing (en) AT "https://example.invalid" VERSION "1" =
  IMPORTS DoesNotExist;
END Missing.
)ili",1);
   ilic::CompilationRequest missingRequest;
   missingRequest.roots = {missingUri};
   const ilic::SemanticSnapshot missing = missingSession.analyze(missingRequest);
   assert(!missing.success);
   assert(std::find(missing.missingModels.begin(),missing.missingModels.end(),
      "DoesNotExist") != missing.missingModels.end());
   return 0;
}
