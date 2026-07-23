#include "ilic/Compiler.h"
#include "ilic/Semantic.h"

#include <algorithm>
#include <cassert>
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
   const std::string unrelatedUri = "memory:///UnrelatedBroken.ili";
   session.putSource(unrelatedUri,"this is not INTERLIS",10);

   ilic::CompilationRequest request;
   request.roots = {uri};
   const auto compileCountBefore = ilic::CompilerSessionTestAccess::compileInvocationCount(session);
   const ilic::CompilationAnalysisResult analysis = session.compileAndAnalyze(request);
   assert(ilic::CompilerSessionTestAccess::compileInvocationCount(session) == compileCountBefore + 1);
   const ilic::SemanticSnapshot &snapshot = analysis.semantic;
   assert(snapshot.success);
   assert(analysis.compilation.success);
   const std::regex completion("^inf: ilic completed with no errors, no warnings [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$");
   assert(std::any_of(analysis.compilation.transcript.begin(),analysis.compilation.transcript.end(),
      [&completion](const auto &line) { return std::regex_match(line,completion); }));
   assert(std::none_of(analysis.compilation.transcript.begin(),analysis.compilation.transcript.end(),
      [](const auto &line) { return line.find("compiler run done") != std::string::npos; }));
   assert(snapshot.documentVersions.at(uri) == 9);
   assert(snapshot.documentVersions.count(unrelatedUri) == 0);
   assert(analysis.syntax.size() == 2);
   assert(std::any_of(analysis.syntax.begin(),analysis.syntax.end(),[&baseUri](const auto &syntax) {
      return syntax.uri == baseUri;
   }));
   assert(std::none_of(analysis.syntax.begin(),analysis.syntax.end(),[&unrelatedUri](const auto &syntax) {
      return syntax.uri == unrelatedUri;
   }));
   assert(snapshot.missingModels.empty());
   assert(std::any_of(snapshot.symbols.begin(),snapshot.symbols.end(),[](const auto &symbol) {
      return symbol.qualifiedName == "Semantic.Data.Item" && symbol.kind == "class" &&
         symbol.selectionRange.valid && symbol.selectionRange.start.line == 7 &&
         symbol.selectionRange.start.character == 10 && symbol.selectionRange.end.character == 14 &&
         symbol.endRange.valid && symbol.endRange.start.line == 9 &&
         symbol.endRange.start.character == 8 && symbol.endRange.end.character == 12;
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
      return edge.kind == "association" && edge.label == "left–right" &&
         edge.sourceCardinality == "1" && edge.targetCardinality == "0..*" &&
         edge.cardinality == "left 1 / right 0..*";
   }));
   assert(!snapshot.documentation.title.empty());

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
   assert(extended.success);
   const auto baseAttribute = std::find_if(extended.symbols.begin(),extended.symbols.end(),
      [](const auto &symbol) { return symbol.qualifiedName == "Extended.Data.Base.Name"; });
   const auto childAttribute = std::find_if(extended.symbols.begin(),extended.symbols.end(),
      [](const auto &symbol) { return symbol.qualifiedName == "Extended.Data.Child.Name"; });
   assert(baseAttribute != extended.symbols.end());
   assert(childAttribute != extended.symbols.end());
   assert(std::any_of(extended.references.begin(),extended.references.end(),
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
   assert(diagram.success);
   assert(std::any_of(diagram.symbols.begin(),diagram.symbols.end(),[](const auto &symbol) {
      return symbol.qualifiedName == "DiagramLibrary.ImportedData.ImportedClass";
   }));
   const auto modelScope = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "modelScope" && node.label == "Model Scope"; });
   assert(modelScope != diagram.diagram.nodes.end());
   assert(modelScope->containerId.empty());
   const auto rootColors = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "enumeration" && node.label == "RootColors"; });
   assert(rootColors != diagram.diagram.nodes.end());
   assert(rootColors->containerId == modelScope->id);
   assert((rootColors->stereotypes ==
      std::vector<std::string>{"Abstract","Enumeration"}));
   assert((rootColors->enumValues == std::vector<std::string>{"red","blue"}));
   const auto topicTree = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "enumeration" && node.label == "TopicTree"; });
   assert(topicTree != diagram.diagram.nodes.end());
   assert((topicTree->enumValues == std::vector<std::string>{"red","blue"}));
   const auto rootClass = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "class" && node.label == "RootClass"; });
   assert(rootClass != diagram.diagram.nodes.end());
   assert(rootClass->containerId == modelScope->id);
   assert((rootClass->stereotypes == std::vector<std::string>{"Abstract"}));
   assert(rootClass->members.size() == 4 && rootClass->members.front().type == "TEXT" &&
      rootClass->members.front().cardinality == "1");
   assert((rootClass->members[1].inlineEnumValues ==
      std::vector<std::string>{"open","closed","archived"}));
   assert(rootClass->members[2].type == "TEXT" &&
      rootClass->members[2].cardinality == "1..*");
   assert(rootClass->members[3].type == "ENUMERATION" &&
      rootClass->members[3].cardinality == "1..*" &&
      rootClass->members[3].inlineEnumValues ==
         std::vector<std::string>({"draft","final"}));
   assert((rootClass->operations ==
      std::vector<std::string>{"Named()","constraint1()"}));
   const auto rootChild = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "class" && node.label == "RootChild"; });
   assert(rootChild != diagram.diagram.nodes.end());
   assert(rootChild->members.size() == 5);
   assert(rootChild->members[1].inherited &&
      rootChild->members[1].declaringType == "RootClass");
   assert(std::any_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [&modelScope](const auto &node) {
         return node.kind == "external" && node.label == "ImportedBase" &&
            node.containerId == modelScope->id &&
            node.stereotypes ==
               std::vector<std::string>{"Abstract","External"};
      }));
   assert(std::any_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [&modelScope](const auto &node) {
         return node.kind == "function" && node.label == "RootFunction" &&
            node.containerId == modelScope->id &&
            node.stereotypes == std::vector<std::string>{"Function"};
      }));
   const auto topic = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) {
         return node.kind == "topic" && node.label == "Data (DiagramRoot)";
      });
   assert(topic != diagram.diagram.nodes.end());
   assert(topic->containerId.empty());
   assert(topic->abstract);
   assert((topic->stereotypes == std::vector<std::string>{"Abstract"}));
   const auto topicClass = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "class" && node.label == "TopicClass"; });
   const auto topicStruct = std::find_if(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [](const auto &node) { return node.kind == "structure" && node.label == "TopicStruct"; });
   assert(topicClass != diagram.diagram.nodes.end() && topicClass->abstract);
   assert(topicStruct != diagram.diagram.nodes.end() && topicStruct->abstract);
   assert((topicClass->stereotypes == std::vector<std::string>{"Abstract"}));
   assert((topicStruct->stereotypes ==
      std::vector<std::string>{"Abstract","Structure"}));
   assert(topicClass->containerId == topic->id);
   assert(topicStruct->containerId == topic->id);
   assert(std::any_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [&topic](const auto &node) {
         return node.kind == "view" && node.label == "TopicView" &&
            node.containerId == topic->id &&
            node.stereotypes == std::vector<std::string>{"Abstract","View"};
      }));
   assert(std::any_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),
      [&topic](const auto &node) {
         return node.kind == "function" && node.label == "TopicFunction" &&
            node.containerId == topic->id;
      }));
   assert(std::none_of(diagram.diagram.nodes.begin(),diagram.diagram.nodes.end(),[](const auto &node) {
      return node.id.find("DiagramLibrary.ImportedData") != std::string::npos ||
         node.id.find("DiagramLibrary.ImportedColors") != std::string::npos;
   }));
   assert(std::any_of(diagram.symbols.begin(),diagram.symbols.end(),[](const auto &symbol) {
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
   assert(!missing.success);
   assert(std::find(missing.missingModels.begin(),missing.missingModels.end(),
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
   assert(!invalidImport.compilation.success);
   assert(!invalidImport.compilation.diagnostics.empty());
   assert(std::any_of(invalidImport.syntax.begin(),invalidImport.syntax.end(),
      [&invalidImportUri](const auto &syntax) { return syntax.uri == invalidImportUri; }));
   return 0;
}
