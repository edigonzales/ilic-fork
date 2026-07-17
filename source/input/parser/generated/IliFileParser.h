
// Generated from IliFileParser.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"


namespace parser {


class  IliFileParser : public antlr4::Parser {
public:
  enum {
    INTERLIS = 1, DOT = 2, COMMA = 3, SEMI = 4, TRANSFER = 5, MODEL = 6, 
    TRANSLATION = 7, OF = 8, IMPORTS = 9, UNQUALIFIED = 10, NAME = 11, DIGIT = 12, 
    LETTER = 13, WS = 14, SL_COMMENT = 15, ILI_DOC = 16, ILI_BLOCKCOMMENT = 17, 
    ANYCHAR = 18
  };

  enum {
    RuleIliFile = 0, RuleVersion = 1, RuleModelName = 2, RuleTranslationOf = 3, 
    RuleImports = 4, RuleModelImport = 5
  };

  IliFileParser(antlr4::TokenStream *input);
  ~IliFileParser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


  class IliFileContext;
  class VersionContext;
  class ModelNameContext;
  class TranslationOfContext;
  class ImportsContext;
  class ModelImportContext; 

  class  IliFileContext : public antlr4::ParserRuleContext {
  public:
    IliFileContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    VersionContext *version();
    std::vector<ModelNameContext *> modelName();
    ModelNameContext* modelName(size_t i);
    std::vector<TranslationOfContext *> translationOf();
    TranslationOfContext* translationOf(size_t i);
    std::vector<ImportsContext *> imports();
    ImportsContext* imports(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IliFileContext* iliFile();

  class  VersionContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *majorversion = nullptr;;
    antlr4::Token *minorversion = nullptr;;
    antlr4::Token *transfername = nullptr;;
    VersionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTERLIS();
    antlr4::tree::TerminalNode *DOT();
    std::vector<antlr4::tree::TerminalNode *> DIGIT();
    antlr4::tree::TerminalNode* DIGIT(size_t i);
    antlr4::tree::TerminalNode *TRANSFER();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  VersionContext* version();

  class  ModelNameContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *modelname = nullptr;;
    ModelNameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MODEL();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ModelNameContext* modelName();

  class  TranslationOfContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *modelname = nullptr;;
    TranslationOfContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TRANSLATION();
    antlr4::tree::TerminalNode *OF();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TranslationOfContext* translationOf();

  class  ImportsContext : public antlr4::ParserRuleContext {
  public:
    ImportsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IMPORTS();
    std::vector<ModelImportContext *> modelImport();
    ModelImportContext* modelImport(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportsContext* imports();

  class  ModelImportContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *modelname = nullptr;;
    ModelImportContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UNQUALIFIED();
    antlr4::tree::TerminalNode *NAME();
    antlr4::tree::TerminalNode *INTERLIS();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ModelImportContext* modelImport();


private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

}  // namespace parser
