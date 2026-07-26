#include "ilic/Compiler.h"

#include <algorithm>
#include <cassert>
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
   assert(result.success);
   assert(result.errorCount == 0);
   assert(!result.models.empty());
   assert(session.sources().position(uri, 0).line == 0);

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
   assert(!invalid.success);
   assert(!invalid.diagnostics.empty());
   const std::regex completion("^inf: ilic completed with [0-9]+ errors?, no warnings [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$");
   assert(std::any_of(invalid.transcript.begin(),invalid.transcript.end(),
      [&completion](const auto &line) { return std::regex_match(line,completion); }));
   assert(std::none_of(invalid.transcript.begin(),invalid.transcript.end(),
      [](const auto &line) { return line.find("compiler run failed") != std::string::npos; }));
   bool located = false;
   bool specificallyCoded = false;
   for (const auto &diagnostic : invalid.diagnostics) {
      if (diagnostic.range.valid && diagnostic.range.uri == invalidUri) located = true;
      if (diagnostic.code == "ILIC-NAME-TYPE-NOT-FOUND") specificallyCoded = true;
   }
   assert(located);
   assert(specificallyCoded);

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
   assert(meta.success);
   bool metaFound = false;
   for (const auto &model : meta.models) {
      if (model.name != "MetaAttributes") continue;
      for (const auto &attribute : model.metaAttributes) {
         if (attribute.name == "displayName" && attribute.value == "A model with spaces") metaFound = true;
      }
   }
   assert(metaFound);

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
   assert(translation.success);

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
   assert(!diagnosticResult.success);
   assert(std::count_if(diagnosticResult.diagnostics.begin(),diagnosticResult.diagnostics.end(),
      [](const auto &diagnostic) {
         return diagnostic.code == "ILIC-TRANSLATION-MISMATCH" &&
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
   assert(!anonymousResult.success);
   assert(std::any_of(anonymousResult.diagnostics.begin(),anonymousResult.diagnostics.end(),
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
   assert(!invalidAssociationResult.success);
   assert(std::count_if(
      invalidAssociationResult.diagnostics.begin(),
      invalidAssociationResult.diagnostics.end(),
      [](const auto &diagnostic) {
         return diagnostic.severity == ilic::DiagnosticSeverity::Error;
      }) == 1);
   assert(invalidAssociationResult.diagnostics.front().message
      == "NotAnAssociation is no association");
   return 0;
}
