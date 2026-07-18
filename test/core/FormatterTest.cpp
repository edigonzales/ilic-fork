#include "ilic/Compiler.h"
#include "ilic/Formatter.h"

#include <cassert>

int main()
{
   const std::string uri = "memory:///Formatting.ili";
   const std::string source = R"ili(INTERLIS 2.3;

!!@ displayName = "Formatting test"
!! this comment must survive
MODEL Formatting AT "https://example.invalid/ilic/tests" VERSION "1" =
TOPIC Topic =
DOMAIN
Kind = (
One,
Two
);
CLASS Item =
Value : MANDATORY Kind;
END Item;
END Topic;
END Formatting.
)ili";

   ilic::Formatter formatter;
   ilic::FormatResult formatted = formatter.format(uri,source);
   assert(formatted.success && formatted.applicable);
   assert(formatted.text.find("!! this comment must survive") != std::string::npos);
   assert(formatted.text.find("!!@ displayName=\"Formatting test\"") != std::string::npos);
   assert(formatted.text.find("  TOPIC Topic =") != std::string::npos);
   assert(formatter.format(uri,formatted.text).text == formatted.text);

   ilic::CompilerSession session;
   session.putSource(uri,formatted.text);
   ilic::CompilationRequest request;
   request.roots.push_back(uri);
   assert(session.compile(request).success);
   return 0;
}
