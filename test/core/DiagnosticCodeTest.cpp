#include "DiagnosticCode.h"
#include "Logger.h"

#include <cassert>
#include <regex>
#include <set>

int main()
{
   using util::diagnosticCodeForMessage;

   std::set<std::string_view> codes;
   const std::regex publicCode("^ILIC-[A-Z0-9]+(?:-[A-Z0-9]+)*$");
   for (const auto &definition : util::diagnosticDefinitions()) {
      assert(std::regex_match(definition.code.begin(),definition.code.end(),publicCode));
      assert(codes.insert(definition.code).second);
      assert(util::diagnosticCode(definition.id) == definition.code);
   }
   assert(!codes.empty());

   assert(diagnosticCodeForMessage("type MissingDomain not found.")
      == "ILIC-NAME-TYPE-NOT-FOUND");
   assert(diagnosticCodeForMessage("duplicate role left in anonymous association")
      == "ILIC-NAME-DUPLICATE");
   assert(diagnosticCodeForMessage("translation mismatch for B.x against A.x: mandatory")
      == "ILIC-TRANSLATION-MISMATCH");
   assert(diagnosticCodeForMessage("cardinality of extended role owner is not a subset of its base")
      == "ILIC-CARDINALITY-RULE");
   assert(diagnosticCodeForMessage("expression must return a boolean value")
      == "ILIC-TYPE-MISMATCH");
   assert(diagnosticCodeForMessage("unsupported iliversion 2.2")
      == "ILIC-INPUT-UNSUPPORTED-VERSION");
   assert(diagnosticCodeForMessage("an association requires at least two roles")
      == "ILIC-ASSOCIATION-ROLE-COUNT");
   assert(diagnosticCodeForMessage("topicname TopicB must match TopicA")
      == "ILIC-NAME-END-MISMATCH");
   assert(diagnosticCodeForMessage("PARENT is only valid in a normal inspection view")
      == "ILIC-REFERENCE-RULE");
   assert(diagnosticCodeForMessage("NotAnAssociation is no association")
      == "ILIC-TYPE-MISMATCH");

   Log.reset();
   Log.displayErrors(false);
   ilic::SourceRange range;
   range.valid = true;
   range.uri = "memory:///range.ili";
   range.start.line = 4;
   range.start.character = 2;
   range.end.line = 4;
   range.end.character = 12;
   ilic::SourceRange relatedRange = range;
   relatedRange.uri = "memory:///base.ili";
   Log.error(
      util::DiagnosticId::TranslationCoordDimensionMismatch,
      "precisely ranged diagnostic",
      range,
      {{relatedRange,"Base declaration"}}
   );
   const auto &diagnostic = Log.getDiagnostics().front();
   assert(diagnostic.code == "ILIC-TRANSLATION-COORD-DIMENSION-MISMATCH");
   assert(diagnostic.range.uri == "memory:///range.ili");
   assert(diagnostic.range.start.line == 4);
   assert(diagnostic.range.end.character == 12);
   assert(diagnostic.relatedInformation.front().range.uri == "memory:///base.ili");
   Log.reset();
   return 0;
}
