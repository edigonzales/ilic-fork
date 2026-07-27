#include "ilic/Compiler.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace {

ilic::CompilationResult compile(const char *uri,const char *source)
{
   ilic::CompilerSession session;
   session.putSource(uri,source);
   ilic::CompilationRequest request;
   request.roots.push_back(uri);
   return session.compile(request);
}

ilic::CompilationResult compile(
   const std::map<std::string,std::string> &sources,
   const std::vector<std::string> &roots
)
{
   ilic::CompilerSession session;
   for (const auto &[uri,source] : sources) session.putSource(uri,source);
   ilic::CompilationRequest request;
   request.roots = roots;
   return session.compile(request);
}

const ilic::Diagnostic &assert_single_diagnostic(
   const ilic::CompilationResult &result,
   const std::string &code,
   size_t line,
   std::initializer_list<std::string> messageParts,
   size_t minimumRelatedInformation = 1
)
{
   if (result.errorCount != 1 || result.diagnostics.size() != 1) {
      std::cerr << "Expected one " << code << " diagnostic, got "
                << result.errorCount << " errors and "
                << result.diagnostics.size() << " diagnostics\n";
      for (const auto &diagnostic : result.diagnostics) {
         std::cerr << diagnostic.code << ": " << diagnostic.message << "\n";
      }
   }
   assert(!result.success);
   assert(result.errorCount == 1);
   assert(result.diagnostics.size() == 1);
   const auto &diagnostic = result.diagnostics.front();
   assert(diagnostic.code == code);
   assert(diagnostic.range.valid);
   if (diagnostic.range.start.line != line) {
      std::cerr << code << " expected zero-based line " << line
                << ", got " << diagnostic.range.start.line << "\n";
   }
   assert(diagnostic.range.start.line == line);
   assert(diagnostic.relatedInformation.size() >= minimumRelatedInformation);
   for (const auto &part : messageParts) {
      if (diagnostic.message.find(part) == std::string::npos) {
         std::cerr << code << " message does not contain \"" << part
                   << "\": " << diagnostic.message << "\n";
      }
      assert(diagnostic.message.find(part) != std::string::npos);
   }
   assert(diagnostic.message.find("TOP") == std::string::npos);
   assert(diagnostic.message.find("nullptr") == std::string::npos);
   return diagnostic;
}

} // namespace

