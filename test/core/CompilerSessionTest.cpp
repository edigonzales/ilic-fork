#include "ilic/Compiler.h"

#include <algorithm>
#include "ilic/test/TestHarness.h"
#include <iostream>
#include <regex>

int main()
{
   ilic::CompilerSession session;
   const char *uri = "memory:///AnyClassRegression.ili";
   session.putSource(uri, R"ili(INTERLIS 2.3;

MODEL AnyClassRegression AT "https://example.invalid/ilic/tests" VERSION "1" =
  TOPIC Topic =
    STRUCTURE Holder =
      Ref : REFERENCE TO ANYCLASS;
    END Holder;
  END Topic;
END AnyClassRegression.
)ili", 7);

   ilic::CompilationRequest request;
   request.roots.push_back(uri);
   ilic::CompilationResult result = session.compile(request);
   for (const auto &diagnostic : result.diagnostics) {
      if (!result.success) std::cerr << diagnostic.code << ": " << diagnostic.message << "\n";
   }
   ILIC_REQUIRE(result.success);
   ILIC_REQUIRE(result.errorCount == 0);
   ILIC_REQUIRE(!result.models.empty());
   const auto &sources = static_cast<const ilic::CompilerSession &>(session).sources();
   ILIC_REQUIRE(sources.position(uri, 0).line == 0);

   const char *invalidUri = "memory:///UnknownDomain.ili";
   session.putSource(invalidUri, R"ili(INTERLIS 2.3;
MODEL UnknownDomain AT "https://example.invalid/ilic/tests" VERSION "1" =
  TOPIC Topic =
    CLASS Item =
      Value : MissingDomain;
    END Item;
  END Topic;
END UnknownDomain.
)ili");
   ilic::CompilationRequest invalidRequest;
   invalidRequest.roots.push_back(invalidUri);
   ilic::CompilationResult invalid = session.compile(invalidRequest);
   ILIC_REQUIRE(!invalid.success);
   ILIC_REQUIRE(!invalid.diagnostics.empty());
   const std::regex completion("^inf: ilic completed with [0-9]+ errors?, no warnings [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$");
   ILIC_REQUIRE(std::any_of(invalid.transcript.begin(),invalid.transcript.end(),
      [&completion](const auto &line) { return std::regex_match(line,completion); }));
   ILIC_REQUIRE(std::none_of(invalid.transcript.begin(),invalid.transcript.end(),
      [](const auto &line) { return line.find("compiler run failed") != std::string::npos; }));
   bool located = false;
   bool specificallyCoded = false;
   for (const auto &diagnostic : invalid.diagnostics) {
      if (diagnostic.range.valid && diagnostic.range.uri == invalidUri) located = true;
      if (diagnostic.code == "ILIC-NAME-TYPE-NOT-FOUND") specificallyCoded = true;
   }
   ILIC_REQUIRE(located);
   ILIC_REQUIRE(specificallyCoded);

   ilic::CompilerSession metaSession;
   const char *metaUri = "memory:///MetaAttributes.ili";
   metaSession.putSource(metaUri, R"ili(INTERLIS 2.3;
!!@ displayName = "A model with spaces"
!! this ordinary comment must not consume the attribute
/** documentation also remains between attribute and declaration */
MODEL MetaAttributes AT "https://example.invalid/ilic/tests" VERSION "1" =
END MetaAttributes.
)ili");
   ilic::CompilationRequest metaRequest;
   metaRequest.roots.push_back(metaUri);
   ilic::CompilationResult meta = metaSession.compile(metaRequest);
   ILIC_REQUIRE(meta.success);
   bool metaFound = false;
   for (const auto &model : meta.models) {
      if (model.name != "MetaAttributes") continue;
      for (const auto &attribute : model.metaAttributes) {
         if (attribute.name == "displayName" && attribute.value == "A model with spaces") metaFound = true;
      }
   }
   ILIC_REQUIRE(metaFound);

   ilic::CompilerSession translationSession;
   const char *translationUri = "memory:///ExternalTranslation.ili";
   translationSession.putSource(translationUri, R"ili(INTERLIS 2.3;
MODEL Base AT "https://example.invalid/ilic/tests" VERSION "1" =
END Base.
MODEL Translation AT "https://example.invalid/ilic/tests" VERSION "1" =
END Translation.
)ili");
   ilic::CompilationRequest translationRequest;
   translationRequest.roots.push_back(translationUri);
   translationRequest.externalMetaAttributes.push_back(
      {"Translation","ili2c.translationOf","Base"});
   ilic::CompilationResult translation = translationSession.compile(translationRequest);
   ILIC_REQUIRE(translation.success);

   ilic::CompilerSession diagnosticSession;
   const char *diagnosticUri = "memory:///DiagnosticQuality.ili";
   diagnosticSession.putSource(diagnosticUri,R"ili(INTERLIS 2.3;
MODEL ModelA (de) AT "https://example.invalid" VERSION "1" =
  TOPIC TopicA =
    CLASS A = END A;
    CLASS B = END B;
    ASSOCIATION Link (ABSTRACT) =
      left (ABSTRACT) -- A;
      right -- B;
    END Link;
  END TopicA;
END ModelA.
MODEL ModelB (fr) AT "https://example.invalid" VERSION "1"
TRANSLATION OF ModelA [ "1" ] =
  TOPIC TopicB =
    CLASS A = END A;
    CLASS B = END B;
    ASSOCIATION Link (ABSTRACT) =
      left -- A;
      right -- B;
    END Link;
  END TopicB;
END ModelB.
)ili");
   ilic::CompilationRequest diagnosticRequest;
   diagnosticRequest.roots.push_back(diagnosticUri);
   const ilic::CompilationResult diagnosticResult = diagnosticSession.compile(diagnosticRequest);
   ILIC_REQUIRE(!diagnosticResult.success);
   ILIC_REQUIRE(std::count_if(diagnosticResult.diagnostics.begin(),diagnosticResult.diagnostics.end(),
      [](const auto &diagnostic) {
         return diagnostic.code == "ILIC-TRANSLATION-ABSTRACT-MISMATCH" &&
            diagnostic.message.find("nullptr") == std::string::npos &&
            diagnostic.message.find("left") != std::string::npos &&
            diagnostic.range.valid && !diagnostic.relatedInformation.empty() &&
            diagnostic.relatedInformation.front().range.valid;
      }) == 1);

   ilic::CompilerSession anonymousAssociationSession;
   const char *anonymousUri = "memory:///AnonymousAssociation.ili";
   anonymousAssociationSession.putSource(anonymousUri,R"ili(INTERLIS 2.3;
MODEL AnonymousAssociation AT "https://example.invalid" VERSION "1" =
  TOPIC Topic =
    CLASS A = END A;
    CLASS B = END B;
    ASSOCIATION =
      duplicate -- A;
      duplicate -- B;
    END;
  END Topic;
END AnonymousAssociation.
)ili");
   ilic::CompilationRequest anonymousRequest;
   anonymousRequest.roots.push_back(anonymousUri);
   const auto anonymousResult = anonymousAssociationSession.compile(anonymousRequest);
   ILIC_REQUIRE(!anonymousResult.success);
   ILIC_REQUIRE(std::any_of(anonymousResult.diagnostics.begin(),anonymousResult.diagnostics.end(),
      [](const auto &diagnostic) {
         return diagnostic.code == "ILIC-ASSOCIATION-DUPLICATE-ROLE" &&
            diagnostic.message.find("anonymous association") != std::string::npos &&
            diagnostic.message.find("???") == std::string::npos;
      }));

   ilic::CompilerSession invalidAssociationSession;
   const char *invalidAssociationUri = "memory:///InvalidAssociationBase.ili";
   invalidAssociationSession.putSource(invalidAssociationUri,R"ili(INTERLIS 2.3;
MODEL InvalidAssociationBase AT "https://example.invalid" VERSION "1" =
  TOPIC Topic =
    CLASS A = END A;
    CLASS B = END B;
    CLASS NotAnAssociation = END NotAnAssociation;
    ASSOCIATION Link EXTENDS NotAnAssociation =
      left -- A;
      right -- B;
    END Link;
  END Topic;
END InvalidAssociationBase.
)ili");
   ilic::CompilationRequest invalidAssociationRequest;
   invalidAssociationRequest.roots.push_back(invalidAssociationUri);
   const auto invalidAssociationResult =
      invalidAssociationSession.compile(invalidAssociationRequest);
   ILIC_REQUIRE(!invalidAssociationResult.success);
   ILIC_REQUIRE(std::count_if(
      invalidAssociationResult.diagnostics.begin(),
      invalidAssociationResult.diagnostics.end(),
      [](const auto &diagnostic) {
         return diagnostic.severity == ilic::DiagnosticSeverity::Error;
      }) == 1);
   ILIC_REQUIRE(invalidAssociationResult.diagnostics.size() == 1);
   const auto &invalidAssociationDiagnostic =
      invalidAssociationResult.diagnostics.front();
   ILIC_REQUIRE(invalidAssociationDiagnostic.code
      == "ILIC-ASSOCIATION-INVALID-BASE-KIND");
   ILIC_REQUIRE(invalidAssociationDiagnostic.message.find("NotAnAssociation")
      != std::string::npos);
   ILIC_REQUIRE(invalidAssociationDiagnostic.message.find("CLASS")
      != std::string::npos);
   ILIC_REQUIRE(invalidAssociationDiagnostic.message.find("ASSOCIATION")
      != std::string::npos);
   ILIC_REQUIRE(invalidAssociationDiagnostic.range.valid);
   ILIC_REQUIRE(invalidAssociationDiagnostic.range.start.line == 6);
   ILIC_REQUIRE(invalidAssociationDiagnostic.relatedInformation.size() == 1);
   ILIC_REQUIRE(invalidAssociationDiagnostic.relatedInformation.front().range.valid);
   ILIC_REQUIRE(invalidAssociationDiagnostic.relatedInformation.front().range.start.line == 5);

   ilic::CompilerSession attributeExtensionSession;
   const char *attributeExtensionUri =
      "memory:///IncompatibleAttributeExtension.ili";
   attributeExtensionSession.putSource(attributeExtensionUri,R"ili(INTERLIS 2.3;
MODEL IncompatibleAttributeExtension AT "https://example.invalid" VERSION "1" =
  DOMAIN Height = 0.0 .. 100.0;
  DOMAIN Position = COORD 0.0 .. 100.0, 0.0 .. 100.0;
  TOPIC Topic =
    CLASS Base = Value : Height; END Base;
    CLASS Derived EXTENDS Base =
      Value (EXTENDED) : Position;
    END Derived;
  END Topic;
END IncompatibleAttributeExtension.
)ili");
   ilic::CompilationRequest attributeExtensionRequest;
   attributeExtensionRequest.roots.push_back(attributeExtensionUri);
   const auto attributeExtensionResult =
      attributeExtensionSession.compile(attributeExtensionRequest);
   ILIC_REQUIRE(!attributeExtensionResult.success);
   ILIC_REQUIRE(attributeExtensionResult.errorCount == 1);
   ILIC_REQUIRE(attributeExtensionResult.diagnostics.size() == 1);
   const auto &attributeExtensionDiagnostic =
      attributeExtensionResult.diagnostics.front();
   ILIC_REQUIRE(attributeExtensionDiagnostic.code
      == "ILIC-ATTRIBUTE-INCOMPATIBLE-EXTENSION");
   ILIC_REQUIRE(attributeExtensionDiagnostic.message.find("Position")
      != std::string::npos);
   ILIC_REQUIRE(attributeExtensionDiagnostic.message.find("Height")
      != std::string::npos);
   ILIC_REQUIRE(attributeExtensionDiagnostic.range.valid);
   ILIC_REQUIRE(attributeExtensionDiagnostic.range.start.line == 7);
   ILIC_REQUIRE(attributeExtensionDiagnostic.relatedInformation.size() == 2);

   ilic::CompilerSession classExtensionSession;
   const char *classExtensionUri = "memory:///ClassExtendedRequired.ili";
   classExtensionSession.putSource(classExtensionUri,R"ili(INTERLIS 2.3;
MODEL ClassExtendedRequired AT "https://example.invalid" VERSION "1" =
  TOPIC BaseTopic =
    CLASS Item = END Item;
  END BaseTopic;
  TOPIC DerivedTopic EXTENDS BaseTopic =
    CLASS Item EXTENDS ClassExtendedRequired.BaseTopic.Item =
    END Item;
  END DerivedTopic;
END ClassExtendedRequired.
)ili");
   ilic::CompilationRequest classExtensionRequest;
   classExtensionRequest.roots.push_back(classExtensionUri);
   const auto classExtensionResult =
      classExtensionSession.compile(classExtensionRequest);
   ILIC_REQUIRE(!classExtensionResult.success);
   ILIC_REQUIRE(classExtensionResult.errorCount == 1);
   ILIC_REQUIRE(classExtensionResult.diagnostics.size() == 1);
   const auto &classExtensionDiagnostic =
      classExtensionResult.diagnostics.front();
   ILIC_REQUIRE(classExtensionDiagnostic.code == "ILIC-CLASS-EXTENDED-REQUIRED");
   ILIC_REQUIRE(classExtensionDiagnostic.message.find("DerivedTopic")
      != std::string::npos);
   ILIC_REQUIRE(classExtensionDiagnostic.message.find("BaseTopic")
      != std::string::npos);
   ILIC_REQUIRE(classExtensionDiagnostic.message.find("EXTENDED")
      != std::string::npos);
   ILIC_REQUIRE(classExtensionDiagnostic.range.valid);
   ILIC_REQUIRE(classExtensionDiagnostic.range.start.line == 6);
   ILIC_REQUIRE(classExtensionDiagnostic.relatedInformation.size() == 1);

   struct NamespaceCase {
      const char *uri;
      const char *source;
      size_t duplicateLine;
      std::string firstKind;
      std::string secondKind;
   };
   const NamespaceCase namespaceCases[] = {
      {
         "memory:///DuplicateTopic.ili",
         R"ili(INTERLIS 2.3;
MODEL DuplicateTopic AT "https://example.invalid" VERSION "1" =
  TOPIC Topic = END Topic;
  TOPIC Topic = END Topic;
END DuplicateTopic.
)ili",
         3,"TOPIC","TOPIC"
      },
      {
         "memory:///ClassDomainCollision.ili",
         R"ili(INTERLIS 2.3;
MODEL ClassDomainCollision AT "https://example.invalid" VERSION "1" =
  TOPIC Topic =
    DOMAIN Item = TEXT*3;
    CLASS Item = END Item;
  END Topic;
END ClassDomainCollision.
)ili",
         4,"DOMAIN","CLASS"
      },
      {
         "memory:///DuplicateClass.ili",
         R"ili(INTERLIS 2.3;
MODEL DuplicateClass AT "https://example.invalid" VERSION "1" =
  TOPIC Topic =
    CLASS Item = END Item;
    CLASS Item = END Item;
  END Topic;
END DuplicateClass.
)ili",
         4,"CLASS","CLASS"
      }
   };
   for (const auto &namespaceCase : namespaceCases) {
      ilic::CompilerSession namespaceSession;
      namespaceSession.putSource(namespaceCase.uri,namespaceCase.source);
      ilic::CompilationRequest namespaceRequest;
      namespaceRequest.roots.push_back(namespaceCase.uri);
      const auto namespaceResult = namespaceSession.compile(namespaceRequest);
      ILIC_REQUIRE(!namespaceResult.success);
      ILIC_REQUIRE(namespaceResult.errorCount == 1);
      ILIC_REQUIRE(namespaceResult.diagnostics.size() == 1);
      const auto &namespaceDiagnostic = namespaceResult.diagnostics.front();
      ILIC_REQUIRE(namespaceDiagnostic.code
         == "ILIC-NAMESPACE-DUPLICATE-DECLARATION");
      ILIC_REQUIRE(namespaceDiagnostic.message.find(namespaceCase.firstKind)
         != std::string::npos);
      ILIC_REQUIRE(namespaceDiagnostic.message.find(namespaceCase.secondKind)
         != std::string::npos);
      ILIC_REQUIRE(namespaceDiagnostic.range.valid);
      ILIC_REQUIRE(namespaceDiagnostic.range.start.line == namespaceCase.duplicateLine);
      ILIC_REQUIRE(namespaceDiagnostic.relatedInformation.size() == 1);
      ILIC_REQUIRE(namespaceDiagnostic.relatedInformation.front().range.valid);
      ILIC_REQUIRE(namespaceDiagnostic.relatedInformation.front().range.start.line
         < namespaceDiagnostic.range.start.line);
   }
   return 0;
}
