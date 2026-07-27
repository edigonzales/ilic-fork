#include "DiagnosticCode.h"
#include "Logger.h"

#include <cassert>

int main()
{
   using util::diagnosticCodeForMessage;

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
      "precisely ranged diagnostic",
      range,
      "ILIC-TEST-PRECISE-RANGE",
      {{relatedRange,"Base declaration"}}
   );
   const auto &diagnostic = Log.getDiagnostics().front();
   assert(diagnostic.code == "ILIC-TEST-PRECISE-RANGE");
   assert(diagnostic.range.uri == "memory:///range.ili");
   assert(diagnostic.range.start.line == 4);
   assert(diagnostic.range.end.character == 12);
   assert(diagnostic.relatedInformation.front().range.uri == "memory:///base.ili");
   Log.reset();
   return 0;
}
