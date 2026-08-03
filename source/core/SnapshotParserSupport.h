#pragma once

#include "../../include/ilic/Compiler.h"
#include "../input/parser/generated/Ili2Parser.h"
#include "../input/parser/ParserDiagnosticTranslator.h"
#include "BaseErrorListener.h"
#include "CommonTokenStream.h"
#include "SourceRangeMapper.h"
#include "LanguageDetector.h"

#include <cstddef>
#include <exception>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace ilic::detail {

inline DetectedLanguage detectLanguage(const SourceBuffer &source) noexcept
{
   return LanguageDetector::detect(source);
}

class SnapshotErrorCollector final : public antlr4::BaseErrorListener {
public:
   SnapshotErrorCollector(const SourceRangeMapper &ranges,const char *source)
      : ranges_(ranges),source_(source) {}

   void syntaxError(antlr4::Recognizer *,antlr4::Token *offendingSymbol,
      std::size_t,std::size_t,const std::string &message,std::exception_ptr) override
   {
      Diagnostic diagnostic;
      diagnostic.severity = DiagnosticSeverity::Error;
      const auto translation = parser::translateParserDiagnostic(message,
         offendingSymbol == nullptr ? std::string_view{} : offendingSymbol->getText());
      diagnostic.code = translation.code;
      diagnostic.message = translation.message;
      diagnostic.range = offendingSymbol == nullptr ? ranges_.eof() : ranges_.token(offendingSymbol);
      if (!diagnostic.range.valid) diagnostic.range = ranges_.eof();
      diagnostic.source = source_;
      diagnostic.phase = DiagnosticPhase::Syntax;
      diagnostics.push_back(std::move(diagnostic));
   }

   std::vector<Diagnostic> diagnostics;

private:
   const SourceRangeMapper &ranges_;
   const char *source_;
};

class EditorRecoveryTokenStream final : public antlr4::CommonTokenStream {
public:
   explicit EditorRecoveryTokenStream(antlr4::TokenSource *source)
      : antlr4::CommonTokenStream(source) {}

   void recoverBareModelHeaders()
   {
      fill();
      if (_tokens.empty()) return;

      std::set<std::size_t> recoveredEquals;
      for (std::size_t index = 0; index < _tokens.size(); ++index) {
         const auto *token = _tokens[index].get();
         if (token != nullptr && token->getType() == parser::Ili2Parser::EQUAL
            && token->getChannel() == antlr4::Token::DEFAULT_CHANNEL
            && bareModelHeaderBefore(index))
            recoveredEquals.insert(index);
      }
      if (recoveredEquals.empty()) return;

      std::vector<std::unique_ptr<antlr4::Token>> rewritten;
      rewritten.reserve(_tokens.size());
      for (std::size_t index = 0; index < _tokens.size(); ++index) {
         const auto *token = _tokens[index].get();
         if (recoveredEquals.find(index) != recoveredEquals.end()) {
            addSynthetic(rewritten,parser::Ili2Parser::ATT,"AT",token);
            addSynthetic(rewritten,parser::Ili2Parser::STRING,"\"\"",token);
            addSynthetic(rewritten,parser::Ili2Parser::VERSION,"VERSION",token);
            addSynthetic(rewritten,parser::Ili2Parser::STRING,"\"1\"",token);
         }
         rewritten.push_back(std::move(_tokens[index]));
      }
      _tokens.swap(rewritten);
      for (std::size_t index = 0; index < _tokens.size(); ++index)
         if (auto *writable = dynamic_cast<antlr4::WritableToken *>(_tokens[index].get()))
            writable->setTokenIndex(index);
      recovered_ = true;
   }

   bool recovered() const noexcept { return recovered_; }

private:
   static const antlr4::Token *nextDefault(const std::vector<std::unique_ptr<antlr4::Token>> &tokens,
      std::size_t &index)
   {
      while (++index < tokens.size()) {
         const auto *token = tokens[index].get();
         if (token != nullptr && token->getChannel() == antlr4::Token::DEFAULT_CHANNEL)
            return token;
      }
      return nullptr;
   }

   bool bareModelHeaderBefore(std::size_t equalIndex) const
   {
      std::size_t modelIndex = equalIndex;
      const auto *token = _tokens[modelIndex].get();
      while (modelIndex > 0) {
         --modelIndex;
         token = _tokens[modelIndex].get();
         if (token != nullptr && token->getChannel() == antlr4::Token::DEFAULT_CHANNEL) break;
      }
      if (token == nullptr || token->getType() != parser::Ili2Parser::NAME) return false;
      if (modelIndex == 0) return false;
      --modelIndex;
      token = _tokens[modelIndex].get();
      while (modelIndex > 0 && (token == nullptr
         || token->getChannel() != antlr4::Token::DEFAULT_CHANNEL)) {
         --modelIndex;
         token = _tokens[modelIndex].get();
      }
      if (token == nullptr || token->getType() != parser::Ili2Parser::MODEL) return false;

      std::size_t nextIndex = modelIndex;
      if (nextDefault(_tokens,nextIndex) == nullptr
         || _tokens[nextIndex]->getType() != parser::Ili2Parser::NAME)
         return false;
      if (nextDefault(_tokens,nextIndex) == nullptr) return false;
      if (_tokens[nextIndex]->getType() == parser::Ili2Parser::LPAREN) {
         if (nextDefault(_tokens,nextIndex) == nullptr
            || _tokens[nextIndex]->getType() != parser::Ili2Parser::NAME
            || nextDefault(_tokens,nextIndex) == nullptr
            || _tokens[nextIndex]->getType() != parser::Ili2Parser::RPAREN)
            return false;
         if (nextDefault(_tokens,nextIndex) == nullptr) return false;
      }
      if (_tokens[nextIndex]->getType() == parser::Ili2Parser::NOINCREMENTALTRANSFER) {
         if (nextDefault(_tokens,nextIndex) == nullptr) return false;
      }
      return _tokens[nextIndex]->getType() == parser::Ili2Parser::EQUAL;
   }

   void addSynthetic(std::vector<std::unique_ptr<antlr4::Token>> &tokens,std::size_t type,
      const std::string &text,const antlr4::Token *location)
   {
      auto factory = getTokenSource()->getTokenFactory();
      tokens.push_back(factory->create(
         {location->getTokenSource(),location->getInputStream()},type,text,
         antlr4::Token::DEFAULT_CHANNEL,INVALID_INDEX,INVALID_INDEX,
         location->getLine(),location->getCharPositionInLine()));
   }

   bool recovered_ = false;
};



void sortDiagnostics(std::vector<Diagnostic> &diagnostics);

} // namespace ilic::detail
