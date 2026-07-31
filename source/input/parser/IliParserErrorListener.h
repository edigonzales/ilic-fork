#pragma once

#include "ANTLRErrorListener.h"
#include "../../util/Logger.h"

using namespace antlr4;

namespace parser {

   class IliParserErrorListener : public antlr4::ANTLRErrorListener {
   public:

      explicit IliParserErrorListener(util::Logger &logger) : logger_(logger) {}

      void syntaxError(
         Recognizer *recognizer, Token *offendingSymbol, size_t line,
         size_t charPositionInLine, const std::string &msg, std::exception_ptr e
      );

      void reportAmbiguity(
         Parser *recognizer, const dfa::DFA &dfa, size_t startIndex, size_t stopIndex, bool exact,
         const antlrcpp::BitSet &ambigAlts, atn::ATNConfigSet *configs
      );

      void reportAttemptingFullContext(
         Parser *recognizer, const dfa::DFA &dfa, size_t startIndex, size_t stopIndex,
         const antlrcpp::BitSet &conflictingAlts, atn::ATNConfigSet *configs
      );

      void reportContextSensitivity(
         Parser *recognizer, const dfa::DFA &dfa, size_t startIndex, size_t stopIndex,
         size_t prediction, atn::ATNConfigSet *configs
      );

   private:
      util::Logger &logger_;

   };

};
