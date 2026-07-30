#include "ilic/Compiler.h"
#include "ilic/Semantic.h"

#include <algorithm>
#include "ilic/test/TestHarness.h"
#include <regex>
#include <string>

namespace ilic {
struct CompilerSessionTestAccess {
   static std::uint64_t compileInvocationCount(const CompilerSession &session)
   {
      return session.compileInvocationCount_;
   }
};
}

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
      /** Base name documentation */ Name : BaseLibrary.Remote;
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
   const std::string unrelatedUri = "memory:///UnrelatedBroken.ili";
   session.putSource(unrelatedUri,"this is not INTERLIS",10);

   ilic::CompilationRequest request;
   request.roots = {uri};
   const auto compileCountBefore = ilic::CompilerSessionTestAccess::compileInvocationCount(session);
   const ilic::CompilationAnalysisResult analysis = session.compileAndAnalyze(request);
   ILIC_REQUIRE(ilic::CompilerSessionTestAccess::compileInvocationCount(session) == compileCountBefore + 1);
   const ilic::SemanticSnapshot &snapshot = analysis.semantic;
   ILIC_REQUIRE(snapshot.success);
   ILIC_REQUIRE(analysis.compilation.success);
   const std::regex completion("^inf: ilic completed with no errors, no warnings [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$");
   ILIC_REQUIRE(std::any_of(analysis.compilation.transcript.begin(),analysis.compilation.transcript.end(),
      [&completion](const auto &line) { return std::regex_match(line,completion); }));
   ILIC_REQUIRE(std::none_of(analysis.compilation.transcript.begin(),analysis.compilation.transcript.end(),
      [](const auto &line) { return line.find("compiler run done") != std::string::npos; }));
   ILIC_REQUIRE(snapshot.documentVersions.count(uri) == 1);
   ILIC_REQUIRE(snapshot.documentVersions.at(uri) == 9);
   ILIC_REQUIRE(snapshot.documentVersions.count(unrelatedUri) == 0);
   ILIC_REQUIRE(analysis.syntax.size() == 2);
   ILIC_REQUIRE(std::any_of(analysis.syntax.begin(),analysis.syntax.end(),[&baseUri](const auto &syntax) {
      return syntax.uri == baseUri;
   }));
   ILIC_REQUIRE(std::none_of(analysis.syntax.begin(),analysis.syntax.end(),[&unrelatedUri](const auto &syntax) {
      return syntax.uri == unrelatedUri;
   }));
   ILIC_REQUIRE(snapshot.missingModels.empty());
   ILIC_REQUIRE(std::any_of(snapshot.symbols.begin(),snapshot.symbols.end(),[](const auto &symbol) {
      return symbol.qualifiedName == "Semantic.Data.Item" && symbol.kind == "class" &&
         symbol.selectionRange.valid && symbol.selectionRange.start.line == 7 &&
         symbol.selectionRange.start.character == 10 && symbol.selectionRange.end.character == 14 &&
         symbol.endRange.valid && symbol.endRange.start.line == 9 &&
         symbol.endRange.start.character == 8 && symbol.endRange.end.character == 12;
   }));
   ILIC_REQUIRE(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == "inheritance" && reference.range.valid &&
         reference.range.start.line == 7 && reference.range.start.character == 23 &&
         reference.range.end.character == 27;
   }));
   ILIC_REQUIRE(std::any_of(snapshot.references.begin(),snapshot.references.end(),
      [&semanticSource](const auto &reference) {
      return reference.kind == "type" && reference.range.valid &&
         reference.range.start.line == 5 && reference.range.start.character == 56 &&
         reference.range.end.character == 62 &&
         reference.range.start.byteOffset == semanticSource.find("Remote;");
   }));
   ILIC_REQUIRE(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == "qualifier" && reference.range.valid &&
         reference.range.start.line == 5 && reference.range.start.character == 44 &&
         reference.range.end.character == 55;
   }));
   ILIC_REQUIRE(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == "role" && reference.range.valid &&
         reference.range.start.line == 11 && reference.range.start.character == 18 &&
         reference.range.end.character == 22;
   }));
   ILIC_REQUIRE(std::any_of(snapshot.references.begin(),snapshot.references.end(),[](const auto &reference) {
      return reference.kind == "import" && reference.range.valid &&
         reference.range.start.line == 2 && reference.range.start.character == 10 &&
         reference.range.end.character == 21;
   }));
   ILIC_REQUIRE(std::any_of(snapshot.dependencies.begin(),snapshot.dependencies.end(),
      [&uri,&baseUri](const auto &dependency) {
      return dependency.sourceUri == uri && dependency.targetUri == baseUri &&
         dependency.model == "BaseLibrary" && dependency.range.valid &&
         dependency.range.start.line == 2 && dependency.range.start.character == 10;
   }));
   ILIC_REQUIRE(std::any_of(snapshot.diagram.edges.begin(),snapshot.diagram.edges.end(),[](const auto &edge) {
      return edge.kind == "association" && edge.label == "left–right" &&
         edge.sourceCardinality == "1" && edge.targetCardinality == "0..*" &&
         edge.cardinality == "left 1 / right 0..*";
   }));
   ILIC_REQUIRE(!snapshot.documentation.title.empty());
   ILIC_REQUIRE(snapshot.documentation.models.size() == 1);
   const auto &documentationModel = snapshot.documentation.models.front();
   ILIC_REQUIRE(documentationModel.name == "Semantic");
   ILIC_REQUIRE(documentationModel.uri == uri);
   ILIC_REQUIRE(documentationModel.topics.size() == 1);
   ILIC_REQUIRE(documentationModel.topics.front().name == "Data");
   const auto itemDocumentation = std::find_if(
      documentationModel.topics.front().viewables.begin(),
      documentationModel.topics.front().viewables.end(),
      [](const auto &viewable) { return viewable.name == "Item"; });
   ILIC_REQUIRE(itemDocumentation != documentationModel.topics.front().viewables.end());
   ILIC_REQUIRE(std::any_of(itemDocumentation->rows.begin(),itemDocumentation->rows.end(),
      [](const auto &row) {
         return row.name == "left" && row.cardinality == "1" && row.type == "Base";
      }));
   ILIC_REQUIRE(std::any_of(itemDocumentation->rows.begin(),itemDocumentation->rows.end(),
      [](const auto &row) {
         return row.name == "Code" && row.description == "";
      }));
   ILIC_REQUIRE(std::none_of(documentationModel.topics.front().viewables.begin(),
      documentationModel.topics.front().viewables.end(),
      [](const auto &viewable) { return viewable.name == "Link"; }));
   const auto baseDocumentation = std::find_if(
      documentationModel.topics.front().viewables.begin(),
      documentationModel.topics.front().viewables.end(),
      [](const auto &viewable) { return viewable.name == "Base"; });
   ILIC_REQUIRE(baseDocumentation != documentationModel.topics.front().viewables.end());
   ILIC_REQUIRE(std::any_of(baseDocumentation->rows.begin(),baseDocumentation->rows.end(),
      [](const auto &row) {
         return row.name == "Name" && row.description == "Base name documentation";
      }));

   ilic::CompilerSession extendedSession;
   const std::string extendedUri = "memory:///Extended.ili";
   extendedSession.putSource(extendedUri,R"ili(INTERLIS 2.4;
MODEL Extended (en) AT "https://example.invalid" VERSION "1" =
  TOPIC Data =
    CLASS Base (ABSTRACT) =
      Name : TEXT * 80;
    END Base;
    CLASS Child EXTENDS Base =
      Name (EXTENDED) : TEXT * 80;
    END Child;
  END Data;
END Extended.
)ili",1);
   ilic::CompilationRequest extendedRequest;
   extendedRequest.roots = {extendedUri};
   const ilic::SemanticSnapshot extended = extendedSession.analyze(extendedRequest);
   ILIC_REQUIRE(extended.success);
   const auto baseAttribute = std::find_if(extended.symbols.begin(),extended.symbols.end(),
      [](const auto &symbol) { return symbol.qualifiedName == "Extended.Data.Base.Name"; });
   const auto childAttribute = std::find_if(extended.symbols.begin(),extended.symbols.end(),
      [](const auto &symbol) { return symbol.qualifiedName == "Extended.Data.Child.Name"; });
   ILIC_REQUIRE(baseAttribute != extended.symbols.end());
   ILIC_REQUIRE(childAttribute != extended.symbols.end());
   ILIC_REQUIRE(std::any_of(extended.references.begin(),extended.references.end(),
      [&baseAttribute,&childAttribute](const auto &reference) {
      return reference.kind == "inheritance" &&
         reference.sourceId == childAttribute->id && reference.targetId == baseAttribute->id &&
         reference.range.valid && reference.range.start.line == 7 &&
         reference.range.start.character == 6 && reference.range.end.character == 10;
   }));

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
   ILIC_REQUIRE(ili23.success);
   ILIC_REQUIRE(std::any_of(ili23.references.begin(),ili23.references.end(),[](const auto &reference) {
      return reference.kind == "type" && reference.range.valid &&
         reference.range.start.line == 5 && reference.range.start.character == 24 &&
         reference.range.end.character == 28;
   }));
   ILIC_REQUIRE(std::any_of(ili23.references.begin(),ili23.references.end(),[](const auto &reference) {
      return reference.kind == "qualifier" && reference.range.valid &&
         reference.range.start.line == 5 && reference.range.start.character == 14 &&
         reference.range.end.character == 23;
   }));

   ilic::CompilerSession diagramSession;
   const std::string diagramLibraryUri = "memory:///DiagramLibrary.ili";
   const std::string diagramRootUri = "memory:///DiagramRoot.ili";
   diagramSession.putSource(diagramLibraryUri,R"ili(INTERLIS 2.4;
MODEL DiagramLibrary (en) AT "https://example.invalid" VERSION "1" =
  DOMAIN ImportedColors = (red, blue);
  CLASS ImportedBase (ABSTRACT) =
    ImportedName : MANDATORY TEXT * 40;
  END ImportedBase;
  TOPIC ImportedData =
    CLASS ImportedClass =
      Name : TEXT * 20;
    END ImportedClass;
  END ImportedData;
END DiagramLibrary.
)ili",1);
   diagramSession.putSource(diagramRootUri,R"ili(INTERLIS 2.4;
MODEL DiagramRoot (en) AT "https://example.invalid" VERSION "1" =
  IMPORTS DiagramLibrary;
  DOMAIN RootColors (ABSTRACT) = (red, blue);
  FUNCTION RootFunction(value : TEXT) : TEXT;
  STRUCTURE RootStruct (ABSTRACT) =
    Value : TEXT * 20;
  END RootStruct;
  CLASS RootClass (ABSTRACT) =
    Name : MANDATORY TEXT * 20;
    State : (open, closed, archived);
    Values : LIST {1..*} OF TEXT;
    States : LIST {1..*} OF (draft, final);
    MANDATORY CONSTRAINT Named: DEFINED(Name);
    MANDATORY CONSTRAINT DEFINED(Name);
  END RootClass;
  CLASS RootChild (ABSTRACT) EXTENDS RootClass =
    ChildName : TEXT;
  END RootChild;
  CLASS ExternalChild (ABSTRACT) EXTENDS DiagramLibrary.ImportedBase =
    LocalName : TEXT;
  END ExternalChild;
  TOPIC Data (ABSTRACT) =
    DOMAIN TopicColors = (one, two);
    DOMAIN TopicTree = ALL OF RootColors;
    FUNCTION TopicFunction(value : TEXT) : TEXT;
    STRUCTURE TopicStruct (ABSTRACT) =
      Value : TEXT * 20;
    END TopicStruct;
    CLASS TopicClass (ABSTRACT) =
      Name : TEXT * 20;
    END TopicClass;
    VIEW TopicView (ABSTRACT)
      PROJECTION OF source ~ TopicClass;
    =
      ATTRIBUTE ALL OF source;
    END TopicView;
  END Data;
END DiagramRoot.
)ili",2);
   ilic::CompilationRequest diagramRequest;
   diagramRequest.roots = {diagramRootUri};
   const ilic::SemanticSnapshot diagram = diagramSession.analyze(diagramRequest);
   ILIC_REQUIRE(diagram.success);
   ILIC_REQUIRE(diagram.documentation.models.size() == 1);
   const auto &diagramDocumentation = diagram.documentation.models.front();
   ILIC_REQUIRE(diagramDocumentation.name == "DiagramRoot");
   ILIC_REQUIRE(diagramDocumentation.topics.size() == 1);
   ILIC_REQUIRE(diagramDocumentation.enumerations.size() == 1);
   ILIC_REQUIRE(diagramDocumentation.enumerations.front().name == "RootColors");
   ILIC_REQUIRE(diagramDocumentation.enumerations.front().entries.size() == 2);
   ILIC_REQUIRE(diagramDocumentation.enumerations.front().entries.front().value == "red");
   const auto documentationRootClass = std::find_if(
      diagramDocumentation.viewables.begin(),diagramDocumentation.viewables.end(),
      [](const auto &viewable) { return viewable.name == "RootClass"; });
   ILIC_REQUIRE(documentationRootClass != diagramDocumentation.viewables.end());
   ILIC_REQUIRE(documentationRootClass->isAbstract);
   const auto documentationState = std::find_if(
      documentationRootClass->rows.begin(),documentationRootClass->rows.end(),
      [](const auto &row) { return row.name == "State"; });
   ILIC_REQUIRE(documentationState != documentationRootClass->rows.end());
   ILIC_REQUIRE(documentationState->type == "Enumeration");
   ILIC_REQUIRE(documentationState->description == "open, closed, archived");
   ILIC_REQUIRE(diagramDocumentation.topics.front().enumerations.size() == 2);
   ILIC_REQUIRE(std::any_of(diagramDocumentation.topics.front().enumerations.begin(),
      diagramDocumentation.topics.front().enumerations.end(),[](const auto &enumeration) {
         return enumeration.name == "TopicTree" && enumeration.entries.size() == 2;
      }));
   ILIC_REQUIRE(std::any_of(diagram.symbols.begin(),diagram.symbols.end(),[](const auto &symbol) {
      return symbol.qualifiedName == "DiagramLibrary.ImportedData.ImportedClass";
   }));
   const auto modelScope = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "modelScope" && node.label == "Model Scope"; });
   ILIC_REQUIRE(modelScope != diagram.diagram.nodes.end());
   ILIC_REQUIRE(modelScope->containerId.empty());
   const auto rootColors = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "enumeration" && node.label == "RootColors"; });
   ILIC_REQUIRE(rootColors != diagram.diagram.nodes.end());
   ILIC_REQUIRE(rootColors->containerId == modelScope->id);
   ILIC_REQUIRE((rootColors->stereotypes ==
      std::vector<std::string>{"Abstract","Enumeration"}));
   ILIC_REQUIRE((rootColors->enumValues == std::vector<std::string>{"red","blue"}));
   const auto topicTree = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "enumeration" && node.label == "TopicTree"; });
   ILIC_REQUIRE(topicTree != diagram.diagram.nodes.end());
   ILIC_REQUIRE((topicTree->enumValues == std::vector<std::string>{"red","blue"}));
   const auto rootClass = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "class" && node.label == "RootClass"; });
   ILIC_REQUIRE(rootClass != diagram.diagram.nodes.end());
   ILIC_REQUIRE(rootClass->containerId == modelScope->id);
   ILIC_REQUIRE((rootClass->stereotypes == std::vector<std::string>{"Abstract"}));
   ILIC_REQUIRE(rootClass->members.size() == 4);
   ILIC_REQUIRE(rootClass->members.front().type == "TEXT");
   ILIC_REQUIRE(rootClass->members.front().cardinality == "1");
   ILIC_REQUIRE((rootClass->members[1].inlineEnumValues ==
      std::vector<std::string>{"open","closed","archived"}));
   ILIC_REQUIRE(rootClass->members[2].type == "TEXT");
   ILIC_REQUIRE(rootClass->members[2].cardinality == "1..*");
   ILIC_REQUIRE(rootClass->members[3].type == "ENUMERATION");
   ILIC_REQUIRE(rootClass->members[3].cardinality == "1..*");
   ILIC_REQUIRE(rootClass->members[3].inlineEnumValues ==
      std::vector<std::string>({"draft","final"}));
   ILIC_REQUIRE((rootClass->operations ==
      std::vector<std::string>{"Named()","constraint1()"}));
   const auto rootChild = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "class" && node.label == "RootChild"; });
   ILIC_REQUIRE(rootChild != diagram.diagram.nodes.end());
   ILIC_REQUIRE(rootChild->members.size() == 5);
   ILIC_REQUIRE(rootChild->members[1].inherited);
   ILIC_REQUIRE(rootChild->members[1].declaringType == "RootClass");
   ILIC_REQUIRE(std::any_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [&modelScope](const auto &node) {
         return node.kind == "external" && node.label == "ImportedBase" &&
            node.containerId == modelScope->id &&
            node.stereotypes ==
               std::vector<std::string>{"Abstract","External"};
      }));
   ILIC_REQUIRE(std::any_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [&modelScope](const auto &node) {
         return node.kind == "function" && node.label == "RootFunction" &&
            node.containerId == modelScope->id &&
            node.stereotypes == std::vector<std::string>{"Function"};
      }));
   const auto topic = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) {
         return node.kind == "topic" && node.label == "Data (DiagramRoot)";
      });
   ILIC_REQUIRE(topic != diagram.diagram.nodes.end());
   ILIC_REQUIRE(topic->containerId.empty());
   ILIC_REQUIRE(topic->abstract);
   ILIC_REQUIRE((topic->stereotypes == std::vector<std::string>{"Abstract"}));
   const auto topicClass = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "class" && node.label == "TopicClass"; });
   const auto topicStruct = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "structure" && node.label == "TopicStruct"; });
   ILIC_REQUIRE(topicClass != diagram.diagram.nodes.end());
   ILIC_REQUIRE(topicClass->abstract);
   ILIC_REQUIRE(topicStruct != diagram.diagram.nodes.end());
   ILIC_REQUIRE(topicStruct->abstract);
   ILIC_REQUIRE((topicClass->stereotypes == std::vector<std::string>{"Abstract"}));
   ILIC_REQUIRE((topicStruct->stereotypes ==
      std::vector<std::string>{"Abstract","Structure"}));
   ILIC_REQUIRE(topicClass->containerId == topic->id);
   ILIC_REQUIRE(topicStruct->containerId == topic->id);
   ILIC_REQUIRE(std::any_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [&topic](const auto &node) {
         return node.kind == "view" && node.label == "TopicView" &&
            node.containerId == topic->id &&
            node.stereotypes == std::vector<std::string>{"Abstract","View"};
      }));
   ILIC_REQUIRE(std::any_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [&topic](const auto &node) {
         return node.kind == "function" && node.label == "TopicFunction" &&
            node.containerId == topic->id;
      }));
   ILIC_REQUIRE(std::none_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),[](const auto &node) {
      return node.id.find("DiagramLibrary.ImportedData") != std::string::npos ||
         node.id.find("DiagramLibrary.ImportedColors") != std::string::npos;
   }));
   ILIC_REQUIRE(std::any_of(diagram.symbols.begin(),diagram.symbols.end(),[](const auto &symbol) {
      return symbol.qualifiedName == "DiagramLibrary.ImportedColors";
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
   ILIC_REQUIRE(!missing.success);
   ILIC_REQUIRE(std::find(missing.missingModels.begin(),missing.missingModels.end(),
      "DoesNotExist") != missing.missingModels.end());

   ilic::CompilerSession invalidImportSession;
   const std::string invalidImportRootUri = "memory:///InvalidImportRoot.ili";
   const std::string invalidImportUri = "memory:///InvalidImport.ili";
   invalidImportSession.putSource(invalidImportRootUri,R"ili(INTERLIS 2.4;
MODEL InvalidImportRoot (en) AT "https://example.invalid" VERSION "1" =
  IMPORTS InvalidImport;
END InvalidImportRoot.
)ili",1);
   invalidImportSession.putSource(invalidImportUri,R"ili(INTERLIS 2.4;
MODEL InvalidImport (en) AT "https://example.invalid" VERSION "1" =
  TOPIC Data =
    CLASS Broken =
      Value : ;
    END Broken;
  END Data;
END InvalidImport.
)ili",2);
   ilic::CompilationRequest invalidImportRequest;
   invalidImportRequest.roots = {invalidImportRootUri};
   const auto invalidImport = invalidImportSession.compileAndAnalyze(invalidImportRequest);
   ILIC_REQUIRE(!invalidImport.compilation.success);
   ILIC_REQUIRE(!invalidImport.compilation.diagnostics.empty());
   ILIC_REQUIRE(std::any_of(invalidImport.syntax.begin(),invalidImport.syntax.end(),
      [&invalidImportUri](const auto &syntax) { return syntax.uri == invalidImportUri; }));
   return 0;
}