int main()
{
   const char *associationUri = "memory:///DerivedAssociationMismatch.ili";
   const auto association = compile(associationUri,R"ili(INTERLIS 2.3;
MODEL ModelA (de) AT "https://example.invalid" VERSION "1" =
  TOPIC TopicA =
    CLASS ClassA0 = END ClassA0;
    CLASS ClassA1 = END ClassA1;
    CLASS ClassA2 = END ClassA2;
    VIEW IntersectionA1
      JOIN OF A0~ClassA0,A1~ClassA1;
    = END IntersectionA1;
    VIEW IntersectionA2
      JOIN OF A0~ClassA0,A1~ClassA2;
    = END IntersectionA2;
    ASSOCIATION assocA4 DERIVED FROM IntersectionA1 =
      a4_0 -- ClassA0 := IntersectionA1 -> A0;
      a4_1 -- ClassA1 := IntersectionA1 -> A1;
    END assocA4;
  END TopicA;
END ModelA.
MODEL ModelB (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF ModelA [ "1" ] =
  TOPIC TopicB =
    CLASS ClassB0 = END ClassB0;
    CLASS ClassB1 = END ClassB1;
    CLASS ClassB2 = END ClassB2;
    VIEW IntersectionB1
      JOIN OF B0~ClassB0,B1~ClassB1;
    = END IntersectionB1;
    VIEW IntersectionB2
      JOIN OF B0~ClassB0,B1~ClassB2;
    = END IntersectionB2;
    ASSOCIATION assocB4 DERIVED FROM IntersectionB2 =
      b4_0 -- ClassB0 := IntersectionB2 -> B0;
      b4_1 -- ClassB1 := IntersectionB2 -> B1;
    END assocB4;
  END TopicB;
END ModelB.
)ili");
   const auto &associationDiagnostic = assert_single_diagnostic(
      association,
      "ILIC-TRANSLATION-DERIVED-ASSOCIATION-MISMATCH",
      30,
      {"assocB4","IntersectionB2","assocA4","IntersectionA1"},
      3
   );
   assert(associationDiagnostic.relatedInformation.front().range.uri
      == associationUri);

   const char *coordinateUri = "memory:///CoordinateDimensionMismatch.ili";
   const auto coordinate = compile(coordinateUri,R"ili(INTERLIS 2.3;
MODEL ModelA (de) AT "https://example.invalid" VERSION "1" =
  TOPIC TopicA =
    DOMAIN CoordA2 = COORD
      460000.000 .. 870000.000,
      45000.000 .. 310000.000,
      -200.000 .. 5000.000;
  END TopicA;
END ModelA.
MODEL ModelB (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF ModelA [ "1" ] =
  TOPIC TopicB =
    DOMAIN CoordB2 = COORD
      460000.000 .. 870000.000,
      45000.000 .. 310000.000;
  END TopicB;
END ModelB.
)ili");
   assert_single_diagnostic(
      coordinate,
      "ILIC-TRANSLATION-COORD-DIMENSION-MISMATCH",
      12,
      {"CoordB2","2 dimensions","CoordA2","3 dimensions"}
   );

   const char *enumerationUri = "memory:///FinalEnumerationMismatch.ili";
   const auto enumeration = compile(enumerationUri,R"ili(INTERLIS 2.3;
MODEL ModelA (de) AT "https://example.invalid" VERSION "1" =
  TOPIC TopicA =
    CLASS ClassA1 = attrA1 : (a1,a2(a21,a22)); END ClassA1;
    CLASS ClassA2 EXTENDS ClassA1 =
      attrA1 (EXTENDED): (FINAL);
    END ClassA2;
  END TopicA;
END ModelA.
MODEL ModelB (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF ModelA [ "1" ] =
  TOPIC TopicB =
    CLASS ClassB1 = attrB1 : (b1,b2(b21,b22)); END ClassB1;
    CLASS ClassB2 EXTENDS ClassB1 =
      attrB1 (EXTENDED): (b1(b11));
    END ClassB2;
  END TopicB;
END ModelB.
)ili");
   assert_single_diagnostic(
      enumeration,
      "ILIC-TRANSLATION-ENUM-FINAL-MISMATCH",
      14,
      {"attrB1","b1","attrA1","final enumeration"}
   );

   const char *horizontalUri = "memory:///HorizontalAlignmentMismatch.ili";
   const auto horizontal = compile(horizontalUri,R"ili(INTERLIS 2.3;
MODEL ModelA (de) AT "https://example.invalid" VERSION "1" =
  TOPIC TopicA =
    CLASS ClassA1 = attrA3 : HALIGNMENT; END ClassA1;
  END TopicA;
END ModelA.
MODEL ModelB (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF ModelA [ "1" ] =
  TOPIC TopicB =
    CLASS ClassB1 = attrB3 : VALIGNMENT; END ClassB1;
  END TopicB;
END ModelB.
)ili");
   const auto &horizontalDiagnostic = assert_single_diagnostic(
      horizontal,
      "ILIC-TRANSLATION-DOMAIN-REFERENCE-MISMATCH",
      9,
      {"attrB3","INTERLIS.VALIGNMENT","attrA3","INTERLIS.HALIGNMENT"},
      3
   );
   assert(horizontalDiagnostic.relatedInformation.front().range.uri
      == horizontalUri);

   const char *verticalUri = "memory:///VerticalAlignmentMismatch.ili";
   const auto vertical = compile(verticalUri,R"ili(INTERLIS 2.3;
MODEL ModelA (de) AT "https://example.invalid" VERSION "1" =
  TOPIC TopicA =
    CLASS ClassA1 = attrA3 : VALIGNMENT; END ClassA1;
  END TopicA;
END ModelA.
MODEL ModelB (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF ModelA [ "1" ] =
  TOPIC TopicB =
    CLASS ClassB1 = attrB3 : HALIGNMENT; END ClassB1;
  END TopicB;
END ModelB.
)ili");
   assert_single_diagnostic(
      vertical,
      "ILIC-TRANSLATION-DOMAIN-REFERENCE-MISMATCH",
      9,
      {"attrB3","INTERLIS.HALIGNMENT","attrA3","INTERLIS.VALIGNMENT"},
      3
   );

   const char *genericRangeUri = "memory:///GenericCoordinateRangeMismatch.ili";
   const auto genericRange = compile(genericRangeUri,R"ili(INTERLIS 2.4;
MODEL GenericCoordinateRangeMismatch AT "https://example.invalid" VERSION "1" =
  DOMAIN
    GenericCoord (GENERIC) = COORD 0 .. 100, NUMERIC;
    ConcreteCoord = COORD 0 .. 200, 0 .. 200;
  CONTEXT default =
    GenericCoord = ConcreteCoord;
  TOPIC Topic =
    CLASS Item = Position : GenericCoord; END Item;
  END Topic;
END GenericCoordinateRangeMismatch.
)ili");
   const auto &genericRangeDiagnostic = assert_single_diagnostic(
      genericRange,
      "ILIC-GENERIC-COORD-RANGE-MISMATCH",
      6,
      {"ConcreteCoord","GenericCoord","axis 1","maximum 200",
       "allowed maximum 100"},
      2
   );
   assert(genericRangeDiagnostic.relatedInformation.front().range.uri
      == genericRangeUri);
   assert(genericRangeDiagnostic.relatedInformation[1].range.uri
      == genericRangeUri);

   const char *runtimeParameterUri = "memory:///RuntimeParameterMismatch.ili";
   const auto runtimeParameter = compile(runtimeParameterUri,R"ili(INTERLIS 2.3;
MODEL ModelA (de) AT "https://example.invalid" VERSION "1" =
  PARAMETER paramA : MANDATORY TEXT * 20;
END ModelA.
MODEL ModelB (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF ModelA [ "1" ] =
  PARAMETER paramB : TEXT * 20;
END ModelB.
)ili");
   const auto &runtimeParameterDiagnostic = assert_single_diagnostic(
      runtimeParameter,
      "ILIC-TRANSLATION-TYPE-PROPERTY-MISMATCH",
      6,
      {"paramB","paramA","mandatory"}
   );
   assert(runtimeParameterDiagnostic.range.uri == runtimeParameterUri);
   assert(runtimeParameterDiagnostic.range.start.character == 12);
   assert(runtimeParameterDiagnostic.range.end.character == 18);
   assert(runtimeParameterDiagnostic.relatedInformation.front().range.uri
      == runtimeParameterUri);
   assert(runtimeParameterDiagnostic.relatedInformation.front().range.start.line == 2);
   assert(runtimeParameterDiagnostic.relatedInformation.front().range.start.character == 12);
   assert(runtimeParameterDiagnostic.relatedInformation.front().range.end.character == 18);

   const char *duplicateRuntimeParameterUri =
      "memory:///DuplicateRuntimeParameter.ili";
   const auto duplicateRuntimeParameter = compile(
      duplicateRuntimeParameterUri,R"ili(INTERLIS 2.3;
MODEL DuplicateRuntimeParameter AT "https://example.invalid" VERSION "1" =
  PARAMETER
    repeated : BOOLEAN;
    repeated : TEXT * 20;
END DuplicateRuntimeParameter.
)ili");
   const auto &duplicateRuntimeParameterDiagnostic = assert_single_diagnostic(
      duplicateRuntimeParameter,
      "ILIC-MODEL-RUNTIME-PARAMETER-DUPLICATE",
      4,
      {"repeated","DuplicateRuntimeParameter"},
      0
   );
   assert(duplicateRuntimeParameterDiagnostic.range.uri
      == duplicateRuntimeParameterUri);
   assert(duplicateRuntimeParameterDiagnostic.range.start.character == 4);
   assert(duplicateRuntimeParameterDiagnostic.range.end.character == 12);

   const char *derivedViewAttributeUri =
      "memory:///DerivedViewAttributeMismatch.ili";
   const auto derivedViewAttribute = compile(
      derivedViewAttributeUri,R"ili(INTERLIS 2.3;
MODEL ModelA (de) AT "https://example.invalid" VERSION "1" =
  TOPIC TopicA =
    CLASS ClassA0 = valueA : 1 .. 10; END ClassA0;
    VIEW ViewA0 PROJECTION OF A0~ClassA0; = END ViewA0;
    CLASS ClassA1 EXTENDS ClassA0 = END ClassA1;
    CLASS ClassA2 EXTENDS ClassA0 = END ClassA2;
    VIEW ViewA1 EXTENDS ViewA0
      BASE A0 EXTENDED BY A0p~ClassA1 =
    END ViewA1;
  END TopicA;
END ModelA.
MODEL ModelB (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF ModelA [ "1" ] =
  TOPIC TopicB =
    CLASS ClassB0 = valueB : 1 .. 10; END ClassB0;
    VIEW ViewB0 PROJECTION OF B0~ClassB0; = END ViewB0;
    CLASS ClassB1 EXTENDS ClassB0 = END ClassB1;
    CLASS ClassB2 EXTENDS ClassB0 = END ClassB2;
    VIEW ViewB1 EXTENDS ViewB0
      BASE B0 EXTENDED BY B0p~ClassB2 =
    END ViewB1;
  END TopicB;
END ModelB.
)ili");
   const auto &derivedViewAttributeDiagnostic = assert_single_diagnostic(
      derivedViewAttribute,
      "ILIC-TRANSLATION-REFERENCE-MISMATCH",
      20,
      {"B0p","A0p","class reference"}
   );
   assert(derivedViewAttributeDiagnostic.range.uri == derivedViewAttributeUri);
   assert(derivedViewAttributeDiagnostic.relatedInformation.front().range.uri
      == derivedViewAttributeUri);
   assert(derivedViewAttributeDiagnostic.relatedInformation.front().range.start.line == 8);

   const char *viewDependencyUri = "memory:///ViewDependencyLocation.ili";
   const auto viewDependency = compile(viewDependencyUri,R"ili(INTERLIS 2.3;
MODEL ViewDependencyLocation AT "https://example.invalid" VERSION "1" =
  TOPIC Base =
    CLASS Item =
    END Item;
  END Base;
  TOPIC Usage =
    VIEW Items
      PROJECTION OF ViewDependencyLocation.Base.Item;
      =
    END Items;
  END Usage;
END ViewDependencyLocation.
)ili");
   const auto &viewDependencyDiagnostic = assert_single_diagnostic(
      viewDependency,
      "ILIC-TOPIC-DEPENDENCY-REQUIRED",
      8,
      {"view base","Usage","Base"}
   );
   assert(viewDependencyDiagnostic.range.uri == viewDependencyUri);
   assert(viewDependencyDiagnostic.range.start.character == 48);
   assert(viewDependencyDiagnostic.range.end.character == 52);
   assert(viewDependencyDiagnostic.relatedInformation.front().range.uri
      == viewDependencyUri);
   assert(viewDependencyDiagnostic.relatedInformation.front().range.start.line == 2);

   const auto fileBackedViewDependencyUri =
      std::filesystem::temp_directory_path() /
      "ilic_file_backed_diagnostic_quality.ili";
   {
      std::ofstream source(fileBackedViewDependencyUri);
      source << R"ili(INTERLIS 2.3;
MODEL FileBackedLocation AT "https://example.invalid" VERSION "1" =
  TOPIC Base =
    CLASS Item =
    END Item;
  END Base;
  TOPIC Usage =
    VIEW Items
      PROJECTION OF FileBackedLocation.Base.Item;
      =
    END Items;
  END Usage;
END FileBackedLocation.
)ili";
   }
   ilic::CompilerSession fileBackedSession;
   ilic::CompilationRequest fileBackedRequest;
   fileBackedRequest.roots.push_back(fileBackedViewDependencyUri.string());
   const auto fileBackedViewDependency = fileBackedSession.compile(fileBackedRequest);
   std::filesystem::remove(fileBackedViewDependencyUri);
   const auto &fileBackedViewDependencyDiagnostic = assert_single_diagnostic(
      fileBackedViewDependency,
      "ILIC-TOPIC-DEPENDENCY-REQUIRED",
      8,
      {"view base","Usage","Base"}
   );
   assert(fileBackedViewDependencyDiagnostic.range.uri
      == fileBackedViewDependencyUri.string());
   assert(fileBackedViewDependencyDiagnostic.range.start.character == 46);
   assert(fileBackedViewDependencyDiagnostic.range.end.character == 50);

   const char *viewNamespaceUri = "memory:///ViewNamespaceLocation.ili";
   const auto viewNamespace = compile(viewNamespaceUri,R"ili(INTERLIS 2.3;
MODEL ViewNamespaceLocation AT "https://example.invalid" VERSION "1" =
  TOPIC Base =
    CLASS Item =
      base : TEXT * 20;
    END Item;
  END Base;
  TOPIC Usage =
    DEPENDS ON Base;
    VIEW Items
      PROJECTION OF base ~ ViewNamespaceLocation.Base.Item;
      =
      ATTRIBUTE
        ALL OF base;
    END Items;
  END Usage;
END ViewNamespaceLocation.
)ili");
   const auto &viewNamespaceDiagnostic = assert_single_diagnostic(
      viewNamespace,
      "ILIC-NAMESPACE-MEMBER-DUPLICATE",
      13,
      {"base","Items"}
   );
   assert(viewNamespaceDiagnostic.range.uri == viewNamespaceUri);
   assert(viewNamespaceDiagnostic.range.start.character == 15);
   assert(viewNamespaceDiagnostic.range.end.character == 19);
   assert(viewNamespaceDiagnostic.relatedInformation.front().range.uri
      == viewNamespaceUri);
   assert(viewNamespaceDiagnostic.relatedInformation.front().range.start.line == 10);

   const std::string translationBaseUri = "memory:///translation/Base.ili";
   const std::string translationUri = "memory:///translation/Translated.ili";
   const std::map<std::string,std::string> translationSources{
      {translationBaseUri,R"ili(INTERLIS 2.3;
MODEL BaseModel (de) AT "https://example.invalid" VERSION "1" =
  TOPIC BaseTopic =
    CLASS BaseClass = Value : TEXT * 20; END BaseClass;
  END BaseTopic;
END BaseModel.
)ili"},
      {translationUri,R"ili(INTERLIS 2.3;
MODEL TranslatedModel (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF BaseModel [ "1" ] =
  TOPIC TranslatedTopic =
    CLASS TranslatedClass = ValueTranslated : TEXT * 30; END TranslatedClass;
  END TranslatedTopic;
END TranslatedModel.
)ili"}
   };
   for (const auto &roots : std::vector<std::vector<std::string>>{
      {translationBaseUri,translationUri},
      {translationUri,translationBaseUri}}) {
      const auto result = compile(translationSources,roots);
      const auto &diagnostic = assert_single_diagnostic(
         result,"ILIC-TRANSLATION-TYPE-PROPERTY-MISMATCH",4,
         {"ValueTranslated","Value","text length"});
      assert(diagnostic.range.uri == translationUri);
      assert(diagnostic.relatedInformation.front().range.uri == translationBaseUri);
      assert(diagnostic.range.start.character == 28);
      assert(diagnostic.range.end.character == 43);
      assert(diagnostic.relatedInformation.front().range.start.line == 3);
      assert(diagnostic.relatedInformation.front().range.start.character == 22);
      assert(diagnostic.relatedInformation.front().range.end.line == 3);
      assert(diagnostic.relatedInformation.front().range.end.character == 27);
   }

   const std::string inheritanceBaseUri = "memory:///inheritance/Base.ili";
   const std::string inheritanceUri = "memory:///inheritance/Derived.ili";
   const std::map<std::string,std::string> inheritanceSources{
      {inheritanceBaseUri,R"ili(INTERLIS 2.3;
MODEL BaseTypes AT "https://example.invalid" VERSION "1" =
  CLASS BaseClass (ABSTRACT) =
    Value : TEXT * 20;
  END BaseClass;
END BaseTypes.
)ili"},
      {inheritanceUri,R"ili(INTERLIS 2.3;
MODEL DerivedTypes AT "https://example.invalid" VERSION "1" =
  IMPORTS BaseTypes;
  CLASS Child (ABSTRACT) EXTENDS BaseTypes.BaseClass =
    Value (EXTENDED) : NUMERIC;
  END Child;
END DerivedTypes.
)ili"}
   };
   for (const auto &roots : std::vector<std::vector<std::string>>{
      {inheritanceBaseUri,inheritanceUri},
      {inheritanceUri,inheritanceBaseUri}}) {
      const auto result = compile(inheritanceSources,roots);
      const auto &diagnostic = assert_single_diagnostic(
         result,"ILIC-ATTRIBUTE-INCOMPATIBLE-EXTENSION",4,
         {"Value","NUMERIC","TEXT"});
      assert(diagnostic.range.uri == inheritanceUri);
      assert(diagnostic.relatedInformation.front().range.uri == inheritanceBaseUri);
      assert(diagnostic.range.start.character == 4);
      assert(diagnostic.range.end.character == 9);
      assert(diagnostic.relatedInformation.front().range.start.line == 3);
      assert(diagnostic.relatedInformation.front().range.start.character == 4);
      assert(diagnostic.relatedInformation.front().range.end.line == 3);
      assert(diagnostic.relatedInformation.front().range.end.character == 9);
   }

   const std::string genericBaseUri = "memory:///generic/Base.ili";
   const std::string genericUri = "memory:///generic/Concrete.ili";
   const std::map<std::string,std::string> genericSources{
      {genericBaseUri,R"ili(INTERLIS 2.4;
MODEL GenericBase AT "https://example.invalid" VERSION "1" =
  DOMAIN GenericCoord (GENERIC) = COORD 0 .. 100, NUMERIC;
END GenericBase.
)ili"},
      {genericUri,R"ili(INTERLIS 2.4;
MODEL ConcreteModel AT "https://example.invalid" VERSION "1" =
  IMPORTS GenericBase;
  DOMAIN ConcreteCoord = COORD 0 .. 200, 0 .. 200;
  CONTEXT default =
    GenericBase.GenericCoord = ConcreteCoord;
END ConcreteModel.
)ili"}
   };
   for (const auto &roots : std::vector<std::vector<std::string>>{
      {genericBaseUri,genericUri},
      {genericUri,genericBaseUri}}) {
      const auto result = compile(genericSources,roots);
      const auto &diagnostic = assert_single_diagnostic(
         result,"ILIC-GENERIC-COORD-RANGE-MISMATCH",5,
         {"ConcreteCoord","GenericCoord","axis 1","maximum 200",
          "allowed maximum 100"},2);
      assert(diagnostic.range.uri == genericUri);
      assert(diagnostic.relatedInformation[0].range.uri == genericUri);
      assert(diagnostic.relatedInformation[1].range.uri == genericBaseUri);
      assert(diagnostic.relatedInformation[0].range.start.line == 3);
      assert(diagnostic.relatedInformation[0].range.start.character == 9);
      assert(diagnostic.relatedInformation[0].range.end.character == 22);
      assert(diagnostic.relatedInformation[1].range.start.line == 2);
      assert(diagnostic.relatedInformation[1].range.start.character == 9);
      assert(diagnostic.relatedInformation[1].range.end.character == 21);
   }
   return 0;
}
