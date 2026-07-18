#include "../../include/ilic/Formatter.h"

#include "../input/parser/generated/Ili2Lexer.h"
#include "../input/parser/generated/Ili2Parser.h"

#include "ANTLRErrorListener.h"
#include "CommonTokenStream.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace ilic {
namespace {

class SyntaxErrorCollector final : public antlr4::ANTLRErrorListener {
public:
   explicit SyntaxErrorCollector(std::string uri) : uri_(std::move(uri)) {}

   void syntaxError(antlr4::Recognizer *,antlr4::Token *,size_t line,size_t column,
      const std::string &message,std::exception_ptr) override
   {
      Diagnostic diagnostic;
      diagnostic.code = "ILIC-PARSE-SYNTAX";
      diagnostic.message = message;
      diagnostic.range.valid = true;
      diagnostic.range.uri = uri_;
      diagnostic.range.start.line = line == 0 ? 0 : line - 1;
      diagnostic.range.start.character = column;
      diagnostic.range.end = diagnostic.range.start;
      ++diagnostic.range.end.character;
      diagnostics.push_back(std::move(diagnostic));
   }

   void reportAmbiguity(antlr4::Parser *,const antlr4::dfa::DFA &,size_t,size_t,bool,
      const antlrcpp::BitSet &,antlr4::atn::ATNConfigSet *) override {}
   void reportAttemptingFullContext(antlr4::Parser *,const antlr4::dfa::DFA &,size_t,size_t,
      const antlrcpp::BitSet &,antlr4::atn::ATNConfigSet *) override {}
   void reportContextSensitivity(antlr4::Parser *,const antlr4::dfa::DFA &,size_t,size_t,size_t,
      antlr4::atn::ATNConfigSet *) override {}

   std::vector<Diagnostic> diagnostics;

private:
   std::string uri_;
};

std::string trim(const std::string &value)
{
   std::size_t first = 0;
   while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first]))) ++first;
   std::size_t last = value.size();
   while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1]))) --last;
   return value.substr(first,last - first);
}

bool startsWord(const std::string &line,const std::string &word)
{
   if (line.compare(0,word.size(),word) != 0) return false;
   return line.size() == word.size() || !std::isalnum(static_cast<unsigned char>(line[word.size()]));
}

bool opensNamedBlock(const std::string &line)
{
   static const std::vector<std::string> words = {
      "MODEL","TYPE MODEL","REFSYSTEM MODEL","SYMBOLOGY MODEL","TOPIC","VIEW TOPIC",
      "CLASS","STRUCTURE","ASSOCIATION","VIEW","GRAPHIC","CONSTRAINTS OF"
   };
   bool declaration = false;
   for (const auto &word : words) {
      if (startsWord(line,word) || line.find(" " + word + " ") != std::string::npos) {
         declaration = true;
         break;
      }
   }
   if (!declaration) return false;
   if (startsWord(line,"VIEW") && line.back() != ';' && line.find('=') == std::string::npos) return true;
   return !line.empty() && line.back() == '=';
}

std::string canonicalMetaAttribute(const std::string &line)
{
   if (line.rfind("!!@",0) != 0) return line;
   std::string value = trim(line.substr(3));
   std::size_t equals = value.find('=');
   if (equals == std::string::npos) return "!!@ " + value;
   return "!!@ " + trim(value.substr(0,equals)) + "=" + trim(value.substr(equals + 1));
}

std::vector<std::string> splitLines(const std::string &text)
{
   std::vector<std::string> lines;
   std::istringstream input(text);
   std::string line;
   while (std::getline(input,line)) {
      if (!line.empty() && line.back() == '\r') line.pop_back();
      lines.push_back(std::move(line));
   }
   return lines;
}

std::string dominantLineEnding(const std::string &text)
{
   std::size_t crlf = 0;
   std::size_t lf = 0;
   for (std::size_t i = 0; i < text.size(); ++i) {
      if (text[i] != '\n') continue;
      if (i > 0 && text[i - 1] == '\r') ++crlf;
      else ++lf;
   }
   return crlf > lf ? "\r\n" : "\n";
}

bool isValidUtf8(const std::string &text)
{
   for (std::size_t i = 0; i < text.size();) {
      unsigned char lead = static_cast<unsigned char>(text[i]);
      if (lead < 0x80) { ++i; continue; }
      std::size_t count = 0;
      if ((lead & 0xe0) == 0xc0) count = 1;
      else if ((lead & 0xf0) == 0xe0) count = 2;
      else if ((lead & 0xf8) == 0xf0) count = 3;
      else return false;
      if (i + count >= text.size()) return false;
      for (std::size_t j = 1; j <= count; ++j)
         if ((static_cast<unsigned char>(text[i + j]) & 0xc0) != 0x80) return false;
      i += count + 1;
   }
   return true;
}

std::string latin1ToUtf8(const std::string &text)
{
   std::string result;
   result.reserve(text.size() * 2);
   for (unsigned char byte : text) {
      if (byte < 0x80) result += static_cast<char>(byte);
      else {
         result += static_cast<char>(0xc0 | (byte >> 6));
         result += static_cast<char>(0x80 | (byte & 0x3f));
      }
   }
   return result;
}

