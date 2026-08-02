#include "IliParserErrorListener.h"
#include "ParserDiagnosticTranslator.h"
#include "Token.h"

#include <algorithm>
#include <string_view>

using namespace parser;

void IliParserErrorListener::syntaxError(
   Recognizer *recognizer, Token *offendingSymbol, size_t line,
   size_t charPositionInLine, const std::string &msg, std::exception_ptr e
)
{
   const auto translation = translateParserDiagnostic(msg,
      offendingSymbol == nullptr ? std::string_view{} : offendingSymbol->getText());
   ilic::SourceRange range;
   if (line > 0 && !logger_.getCurrentSource().empty()) {
      range.valid = true;
      range.uri = logger_.getCurrentSource();
      range.start.line = line - 1;
      range.start.character = charPositionInLine;
      range.end = range.start;
      range.end.character += offendingSymbol == nullptr ? 1 :
         std::max<std::size_t>(1,offendingSymbol->getText().size());
   }
   logger_.error(translation.message,range,translation.code);
}

void IliParserErrorListener::reportAmbiguity(
   Parser *recognizer, const dfa::DFA &dfa, size_t startIndex, size_t stopIndex, bool exact,
   const antlrcpp::BitSet &ambigAlts, atn::ATNConfigSet *configs
)
{
}

void IliParserErrorListener::reportAttemptingFullContext(
   Parser *recognizer, const dfa::DFA &dfa, size_t startIndex, size_t stopIndex,
   const antlrcpp::BitSet &conflictingAlts, atn::ATNConfigSet *configs
)
{
}

void IliParserErrorListener::reportContextSensitivity(
   Parser *recognizer, const dfa::DFA &dfa, size_t startIndex, size_t stopIndex,
   size_t prediction, atn::ATNConfigSet *configs
)
{
}
