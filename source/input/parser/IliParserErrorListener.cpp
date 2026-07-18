#include "IliParserErrorListener.h"

#include "../../util/Logger.h"

using namespace parser;

void IliParserErrorListener::syntaxError(
   Recognizer *recognizer, Token *offendingSymbol, size_t line,
   size_t charPositionInLine, const std::string &msg, std::exception_ptr e
)
{
   Log.error(msg,static_cast<int>(line),static_cast<int>(charPositionInLine),"ILIC-PARSE-SYNTAX");
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
