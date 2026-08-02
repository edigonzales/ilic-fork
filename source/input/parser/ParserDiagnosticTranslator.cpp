#include "ParserDiagnosticTranslator.h"

#include <algorithm>
#include <cctype>

namespace parser {
namespace {

std::string trim(std::string value)
{
   const auto first = value.find_first_not_of(" \t\r\n");
   if (first == std::string::npos) return {};
   const auto last = value.find_last_not_of(" \t\r\n");
   return value.substr(first,last - first + 1);
}

std::string quote(std::string value)
{
   value = trim(std::move(value));
   if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'')
      value = value.substr(1,value.size() - 2);
   if (value == "<EOF>") return "end of file";
   if (value.size() > 32) value.resize(32),value += "…";
   std::string result = "'";
   for (const char character : value) {
      if (character == '\n' || character == '\r') result += "\\n";
      else if (character == '\\' || character == '\'') {
         result.push_back('\\');
         result.push_back(character);
      } else result.push_back(character);
   }
   result.push_back('\'');
   return result;
}

bool contains(std::string_view haystack,std::string_view needle)
{
   return haystack.find(needle) != std::string_view::npos;
}

std::string offending(std::string_view raw,std::string_view token)
{
   if (!token.empty()) return quote(std::string(token));
   const auto colon = raw.find_last_of(':');
   return colon == std::string_view::npos ? "the reported token" : quote(std::string(raw.substr(colon + 1)));
}

} // namespace

ParserDiagnosticTranslation translateParserDiagnostic(
   std::string_view rawMessage,std::string_view offendingText)
{
   if (contains(rawMessage,"token recognition error"))
      return {"ILIC-LEX-UNRECOGNIZED-CHARACTER",
         "Unrecognized character " + offending(rawMessage,offendingText) + "."};
   if (contains(rawMessage,"missing"))
      return {"ILIC-SYN-MISSING-TOKEN","A required token is missing before "
         + offending(rawMessage,offendingText) + "."};
   if (contains(rawMessage,"extraneous input"))
      return {"ILIC-SYN-EXTRANEOUS-TOKEN","Unexpected extra token "
         + offending(rawMessage,offendingText) + "."};
   if (contains(rawMessage,"mismatched input"))
      return {"ILIC-SYN-MISMATCHED-INPUT","Unexpected token "
         + offending(rawMessage,offendingText) + "."};
   if (contains(rawMessage,"no viable alternative"))
      return {"ILIC-SYN-NO-VIABLE-ALTERNATIVE",
         "The input cannot be parsed at this location."};
   if (contains(rawMessage,"<EOF>") || contains(rawMessage,"EOF"))
      return {"ILIC-SYN-UNEXPECTED-EOF",
         "Unexpected end of file; the construct is incomplete."};
   return {"ILIC-SYN-UNEXPECTED-TOKEN","Unexpected token "
      + offending(rawMessage,offendingText) + "."};
}

} // namespace parser
