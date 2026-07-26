#include "DiagnosticCode.h"

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
   return 0;
}
