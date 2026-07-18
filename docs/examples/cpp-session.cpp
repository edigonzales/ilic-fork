#include "ilic/Compiler.h"
#include "ilic/Formatter.h"

#include <iostream>

int main()
{
   const std::string baseUri = "memory:///Base.ili";
   const std::string exampleUri = "memory:///Example.ili";
   const std::string base = R"ili(INTERLIS 2.3;
MODEL Base AT "https://example.invalid/ilic/docs" VERSION "1" =
  DOMAIN Identifier = TEXT*40;
END Base.
)ili";
   const std::string example = R"ili(INTERLIS 2.3;
!!@ displayName = "C++ session example"
MODEL Example AT "https://example.invalid/ilic/docs" VERSION "1" =
  IMPORTS Base;
  TOPIC Data =
    CLASS Item =
      Id : MANDATORY Base.Identifier;
    END Item;
  END Data;
END Example.
)ili";

   ilic::CompilerSession session;
   session.putSource(baseUri,base,1);
   session.putSource(exampleUri,example,2);

   ilic::CompilationRequest request;
   request.roots.push_back(exampleUri);
   ilic::CompilationResult result = session.compile(request);
   for (const auto &diagnostic : result.diagnostics) {
      std::cerr << diagnostic.code << ": " << diagnostic.message << '\n';
   }
   if (!result.success) return 1;

   ilic::FormatResult formatted = ilic::Formatter().format(exampleUri,example);
   if (!formatted.success || formatted.text.find("!!@ displayName") == std::string::npos) return 2;

   std::cout << "compiled " << result.models.size() << " models; formatted="
      << (formatted.changed ? "changed" : "unchanged") << '\n';
   return 0;
}
