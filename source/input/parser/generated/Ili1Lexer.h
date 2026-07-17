
// Generated from Ili1Lexer.l4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"


namespace lexer {


class  Ili1Lexer : public antlr4::Lexer {
public:
  enum {
    WS = 1, SL_COMMENT = 2, ILI_DOC = 3, ILI_OPTION = 4, ILI_BLOCKCOMMENT = 5, 
    MODEL = 6, END = 7, TOPIC = 8, VIEW = 9, DATE = 10, NO = 11, TEXT = 12, 
    LPAREN = 13, RPAREN = 14, LBRACE = 15, RBRACE = 16, STAR = 17, GREATER = 18, 
    SEMI = 19, EQUAL = 20, DOT = 21, DOTDOT = 22, COLON = 23, COMMA = 24, 
    LARROW = 25, RARROW = 26, HALIGNMENT = 27, VALIGNMENT = 28, UNDEFINED = 29, 
    ILIDOMAIN = 30, FORMAT = 31, COORD = 32, ANY = 33, POLYLINE = 34, SURFACE = 35, 
    AREA = 36, WITH = 37, STRAIGHTS = 38, ARCS = 39, VERTEX = 40, WITHOUT = 41, 
    OVERLAPS = 42, FORM = 43, BASE = 44, TRANSFER = 45, OPTIONAL = 46, TABLE = 47, 
    IDENT = 48, COORD2 = 49, COORD3 = 50, DIM1 = 51, DIM2 = 52, RADIANS = 53, 
    DEGREES = 54, GRADS = 55, LINEATTR = 56, DERIVATIVES = 57, VERTEXINFO = 58, 
    PERIPHERY = 59, CONTOUR = 60, FREE = 61, FIX = 62, LINESIZE = 63, TIDSIZE = 64, 
    CODE = 65, FONT = 66, BLANK = 67, DEFAULT = 68, CONTINUE = 69, TID = 70, 
    I16 = 71, I32 = 72, EXPLANATION = 73, ESC = 74, STRING = 75, POSNUMBER = 76, 
    NUMBER = 77, DEC = 78, SCALING = 79, DECIMAL = 80, HEXNUMBER = 81, NAME = 82, 
    ERRORCHAR = 83
  };

  Ili1Lexer(antlr4::CharStream *input);
  ~Ili1Lexer();

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