std::string withoutComments(const std::string &text)
{
   std::string result = text;
   bool quoted = false;
   int blockDepth = 0;
   for (std::size_t i = 0; i < result.size();) {
      if (blockDepth > 0) {
         if (i + 1 < result.size() && result[i] == '/' && result[i + 1] == '*') {
            result[i++] = ' '; result[i++] = ' '; ++blockDepth;
         }
         else if (i + 1 < result.size() && result[i] == '*' && result[i + 1] == '/') {
            result[i++] = ' '; result[i++] = ' '; --blockDepth;
         }
         else {
            if (result[i] != '\r' && result[i] != '\n') result[i] = ' ';
            ++i;
         }
         continue;
      }
      if (result[i] == '"') {
         quoted = !quoted;
         ++i;
         continue;
      }
      if (!quoted && i + 1 < result.size() && result[i] == '/' && result[i + 1] == '*') {
         result[i++] = ' '; result[i++] = ' '; blockDepth = 1;
         continue;
      }
      if (!quoted && i + 1 < result.size() && result[i] == '!' && result[i + 1] == '!') {
         while (i < result.size() && result[i] != '\r' && result[i] != '\n') result[i++] = ' ';
         continue;
      }
      ++i;
   }
   return result;
}

std::vector<Diagnostic> validate(const std::string &uri,const std::string &text)
{
   SyntaxErrorCollector collector(uri);
   antlr4::ANTLRInputStream input(withoutComments(text));
   lexer::Ili2Lexer lexer(&input);
   lexer.removeErrorListeners();
   lexer.addErrorListener(&collector);
   antlr4::CommonTokenStream tokens(&lexer);
   parser::Ili2Parser parser(&tokens);
   parser.removeErrorListeners();
   parser.addErrorListener(&collector);
   parser.interlis2Def();
   return collector.diagnostics;
}

} // namespace

FormatResult Formatter::format(const std::string &uri,const std::string &utf8,
   const FormatOptions &options) const
{
   FormatResult result;
   std::string source = utf8;
   if (!isValidUtf8(source)) {
      source = latin1ToUtf8(source);
      Diagnostic diagnostic;
      diagnostic.severity = DiagnosticSeverity::Warning;
      diagnostic.code = "ILIC-FORMAT-ENCODING";
      diagnostic.message = "input was converted from ISO-8859-1 to UTF-8";
      result.diagnostics.push_back(std::move(diagnostic));
   }
   result.text = source;
   if (source.find("INTERLIS 2.3") == std::string::npos
       && source.find("INTERLIS 2.4") == std::string::npos) {
      Diagnostic diagnostic;
      diagnostic.code = "ILIC-FORMAT-VERSION";
      diagnostic.message = "Ili2cStyle formatting currently supports INTERLIS 2.3 and 2.4";
      result.diagnostics.push_back(std::move(diagnostic));
      return result;
   }
   if (options.requireValidSyntax) {
      try {
         std::vector<Diagnostic> syntax = validate(uri,source);
         result.diagnostics.insert(result.diagnostics.end(),syntax.begin(),syntax.end());
      }
      catch (const std::exception &error) {
         Diagnostic diagnostic;
         diagnostic.code = "ILIC-PARSE-ENCODING";
         diagnostic.message = std::string("unable to decode source as UTF-8: ") + error.what();
         result.diagnostics.push_back(std::move(diagnostic));
      }
      for (const auto &diagnostic : result.diagnostics)
         if (diagnostic.severity == DiagnosticSeverity::Error) return result;
   }

   const std::string newline = dominantLineEnding(source);
   const std::string indentUnit(options.indentSize,' ');
   std::ostringstream output;
   std::size_t indent = 0;
   bool inBlockComment = false;
   bool previousBlank = false;
   const auto lines = splitLines(source);

   for (const auto &original : lines) {
      std::string line = trim(original);
      if (line.empty()) {
         if (!previousBlank) output << newline;
         previousBlank = true;
         continue;
      }
      previousBlank = false;
      const bool commentContinuation = inBlockComment && !line.empty() && line.front() == '*';
      const bool closesBlock = startsWord(line,"END") || line.front() == ')';
      if (closesBlock && indent > 0) --indent;

      for (std::size_t i = 0; i < indent; ++i) output << indentUnit;
      if (commentContinuation) output << ' ';
      output << canonicalMetaAttribute(line) << newline;

      if (!inBlockComment && line.rfind("/*",0) == 0 && line.find("*/",2) == std::string::npos) {
         inBlockComment = true;
      }
      if (inBlockComment && line.find("*/") != std::string::npos) inBlockComment = false;

      if (opensNamedBlock(line)) ++indent;
      else if (!line.empty() && line.back() == '(') ++indent;
   }

   result.text = output.str();
   result.applicable = true;
   result.success = true;
   result.changed = result.text != utf8;
   return result;
}

} // namespace ilic
