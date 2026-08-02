#pragma once

#include <string>
#include <string_view>

namespace parser {

struct ParserDiagnosticTranslation {
   std::string code;
   std::string message;
};

ParserDiagnosticTranslation translateParserDiagnostic(
   std::string_view rawMessage,std::string_view offendingText = {});

} // namespace parser
