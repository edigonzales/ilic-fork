#include "ilic/Compiler.h"

#include <cassert>

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
   assert(result.success);
   assert(result.errorCount == 0);
   assert(!result.models.empty());
   assert(session.sources().position(uri, 0).line == 0);
   return 0;
}
