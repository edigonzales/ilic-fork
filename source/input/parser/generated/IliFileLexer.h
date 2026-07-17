
// Generated from IliFileLexer.l4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"


namespace lexer {


class  IliFileLexer : public antlr4::Lexer {
public:
  enum {
    INTERLIS = 1, DOT = 2, COMMA = 3, SEMI = 4, TRANSFER = 5, MODEL = 6, 
    TRANSLATION = 7, OF = 8, IMPORTS = 9, UNQUALIFIED = 10, NAME = 11, DIGIT = 12, 
    LETTER = 13, WS = 14, SL_COMMENT = 15, ILI_DOC = 16, ILI_BLOCKCOMMENT = 17, 
    ANYCHAR = 18
  };

  IliFileLexer(antlr4::CharStream *input);
  ~IliFileLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

}  // namespace lexer
