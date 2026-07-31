#include "ilic/Compiler.h"

#include "ilic/test/TestHarness.h"

#include <algorithm>
#include <string>

int main()
{
   ilic::CompilerSession session;
   const std::string uri = "memory:///Editor.ili";
   const std::string source =
      "INTERLIS 2.3;\n"
      "MODEL Editor AT \"https://example.invalid\" VERSION \"1\" =\n"
      "  IMPORTS UNQUALIFIED Base, Units;\n"
      "  TOPIC Topic =\n"
      "    CLASS Item EXTENDS Base.Root =\n"
      "      value : TEXT;\n"
      "      values : BAG {0..*} OF Base.Root;\n"
      "      ref : REFERENCE TO Base.Root;\n"
      "    END Item;\n"
      "  END Topic;\n"
      "END Editor.\n";
   session.putSource(uri,source,17);

   const ilic::EditorSnapshot snapshot = session.editorSnapshot(uri);
   ILIC_REQUIRE(snapshot.uri == uri);
   ILIC_REQUIRE(snapshot.documentVersion == 17);
   ILIC_REQUIRE(snapshot.iliVersion == "2.3");
   ILIC_REQUIRE(snapshot.success);
   ILIC_REQUIRE(!snapshot.recovered);
   ILIC_REQUIRE(snapshot.complete);
   ILIC_REQUIRE(snapshot.declarations.size() == 6);
   ILIC_REQUIRE(snapshot.declarations[0].qualifiedName == "Editor");
   ILIC_REQUIRE(snapshot.declarations[1].qualifiedName == "Editor.Topic");
   ILIC_REQUIRE(snapshot.declarations[2].qualifiedName == "Editor.Topic.Item");
   ILIC_REQUIRE(snapshot.declarations[3].qualifiedName == "Editor.Topic.Item.value");
   ILIC_REQUIRE(snapshot.declarations[4].qualifiedName == "Editor.Topic.Item.values");
   ILIC_REQUIRE(snapshot.declarations[5].qualifiedName == "Editor.Topic.Item.ref");
   ILIC_REQUIRE(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == ilic::EditorReferenceKind::Extends && reference.text == "Base.Root";
   }));
   ILIC_REQUIRE(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == ilic::EditorReferenceKind::Collection && reference.text == "Base.Root";
   }));
   ILIC_REQUIRE(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == ilic::EditorReferenceKind::Reference && reference.text == "Base.Root";
   }));
   ILIC_REQUIRE(snapshot.imports.size() == 2);
   ILIC_REQUIRE(snapshot.imports[0].model == "Base");
   ILIC_REQUIRE(snapshot.imports[1].model == "Units");
   ILIC_REQUIRE(snapshot.contexts.size() == 3);

   ilic::CompilerSession ili1;
   const std::string ili1Uri = "memory:///Editor1.ili";
   const std::string ili1Source =
      "TRANSFER Editor1;\n"
      "MODEL Editor1\n"
      "  TOPIC Data =\n"
      "    TABLE Item =\n"
      "      value: TEXT;\n"
      "      NO IDENT\n"
      "    END Item;\n"
      "  END Data.\n"
      "END Editor1.\n"
      "FORMAT FREE;\n"
      "CODE\n"
      "  BLANK = DEFAULT, UNDEFINED = DEFAULT, CONTINUE = DEFAULT;\n"
      "  TID = ANY;\n"
      "END.\n";
   ili1.putSource(ili1Uri,ili1Source,3);
   const ilic::EditorSnapshot ili1Snapshot = ili1.editorSnapshot(ili1Uri);
   ILIC_REQUIRE(ili1Snapshot.iliVersion == "1.0");
   ILIC_REQUIRE(std::any_of(ili1Snapshot.declarations.begin(),ili1Snapshot.declarations.end(),
      [](const auto &declaration) { return declaration.kind == ilic::EditorSymbolKind::Class
         && declaration.name == "Item"; }));
   ILIC_REQUIRE(std::any_of(ili1Snapshot.declarations.begin(),ili1Snapshot.declarations.end(),
      [](const auto &declaration) { return declaration.kind == ilic::EditorSymbolKind::Attribute
         && declaration.name == "value"; }));

   ilic::CompilerSession tolerant;
   const std::string tolerantUri = "memory:///Tolerant.ili";
   std::string tolerantSource = "INTERLIS 2.3;\r\n!! invalid: ";
   tolerantSource.push_back(static_cast<char>(0xff));
   tolerantSource += "\r\nMODEL Tolerant AT \"https://example.invalid\" VERSION \"1\" =\r\n"
      "END Tolerant.\r\n";
   tolerant.putSource(tolerantUri,tolerantSource,4);
   const ilic::EditorSnapshot tolerantSnapshot = tolerant.editorSnapshot(tolerantUri);
   ILIC_REQUIRE(std::any_of(tolerantSnapshot.declarations.begin(),tolerantSnapshot.declarations.end(),
      [](const auto &declaration) { return declaration.name == "Tolerant"; }));
   const auto tolerantModel = std::find_if(tolerantSnapshot.declarations.begin(),
      tolerantSnapshot.declarations.end(),[](const auto &declaration) {
         return declaration.name == "Tolerant";
      });
   ILIC_REQUIRE(tolerantModel != tolerantSnapshot.declarations.end());
   ILIC_REQUIRE(tolerantModel->selectionRange.start.line == 2);

   const ilic::EditorSnapshot missing = tolerant.editorSnapshot("memory:///missing.ili");
   ILIC_REQUIRE(!missing.success);
   ILIC_REQUIRE(!missing.complete);
   ILIC_REQUIRE(!missing.diagnostics.empty());
   return 0;
}
