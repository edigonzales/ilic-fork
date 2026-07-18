
// Generated from Ili2Parser.g4 by ANTLR 4.7.1


#include "Ili2ParserVisitor.h"

#include "Ili2Parser.h"


using namespace antlrcpp;
using namespace parser;
using namespace antlr4;

Ili2Parser::Ili2Parser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

Ili2Parser::~Ili2Parser() {
  delete _interpreter;
}

std::string Ili2Parser::getGrammarFileName() const {
  return "Ili2Parser.g4";
}

const std::vector<std::string>& Ili2Parser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& Ili2Parser::getVocabulary() const {
  return _vocabulary;
}


//----------------- DecimalContext ------------------------------------------------------------------

Ili2Parser::DecimalContext::DecimalContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::DecimalContext::DEC() {
  return getToken(Ili2Parser::DEC, 0);
}

tree::TerminalNode* Ili2Parser::DecimalContext::POSNUMBER() {
  return getToken(Ili2Parser::POSNUMBER, 0);
}

tree::TerminalNode* Ili2Parser::DecimalContext::NUMBER() {
  return getToken(Ili2Parser::NUMBER, 0);
}


size_t Ili2Parser::DecimalContext::getRuleIndex() const {
  return Ili2Parser::RuleDecimal;
}

antlrcpp::Any Ili2Parser::DecimalContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitDecimal(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::DecimalContext* Ili2Parser::decimal() {
  DecimalContext *_localctx = _tracker.createInstance<DecimalContext>(_ctx, getState());
  enterRule(_localctx, 0, Ili2Parser::RuleDecimal);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(236);
    _la = _input->LA(1);
    if (!(((((_la - 190) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 190)) & ((1ULL << (Ili2Parser::POSNUMBER - 190))
      | (1ULL << (Ili2Parser::NUMBER - 190))
      | (1ULL << (Ili2Parser::DEC - 190)))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PathContext ------------------------------------------------------------------

Ili2Parser::PathContext::PathContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::PathContext::SIGN() {
  return getToken(Ili2Parser::SIGN, 0);
}

tree::TerminalNode* Ili2Parser::PathContext::URI() {
  return getToken(Ili2Parser::URI, 0);
}

tree::TerminalNode* Ili2Parser::PathContext::REFSYSTEM() {
  return getToken(Ili2Parser::REFSYSTEM, 0);
}

tree::TerminalNode* Ili2Parser::PathContext::BOOLEAN() {
  return getToken(Ili2Parser::BOOLEAN, 0);
}

tree::TerminalNode* Ili2Parser::PathContext::HALIGNMENT() {
  return getToken(Ili2Parser::HALIGNMENT, 0);
}

tree::TerminalNode* Ili2Parser::PathContext::VALIGNMENT() {
  return getToken(Ili2Parser::VALIGNMENT, 0);
}

tree::TerminalNode* Ili2Parser::PathContext::METAOBJECT() {
  return getToken(Ili2Parser::METAOBJECT, 0);
}

tree::TerminalNode* Ili2Parser::PathContext::INTERLIS() {
  return getToken(Ili2Parser::INTERLIS, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::PathContext::DOT() {
  return getTokens(Ili2Parser::DOT);
}

tree::TerminalNode* Ili2Parser::PathContext::DOT(size_t i) {
  return getToken(Ili2Parser::DOT, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PathContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::PathContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}


size_t Ili2Parser::PathContext::getRuleIndex() const {
  return Ili2Parser::RulePath;
}

antlrcpp::Any Ili2Parser::PathContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitPath(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::PathContext* Ili2Parser::path() {
  PathContext *_localctx = _tracker.createInstance<PathContext>(_ctx, getState());
  enterRule(_localctx, 2, Ili2Parser::RulePath);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    setState(255);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(240);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == Ili2Parser::INTERLIS) {
        setState(238);
        match(Ili2Parser::INTERLIS);
        setState(239);
        match(Ili2Parser::DOT);
      }
      setState(242);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << Ili2Parser::REFSYSTEM)
        | (1ULL << Ili2Parser::BOOLEAN)
        | (1ULL << Ili2Parser::HALIGNMENT)
        | (1ULL << Ili2Parser::VALIGNMENT))) != 0) || ((((_la - 110) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 110)) & ((1ULL << (Ili2Parser::URI - 110))
        | (1ULL << (Ili2Parser::SIGN - 110))
        | (1ULL << (Ili2Parser::METAOBJECT - 110)))) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(245);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == Ili2Parser::INTERLIS) {
        setState(243);
        match(Ili2Parser::INTERLIS);
        setState(244);
        match(Ili2Parser::DOT);
      }
      setState(247);
      match(Ili2Parser::NAME);
      setState(252);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
      while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
        if (alt == 1) {
          setState(248);
          match(Ili2Parser::DOT);
          setState(249);
          match(Ili2Parser::NAME); 
        }
        setState(254);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 2, _ctx);
      }
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Interlis2DefContext ------------------------------------------------------------------

Ili2Parser::Interlis2DefContext::Interlis2DefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::Interlis2DefContext::INTERLIS() {
  return getToken(Ili2Parser::INTERLIS, 0);
}

tree::TerminalNode* Ili2Parser::Interlis2DefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::Interlis2DefContext::EOF() {
  return getToken(Ili2Parser::EOF, 0);
}

tree::TerminalNode* Ili2Parser::Interlis2DefContext::ILI23() {
  return getToken(Ili2Parser::ILI23, 0);
}

tree::TerminalNode* Ili2Parser::Interlis2DefContext::ILI24() {
  return getToken(Ili2Parser::ILI24, 0);
}

std::vector<Ili2Parser::ModelDefContext *> Ili2Parser::Interlis2DefContext::modelDef() {
  return getRuleContexts<Ili2Parser::ModelDefContext>();
}

Ili2Parser::ModelDefContext* Ili2Parser::Interlis2DefContext::modelDef(size_t i) {
  return getRuleContext<Ili2Parser::ModelDefContext>(i);
}


size_t Ili2Parser::Interlis2DefContext::getRuleIndex() const {
  return Ili2Parser::RuleInterlis2Def;
}

antlrcpp::Any Ili2Parser::Interlis2DefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitInterlis2Def(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::Interlis2DefContext* Ili2Parser::interlis2Def() {
  Interlis2DefContext *_localctx = _tracker.createInstance<Interlis2DefContext>(_ctx, getState());
  enterRule(_localctx, 4, Ili2Parser::RuleInterlis2Def);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(257);
    match(Ili2Parser::INTERLIS);
    setState(262);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::ILI23: {
        setState(258);
        dynamic_cast<Interlis2DefContext *>(_localctx)->iliversion = match(Ili2Parser::ILI23);
        ili23=true;
        break;
      }

      case Ili2Parser::ILI24: {
        setState(260);
        dynamic_cast<Interlis2DefContext *>(_localctx)->iliversion = match(Ili2Parser::ILI24);
        ili24=true;
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(264);
    match(Ili2Parser::SEMI);
    setState(268);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << Ili2Parser::CONTRACTED)
      | (1ULL << Ili2Parser::REFSYSTEM)
      | (1ULL << Ili2Parser::SYMBOLOGY)
      | (1ULL << Ili2Parser::TYPE)
      | (1ULL << Ili2Parser::MODEL))) != 0)) {
      setState(265);
      modelDef();
      setState(270);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(271);
    match(Ili2Parser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ModelDefContext ------------------------------------------------------------------

Ili2Parser::ModelDefContext::ModelDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ModelDefContext::MODEL() {
  return getToken(Ili2Parser::MODEL, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::ATT() {
  return getToken(Ili2Parser::ATT, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::VERSION() {
  return getToken(Ili2Parser::VERSION, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::END() {
  return getToken(Ili2Parser::END, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::DOT() {
  return getToken(Ili2Parser::DOT, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::ModelDefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::ModelDefContext::STRING() {
  return getTokens(Ili2Parser::STRING);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::STRING(size_t i) {
  return getToken(Ili2Parser::STRING, i);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::NOINCREMENTALTRANSFER() {
  return getToken(Ili2Parser::NOINCREMENTALTRANSFER, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::TRANSLATION() {
  return getToken(Ili2Parser::TRANSLATION, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::LBRACE() {
  return getToken(Ili2Parser::LBRACE, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::RBRACE() {
  return getToken(Ili2Parser::RBRACE, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::CHARSET() {
  return getToken(Ili2Parser::CHARSET, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::ModelDefContext::SEMI() {
  return getTokens(Ili2Parser::SEMI);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::SEMI(size_t i) {
  return getToken(Ili2Parser::SEMI, i);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::XMLNS() {
  return getToken(Ili2Parser::XMLNS, 0);
}

std::vector<Ili2Parser::ImportDefContext *> Ili2Parser::ModelDefContext::importDef() {
  return getRuleContexts<Ili2Parser::ImportDefContext>();
}

Ili2Parser::ImportDefContext* Ili2Parser::ModelDefContext::importDef(size_t i) {
  return getRuleContext<Ili2Parser::ImportDefContext>(i);
}

std::vector<Ili2Parser::MetaDataBasketDefContext *> Ili2Parser::ModelDefContext::metaDataBasketDef() {
  return getRuleContexts<Ili2Parser::MetaDataBasketDefContext>();
}

Ili2Parser::MetaDataBasketDefContext* Ili2Parser::ModelDefContext::metaDataBasketDef(size_t i) {
  return getRuleContext<Ili2Parser::MetaDataBasketDefContext>(i);
}

std::vector<Ili2Parser::UnitDeclContext *> Ili2Parser::ModelDefContext::unitDecl() {
  return getRuleContexts<Ili2Parser::UnitDeclContext>();
}

Ili2Parser::UnitDeclContext* Ili2Parser::ModelDefContext::unitDecl(size_t i) {
  return getRuleContext<Ili2Parser::UnitDeclContext>(i);
}

std::vector<Ili2Parser::FunctionDefContext *> Ili2Parser::ModelDefContext::functionDef() {
  return getRuleContexts<Ili2Parser::FunctionDefContext>();
}

Ili2Parser::FunctionDefContext* Ili2Parser::ModelDefContext::functionDef(size_t i) {
  return getRuleContext<Ili2Parser::FunctionDefContext>(i);
}

std::vector<Ili2Parser::LineFormTypeDefContext *> Ili2Parser::ModelDefContext::lineFormTypeDef() {
  return getRuleContexts<Ili2Parser::LineFormTypeDefContext>();
}

Ili2Parser::LineFormTypeDefContext* Ili2Parser::ModelDefContext::lineFormTypeDef(size_t i) {
  return getRuleContext<Ili2Parser::LineFormTypeDefContext>(i);
}

std::vector<Ili2Parser::DomainDefContext *> Ili2Parser::ModelDefContext::domainDef() {
  return getRuleContexts<Ili2Parser::DomainDefContext>();
}

Ili2Parser::DomainDefContext* Ili2Parser::ModelDefContext::domainDef(size_t i) {
  return getRuleContext<Ili2Parser::DomainDefContext>(i);
}

std::vector<Ili2Parser::ContextDefContext *> Ili2Parser::ModelDefContext::contextDef() {
  return getRuleContexts<Ili2Parser::ContextDefContext>();
}

Ili2Parser::ContextDefContext* Ili2Parser::ModelDefContext::contextDef(size_t i) {
  return getRuleContext<Ili2Parser::ContextDefContext>(i);
}

std::vector<Ili2Parser::RunTimeParameterDefContext *> Ili2Parser::ModelDefContext::runTimeParameterDef() {
  return getRuleContexts<Ili2Parser::RunTimeParameterDefContext>();
}

Ili2Parser::RunTimeParameterDefContext* Ili2Parser::ModelDefContext::runTimeParameterDef(size_t i) {
  return getRuleContext<Ili2Parser::RunTimeParameterDefContext>(i);
}

std::vector<Ili2Parser::ClassDefContext *> Ili2Parser::ModelDefContext::classDef() {
  return getRuleContexts<Ili2Parser::ClassDefContext>();
}

Ili2Parser::ClassDefContext* Ili2Parser::ModelDefContext::classDef(size_t i) {
  return getRuleContext<Ili2Parser::ClassDefContext>(i);
}

std::vector<Ili2Parser::StructureDefContext *> Ili2Parser::ModelDefContext::structureDef() {
  return getRuleContexts<Ili2Parser::StructureDefContext>();
}

Ili2Parser::StructureDefContext* Ili2Parser::ModelDefContext::structureDef(size_t i) {
  return getRuleContext<Ili2Parser::StructureDefContext>(i);
}

std::vector<Ili2Parser::TopicDefContext *> Ili2Parser::ModelDefContext::topicDef() {
  return getRuleContexts<Ili2Parser::TopicDefContext>();
}

Ili2Parser::TopicDefContext* Ili2Parser::ModelDefContext::topicDef(size_t i) {
  return getRuleContext<Ili2Parser::TopicDefContext>(i);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::CONTRACTED() {
  return getToken(Ili2Parser::CONTRACTED, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::REFSYSTEM() {
  return getToken(Ili2Parser::REFSYSTEM, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::SYMBOLOGY() {
  return getToken(Ili2Parser::SYMBOLOGY, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::TYPE() {
  return getToken(Ili2Parser::TYPE, 0);
}

tree::TerminalNode* Ili2Parser::ModelDefContext::EXPLANATION() {
  return getToken(Ili2Parser::EXPLANATION, 0);
}


size_t Ili2Parser::ModelDefContext::getRuleIndex() const {
  return Ili2Parser::RuleModelDef;
}

antlrcpp::Any Ili2Parser::ModelDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitModelDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ModelDefContext* Ili2Parser::modelDef() {
  ModelDefContext *_localctx = _tracker.createInstance<ModelDefContext>(_ctx, getState());
  enterRule(_localctx, 6, Ili2Parser::RuleModelDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(274);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::CONTRACTED) {
      setState(273);
      dynamic_cast<ModelDefContext *>(_localctx)->contracted = match(Ili2Parser::CONTRACTED);
    }
    setState(279);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::REFSYSTEM: {
        setState(276);
        dynamic_cast<ModelDefContext *>(_localctx)->refsystemmodel = match(Ili2Parser::REFSYSTEM);
        break;
      }

      case Ili2Parser::SYMBOLOGY: {
        setState(277);
        dynamic_cast<ModelDefContext *>(_localctx)->symbologymodel = match(Ili2Parser::SYMBOLOGY);
        break;
      }

      case Ili2Parser::TYPE: {
        setState(278);
        dynamic_cast<ModelDefContext *>(_localctx)->typemodel = match(Ili2Parser::TYPE);
        break;
      }

      case Ili2Parser::MODEL: {
        break;
      }

    default:
      break;
    }
    setState(281);
    match(Ili2Parser::MODEL);
    setState(282);
    dynamic_cast<ModelDefContext *>(_localctx)->modelname1 = match(Ili2Parser::NAME);
    setState(286);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 8, _ctx)) {
    case 1: {
      setState(283);
      match(Ili2Parser::LPAREN);
      setState(284);
      dynamic_cast<ModelDefContext *>(_localctx)->language = match(Ili2Parser::NAME);
      setState(285);
      match(Ili2Parser::RPAREN);
      break;
    }

    }
    setState(290);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 9, _ctx)) {
    case 1: {
      setState(288);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(289);
      match(Ili2Parser::NOINCREMENTALTRANSFER);
      break;
    }

    }
    setState(292);
    match(Ili2Parser::ATT);
    setState(293);
    dynamic_cast<ModelDefContext *>(_localctx)->issuerurl = match(Ili2Parser::STRING);
    setState(294);
    match(Ili2Parser::VERSION);
    setState(295);
    dynamic_cast<ModelDefContext *>(_localctx)->modelversion = match(Ili2Parser::STRING);
    setState(297);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXPLANATION) {
      setState(296);
      dynamic_cast<ModelDefContext *>(_localctx)->modelversion_expl = match(Ili2Parser::EXPLANATION);
    }
    setState(305);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::TRANSLATION) {
      setState(299);
      match(Ili2Parser::TRANSLATION);
      setState(300);
      match(Ili2Parser::OF);
      setState(301);
      dynamic_cast<ModelDefContext *>(_localctx)->translationOf = match(Ili2Parser::NAME);
      setState(302);
      match(Ili2Parser::LBRACE);
      setState(303);
      dynamic_cast<ModelDefContext *>(_localctx)->translationOfVersion = match(Ili2Parser::STRING);
      setState(304);
      match(Ili2Parser::RBRACE);
    }
    setState(307);
    match(Ili2Parser::EQUAL);
    setState(312);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
    case 1: {
      setState(308);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(309);
      match(Ili2Parser::CHARSET);
      setState(310);
      dynamic_cast<ModelDefContext *>(_localctx)->iananame = match(Ili2Parser::STRING);
      setState(311);
      match(Ili2Parser::SEMI);
      break;
    }

    }
    setState(318);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 13, _ctx)) {
    case 1: {
      setState(314);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(315);
      match(Ili2Parser::XMLNS);
      setState(316);
      dynamic_cast<ModelDefContext *>(_localctx)->xmlns = match(Ili2Parser::STRING);
      setState(317);
      match(Ili2Parser::SEMI);
      break;
    }

    }
    setState(323);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(320);
        importDef(); 
      }
      setState(325);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 14, _ctx);
    }
    setState(339);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(337);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 15, _ctx)) {
        case 1: {
          setState(326);
          metaDataBasketDef();
          break;
        }

        case 2: {
          setState(327);
          unitDecl();
          break;
        }

        case 3: {
          setState(328);
          functionDef();
          break;
        }

        case 4: {
          setState(329);
          lineFormTypeDef();
          break;
        }

        case 5: {
          setState(330);
          domainDef();
          break;
        }

        case 6: {
          setState(331);

          if (!(ili24)) throw FailedPredicateException(this, "ili24");
          setState(332);
          contextDef();
          break;
        }

        case 7: {
          setState(333);
          runTimeParameterDef();
          break;
        }

        case 8: {
          setState(334);
          classDef();
          break;
        }

        case 9: {
          setState(335);
          structureDef();
          break;
        }

        case 10: {
          setState(336);
          topicDef();
          break;
        }

        } 
      }
      setState(341);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 16, _ctx);
    }
    setState(342);
    match(Ili2Parser::END);
    setState(343);
    dynamic_cast<ModelDefContext *>(_localctx)->modelname2 = match(Ili2Parser::NAME);
    setState(344);
    match(Ili2Parser::DOT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImportDefContext ------------------------------------------------------------------

Ili2Parser::ImportDefContext::ImportDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ImportDefContext::IMPORTS() {
  return getToken(Ili2Parser::IMPORTS, 0);
}

std::vector<Ili2Parser::ImportingContext *> Ili2Parser::ImportDefContext::importing() {
  return getRuleContexts<Ili2Parser::ImportingContext>();
}

Ili2Parser::ImportingContext* Ili2Parser::ImportDefContext::importing(size_t i) {
  return getRuleContext<Ili2Parser::ImportingContext>(i);
}

tree::TerminalNode* Ili2Parser::ImportDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::ImportDefContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::ImportDefContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::ImportDefContext::getRuleIndex() const {
  return Ili2Parser::RuleImportDef;
}

antlrcpp::Any Ili2Parser::ImportDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitImportDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ImportDefContext* Ili2Parser::importDef() {
  ImportDefContext *_localctx = _tracker.createInstance<ImportDefContext>(_ctx, getState());
  enterRule(_localctx, 8, Ili2Parser::RuleImportDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(346);
    match(Ili2Parser::IMPORTS);
    setState(347);
    importing();
    setState(352);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::COMMA) {
      setState(348);
      match(Ili2Parser::COMMA);
      setState(349);
      importing();
      setState(354);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(355);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImportingContext ------------------------------------------------------------------

Ili2Parser::ImportingContext::ImportingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ImportingContext::INTERLIS() {
  return getToken(Ili2Parser::INTERLIS, 0);
}

tree::TerminalNode* Ili2Parser::ImportingContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

tree::TerminalNode* Ili2Parser::ImportingContext::UNQUALIFIED() {
  return getToken(Ili2Parser::UNQUALIFIED, 0);
}


size_t Ili2Parser::ImportingContext::getRuleIndex() const {
  return Ili2Parser::RuleImporting;
}

antlrcpp::Any Ili2Parser::ImportingContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitImporting(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ImportingContext* Ili2Parser::importing() {
  ImportingContext *_localctx = _tracker.createInstance<ImportingContext>(_ctx, getState());
  enterRule(_localctx, 10, Ili2Parser::RuleImporting);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(358);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::UNQUALIFIED) {
      setState(357);
      match(Ili2Parser::UNQUALIFIED);
    }
    setState(360);
    _la = _input->LA(1);
    if (!(_la == Ili2Parser::INTERLIS || _la == Ili2Parser::NAME)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TopicDefContext ------------------------------------------------------------------

Ili2Parser::TopicDefContext::TopicDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::TopicDefContext::TOPIC() {
  return getToken(Ili2Parser::TOPIC, 0);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::END() {
  return getToken(Ili2Parser::END, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::TopicDefContext::SEMI() {
  return getTokens(Ili2Parser::SEMI);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::SEMI(size_t i) {
  return getToken(Ili2Parser::SEMI, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::TopicDefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::VIEW() {
  return getToken(Ili2Parser::VIEW, 0);
}

Ili2Parser::PropertiesContext* Ili2Parser::TopicDefContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::EXTENDS() {
  return getToken(Ili2Parser::EXTENDS, 0);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::BASKET() {
  return getToken(Ili2Parser::BASKET, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::TopicDefContext::OID() {
  return getTokens(Ili2Parser::OID);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::OID(size_t i) {
  return getToken(Ili2Parser::OID, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::TopicDefContext::AS() {
  return getTokens(Ili2Parser::AS);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::AS(size_t i) {
  return getToken(Ili2Parser::AS, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::TopicDefContext::DEPENDS() {
  return getTokens(Ili2Parser::DEPENDS);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::DEPENDS(size_t i) {
  return getToken(Ili2Parser::DEPENDS, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::TopicDefContext::ON() {
  return getTokens(Ili2Parser::ON);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::ON(size_t i) {
  return getToken(Ili2Parser::ON, i);
}

std::vector<Ili2Parser::TopicPathContext *> Ili2Parser::TopicDefContext::topicPath() {
  return getRuleContexts<Ili2Parser::TopicPathContext>();
}

Ili2Parser::TopicPathContext* Ili2Parser::TopicDefContext::topicPath(size_t i) {
  return getRuleContext<Ili2Parser::TopicPathContext>(i);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::DEFERRED() {
  return getToken(Ili2Parser::DEFERRED, 0);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::GENERICS() {
  return getToken(Ili2Parser::GENERICS, 0);
}

std::vector<Ili2Parser::MetaDataBasketDefContext *> Ili2Parser::TopicDefContext::metaDataBasketDef() {
  return getRuleContexts<Ili2Parser::MetaDataBasketDefContext>();
}

Ili2Parser::MetaDataBasketDefContext* Ili2Parser::TopicDefContext::metaDataBasketDef(size_t i) {
  return getRuleContext<Ili2Parser::MetaDataBasketDefContext>(i);
}

std::vector<Ili2Parser::UnitDeclContext *> Ili2Parser::TopicDefContext::unitDecl() {
  return getRuleContexts<Ili2Parser::UnitDeclContext>();
}

Ili2Parser::UnitDeclContext* Ili2Parser::TopicDefContext::unitDecl(size_t i) {
  return getRuleContext<Ili2Parser::UnitDeclContext>(i);
}

std::vector<Ili2Parser::FunctionDefContext *> Ili2Parser::TopicDefContext::functionDef() {
  return getRuleContexts<Ili2Parser::FunctionDefContext>();
}

Ili2Parser::FunctionDefContext* Ili2Parser::TopicDefContext::functionDef(size_t i) {
  return getRuleContext<Ili2Parser::FunctionDefContext>(i);
}

std::vector<Ili2Parser::DomainDefContext *> Ili2Parser::TopicDefContext::domainDef() {
  return getRuleContexts<Ili2Parser::DomainDefContext>();
}

Ili2Parser::DomainDefContext* Ili2Parser::TopicDefContext::domainDef(size_t i) {
  return getRuleContext<Ili2Parser::DomainDefContext>(i);
}

std::vector<Ili2Parser::ClassDefContext *> Ili2Parser::TopicDefContext::classDef() {
  return getRuleContexts<Ili2Parser::ClassDefContext>();
}

Ili2Parser::ClassDefContext* Ili2Parser::TopicDefContext::classDef(size_t i) {
  return getRuleContext<Ili2Parser::ClassDefContext>(i);
}

std::vector<Ili2Parser::StructureDefContext *> Ili2Parser::TopicDefContext::structureDef() {
  return getRuleContexts<Ili2Parser::StructureDefContext>();
}

Ili2Parser::StructureDefContext* Ili2Parser::TopicDefContext::structureDef(size_t i) {
  return getRuleContext<Ili2Parser::StructureDefContext>(i);
}

std::vector<Ili2Parser::AssociationDefContext *> Ili2Parser::TopicDefContext::associationDef() {
  return getRuleContexts<Ili2Parser::AssociationDefContext>();
}

Ili2Parser::AssociationDefContext* Ili2Parser::TopicDefContext::associationDef(size_t i) {
  return getRuleContext<Ili2Parser::AssociationDefContext>(i);
}

std::vector<Ili2Parser::ConstraintsDefContext *> Ili2Parser::TopicDefContext::constraintsDef() {
  return getRuleContexts<Ili2Parser::ConstraintsDefContext>();
}

Ili2Parser::ConstraintsDefContext* Ili2Parser::TopicDefContext::constraintsDef(size_t i) {
  return getRuleContext<Ili2Parser::ConstraintsDefContext>(i);
}

std::vector<Ili2Parser::ViewDefContext *> Ili2Parser::TopicDefContext::viewDef() {
  return getRuleContexts<Ili2Parser::ViewDefContext>();
}

Ili2Parser::ViewDefContext* Ili2Parser::TopicDefContext::viewDef(size_t i) {
  return getRuleContext<Ili2Parser::ViewDefContext>(i);
}

std::vector<Ili2Parser::GraphicDefContext *> Ili2Parser::TopicDefContext::graphicDef() {
  return getRuleContexts<Ili2Parser::GraphicDefContext>();
}

Ili2Parser::GraphicDefContext* Ili2Parser::TopicDefContext::graphicDef(size_t i) {
  return getRuleContext<Ili2Parser::GraphicDefContext>(i);
}

std::vector<Ili2Parser::PathContext *> Ili2Parser::TopicDefContext::path() {
  return getRuleContexts<Ili2Parser::PathContext>();
}

Ili2Parser::PathContext* Ili2Parser::TopicDefContext::path(size_t i) {
  return getRuleContext<Ili2Parser::PathContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::TopicDefContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::TopicDefContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::TopicDefContext::getRuleIndex() const {
  return Ili2Parser::RuleTopicDef;
}

antlrcpp::Any Ili2Parser::TopicDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitTopicDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::TopicDefContext* Ili2Parser::topicDef() {
  TopicDefContext *_localctx = _tracker.createInstance<TopicDefContext>(_ctx, getState());
  enterRule(_localctx, 12, Ili2Parser::RuleTopicDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(363);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::VIEW) {
      setState(362);
      match(Ili2Parser::VIEW);
    }
    setState(365);
    match(Ili2Parser::TOPIC);
    setState(366);
    dynamic_cast<TopicDefContext *>(_localctx)->topicname1 = match(Ili2Parser::NAME);
    setState(368);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(367);
      properties();
    }
    setState(372);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXTENDS) {
      setState(370);
      match(Ili2Parser::EXTENDS);
      setState(371);
      dynamic_cast<TopicDefContext *>(_localctx)->topicbase = path();
    }
    setState(374);
    match(Ili2Parser::EQUAL);
    setState(381);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 22, _ctx)) {
    case 1: {
      setState(375);
      match(Ili2Parser::BASKET);
      setState(376);
      match(Ili2Parser::OID);
      setState(377);
      match(Ili2Parser::AS);
      setState(378);
      dynamic_cast<TopicDefContext *>(_localctx)->basketOid = path();
      setState(379);
      match(Ili2Parser::SEMI);
      break;
    }

    }
    setState(388);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      setState(383);
      match(Ili2Parser::OID);
      setState(384);
      match(Ili2Parser::AS);
      setState(385);
      dynamic_cast<TopicDefContext *>(_localctx)->topicOid = path();
      setState(386);
      match(Ili2Parser::SEMI);
      break;
    }

    }
    setState(404);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(390);
        match(Ili2Parser::DEPENDS);
        setState(391);
        match(Ili2Parser::ON);
        setState(392);
        topicPath();
        setState(397);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == Ili2Parser::COMMA) {
          setState(393);
          match(Ili2Parser::COMMA);
          setState(394);
          topicPath();
          setState(399);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(400);
        match(Ili2Parser::SEMI); 
      }
      setState(406);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx);
    }
    setState(420);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 27, _ctx)) {
    case 1: {
      setState(407);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(408);
      match(Ili2Parser::DEFERRED);
      setState(409);
      match(Ili2Parser::GENERICS);
      setState(410);
      dynamic_cast<TopicDefContext *>(_localctx)->genericref = path();
      setState(415);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili2Parser::COMMA) {
        setState(411);
        match(Ili2Parser::COMMA);
        setState(412);
        dynamic_cast<TopicDefContext *>(_localctx)->genericref = path();
        setState(417);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(418);
      match(Ili2Parser::SEMI);
      break;
    }

    }
    setState(434);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << Ili2Parser::REFSYSTEM)
      | (1ULL << Ili2Parser::VIEW)
      | (1ULL << Ili2Parser::CLASS))) != 0) || ((((_la - 86) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 86)) & ((1ULL << (Ili2Parser::STRUCTURE - 86))
      | (1ULL << (Ili2Parser::ASSOCIATION - 86))
      | (1ULL << (Ili2Parser::ILIDOMAIN - 86))
      | (1ULL << (Ili2Parser::UNIT - 86))
      | (1ULL << (Ili2Parser::FUNCTION - 86))
      | (1ULL << (Ili2Parser::SIGN - 86)))) != 0) || _la == Ili2Parser::CONSTRAINTS

    || _la == Ili2Parser::GRAPHIC) {
      setState(432);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case Ili2Parser::REFSYSTEM:
        case Ili2Parser::SIGN: {
          setState(422);
          metaDataBasketDef();
          break;
        }

        case Ili2Parser::UNIT: {
          setState(423);
          unitDecl();
          break;
        }

        case Ili2Parser::FUNCTION: {
          setState(424);
          functionDef();
          break;
        }

        case Ili2Parser::ILIDOMAIN: {
          setState(425);
          domainDef();
          break;
        }

        case Ili2Parser::CLASS: {
          setState(426);
          classDef();
          break;
        }

        case Ili2Parser::STRUCTURE: {
          setState(427);
          structureDef();
          break;
        }

        case Ili2Parser::ASSOCIATION: {
          setState(428);
          associationDef();
          break;
        }

        case Ili2Parser::CONSTRAINTS: {
          setState(429);
          constraintsDef();
          break;
        }

        case Ili2Parser::VIEW: {
          setState(430);
          viewDef();
          break;
        }

        case Ili2Parser::GRAPHIC: {
          setState(431);
          graphicDef();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(436);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(437);
    match(Ili2Parser::END);
    setState(438);
    dynamic_cast<TopicDefContext *>(_localctx)->topicname2 = match(Ili2Parser::NAME);
    setState(439);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TopicPathContext ------------------------------------------------------------------

Ili2Parser::TopicPathContext::TopicPathContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::PathContext* Ili2Parser::TopicPathContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::TopicPathContext::getRuleIndex() const {
  return Ili2Parser::RuleTopicPath;
}

antlrcpp::Any Ili2Parser::TopicPathContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitTopicPath(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::TopicPathContext* Ili2Parser::topicPath() {
  TopicPathContext *_localctx = _tracker.createInstance<TopicPathContext>(_ctx, getState());
  enterRule(_localctx, 14, Ili2Parser::RuleTopicPath);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(441);
    path();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassDefContext ------------------------------------------------------------------

Ili2Parser::ClassDefContext::ClassDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ClassDefContext::CLASS() {
  return getToken(Ili2Parser::CLASS, 0);
}

tree::TerminalNode* Ili2Parser::ClassDefContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

Ili2Parser::ClassOrStructureDefContext* Ili2Parser::ClassDefContext::classOrStructureDef() {
  return getRuleContext<Ili2Parser::ClassOrStructureDefContext>(0);
}

tree::TerminalNode* Ili2Parser::ClassDefContext::END() {
  return getToken(Ili2Parser::END, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::ClassDefContext::SEMI() {
  return getTokens(Ili2Parser::SEMI);
}

tree::TerminalNode* Ili2Parser::ClassDefContext::SEMI(size_t i) {
  return getToken(Ili2Parser::SEMI, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::ClassDefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::ClassDefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

Ili2Parser::PropertiesContext* Ili2Parser::ClassDefContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

tree::TerminalNode* Ili2Parser::ClassDefContext::EXTENDS() {
  return getToken(Ili2Parser::EXTENDS, 0);
}

std::vector<Ili2Parser::PathContext *> Ili2Parser::ClassDefContext::path() {
  return getRuleContexts<Ili2Parser::PathContext>();
}

Ili2Parser::PathContext* Ili2Parser::ClassDefContext::path(size_t i) {
  return getRuleContext<Ili2Parser::PathContext>(i);
}

tree::TerminalNode* Ili2Parser::ClassDefContext::OID() {
  return getToken(Ili2Parser::OID, 0);
}

tree::TerminalNode* Ili2Parser::ClassDefContext::AS() {
  return getToken(Ili2Parser::AS, 0);
}

tree::TerminalNode* Ili2Parser::ClassDefContext::NO() {
  return getToken(Ili2Parser::NO, 0);
}


size_t Ili2Parser::ClassDefContext::getRuleIndex() const {
  return Ili2Parser::RuleClassDef;
}

antlrcpp::Any Ili2Parser::ClassDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitClassDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ClassDefContext* Ili2Parser::classDef() {
  ClassDefContext *_localctx = _tracker.createInstance<ClassDefContext>(_ctx, getState());
  enterRule(_localctx, 16, Ili2Parser::RuleClassDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(443);
    match(Ili2Parser::CLASS);
    setState(444);
    dynamic_cast<ClassDefContext *>(_localctx)->classname1 = match(Ili2Parser::NAME);
    setState(446);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(445);
      properties();
    }
    setState(450);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXTENDS) {
      setState(448);
      match(Ili2Parser::EXTENDS);
      setState(449);
      dynamic_cast<ClassDefContext *>(_localctx)->classbase = path();
    }
    setState(452);
    match(Ili2Parser::EQUAL);
    setState(461);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::OID

    || _la == Ili2Parser::NO) {
      setState(458);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case Ili2Parser::OID: {
          setState(453);
          match(Ili2Parser::OID);
          setState(454);
          match(Ili2Parser::AS);
          setState(455);
          dynamic_cast<ClassDefContext *>(_localctx)->classoid = path();
          break;
        }

        case Ili2Parser::NO: {
          setState(456);
          match(Ili2Parser::NO);
          setState(457);
          match(Ili2Parser::OID);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(460);
      match(Ili2Parser::SEMI);
    }
    setState(463);
    classOrStructureDef();
    setState(464);
    match(Ili2Parser::END);
    setState(465);
    dynamic_cast<ClassDefContext *>(_localctx)->classname2 = match(Ili2Parser::NAME);
    setState(466);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StructureDefContext ------------------------------------------------------------------

Ili2Parser::StructureDefContext::StructureDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::StructureDefContext::STRUCTURE() {
  return getToken(Ili2Parser::STRUCTURE, 0);
}

tree::TerminalNode* Ili2Parser::StructureDefContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

Ili2Parser::ClassOrStructureDefContext* Ili2Parser::StructureDefContext::classOrStructureDef() {
  return getRuleContext<Ili2Parser::ClassOrStructureDefContext>(0);
}

tree::TerminalNode* Ili2Parser::StructureDefContext::END() {
  return getToken(Ili2Parser::END, 0);
}

tree::TerminalNode* Ili2Parser::StructureDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::StructureDefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::StructureDefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

Ili2Parser::PropertiesContext* Ili2Parser::StructureDefContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

tree::TerminalNode* Ili2Parser::StructureDefContext::EXTENDS() {
  return getToken(Ili2Parser::EXTENDS, 0);
}

Ili2Parser::PathContext* Ili2Parser::StructureDefContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::StructureDefContext::getRuleIndex() const {
  return Ili2Parser::RuleStructureDef;
}

antlrcpp::Any Ili2Parser::StructureDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitStructureDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::StructureDefContext* Ili2Parser::structureDef() {
  StructureDefContext *_localctx = _tracker.createInstance<StructureDefContext>(_ctx, getState());
  enterRule(_localctx, 18, Ili2Parser::RuleStructureDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(468);
    match(Ili2Parser::STRUCTURE);
    setState(469);
    dynamic_cast<StructureDefContext *>(_localctx)->structurename1 = match(Ili2Parser::NAME);
    setState(471);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(470);
      properties();
    }
    setState(475);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXTENDS) {
      setState(473);
      match(Ili2Parser::EXTENDS);
      setState(474);
      dynamic_cast<StructureDefContext *>(_localctx)->structurebase = path();
    }
    setState(477);
    match(Ili2Parser::EQUAL);
    setState(478);
    classOrStructureDef();
    setState(479);
    match(Ili2Parser::END);
    setState(480);
    dynamic_cast<StructureDefContext *>(_localctx)->structurename2 = match(Ili2Parser::NAME);
    setState(481);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassOrStructureDefContext ------------------------------------------------------------------

Ili2Parser::ClassOrStructureDefContext::ClassOrStructureDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ClassOrStructureDefContext::ATTRIBUTE() {
  return getToken(Ili2Parser::ATTRIBUTE, 0);
}

std::vector<Ili2Parser::AttributeDefContext *> Ili2Parser::ClassOrStructureDefContext::attributeDef() {
  return getRuleContexts<Ili2Parser::AttributeDefContext>();
}

Ili2Parser::AttributeDefContext* Ili2Parser::ClassOrStructureDefContext::attributeDef(size_t i) {
  return getRuleContext<Ili2Parser::AttributeDefContext>(i);
}

std::vector<Ili2Parser::ConstraintDefContext *> Ili2Parser::ClassOrStructureDefContext::constraintDef() {
  return getRuleContexts<Ili2Parser::ConstraintDefContext>();
}

Ili2Parser::ConstraintDefContext* Ili2Parser::ClassOrStructureDefContext::constraintDef(size_t i) {
  return getRuleContext<Ili2Parser::ConstraintDefContext>(i);
}

tree::TerminalNode* Ili2Parser::ClassOrStructureDefContext::PARAMETER() {
  return getToken(Ili2Parser::PARAMETER, 0);
}

std::vector<Ili2Parser::ParameterDefContext *> Ili2Parser::ClassOrStructureDefContext::parameterDef() {
  return getRuleContexts<Ili2Parser::ParameterDefContext>();
}

Ili2Parser::ParameterDefContext* Ili2Parser::ClassOrStructureDefContext::parameterDef(size_t i) {
  return getRuleContext<Ili2Parser::ParameterDefContext>(i);
}


size_t Ili2Parser::ClassOrStructureDefContext::getRuleIndex() const {
  return Ili2Parser::RuleClassOrStructureDef;
}

antlrcpp::Any Ili2Parser::ClassOrStructureDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitClassOrStructureDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ClassOrStructureDefContext* Ili2Parser::classOrStructureDef() {
  ClassOrStructureDefContext *_localctx = _tracker.createInstance<ClassOrStructureDefContext>(_ctx, getState());
  enterRule(_localctx, 20, Ili2Parser::RuleClassOrStructureDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(484);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::ATTRIBUTE) {
      setState(483);
      match(Ili2Parser::ATTRIBUTE);
    }
    setState(489);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::CONTINUOUS

    || _la == Ili2Parser::SUBDIVISION || _la == Ili2Parser::NAME) {
      setState(486);
      attributeDef();
      setState(491);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(495);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::MANDATORY

    || _la == Ili2Parser::CONSTRAINT || ((((_la - 152) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 152)) & ((1ULL << (Ili2Parser::EXISTENCE - 152))
      | (1ULL << (Ili2Parser::UNIQUE - 152))
      | (1ULL << (Ili2Parser::SET - 152)))) != 0)) {
      setState(492);
      constraintDef();
      setState(497);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(505);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::PARAMETER) {
      setState(498);
      match(Ili2Parser::PARAMETER);
      setState(502);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili2Parser::NAME) {
        setState(499);
        parameterDef();
        setState(504);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttributeDefContext ------------------------------------------------------------------

Ili2Parser::AttributeDefContext::AttributeDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::AttributeDefContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

Ili2Parser::AttrTypeDefContext* Ili2Parser::AttributeDefContext::attrTypeDef() {
  return getRuleContext<Ili2Parser::AttrTypeDefContext>(0);
}

tree::TerminalNode* Ili2Parser::AttributeDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::AttributeDefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

tree::TerminalNode* Ili2Parser::AttributeDefContext::SUBDIVISION() {
  return getToken(Ili2Parser::SUBDIVISION, 0);
}

Ili2Parser::PropertiesContext* Ili2Parser::AttributeDefContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

tree::TerminalNode* Ili2Parser::AttributeDefContext::COLONEQUAL() {
  return getToken(Ili2Parser::COLONEQUAL, 0);
}

std::vector<Ili2Parser::FactorContext *> Ili2Parser::AttributeDefContext::factor() {
  return getRuleContexts<Ili2Parser::FactorContext>();
}

Ili2Parser::FactorContext* Ili2Parser::AttributeDefContext::factor(size_t i) {
  return getRuleContext<Ili2Parser::FactorContext>(i);
}

tree::TerminalNode* Ili2Parser::AttributeDefContext::CONTINUOUS() {
  return getToken(Ili2Parser::CONTINUOUS, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::AttributeDefContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::AttributeDefContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::AttributeDefContext::getRuleIndex() const {
  return Ili2Parser::RuleAttributeDef;
}

antlrcpp::Any Ili2Parser::AttributeDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAttributeDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AttributeDefContext* Ili2Parser::attributeDef() {
  AttributeDefContext *_localctx = _tracker.createInstance<AttributeDefContext>(_ctx, getState());
  enterRule(_localctx, 22, Ili2Parser::RuleAttributeDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(511);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::CONTINUOUS

    || _la == Ili2Parser::SUBDIVISION) {
      setState(508);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == Ili2Parser::CONTINUOUS) {
        setState(507);
        match(Ili2Parser::CONTINUOUS);
      }
      setState(510);
      match(Ili2Parser::SUBDIVISION);
    }
    setState(513);
    dynamic_cast<AttributeDefContext *>(_localctx)->attributname = match(Ili2Parser::NAME);
    setState(515);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(514);
      properties();
    }
    setState(517);
    match(Ili2Parser::COLON);
    setState(518);
    attrTypeDef();
    setState(528);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::COLONEQUAL) {
      setState(519);
      match(Ili2Parser::COLONEQUAL);
      setState(520);
      factor();
      setState(525);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili2Parser::COMMA) {
        setState(521);
        match(Ili2Parser::COMMA);
        setState(522);
        factor();
        setState(527);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(530);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttrTypeDefContext ------------------------------------------------------------------

Ili2Parser::AttrTypeDefContext::AttrTypeDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::AttrTypeDefContext::MANDATORY() {
  return getToken(Ili2Parser::MANDATORY, 0);
}

Ili2Parser::AttrTypeContext* Ili2Parser::AttrTypeDefContext::attrType() {
  return getRuleContext<Ili2Parser::AttrTypeContext>(0);
}

Ili2Parser::BagOrListTypeContext* Ili2Parser::AttrTypeDefContext::bagOrListType() {
  return getRuleContext<Ili2Parser::BagOrListTypeContext>(0);
}


size_t Ili2Parser::AttrTypeDefContext::getRuleIndex() const {
  return Ili2Parser::RuleAttrTypeDef;
}

antlrcpp::Any Ili2Parser::AttrTypeDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAttrTypeDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AttrTypeDefContext* Ili2Parser::attrTypeDef() {
  AttrTypeDefContext *_localctx = _tracker.createInstance<AttrTypeDefContext>(_ctx, getState());
  enterRule(_localctx, 24, Ili2Parser::RuleAttrTypeDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(538);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 47, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(532);
      match(Ili2Parser::MANDATORY);
      setState(534);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 46, _ctx)) {
      case 1: {
        setState(533);
        attrType();
        break;
      }

      }
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(536);
      attrType();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(537);
      bagOrListType();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BagOrListTypeContext ------------------------------------------------------------------

Ili2Parser::BagOrListTypeContext::BagOrListTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::BagOrListTypeContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

tree::TerminalNode* Ili2Parser::BagOrListTypeContext::BAG() {
  return getToken(Ili2Parser::BAG, 0);
}

tree::TerminalNode* Ili2Parser::BagOrListTypeContext::LIST() {
  return getToken(Ili2Parser::LIST, 0);
}

Ili2Parser::RestrictedRefContext* Ili2Parser::BagOrListTypeContext::restrictedRef() {
  return getRuleContext<Ili2Parser::RestrictedRefContext>(0);
}

Ili2Parser::AttrTypeContext* Ili2Parser::BagOrListTypeContext::attrType() {
  return getRuleContext<Ili2Parser::AttrTypeContext>(0);
}

Ili2Parser::CardinalityContext* Ili2Parser::BagOrListTypeContext::cardinality() {
  return getRuleContext<Ili2Parser::CardinalityContext>(0);
}


size_t Ili2Parser::BagOrListTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleBagOrListType;
}

antlrcpp::Any Ili2Parser::BagOrListTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitBagOrListType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::BagOrListTypeContext* Ili2Parser::bagOrListType() {
  BagOrListTypeContext *_localctx = _tracker.createInstance<BagOrListTypeContext>(_ctx, getState());
  enterRule(_localctx, 26, Ili2Parser::RuleBagOrListType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(540);
    _la = _input->LA(1);
    if (!(_la == Ili2Parser::BAG

    || _la == Ili2Parser::LIST)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(542);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LCURLY) {
      setState(541);
      cardinality();
    }
    setState(544);
    match(Ili2Parser::OF);
    setState(549);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 49, _ctx)) {
    case 1: {
      setState(545);

      if (!(ili23)) throw FailedPredicateException(this, "ili23");
      setState(546);
      restrictedRef();
      break;
    }

    case 2: {
      setState(547);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(548);
      attrType();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttrTypeContext ------------------------------------------------------------------

Ili2Parser::AttrTypeContext::AttrTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::TypeContext* Ili2Parser::AttrTypeContext::type() {
  return getRuleContext<Ili2Parser::TypeContext>(0);
}

Ili2Parser::PathContext* Ili2Parser::AttrTypeContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

Ili2Parser::ReferenceAttrContext* Ili2Parser::AttrTypeContext::referenceAttr() {
  return getRuleContext<Ili2Parser::ReferenceAttrContext>(0);
}

Ili2Parser::RestrictedRefContext* Ili2Parser::AttrTypeContext::restrictedRef() {
  return getRuleContext<Ili2Parser::RestrictedRefContext>(0);
}


size_t Ili2Parser::AttrTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleAttrType;
}

antlrcpp::Any Ili2Parser::AttrTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAttrType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AttrTypeContext* Ili2Parser::attrType() {
  AttrTypeContext *_localctx = _tracker.createInstance<AttrTypeContext>(_ctx, getState());
  enterRule(_localctx, 28, Ili2Parser::RuleAttrType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(555);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 50, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(551);
      type();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(552);
      path();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(553);
      referenceAttr();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(554);
      restrictedRef();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ReferenceAttrContext ------------------------------------------------------------------

Ili2Parser::ReferenceAttrContext::ReferenceAttrContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ReferenceAttrContext::REFERENCE() {
  return getToken(Ili2Parser::REFERENCE, 0);
}

tree::TerminalNode* Ili2Parser::ReferenceAttrContext::TO() {
  return getToken(Ili2Parser::TO, 0);
}

Ili2Parser::RestrictedRefContext* Ili2Parser::ReferenceAttrContext::restrictedRef() {
  return getRuleContext<Ili2Parser::RestrictedRefContext>(0);
}

tree::TerminalNode* Ili2Parser::ReferenceAttrContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::ReferenceAttrContext::EXTERNAL() {
  return getToken(Ili2Parser::EXTERNAL, 0);
}

tree::TerminalNode* Ili2Parser::ReferenceAttrContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}


size_t Ili2Parser::ReferenceAttrContext::getRuleIndex() const {
  return Ili2Parser::RuleReferenceAttr;
}

antlrcpp::Any Ili2Parser::ReferenceAttrContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitReferenceAttr(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ReferenceAttrContext* Ili2Parser::referenceAttr() {
  ReferenceAttrContext *_localctx = _tracker.createInstance<ReferenceAttrContext>(_ctx, getState());
  enterRule(_localctx, 30, Ili2Parser::RuleReferenceAttr);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(557);
    match(Ili2Parser::REFERENCE);
    setState(558);
    match(Ili2Parser::TO);
    setState(562);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(559);
      match(Ili2Parser::LPAREN);
      setState(560);
      match(Ili2Parser::EXTERNAL);
      setState(561);
      match(Ili2Parser::RPAREN);
    }
    setState(564);
    restrictedRef();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RestrictedRefContext ------------------------------------------------------------------

Ili2Parser::RestrictedRefContext::RestrictedRefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::RestrictedRefContext::ANYCLASS() {
  return getToken(Ili2Parser::ANYCLASS, 0);
}

tree::TerminalNode* Ili2Parser::RestrictedRefContext::ANYSTRUCTURE() {
  return getToken(Ili2Parser::ANYSTRUCTURE, 0);
}

Ili2Parser::PathContext* Ili2Parser::RestrictedRefContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

Ili2Parser::RestrictionContext* Ili2Parser::RestrictedRefContext::restriction() {
  return getRuleContext<Ili2Parser::RestrictionContext>(0);
}


size_t Ili2Parser::RestrictedRefContext::getRuleIndex() const {
  return Ili2Parser::RuleRestrictedRef;
}

antlrcpp::Any Ili2Parser::RestrictedRefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitRestrictedRef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::RestrictedRefContext* Ili2Parser::restrictedRef() {
  RestrictedRefContext *_localctx = _tracker.createInstance<RestrictedRefContext>(_ctx, getState());
  enterRule(_localctx, 32, Ili2Parser::RuleRestrictedRef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(569);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::INTERLIS:
      case Ili2Parser::REFSYSTEM:
      case Ili2Parser::BOOLEAN:
      case Ili2Parser::HALIGNMENT:
      case Ili2Parser::VALIGNMENT:
      case Ili2Parser::URI:
      case Ili2Parser::SIGN:
      case Ili2Parser::METAOBJECT:
      case Ili2Parser::NAME: {
        setState(566);
        dynamic_cast<RestrictedRefContext *>(_localctx)->typeref = path();
        break;
      }

      case Ili2Parser::ANYCLASS: {
        setState(567);
        match(Ili2Parser::ANYCLASS);
        break;
      }

      case Ili2Parser::ANYSTRUCTURE: {
        setState(568);
        match(Ili2Parser::ANYSTRUCTURE);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(572);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::RESTRICTION) {
      setState(571);
      restriction();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RestrictionContext ------------------------------------------------------------------

Ili2Parser::RestrictionContext::RestrictionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::RestrictionContext::RESTRICTION() {
  return getToken(Ili2Parser::RESTRICTION, 0);
}

tree::TerminalNode* Ili2Parser::RestrictionContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

std::vector<Ili2Parser::PathContext *> Ili2Parser::RestrictionContext::path() {
  return getRuleContexts<Ili2Parser::PathContext>();
}

Ili2Parser::PathContext* Ili2Parser::RestrictionContext::path(size_t i) {
  return getRuleContext<Ili2Parser::PathContext>(i);
}

tree::TerminalNode* Ili2Parser::RestrictionContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::RestrictionContext::SEMI() {
  return getTokens(Ili2Parser::SEMI);
}

tree::TerminalNode* Ili2Parser::RestrictionContext::SEMI(size_t i) {
  return getToken(Ili2Parser::SEMI, i);
}


size_t Ili2Parser::RestrictionContext::getRuleIndex() const {
  return Ili2Parser::RuleRestriction;
}

antlrcpp::Any Ili2Parser::RestrictionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitRestriction(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::RestrictionContext* Ili2Parser::restriction() {
  RestrictionContext *_localctx = _tracker.createInstance<RestrictionContext>(_ctx, getState());
  enterRule(_localctx, 34, Ili2Parser::RuleRestriction);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(574);
    match(Ili2Parser::RESTRICTION);
    setState(575);
    match(Ili2Parser::LPAREN);
    setState(576);
    path();
    setState(581);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::SEMI) {
      setState(577);
      match(Ili2Parser::SEMI);
      setState(578);
      path();
      setState(583);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(584);
    match(Ili2Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssociationDefContext ------------------------------------------------------------------

Ili2Parser::AssociationDefContext::AssociationDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::ASSOCIATION() {
  return getToken(Ili2Parser::ASSOCIATION, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::AssociationDefContext::EQUAL() {
  return getTokens(Ili2Parser::EQUAL);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::EQUAL(size_t i) {
  return getToken(Ili2Parser::EQUAL, i);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::END() {
  return getToken(Ili2Parser::END, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::AssociationDefContext::SEMI() {
  return getTokens(Ili2Parser::SEMI);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::SEMI(size_t i) {
  return getToken(Ili2Parser::SEMI, i);
}

Ili2Parser::PropertiesContext* Ili2Parser::AssociationDefContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::EXTENDS() {
  return getToken(Ili2Parser::EXTENDS, 0);
}

Ili2Parser::AssociationRefContext* Ili2Parser::AssociationDefContext::associationRef() {
  return getRuleContext<Ili2Parser::AssociationRefContext>(0);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::DERIVED() {
  return getToken(Ili2Parser::DERIVED, 0);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::FROM() {
  return getToken(Ili2Parser::FROM, 0);
}

Ili2Parser::RenamedViewableRefContext* Ili2Parser::AssociationDefContext::renamedViewableRef() {
  return getRuleContext<Ili2Parser::RenamedViewableRefContext>(0);
}

std::vector<Ili2Parser::RoleDefContext *> Ili2Parser::AssociationDefContext::roleDef() {
  return getRuleContexts<Ili2Parser::RoleDefContext>();
}

Ili2Parser::RoleDefContext* Ili2Parser::AssociationDefContext::roleDef(size_t i) {
  return getRuleContext<Ili2Parser::RoleDefContext>(i);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::ATTRIBUTE() {
  return getToken(Ili2Parser::ATTRIBUTE, 0);
}

std::vector<Ili2Parser::AttributeDefContext *> Ili2Parser::AssociationDefContext::attributeDef() {
  return getRuleContexts<Ili2Parser::AttributeDefContext>();
}

Ili2Parser::AttributeDefContext* Ili2Parser::AssociationDefContext::attributeDef(size_t i) {
  return getRuleContext<Ili2Parser::AttributeDefContext>(i);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::CARDINALITY() {
  return getToken(Ili2Parser::CARDINALITY, 0);
}

Ili2Parser::CardinalityContext* Ili2Parser::AssociationDefContext::cardinality() {
  return getRuleContext<Ili2Parser::CardinalityContext>(0);
}

std::vector<Ili2Parser::ConstraintDefContext *> Ili2Parser::AssociationDefContext::constraintDef() {
  return getRuleContexts<Ili2Parser::ConstraintDefContext>();
}

Ili2Parser::ConstraintDefContext* Ili2Parser::AssociationDefContext::constraintDef(size_t i) {
  return getRuleContext<Ili2Parser::ConstraintDefContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::AssociationDefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::OID() {
  return getToken(Ili2Parser::OID, 0);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::AS() {
  return getToken(Ili2Parser::AS, 0);
}

tree::TerminalNode* Ili2Parser::AssociationDefContext::NO() {
  return getToken(Ili2Parser::NO, 0);
}

Ili2Parser::PathContext* Ili2Parser::AssociationDefContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::AssociationDefContext::getRuleIndex() const {
  return Ili2Parser::RuleAssociationDef;
}

antlrcpp::Any Ili2Parser::AssociationDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAssociationDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AssociationDefContext* Ili2Parser::associationDef() {
  AssociationDefContext *_localctx = _tracker.createInstance<AssociationDefContext>(_ctx, getState());
  enterRule(_localctx, 36, Ili2Parser::RuleAssociationDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(586);
    match(Ili2Parser::ASSOCIATION);
    setState(588);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::NAME) {
      setState(587);
      dynamic_cast<AssociationDefContext *>(_localctx)->associationname1 = match(Ili2Parser::NAME);
    }
    setState(591);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(590);
      properties();
    }
    setState(595);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXTENDS) {
      setState(593);
      match(Ili2Parser::EXTENDS);
      setState(594);
      associationRef();
    }
    setState(600);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::DERIVED) {
      setState(597);
      match(Ili2Parser::DERIVED);
      setState(598);
      match(Ili2Parser::FROM);
      setState(599);
      renamedViewableRef();
    }
    setState(602);
    match(Ili2Parser::EQUAL);
    setState(611);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::OID

    || _la == Ili2Parser::NO) {
      setState(608);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case Ili2Parser::OID: {
          setState(603);
          match(Ili2Parser::OID);
          setState(604);
          match(Ili2Parser::AS);
          setState(605);
          dynamic_cast<AssociationDefContext *>(_localctx)->assocoid = path();
          break;
        }

        case Ili2Parser::NO: {
          setState(606);
          match(Ili2Parser::NO);
          setState(607);
          match(Ili2Parser::OID);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(610);
      match(Ili2Parser::SEMI);
    }
    setState(616);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 61, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(613);
        roleDef(); 
      }
      setState(618);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 61, _ctx);
    }
    setState(620);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::ATTRIBUTE) {
      setState(619);
      match(Ili2Parser::ATTRIBUTE);
    }
    setState(625);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::CONTINUOUS

    || _la == Ili2Parser::SUBDIVISION || _la == Ili2Parser::NAME) {
      setState(622);
      attributeDef();
      setState(627);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(633);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::CARDINALITY) {
      setState(628);
      match(Ili2Parser::CARDINALITY);
      setState(629);
      match(Ili2Parser::EQUAL);
      setState(630);
      cardinality();
      setState(631);
      match(Ili2Parser::SEMI);
    }
    setState(638);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::MANDATORY

    || _la == Ili2Parser::CONSTRAINT || ((((_la - 152) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 152)) & ((1ULL << (Ili2Parser::EXISTENCE - 152))
      | (1ULL << (Ili2Parser::UNIQUE - 152))
      | (1ULL << (Ili2Parser::SET - 152)))) != 0)) {
      setState(635);
      constraintDef();
      setState(640);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(641);
    match(Ili2Parser::END);
    setState(643);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::NAME) {
      setState(642);
      dynamic_cast<AssociationDefContext *>(_localctx)->associationname2 = match(Ili2Parser::NAME);
    }
    setState(645);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssociationRefContext ------------------------------------------------------------------

Ili2Parser::AssociationRefContext::AssociationRefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> Ili2Parser::AssociationRefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::AssociationRefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::AssociationRefContext::DOT() {
  return getTokens(Ili2Parser::DOT);
}

tree::TerminalNode* Ili2Parser::AssociationRefContext::DOT(size_t i) {
  return getToken(Ili2Parser::DOT, i);
}


size_t Ili2Parser::AssociationRefContext::getRuleIndex() const {
  return Ili2Parser::RuleAssociationRef;
}

antlrcpp::Any Ili2Parser::AssociationRefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAssociationRef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AssociationRefContext* Ili2Parser::associationRef() {
  AssociationRefContext *_localctx = _tracker.createInstance<AssociationRefContext>(_ctx, getState());
  enterRule(_localctx, 38, Ili2Parser::RuleAssociationRef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(653);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 68, _ctx)) {
    case 1: {
      setState(649);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 67, _ctx)) {
      case 1: {
        setState(647);
        dynamic_cast<AssociationRefContext *>(_localctx)->modelname = match(Ili2Parser::NAME);
        setState(648);
        match(Ili2Parser::DOT);
        break;
      }

      }
      setState(651);
      dynamic_cast<AssociationRefContext *>(_localctx)->topicname = match(Ili2Parser::NAME);
      setState(652);
      match(Ili2Parser::DOT);
      break;
    }

    }
    setState(655);
    dynamic_cast<AssociationRefContext *>(_localctx)->associationname = match(Ili2Parser::NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RoleDefContext ------------------------------------------------------------------

Ili2Parser::RoleDefContext::RoleDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<Ili2Parser::RestrictedRefContext *> Ili2Parser::RoleDefContext::restrictedRef() {
  return getRuleContexts<Ili2Parser::RestrictedRefContext>();
}

Ili2Parser::RestrictedRefContext* Ili2Parser::RoleDefContext::restrictedRef(size_t i) {
  return getRuleContext<Ili2Parser::RestrictedRefContext>(i);
}

tree::TerminalNode* Ili2Parser::RoleDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::RoleDefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

tree::TerminalNode* Ili2Parser::RoleDefContext::ASSOCIATE() {
  return getToken(Ili2Parser::ASSOCIATE, 0);
}

tree::TerminalNode* Ili2Parser::RoleDefContext::AGGREGATE() {
  return getToken(Ili2Parser::AGGREGATE, 0);
}

tree::TerminalNode* Ili2Parser::RoleDefContext::COMPOSITE() {
  return getToken(Ili2Parser::COMPOSITE, 0);
}

Ili2Parser::PropertiesContext* Ili2Parser::RoleDefContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

Ili2Parser::CardinalityContext* Ili2Parser::RoleDefContext::cardinality() {
  return getRuleContext<Ili2Parser::CardinalityContext>(0);
}

std::vector<tree::TerminalNode *> Ili2Parser::RoleDefContext::OR() {
  return getTokens(Ili2Parser::OR);
}

tree::TerminalNode* Ili2Parser::RoleDefContext::OR(size_t i) {
  return getToken(Ili2Parser::OR, i);
}

tree::TerminalNode* Ili2Parser::RoleDefContext::COLONEQUAL() {
  return getToken(Ili2Parser::COLONEQUAL, 0);
}

Ili2Parser::FactorContext* Ili2Parser::RoleDefContext::factor() {
  return getRuleContext<Ili2Parser::FactorContext>(0);
}


size_t Ili2Parser::RoleDefContext::getRuleIndex() const {
  return Ili2Parser::RuleRoleDef;
}

antlrcpp::Any Ili2Parser::RoleDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitRoleDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::RoleDefContext* Ili2Parser::roleDef() {
  RoleDefContext *_localctx = _tracker.createInstance<RoleDefContext>(_ctx, getState());
  enterRule(_localctx, 40, Ili2Parser::RuleRoleDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(657);
    dynamic_cast<RoleDefContext *>(_localctx)->rolename = match(Ili2Parser::NAME);
    setState(659);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(658);
      properties();
    }
    setState(661);
    _la = _input->LA(1);
    if (!(((((_la - 83) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 83)) & ((1ULL << (Ili2Parser::AGGREGATE - 83))
      | (1ULL << (Ili2Parser::ASSOCIATE - 83))
      | (1ULL << (Ili2Parser::COMPOSITE - 83)))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(663);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LCURLY) {
      setState(662);
      cardinality();
    }
    setState(665);
    restrictedRef();
    setState(670);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::OR) {
      setState(666);
      match(Ili2Parser::OR);
      setState(667);
      restrictedRef();
      setState(672);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(675);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::COLONEQUAL) {
      setState(673);
      match(Ili2Parser::COLONEQUAL);
      setState(674);
      dynamic_cast<RoleDefContext *>(_localctx)->role = factor();
    }
    setState(677);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CardinalityContext ------------------------------------------------------------------

Ili2Parser::CardinalityContext::CardinalityContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::CardinalityContext::LCURLY() {
  return getToken(Ili2Parser::LCURLY, 0);
}

tree::TerminalNode* Ili2Parser::CardinalityContext::RCURLY() {
  return getToken(Ili2Parser::RCURLY, 0);
}

tree::TerminalNode* Ili2Parser::CardinalityContext::STAR() {
  return getToken(Ili2Parser::STAR, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::CardinalityContext::POSNUMBER() {
  return getTokens(Ili2Parser::POSNUMBER);
}

tree::TerminalNode* Ili2Parser::CardinalityContext::POSNUMBER(size_t i) {
  return getToken(Ili2Parser::POSNUMBER, i);
}

tree::TerminalNode* Ili2Parser::CardinalityContext::DOTDOT() {
  return getToken(Ili2Parser::DOTDOT, 0);
}


size_t Ili2Parser::CardinalityContext::getRuleIndex() const {
  return Ili2Parser::RuleCardinality;
}

antlrcpp::Any Ili2Parser::CardinalityContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitCardinality(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::CardinalityContext* Ili2Parser::cardinality() {
  CardinalityContext *_localctx = _tracker.createInstance<CardinalityContext>(_ctx, getState());
  enterRule(_localctx, 42, Ili2Parser::RuleCardinality);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(679);
    match(Ili2Parser::LCURLY);
    setState(689);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::STAR: {
        setState(680);
        match(Ili2Parser::STAR);
        break;
      }

      case Ili2Parser::POSNUMBER: {
        setState(681);
        dynamic_cast<CardinalityContext *>(_localctx)->min = match(Ili2Parser::POSNUMBER);
        setState(687);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == Ili2Parser::DOTDOT) {
          setState(682);
          match(Ili2Parser::DOTDOT);
          setState(685);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case Ili2Parser::POSNUMBER: {
              setState(683);
              dynamic_cast<CardinalityContext *>(_localctx)->max = match(Ili2Parser::POSNUMBER);
              break;
            }

            case Ili2Parser::STAR: {
              setState(684);
              match(Ili2Parser::STAR);
              break;
            }

          default:
            throw NoViableAltException(this);
          }
        }
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(691);
    match(Ili2Parser::RCURLY);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DomainDefContext ------------------------------------------------------------------

Ili2Parser::DomainDefContext::DomainDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::DomainDefContext::ILIDOMAIN() {
  return getToken(Ili2Parser::ILIDOMAIN, 0);
}

std::vector<Ili2Parser::DomainTypeContext *> Ili2Parser::DomainDefContext::domainType() {
  return getRuleContexts<Ili2Parser::DomainTypeContext>();
}

Ili2Parser::DomainTypeContext* Ili2Parser::DomainDefContext::domainType(size_t i) {
  return getRuleContext<Ili2Parser::DomainTypeContext>(i);
}


size_t Ili2Parser::DomainDefContext::getRuleIndex() const {
  return Ili2Parser::RuleDomainDef;
}

antlrcpp::Any Ili2Parser::DomainDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitDomainDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::DomainDefContext* Ili2Parser::domainDef() {
  DomainDefContext *_localctx = _tracker.createInstance<DomainDefContext>(_ctx, getState());
  enterRule(_localctx, 44, Ili2Parser::RuleDomainDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(693);
    match(Ili2Parser::ILIDOMAIN);
    setState(697);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 76, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(694);
        domainType(); 
      }
      setState(699);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 76, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DomainTypeContext ------------------------------------------------------------------

Ili2Parser::DomainTypeContext::DomainTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::DomainTypeContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

tree::TerminalNode* Ili2Parser::DomainTypeContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::DomainTypeContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::DomainTypeContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

tree::TerminalNode* Ili2Parser::DomainTypeContext::MANDATORY() {
  return getToken(Ili2Parser::MANDATORY, 0);
}

Ili2Parser::TypeContext* Ili2Parser::DomainTypeContext::type() {
  return getRuleContext<Ili2Parser::TypeContext>(0);
}

Ili2Parser::PropertiesContext* Ili2Parser::DomainTypeContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

tree::TerminalNode* Ili2Parser::DomainTypeContext::EXTENDS() {
  return getToken(Ili2Parser::EXTENDS, 0);
}

tree::TerminalNode* Ili2Parser::DomainTypeContext::CONSTRAINTS() {
  return getToken(Ili2Parser::CONSTRAINTS, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::DomainTypeContext::COLON() {
  return getTokens(Ili2Parser::COLON);
}

tree::TerminalNode* Ili2Parser::DomainTypeContext::COLON(size_t i) {
  return getToken(Ili2Parser::COLON, i);
}

std::vector<Ili2Parser::ExpressionContext *> Ili2Parser::DomainTypeContext::expression() {
  return getRuleContexts<Ili2Parser::ExpressionContext>();
}

Ili2Parser::ExpressionContext* Ili2Parser::DomainTypeContext::expression(size_t i) {
  return getRuleContext<Ili2Parser::ExpressionContext>(i);
}

Ili2Parser::PathContext* Ili2Parser::DomainTypeContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

std::vector<tree::TerminalNode *> Ili2Parser::DomainTypeContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::DomainTypeContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::DomainTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleDomainType;
}

antlrcpp::Any Ili2Parser::DomainTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitDomainType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::DomainTypeContext* Ili2Parser::domainType() {
  DomainTypeContext *_localctx = _tracker.createInstance<DomainTypeContext>(_ctx, getState());
  enterRule(_localctx, 46, Ili2Parser::RuleDomainType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(700);
    dynamic_cast<DomainTypeContext *>(_localctx)->domainname = match(Ili2Parser::NAME);
    setState(702);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(701);
      properties();
    }
    setState(706);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXTENDS) {
      setState(704);
      match(Ili2Parser::EXTENDS);
      setState(705);
      dynamic_cast<DomainTypeContext *>(_localctx)->basedomain = path();
    }
    setState(708);
    match(Ili2Parser::EQUAL);
    setState(714);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 80, _ctx)) {
    case 1: {
      setState(709);
      match(Ili2Parser::MANDATORY);
      setState(711);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 79, _ctx)) {
      case 1: {
        setState(710);
        type();
        break;
      }

      }
      break;
    }

    case 2: {
      setState(713);
      type();
      break;
    }

    }
    setState(730);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 82, _ctx)) {
    case 1: {
      setState(716);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(717);
      match(Ili2Parser::CONSTRAINTS);
      setState(718);
      match(Ili2Parser::NAME);
      setState(719);
      match(Ili2Parser::COLON);
      setState(720);
      expression();
      setState(727);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili2Parser::COMMA) {
        setState(721);
        match(Ili2Parser::COMMA);
        setState(722);
        match(Ili2Parser::NAME);
        setState(723);
        match(Ili2Parser::COLON);
        setState(724);
        expression();
        setState(729);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      break;
    }

    }
    setState(732);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext ------------------------------------------------------------------

Ili2Parser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::BaseTypeContext* Ili2Parser::TypeContext::baseType() {
  return getRuleContext<Ili2Parser::BaseTypeContext>(0);
}

Ili2Parser::LineTypeContext* Ili2Parser::TypeContext::lineType() {
  return getRuleContext<Ili2Parser::LineTypeContext>(0);
}


size_t Ili2Parser::TypeContext::getRuleIndex() const {
  return Ili2Parser::RuleType;
}

antlrcpp::Any Ili2Parser::TypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::TypeContext* Ili2Parser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 48, Ili2Parser::RuleType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(736);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 83, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(734);
      baseType();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(735);
      lineType();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BaseTypeContext ------------------------------------------------------------------

Ili2Parser::BaseTypeContext::BaseTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::TextTypeContext* Ili2Parser::BaseTypeContext::textType() {
  return getRuleContext<Ili2Parser::TextTypeContext>(0);
}

Ili2Parser::EnumerationTypeContext* Ili2Parser::BaseTypeContext::enumerationType() {
  return getRuleContext<Ili2Parser::EnumerationTypeContext>(0);
}

Ili2Parser::EnumTreeValueTypeContext* Ili2Parser::BaseTypeContext::enumTreeValueType() {
  return getRuleContext<Ili2Parser::EnumTreeValueTypeContext>(0);
}

Ili2Parser::AlignmentTypeContext* Ili2Parser::BaseTypeContext::alignmentType() {
  return getRuleContext<Ili2Parser::AlignmentTypeContext>(0);
}

Ili2Parser::BooleanTypeContext* Ili2Parser::BaseTypeContext::booleanType() {
  return getRuleContext<Ili2Parser::BooleanTypeContext>(0);
}

Ili2Parser::NumericTypeContext* Ili2Parser::BaseTypeContext::numericType() {
  return getRuleContext<Ili2Parser::NumericTypeContext>(0);
}

Ili2Parser::FormattedTypeContext* Ili2Parser::BaseTypeContext::formattedType() {
  return getRuleContext<Ili2Parser::FormattedTypeContext>(0);
}

Ili2Parser::DateTimeTypeContext* Ili2Parser::BaseTypeContext::dateTimeType() {
  return getRuleContext<Ili2Parser::DateTimeTypeContext>(0);
}

Ili2Parser::CoordinateTypeContext* Ili2Parser::BaseTypeContext::coordinateType() {
  return getRuleContext<Ili2Parser::CoordinateTypeContext>(0);
}

Ili2Parser::OIDTypeContext* Ili2Parser::BaseTypeContext::oIDType() {
  return getRuleContext<Ili2Parser::OIDTypeContext>(0);
}

Ili2Parser::BlackboxTypeContext* Ili2Parser::BaseTypeContext::blackboxType() {
  return getRuleContext<Ili2Parser::BlackboxTypeContext>(0);
}

Ili2Parser::ClassRefTypeContext* Ili2Parser::BaseTypeContext::classRefType() {
  return getRuleContext<Ili2Parser::ClassRefTypeContext>(0);
}

Ili2Parser::AttributePathTypeContext* Ili2Parser::BaseTypeContext::attributePathType() {
  return getRuleContext<Ili2Parser::AttributePathTypeContext>(0);
}


size_t Ili2Parser::BaseTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleBaseType;
}

antlrcpp::Any Ili2Parser::BaseTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitBaseType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::BaseTypeContext* Ili2Parser::baseType() {
  BaseTypeContext *_localctx = _tracker.createInstance<BaseTypeContext>(_ctx, getState());
  enterRule(_localctx, 50, Ili2Parser::RuleBaseType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(752);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 84, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(738);
      textType();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(739);
      enumerationType();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(740);
      enumTreeValueType();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(741);
      alignmentType();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(742);
      booleanType();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(743);
      numericType();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(744);
      formattedType();
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(745);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(746);
      dateTimeType();
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(747);
      coordinateType();
      break;
    }

    case 10: {
      enterOuterAlt(_localctx, 10);
      setState(748);
      oIDType();
      break;
    }

    case 11: {
      enterOuterAlt(_localctx, 11);
      setState(749);
      blackboxType();
      break;
    }

    case 12: {
      enterOuterAlt(_localctx, 12);
      setState(750);
      classRefType();
      break;
    }

    case 13: {
      enterOuterAlt(_localctx, 13);
      setState(751);
      attributePathType();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstantContext ------------------------------------------------------------------

Ili2Parser::ConstantContext::ConstantContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ConstantContext::UNDEFINED() {
  return getToken(Ili2Parser::UNDEFINED, 0);
}

Ili2Parser::NumericConstContext* Ili2Parser::ConstantContext::numericConst() {
  return getRuleContext<Ili2Parser::NumericConstContext>(0);
}

Ili2Parser::TextConstContext* Ili2Parser::ConstantContext::textConst() {
  return getRuleContext<Ili2Parser::TextConstContext>(0);
}

Ili2Parser::FormattedConstContext* Ili2Parser::ConstantContext::formattedConst() {
  return getRuleContext<Ili2Parser::FormattedConstContext>(0);
}

Ili2Parser::EnumConstContext* Ili2Parser::ConstantContext::enumConst() {
  return getRuleContext<Ili2Parser::EnumConstContext>(0);
}

Ili2Parser::ClassConstContext* Ili2Parser::ConstantContext::classConst() {
  return getRuleContext<Ili2Parser::ClassConstContext>(0);
}

Ili2Parser::AttributePathConstContext* Ili2Parser::ConstantContext::attributePathConst() {
  return getRuleContext<Ili2Parser::AttributePathConstContext>(0);
}


size_t Ili2Parser::ConstantContext::getRuleIndex() const {
  return Ili2Parser::RuleConstant;
}

antlrcpp::Any Ili2Parser::ConstantContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitConstant(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ConstantContext* Ili2Parser::constant() {
  ConstantContext *_localctx = _tracker.createInstance<ConstantContext>(_ctx, getState());
  enterRule(_localctx, 52, Ili2Parser::RuleConstant);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(761);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 85, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(754);
      match(Ili2Parser::UNDEFINED);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(755);
      numericConst();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(756);
      textConst();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(757);
      formattedConst();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(758);
      enumConst();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(759);
      classConst();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(760);
      attributePathConst();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TextTypeContext ------------------------------------------------------------------

Ili2Parser::TextTypeContext::TextTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::TextTypeContext::MTEXT() {
  return getToken(Ili2Parser::MTEXT, 0);
}

tree::TerminalNode* Ili2Parser::TextTypeContext::STAR() {
  return getToken(Ili2Parser::STAR, 0);
}

tree::TerminalNode* Ili2Parser::TextTypeContext::POSNUMBER() {
  return getToken(Ili2Parser::POSNUMBER, 0);
}

tree::TerminalNode* Ili2Parser::TextTypeContext::TEXT() {
  return getToken(Ili2Parser::TEXT, 0);
}

tree::TerminalNode* Ili2Parser::TextTypeContext::NAME_CONST() {
  return getToken(Ili2Parser::NAME_CONST, 0);
}

tree::TerminalNode* Ili2Parser::TextTypeContext::URI() {
  return getToken(Ili2Parser::URI, 0);
}


size_t Ili2Parser::TextTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleTextType;
}

antlrcpp::Any Ili2Parser::TextTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitTextType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::TextTypeContext* Ili2Parser::textType() {
  TextTypeContext *_localctx = _tracker.createInstance<TextTypeContext>(_ctx, getState());
  enterRule(_localctx, 54, Ili2Parser::RuleTextType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(775);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::MTEXT: {
        enterOuterAlt(_localctx, 1);
        setState(763);
        match(Ili2Parser::MTEXT);
        setState(766);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 86, _ctx)) {
        case 1: {
          setState(764);
          match(Ili2Parser::STAR);
          setState(765);
          dynamic_cast<TextTypeContext *>(_localctx)->maxlength = match(Ili2Parser::POSNUMBER);
          break;
        }

        }
        break;
      }

      case Ili2Parser::TEXT: {
        enterOuterAlt(_localctx, 2);
        setState(768);
        match(Ili2Parser::TEXT);
        setState(771);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 87, _ctx)) {
        case 1: {
          setState(769);
          match(Ili2Parser::STAR);
          setState(770);
          dynamic_cast<TextTypeContext *>(_localctx)->maxlength = match(Ili2Parser::POSNUMBER);
          break;
        }

        }
        break;
      }

      case Ili2Parser::NAME_CONST: {
        enterOuterAlt(_localctx, 3);
        setState(773);
        match(Ili2Parser::NAME_CONST);
        break;
      }

      case Ili2Parser::URI: {
        enterOuterAlt(_localctx, 4);
        setState(774);
        match(Ili2Parser::URI);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TextConstContext ------------------------------------------------------------------

Ili2Parser::TextConstContext::TextConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::TextConstContext::STRING() {
  return getToken(Ili2Parser::STRING, 0);
}


size_t Ili2Parser::TextConstContext::getRuleIndex() const {
  return Ili2Parser::RuleTextConst;
}

antlrcpp::Any Ili2Parser::TextConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitTextConst(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::TextConstContext* Ili2Parser::textConst() {
  TextConstContext *_localctx = _tracker.createInstance<TextConstContext>(_ctx, getState());
  enterRule(_localctx, 56, Ili2Parser::RuleTextConst);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(777);
    dynamic_cast<TextConstContext *>(_localctx)->textconst = match(Ili2Parser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumerationTypeContext ------------------------------------------------------------------

Ili2Parser::EnumerationTypeContext::EnumerationTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::EnumerationContext* Ili2Parser::EnumerationTypeContext::enumeration() {
  return getRuleContext<Ili2Parser::EnumerationContext>(0);
}

tree::TerminalNode* Ili2Parser::EnumerationTypeContext::ORDERED() {
  return getToken(Ili2Parser::ORDERED, 0);
}

tree::TerminalNode* Ili2Parser::EnumerationTypeContext::CIRCULAR() {
  return getToken(Ili2Parser::CIRCULAR, 0);
}


size_t Ili2Parser::EnumerationTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleEnumerationType;
}

antlrcpp::Any Ili2Parser::EnumerationTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitEnumerationType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::EnumerationTypeContext* Ili2Parser::enumerationType() {
  EnumerationTypeContext *_localctx = _tracker.createInstance<EnumerationTypeContext>(_ctx, getState());
  enterRule(_localctx, 58, Ili2Parser::RuleEnumerationType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(779);
    enumeration();
    setState(781);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 89, _ctx)) {
    case 1: {
      setState(780);
      _la = _input->LA(1);
      if (!(_la == Ili2Parser::CIRCULAR

      || _la == Ili2Parser::ORDERED)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumTreeValueTypeContext ------------------------------------------------------------------

Ili2Parser::EnumTreeValueTypeContext::EnumTreeValueTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::EnumTreeValueTypeContext::ALL() {
  return getToken(Ili2Parser::ALL, 0);
}

tree::TerminalNode* Ili2Parser::EnumTreeValueTypeContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

Ili2Parser::PathContext* Ili2Parser::EnumTreeValueTypeContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::EnumTreeValueTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleEnumTreeValueType;
}

antlrcpp::Any Ili2Parser::EnumTreeValueTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitEnumTreeValueType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::EnumTreeValueTypeContext* Ili2Parser::enumTreeValueType() {
  EnumTreeValueTypeContext *_localctx = _tracker.createInstance<EnumTreeValueTypeContext>(_ctx, getState());
  enterRule(_localctx, 60, Ili2Parser::RuleEnumTreeValueType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(783);
    match(Ili2Parser::ALL);
    setState(784);
    match(Ili2Parser::OF);
    setState(785);
    dynamic_cast<EnumTreeValueTypeContext *>(_localctx)->typeref = path();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AlignmentTypeContext ------------------------------------------------------------------

Ili2Parser::AlignmentTypeContext::AlignmentTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::AlignmentTypeContext::HALIGNMENT() {
  return getToken(Ili2Parser::HALIGNMENT, 0);
}

tree::TerminalNode* Ili2Parser::AlignmentTypeContext::VALIGNMENT() {
  return getToken(Ili2Parser::VALIGNMENT, 0);
}


size_t Ili2Parser::AlignmentTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleAlignmentType;
}

antlrcpp::Any Ili2Parser::AlignmentTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAlignmentType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AlignmentTypeContext* Ili2Parser::alignmentType() {
  AlignmentTypeContext *_localctx = _tracker.createInstance<AlignmentTypeContext>(_ctx, getState());
  enterRule(_localctx, 62, Ili2Parser::RuleAlignmentType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(787);
    _la = _input->LA(1);
    if (!(_la == Ili2Parser::HALIGNMENT

    || _la == Ili2Parser::VALIGNMENT)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BooleanTypeContext ------------------------------------------------------------------

Ili2Parser::BooleanTypeContext::BooleanTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::BooleanTypeContext::BOOLEAN() {
  return getToken(Ili2Parser::BOOLEAN, 0);
}


size_t Ili2Parser::BooleanTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleBooleanType;
}

antlrcpp::Any Ili2Parser::BooleanTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitBooleanType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::BooleanTypeContext* Ili2Parser::booleanType() {
  BooleanTypeContext *_localctx = _tracker.createInstance<BooleanTypeContext>(_ctx, getState());
  enterRule(_localctx, 64, Ili2Parser::RuleBooleanType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(789);
    match(Ili2Parser::BOOLEAN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumerationContext ------------------------------------------------------------------

Ili2Parser::EnumerationContext::EnumerationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::EnumerationContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

std::vector<Ili2Parser::EnumElementContext *> Ili2Parser::EnumerationContext::enumElement() {
  return getRuleContexts<Ili2Parser::EnumElementContext>();
}

Ili2Parser::EnumElementContext* Ili2Parser::EnumerationContext::enumElement(size_t i) {
  return getRuleContext<Ili2Parser::EnumElementContext>(i);
}

tree::TerminalNode* Ili2Parser::EnumerationContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::EnumerationContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::EnumerationContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}

tree::TerminalNode* Ili2Parser::EnumerationContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

tree::TerminalNode* Ili2Parser::EnumerationContext::FINAL() {
  return getToken(Ili2Parser::FINAL, 0);
}


size_t Ili2Parser::EnumerationContext::getRuleIndex() const {
  return Ili2Parser::RuleEnumeration;
}

antlrcpp::Any Ili2Parser::EnumerationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitEnumeration(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::EnumerationContext* Ili2Parser::enumeration() {
  EnumerationContext *_localctx = _tracker.createInstance<EnumerationContext>(_ctx, getState());
  enterRule(_localctx, 66, Ili2Parser::RuleEnumeration);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(809);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 92, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(791);
      match(Ili2Parser::LPAREN);
      setState(792);
      enumElement();
      setState(797);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili2Parser::COMMA) {
        setState(793);
        match(Ili2Parser::COMMA);
        setState(794);
        enumElement();
        setState(799);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(802);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == Ili2Parser::COLON) {
        setState(800);
        match(Ili2Parser::COLON);
        setState(801);
        match(Ili2Parser::FINAL);
      }
      setState(804);
      match(Ili2Parser::RPAREN);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(806);
      match(Ili2Parser::LPAREN);
      setState(807);
      match(Ili2Parser::FINAL);
      setState(808);
      match(Ili2Parser::RPAREN);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumElementContext ------------------------------------------------------------------

Ili2Parser::EnumElementContext::EnumElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> Ili2Parser::EnumElementContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::EnumElementContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::EnumElementContext::DOT() {
  return getTokens(Ili2Parser::DOT);
}

tree::TerminalNode* Ili2Parser::EnumElementContext::DOT(size_t i) {
  return getToken(Ili2Parser::DOT, i);
}

Ili2Parser::EnumerationContext* Ili2Parser::EnumElementContext::enumeration() {
  return getRuleContext<Ili2Parser::EnumerationContext>(0);
}


size_t Ili2Parser::EnumElementContext::getRuleIndex() const {
  return Ili2Parser::RuleEnumElement;
}

antlrcpp::Any Ili2Parser::EnumElementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitEnumElement(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::EnumElementContext* Ili2Parser::enumElement() {
  EnumElementContext *_localctx = _tracker.createInstance<EnumElementContext>(_ctx, getState());
  enterRule(_localctx, 68, Ili2Parser::RuleEnumElement);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(811);
    match(Ili2Parser::NAME);
    setState(816);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::DOT) {
      setState(812);
      match(Ili2Parser::DOT);
      setState(813);
      match(Ili2Parser::NAME);
      setState(818);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(820);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(819);
      dynamic_cast<EnumElementContext *>(_localctx)->sub = enumeration();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumConstContext ------------------------------------------------------------------

Ili2Parser::EnumConstContext::EnumConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::EnumConstContext::HASH() {
  return getToken(Ili2Parser::HASH, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::EnumConstContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::EnumConstContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

tree::TerminalNode* Ili2Parser::EnumConstContext::OTHERS() {
  return getToken(Ili2Parser::OTHERS, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::EnumConstContext::DOT() {
  return getTokens(Ili2Parser::DOT);
}

tree::TerminalNode* Ili2Parser::EnumConstContext::DOT(size_t i) {
  return getToken(Ili2Parser::DOT, i);
}


size_t Ili2Parser::EnumConstContext::getRuleIndex() const {
  return Ili2Parser::RuleEnumConst;
}

antlrcpp::Any Ili2Parser::EnumConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitEnumConst(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::EnumConstContext* Ili2Parser::enumConst() {
  EnumConstContext *_localctx = _tracker.createInstance<EnumConstContext>(_ctx, getState());
  enterRule(_localctx, 70, Ili2Parser::RuleEnumConst);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(822);
    match(Ili2Parser::HASH);
    setState(836);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::NAME: {
        setState(823);
        match(Ili2Parser::NAME);
        setState(828);
        _errHandler->sync(this);
        alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 95, _ctx);
        while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
          if (alt == 1) {
            setState(824);
            match(Ili2Parser::DOT);
            setState(825);
            match(Ili2Parser::NAME); 
          }
          setState(830);
          _errHandler->sync(this);
          alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 95, _ctx);
        }
        setState(833);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 96, _ctx)) {
        case 1: {
          setState(831);
          match(Ili2Parser::DOT);
          setState(832);
          match(Ili2Parser::OTHERS);
          break;
        }

        }
        break;
      }

      case Ili2Parser::OTHERS: {
        setState(835);
        match(Ili2Parser::OTHERS);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumericTypeContext ------------------------------------------------------------------

Ili2Parser::NumericTypeContext::NumericTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::NumericTypeContext::DOTDOT() {
  return getToken(Ili2Parser::DOTDOT, 0);
}

tree::TerminalNode* Ili2Parser::NumericTypeContext::NUMERIC() {
  return getToken(Ili2Parser::NUMERIC, 0);
}

std::vector<Ili2Parser::DecimalContext *> Ili2Parser::NumericTypeContext::decimal() {
  return getRuleContexts<Ili2Parser::DecimalContext>();
}

Ili2Parser::DecimalContext* Ili2Parser::NumericTypeContext::decimal(size_t i) {
  return getRuleContext<Ili2Parser::DecimalContext>(i);
}

tree::TerminalNode* Ili2Parser::NumericTypeContext::CIRCULAR() {
  return getToken(Ili2Parser::CIRCULAR, 0);
}

tree::TerminalNode* Ili2Parser::NumericTypeContext::LBRACE() {
  return getToken(Ili2Parser::LBRACE, 0);
}

tree::TerminalNode* Ili2Parser::NumericTypeContext::RBRACE() {
  return getToken(Ili2Parser::RBRACE, 0);
}

tree::TerminalNode* Ili2Parser::NumericTypeContext::CLOCKWISE() {
  return getToken(Ili2Parser::CLOCKWISE, 0);
}

tree::TerminalNode* Ili2Parser::NumericTypeContext::COUNTERCLOCKWISE() {
  return getToken(Ili2Parser::COUNTERCLOCKWISE, 0);
}

Ili2Parser::RefSysContext* Ili2Parser::NumericTypeContext::refSys() {
  return getRuleContext<Ili2Parser::RefSysContext>(0);
}

Ili2Parser::PathContext* Ili2Parser::NumericTypeContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::NumericTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleNumericType;
}

antlrcpp::Any Ili2Parser::NumericTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitNumericType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::NumericTypeContext* Ili2Parser::numericType() {
  NumericTypeContext *_localctx = _tracker.createInstance<NumericTypeContext>(_ctx, getState());
  enterRule(_localctx, 72, Ili2Parser::RuleNumericType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(843);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::POSNUMBER:
      case Ili2Parser::NUMBER:
      case Ili2Parser::DEC: {
        setState(838);
        dynamic_cast<NumericTypeContext *>(_localctx)->min = decimal();
        setState(839);
        match(Ili2Parser::DOTDOT);
        setState(840);
        dynamic_cast<NumericTypeContext *>(_localctx)->max = decimal();
        break;
      }

      case Ili2Parser::NUMERIC: {
        setState(842);
        match(Ili2Parser::NUMERIC);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(846);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 99, _ctx)) {
    case 1: {
      setState(845);
      match(Ili2Parser::CIRCULAR);
      break;
    }

    }
    setState(852);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 100, _ctx)) {
    case 1: {
      setState(848);
      match(Ili2Parser::LBRACE);
      setState(849);
      dynamic_cast<NumericTypeContext *>(_localctx)->unitref = path();
      setState(850);
      match(Ili2Parser::RBRACE);
      break;
    }

    }
    setState(857);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 101, _ctx)) {
    case 1: {
      setState(854);
      match(Ili2Parser::CLOCKWISE);
      break;
    }

    case 2: {
      setState(855);
      match(Ili2Parser::COUNTERCLOCKWISE);
      break;
    }

    case 3: {
      setState(856);
      refSys();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DateTimeTypeContext ------------------------------------------------------------------

Ili2Parser::DateTimeTypeContext::DateTimeTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::DateTimeTypeContext::DATE() {
  return getToken(Ili2Parser::DATE, 0);
}

tree::TerminalNode* Ili2Parser::DateTimeTypeContext::TIMEOFDAY() {
  return getToken(Ili2Parser::TIMEOFDAY, 0);
}

tree::TerminalNode* Ili2Parser::DateTimeTypeContext::DATETIME() {
  return getToken(Ili2Parser::DATETIME, 0);
}


size_t Ili2Parser::DateTimeTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleDateTimeType;
}

antlrcpp::Any Ili2Parser::DateTimeTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitDateTimeType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::DateTimeTypeContext* Ili2Parser::dateTimeType() {
  DateTimeTypeContext *_localctx = _tracker.createInstance<DateTimeTypeContext>(_ctx, getState());
  enterRule(_localctx, 74, Ili2Parser::RuleDateTimeType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(859);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << Ili2Parser::DATE)
      | (1ULL << Ili2Parser::TIMEOFDAY)
      | (1ULL << Ili2Parser::DATETIME))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RefSysContext ------------------------------------------------------------------

Ili2Parser::RefSysContext::RefSysContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::RefSysContext::LCURLY() {
  return getToken(Ili2Parser::LCURLY, 0);
}

tree::TerminalNode* Ili2Parser::RefSysContext::RCURLY() {
  return getToken(Ili2Parser::RCURLY, 0);
}

tree::TerminalNode* Ili2Parser::RefSysContext::LESS() {
  return getToken(Ili2Parser::LESS, 0);
}

tree::TerminalNode* Ili2Parser::RefSysContext::GREATER() {
  return getToken(Ili2Parser::GREATER, 0);
}

Ili2Parser::MetaObjectRefContext* Ili2Parser::RefSysContext::metaObjectRef() {
  return getRuleContext<Ili2Parser::MetaObjectRefContext>(0);
}

Ili2Parser::PathContext* Ili2Parser::RefSysContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

tree::TerminalNode* Ili2Parser::RefSysContext::LBRACE() {
  return getToken(Ili2Parser::LBRACE, 0);
}

tree::TerminalNode* Ili2Parser::RefSysContext::RBRACE() {
  return getToken(Ili2Parser::RBRACE, 0);
}

tree::TerminalNode* Ili2Parser::RefSysContext::POSNUMBER() {
  return getToken(Ili2Parser::POSNUMBER, 0);
}


size_t Ili2Parser::RefSysContext::getRuleIndex() const {
  return Ili2Parser::RuleRefSys;
}

antlrcpp::Any Ili2Parser::RefSysContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitRefSys(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::RefSysContext* Ili2Parser::refSys() {
  RefSysContext *_localctx = _tracker.createInstance<RefSysContext>(_ctx, getState());
  enterRule(_localctx, 76, Ili2Parser::RuleRefSys);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(879);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::LCURLY: {
        setState(861);
        match(Ili2Parser::LCURLY);
        setState(862);
        dynamic_cast<RefSysContext *>(_localctx)->refsys = metaObjectRef();
        setState(866);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == Ili2Parser::LBRACE) {
          setState(863);
          match(Ili2Parser::LBRACE);
          setState(864);
          dynamic_cast<RefSysContext *>(_localctx)->axis = match(Ili2Parser::POSNUMBER);
          setState(865);
          match(Ili2Parser::RBRACE);
        }
        setState(868);
        match(Ili2Parser::RCURLY);
        break;
      }

      case Ili2Parser::LESS: {
        setState(870);
        match(Ili2Parser::LESS);
        setState(871);
        dynamic_cast<RefSysContext *>(_localctx)->coord = path();
        setState(875);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == Ili2Parser::LBRACE) {
          setState(872);
          match(Ili2Parser::LBRACE);
          setState(873);
          dynamic_cast<RefSysContext *>(_localctx)->axis = match(Ili2Parser::POSNUMBER);
          setState(874);
          match(Ili2Parser::RBRACE);
        }
        setState(877);
        match(Ili2Parser::GREATER);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DecConstContext ------------------------------------------------------------------

Ili2Parser::DecConstContext::DecConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::DecimalContext* Ili2Parser::DecConstContext::decimal() {
  return getRuleContext<Ili2Parser::DecimalContext>(0);
}

tree::TerminalNode* Ili2Parser::DecConstContext::PI() {
  return getToken(Ili2Parser::PI, 0);
}

tree::TerminalNode* Ili2Parser::DecConstContext::LNBASE() {
  return getToken(Ili2Parser::LNBASE, 0);
}


size_t Ili2Parser::DecConstContext::getRuleIndex() const {
  return Ili2Parser::RuleDecConst;
}

antlrcpp::Any Ili2Parser::DecConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitDecConst(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::DecConstContext* Ili2Parser::decConst() {
  DecConstContext *_localctx = _tracker.createInstance<DecConstContext>(_ctx, getState());
  enterRule(_localctx, 78, Ili2Parser::RuleDecConst);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(884);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::POSNUMBER:
      case Ili2Parser::NUMBER:
      case Ili2Parser::DEC: {
        enterOuterAlt(_localctx, 1);
        setState(881);
        dynamic_cast<DecConstContext *>(_localctx)->dec = decimal();
        break;
      }

      case Ili2Parser::PI: {
        enterOuterAlt(_localctx, 2);
        setState(882);
        match(Ili2Parser::PI);
        break;
      }

      case Ili2Parser::LNBASE: {
        enterOuterAlt(_localctx, 3);
        setState(883);
        match(Ili2Parser::LNBASE);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumericConstContext ------------------------------------------------------------------

Ili2Parser::NumericConstContext::NumericConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::DecConstContext* Ili2Parser::NumericConstContext::decConst() {
  return getRuleContext<Ili2Parser::DecConstContext>(0);
}

tree::TerminalNode* Ili2Parser::NumericConstContext::LBRACE() {
  return getToken(Ili2Parser::LBRACE, 0);
}

tree::TerminalNode* Ili2Parser::NumericConstContext::RBRACE() {
  return getToken(Ili2Parser::RBRACE, 0);
}

Ili2Parser::PathContext* Ili2Parser::NumericConstContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::NumericConstContext::getRuleIndex() const {
  return Ili2Parser::RuleNumericConst;
}

antlrcpp::Any Ili2Parser::NumericConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitNumericConst(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::NumericConstContext* Ili2Parser::numericConst() {
  NumericConstContext *_localctx = _tracker.createInstance<NumericConstContext>(_ctx, getState());
  enterRule(_localctx, 80, Ili2Parser::RuleNumericConst);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(886);
    decConst();
    setState(891);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 106, _ctx)) {
    case 1: {
      setState(887);
      match(Ili2Parser::LBRACE);
      setState(888);
      dynamic_cast<NumericConstContext *>(_localctx)->unitref = path();
      setState(889);
      match(Ili2Parser::RBRACE);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FormattedTypeContext ------------------------------------------------------------------

Ili2Parser::FormattedTypeContext::FormattedTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::FormattedTypeContext::FORMAT() {
  return getToken(Ili2Parser::FORMAT, 0);
}

tree::TerminalNode* Ili2Parser::FormattedTypeContext::BASED() {
  return getToken(Ili2Parser::BASED, 0);
}

tree::TerminalNode* Ili2Parser::FormattedTypeContext::ON() {
  return getToken(Ili2Parser::ON, 0);
}

Ili2Parser::FormatDefContext* Ili2Parser::FormattedTypeContext::formatDef() {
  return getRuleContext<Ili2Parser::FormatDefContext>(0);
}

Ili2Parser::PathContext* Ili2Parser::FormattedTypeContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

tree::TerminalNode* Ili2Parser::FormattedTypeContext::DOTDOT() {
  return getToken(Ili2Parser::DOTDOT, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::FormattedTypeContext::STRING() {
  return getTokens(Ili2Parser::STRING);
}

tree::TerminalNode* Ili2Parser::FormattedTypeContext::STRING(size_t i) {
  return getToken(Ili2Parser::STRING, i);
}


size_t Ili2Parser::FormattedTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleFormattedType;
}

antlrcpp::Any Ili2Parser::FormattedTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitFormattedType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::FormattedTypeContext* Ili2Parser::formattedType() {
  FormattedTypeContext *_localctx = _tracker.createInstance<FormattedTypeContext>(_ctx, getState());
  enterRule(_localctx, 82, Ili2Parser::RuleFormattedType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(912);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 108, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(893);
      match(Ili2Parser::FORMAT);
      setState(894);
      match(Ili2Parser::BASED);
      setState(895);
      match(Ili2Parser::ON);
      setState(896);
      dynamic_cast<FormattedTypeContext *>(_localctx)->structref = path();
      setState(897);
      formatDef();
      setState(901);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 107, _ctx)) {
      case 1: {
        setState(898);
        dynamic_cast<FormattedTypeContext *>(_localctx)->min = match(Ili2Parser::STRING);
        setState(899);
        match(Ili2Parser::DOTDOT);
        setState(900);
        dynamic_cast<FormattedTypeContext *>(_localctx)->max = match(Ili2Parser::STRING);
        break;
      }

      }
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(903);
      match(Ili2Parser::FORMAT);
      setState(904);
      dynamic_cast<FormattedTypeContext *>(_localctx)->formatref = path();
      setState(905);
      dynamic_cast<FormattedTypeContext *>(_localctx)->min = match(Ili2Parser::STRING);
      setState(906);
      match(Ili2Parser::DOTDOT);
      setState(907);
      dynamic_cast<FormattedTypeContext *>(_localctx)->max = match(Ili2Parser::STRING);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(909);
      dynamic_cast<FormattedTypeContext *>(_localctx)->min = match(Ili2Parser::STRING);
      setState(910);
      match(Ili2Parser::DOTDOT);
      setState(911);
      dynamic_cast<FormattedTypeContext *>(_localctx)->max = match(Ili2Parser::STRING);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FormatDefContext ------------------------------------------------------------------

Ili2Parser::FormatDefContext::FormatDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::FormatDefContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

std::vector<Ili2Parser::BaseAttrRefContext *> Ili2Parser::FormatDefContext::baseAttrRef() {
  return getRuleContexts<Ili2Parser::BaseAttrRefContext>();
}

Ili2Parser::BaseAttrRefContext* Ili2Parser::FormatDefContext::baseAttrRef(size_t i) {
  return getRuleContext<Ili2Parser::BaseAttrRefContext>(i);
}

tree::TerminalNode* Ili2Parser::FormatDefContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

tree::TerminalNode* Ili2Parser::FormatDefContext::INHERITANCE() {
  return getToken(Ili2Parser::INHERITANCE, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::FormatDefContext::STRING() {
  return getTokens(Ili2Parser::STRING);
}

tree::TerminalNode* Ili2Parser::FormatDefContext::STRING(size_t i) {
  return getToken(Ili2Parser::STRING, i);
}


size_t Ili2Parser::FormatDefContext::getRuleIndex() const {
  return Ili2Parser::RuleFormatDef;
}

antlrcpp::Any Ili2Parser::FormatDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitFormatDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::FormatDefContext* Ili2Parser::formatDef() {
  FormatDefContext *_localctx = _tracker.createInstance<FormatDefContext>(_ctx, getState());
  enterRule(_localctx, 84, Ili2Parser::RuleFormatDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(914);
    match(Ili2Parser::LPAREN);
    setState(916);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::INHERITANCE) {
      setState(915);
      match(Ili2Parser::INHERITANCE);
    }
    setState(919);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::STRING) {
      setState(918);
      dynamic_cast<FormatDefContext *>(_localctx)->nonnumeric = match(Ili2Parser::STRING);
    }
    setState(926);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 111, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(921);
        baseAttrRef();
        setState(922);
        dynamic_cast<FormatDefContext *>(_localctx)->nonnumeric = match(Ili2Parser::STRING); 
      }
      setState(928);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 111, _ctx);
    }
    setState(929);
    baseAttrRef();
    setState(931);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::STRING) {
      setState(930);
      dynamic_cast<FormatDefContext *>(_localctx)->nonnumeric = match(Ili2Parser::STRING);
    }
    setState(933);
    match(Ili2Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BaseAttrRefContext ------------------------------------------------------------------

Ili2Parser::BaseAttrRefContext::BaseAttrRefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::BaseAttrRefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

tree::TerminalNode* Ili2Parser::BaseAttrRefContext::SLASH() {
  return getToken(Ili2Parser::SLASH, 0);
}

tree::TerminalNode* Ili2Parser::BaseAttrRefContext::POSNUMBER() {
  return getToken(Ili2Parser::POSNUMBER, 0);
}

Ili2Parser::PathContext* Ili2Parser::BaseAttrRefContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::BaseAttrRefContext::getRuleIndex() const {
  return Ili2Parser::RuleBaseAttrRef;
}

antlrcpp::Any Ili2Parser::BaseAttrRefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitBaseAttrRef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::BaseAttrRefContext* Ili2Parser::baseAttrRef() {
  BaseAttrRefContext *_localctx = _tracker.createInstance<BaseAttrRefContext>(_ctx, getState());
  enterRule(_localctx, 86, Ili2Parser::RuleBaseAttrRef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(943);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 114, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(935);
      match(Ili2Parser::NAME);
      setState(938);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == Ili2Parser::SLASH) {
        setState(936);
        match(Ili2Parser::SLASH);
        setState(937);
        match(Ili2Parser::POSNUMBER);
      }
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(940);
      match(Ili2Parser::NAME);
      setState(941);
      match(Ili2Parser::SLASH);
      setState(942);
      dynamic_cast<BaseAttrRefContext *>(_localctx)->baseattr = path();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FormattedConstContext ------------------------------------------------------------------

Ili2Parser::FormattedConstContext::FormattedConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::FormattedConstContext::STRING() {
  return getToken(Ili2Parser::STRING, 0);
}


size_t Ili2Parser::FormattedConstContext::getRuleIndex() const {
  return Ili2Parser::RuleFormattedConst;
}

antlrcpp::Any Ili2Parser::FormattedConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitFormattedConst(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::FormattedConstContext* Ili2Parser::formattedConst() {
  FormattedConstContext *_localctx = _tracker.createInstance<FormattedConstContext>(_ctx, getState());
  enterRule(_localctx, 88, Ili2Parser::RuleFormattedConst);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(945);
    dynamic_cast<FormattedConstContext *>(_localctx)->formattedconst = match(Ili2Parser::STRING);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ContextDefContext ------------------------------------------------------------------

Ili2Parser::ContextDefContext::ContextDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ContextDefContext::CONTEXT() {
  return getToken(Ili2Parser::CONTEXT, 0);
}

tree::TerminalNode* Ili2Parser::ContextDefContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

tree::TerminalNode* Ili2Parser::ContextDefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

std::vector<Ili2Parser::ContextDeclContext *> Ili2Parser::ContextDefContext::contextDecl() {
  return getRuleContexts<Ili2Parser::ContextDeclContext>();
}

Ili2Parser::ContextDeclContext* Ili2Parser::ContextDefContext::contextDecl(size_t i) {
  return getRuleContext<Ili2Parser::ContextDeclContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::ContextDefContext::SEMI() {
  return getTokens(Ili2Parser::SEMI);
}

tree::TerminalNode* Ili2Parser::ContextDefContext::SEMI(size_t i) {
  return getToken(Ili2Parser::SEMI, i);
}


size_t Ili2Parser::ContextDefContext::getRuleIndex() const {
  return Ili2Parser::RuleContextDef;
}

antlrcpp::Any Ili2Parser::ContextDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitContextDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ContextDefContext* Ili2Parser::contextDef() {
  ContextDefContext *_localctx = _tracker.createInstance<ContextDefContext>(_ctx, getState());
  enterRule(_localctx, 90, Ili2Parser::RuleContextDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(947);
    match(Ili2Parser::CONTEXT);
    setState(948);
    dynamic_cast<ContextDefContext *>(_localctx)->name = match(Ili2Parser::NAME);
    setState(949);
    match(Ili2Parser::EQUAL);
    setState(955);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 115, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(950);
        contextDecl();
        setState(951);
        match(Ili2Parser::SEMI); 
      }
      setState(957);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 115, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ContextDeclContext ------------------------------------------------------------------

Ili2Parser::ContextDeclContext::ContextDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ContextDeclContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

std::vector<Ili2Parser::PathContext *> Ili2Parser::ContextDeclContext::path() {
  return getRuleContexts<Ili2Parser::PathContext>();
}

Ili2Parser::PathContext* Ili2Parser::ContextDeclContext::path(size_t i) {
  return getRuleContext<Ili2Parser::PathContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::ContextDeclContext::OR() {
  return getTokens(Ili2Parser::OR);
}

tree::TerminalNode* Ili2Parser::ContextDeclContext::OR(size_t i) {
  return getToken(Ili2Parser::OR, i);
}


size_t Ili2Parser::ContextDeclContext::getRuleIndex() const {
  return Ili2Parser::RuleContextDecl;
}

antlrcpp::Any Ili2Parser::ContextDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitContextDecl(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ContextDeclContext* Ili2Parser::contextDecl() {
  ContextDeclContext *_localctx = _tracker.createInstance<ContextDeclContext>(_ctx, getState());
  enterRule(_localctx, 92, Ili2Parser::RuleContextDecl);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(958);
    dynamic_cast<ContextDeclContext *>(_localctx)->generic = path();
    setState(959);
    match(Ili2Parser::EQUAL);
    setState(960);
    path();
    setState(965);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::OR) {
      setState(961);
      match(Ili2Parser::OR);
      setState(962);
      path();
      setState(967);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CoordinateTypeContext ------------------------------------------------------------------

Ili2Parser::CoordinateTypeContext::CoordinateTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<Ili2Parser::NumericTypeContext *> Ili2Parser::CoordinateTypeContext::numericType() {
  return getRuleContexts<Ili2Parser::NumericTypeContext>();
}

Ili2Parser::NumericTypeContext* Ili2Parser::CoordinateTypeContext::numericType(size_t i) {
  return getRuleContext<Ili2Parser::NumericTypeContext>(i);
}

tree::TerminalNode* Ili2Parser::CoordinateTypeContext::COORD() {
  return getToken(Ili2Parser::COORD, 0);
}

tree::TerminalNode* Ili2Parser::CoordinateTypeContext::MULTICOORD() {
  return getToken(Ili2Parser::MULTICOORD, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::CoordinateTypeContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::CoordinateTypeContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}

Ili2Parser::RotationDefContext* Ili2Parser::CoordinateTypeContext::rotationDef() {
  return getRuleContext<Ili2Parser::RotationDefContext>(0);
}


size_t Ili2Parser::CoordinateTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleCoordinateType;
}

antlrcpp::Any Ili2Parser::CoordinateTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitCoordinateType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::CoordinateTypeContext* Ili2Parser::coordinateType() {
  CoordinateTypeContext *_localctx = _tracker.createInstance<CoordinateTypeContext>(_ctx, getState());
  enterRule(_localctx, 94, Ili2Parser::RuleCoordinateType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(971);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 117, _ctx)) {
    case 1: {
      setState(968);
      match(Ili2Parser::COORD);
      break;
    }

    case 2: {
      setState(969);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(970);
      match(Ili2Parser::MULTICOORD);
      break;
    }

    }
    setState(973);
    dynamic_cast<CoordinateTypeContext *>(_localctx)->numtype1 = numericType();
    setState(980);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 119, _ctx)) {
    case 1: {
      setState(974);
      match(Ili2Parser::COMMA);
      setState(975);
      dynamic_cast<CoordinateTypeContext *>(_localctx)->numtype2 = numericType();
      setState(978);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 118, _ctx)) {
      case 1: {
        setState(976);
        match(Ili2Parser::COMMA);
        setState(977);
        dynamic_cast<CoordinateTypeContext *>(_localctx)->numtype3 = numericType();
        break;
      }

      }
      break;
    }

    }
    setState(984);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 120, _ctx)) {
    case 1: {
      setState(982);
      match(Ili2Parser::COMMA);
      setState(983);
      rotationDef();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RotationDefContext ------------------------------------------------------------------

Ili2Parser::RotationDefContext::RotationDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::RotationDefContext::ROTATION() {
  return getToken(Ili2Parser::ROTATION, 0);
}

tree::TerminalNode* Ili2Parser::RotationDefContext::RARROW() {
  return getToken(Ili2Parser::RARROW, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::RotationDefContext::POSNUMBER() {
  return getTokens(Ili2Parser::POSNUMBER);
}

tree::TerminalNode* Ili2Parser::RotationDefContext::POSNUMBER(size_t i) {
  return getToken(Ili2Parser::POSNUMBER, i);
}


size_t Ili2Parser::RotationDefContext::getRuleIndex() const {
  return Ili2Parser::RuleRotationDef;
}

antlrcpp::Any Ili2Parser::RotationDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitRotationDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::RotationDefContext* Ili2Parser::rotationDef() {
  RotationDefContext *_localctx = _tracker.createInstance<RotationDefContext>(_ctx, getState());
  enterRule(_localctx, 96, Ili2Parser::RuleRotationDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(986);
    match(Ili2Parser::ROTATION);
    setState(987);
    dynamic_cast<RotationDefContext *>(_localctx)->nullaxis = match(Ili2Parser::POSNUMBER);
    setState(988);
    match(Ili2Parser::RARROW);
    setState(989);
    dynamic_cast<RotationDefContext *>(_localctx)->pihalfaxis = match(Ili2Parser::POSNUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- OIDTypeContext ------------------------------------------------------------------

Ili2Parser::OIDTypeContext::OIDTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::OIDTypeContext::OID() {
  return getToken(Ili2Parser::OID, 0);
}

tree::TerminalNode* Ili2Parser::OIDTypeContext::ANY() {
  return getToken(Ili2Parser::ANY, 0);
}

Ili2Parser::NumericTypeContext* Ili2Parser::OIDTypeContext::numericType() {
  return getRuleContext<Ili2Parser::NumericTypeContext>(0);
}

Ili2Parser::TextTypeContext* Ili2Parser::OIDTypeContext::textType() {
  return getRuleContext<Ili2Parser::TextTypeContext>(0);
}


size_t Ili2Parser::OIDTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleOIDType;
}

antlrcpp::Any Ili2Parser::OIDTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitOIDType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::OIDTypeContext* Ili2Parser::oIDType() {
  OIDTypeContext *_localctx = _tracker.createInstance<OIDTypeContext>(_ctx, getState());
  enterRule(_localctx, 98, Ili2Parser::RuleOIDType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(991);
    match(Ili2Parser::OID);
    setState(995);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::ANY: {
        setState(992);
        match(Ili2Parser::ANY);
        break;
      }

      case Ili2Parser::NUMERIC:
      case Ili2Parser::POSNUMBER:
      case Ili2Parser::NUMBER:
      case Ili2Parser::DEC: {
        setState(993);
        numericType();
        break;
      }

      case Ili2Parser::TEXT:
      case Ili2Parser::MTEXT:
      case Ili2Parser::NAME_CONST:
      case Ili2Parser::URI: {
        setState(994);
        textType();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlackboxTypeContext ------------------------------------------------------------------

Ili2Parser::BlackboxTypeContext::BlackboxTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::BlackboxTypeContext::BLACKBOX() {
  return getToken(Ili2Parser::BLACKBOX, 0);
}

tree::TerminalNode* Ili2Parser::BlackboxTypeContext::XML() {
  return getToken(Ili2Parser::XML, 0);
}

tree::TerminalNode* Ili2Parser::BlackboxTypeContext::BINARY() {
  return getToken(Ili2Parser::BINARY, 0);
}


size_t Ili2Parser::BlackboxTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleBlackboxType;
}

antlrcpp::Any Ili2Parser::BlackboxTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitBlackboxType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::BlackboxTypeContext* Ili2Parser::blackboxType() {
  BlackboxTypeContext *_localctx = _tracker.createInstance<BlackboxTypeContext>(_ctx, getState());
  enterRule(_localctx, 100, Ili2Parser::RuleBlackboxType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(997);
    match(Ili2Parser::BLACKBOX);
    setState(998);
    _la = _input->LA(1);
    if (!(_la == Ili2Parser::XML

    || _la == Ili2Parser::BINARY)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassRefTypeContext ------------------------------------------------------------------

Ili2Parser::ClassRefTypeContext::ClassRefTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ClassRefTypeContext::CLASS() {
  return getToken(Ili2Parser::CLASS, 0);
}

Ili2Parser::RestrictionContext* Ili2Parser::ClassRefTypeContext::restriction() {
  return getRuleContext<Ili2Parser::RestrictionContext>(0);
}

tree::TerminalNode* Ili2Parser::ClassRefTypeContext::STRUCTURE() {
  return getToken(Ili2Parser::STRUCTURE, 0);
}


size_t Ili2Parser::ClassRefTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleClassRefType;
}

antlrcpp::Any Ili2Parser::ClassRefTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitClassRefType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ClassRefTypeContext* Ili2Parser::classRefType() {
  ClassRefTypeContext *_localctx = _tracker.createInstance<ClassRefTypeContext>(_ctx, getState());
  enterRule(_localctx, 102, Ili2Parser::RuleClassRefType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1008);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::CLASS: {
        enterOuterAlt(_localctx, 1);
        setState(1000);
        match(Ili2Parser::CLASS);
        setState(1002);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 122, _ctx)) {
        case 1: {
          setState(1001);
          restriction();
          break;
        }

        }
        break;
      }

      case Ili2Parser::STRUCTURE: {
        enterOuterAlt(_localctx, 2);
        setState(1004);
        match(Ili2Parser::STRUCTURE);
        setState(1006);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 123, _ctx)) {
        case 1: {
          setState(1005);
          restriction();
          break;
        }

        }
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttributePathTypeContext ------------------------------------------------------------------

Ili2Parser::AttributePathTypeContext::AttributePathTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::AttributePathTypeContext::ATTRIBUTE() {
  return getToken(Ili2Parser::ATTRIBUTE, 0);
}

tree::TerminalNode* Ili2Parser::AttributePathTypeContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

tree::TerminalNode* Ili2Parser::AttributePathTypeContext::RESTRICTION() {
  return getToken(Ili2Parser::RESTRICTION, 0);
}

tree::TerminalNode* Ili2Parser::AttributePathTypeContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

std::vector<Ili2Parser::AttrTypeDefContext *> Ili2Parser::AttributePathTypeContext::attrTypeDef() {
  return getRuleContexts<Ili2Parser::AttrTypeDefContext>();
}

Ili2Parser::AttrTypeDefContext* Ili2Parser::AttributePathTypeContext::attrTypeDef(size_t i) {
  return getRuleContext<Ili2Parser::AttrTypeDefContext>(i);
}

tree::TerminalNode* Ili2Parser::AttributePathTypeContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

Ili2Parser::AttributePathContext* Ili2Parser::AttributePathTypeContext::attributePath() {
  return getRuleContext<Ili2Parser::AttributePathContext>(0);
}

tree::TerminalNode* Ili2Parser::AttributePathTypeContext::AT() {
  return getToken(Ili2Parser::AT, 0);
}

tree::TerminalNode* Ili2Parser::AttributePathTypeContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::AttributePathTypeContext::COLON() {
  return getTokens(Ili2Parser::COLON);
}

tree::TerminalNode* Ili2Parser::AttributePathTypeContext::COLON(size_t i) {
  return getToken(Ili2Parser::COLON, i);
}


size_t Ili2Parser::AttributePathTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleAttributePathType;
}

antlrcpp::Any Ili2Parser::AttributePathTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAttributePathType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AttributePathTypeContext* Ili2Parser::attributePathType() {
  AttributePathTypeContext *_localctx = _tracker.createInstance<AttributePathTypeContext>(_ctx, getState());
  enterRule(_localctx, 104, Ili2Parser::RuleAttributePathType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1010);
    match(Ili2Parser::ATTRIBUTE);
    setState(1017);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 126, _ctx)) {
    case 1: {
      setState(1011);
      match(Ili2Parser::OF);
      setState(1015);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case Ili2Parser::BACKSLASH:
        case Ili2Parser::THIS:
        case Ili2Parser::THISAREA:
        case Ili2Parser::THATAREA:
        case Ili2Parser::PARENT:
        case Ili2Parser::AGGREGATES:
        case Ili2Parser::NAME: {
          setState(1012);
          attributePath();
          break;
        }

        case Ili2Parser::AT: {
          setState(1013);
          match(Ili2Parser::AT);
          setState(1014);
          match(Ili2Parser::NAME);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      break;
    }

    }
    setState(1031);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 128, _ctx)) {
    case 1: {
      setState(1019);
      match(Ili2Parser::RESTRICTION);
      setState(1020);
      match(Ili2Parser::LPAREN);
      setState(1021);
      attrTypeDef();
      setState(1026);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili2Parser::COLON) {
        setState(1022);
        match(Ili2Parser::COLON);
        setState(1023);
        attrTypeDef();
        setState(1028);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(1029);
      match(Ili2Parser::RPAREN);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ClassConstContext ------------------------------------------------------------------

Ili2Parser::ClassConstContext::ClassConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ClassConstContext::GREATER() {
  return getToken(Ili2Parser::GREATER, 0);
}

Ili2Parser::PathContext* Ili2Parser::ClassConstContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::ClassConstContext::getRuleIndex() const {
  return Ili2Parser::RuleClassConst;
}

antlrcpp::Any Ili2Parser::ClassConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitClassConst(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ClassConstContext* Ili2Parser::classConst() {
  ClassConstContext *_localctx = _tracker.createInstance<ClassConstContext>(_ctx, getState());
  enterRule(_localctx, 106, Ili2Parser::RuleClassConst);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1033);
    match(Ili2Parser::GREATER);
    setState(1034);
    dynamic_cast<ClassConstContext *>(_localctx)->classref = path();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttributePathConstContext ------------------------------------------------------------------

Ili2Parser::AttributePathConstContext::AttributePathConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::AttributePathConstContext::GREATERGREATER() {
  return getToken(Ili2Parser::GREATERGREATER, 0);
}

tree::TerminalNode* Ili2Parser::AttributePathConstContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

tree::TerminalNode* Ili2Parser::AttributePathConstContext::RARROW() {
  return getToken(Ili2Parser::RARROW, 0);
}

Ili2Parser::PathContext* Ili2Parser::AttributePathConstContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::AttributePathConstContext::getRuleIndex() const {
  return Ili2Parser::RuleAttributePathConst;
}

antlrcpp::Any Ili2Parser::AttributePathConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAttributePathConst(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AttributePathConstContext* Ili2Parser::attributePathConst() {
  AttributePathConstContext *_localctx = _tracker.createInstance<AttributePathConstContext>(_ctx, getState());
  enterRule(_localctx, 108, Ili2Parser::RuleAttributePathConst);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1036);
    match(Ili2Parser::GREATERGREATER);
    setState(1040);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 129, _ctx)) {
    case 1: {
      setState(1037);
      dynamic_cast<AttributePathConstContext *>(_localctx)->classref = path();
      setState(1038);
      match(Ili2Parser::RARROW);
      break;
    }

    }
    setState(1042);
    dynamic_cast<AttributePathConstContext *>(_localctx)->attribute = match(Ili2Parser::NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineTypeContext ------------------------------------------------------------------

Ili2Parser::LineTypeContext::LineTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::LineTypeContext::POLYLINE() {
  return getToken(Ili2Parser::POLYLINE, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::SURFACE() {
  return getToken(Ili2Parser::SURFACE, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::AREA() {
  return getToken(Ili2Parser::AREA, 0);
}

Ili2Parser::LineFormContext* Ili2Parser::LineTypeContext::lineForm() {
  return getRuleContext<Ili2Parser::LineFormContext>(0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::VERTEX() {
  return getToken(Ili2Parser::VERTEX, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::WITHOUT() {
  return getToken(Ili2Parser::WITHOUT, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::OVERLAPS() {
  return getToken(Ili2Parser::OVERLAPS, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::GREATER() {
  return getToken(Ili2Parser::GREATER, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::LINE() {
  return getToken(Ili2Parser::LINE, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::ATTRIBUTES() {
  return getToken(Ili2Parser::ATTRIBUTES, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::MULTISURFACE() {
  return getToken(Ili2Parser::MULTISURFACE, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::MULTIAREA() {
  return getToken(Ili2Parser::MULTIAREA, 0);
}

std::vector<Ili2Parser::PathContext *> Ili2Parser::LineTypeContext::path() {
  return getRuleContexts<Ili2Parser::PathContext>();
}

Ili2Parser::PathContext* Ili2Parser::LineTypeContext::path(size_t i) {
  return getRuleContext<Ili2Parser::PathContext>(i);
}

Ili2Parser::DecimalContext* Ili2Parser::LineTypeContext::decimal() {
  return getRuleContext<Ili2Parser::DecimalContext>(0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::DIRECTED() {
  return getToken(Ili2Parser::DIRECTED, 0);
}

tree::TerminalNode* Ili2Parser::LineTypeContext::MULTIPOLYLINE() {
  return getToken(Ili2Parser::MULTIPOLYLINE, 0);
}


size_t Ili2Parser::LineTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleLineType;
}

antlrcpp::Any Ili2Parser::LineTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitLineType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::LineTypeContext* Ili2Parser::lineType() {
  LineTypeContext *_localctx = _tracker.createInstance<LineTypeContext>(_ctx, getState());
  enterRule(_localctx, 110, Ili2Parser::RuleLineType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1059);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 133, _ctx)) {
    case 1: {
      setState(1045);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == Ili2Parser::DIRECTED) {
        setState(1044);
        dynamic_cast<LineTypeContext *>(_localctx)->directed = match(Ili2Parser::DIRECTED);
      }
      setState(1047);
      match(Ili2Parser::POLYLINE);
      break;
    }

    case 2: {
      setState(1048);
      match(Ili2Parser::SURFACE);
      break;
    }

    case 3: {
      setState(1049);
      match(Ili2Parser::AREA);
      break;
    }

    case 4: {
      setState(1057);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 132, _ctx)) {
      case 1: {
        setState(1050);

        if (!(ili24)) throw FailedPredicateException(this, "ili24");

        setState(1052);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == Ili2Parser::DIRECTED) {
          setState(1051);
          dynamic_cast<LineTypeContext *>(_localctx)->multdir = match(Ili2Parser::DIRECTED);
        }
        setState(1054);
        match(Ili2Parser::MULTIPOLYLINE);
        break;
      }

      case 2: {
        setState(1055);
        match(Ili2Parser::MULTISURFACE);
        break;
      }

      case 3: {
        setState(1056);
        match(Ili2Parser::MULTIAREA);
        break;
      }

      }
      break;
    }

    }
    setState(1062);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 134, _ctx)) {
    case 1: {
      setState(1061);
      lineForm();
      break;
    }

    }
    setState(1066);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 135, _ctx)) {
    case 1: {
      setState(1064);
      match(Ili2Parser::VERTEX);
      setState(1065);
      dynamic_cast<LineTypeContext *>(_localctx)->coordref = path();
      break;
    }

    }
    setState(1075);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 136, _ctx)) {
    case 1: {
      setState(1068);
      match(Ili2Parser::WITHOUT);
      setState(1069);
      match(Ili2Parser::OVERLAPS);
      setState(1070);
      match(Ili2Parser::GREATER);
      setState(1071);
      dynamic_cast<LineTypeContext *>(_localctx)->overlap = decimal();
      break;
    }

    case 2: {
      setState(1072);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1073);
      match(Ili2Parser::WITHOUT);
      setState(1074);
      match(Ili2Parser::OVERLAPS);
      break;
    }

    }
    setState(1081);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 137, _ctx)) {
    case 1: {
      setState(1077);

      if (!(ili23)) throw FailedPredicateException(this, "ili23");
      setState(1078);
      match(Ili2Parser::LINE);
      setState(1079);
      match(Ili2Parser::ATTRIBUTES);
      setState(1080);
      dynamic_cast<LineTypeContext *>(_localctx)->lineattrstruct = path();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineFormContext ------------------------------------------------------------------

Ili2Parser::LineFormContext::LineFormContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::LineFormContext::WITH() {
  return getToken(Ili2Parser::WITH, 0);
}

tree::TerminalNode* Ili2Parser::LineFormContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

std::vector<Ili2Parser::LineFormTypeContext *> Ili2Parser::LineFormContext::lineFormType() {
  return getRuleContexts<Ili2Parser::LineFormTypeContext>();
}

Ili2Parser::LineFormTypeContext* Ili2Parser::LineFormContext::lineFormType(size_t i) {
  return getRuleContext<Ili2Parser::LineFormTypeContext>(i);
}

tree::TerminalNode* Ili2Parser::LineFormContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::LineFormContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::LineFormContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::LineFormContext::getRuleIndex() const {
  return Ili2Parser::RuleLineForm;
}

antlrcpp::Any Ili2Parser::LineFormContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitLineForm(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::LineFormContext* Ili2Parser::lineForm() {
  LineFormContext *_localctx = _tracker.createInstance<LineFormContext>(_ctx, getState());
  enterRule(_localctx, 112, Ili2Parser::RuleLineForm);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1083);
    match(Ili2Parser::WITH);
    setState(1084);
    match(Ili2Parser::LPAREN);
    setState(1085);
    lineFormType();
    setState(1090);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::COMMA) {
      setState(1086);
      match(Ili2Parser::COMMA);
      setState(1087);
      lineFormType();
      setState(1092);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1093);
    match(Ili2Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineFormTypeContext ------------------------------------------------------------------

Ili2Parser::LineFormTypeContext::LineFormTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::LineFormTypeContext::STRAIGHTS() {
  return getToken(Ili2Parser::STRAIGHTS, 0);
}

tree::TerminalNode* Ili2Parser::LineFormTypeContext::ARCS() {
  return getToken(Ili2Parser::ARCS, 0);
}

Ili2Parser::PathContext* Ili2Parser::LineFormTypeContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::LineFormTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleLineFormType;
}

antlrcpp::Any Ili2Parser::LineFormTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitLineFormType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::LineFormTypeContext* Ili2Parser::lineFormType() {
  LineFormTypeContext *_localctx = _tracker.createInstance<LineFormTypeContext>(_ctx, getState());
  enterRule(_localctx, 114, Ili2Parser::RuleLineFormType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1098);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::STRAIGHTS: {
        enterOuterAlt(_localctx, 1);
        setState(1095);
        match(Ili2Parser::STRAIGHTS);
        break;
      }

      case Ili2Parser::ARCS: {
        enterOuterAlt(_localctx, 2);
        setState(1096);
        match(Ili2Parser::ARCS);
        break;
      }

      case Ili2Parser::INTERLIS:
      case Ili2Parser::REFSYSTEM:
      case Ili2Parser::BOOLEAN:
      case Ili2Parser::HALIGNMENT:
      case Ili2Parser::VALIGNMENT:
      case Ili2Parser::URI:
      case Ili2Parser::SIGN:
      case Ili2Parser::METAOBJECT:
      case Ili2Parser::NAME: {
        enterOuterAlt(_localctx, 3);
        setState(1097);
        path();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineFormTypeDefContext ------------------------------------------------------------------

Ili2Parser::LineFormTypeDefContext::LineFormTypeDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::LineFormTypeDefContext::LINE() {
  return getToken(Ili2Parser::LINE, 0);
}

tree::TerminalNode* Ili2Parser::LineFormTypeDefContext::FORM() {
  return getToken(Ili2Parser::FORM, 0);
}

std::vector<Ili2Parser::LineFormTypeDeclContext *> Ili2Parser::LineFormTypeDefContext::lineFormTypeDecl() {
  return getRuleContexts<Ili2Parser::LineFormTypeDeclContext>();
}

Ili2Parser::LineFormTypeDeclContext* Ili2Parser::LineFormTypeDefContext::lineFormTypeDecl(size_t i) {
  return getRuleContext<Ili2Parser::LineFormTypeDeclContext>(i);
}


size_t Ili2Parser::LineFormTypeDefContext::getRuleIndex() const {
  return Ili2Parser::RuleLineFormTypeDef;
}

antlrcpp::Any Ili2Parser::LineFormTypeDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitLineFormTypeDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::LineFormTypeDefContext* Ili2Parser::lineFormTypeDef() {
  LineFormTypeDefContext *_localctx = _tracker.createInstance<LineFormTypeDefContext>(_ctx, getState());
  enterRule(_localctx, 116, Ili2Parser::RuleLineFormTypeDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(1100);
    match(Ili2Parser::LINE);
    setState(1101);
    match(Ili2Parser::FORM);
    setState(1105);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 140, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(1102);
        lineFormTypeDecl(); 
      }
      setState(1107);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 140, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineFormTypeDeclContext ------------------------------------------------------------------

Ili2Parser::LineFormTypeDeclContext::LineFormTypeDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::LineFormTypeDeclContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

tree::TerminalNode* Ili2Parser::LineFormTypeDeclContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::LineFormTypeDeclContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

Ili2Parser::PathContext* Ili2Parser::LineFormTypeDeclContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::LineFormTypeDeclContext::getRuleIndex() const {
  return Ili2Parser::RuleLineFormTypeDecl;
}

antlrcpp::Any Ili2Parser::LineFormTypeDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitLineFormTypeDecl(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::LineFormTypeDeclContext* Ili2Parser::lineFormTypeDecl() {
  LineFormTypeDeclContext *_localctx = _tracker.createInstance<LineFormTypeDeclContext>(_ctx, getState());
  enterRule(_localctx, 118, Ili2Parser::RuleLineFormTypeDecl);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1108);
    dynamic_cast<LineFormTypeDeclContext *>(_localctx)->lineformname = match(Ili2Parser::NAME);
    setState(1109);
    match(Ili2Parser::COLON);
    setState(1110);
    dynamic_cast<LineFormTypeDeclContext *>(_localctx)->structureref = path();
    setState(1111);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnitDeclContext ------------------------------------------------------------------

Ili2Parser::UnitDeclContext::UnitDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::UnitDeclContext::UNIT() {
  return getToken(Ili2Parser::UNIT, 0);
}

std::vector<Ili2Parser::UnitDefContext *> Ili2Parser::UnitDeclContext::unitDef() {
  return getRuleContexts<Ili2Parser::UnitDefContext>();
}

Ili2Parser::UnitDefContext* Ili2Parser::UnitDeclContext::unitDef(size_t i) {
  return getRuleContext<Ili2Parser::UnitDefContext>(i);
}


size_t Ili2Parser::UnitDeclContext::getRuleIndex() const {
  return Ili2Parser::RuleUnitDecl;
}

antlrcpp::Any Ili2Parser::UnitDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitUnitDecl(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::UnitDeclContext* Ili2Parser::unitDecl() {
  UnitDeclContext *_localctx = _tracker.createInstance<UnitDeclContext>(_ctx, getState());
  enterRule(_localctx, 120, Ili2Parser::RuleUnitDecl);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(1113);
    match(Ili2Parser::UNIT);
    setState(1117);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 141, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(1114);
        unitDef(); 
      }
      setState(1119);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 141, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnitDefContext ------------------------------------------------------------------

Ili2Parser::UnitDefContext::UnitDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::UnitDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::UnitDefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

tree::TerminalNode* Ili2Parser::UnitDefContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::UnitDefContext::ABSTRACT() {
  return getToken(Ili2Parser::ABSTRACT, 0);
}

tree::TerminalNode* Ili2Parser::UnitDefContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

tree::TerminalNode* Ili2Parser::UnitDefContext::LBRACE() {
  return getToken(Ili2Parser::LBRACE, 0);
}

tree::TerminalNode* Ili2Parser::UnitDefContext::RBRACE() {
  return getToken(Ili2Parser::RBRACE, 0);
}

tree::TerminalNode* Ili2Parser::UnitDefContext::EXTENDS() {
  return getToken(Ili2Parser::EXTENDS, 0);
}

tree::TerminalNode* Ili2Parser::UnitDefContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

std::vector<Ili2Parser::PathContext *> Ili2Parser::UnitDefContext::path() {
  return getRuleContexts<Ili2Parser::PathContext>();
}

Ili2Parser::PathContext* Ili2Parser::UnitDefContext::path(size_t i) {
  return getRuleContext<Ili2Parser::PathContext>(i);
}

Ili2Parser::DerivedUnitContext* Ili2Parser::UnitDefContext::derivedUnit() {
  return getRuleContext<Ili2Parser::DerivedUnitContext>(0);
}

Ili2Parser::ComposedUnitContext* Ili2Parser::UnitDefContext::composedUnit() {
  return getRuleContext<Ili2Parser::ComposedUnitContext>(0);
}


size_t Ili2Parser::UnitDefContext::getRuleIndex() const {
  return Ili2Parser::RuleUnitDef;
}

antlrcpp::Any Ili2Parser::UnitDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitUnitDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::UnitDefContext* Ili2Parser::unitDef() {
  UnitDefContext *_localctx = _tracker.createInstance<UnitDefContext>(_ctx, getState());
  enterRule(_localctx, 122, Ili2Parser::RuleUnitDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1120);
    dynamic_cast<UnitDefContext *>(_localctx)->unitname = match(Ili2Parser::NAME);
    setState(1128);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::LPAREN: {
        setState(1121);
        match(Ili2Parser::LPAREN);
        setState(1122);
        match(Ili2Parser::ABSTRACT);
        setState(1123);
        match(Ili2Parser::RPAREN);
        break;
      }

      case Ili2Parser::LBRACE: {
        setState(1124);
        match(Ili2Parser::LBRACE);
        setState(1125);
        dynamic_cast<UnitDefContext *>(_localctx)->unitshort = path();
        setState(1126);
        match(Ili2Parser::RBRACE);
        break;
      }

      case Ili2Parser::EXTENDS:
      case Ili2Parser::SEMI:
      case Ili2Parser::EQUAL: {
        break;
      }

    default:
      break;
    }
    setState(1132);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXTENDS) {
      setState(1130);
      match(Ili2Parser::EXTENDS);
      setState(1131);
      dynamic_cast<UnitDefContext *>(_localctx)->super = path();
    }
    setState(1139);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EQUAL) {
      setState(1134);
      match(Ili2Parser::EQUAL);
      setState(1137);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case Ili2Parser::LBRACE:
        case Ili2Parser::PI:
        case Ili2Parser::LNBASE:
        case Ili2Parser::FUNCTION:
        case Ili2Parser::POSNUMBER:
        case Ili2Parser::NUMBER:
        case Ili2Parser::DEC: {
          setState(1135);
          derivedUnit();
          break;
        }

        case Ili2Parser::LPAREN: {
          setState(1136);
          composedUnit();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
    }
    setState(1141);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DerivedUnitContext ------------------------------------------------------------------

Ili2Parser::DerivedUnitContext::DerivedUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::DerivedUnitContext::LBRACE() {
  return getToken(Ili2Parser::LBRACE, 0);
}

tree::TerminalNode* Ili2Parser::DerivedUnitContext::RBRACE() {
  return getToken(Ili2Parser::RBRACE, 0);
}

Ili2Parser::PathContext* Ili2Parser::DerivedUnitContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

std::vector<Ili2Parser::DecConstContext *> Ili2Parser::DerivedUnitContext::decConst() {
  return getRuleContexts<Ili2Parser::DecConstContext>();
}

Ili2Parser::DecConstContext* Ili2Parser::DerivedUnitContext::decConst(size_t i) {
  return getRuleContext<Ili2Parser::DecConstContext>(i);
}

tree::TerminalNode* Ili2Parser::DerivedUnitContext::FUNCTION() {
  return getToken(Ili2Parser::FUNCTION, 0);
}

tree::TerminalNode* Ili2Parser::DerivedUnitContext::EXPLANATION() {
  return getToken(Ili2Parser::EXPLANATION, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::DerivedUnitContext::STAR() {
  return getTokens(Ili2Parser::STAR);
}

tree::TerminalNode* Ili2Parser::DerivedUnitContext::STAR(size_t i) {
  return getToken(Ili2Parser::STAR, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::DerivedUnitContext::SLASH() {
  return getTokens(Ili2Parser::SLASH);
}

tree::TerminalNode* Ili2Parser::DerivedUnitContext::SLASH(size_t i) {
  return getToken(Ili2Parser::SLASH, i);
}


size_t Ili2Parser::DerivedUnitContext::getRuleIndex() const {
  return Ili2Parser::RuleDerivedUnit;
}

antlrcpp::Any Ili2Parser::DerivedUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitDerivedUnit(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::DerivedUnitContext* Ili2Parser::derivedUnit() {
  DerivedUnitContext *_localctx = _tracker.createInstance<DerivedUnitContext>(_ctx, getState());
  enterRule(_localctx, 124, Ili2Parser::RuleDerivedUnit);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1153);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::PI:
      case Ili2Parser::LNBASE:
      case Ili2Parser::POSNUMBER:
      case Ili2Parser::NUMBER:
      case Ili2Parser::DEC: {
        setState(1143);
        decConst();
        setState(1148);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == Ili2Parser::STAR

        || _la == Ili2Parser::SLASH) {
          setState(1144);
          dynamic_cast<DerivedUnitContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == Ili2Parser::STAR

          || _la == Ili2Parser::SLASH)) {
            dynamic_cast<DerivedUnitContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(1145);
          decConst();
          setState(1150);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        break;
      }

      case Ili2Parser::FUNCTION: {
        setState(1151);
        match(Ili2Parser::FUNCTION);
        setState(1152);
        match(Ili2Parser::EXPLANATION);
        break;
      }

      case Ili2Parser::LBRACE: {
        break;
      }

    default:
      break;
    }
    setState(1155);
    match(Ili2Parser::LBRACE);
    setState(1156);
    dynamic_cast<DerivedUnitContext *>(_localctx)->unitref = path();
    setState(1157);
    match(Ili2Parser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ComposedUnitContext ------------------------------------------------------------------

Ili2Parser::ComposedUnitContext::ComposedUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ComposedUnitContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

Ili2Parser::ComposedUnitExprContext* Ili2Parser::ComposedUnitContext::composedUnitExpr() {
  return getRuleContext<Ili2Parser::ComposedUnitExprContext>(0);
}

tree::TerminalNode* Ili2Parser::ComposedUnitContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}


size_t Ili2Parser::ComposedUnitContext::getRuleIndex() const {
  return Ili2Parser::RuleComposedUnit;
}

antlrcpp::Any Ili2Parser::ComposedUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitComposedUnit(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ComposedUnitContext* Ili2Parser::composedUnit() {
  ComposedUnitContext *_localctx = _tracker.createInstance<ComposedUnitContext>(_ctx, getState());
  enterRule(_localctx, 126, Ili2Parser::RuleComposedUnit);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1159);
    match(Ili2Parser::LPAREN);
    setState(1160);
    composedUnitExpr(0);
    setState(1161);
    match(Ili2Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ComposedUnitExprContext ------------------------------------------------------------------

Ili2Parser::ComposedUnitExprContext::ComposedUnitExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::PathContext* Ili2Parser::ComposedUnitExprContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

Ili2Parser::ComposedUnitExprContext* Ili2Parser::ComposedUnitExprContext::composedUnitExpr() {
  return getRuleContext<Ili2Parser::ComposedUnitExprContext>(0);
}

tree::TerminalNode* Ili2Parser::ComposedUnitExprContext::STAR() {
  return getToken(Ili2Parser::STAR, 0);
}

tree::TerminalNode* Ili2Parser::ComposedUnitExprContext::SLASH() {
  return getToken(Ili2Parser::SLASH, 0);
}


size_t Ili2Parser::ComposedUnitExprContext::getRuleIndex() const {
  return Ili2Parser::RuleComposedUnitExpr;
}

antlrcpp::Any Ili2Parser::ComposedUnitExprContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitComposedUnitExpr(this);
  else
    return visitor->visitChildren(this);
}


Ili2Parser::ComposedUnitExprContext* Ili2Parser::composedUnitExpr() {
   return composedUnitExpr(0);
}

Ili2Parser::ComposedUnitExprContext* Ili2Parser::composedUnitExpr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  Ili2Parser::ComposedUnitExprContext *_localctx = _tracker.createInstance<ComposedUnitExprContext>(_ctx, parentState);
  Ili2Parser::ComposedUnitExprContext *previousContext = _localctx;
  size_t startState = 128;
  enterRecursionRule(_localctx, 128, Ili2Parser::RuleComposedUnitExpr, precedence);

    

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(1164);
    path();
    _ctx->stop = _input->LT(-1);
    setState(1174);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 149, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(1172);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 148, _ctx)) {
        case 1: {
          _localctx = _tracker.createInstance<ComposedUnitExprContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleComposedUnitExpr);
          setState(1166);

          if (!(precpred(_ctx, 2))) throw FailedPredicateException(this, "precpred(_ctx, 2)");
          setState(1167);
          match(Ili2Parser::STAR);
          setState(1168);
          path();
          break;
        }

        case 2: {
          _localctx = _tracker.createInstance<ComposedUnitExprContext>(parentContext, parentState);
          pushNewRecursionContext(_localctx, startState, RuleComposedUnitExpr);
          setState(1169);

          if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
          setState(1170);
          match(Ili2Parser::SLASH);
          setState(1171);
          path();
          break;
        }

        } 
      }
      setState(1176);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 149, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- MetaDataBasketDefContext ------------------------------------------------------------------

Ili2Parser::MetaDataBasketDefContext::MetaDataBasketDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::BASKET() {
  return getToken(Ili2Parser::BASKET, 0);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::TILDE() {
  return getToken(Ili2Parser::TILDE, 0);
}

Ili2Parser::PathContext* Ili2Parser::MetaDataBasketDefContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::SIGN() {
  return getToken(Ili2Parser::SIGN, 0);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::REFSYSTEM() {
  return getToken(Ili2Parser::REFSYSTEM, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::MetaDataBasketDefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::FINAL() {
  return getToken(Ili2Parser::FINAL, 0);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::EXTENDS() {
  return getToken(Ili2Parser::EXTENDS, 0);
}

Ili2Parser::MetaDataBasketRefContext* Ili2Parser::MetaDataBasketDefContext::metaDataBasketRef() {
  return getRuleContext<Ili2Parser::MetaDataBasketRefContext>(0);
}

std::vector<tree::TerminalNode *> Ili2Parser::MetaDataBasketDefContext::OBJECTS() {
  return getTokens(Ili2Parser::OBJECTS);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::OBJECTS(size_t i) {
  return getToken(Ili2Parser::OBJECTS, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::MetaDataBasketDefContext::OF() {
  return getTokens(Ili2Parser::OF);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::OF(size_t i) {
  return getToken(Ili2Parser::OF, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::MetaDataBasketDefContext::COLON() {
  return getTokens(Ili2Parser::COLON);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::COLON(size_t i) {
  return getToken(Ili2Parser::COLON, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::MetaDataBasketDefContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketDefContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::MetaDataBasketDefContext::getRuleIndex() const {
  return Ili2Parser::RuleMetaDataBasketDef;
}

antlrcpp::Any Ili2Parser::MetaDataBasketDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitMetaDataBasketDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::MetaDataBasketDefContext* Ili2Parser::metaDataBasketDef() {
  MetaDataBasketDefContext *_localctx = _tracker.createInstance<MetaDataBasketDefContext>(_ctx, getState());
  enterRule(_localctx, 130, Ili2Parser::RuleMetaDataBasketDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1177);
    _la = _input->LA(1);
    if (!(_la == Ili2Parser::REFSYSTEM || _la == Ili2Parser::SIGN)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(1178);
    match(Ili2Parser::BASKET);
    setState(1179);
    dynamic_cast<MetaDataBasketDefContext *>(_localctx)->basketname = match(Ili2Parser::NAME);
    setState(1183);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(1180);
      match(Ili2Parser::LPAREN);
      setState(1181);
      match(Ili2Parser::FINAL);
      setState(1182);
      match(Ili2Parser::RPAREN);
    }
    setState(1187);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXTENDS) {
      setState(1185);
      match(Ili2Parser::EXTENDS);
      setState(1186);
      metaDataBasketRef();
    }
    setState(1189);
    match(Ili2Parser::TILDE);
    setState(1190);
    path();
    setState(1205);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::OBJECTS) {
      setState(1191);
      match(Ili2Parser::OBJECTS);
      setState(1192);
      match(Ili2Parser::OF);
      setState(1193);
      dynamic_cast<MetaDataBasketDefContext *>(_localctx)->classname = match(Ili2Parser::NAME);
      setState(1194);
      match(Ili2Parser::COLON);
      setState(1195);
      dynamic_cast<MetaDataBasketDefContext *>(_localctx)->metabjectname = match(Ili2Parser::NAME);
      setState(1200);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili2Parser::COMMA) {
        setState(1196);
        match(Ili2Parser::COMMA);
        setState(1197);
        dynamic_cast<MetaDataBasketDefContext *>(_localctx)->metaobjectname = match(Ili2Parser::NAME);
        setState(1202);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(1207);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1208);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MetaDataBasketRefContext ------------------------------------------------------------------

Ili2Parser::MetaDataBasketRefContext::MetaDataBasketRefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> Ili2Parser::MetaDataBasketRefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketRefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::MetaDataBasketRefContext::DOT() {
  return getTokens(Ili2Parser::DOT);
}

tree::TerminalNode* Ili2Parser::MetaDataBasketRefContext::DOT(size_t i) {
  return getToken(Ili2Parser::DOT, i);
}


size_t Ili2Parser::MetaDataBasketRefContext::getRuleIndex() const {
  return Ili2Parser::RuleMetaDataBasketRef;
}

antlrcpp::Any Ili2Parser::MetaDataBasketRefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitMetaDataBasketRef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::MetaDataBasketRefContext* Ili2Parser::metaDataBasketRef() {
  MetaDataBasketRefContext *_localctx = _tracker.createInstance<MetaDataBasketRefContext>(_ctx, getState());
  enterRule(_localctx, 132, Ili2Parser::RuleMetaDataBasketRef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1216);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 155, _ctx)) {
    case 1: {
      setState(1212);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 154, _ctx)) {
      case 1: {
        setState(1210);
        dynamic_cast<MetaDataBasketRefContext *>(_localctx)->modelname = match(Ili2Parser::NAME);
        setState(1211);
        match(Ili2Parser::DOT);
        break;
      }

      }
      setState(1214);
      dynamic_cast<MetaDataBasketRefContext *>(_localctx)->topicname = match(Ili2Parser::NAME);
      setState(1215);
      match(Ili2Parser::DOT);
      break;
    }

    }
    setState(1218);
    dynamic_cast<MetaDataBasketRefContext *>(_localctx)->basketname = match(Ili2Parser::NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MetaObjectRefContext ------------------------------------------------------------------

Ili2Parser::MetaObjectRefContext::MetaObjectRefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::MetaObjectRefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

Ili2Parser::MetaDataBasketRefContext* Ili2Parser::MetaObjectRefContext::metaDataBasketRef() {
  return getRuleContext<Ili2Parser::MetaDataBasketRefContext>(0);
}

tree::TerminalNode* Ili2Parser::MetaObjectRefContext::DOT() {
  return getToken(Ili2Parser::DOT, 0);
}


size_t Ili2Parser::MetaObjectRefContext::getRuleIndex() const {
  return Ili2Parser::RuleMetaObjectRef;
}

antlrcpp::Any Ili2Parser::MetaObjectRefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitMetaObjectRef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::MetaObjectRefContext* Ili2Parser::metaObjectRef() {
  MetaObjectRefContext *_localctx = _tracker.createInstance<MetaObjectRefContext>(_ctx, getState());
  enterRule(_localctx, 134, Ili2Parser::RuleMetaObjectRef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1223);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 156, _ctx)) {
    case 1: {
      setState(1220);
      metaDataBasketRef();
      setState(1221);
      match(Ili2Parser::DOT);
      break;
    }

    }
    setState(1225);
    dynamic_cast<MetaObjectRefContext *>(_localctx)->metaobjectname = match(Ili2Parser::NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ParameterDefContext ------------------------------------------------------------------

Ili2Parser::ParameterDefContext::ParameterDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ParameterDefContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

tree::TerminalNode* Ili2Parser::ParameterDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::ParameterDefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

Ili2Parser::AttrTypeDefContext* Ili2Parser::ParameterDefContext::attrTypeDef() {
  return getRuleContext<Ili2Parser::AttrTypeDefContext>(0);
}

tree::TerminalNode* Ili2Parser::ParameterDefContext::METAOBJECT() {
  return getToken(Ili2Parser::METAOBJECT, 0);
}

Ili2Parser::PropertiesContext* Ili2Parser::ParameterDefContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

tree::TerminalNode* Ili2Parser::ParameterDefContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

Ili2Parser::PathContext* Ili2Parser::ParameterDefContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::ParameterDefContext::getRuleIndex() const {
  return Ili2Parser::RuleParameterDef;
}

antlrcpp::Any Ili2Parser::ParameterDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitParameterDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ParameterDefContext* Ili2Parser::parameterDef() {
  ParameterDefContext *_localctx = _tracker.createInstance<ParameterDefContext>(_ctx, getState());
  enterRule(_localctx, 136, Ili2Parser::RuleParameterDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1227);
    dynamic_cast<ParameterDefContext *>(_localctx)->parameterName = match(Ili2Parser::NAME);
    setState(1229);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(1228);
      properties();
    }
    setState(1231);
    match(Ili2Parser::COLON);
    setState(1238);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 159, _ctx)) {
    case 1: {
      setState(1232);
      attrTypeDef();
      break;
    }

    case 2: {
      setState(1233);
      match(Ili2Parser::METAOBJECT);
      setState(1236);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == Ili2Parser::OF) {
        setState(1234);
        match(Ili2Parser::OF);
        setState(1235);
        path();
      }
      break;
    }

    }
    setState(1240);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PropertiesContext ------------------------------------------------------------------

Ili2Parser::PropertiesContext::PropertiesContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::PropertiesContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::ABSTRACT() {
  return getTokens(Ili2Parser::ABSTRACT);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::ABSTRACT(size_t i) {
  return getToken(Ili2Parser::ABSTRACT, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::EXTENDED() {
  return getTokens(Ili2Parser::EXTENDED);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::EXTENDED(size_t i) {
  return getToken(Ili2Parser::EXTENDED, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::FINAL() {
  return getTokens(Ili2Parser::FINAL);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::FINAL(size_t i) {
  return getToken(Ili2Parser::FINAL, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::TRANSIENT() {
  return getTokens(Ili2Parser::TRANSIENT);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::TRANSIENT(size_t i) {
  return getToken(Ili2Parser::TRANSIENT, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::OID() {
  return getTokens(Ili2Parser::OID);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::OID(size_t i) {
  return getToken(Ili2Parser::OID, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::HIDING() {
  return getTokens(Ili2Parser::HIDING);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::HIDING(size_t i) {
  return getToken(Ili2Parser::HIDING, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::ORDERED() {
  return getTokens(Ili2Parser::ORDERED);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::ORDERED(size_t i) {
  return getToken(Ili2Parser::ORDERED, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::EXTERNAL() {
  return getTokens(Ili2Parser::EXTERNAL);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::EXTERNAL(size_t i) {
  return getToken(Ili2Parser::EXTERNAL, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::GENERIC() {
  return getTokens(Ili2Parser::GENERIC);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::GENERIC(size_t i) {
  return getToken(Ili2Parser::GENERIC, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::PropertiesContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::PropertiesContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::PropertiesContext::getRuleIndex() const {
  return Ili2Parser::RuleProperties;
}

antlrcpp::Any Ili2Parser::PropertiesContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitProperties(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::PropertiesContext* Ili2Parser::properties() {
  PropertiesContext *_localctx = _tracker.createInstance<PropertiesContext>(_ctx, getState());
  enterRule(_localctx, 138, Ili2Parser::RuleProperties);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1242);
    match(Ili2Parser::LPAREN);
    setState(1243);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << Ili2Parser::OID)
      | (1ULL << Ili2Parser::ABSTRACT)
      | (1ULL << Ili2Parser::EXTENDED)
      | (1ULL << Ili2Parser::FINAL)
      | (1ULL << Ili2Parser::TRANSIENT))) != 0) || ((((_la - 89) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 89)) & ((1ULL << (Ili2Parser::GENERIC - 89))
      | (1ULL << (Ili2Parser::EXTERNAL - 89))
      | (1ULL << (Ili2Parser::HIDING - 89))
      | (1ULL << (Ili2Parser::ORDERED - 89)))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(1248);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::COMMA) {
      setState(1244);
      match(Ili2Parser::COMMA);
      setState(1245);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << Ili2Parser::OID)
        | (1ULL << Ili2Parser::ABSTRACT)
        | (1ULL << Ili2Parser::EXTENDED)
        | (1ULL << Ili2Parser::FINAL)
        | (1ULL << Ili2Parser::TRANSIENT))) != 0) || ((((_la - 89) & ~ 0x3fULL) == 0) &&
        ((1ULL << (_la - 89)) & ((1ULL << (Ili2Parser::GENERIC - 89))
        | (1ULL << (Ili2Parser::EXTERNAL - 89))
        | (1ULL << (Ili2Parser::HIDING - 89))
        | (1ULL << (Ili2Parser::ORDERED - 89)))) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(1250);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1251);
    match(Ili2Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RunTimeParameterDefContext ------------------------------------------------------------------

Ili2Parser::RunTimeParameterDefContext::RunTimeParameterDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::RunTimeParameterDefContext::PARAMETER() {
  return getToken(Ili2Parser::PARAMETER, 0);
}

std::vector<Ili2Parser::RunTimeParameterContext *> Ili2Parser::RunTimeParameterDefContext::runTimeParameter() {
  return getRuleContexts<Ili2Parser::RunTimeParameterContext>();
}

Ili2Parser::RunTimeParameterContext* Ili2Parser::RunTimeParameterDefContext::runTimeParameter(size_t i) {
  return getRuleContext<Ili2Parser::RunTimeParameterContext>(i);
}


size_t Ili2Parser::RunTimeParameterDefContext::getRuleIndex() const {
  return Ili2Parser::RuleRunTimeParameterDef;
}

antlrcpp::Any Ili2Parser::RunTimeParameterDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitRunTimeParameterDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::RunTimeParameterDefContext* Ili2Parser::runTimeParameterDef() {
  RunTimeParameterDefContext *_localctx = _tracker.createInstance<RunTimeParameterDefContext>(_ctx, getState());
  enterRule(_localctx, 140, Ili2Parser::RuleRunTimeParameterDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(1253);
    match(Ili2Parser::PARAMETER);
    setState(1257);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 161, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(1254);
        runTimeParameter(); 
      }
      setState(1259);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 161, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RunTimeParameterContext ------------------------------------------------------------------

Ili2Parser::RunTimeParameterContext::RunTimeParameterContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::RunTimeParameterContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

Ili2Parser::AttrTypeDefContext* Ili2Parser::RunTimeParameterContext::attrTypeDef() {
  return getRuleContext<Ili2Parser::AttrTypeDefContext>(0);
}

tree::TerminalNode* Ili2Parser::RunTimeParameterContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::RunTimeParameterContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}


size_t Ili2Parser::RunTimeParameterContext::getRuleIndex() const {
  return Ili2Parser::RuleRunTimeParameter;
}

antlrcpp::Any Ili2Parser::RunTimeParameterContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitRunTimeParameter(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::RunTimeParameterContext* Ili2Parser::runTimeParameter() {
  RunTimeParameterContext *_localctx = _tracker.createInstance<RunTimeParameterContext>(_ctx, getState());
  enterRule(_localctx, 142, Ili2Parser::RuleRunTimeParameter);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1260);
    dynamic_cast<RunTimeParameterContext *>(_localctx)->runtimeparametername = match(Ili2Parser::NAME);
    setState(1261);
    match(Ili2Parser::COLON);
    setState(1262);
    attrTypeDef();
    setState(1263);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstraintDefContext ------------------------------------------------------------------

Ili2Parser::ConstraintDefContext::ConstraintDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::MandatoryConstraintContext* Ili2Parser::ConstraintDefContext::mandatoryConstraint() {
  return getRuleContext<Ili2Parser::MandatoryConstraintContext>(0);
}

Ili2Parser::PlausibilityConstraintContext* Ili2Parser::ConstraintDefContext::plausibilityConstraint() {
  return getRuleContext<Ili2Parser::PlausibilityConstraintContext>(0);
}

Ili2Parser::ExistenceConstraintContext* Ili2Parser::ConstraintDefContext::existenceConstraint() {
  return getRuleContext<Ili2Parser::ExistenceConstraintContext>(0);
}

Ili2Parser::UniquenessConstraintContext* Ili2Parser::ConstraintDefContext::uniquenessConstraint() {
  return getRuleContext<Ili2Parser::UniquenessConstraintContext>(0);
}

Ili2Parser::SetConstraintContext* Ili2Parser::ConstraintDefContext::setConstraint() {
  return getRuleContext<Ili2Parser::SetConstraintContext>(0);
}


size_t Ili2Parser::ConstraintDefContext::getRuleIndex() const {
  return Ili2Parser::RuleConstraintDef;
}

antlrcpp::Any Ili2Parser::ConstraintDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitConstraintDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ConstraintDefContext* Ili2Parser::constraintDef() {
  ConstraintDefContext *_localctx = _tracker.createInstance<ConstraintDefContext>(_ctx, getState());
  enterRule(_localctx, 144, Ili2Parser::RuleConstraintDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1270);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::MANDATORY: {
        enterOuterAlt(_localctx, 1);
        setState(1265);
        mandatoryConstraint();
        break;
      }

      case Ili2Parser::CONSTRAINT: {
        enterOuterAlt(_localctx, 2);
        setState(1266);
        plausibilityConstraint();
        break;
      }

      case Ili2Parser::EXISTENCE: {
        enterOuterAlt(_localctx, 3);
        setState(1267);
        existenceConstraint();
        break;
      }

      case Ili2Parser::UNIQUE: {
        enterOuterAlt(_localctx, 4);
        setState(1268);
        uniquenessConstraint();
        break;
      }

      case Ili2Parser::SET: {
        enterOuterAlt(_localctx, 5);
        setState(1269);
        setConstraint();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MandatoryConstraintContext ------------------------------------------------------------------

Ili2Parser::MandatoryConstraintContext::MandatoryConstraintContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::MandatoryConstraintContext::MANDATORY() {
  return getToken(Ili2Parser::MANDATORY, 0);
}

tree::TerminalNode* Ili2Parser::MandatoryConstraintContext::CONSTRAINT() {
  return getToken(Ili2Parser::CONSTRAINT, 0);
}

tree::TerminalNode* Ili2Parser::MandatoryConstraintContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

Ili2Parser::ExpressionContext* Ili2Parser::MandatoryConstraintContext::expression() {
  return getRuleContext<Ili2Parser::ExpressionContext>(0);
}

tree::TerminalNode* Ili2Parser::MandatoryConstraintContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

tree::TerminalNode* Ili2Parser::MandatoryConstraintContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}


size_t Ili2Parser::MandatoryConstraintContext::getRuleIndex() const {
  return Ili2Parser::RuleMandatoryConstraint;
}

antlrcpp::Any Ili2Parser::MandatoryConstraintContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitMandatoryConstraint(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::MandatoryConstraintContext* Ili2Parser::mandatoryConstraint() {
  MandatoryConstraintContext *_localctx = _tracker.createInstance<MandatoryConstraintContext>(_ctx, getState());
  enterRule(_localctx, 146, Ili2Parser::RuleMandatoryConstraint);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1272);
    match(Ili2Parser::MANDATORY);
    setState(1273);
    match(Ili2Parser::CONSTRAINT);
    setState(1277);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 163, _ctx)) {
    case 1: {
      setState(1274);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1275);
      dynamic_cast<MandatoryConstraintContext *>(_localctx)->name = match(Ili2Parser::NAME);
      setState(1276);
      match(Ili2Parser::COLON);
      break;
    }

    }
    setState(1279);
    dynamic_cast<MandatoryConstraintContext *>(_localctx)->booleanexp = expression();
    setState(1280);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PlausibilityConstraintContext ------------------------------------------------------------------

Ili2Parser::PlausibilityConstraintContext::PlausibilityConstraintContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::PlausibilityConstraintContext::CONSTRAINT() {
  return getToken(Ili2Parser::CONSTRAINT, 0);
}

tree::TerminalNode* Ili2Parser::PlausibilityConstraintContext::PERCENT() {
  return getToken(Ili2Parser::PERCENT, 0);
}

Ili2Parser::ExpressionContext* Ili2Parser::PlausibilityConstraintContext::expression() {
  return getRuleContext<Ili2Parser::ExpressionContext>(0);
}

tree::TerminalNode* Ili2Parser::PlausibilityConstraintContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::PlausibilityConstraintContext::LESSEQUAL() {
  return getToken(Ili2Parser::LESSEQUAL, 0);
}

tree::TerminalNode* Ili2Parser::PlausibilityConstraintContext::GREATEREQUAL() {
  return getToken(Ili2Parser::GREATEREQUAL, 0);
}

Ili2Parser::DecimalContext* Ili2Parser::PlausibilityConstraintContext::decimal() {
  return getRuleContext<Ili2Parser::DecimalContext>(0);
}

tree::TerminalNode* Ili2Parser::PlausibilityConstraintContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

tree::TerminalNode* Ili2Parser::PlausibilityConstraintContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}


size_t Ili2Parser::PlausibilityConstraintContext::getRuleIndex() const {
  return Ili2Parser::RulePlausibilityConstraint;
}

antlrcpp::Any Ili2Parser::PlausibilityConstraintContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitPlausibilityConstraint(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::PlausibilityConstraintContext* Ili2Parser::plausibilityConstraint() {
  PlausibilityConstraintContext *_localctx = _tracker.createInstance<PlausibilityConstraintContext>(_ctx, getState());
  enterRule(_localctx, 148, Ili2Parser::RulePlausibilityConstraint);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1282);
    match(Ili2Parser::CONSTRAINT);
    setState(1286);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 164, _ctx)) {
    case 1: {
      setState(1283);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1284);
      dynamic_cast<PlausibilityConstraintContext *>(_localctx)->name = match(Ili2Parser::NAME);
      setState(1285);
      match(Ili2Parser::COLON);
      break;
    }

    }
    setState(1288);
    _la = _input->LA(1);
    if (!(_la == Ili2Parser::LESSEQUAL

    || _la == Ili2Parser::GREATEREQUAL)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(1289);
    dynamic_cast<PlausibilityConstraintContext *>(_localctx)->percentage = decimal();
    setState(1290);
    match(Ili2Parser::PERCENT);
    setState(1291);
    expression();
    setState(1292);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExistenceConstraintContext ------------------------------------------------------------------

Ili2Parser::ExistenceConstraintContext::ExistenceConstraintContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ExistenceConstraintContext::EXISTENCE() {
  return getToken(Ili2Parser::EXISTENCE, 0);
}

tree::TerminalNode* Ili2Parser::ExistenceConstraintContext::CONSTRAINT() {
  return getToken(Ili2Parser::CONSTRAINT, 0);
}

std::vector<Ili2Parser::AttributePathContext *> Ili2Parser::ExistenceConstraintContext::attributePath() {
  return getRuleContexts<Ili2Parser::AttributePathContext>();
}

Ili2Parser::AttributePathContext* Ili2Parser::ExistenceConstraintContext::attributePath(size_t i) {
  return getRuleContext<Ili2Parser::AttributePathContext>(i);
}

tree::TerminalNode* Ili2Parser::ExistenceConstraintContext::REQUIRED() {
  return getToken(Ili2Parser::REQUIRED, 0);
}

tree::TerminalNode* Ili2Parser::ExistenceConstraintContext::IN() {
  return getToken(Ili2Parser::IN, 0);
}

std::vector<Ili2Parser::PathContext *> Ili2Parser::ExistenceConstraintContext::path() {
  return getRuleContexts<Ili2Parser::PathContext>();
}

Ili2Parser::PathContext* Ili2Parser::ExistenceConstraintContext::path(size_t i) {
  return getRuleContext<Ili2Parser::PathContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::ExistenceConstraintContext::COLON() {
  return getTokens(Ili2Parser::COLON);
}

tree::TerminalNode* Ili2Parser::ExistenceConstraintContext::COLON(size_t i) {
  return getToken(Ili2Parser::COLON, i);
}

tree::TerminalNode* Ili2Parser::ExistenceConstraintContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::ExistenceConstraintContext::OR() {
  return getTokens(Ili2Parser::OR);
}

tree::TerminalNode* Ili2Parser::ExistenceConstraintContext::OR(size_t i) {
  return getToken(Ili2Parser::OR, i);
}

tree::TerminalNode* Ili2Parser::ExistenceConstraintContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}


size_t Ili2Parser::ExistenceConstraintContext::getRuleIndex() const {
  return Ili2Parser::RuleExistenceConstraint;
}

antlrcpp::Any Ili2Parser::ExistenceConstraintContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitExistenceConstraint(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ExistenceConstraintContext* Ili2Parser::existenceConstraint() {
  ExistenceConstraintContext *_localctx = _tracker.createInstance<ExistenceConstraintContext>(_ctx, getState());
  enterRule(_localctx, 150, Ili2Parser::RuleExistenceConstraint);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1294);
    match(Ili2Parser::EXISTENCE);
    setState(1295);
    match(Ili2Parser::CONSTRAINT);
    setState(1299);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 165, _ctx)) {
    case 1: {
      setState(1296);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1297);
      dynamic_cast<ExistenceConstraintContext *>(_localctx)->name = match(Ili2Parser::NAME);
      setState(1298);
      match(Ili2Parser::COLON);
      break;
    }

    }
    setState(1301);
    attributePath();
    setState(1302);
    match(Ili2Parser::REQUIRED);
    setState(1303);
    match(Ili2Parser::IN);
    setState(1304);
    path();
    setState(1305);
    match(Ili2Parser::COLON);
    setState(1306);
    attributePath();
    setState(1314);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::OR) {
      setState(1307);
      match(Ili2Parser::OR);
      setState(1308);
      path();
      setState(1309);
      match(Ili2Parser::COLON);
      setState(1310);
      attributePath();
      setState(1316);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1317);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UniquenessConstraintContext ------------------------------------------------------------------

Ili2Parser::UniquenessConstraintContext::UniquenessConstraintContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::UniquenessConstraintContext::UNIQUE() {
  return getToken(Ili2Parser::UNIQUE, 0);
}

tree::TerminalNode* Ili2Parser::UniquenessConstraintContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

Ili2Parser::GlobalUniquenessContext* Ili2Parser::UniquenessConstraintContext::globalUniqueness() {
  return getRuleContext<Ili2Parser::GlobalUniquenessContext>(0);
}

Ili2Parser::LocalUniquenessContext* Ili2Parser::UniquenessConstraintContext::localUniqueness() {
  return getRuleContext<Ili2Parser::LocalUniquenessContext>(0);
}

tree::TerminalNode* Ili2Parser::UniquenessConstraintContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::UniquenessConstraintContext::BASKET() {
  return getToken(Ili2Parser::BASKET, 0);
}

tree::TerminalNode* Ili2Parser::UniquenessConstraintContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::UniquenessConstraintContext::COLON() {
  return getTokens(Ili2Parser::COLON);
}

tree::TerminalNode* Ili2Parser::UniquenessConstraintContext::COLON(size_t i) {
  return getToken(Ili2Parser::COLON, i);
}

tree::TerminalNode* Ili2Parser::UniquenessConstraintContext::WHERE() {
  return getToken(Ili2Parser::WHERE, 0);
}

Ili2Parser::ExpressionContext* Ili2Parser::UniquenessConstraintContext::expression() {
  return getRuleContext<Ili2Parser::ExpressionContext>(0);
}

tree::TerminalNode* Ili2Parser::UniquenessConstraintContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}


size_t Ili2Parser::UniquenessConstraintContext::getRuleIndex() const {
  return Ili2Parser::RuleUniquenessConstraint;
}

antlrcpp::Any Ili2Parser::UniquenessConstraintContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitUniquenessConstraint(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::UniquenessConstraintContext* Ili2Parser::uniquenessConstraint() {
  UniquenessConstraintContext *_localctx = _tracker.createInstance<UniquenessConstraintContext>(_ctx, getState());
  enterRule(_localctx, 152, Ili2Parser::RuleUniquenessConstraint);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1319);
    match(Ili2Parser::UNIQUE);
    setState(1324);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 167, _ctx)) {
    case 1: {
      setState(1320);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1321);
      match(Ili2Parser::LPAREN);
      setState(1322);
      match(Ili2Parser::BASKET);
      setState(1323);
      match(Ili2Parser::RPAREN);
      break;
    }

    }
    setState(1329);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 168, _ctx)) {
    case 1: {
      setState(1326);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1327);
      dynamic_cast<UniquenessConstraintContext *>(_localctx)->name = match(Ili2Parser::NAME);
      setState(1328);
      match(Ili2Parser::COLON);
      break;
    }

    }
    setState(1335);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::WHERE) {
      setState(1331);
      match(Ili2Parser::WHERE);
      setState(1332);
      expression();
      setState(1333);
      match(Ili2Parser::COLON);
    }
    setState(1339);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::BACKSLASH:
      case Ili2Parser::THIS:
      case Ili2Parser::THISAREA:
      case Ili2Parser::THATAREA:
      case Ili2Parser::PARENT:
      case Ili2Parser::AGGREGATES:
      case Ili2Parser::NAME: {
        setState(1337);
        globalUniqueness();
        break;
      }

      case Ili2Parser::LPAREN: {
        setState(1338);
        localUniqueness();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(1341);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GlobalUniquenessContext ------------------------------------------------------------------

Ili2Parser::GlobalUniquenessContext::GlobalUniquenessContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::UniqueElContext* Ili2Parser::GlobalUniquenessContext::uniqueEl() {
  return getRuleContext<Ili2Parser::UniqueElContext>(0);
}


size_t Ili2Parser::GlobalUniquenessContext::getRuleIndex() const {
  return Ili2Parser::RuleGlobalUniqueness;
}

antlrcpp::Any Ili2Parser::GlobalUniquenessContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitGlobalUniqueness(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::GlobalUniquenessContext* Ili2Parser::globalUniqueness() {
  GlobalUniquenessContext *_localctx = _tracker.createInstance<GlobalUniquenessContext>(_ctx, getState());
  enterRule(_localctx, 154, Ili2Parser::RuleGlobalUniqueness);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1343);
    uniqueEl();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UniqueElContext ------------------------------------------------------------------

Ili2Parser::UniqueElContext::UniqueElContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<Ili2Parser::ObjectOrAttributePathContext *> Ili2Parser::UniqueElContext::objectOrAttributePath() {
  return getRuleContexts<Ili2Parser::ObjectOrAttributePathContext>();
}

Ili2Parser::ObjectOrAttributePathContext* Ili2Parser::UniqueElContext::objectOrAttributePath(size_t i) {
  return getRuleContext<Ili2Parser::ObjectOrAttributePathContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::UniqueElContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::UniqueElContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::UniqueElContext::getRuleIndex() const {
  return Ili2Parser::RuleUniqueEl;
}

antlrcpp::Any Ili2Parser::UniqueElContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitUniqueEl(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::UniqueElContext* Ili2Parser::uniqueEl() {
  UniqueElContext *_localctx = _tracker.createInstance<UniqueElContext>(_ctx, getState());
  enterRule(_localctx, 156, Ili2Parser::RuleUniqueEl);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1345);
    objectOrAttributePath();
    setState(1350);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::COMMA) {
      setState(1346);
      match(Ili2Parser::COMMA);
      setState(1347);
      objectOrAttributePath();
      setState(1352);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LocalUniquenessContext ------------------------------------------------------------------

Ili2Parser::LocalUniquenessContext::LocalUniquenessContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::LocalUniquenessContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::LocalUniquenessContext::LOCAL() {
  return getToken(Ili2Parser::LOCAL, 0);
}

tree::TerminalNode* Ili2Parser::LocalUniquenessContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

Ili2Parser::LocalUniqueElContext* Ili2Parser::LocalUniquenessContext::localUniqueEl() {
  return getRuleContext<Ili2Parser::LocalUniqueElContext>(0);
}


size_t Ili2Parser::LocalUniquenessContext::getRuleIndex() const {
  return Ili2Parser::RuleLocalUniqueness;
}

antlrcpp::Any Ili2Parser::LocalUniquenessContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitLocalUniqueness(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::LocalUniquenessContext* Ili2Parser::localUniqueness() {
  LocalUniquenessContext *_localctx = _tracker.createInstance<LocalUniquenessContext>(_ctx, getState());
  enterRule(_localctx, 158, Ili2Parser::RuleLocalUniqueness);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1353);
    match(Ili2Parser::LPAREN);
    setState(1354);
    match(Ili2Parser::LOCAL);
    setState(1355);
    match(Ili2Parser::RPAREN);
    setState(1356);
    localUniqueEl();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LocalUniqueElContext ------------------------------------------------------------------

Ili2Parser::LocalUniqueElContext::LocalUniqueElContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::LocalUniqueElContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::LocalUniqueElContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::LocalUniqueElContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::LocalUniqueElContext::RARROW() {
  return getTokens(Ili2Parser::RARROW);
}

tree::TerminalNode* Ili2Parser::LocalUniqueElContext::RARROW(size_t i) {
  return getToken(Ili2Parser::RARROW, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::LocalUniqueElContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::LocalUniqueElContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::LocalUniqueElContext::getRuleIndex() const {
  return Ili2Parser::RuleLocalUniqueEl;
}

antlrcpp::Any Ili2Parser::LocalUniqueElContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitLocalUniqueEl(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::LocalUniqueElContext* Ili2Parser::localUniqueEl() {
  LocalUniqueElContext *_localctx = _tracker.createInstance<LocalUniqueElContext>(_ctx, getState());
  enterRule(_localctx, 160, Ili2Parser::RuleLocalUniqueEl);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1358);
    dynamic_cast<LocalUniqueElContext *>(_localctx)->structureattributename = match(Ili2Parser::NAME);
    setState(1363);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::RARROW) {
      setState(1359);
      match(Ili2Parser::RARROW);
      setState(1360);
      dynamic_cast<LocalUniqueElContext *>(_localctx)->structureattributename = match(Ili2Parser::NAME);
      setState(1365);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1366);
    match(Ili2Parser::COLON);
    setState(1367);
    dynamic_cast<LocalUniqueElContext *>(_localctx)->attributename = match(Ili2Parser::NAME);
    setState(1372);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::COMMA) {
      setState(1368);
      match(Ili2Parser::COMMA);
      setState(1369);
      dynamic_cast<LocalUniqueElContext *>(_localctx)->attributename = match(Ili2Parser::NAME);
      setState(1374);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SetConstraintContext ------------------------------------------------------------------

Ili2Parser::SetConstraintContext::SetConstraintContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::SetConstraintContext::SET() {
  return getToken(Ili2Parser::SET, 0);
}

tree::TerminalNode* Ili2Parser::SetConstraintContext::CONSTRAINT() {
  return getToken(Ili2Parser::CONSTRAINT, 0);
}

std::vector<Ili2Parser::ExpressionContext *> Ili2Parser::SetConstraintContext::expression() {
  return getRuleContexts<Ili2Parser::ExpressionContext>();
}

Ili2Parser::ExpressionContext* Ili2Parser::SetConstraintContext::expression(size_t i) {
  return getRuleContext<Ili2Parser::ExpressionContext>(i);
}

tree::TerminalNode* Ili2Parser::SetConstraintContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::SetConstraintContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::SetConstraintContext::BASKET() {
  return getToken(Ili2Parser::BASKET, 0);
}

tree::TerminalNode* Ili2Parser::SetConstraintContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::SetConstraintContext::COLON() {
  return getTokens(Ili2Parser::COLON);
}

tree::TerminalNode* Ili2Parser::SetConstraintContext::COLON(size_t i) {
  return getToken(Ili2Parser::COLON, i);
}

tree::TerminalNode* Ili2Parser::SetConstraintContext::WHERE() {
  return getToken(Ili2Parser::WHERE, 0);
}

tree::TerminalNode* Ili2Parser::SetConstraintContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}


size_t Ili2Parser::SetConstraintContext::getRuleIndex() const {
  return Ili2Parser::RuleSetConstraint;
}

antlrcpp::Any Ili2Parser::SetConstraintContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitSetConstraint(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::SetConstraintContext* Ili2Parser::setConstraint() {
  SetConstraintContext *_localctx = _tracker.createInstance<SetConstraintContext>(_ctx, getState());
  enterRule(_localctx, 162, Ili2Parser::RuleSetConstraint);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1375);
    match(Ili2Parser::SET);
    setState(1376);
    match(Ili2Parser::CONSTRAINT);
    setState(1381);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 174, _ctx)) {
    case 1: {
      setState(1377);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1378);
      match(Ili2Parser::LPAREN);
      setState(1379);
      match(Ili2Parser::BASKET);
      setState(1380);
      match(Ili2Parser::RPAREN);
      break;
    }

    }
    setState(1386);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 175, _ctx)) {
    case 1: {
      setState(1383);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1384);
      dynamic_cast<SetConstraintContext *>(_localctx)->name = match(Ili2Parser::NAME);
      setState(1385);
      match(Ili2Parser::COLON);
      break;
    }

    }
    setState(1392);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::WHERE) {
      setState(1388);
      match(Ili2Parser::WHERE);
      setState(1389);
      dynamic_cast<SetConstraintContext *>(_localctx)->logical = expression();
      setState(1390);
      match(Ili2Parser::COLON);
    }
    setState(1394);
    expression();
    setState(1395);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstraintsDefContext ------------------------------------------------------------------

Ili2Parser::ConstraintsDefContext::ConstraintsDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ConstraintsDefContext::CONSTRAINTS() {
  return getToken(Ili2Parser::CONSTRAINTS, 0);
}

tree::TerminalNode* Ili2Parser::ConstraintsDefContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

Ili2Parser::PathContext* Ili2Parser::ConstraintsDefContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

tree::TerminalNode* Ili2Parser::ConstraintsDefContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

tree::TerminalNode* Ili2Parser::ConstraintsDefContext::END() {
  return getToken(Ili2Parser::END, 0);
}

tree::TerminalNode* Ili2Parser::ConstraintsDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

std::vector<Ili2Parser::ConstraintDefContext *> Ili2Parser::ConstraintsDefContext::constraintDef() {
  return getRuleContexts<Ili2Parser::ConstraintDefContext>();
}

Ili2Parser::ConstraintDefContext* Ili2Parser::ConstraintsDefContext::constraintDef(size_t i) {
  return getRuleContext<Ili2Parser::ConstraintDefContext>(i);
}


size_t Ili2Parser::ConstraintsDefContext::getRuleIndex() const {
  return Ili2Parser::RuleConstraintsDef;
}

antlrcpp::Any Ili2Parser::ConstraintsDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitConstraintsDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ConstraintsDefContext* Ili2Parser::constraintsDef() {
  ConstraintsDefContext *_localctx = _tracker.createInstance<ConstraintsDefContext>(_ctx, getState());
  enterRule(_localctx, 164, Ili2Parser::RuleConstraintsDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1397);
    match(Ili2Parser::CONSTRAINTS);
    setState(1398);
    match(Ili2Parser::OF);
    setState(1399);
    path();
    setState(1400);
    match(Ili2Parser::EQUAL);
    setState(1404);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::MANDATORY

    || _la == Ili2Parser::CONSTRAINT || ((((_la - 152) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 152)) & ((1ULL << (Ili2Parser::EXISTENCE - 152))
      | (1ULL << (Ili2Parser::UNIQUE - 152))
      | (1ULL << (Ili2Parser::SET - 152)))) != 0)) {
      setState(1401);
      constraintDef();
      setState(1406);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1407);
    match(Ili2Parser::END);
    setState(1408);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpressionContext ------------------------------------------------------------------

Ili2Parser::ExpressionContext::ExpressionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::Term1Context* Ili2Parser::ExpressionContext::term1() {
  return getRuleContext<Ili2Parser::Term1Context>(0);
}


size_t Ili2Parser::ExpressionContext::getRuleIndex() const {
  return Ili2Parser::RuleExpression;
}

antlrcpp::Any Ili2Parser::ExpressionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitExpression(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ExpressionContext* Ili2Parser::expression() {
  ExpressionContext *_localctx = _tracker.createInstance<ExpressionContext>(_ctx, getState());
  enterRule(_localctx, 166, Ili2Parser::RuleExpression);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1410);
    term1();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Term1Context ------------------------------------------------------------------

Ili2Parser::Term1Context::Term1Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<Ili2Parser::Term2Context *> Ili2Parser::Term1Context::term2() {
  return getRuleContexts<Ili2Parser::Term2Context>();
}

Ili2Parser::Term2Context* Ili2Parser::Term1Context::term2(size_t i) {
  return getRuleContext<Ili2Parser::Term2Context>(i);
}

std::vector<Ili2Parser::Operator1Context *> Ili2Parser::Term1Context::operator1() {
  return getRuleContexts<Ili2Parser::Operator1Context>();
}

Ili2Parser::Operator1Context* Ili2Parser::Term1Context::operator1(size_t i) {
  return getRuleContext<Ili2Parser::Operator1Context>(i);
}


size_t Ili2Parser::Term1Context::getRuleIndex() const {
  return Ili2Parser::RuleTerm1;
}

antlrcpp::Any Ili2Parser::Term1Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitTerm1(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::Term1Context* Ili2Parser::term1() {
  Term1Context *_localctx = _tracker.createInstance<Term1Context>(_ctx, getState());
  enterRule(_localctx, 168, Ili2Parser::RuleTerm1);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(1412);
    term2();
    setState(1418);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 178, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(1413);
        operator1();
        setState(1414);
        term2(); 
      }
      setState(1420);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 178, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Operator1Context ------------------------------------------------------------------

Ili2Parser::Operator1Context::Operator1Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::Operator1Context::OR() {
  return getToken(Ili2Parser::OR, 0);
}

tree::TerminalNode* Ili2Parser::Operator1Context::PLUS() {
  return getToken(Ili2Parser::PLUS, 0);
}

tree::TerminalNode* Ili2Parser::Operator1Context::MINUS() {
  return getToken(Ili2Parser::MINUS, 0);
}

tree::TerminalNode* Ili2Parser::Operator1Context::IMPL() {
  return getToken(Ili2Parser::IMPL, 0);
}


size_t Ili2Parser::Operator1Context::getRuleIndex() const {
  return Ili2Parser::RuleOperator1;
}

antlrcpp::Any Ili2Parser::Operator1Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitOperator1(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::Operator1Context* Ili2Parser::operator1() {
  Operator1Context *_localctx = _tracker.createInstance<Operator1Context>(_ctx, getState());
  enterRule(_localctx, 170, Ili2Parser::RuleOperator1);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1428);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 179, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(1421);
      match(Ili2Parser::OR);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(1422);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1423);
      match(Ili2Parser::PLUS);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(1424);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1425);
      match(Ili2Parser::MINUS);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(1426);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1427);
      match(Ili2Parser::IMPL);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Term2Context ------------------------------------------------------------------

Ili2Parser::Term2Context::Term2Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<Ili2Parser::Term3Context *> Ili2Parser::Term2Context::term3() {
  return getRuleContexts<Ili2Parser::Term3Context>();
}

Ili2Parser::Term3Context* Ili2Parser::Term2Context::term3(size_t i) {
  return getRuleContext<Ili2Parser::Term3Context>(i);
}

std::vector<Ili2Parser::Operator2Context *> Ili2Parser::Term2Context::operator2() {
  return getRuleContexts<Ili2Parser::Operator2Context>();
}

Ili2Parser::Operator2Context* Ili2Parser::Term2Context::operator2(size_t i) {
  return getRuleContext<Ili2Parser::Operator2Context>(i);
}


size_t Ili2Parser::Term2Context::getRuleIndex() const {
  return Ili2Parser::RuleTerm2;
}

antlrcpp::Any Ili2Parser::Term2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitTerm2(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::Term2Context* Ili2Parser::term2() {
  Term2Context *_localctx = _tracker.createInstance<Term2Context>(_ctx, getState());
  enterRule(_localctx, 172, Ili2Parser::RuleTerm2);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(1430);
    term3();
    setState(1436);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 180, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(1431);
        operator2();
        setState(1432);
        term3(); 
      }
      setState(1438);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 180, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Operator2Context ------------------------------------------------------------------

Ili2Parser::Operator2Context::Operator2Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::Operator2Context::AND() {
  return getToken(Ili2Parser::AND, 0);
}

tree::TerminalNode* Ili2Parser::Operator2Context::STAR() {
  return getToken(Ili2Parser::STAR, 0);
}

tree::TerminalNode* Ili2Parser::Operator2Context::SLASH() {
  return getToken(Ili2Parser::SLASH, 0);
}


size_t Ili2Parser::Operator2Context::getRuleIndex() const {
  return Ili2Parser::RuleOperator2;
}

antlrcpp::Any Ili2Parser::Operator2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitOperator2(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::Operator2Context* Ili2Parser::operator2() {
  Operator2Context *_localctx = _tracker.createInstance<Operator2Context>(_ctx, getState());
  enterRule(_localctx, 174, Ili2Parser::RuleOperator2);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1444);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 181, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(1439);
      match(Ili2Parser::AND);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(1440);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1441);
      match(Ili2Parser::STAR);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(1442);

      if (!(ili24)) throw FailedPredicateException(this, "ili24");
      setState(1443);
      match(Ili2Parser::SLASH);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Term3Context ------------------------------------------------------------------

Ili2Parser::Term3Context::Term3Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<Ili2Parser::TermContext *> Ili2Parser::Term3Context::term() {
  return getRuleContexts<Ili2Parser::TermContext>();
}

Ili2Parser::TermContext* Ili2Parser::Term3Context::term(size_t i) {
  return getRuleContext<Ili2Parser::TermContext>(i);
}

Ili2Parser::RelationContext* Ili2Parser::Term3Context::relation() {
  return getRuleContext<Ili2Parser::RelationContext>(0);
}


size_t Ili2Parser::Term3Context::getRuleIndex() const {
  return Ili2Parser::RuleTerm3;
}

antlrcpp::Any Ili2Parser::Term3Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitTerm3(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::Term3Context* Ili2Parser::term3() {
  Term3Context *_localctx = _tracker.createInstance<Term3Context>(_ctx, getState());
  enterRule(_localctx, 176, Ili2Parser::RuleTerm3);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1446);
    dynamic_cast<Term3Context *>(_localctx)->t1 = term();
    setState(1450);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 182, _ctx)) {
    case 1: {
      setState(1447);
      relation();
      setState(1448);
      dynamic_cast<Term3Context *>(_localctx)->t2 = term();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TermContext ------------------------------------------------------------------

Ili2Parser::TermContext::TermContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::FactorContext* Ili2Parser::TermContext::factor() {
  return getRuleContext<Ili2Parser::FactorContext>(0);
}

tree::TerminalNode* Ili2Parser::TermContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

Ili2Parser::ExpressionContext* Ili2Parser::TermContext::expression() {
  return getRuleContext<Ili2Parser::ExpressionContext>(0);
}

tree::TerminalNode* Ili2Parser::TermContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

tree::TerminalNode* Ili2Parser::TermContext::NOT() {
  return getToken(Ili2Parser::NOT, 0);
}

tree::TerminalNode* Ili2Parser::TermContext::DEFINED() {
  return getToken(Ili2Parser::DEFINED, 0);
}


size_t Ili2Parser::TermContext::getRuleIndex() const {
  return Ili2Parser::RuleTerm;
}

antlrcpp::Any Ili2Parser::TermContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitTerm(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::TermContext* Ili2Parser::term() {
  TermContext *_localctx = _tracker.createInstance<TermContext>(_ctx, getState());
  enterRule(_localctx, 178, Ili2Parser::RuleTerm);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1465);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::INTERLIS:
      case Ili2Parser::REFSYSTEM:
      case Ili2Parser::BOOLEAN:
      case Ili2Parser::HALIGNMENT:
      case Ili2Parser::VALIGNMENT:
      case Ili2Parser::BACKSLASH:
      case Ili2Parser::HASH:
      case Ili2Parser::GREATERGREATER:
      case Ili2Parser::GREATER:
      case Ili2Parser::PARAMETER:
      case Ili2Parser::UNDEFINED:
      case Ili2Parser::URI:
      case Ili2Parser::PI:
      case Ili2Parser::LNBASE:
      case Ili2Parser::AREA:
      case Ili2Parser::SIGN:
      case Ili2Parser::METAOBJECT:
      case Ili2Parser::INSPECTION:
      case Ili2Parser::THIS:
      case Ili2Parser::THISAREA:
      case Ili2Parser::THATAREA:
      case Ili2Parser::PARENT:
      case Ili2Parser::AGGREGATES:
      case Ili2Parser::STRING:
      case Ili2Parser::POSNUMBER:
      case Ili2Parser::NUMBER:
      case Ili2Parser::DEC:
      case Ili2Parser::NAME: {
        enterOuterAlt(_localctx, 1);
        setState(1452);
        factor();
        break;
      }

      case Ili2Parser::LPAREN:
      case Ili2Parser::NOT: {
        enterOuterAlt(_localctx, 2);
        setState(1454);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == Ili2Parser::NOT) {
          setState(1453);
          match(Ili2Parser::NOT);
        }
        setState(1456);
        match(Ili2Parser::LPAREN);
        setState(1457);
        expression();
        setState(1458);
        match(Ili2Parser::RPAREN);
        break;
      }

      case Ili2Parser::DEFINED: {
        enterOuterAlt(_localctx, 3);
        setState(1460);
        match(Ili2Parser::DEFINED);
        setState(1461);
        match(Ili2Parser::LPAREN);
        setState(1462);
        factor();
        setState(1463);
        match(Ili2Parser::RPAREN);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RelationContext ------------------------------------------------------------------

Ili2Parser::RelationContext::RelationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::RelationContext::EQUALEQUAL() {
  return getToken(Ili2Parser::EQUALEQUAL, 0);
}

tree::TerminalNode* Ili2Parser::RelationContext::NOTEQUAL() {
  return getToken(Ili2Parser::NOTEQUAL, 0);
}

tree::TerminalNode* Ili2Parser::RelationContext::LESSGREATER() {
  return getToken(Ili2Parser::LESSGREATER, 0);
}

tree::TerminalNode* Ili2Parser::RelationContext::LESSEQUAL() {
  return getToken(Ili2Parser::LESSEQUAL, 0);
}

tree::TerminalNode* Ili2Parser::RelationContext::GREATEREQUAL() {
  return getToken(Ili2Parser::GREATEREQUAL, 0);
}

tree::TerminalNode* Ili2Parser::RelationContext::LESS() {
  return getToken(Ili2Parser::LESS, 0);
}

tree::TerminalNode* Ili2Parser::RelationContext::GREATER() {
  return getToken(Ili2Parser::GREATER, 0);
}


size_t Ili2Parser::RelationContext::getRuleIndex() const {
  return Ili2Parser::RuleRelation;
}

antlrcpp::Any Ili2Parser::RelationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitRelation(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::RelationContext* Ili2Parser::relation() {
  RelationContext *_localctx = _tracker.createInstance<RelationContext>(_ctx, getState());
  enterRule(_localctx, 180, Ili2Parser::RuleRelation);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1467);
    _la = _input->LA(1);
    if (!(((((_la - 62) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 62)) & ((1ULL << (Ili2Parser::LESS - 62))
      | (1ULL << (Ili2Parser::LESSEQUAL - 62))
      | (1ULL << (Ili2Parser::GREATER - 62))
      | (1ULL << (Ili2Parser::GREATEREQUAL - 62))
      | (1ULL << (Ili2Parser::EQUALEQUAL - 62))
      | (1ULL << (Ili2Parser::LESSGREATER - 62))
      | (1ULL << (Ili2Parser::NOTEQUAL - 62)))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FactorContext ------------------------------------------------------------------

Ili2Parser::FactorContext::FactorContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::ObjectOrAttributePathContext* Ili2Parser::FactorContext::objectOrAttributePath() {
  return getRuleContext<Ili2Parser::ObjectOrAttributePathContext>(0);
}

Ili2Parser::InspectionContext* Ili2Parser::FactorContext::inspection() {
  return getRuleContext<Ili2Parser::InspectionContext>(0);
}

tree::TerminalNode* Ili2Parser::FactorContext::INSPECTION() {
  return getToken(Ili2Parser::INSPECTION, 0);
}

Ili2Parser::PathContext* Ili2Parser::FactorContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

tree::TerminalNode* Ili2Parser::FactorContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

Ili2Parser::FunctionCallContext* Ili2Parser::FactorContext::functionCall() {
  return getRuleContext<Ili2Parser::FunctionCallContext>(0);
}

tree::TerminalNode* Ili2Parser::FactorContext::PARAMETER() {
  return getToken(Ili2Parser::PARAMETER, 0);
}

Ili2Parser::ConstantContext* Ili2Parser::FactorContext::constant() {
  return getRuleContext<Ili2Parser::ConstantContext>(0);
}


size_t Ili2Parser::FactorContext::getRuleIndex() const {
  return Ili2Parser::RuleFactor;
}

antlrcpp::Any Ili2Parser::FactorContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitFactor(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::FactorContext* Ili2Parser::factor() {
  FactorContext *_localctx = _tracker.createInstance<FactorContext>(_ctx, getState());
  enterRule(_localctx, 182, Ili2Parser::RuleFactor);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1483);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 187, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(1469);
      dynamic_cast<FactorContext *>(_localctx)->objectpath = objectOrAttributePath();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(1473);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 185, _ctx)) {
      case 1: {
        setState(1470);
        inspection();
        break;
      }

      case 2: {
        setState(1471);
        match(Ili2Parser::INSPECTION);
        setState(1472);
        path();
        break;
      }

      }
      setState(1477);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 186, _ctx)) {
      case 1: {
        setState(1475);
        match(Ili2Parser::OF);
        setState(1476);
        dynamic_cast<FactorContext *>(_localctx)->inspaectionpath = objectOrAttributePath();
        break;
      }

      }
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(1479);
      functionCall();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(1480);
      match(Ili2Parser::PARAMETER);
      setState(1481);
      dynamic_cast<FactorContext *>(_localctx)->parampath = path();
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(1482);
      constant();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ObjectOrAttributePathContext ------------------------------------------------------------------

Ili2Parser::ObjectOrAttributePathContext::ObjectOrAttributePathContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<Ili2Parser::PathElContext *> Ili2Parser::ObjectOrAttributePathContext::pathEl() {
  return getRuleContexts<Ili2Parser::PathElContext>();
}

Ili2Parser::PathElContext* Ili2Parser::ObjectOrAttributePathContext::pathEl(size_t i) {
  return getRuleContext<Ili2Parser::PathElContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::ObjectOrAttributePathContext::RARROW() {
  return getTokens(Ili2Parser::RARROW);
}

tree::TerminalNode* Ili2Parser::ObjectOrAttributePathContext::RARROW(size_t i) {
  return getToken(Ili2Parser::RARROW, i);
}


size_t Ili2Parser::ObjectOrAttributePathContext::getRuleIndex() const {
  return Ili2Parser::RuleObjectOrAttributePath;
}

antlrcpp::Any Ili2Parser::ObjectOrAttributePathContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitObjectOrAttributePath(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ObjectOrAttributePathContext* Ili2Parser::objectOrAttributePath() {
  ObjectOrAttributePathContext *_localctx = _tracker.createInstance<ObjectOrAttributePathContext>(_ctx, getState());
  enterRule(_localctx, 184, Ili2Parser::RuleObjectOrAttributePath);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(1485);
    pathEl();
    setState(1490);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 188, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(1486);
        match(Ili2Parser::RARROW);
        setState(1487);
        pathEl(); 
      }
      setState(1492);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 188, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttributePathContext ------------------------------------------------------------------

Ili2Parser::AttributePathContext::AttributePathContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::ObjectOrAttributePathContext* Ili2Parser::AttributePathContext::objectOrAttributePath() {
  return getRuleContext<Ili2Parser::ObjectOrAttributePathContext>(0);
}


size_t Ili2Parser::AttributePathContext::getRuleIndex() const {
  return Ili2Parser::RuleAttributePath;
}

antlrcpp::Any Ili2Parser::AttributePathContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAttributePath(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AttributePathContext* Ili2Parser::attributePath() {
  AttributePathContext *_localctx = _tracker.createInstance<AttributePathContext>(_ctx, getState());
  enterRule(_localctx, 186, Ili2Parser::RuleAttributePath);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1493);
    objectOrAttributePath();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PathElContext ------------------------------------------------------------------

Ili2Parser::PathElContext::PathElContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::PathElContext::THIS() {
  return getToken(Ili2Parser::THIS, 0);
}

tree::TerminalNode* Ili2Parser::PathElContext::THISAREA() {
  return getToken(Ili2Parser::THISAREA, 0);
}

tree::TerminalNode* Ili2Parser::PathElContext::THATAREA() {
  return getToken(Ili2Parser::THATAREA, 0);
}

tree::TerminalNode* Ili2Parser::PathElContext::PARENT() {
  return getToken(Ili2Parser::PARENT, 0);
}

Ili2Parser::ObjectRefContext* Ili2Parser::PathElContext::objectRef() {
  return getRuleContext<Ili2Parser::ObjectRefContext>(0);
}


size_t Ili2Parser::PathElContext::getRuleIndex() const {
  return Ili2Parser::RulePathEl;
}

antlrcpp::Any Ili2Parser::PathElContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitPathEl(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::PathElContext* Ili2Parser::pathEl() {
  PathElContext *_localctx = _tracker.createInstance<PathElContext>(_ctx, getState());
  enterRule(_localctx, 188, Ili2Parser::RulePathEl);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1500);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::THIS: {
        enterOuterAlt(_localctx, 1);
        setState(1495);
        match(Ili2Parser::THIS);
        break;
      }

      case Ili2Parser::THISAREA: {
        enterOuterAlt(_localctx, 2);
        setState(1496);
        match(Ili2Parser::THISAREA);
        break;
      }

      case Ili2Parser::THATAREA: {
        enterOuterAlt(_localctx, 3);
        setState(1497);
        match(Ili2Parser::THATAREA);
        break;
      }

      case Ili2Parser::PARENT: {
        enterOuterAlt(_localctx, 4);
        setState(1498);
        match(Ili2Parser::PARENT);
        break;
      }

      case Ili2Parser::BACKSLASH:
      case Ili2Parser::AGGREGATES:
      case Ili2Parser::NAME: {
        enterOuterAlt(_localctx, 5);
        setState(1499);
        objectRef();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ObjectRefContext ------------------------------------------------------------------

Ili2Parser::ObjectRefContext::ObjectRefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ObjectRefContext::AGGREGATES() {
  return getToken(Ili2Parser::AGGREGATES, 0);
}

tree::TerminalNode* Ili2Parser::ObjectRefContext::BACKSLASH() {
  return getToken(Ili2Parser::BACKSLASH, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::ObjectRefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::ObjectRefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

tree::TerminalNode* Ili2Parser::ObjectRefContext::LBRACE() {
  return getToken(Ili2Parser::LBRACE, 0);
}

tree::TerminalNode* Ili2Parser::ObjectRefContext::RBRACE() {
  return getToken(Ili2Parser::RBRACE, 0);
}

tree::TerminalNode* Ili2Parser::ObjectRefContext::FIRST() {
  return getToken(Ili2Parser::FIRST, 0);
}

tree::TerminalNode* Ili2Parser::ObjectRefContext::LAST() {
  return getToken(Ili2Parser::LAST, 0);
}

tree::TerminalNode* Ili2Parser::ObjectRefContext::POSNUMBER() {
  return getToken(Ili2Parser::POSNUMBER, 0);
}


size_t Ili2Parser::ObjectRefContext::getRuleIndex() const {
  return Ili2Parser::RuleObjectRef;
}

antlrcpp::Any Ili2Parser::ObjectRefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitObjectRef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ObjectRefContext* Ili2Parser::objectRef() {
  ObjectRefContext *_localctx = _tracker.createInstance<ObjectRefContext>(_ctx, getState());
  enterRule(_localctx, 190, Ili2Parser::RuleObjectRef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1503);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::BACKSLASH) {
      setState(1502);
      match(Ili2Parser::BACKSLASH);
    }
    setState(1517);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::NAME: {
        setState(1505);
        dynamic_cast<ObjectRefContext *>(_localctx)->name = match(Ili2Parser::NAME);
        setState(1514);
        _errHandler->sync(this);

        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 192, _ctx)) {
        case 1: {
          setState(1506);
          match(Ili2Parser::LBRACE);
          setState(1511);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case Ili2Parser::FIRST: {
              setState(1507);
              match(Ili2Parser::FIRST);
              break;
            }

            case Ili2Parser::LAST: {
              setState(1508);
              match(Ili2Parser::LAST);
              break;
            }

            case Ili2Parser::POSNUMBER: {
              setState(1509);
              dynamic_cast<ObjectRefContext *>(_localctx)->axislistindex = match(Ili2Parser::POSNUMBER);
              break;
            }

            case Ili2Parser::NAME: {
              setState(1510);
              dynamic_cast<ObjectRefContext *>(_localctx)->associationname = match(Ili2Parser::NAME);
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(1513);
          match(Ili2Parser::RBRACE);
          break;
        }

        }
        break;
      }

      case Ili2Parser::AGGREGATES: {
        setState(1516);
        match(Ili2Parser::AGGREGATES);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionCallContext ------------------------------------------------------------------

Ili2Parser::FunctionCallContext::FunctionCallContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::FunctionCallContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::FunctionCallContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

Ili2Parser::PathContext* Ili2Parser::FunctionCallContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

std::vector<Ili2Parser::FunctionCallArgumentContext *> Ili2Parser::FunctionCallContext::functionCallArgument() {
  return getRuleContexts<Ili2Parser::FunctionCallArgumentContext>();
}

Ili2Parser::FunctionCallArgumentContext* Ili2Parser::FunctionCallContext::functionCallArgument(size_t i) {
  return getRuleContext<Ili2Parser::FunctionCallArgumentContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::FunctionCallContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::FunctionCallContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::FunctionCallContext::getRuleIndex() const {
  return Ili2Parser::RuleFunctionCall;
}

antlrcpp::Any Ili2Parser::FunctionCallContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitFunctionCall(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::FunctionCallContext* Ili2Parser::functionCall() {
  FunctionCallContext *_localctx = _tracker.createInstance<FunctionCallContext>(_ctx, getState());
  enterRule(_localctx, 192, Ili2Parser::RuleFunctionCall);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1519);
    dynamic_cast<FunctionCallContext *>(_localctx)->functionname = path();
    setState(1520);
    match(Ili2Parser::LPAREN);
    setState(1529);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << Ili2Parser::INTERLIS)
      | (1ULL << Ili2Parser::REFSYSTEM)
      | (1ULL << Ili2Parser::BOOLEAN)
      | (1ULL << Ili2Parser::HALIGNMENT)
      | (1ULL << Ili2Parser::VALIGNMENT)
      | (1ULL << Ili2Parser::LPAREN)
      | (1ULL << Ili2Parser::BACKSLASH)
      | (1ULL << Ili2Parser::HASH))) != 0) || ((((_la - 65) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 65)) & ((1ULL << (Ili2Parser::GREATERGREATER - 65))
      | (1ULL << (Ili2Parser::GREATER - 65))
      | (1ULL << (Ili2Parser::PARAMETER - 65))
      | (1ULL << (Ili2Parser::UNDEFINED - 65))
      | (1ULL << (Ili2Parser::URI - 65))
      | (1ULL << (Ili2Parser::ALL - 65))
      | (1ULL << (Ili2Parser::PI - 65))
      | (1ULL << (Ili2Parser::LNBASE - 65)))) != 0) || ((((_la - 135) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 135)) & ((1ULL << (Ili2Parser::AREA - 135))
      | (1ULL << (Ili2Parser::SIGN - 135))
      | (1ULL << (Ili2Parser::METAOBJECT - 135))
      | (1ULL << (Ili2Parser::NOT - 135))
      | (1ULL << (Ili2Parser::DEFINED - 135))
      | (1ULL << (Ili2Parser::INSPECTION - 135))
      | (1ULL << (Ili2Parser::THIS - 135))
      | (1ULL << (Ili2Parser::THISAREA - 135))
      | (1ULL << (Ili2Parser::THATAREA - 135))
      | (1ULL << (Ili2Parser::PARENT - 135))
      | (1ULL << (Ili2Parser::AGGREGATES - 135))
      | (1ULL << (Ili2Parser::STRING - 135))
      | (1ULL << (Ili2Parser::POSNUMBER - 135))
      | (1ULL << (Ili2Parser::NUMBER - 135))
      | (1ULL << (Ili2Parser::DEC - 135))
      | (1ULL << (Ili2Parser::NAME - 135)))) != 0)) {
      setState(1521);
      functionCallArgument();
      setState(1526);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili2Parser::COMMA) {
        setState(1522);
        match(Ili2Parser::COMMA);
        setState(1523);
        functionCallArgument();
        setState(1528);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(1531);
    match(Ili2Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionCallArgumentContext ------------------------------------------------------------------

Ili2Parser::FunctionCallArgumentContext::FunctionCallArgumentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::ExpressionContext* Ili2Parser::FunctionCallArgumentContext::expression() {
  return getRuleContext<Ili2Parser::ExpressionContext>(0);
}

tree::TerminalNode* Ili2Parser::FunctionCallArgumentContext::ALL() {
  return getToken(Ili2Parser::ALL, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::FunctionCallArgumentContext::LPAREN() {
  return getTokens(Ili2Parser::LPAREN);
}

tree::TerminalNode* Ili2Parser::FunctionCallArgumentContext::LPAREN(size_t i) {
  return getToken(Ili2Parser::LPAREN, i);
}

std::vector<Ili2Parser::RestrictedRefContext *> Ili2Parser::FunctionCallArgumentContext::restrictedRef() {
  return getRuleContexts<Ili2Parser::RestrictedRefContext>();
}

Ili2Parser::RestrictedRefContext* Ili2Parser::FunctionCallArgumentContext::restrictedRef(size_t i) {
  return getRuleContext<Ili2Parser::RestrictedRefContext>(i);
}

std::vector<Ili2Parser::PathContext *> Ili2Parser::FunctionCallArgumentContext::path() {
  return getRuleContexts<Ili2Parser::PathContext>();
}

Ili2Parser::PathContext* Ili2Parser::FunctionCallArgumentContext::path(size_t i) {
  return getRuleContext<Ili2Parser::PathContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::FunctionCallArgumentContext::RPAREN() {
  return getTokens(Ili2Parser::RPAREN);
}

tree::TerminalNode* Ili2Parser::FunctionCallArgumentContext::RPAREN(size_t i) {
  return getToken(Ili2Parser::RPAREN, i);
}


size_t Ili2Parser::FunctionCallArgumentContext::getRuleIndex() const {
  return Ili2Parser::RuleFunctionCallArgument;
}

antlrcpp::Any Ili2Parser::FunctionCallArgumentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitFunctionCallArgument(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::FunctionCallArgumentContext* Ili2Parser::functionCallArgument() {
  FunctionCallArgumentContext *_localctx = _tracker.createInstance<FunctionCallArgumentContext>(_ctx, getState());
  enterRule(_localctx, 194, Ili2Parser::RuleFunctionCallArgument);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1545);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::INTERLIS:
      case Ili2Parser::REFSYSTEM:
      case Ili2Parser::BOOLEAN:
      case Ili2Parser::HALIGNMENT:
      case Ili2Parser::VALIGNMENT:
      case Ili2Parser::LPAREN:
      case Ili2Parser::BACKSLASH:
      case Ili2Parser::HASH:
      case Ili2Parser::GREATERGREATER:
      case Ili2Parser::GREATER:
      case Ili2Parser::PARAMETER:
      case Ili2Parser::UNDEFINED:
      case Ili2Parser::URI:
      case Ili2Parser::PI:
      case Ili2Parser::LNBASE:
      case Ili2Parser::AREA:
      case Ili2Parser::SIGN:
      case Ili2Parser::METAOBJECT:
      case Ili2Parser::NOT:
      case Ili2Parser::DEFINED:
      case Ili2Parser::INSPECTION:
      case Ili2Parser::THIS:
      case Ili2Parser::THISAREA:
      case Ili2Parser::THATAREA:
      case Ili2Parser::PARENT:
      case Ili2Parser::AGGREGATES:
      case Ili2Parser::STRING:
      case Ili2Parser::POSNUMBER:
      case Ili2Parser::NUMBER:
      case Ili2Parser::DEC:
      case Ili2Parser::NAME: {
        enterOuterAlt(_localctx, 1);
        setState(1533);
        expression();
        break;
      }

      case Ili2Parser::ALL: {
        enterOuterAlt(_localctx, 2);
        setState(1534);
        match(Ili2Parser::ALL);
        setState(1542);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << Ili2Parser::INTERLIS)
          | (1ULL << Ili2Parser::REFSYSTEM)
          | (1ULL << Ili2Parser::BOOLEAN)
          | (1ULL << Ili2Parser::HALIGNMENT)
          | (1ULL << Ili2Parser::VALIGNMENT)
          | (1ULL << Ili2Parser::LPAREN))) != 0) || ((((_la - 110) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 110)) & ((1ULL << (Ili2Parser::URI - 110))
          | (1ULL << (Ili2Parser::SIGN - 110))
          | (1ULL << (Ili2Parser::METAOBJECT - 110)))) != 0) || _la == Ili2Parser::NAME) {
          setState(1540);
          _errHandler->sync(this);
          switch (_input->LA(1)) {
            case Ili2Parser::LPAREN: {
              setState(1535);
              match(Ili2Parser::LPAREN);
              setState(1536);
              restrictedRef();
              break;
            }

            case Ili2Parser::INTERLIS:
            case Ili2Parser::REFSYSTEM:
            case Ili2Parser::BOOLEAN:
            case Ili2Parser::HALIGNMENT:
            case Ili2Parser::VALIGNMENT:
            case Ili2Parser::URI:
            case Ili2Parser::SIGN:
            case Ili2Parser::METAOBJECT:
            case Ili2Parser::NAME: {
              setState(1537);
              path();
              setState(1538);
              match(Ili2Parser::RPAREN);
              break;
            }

          default:
            throw NoViableAltException(this);
          }
          setState(1544);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionDefContext ------------------------------------------------------------------

Ili2Parser::FunctionDefContext::FunctionDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::FunctionDefContext::FUNCTION() {
  return getToken(Ili2Parser::FUNCTION, 0);
}

tree::TerminalNode* Ili2Parser::FunctionDefContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

tree::TerminalNode* Ili2Parser::FunctionDefContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

tree::TerminalNode* Ili2Parser::FunctionDefContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::FunctionDefContext::SEMI() {
  return getTokens(Ili2Parser::SEMI);
}

tree::TerminalNode* Ili2Parser::FunctionDefContext::SEMI(size_t i) {
  return getToken(Ili2Parser::SEMI, i);
}

tree::TerminalNode* Ili2Parser::FunctionDefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

Ili2Parser::ArgumentTypeContext* Ili2Parser::FunctionDefContext::argumentType() {
  return getRuleContext<Ili2Parser::ArgumentTypeContext>(0);
}

std::vector<Ili2Parser::FunctionDefParamContext *> Ili2Parser::FunctionDefContext::functionDefParam() {
  return getRuleContexts<Ili2Parser::FunctionDefParamContext>();
}

Ili2Parser::FunctionDefParamContext* Ili2Parser::FunctionDefContext::functionDefParam(size_t i) {
  return getRuleContext<Ili2Parser::FunctionDefParamContext>(i);
}

tree::TerminalNode* Ili2Parser::FunctionDefContext::EXPLANATION() {
  return getToken(Ili2Parser::EXPLANATION, 0);
}


size_t Ili2Parser::FunctionDefContext::getRuleIndex() const {
  return Ili2Parser::RuleFunctionDef;
}

antlrcpp::Any Ili2Parser::FunctionDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitFunctionDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::FunctionDefContext* Ili2Parser::functionDef() {
  FunctionDefContext *_localctx = _tracker.createInstance<FunctionDefContext>(_ctx, getState());
  enterRule(_localctx, 196, Ili2Parser::RuleFunctionDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1547);
    match(Ili2Parser::FUNCTION);
    setState(1548);
    dynamic_cast<FunctionDefContext *>(_localctx)->functioname = match(Ili2Parser::NAME);
    setState(1549);
    match(Ili2Parser::LPAREN);
    setState(1558);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::NAME) {
      setState(1550);
      functionDefParam();
      setState(1555);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili2Parser::SEMI) {
        setState(1551);
        match(Ili2Parser::SEMI);
        setState(1552);
        functionDefParam();
        setState(1557);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
    setState(1560);
    match(Ili2Parser::RPAREN);
    setState(1561);
    match(Ili2Parser::COLON);
    setState(1562);
    dynamic_cast<FunctionDefContext *>(_localctx)->result = argumentType();
    setState(1564);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXPLANATION) {
      setState(1563);
      match(Ili2Parser::EXPLANATION);
    }
    setState(1566);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FunctionDefParamContext ------------------------------------------------------------------

Ili2Parser::FunctionDefParamContext::FunctionDefParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::FunctionDefParamContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

Ili2Parser::ArgumentTypeContext* Ili2Parser::FunctionDefParamContext::argumentType() {
  return getRuleContext<Ili2Parser::ArgumentTypeContext>(0);
}

tree::TerminalNode* Ili2Parser::FunctionDefParamContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}


size_t Ili2Parser::FunctionDefParamContext::getRuleIndex() const {
  return Ili2Parser::RuleFunctionDefParam;
}

antlrcpp::Any Ili2Parser::FunctionDefParamContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitFunctionDefParam(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::FunctionDefParamContext* Ili2Parser::functionDefParam() {
  FunctionDefParamContext *_localctx = _tracker.createInstance<FunctionDefParamContext>(_ctx, getState());
  enterRule(_localctx, 198, Ili2Parser::RuleFunctionDefParam);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1568);
    dynamic_cast<FunctionDefParamContext *>(_localctx)->argumentname = match(Ili2Parser::NAME);
    setState(1569);
    match(Ili2Parser::COLON);
    setState(1570);
    argumentType();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArgumentTypeContext ------------------------------------------------------------------

Ili2Parser::ArgumentTypeContext::ArgumentTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::AttrTypeDefContext* Ili2Parser::ArgumentTypeContext::attrTypeDef() {
  return getRuleContext<Ili2Parser::AttrTypeDefContext>(0);
}

tree::TerminalNode* Ili2Parser::ArgumentTypeContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

tree::TerminalNode* Ili2Parser::ArgumentTypeContext::OBJECT() {
  return getToken(Ili2Parser::OBJECT, 0);
}

tree::TerminalNode* Ili2Parser::ArgumentTypeContext::OBJECTS() {
  return getToken(Ili2Parser::OBJECTS, 0);
}

Ili2Parser::RestrictedRefContext* Ili2Parser::ArgumentTypeContext::restrictedRef() {
  return getRuleContext<Ili2Parser::RestrictedRefContext>(0);
}

Ili2Parser::PathContext* Ili2Parser::ArgumentTypeContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

tree::TerminalNode* Ili2Parser::ArgumentTypeContext::ENUMVAL() {
  return getToken(Ili2Parser::ENUMVAL, 0);
}

tree::TerminalNode* Ili2Parser::ArgumentTypeContext::ENUMTREEVAL() {
  return getToken(Ili2Parser::ENUMTREEVAL, 0);
}


size_t Ili2Parser::ArgumentTypeContext::getRuleIndex() const {
  return Ili2Parser::RuleArgumentType;
}

antlrcpp::Any Ili2Parser::ArgumentTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitArgumentType(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ArgumentTypeContext* Ili2Parser::argumentType() {
  ArgumentTypeContext *_localctx = _tracker.createInstance<ArgumentTypeContext>(_ctx, getState());
  enterRule(_localctx, 200, Ili2Parser::RuleArgumentType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1581);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 203, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(1572);
      attrTypeDef();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(1573);
      _la = _input->LA(1);
      if (!(_la == Ili2Parser::OBJECTS

      || _la == Ili2Parser::OBJECT)) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      setState(1574);
      match(Ili2Parser::OF);
      setState(1577);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 202, _ctx)) {
      case 1: {
        setState(1575);
        restrictedRef();
        break;
      }

      case 2: {
        setState(1576);
        dynamic_cast<ArgumentTypeContext *>(_localctx)->viewref = path();
        break;
      }

      }
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(1579);
      match(Ili2Parser::ENUMVAL);
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(1580);
      match(Ili2Parser::ENUMTREEVAL);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ViewDefContext ------------------------------------------------------------------

Ili2Parser::ViewDefContext::ViewDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ViewDefContext::VIEW() {
  return getToken(Ili2Parser::VIEW, 0);
}

tree::TerminalNode* Ili2Parser::ViewDefContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

tree::TerminalNode* Ili2Parser::ViewDefContext::END() {
  return getToken(Ili2Parser::END, 0);
}

tree::TerminalNode* Ili2Parser::ViewDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::ViewDefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::ViewDefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

tree::TerminalNode* Ili2Parser::ViewDefContext::EXTENDS() {
  return getToken(Ili2Parser::EXTENDS, 0);
}

Ili2Parser::PropertiesContext* Ili2Parser::ViewDefContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

Ili2Parser::PathContext* Ili2Parser::ViewDefContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

std::vector<Ili2Parser::BaseExtensionDefContext *> Ili2Parser::ViewDefContext::baseExtensionDef() {
  return getRuleContexts<Ili2Parser::BaseExtensionDefContext>();
}

Ili2Parser::BaseExtensionDefContext* Ili2Parser::ViewDefContext::baseExtensionDef(size_t i) {
  return getRuleContext<Ili2Parser::BaseExtensionDefContext>(i);
}

std::vector<Ili2Parser::SelectionContext *> Ili2Parser::ViewDefContext::selection() {
  return getRuleContexts<Ili2Parser::SelectionContext>();
}

Ili2Parser::SelectionContext* Ili2Parser::ViewDefContext::selection(size_t i) {
  return getRuleContext<Ili2Parser::SelectionContext>(i);
}

tree::TerminalNode* Ili2Parser::ViewDefContext::ATTRIBUTE() {
  return getToken(Ili2Parser::ATTRIBUTE, 0);
}

std::vector<Ili2Parser::ViewAttributeContext *> Ili2Parser::ViewDefContext::viewAttribute() {
  return getRuleContexts<Ili2Parser::ViewAttributeContext>();
}

Ili2Parser::ViewAttributeContext* Ili2Parser::ViewDefContext::viewAttribute(size_t i) {
  return getRuleContext<Ili2Parser::ViewAttributeContext>(i);
}

std::vector<Ili2Parser::ConstraintDefContext *> Ili2Parser::ViewDefContext::constraintDef() {
  return getRuleContexts<Ili2Parser::ConstraintDefContext>();
}

Ili2Parser::ConstraintDefContext* Ili2Parser::ViewDefContext::constraintDef(size_t i) {
  return getRuleContext<Ili2Parser::ConstraintDefContext>(i);
}

Ili2Parser::FormationDefContext* Ili2Parser::ViewDefContext::formationDef() {
  return getRuleContext<Ili2Parser::FormationDefContext>(0);
}


size_t Ili2Parser::ViewDefContext::getRuleIndex() const {
  return Ili2Parser::RuleViewDef;
}

antlrcpp::Any Ili2Parser::ViewDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitViewDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ViewDefContext* Ili2Parser::viewDef() {
  ViewDefContext *_localctx = _tracker.createInstance<ViewDefContext>(_ctx, getState());
  enterRule(_localctx, 202, Ili2Parser::RuleViewDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1583);
    match(Ili2Parser::VIEW);
    setState(1584);
    dynamic_cast<ViewDefContext *>(_localctx)->viewname1 = match(Ili2Parser::NAME);
    setState(1586);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(1585);
      properties();
    }
    setState(1593);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::EQUAL:
      case Ili2Parser::AREA:
      case Ili2Parser::WHERE:
      case Ili2Parser::BASE:
      case Ili2Parser::INSPECTION:
      case Ili2Parser::PROJECTION:
      case Ili2Parser::JOIN:
      case Ili2Parser::UNION:
      case Ili2Parser::AGGREGATION: {
        setState(1589);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (((((_la - 135) & ~ 0x3fULL) == 0) &&
          ((1ULL << (_la - 135)) & ((1ULL << (Ili2Parser::AREA - 135))
          | (1ULL << (Ili2Parser::INSPECTION - 135))
          | (1ULL << (Ili2Parser::PROJECTION - 135))
          | (1ULL << (Ili2Parser::JOIN - 135))
          | (1ULL << (Ili2Parser::UNION - 135))
          | (1ULL << (Ili2Parser::AGGREGATION - 135)))) != 0)) {
          setState(1588);
          formationDef();
        }
        break;
      }

      case Ili2Parser::EXTENDS: {
        setState(1591);
        match(Ili2Parser::EXTENDS);
        setState(1592);
        dynamic_cast<ViewDefContext *>(_localctx)->viewref = path();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(1598);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::BASE) {
      setState(1595);
      baseExtensionDef();
      setState(1600);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1604);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::WHERE) {
      setState(1601);
      selection();
      setState(1606);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1607);
    match(Ili2Parser::EQUAL);
    setState(1609);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::ATTRIBUTE) {
      setState(1608);
      match(Ili2Parser::ATTRIBUTE);
    }
    setState(1614);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::CONTINUOUS

    || _la == Ili2Parser::SUBDIVISION || _la == Ili2Parser::ALL || _la == Ili2Parser::NAME) {
      setState(1611);
      viewAttribute();
      setState(1616);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1620);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::MANDATORY

    || _la == Ili2Parser::CONSTRAINT || ((((_la - 152) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 152)) & ((1ULL << (Ili2Parser::EXISTENCE - 152))
      | (1ULL << (Ili2Parser::UNIQUE - 152))
      | (1ULL << (Ili2Parser::SET - 152)))) != 0)) {
      setState(1617);
      constraintDef();
      setState(1622);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1623);
    match(Ili2Parser::END);
    setState(1624);
    dynamic_cast<ViewDefContext *>(_localctx)->viewname2 = match(Ili2Parser::NAME);
    setState(1625);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FormationDefContext ------------------------------------------------------------------

Ili2Parser::FormationDefContext::FormationDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::FormationDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

Ili2Parser::ProjectionContext* Ili2Parser::FormationDefContext::projection() {
  return getRuleContext<Ili2Parser::ProjectionContext>(0);
}

Ili2Parser::JoinContext* Ili2Parser::FormationDefContext::join() {
  return getRuleContext<Ili2Parser::JoinContext>(0);
}

Ili2Parser::IliunionContext* Ili2Parser::FormationDefContext::iliunion() {
  return getRuleContext<Ili2Parser::IliunionContext>(0);
}

Ili2Parser::AggregationContext* Ili2Parser::FormationDefContext::aggregation() {
  return getRuleContext<Ili2Parser::AggregationContext>(0);
}

Ili2Parser::InspectionContext* Ili2Parser::FormationDefContext::inspection() {
  return getRuleContext<Ili2Parser::InspectionContext>(0);
}


size_t Ili2Parser::FormationDefContext::getRuleIndex() const {
  return Ili2Parser::RuleFormationDef;
}

antlrcpp::Any Ili2Parser::FormationDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitFormationDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::FormationDefContext* Ili2Parser::formationDef() {
  FormationDefContext *_localctx = _tracker.createInstance<FormationDefContext>(_ctx, getState());
  enterRule(_localctx, 204, Ili2Parser::RuleFormationDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1632);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::PROJECTION: {
        setState(1627);
        projection();
        break;
      }

      case Ili2Parser::JOIN: {
        setState(1628);
        join();
        break;
      }

      case Ili2Parser::UNION: {
        setState(1629);
        iliunion();
        break;
      }

      case Ili2Parser::AGGREGATION: {
        setState(1630);
        aggregation();
        break;
      }

      case Ili2Parser::AREA:
      case Ili2Parser::INSPECTION: {
        setState(1631);
        inspection();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(1634);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ProjectionContext ------------------------------------------------------------------

Ili2Parser::ProjectionContext::ProjectionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ProjectionContext::PROJECTION() {
  return getToken(Ili2Parser::PROJECTION, 0);
}

tree::TerminalNode* Ili2Parser::ProjectionContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

Ili2Parser::RenamedViewableRefContext* Ili2Parser::ProjectionContext::renamedViewableRef() {
  return getRuleContext<Ili2Parser::RenamedViewableRefContext>(0);
}


size_t Ili2Parser::ProjectionContext::getRuleIndex() const {
  return Ili2Parser::RuleProjection;
}

antlrcpp::Any Ili2Parser::ProjectionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitProjection(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ProjectionContext* Ili2Parser::projection() {
  ProjectionContext *_localctx = _tracker.createInstance<ProjectionContext>(_ctx, getState());
  enterRule(_localctx, 206, Ili2Parser::RuleProjection);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1636);
    match(Ili2Parser::PROJECTION);
    setState(1637);
    match(Ili2Parser::OF);
    setState(1638);
    renamedViewableRef();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- JoinContext ------------------------------------------------------------------

Ili2Parser::JoinContext::JoinContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::JoinContext::JOIN() {
  return getToken(Ili2Parser::JOIN, 0);
}

tree::TerminalNode* Ili2Parser::JoinContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

std::vector<Ili2Parser::RenamedViewableRefContext *> Ili2Parser::JoinContext::renamedViewableRef() {
  return getRuleContexts<Ili2Parser::RenamedViewableRefContext>();
}

Ili2Parser::RenamedViewableRefContext* Ili2Parser::JoinContext::renamedViewableRef(size_t i) {
  return getRuleContext<Ili2Parser::RenamedViewableRefContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::JoinContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::JoinContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::JoinContext::LPAREN() {
  return getTokens(Ili2Parser::LPAREN);
}

tree::TerminalNode* Ili2Parser::JoinContext::LPAREN(size_t i) {
  return getToken(Ili2Parser::LPAREN, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::JoinContext::OR() {
  return getTokens(Ili2Parser::OR);
}

tree::TerminalNode* Ili2Parser::JoinContext::OR(size_t i) {
  return getToken(Ili2Parser::OR, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::JoinContext::ILINULL() {
  return getTokens(Ili2Parser::ILINULL);
}

tree::TerminalNode* Ili2Parser::JoinContext::ILINULL(size_t i) {
  return getToken(Ili2Parser::ILINULL, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::JoinContext::RPAREN() {
  return getTokens(Ili2Parser::RPAREN);
}

tree::TerminalNode* Ili2Parser::JoinContext::RPAREN(size_t i) {
  return getToken(Ili2Parser::RPAREN, i);
}


size_t Ili2Parser::JoinContext::getRuleIndex() const {
  return Ili2Parser::RuleJoin;
}

antlrcpp::Any Ili2Parser::JoinContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitJoin(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::JoinContext* Ili2Parser::join() {
  JoinContext *_localctx = _tracker.createInstance<JoinContext>(_ctx, getState());
  enterRule(_localctx, 208, Ili2Parser::RuleJoin);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1640);
    match(Ili2Parser::JOIN);
    setState(1641);
    match(Ili2Parser::OF);
    setState(1642);
    renamedViewableRef();
    setState(1651); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(1643);
      match(Ili2Parser::COMMA);
      setState(1644);
      renamedViewableRef();
      setState(1649);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == Ili2Parser::LPAREN) {
        setState(1645);
        match(Ili2Parser::LPAREN);
        setState(1646);
        match(Ili2Parser::OR);
        setState(1647);
        match(Ili2Parser::ILINULL);
        setState(1648);
        match(Ili2Parser::RPAREN);
      }
      setState(1653); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == Ili2Parser::COMMA);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IliunionContext ------------------------------------------------------------------

Ili2Parser::IliunionContext::IliunionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::IliunionContext::UNION() {
  return getToken(Ili2Parser::UNION, 0);
}

tree::TerminalNode* Ili2Parser::IliunionContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

std::vector<Ili2Parser::RenamedViewableRefContext *> Ili2Parser::IliunionContext::renamedViewableRef() {
  return getRuleContexts<Ili2Parser::RenamedViewableRefContext>();
}

Ili2Parser::RenamedViewableRefContext* Ili2Parser::IliunionContext::renamedViewableRef(size_t i) {
  return getRuleContext<Ili2Parser::RenamedViewableRefContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::IliunionContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::IliunionContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::IliunionContext::getRuleIndex() const {
  return Ili2Parser::RuleIliunion;
}

antlrcpp::Any Ili2Parser::IliunionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitIliunion(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::IliunionContext* Ili2Parser::iliunion() {
  IliunionContext *_localctx = _tracker.createInstance<IliunionContext>(_ctx, getState());
  enterRule(_localctx, 210, Ili2Parser::RuleIliunion);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1655);
    match(Ili2Parser::UNION);
    setState(1656);
    match(Ili2Parser::OF);
    setState(1657);
    renamedViewableRef();
    setState(1662);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::COMMA) {
      setState(1658);
      match(Ili2Parser::COMMA);
      setState(1659);
      renamedViewableRef();
      setState(1664);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AggregationContext ------------------------------------------------------------------

Ili2Parser::AggregationContext::AggregationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::AggregationContext::AGGREGATION() {
  return getToken(Ili2Parser::AGGREGATION, 0);
}

tree::TerminalNode* Ili2Parser::AggregationContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

Ili2Parser::RenamedViewableRefContext* Ili2Parser::AggregationContext::renamedViewableRef() {
  return getRuleContext<Ili2Parser::RenamedViewableRefContext>(0);
}

tree::TerminalNode* Ili2Parser::AggregationContext::ALL() {
  return getToken(Ili2Parser::ALL, 0);
}

tree::TerminalNode* Ili2Parser::AggregationContext::EQUAL_CONST() {
  return getToken(Ili2Parser::EQUAL_CONST, 0);
}

tree::TerminalNode* Ili2Parser::AggregationContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

Ili2Parser::UniqueElContext* Ili2Parser::AggregationContext::uniqueEl() {
  return getRuleContext<Ili2Parser::UniqueElContext>(0);
}

tree::TerminalNode* Ili2Parser::AggregationContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}


size_t Ili2Parser::AggregationContext::getRuleIndex() const {
  return Ili2Parser::RuleAggregation;
}

antlrcpp::Any Ili2Parser::AggregationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitAggregation(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::AggregationContext* Ili2Parser::aggregation() {
  AggregationContext *_localctx = _tracker.createInstance<AggregationContext>(_ctx, getState());
  enterRule(_localctx, 212, Ili2Parser::RuleAggregation);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1665);
    match(Ili2Parser::AGGREGATION);
    setState(1666);
    match(Ili2Parser::OF);
    setState(1667);
    renamedViewableRef();
    setState(1674);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::ALL: {
        setState(1668);
        match(Ili2Parser::ALL);
        break;
      }

      case Ili2Parser::EQUAL_CONST: {
        setState(1669);
        match(Ili2Parser::EQUAL_CONST);
        setState(1670);
        match(Ili2Parser::LPAREN);
        setState(1671);
        uniqueEl();
        setState(1672);
        match(Ili2Parser::RPAREN);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InspectionContext ------------------------------------------------------------------

Ili2Parser::InspectionContext::InspectionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::InspectionContext::INSPECTION() {
  return getToken(Ili2Parser::INSPECTION, 0);
}

tree::TerminalNode* Ili2Parser::InspectionContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

Ili2Parser::RenamedViewableRefContext* Ili2Parser::InspectionContext::renamedViewableRef() {
  return getRuleContext<Ili2Parser::RenamedViewableRefContext>(0);
}

tree::TerminalNode* Ili2Parser::InspectionContext::AREA() {
  return getToken(Ili2Parser::AREA, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::InspectionContext::RARROW() {
  return getTokens(Ili2Parser::RARROW);
}

tree::TerminalNode* Ili2Parser::InspectionContext::RARROW(size_t i) {
  return getToken(Ili2Parser::RARROW, i);
}

std::vector<tree::TerminalNode *> Ili2Parser::InspectionContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::InspectionContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}


size_t Ili2Parser::InspectionContext::getRuleIndex() const {
  return Ili2Parser::RuleInspection;
}

antlrcpp::Any Ili2Parser::InspectionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitInspection(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::InspectionContext* Ili2Parser::inspection() {
  InspectionContext *_localctx = _tracker.createInstance<InspectionContext>(_ctx, getState());
  enterRule(_localctx, 214, Ili2Parser::RuleInspection);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(1677);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::AREA) {
      setState(1676);
      match(Ili2Parser::AREA);
    }
    setState(1679);
    match(Ili2Parser::INSPECTION);
    setState(1680);
    match(Ili2Parser::OF);
    setState(1681);
    renamedViewableRef();
    setState(1684); 
    _errHandler->sync(this);
    alt = 1;
    do {
      switch (alt) {
        case 1: {
              setState(1682);
              match(Ili2Parser::RARROW);
              setState(1683);
              dynamic_cast<InspectionContext *>(_localctx)->structureorlineattributename = match(Ili2Parser::NAME);
              break;
            }

      default:
        throw NoViableAltException(this);
      }
      setState(1686); 
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 218, _ctx);
    } while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- RenamedViewableRefContext ------------------------------------------------------------------

Ili2Parser::RenamedViewableRefContext::RenamedViewableRefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili2Parser::PathContext* Ili2Parser::RenamedViewableRefContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}

tree::TerminalNode* Ili2Parser::RenamedViewableRefContext::TILDE() {
  return getToken(Ili2Parser::TILDE, 0);
}

tree::TerminalNode* Ili2Parser::RenamedViewableRefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}


size_t Ili2Parser::RenamedViewableRefContext::getRuleIndex() const {
  return Ili2Parser::RuleRenamedViewableRef;
}

antlrcpp::Any Ili2Parser::RenamedViewableRefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitRenamedViewableRef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::RenamedViewableRefContext* Ili2Parser::renamedViewableRef() {
  RenamedViewableRefContext *_localctx = _tracker.createInstance<RenamedViewableRefContext>(_ctx, getState());
  enterRule(_localctx, 216, Ili2Parser::RuleRenamedViewableRef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1690);
    _errHandler->sync(this);

    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 219, _ctx)) {
    case 1: {
      setState(1688);
      dynamic_cast<RenamedViewableRefContext *>(_localctx)->basename = match(Ili2Parser::NAME);
      setState(1689);
      match(Ili2Parser::TILDE);
      break;
    }

    }
    setState(1692);
    path();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BaseExtensionDefContext ------------------------------------------------------------------

Ili2Parser::BaseExtensionDefContext::BaseExtensionDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::BaseExtensionDefContext::BASE() {
  return getToken(Ili2Parser::BASE, 0);
}

tree::TerminalNode* Ili2Parser::BaseExtensionDefContext::EXTENDED() {
  return getToken(Ili2Parser::EXTENDED, 0);
}

tree::TerminalNode* Ili2Parser::BaseExtensionDefContext::BY() {
  return getToken(Ili2Parser::BY, 0);
}

std::vector<Ili2Parser::RenamedViewableRefContext *> Ili2Parser::BaseExtensionDefContext::renamedViewableRef() {
  return getRuleContexts<Ili2Parser::RenamedViewableRefContext>();
}

Ili2Parser::RenamedViewableRefContext* Ili2Parser::BaseExtensionDefContext::renamedViewableRef(size_t i) {
  return getRuleContext<Ili2Parser::RenamedViewableRefContext>(i);
}

tree::TerminalNode* Ili2Parser::BaseExtensionDefContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::BaseExtensionDefContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::BaseExtensionDefContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::BaseExtensionDefContext::getRuleIndex() const {
  return Ili2Parser::RuleBaseExtensionDef;
}

antlrcpp::Any Ili2Parser::BaseExtensionDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitBaseExtensionDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::BaseExtensionDefContext* Ili2Parser::baseExtensionDef() {
  BaseExtensionDefContext *_localctx = _tracker.createInstance<BaseExtensionDefContext>(_ctx, getState());
  enterRule(_localctx, 218, Ili2Parser::RuleBaseExtensionDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1694);
    match(Ili2Parser::BASE);
    setState(1695);
    dynamic_cast<BaseExtensionDefContext *>(_localctx)->basename = match(Ili2Parser::NAME);
    setState(1696);
    match(Ili2Parser::EXTENDED);
    setState(1697);
    match(Ili2Parser::BY);
    setState(1698);
    renamedViewableRef();
    setState(1703);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::COMMA) {
      setState(1699);
      match(Ili2Parser::COMMA);
      setState(1700);
      renamedViewableRef();
      setState(1705);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SelectionContext ------------------------------------------------------------------

Ili2Parser::SelectionContext::SelectionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::SelectionContext::WHERE() {
  return getToken(Ili2Parser::WHERE, 0);
}

Ili2Parser::ExpressionContext* Ili2Parser::SelectionContext::expression() {
  return getRuleContext<Ili2Parser::ExpressionContext>(0);
}

tree::TerminalNode* Ili2Parser::SelectionContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}


size_t Ili2Parser::SelectionContext::getRuleIndex() const {
  return Ili2Parser::RuleSelection;
}

antlrcpp::Any Ili2Parser::SelectionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitSelection(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::SelectionContext* Ili2Parser::selection() {
  SelectionContext *_localctx = _tracker.createInstance<SelectionContext>(_ctx, getState());
  enterRule(_localctx, 220, Ili2Parser::RuleSelection);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1706);
    match(Ili2Parser::WHERE);
    setState(1707);
    expression();
    setState(1708);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ViewAttributeContext ------------------------------------------------------------------

Ili2Parser::ViewAttributeContext::ViewAttributeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::ViewAttributeContext::ALL() {
  return getToken(Ili2Parser::ALL, 0);
}

tree::TerminalNode* Ili2Parser::ViewAttributeContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

tree::TerminalNode* Ili2Parser::ViewAttributeContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

tree::TerminalNode* Ili2Parser::ViewAttributeContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

Ili2Parser::AttributeDefContext* Ili2Parser::ViewAttributeContext::attributeDef() {
  return getRuleContext<Ili2Parser::AttributeDefContext>(0);
}

tree::TerminalNode* Ili2Parser::ViewAttributeContext::COLONEQUAL() {
  return getToken(Ili2Parser::COLONEQUAL, 0);
}

Ili2Parser::FactorContext* Ili2Parser::ViewAttributeContext::factor() {
  return getRuleContext<Ili2Parser::FactorContext>(0);
}

Ili2Parser::PropertiesContext* Ili2Parser::ViewAttributeContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}


size_t Ili2Parser::ViewAttributeContext::getRuleIndex() const {
  return Ili2Parser::RuleViewAttribute;
}

antlrcpp::Any Ili2Parser::ViewAttributeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitViewAttribute(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::ViewAttributeContext* Ili2Parser::viewAttribute() {
  ViewAttributeContext *_localctx = _tracker.createInstance<ViewAttributeContext>(_ctx, getState());
  enterRule(_localctx, 222, Ili2Parser::RuleViewAttribute);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(1723);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 222, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(1710);
      match(Ili2Parser::ALL);
      setState(1711);
      match(Ili2Parser::OF);
      setState(1712);
      dynamic_cast<ViewAttributeContext *>(_localctx)->basename = match(Ili2Parser::NAME);
      setState(1713);
      match(Ili2Parser::SEMI);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(1714);
      attributeDef();
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(1715);
      dynamic_cast<ViewAttributeContext *>(_localctx)->attributename = match(Ili2Parser::NAME);
      setState(1717);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == Ili2Parser::LPAREN) {
        setState(1716);
        properties();
      }
      setState(1719);
      match(Ili2Parser::COLONEQUAL);
      setState(1720);
      factor();
      setState(1721);
      match(Ili2Parser::SEMI);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- GraphicDefContext ------------------------------------------------------------------

Ili2Parser::GraphicDefContext::GraphicDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::GraphicDefContext::GRAPHIC() {
  return getToken(Ili2Parser::GRAPHIC, 0);
}

tree::TerminalNode* Ili2Parser::GraphicDefContext::EQUAL() {
  return getToken(Ili2Parser::EQUAL, 0);
}

tree::TerminalNode* Ili2Parser::GraphicDefContext::END() {
  return getToken(Ili2Parser::END, 0);
}

tree::TerminalNode* Ili2Parser::GraphicDefContext::SEMI() {
  return getToken(Ili2Parser::SEMI, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::GraphicDefContext::NAME() {
  return getTokens(Ili2Parser::NAME);
}

tree::TerminalNode* Ili2Parser::GraphicDefContext::NAME(size_t i) {
  return getToken(Ili2Parser::NAME, i);
}

Ili2Parser::PropertiesContext* Ili2Parser::GraphicDefContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

tree::TerminalNode* Ili2Parser::GraphicDefContext::EXTENDS() {
  return getToken(Ili2Parser::EXTENDS, 0);
}

tree::TerminalNode* Ili2Parser::GraphicDefContext::BASED() {
  return getToken(Ili2Parser::BASED, 0);
}

tree::TerminalNode* Ili2Parser::GraphicDefContext::ON() {
  return getToken(Ili2Parser::ON, 0);
}

std::vector<Ili2Parser::SelectionContext *> Ili2Parser::GraphicDefContext::selection() {
  return getRuleContexts<Ili2Parser::SelectionContext>();
}

Ili2Parser::SelectionContext* Ili2Parser::GraphicDefContext::selection(size_t i) {
  return getRuleContext<Ili2Parser::SelectionContext>(i);
}

std::vector<Ili2Parser::DrawingRuleContext *> Ili2Parser::GraphicDefContext::drawingRule() {
  return getRuleContexts<Ili2Parser::DrawingRuleContext>();
}

Ili2Parser::DrawingRuleContext* Ili2Parser::GraphicDefContext::drawingRule(size_t i) {
  return getRuleContext<Ili2Parser::DrawingRuleContext>(i);
}

std::vector<Ili2Parser::PathContext *> Ili2Parser::GraphicDefContext::path() {
  return getRuleContexts<Ili2Parser::PathContext>();
}

Ili2Parser::PathContext* Ili2Parser::GraphicDefContext::path(size_t i) {
  return getRuleContext<Ili2Parser::PathContext>(i);
}


size_t Ili2Parser::GraphicDefContext::getRuleIndex() const {
  return Ili2Parser::RuleGraphicDef;
}

antlrcpp::Any Ili2Parser::GraphicDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitGraphicDef(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::GraphicDefContext* Ili2Parser::graphicDef() {
  GraphicDefContext *_localctx = _tracker.createInstance<GraphicDefContext>(_ctx, getState());
  enterRule(_localctx, 224, Ili2Parser::RuleGraphicDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1725);
    match(Ili2Parser::GRAPHIC);
    setState(1726);
    dynamic_cast<GraphicDefContext *>(_localctx)->graphicname1 = match(Ili2Parser::NAME);
    setState(1728);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(1727);
      properties();
    }
    setState(1732);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::EXTENDS) {
      setState(1730);
      match(Ili2Parser::EXTENDS);
      setState(1731);
      dynamic_cast<GraphicDefContext *>(_localctx)->expath = path();
    }
    setState(1737);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::BASED) {
      setState(1734);
      match(Ili2Parser::BASED);
      setState(1735);
      match(Ili2Parser::ON);
      setState(1736);
      dynamic_cast<GraphicDefContext *>(_localctx)->bpath = path();
    }
    setState(1739);
    match(Ili2Parser::EQUAL);
    setState(1743);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::WHERE) {
      setState(1740);
      selection();
      setState(1745);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1749);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::NAME) {
      setState(1746);
      drawingRule();
      setState(1751);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1752);
    match(Ili2Parser::END);
    setState(1753);
    dynamic_cast<GraphicDefContext *>(_localctx)->graphicname2 = match(Ili2Parser::NAME);
    setState(1754);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DrawingRuleContext ------------------------------------------------------------------

Ili2Parser::DrawingRuleContext::DrawingRuleContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::DrawingRuleContext::COLON() {
  return getToken(Ili2Parser::COLON, 0);
}

std::vector<Ili2Parser::CondSignParamAssignmentContext *> Ili2Parser::DrawingRuleContext::condSignParamAssignment() {
  return getRuleContexts<Ili2Parser::CondSignParamAssignmentContext>();
}

Ili2Parser::CondSignParamAssignmentContext* Ili2Parser::DrawingRuleContext::condSignParamAssignment(size_t i) {
  return getRuleContext<Ili2Parser::CondSignParamAssignmentContext>(i);
}

std::vector<tree::TerminalNode *> Ili2Parser::DrawingRuleContext::SEMI() {
  return getTokens(Ili2Parser::SEMI);
}

tree::TerminalNode* Ili2Parser::DrawingRuleContext::SEMI(size_t i) {
  return getToken(Ili2Parser::SEMI, i);
}

tree::TerminalNode* Ili2Parser::DrawingRuleContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

Ili2Parser::PropertiesContext* Ili2Parser::DrawingRuleContext::properties() {
  return getRuleContext<Ili2Parser::PropertiesContext>(0);
}

tree::TerminalNode* Ili2Parser::DrawingRuleContext::OF() {
  return getToken(Ili2Parser::OF, 0);
}

Ili2Parser::PathContext* Ili2Parser::DrawingRuleContext::path() {
  return getRuleContext<Ili2Parser::PathContext>(0);
}


size_t Ili2Parser::DrawingRuleContext::getRuleIndex() const {
  return Ili2Parser::RuleDrawingRule;
}

antlrcpp::Any Ili2Parser::DrawingRuleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitDrawingRule(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::DrawingRuleContext* Ili2Parser::drawingRule() {
  DrawingRuleContext *_localctx = _tracker.createInstance<DrawingRuleContext>(_ctx, getState());
  enterRule(_localctx, 226, Ili2Parser::RuleDrawingRule);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(1756);
    dynamic_cast<DrawingRuleContext *>(_localctx)->drawingrulename = match(Ili2Parser::NAME);
    setState(1758);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::LPAREN) {
      setState(1757);
      properties();
    }
    setState(1762);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::OF) {
      setState(1760);
      match(Ili2Parser::OF);
      setState(1761);
      path();
    }
    setState(1764);
    match(Ili2Parser::COLON);
    setState(1765);
    condSignParamAssignment();
    setState(1770);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 230, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(1766);
        match(Ili2Parser::SEMI);
        setState(1767);
        condSignParamAssignment(); 
      }
      setState(1772);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 230, _ctx);
    }
    setState(1773);
    match(Ili2Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CondSignParamAssignmentContext ------------------------------------------------------------------

Ili2Parser::CondSignParamAssignmentContext::CondSignParamAssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::CondSignParamAssignmentContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

std::vector<Ili2Parser::SignParamAssignmentContext *> Ili2Parser::CondSignParamAssignmentContext::signParamAssignment() {
  return getRuleContexts<Ili2Parser::SignParamAssignmentContext>();
}

Ili2Parser::SignParamAssignmentContext* Ili2Parser::CondSignParamAssignmentContext::signParamAssignment(size_t i) {
  return getRuleContext<Ili2Parser::SignParamAssignmentContext>(i);
}

tree::TerminalNode* Ili2Parser::CondSignParamAssignmentContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

tree::TerminalNode* Ili2Parser::CondSignParamAssignmentContext::WHERE() {
  return getToken(Ili2Parser::WHERE, 0);
}

std::vector<tree::TerminalNode *> Ili2Parser::CondSignParamAssignmentContext::SEMI() {
  return getTokens(Ili2Parser::SEMI);
}

tree::TerminalNode* Ili2Parser::CondSignParamAssignmentContext::SEMI(size_t i) {
  return getToken(Ili2Parser::SEMI, i);
}

Ili2Parser::ExpressionContext* Ili2Parser::CondSignParamAssignmentContext::expression() {
  return getRuleContext<Ili2Parser::ExpressionContext>(0);
}


size_t Ili2Parser::CondSignParamAssignmentContext::getRuleIndex() const {
  return Ili2Parser::RuleCondSignParamAssignment;
}

antlrcpp::Any Ili2Parser::CondSignParamAssignmentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitCondSignParamAssignment(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::CondSignParamAssignmentContext* Ili2Parser::condSignParamAssignment() {
  CondSignParamAssignmentContext *_localctx = _tracker.createInstance<CondSignParamAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 228, Ili2Parser::RuleCondSignParamAssignment);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1777);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::WHERE) {
      setState(1775);
      match(Ili2Parser::WHERE);
      setState(1776);
      dynamic_cast<CondSignParamAssignmentContext *>(_localctx)->logical = expression();
    }
    setState(1779);
    match(Ili2Parser::LPAREN);
    setState(1780);
    signParamAssignment();
    setState(1785);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili2Parser::SEMI) {
      setState(1781);
      match(Ili2Parser::SEMI);
      setState(1782);
      signParamAssignment();
      setState(1787);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(1788);
    match(Ili2Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SignParamAssignmentContext ------------------------------------------------------------------

Ili2Parser::SignParamAssignmentContext::SignParamAssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::SignParamAssignmentContext::COLONEQUAL() {
  return getToken(Ili2Parser::COLONEQUAL, 0);
}

tree::TerminalNode* Ili2Parser::SignParamAssignmentContext::NAME() {
  return getToken(Ili2Parser::NAME, 0);
}

tree::TerminalNode* Ili2Parser::SignParamAssignmentContext::LCURLY() {
  return getToken(Ili2Parser::LCURLY, 0);
}

Ili2Parser::MetaObjectRefContext* Ili2Parser::SignParamAssignmentContext::metaObjectRef() {
  return getRuleContext<Ili2Parser::MetaObjectRefContext>(0);
}

tree::TerminalNode* Ili2Parser::SignParamAssignmentContext::RCURLY() {
  return getToken(Ili2Parser::RCURLY, 0);
}

Ili2Parser::FactorContext* Ili2Parser::SignParamAssignmentContext::factor() {
  return getRuleContext<Ili2Parser::FactorContext>(0);
}

tree::TerminalNode* Ili2Parser::SignParamAssignmentContext::ACCORDING() {
  return getToken(Ili2Parser::ACCORDING, 0);
}

tree::TerminalNode* Ili2Parser::SignParamAssignmentContext::LPAREN() {
  return getToken(Ili2Parser::LPAREN, 0);
}

std::vector<Ili2Parser::EnumAssignmentContext *> Ili2Parser::SignParamAssignmentContext::enumAssignment() {
  return getRuleContexts<Ili2Parser::EnumAssignmentContext>();
}

Ili2Parser::EnumAssignmentContext* Ili2Parser::SignParamAssignmentContext::enumAssignment(size_t i) {
  return getRuleContext<Ili2Parser::EnumAssignmentContext>(i);
}

tree::TerminalNode* Ili2Parser::SignParamAssignmentContext::RPAREN() {
  return getToken(Ili2Parser::RPAREN, 0);
}

Ili2Parser::AttributePathContext* Ili2Parser::SignParamAssignmentContext::attributePath() {
  return getRuleContext<Ili2Parser::AttributePathContext>(0);
}

std::vector<tree::TerminalNode *> Ili2Parser::SignParamAssignmentContext::COMMA() {
  return getTokens(Ili2Parser::COMMA);
}

tree::TerminalNode* Ili2Parser::SignParamAssignmentContext::COMMA(size_t i) {
  return getToken(Ili2Parser::COMMA, i);
}


size_t Ili2Parser::SignParamAssignmentContext::getRuleIndex() const {
  return Ili2Parser::RuleSignParamAssignment;
}

antlrcpp::Any Ili2Parser::SignParamAssignmentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitSignParamAssignment(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::SignParamAssignmentContext* Ili2Parser::signParamAssignment() {
  SignParamAssignmentContext *_localctx = _tracker.createInstance<SignParamAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 230, Ili2Parser::RuleSignParamAssignment);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1790);
    dynamic_cast<SignParamAssignmentContext *>(_localctx)->signparametername = match(Ili2Parser::NAME);
    setState(1791);
    match(Ili2Parser::COLONEQUAL);
    setState(1810);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::LCURLY: {
        setState(1792);
        match(Ili2Parser::LCURLY);
        setState(1793);
        metaObjectRef();
        setState(1794);
        match(Ili2Parser::RCURLY);
        break;
      }

      case Ili2Parser::INTERLIS:
      case Ili2Parser::REFSYSTEM:
      case Ili2Parser::BOOLEAN:
      case Ili2Parser::HALIGNMENT:
      case Ili2Parser::VALIGNMENT:
      case Ili2Parser::BACKSLASH:
      case Ili2Parser::HASH:
      case Ili2Parser::GREATERGREATER:
      case Ili2Parser::GREATER:
      case Ili2Parser::PARAMETER:
      case Ili2Parser::UNDEFINED:
      case Ili2Parser::URI:
      case Ili2Parser::PI:
      case Ili2Parser::LNBASE:
      case Ili2Parser::AREA:
      case Ili2Parser::SIGN:
      case Ili2Parser::METAOBJECT:
      case Ili2Parser::INSPECTION:
      case Ili2Parser::THIS:
      case Ili2Parser::THISAREA:
      case Ili2Parser::THATAREA:
      case Ili2Parser::PARENT:
      case Ili2Parser::AGGREGATES:
      case Ili2Parser::STRING:
      case Ili2Parser::POSNUMBER:
      case Ili2Parser::NUMBER:
      case Ili2Parser::DEC:
      case Ili2Parser::NAME: {
        setState(1796);
        factor();
        break;
      }

      case Ili2Parser::ACCORDING: {
        setState(1797);
        match(Ili2Parser::ACCORDING);
        setState(1798);
        dynamic_cast<SignParamAssignmentContext *>(_localctx)->enumpath = attributePath();
        setState(1799);
        match(Ili2Parser::LPAREN);
        setState(1800);
        enumAssignment();
        setState(1805);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == Ili2Parser::COMMA) {
          setState(1801);
          match(Ili2Parser::COMMA);
          setState(1802);
          enumAssignment();
          setState(1807);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        setState(1808);
        match(Ili2Parser::RPAREN);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumAssignmentContext ------------------------------------------------------------------

Ili2Parser::EnumAssignmentContext::EnumAssignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili2Parser::EnumAssignmentContext::WHEN() {
  return getToken(Ili2Parser::WHEN, 0);
}

tree::TerminalNode* Ili2Parser::EnumAssignmentContext::IN() {
  return getToken(Ili2Parser::IN, 0);
}

Ili2Parser::EnumRangeContext* Ili2Parser::EnumAssignmentContext::enumRange() {
  return getRuleContext<Ili2Parser::EnumRangeContext>(0);
}

tree::TerminalNode* Ili2Parser::EnumAssignmentContext::LCURLY() {
  return getToken(Ili2Parser::LCURLY, 0);
}

Ili2Parser::MetaObjectRefContext* Ili2Parser::EnumAssignmentContext::metaObjectRef() {
  return getRuleContext<Ili2Parser::MetaObjectRefContext>(0);
}

tree::TerminalNode* Ili2Parser::EnumAssignmentContext::RCURLY() {
  return getToken(Ili2Parser::RCURLY, 0);
}

Ili2Parser::ConstantContext* Ili2Parser::EnumAssignmentContext::constant() {
  return getRuleContext<Ili2Parser::ConstantContext>(0);
}


size_t Ili2Parser::EnumAssignmentContext::getRuleIndex() const {
  return Ili2Parser::RuleEnumAssignment;
}

antlrcpp::Any Ili2Parser::EnumAssignmentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitEnumAssignment(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::EnumAssignmentContext* Ili2Parser::enumAssignment() {
  EnumAssignmentContext *_localctx = _tracker.createInstance<EnumAssignmentContext>(_ctx, getState());
  enterRule(_localctx, 232, Ili2Parser::RuleEnumAssignment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1817);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili2Parser::LCURLY: {
        setState(1812);
        match(Ili2Parser::LCURLY);
        setState(1813);
        metaObjectRef();
        setState(1814);
        match(Ili2Parser::RCURLY);
        break;
      }

      case Ili2Parser::HASH:
      case Ili2Parser::GREATERGREATER:
      case Ili2Parser::GREATER:
      case Ili2Parser::UNDEFINED:
      case Ili2Parser::PI:
      case Ili2Parser::LNBASE:
      case Ili2Parser::STRING:
      case Ili2Parser::POSNUMBER:
      case Ili2Parser::NUMBER:
      case Ili2Parser::DEC: {
        setState(1816);
        constant();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(1819);
    match(Ili2Parser::WHEN);
    setState(1820);
    match(Ili2Parser::IN);
    setState(1821);
    enumRange();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumRangeContext ------------------------------------------------------------------

Ili2Parser::EnumRangeContext::EnumRangeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<Ili2Parser::EnumConstContext *> Ili2Parser::EnumRangeContext::enumConst() {
  return getRuleContexts<Ili2Parser::EnumConstContext>();
}

Ili2Parser::EnumConstContext* Ili2Parser::EnumRangeContext::enumConst(size_t i) {
  return getRuleContext<Ili2Parser::EnumConstContext>(i);
}

tree::TerminalNode* Ili2Parser::EnumRangeContext::DOTDOT() {
  return getToken(Ili2Parser::DOTDOT, 0);
}


size_t Ili2Parser::EnumRangeContext::getRuleIndex() const {
  return Ili2Parser::RuleEnumRange;
}

antlrcpp::Any Ili2Parser::EnumRangeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili2ParserVisitor*>(visitor))
    return parserVisitor->visitEnumRange(this);
  else
    return visitor->visitChildren(this);
}

Ili2Parser::EnumRangeContext* Ili2Parser::enumRange() {
  EnumRangeContext *_localctx = _tracker.createInstance<EnumRangeContext>(_ctx, getState());
  enterRule(_localctx, 234, Ili2Parser::RuleEnumRange);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(1823);
    enumConst();
    setState(1826);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili2Parser::DOTDOT) {
      setState(1824);
      match(Ili2Parser::DOTDOT);
      setState(1825);
      enumConst();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool Ili2Parser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 3: return modelDefSempred(dynamic_cast<ModelDefContext *>(context), predicateIndex);
    case 6: return topicDefSempred(dynamic_cast<TopicDefContext *>(context), predicateIndex);
    case 13: return bagOrListTypeSempred(dynamic_cast<BagOrListTypeContext *>(context), predicateIndex);
    case 23: return domainTypeSempred(dynamic_cast<DomainTypeContext *>(context), predicateIndex);
    case 25: return baseTypeSempred(dynamic_cast<BaseTypeContext *>(context), predicateIndex);
    case 47: return coordinateTypeSempred(dynamic_cast<CoordinateTypeContext *>(context), predicateIndex);
    case 55: return lineTypeSempred(dynamic_cast<LineTypeContext *>(context), predicateIndex);
    case 64: return composedUnitExprSempred(dynamic_cast<ComposedUnitExprContext *>(context), predicateIndex);
    case 73: return mandatoryConstraintSempred(dynamic_cast<MandatoryConstraintContext *>(context), predicateIndex);
    case 74: return plausibilityConstraintSempred(dynamic_cast<PlausibilityConstraintContext *>(context), predicateIndex);
    case 75: return existenceConstraintSempred(dynamic_cast<ExistenceConstraintContext *>(context), predicateIndex);
    case 76: return uniquenessConstraintSempred(dynamic_cast<UniquenessConstraintContext *>(context), predicateIndex);
    case 81: return setConstraintSempred(dynamic_cast<SetConstraintContext *>(context), predicateIndex);
    case 85: return operator1Sempred(dynamic_cast<Operator1Context *>(context), predicateIndex);
    case 87: return operator2Sempred(dynamic_cast<Operator2Context *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool Ili2Parser::modelDefSempred(ModelDefContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return ili24;
    case 1: return ili24;
    case 2: return ili24;
    case 3: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::topicDefSempred(TopicDefContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 4: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::bagOrListTypeSempred(BagOrListTypeContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 5: return ili23;
    case 6: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::domainTypeSempred(DomainTypeContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 7: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::baseTypeSempred(BaseTypeContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 8: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::coordinateTypeSempred(CoordinateTypeContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 9: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::lineTypeSempred(LineTypeContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 10: return ili24;
    case 11: return ili24;
    case 12: return ili23;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::composedUnitExprSempred(ComposedUnitExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 13: return precpred(_ctx, 2);
    case 14: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool Ili2Parser::mandatoryConstraintSempred(MandatoryConstraintContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 15: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::plausibilityConstraintSempred(PlausibilityConstraintContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 16: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::existenceConstraintSempred(ExistenceConstraintContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 17: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::uniquenessConstraintSempred(UniquenessConstraintContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 18: return ili24;
    case 19: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::setConstraintSempred(SetConstraintContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 20: return ili24;
    case 21: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::operator1Sempred(Operator1Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 22: return ili24;
    case 23: return ili24;
    case 24: return ili24;

  default:
    break;
  }
  return true;
}

bool Ili2Parser::operator2Sempred(Operator2Context *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 25: return ili24;
    case 26: return ili24;

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> Ili2Parser::_decisionToDFA;
atn::PredictionContextCache Ili2Parser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN Ili2Parser::_atn;
std::vector<uint16_t> Ili2Parser::_serializedATN;

std::vector<std::string> Ili2Parser::_ruleNames = {
  "decimal", "path", "interlis2Def", "modelDef", "importDef", "importing", 
  "topicDef", "topicPath", "classDef", "structureDef", "classOrStructureDef", 
  "attributeDef", "attrTypeDef", "bagOrListType", "attrType", "referenceAttr", 
  "restrictedRef", "restriction", "associationDef", "associationRef", "roleDef", 
  "cardinality", "domainDef", "domainType", "type", "baseType", "constant", 
  "textType", "textConst", "enumerationType", "enumTreeValueType", "alignmentType", 
  "booleanType", "enumeration", "enumElement", "enumConst", "numericType", 
  "dateTimeType", "refSys", "decConst", "numericConst", "formattedType", 
  "formatDef", "baseAttrRef", "formattedConst", "contextDef", "contextDecl", 
  "coordinateType", "rotationDef", "oIDType", "blackboxType", "classRefType", 
  "attributePathType", "classConst", "attributePathConst", "lineType", "lineForm", 
  "lineFormType", "lineFormTypeDef", "lineFormTypeDecl", "unitDecl", "unitDef", 
  "derivedUnit", "composedUnit", "composedUnitExpr", "metaDataBasketDef", 
  "metaDataBasketRef", "metaObjectRef", "parameterDef", "properties", "runTimeParameterDef", 
  "runTimeParameter", "constraintDef", "mandatoryConstraint", "plausibilityConstraint", 
  "existenceConstraint", "uniquenessConstraint", "globalUniqueness", "uniqueEl", 
  "localUniqueness", "localUniqueEl", "setConstraint", "constraintsDef", 
  "expression", "term1", "operator1", "term2", "operator2", "term3", "term", 
  "relation", "factor", "objectOrAttributePath", "attributePath", "pathEl", 
  "objectRef", "functionCall", "functionCallArgument", "functionDef", "functionDefParam", 
  "argumentType", "viewDef", "formationDef", "projection", "join", "iliunion", 
  "aggregation", "inspection", "renamedViewableRef", "baseExtensionDef", 
  "selection", "viewAttribute", "graphicDef", "drawingRule", "condSignParamAssignment", 
  "signParamAssignment", "enumAssignment", "enumRange"
};

std::vector<std::string> Ili2Parser::_literalNames = {
  "", "", "", "", "", "", "", "'INTERLIS'", "'2.3'", "'2.4'", "'CONTRACTED'", 
  "'REFSYSTEM'", "'SYMBOLOGY'", "'TYPE'", "'MODEL'", "'END'", "'VERSION'", 
  "'NOINCREMENTALTRANSFER'", "'TRANSLATION'", "'OF'", "'AT'", "'CHARSET'", 
  "'XMLNS'", "'IMPORTS'", "'UNQUALIFIED'", "'TOPIC'", "'DEPENDS'", "'ON'", 
  "'AS'", "'VIEW'", "'EXTENDS'", "'BASKET'", "'OID'", "'BOOLEAN'", "'HALIGNMENT'", 
  "'VALIGNMENT'", "'CLASS'", "'ABSTRACT'", "'EXTENDED'", "'FINAL'", "'TRANSIENT'", 
  "'CONTINUOUS'", "'SUBDIVISION'", "'NO'", "'ATTRIBUTE'", "'TEXT'", "'DATE'", 
  "'TIMEOFDAY'", "'DATETIME'", "'('", "')'", "'['", "']'", "'{'", "'}'", 
  "'*'", "'/'", "'\\'", "'%'", "'@'", "'#'", "'~'", "'<'", "'<='", "'=>'", 
  "'>>'", "'>'", "'>='", "';'", "'='", "'EQUAL'", "'=='", "'<>'", "'!='", 
  "':='", "'.'", "'..'", "':'", "','", "'<-'", "'->'", "'+'", "'-'", "'-<>'", 
  "'--'", "'-<#>'", "'STRUCTURE'", "'PARAMETER'", "'MANDATORY'", "'GENERIC'", 
  "'GENERICS'", "'DEFERRED'", "'CONTEXT'", "'MULTICOORD'", "'MULTIPOLYLINE'", 
  "'MULTISURFACE'", "'MULTIAREA'", "'BAG'", "'LIST'", "'REFERENCE'", "'TO'", 
  "'ANYCLASS'", "'RESTRICTION'", "'ASSOCIATION'", "'DERIVED'", "'EXTERNAL'", 
  "'FROM'", "'UNDEFINED'", "'MTEXT'", "'NAME'", "'URI'", "'ALL'", "'CIRCULAR'", 
  "'OTHERS'", "'NUMERIC'", "'CLOCKWISE'", "'COUNTERCLOCKWISE'", "'CARDINALITY'", 
  "'OR'", "'HIDING'", "'ORDERED'", "'DOMAIN'", "'PI'", "'LNBASE'", "'FORMAT'", 
  "'INHERIT'", "'COORD'", "'ROTATION'", "'ANY'", "'BLACKBOX'", "'XML'", 
  "'BINARY'", "'DIRECTED'", "'POLYLINE'", "'SURFACE'", "'AREA'", "'WITH'", 
  "'STRAIGHTS'", "'ARCS'", "'VERTEX'", "'WITHOUT'", "'OVERLAPS'", "'LINE'", 
  "'ATTRIBUTES'", "'FORM'", "'UNIT'", "'FUNCTION'", "'SIGN'", "'OBJECTS'", 
  "'METAOBJECT'", "'CONSTRAINT'", "'CONSTRAINTS'", "'EXISTENCE'", "'REQUIRED'", 
  "'IN'", "'UNIQUE'", "'WHERE'", "'LOCAL'", "'SET'", "'AND'", "'NOT'", "'BASED'", 
  "'BASE'", "'INHERITANCE'", "'DEFINED'", "'INSPECTION'", "'THIS'", "'THISAREA'", 
  "'THATAREA'", "'PARENT'", "'FIRST'", "'LAST'", "'AGGREGATES'", "'OBJECT'", 
  "'ENUMVAL'", "'ENUMTREEVAL'", "'PROJECTION'", "'JOIN'", "'NULL'", "'UNION'", 
  "'AGGREGATION'", "'BY'", "'GRAPHIC'", "'ACCORDING'", "'WHEN'", "'ANYSTRUCTURE'", 
  "'TRANSFER'"
};

std::vector<std::string> Ili2Parser::_symbolicNames = {
  "", "SL_COMMENT", "ILI_OPTION", "ILI_BLOCKCOMMENT", "WS", "NL", "ILI_DOC", 
  "INTERLIS", "ILI23", "ILI24", "CONTRACTED", "REFSYSTEM", "SYMBOLOGY", 
  "TYPE", "MODEL", "END", "VERSION", "NOINCREMENTALTRANSFER", "TRANSLATION", 
  "OF", "ATT", "CHARSET", "XMLNS", "IMPORTS", "UNQUALIFIED", "TOPIC", "DEPENDS", 
  "ON", "AS", "VIEW", "EXTENDS", "BASKET", "OID", "BOOLEAN", "HALIGNMENT", 
  "VALIGNMENT", "CLASS", "ABSTRACT", "EXTENDED", "FINAL", "TRANSIENT", "CONTINUOUS", 
  "SUBDIVISION", "NO", "ATTRIBUTE", "TEXT", "DATE", "TIMEOFDAY", "DATETIME", 
  "LPAREN", "RPAREN", "LBRACE", "RBRACE", "LCURLY", "RCURLY", "STAR", "SLASH", 
  "BACKSLASH", "PERCENT", "AT", "HASH", "TILDE", "LESS", "LESSEQUAL", "IMPL", 
  "GREATERGREATER", "GREATER", "GREATEREQUAL", "SEMI", "EQUAL", "EQUAL_CONST", 
  "EQUALEQUAL", "LESSGREATER", "NOTEQUAL", "COLONEQUAL", "DOT", "DOTDOT", 
  "COLON", "COMMA", "LARROW", "RARROW", "PLUS", "MINUS", "AGGREGATE", "ASSOCIATE", 
  "COMPOSITE", "STRUCTURE", "PARAMETER", "MANDATORY", "GENERIC", "GENERICS", 
  "DEFERRED", "CONTEXT", "MULTICOORD", "MULTIPOLYLINE", "MULTISURFACE", 
  "MULTIAREA", "BAG", "LIST", "REFERENCE", "TO", "ANYCLASS", "RESTRICTION", 
  "ASSOCIATION", "DERIVED", "EXTERNAL", "FROM", "UNDEFINED", "MTEXT", "NAME_CONST", 
  "URI", "ALL", "CIRCULAR", "OTHERS", "NUMERIC", "CLOCKWISE", "COUNTERCLOCKWISE", 
  "CARDINALITY", "OR", "HIDING", "ORDERED", "ILIDOMAIN", "PI", "LNBASE", 
  "FORMAT", "INHERIT", "COORD", "ROTATION", "ANY", "BLACKBOX", "XML", "BINARY", 
  "DIRECTED", "POLYLINE", "SURFACE", "AREA", "WITH", "STRAIGHTS", "ARCS", 
  "VERTEX", "WITHOUT", "OVERLAPS", "LINE", "ATTRIBUTES", "FORM", "UNIT", 
  "FUNCTION", "SIGN", "OBJECTS", "METAOBJECT", "CONSTRAINT", "CONSTRAINTS", 
  "EXISTENCE", "REQUIRED", "IN", "UNIQUE", "WHERE", "LOCAL", "SET", "AND", 
  "NOT", "BASED", "BASE", "INHERITANCE", "DEFINED", "INSPECTION", "THIS", 
  "THISAREA", "THATAREA", "PARENT", "FIRST", "LAST", "AGGREGATES", "OBJECT", 
  "ENUMVAL", "ENUMTREEVAL", "PROJECTION", "JOIN", "ILINULL", "UNION", "AGGREGATION", 
  "BY", "GRAPHIC", "ACCORDING", "WHEN", "ANYSTRUCTURE", "TRANSFER", "EXPLANATION", 
  "ESC", "STRING", "POSNUMBER", "NUMBER", "DEC", "DECIMAL", "HEXNUMBER", 
  "NAME"
};

dfa::Vocabulary Ili2Parser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> Ili2Parser::_tokenNames;

Ili2Parser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0xc5, 0x727, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
    0x9, 0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 
    0x4, 0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 
    0x9, 0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 0xe, 0x9, 0xe, 
    0x4, 0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 0x9, 0x11, 0x4, 
    0x12, 0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 0x9, 0x14, 0x4, 0x15, 
    0x9, 0x15, 0x4, 0x16, 0x9, 0x16, 0x4, 0x17, 0x9, 0x17, 0x4, 0x18, 0x9, 
    0x18, 0x4, 0x19, 0x9, 0x19, 0x4, 0x1a, 0x9, 0x1a, 0x4, 0x1b, 0x9, 0x1b, 
    0x4, 0x1c, 0x9, 0x1c, 0x4, 0x1d, 0x9, 0x1d, 0x4, 0x1e, 0x9, 0x1e, 0x4, 
    0x1f, 0x9, 0x1f, 0x4, 0x20, 0x9, 0x20, 0x4, 0x21, 0x9, 0x21, 0x4, 0x22, 
    0x9, 0x22, 0x4, 0x23, 0x9, 0x23, 0x4, 0x24, 0x9, 0x24, 0x4, 0x25, 0x9, 
    0x25, 0x4, 0x26, 0x9, 0x26, 0x4, 0x27, 0x9, 0x27, 0x4, 0x28, 0x9, 0x28, 
    0x4, 0x29, 0x9, 0x29, 0x4, 0x2a, 0x9, 0x2a, 0x4, 0x2b, 0x9, 0x2b, 0x4, 
    0x2c, 0x9, 0x2c, 0x4, 0x2d, 0x9, 0x2d, 0x4, 0x2e, 0x9, 0x2e, 0x4, 0x2f, 
    0x9, 0x2f, 0x4, 0x30, 0x9, 0x30, 0x4, 0x31, 0x9, 0x31, 0x4, 0x32, 0x9, 
    0x32, 0x4, 0x33, 0x9, 0x33, 0x4, 0x34, 0x9, 0x34, 0x4, 0x35, 0x9, 0x35, 
    0x4, 0x36, 0x9, 0x36, 0x4, 0x37, 0x9, 0x37, 0x4, 0x38, 0x9, 0x38, 0x4, 
    0x39, 0x9, 0x39, 0x4, 0x3a, 0x9, 0x3a, 0x4, 0x3b, 0x9, 0x3b, 0x4, 0x3c, 
    0x9, 0x3c, 0x4, 0x3d, 0x9, 0x3d, 0x4, 0x3e, 0x9, 0x3e, 0x4, 0x3f, 0x9, 
    0x3f, 0x4, 0x40, 0x9, 0x40, 0x4, 0x41, 0x9, 0x41, 0x4, 0x42, 0x9, 0x42, 
    0x4, 0x43, 0x9, 0x43, 0x4, 0x44, 0x9, 0x44, 0x4, 0x45, 0x9, 0x45, 0x4, 
    0x46, 0x9, 0x46, 0x4, 0x47, 0x9, 0x47, 0x4, 0x48, 0x9, 0x48, 0x4, 0x49, 
    0x9, 0x49, 0x4, 0x4a, 0x9, 0x4a, 0x4, 0x4b, 0x9, 0x4b, 0x4, 0x4c, 0x9, 
    0x4c, 0x4, 0x4d, 0x9, 0x4d, 0x4, 0x4e, 0x9, 0x4e, 0x4, 0x4f, 0x9, 0x4f, 
    0x4, 0x50, 0x9, 0x50, 0x4, 0x51, 0x9, 0x51, 0x4, 0x52, 0x9, 0x52, 0x4, 
    0x53, 0x9, 0x53, 0x4, 0x54, 0x9, 0x54, 0x4, 0x55, 0x9, 0x55, 0x4, 0x56, 
    0x9, 0x56, 0x4, 0x57, 0x9, 0x57, 0x4, 0x58, 0x9, 0x58, 0x4, 0x59, 0x9, 
    0x59, 0x4, 0x5a, 0x9, 0x5a, 0x4, 0x5b, 0x9, 0x5b, 0x4, 0x5c, 0x9, 0x5c, 
    0x4, 0x5d, 0x9, 0x5d, 0x4, 0x5e, 0x9, 0x5e, 0x4, 0x5f, 0x9, 0x5f, 0x4, 
    0x60, 0x9, 0x60, 0x4, 0x61, 0x9, 0x61, 0x4, 0x62, 0x9, 0x62, 0x4, 0x63, 
    0x9, 0x63, 0x4, 0x64, 0x9, 0x64, 0x4, 0x65, 0x9, 0x65, 0x4, 0x66, 0x9, 
    0x66, 0x4, 0x67, 0x9, 0x67, 0x4, 0x68, 0x9, 0x68, 0x4, 0x69, 0x9, 0x69, 
    0x4, 0x6a, 0x9, 0x6a, 0x4, 0x6b, 0x9, 0x6b, 0x4, 0x6c, 0x9, 0x6c, 0x4, 
    0x6d, 0x9, 0x6d, 0x4, 0x6e, 0x9, 0x6e, 0x4, 0x6f, 0x9, 0x6f, 0x4, 0x70, 
    0x9, 0x70, 0x4, 0x71, 0x9, 0x71, 0x4, 0x72, 0x9, 0x72, 0x4, 0x73, 0x9, 
    0x73, 0x4, 0x74, 0x9, 0x74, 0x4, 0x75, 0x9, 0x75, 0x4, 0x76, 0x9, 0x76, 
    0x4, 0x77, 0x9, 0x77, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 
    0xf3, 0xa, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0xf8, 0xa, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0xfd, 0xa, 0x3, 0xc, 0x3, 0xe, 
    0x3, 0x100, 0xb, 0x3, 0x5, 0x3, 0x102, 0xa, 0x3, 0x3, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x109, 0xa, 0x4, 0x3, 0x4, 0x3, 
    0x4, 0x7, 0x4, 0x10d, 0xa, 0x4, 0xc, 0x4, 0xe, 0x4, 0x110, 0xb, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x5, 0x5, 0x5, 0x115, 0xa, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x5, 0x5, 0x11a, 0xa, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x5, 0x5, 0x5, 0x121, 0xa, 0x5, 0x3, 0x5, 0x3, 0x5, 0x5, 
    0x5, 0x125, 0xa, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 
    0x5, 0x5, 0x12c, 0xa, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x5, 0x5, 0x134, 0xa, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 
    0x3, 0x5, 0x3, 0x5, 0x5, 0x5, 0x13b, 0xa, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x5, 0x5, 0x141, 0xa, 0x5, 0x3, 0x5, 0x7, 0x5, 0x144, 
    0xa, 0x5, 0xc, 0x5, 0xe, 0x5, 0x147, 0xb, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x7, 0x5, 0x154, 0xa, 0x5, 0xc, 0x5, 0xe, 0x5, 0x157, 
    0xb, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x7, 0x6, 0x161, 0xa, 0x6, 0xc, 0x6, 0xe, 0x6, 0x164, 
    0xb, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 0x5, 0x7, 0x169, 0xa, 0x7, 0x3, 
    0x7, 0x3, 0x7, 0x3, 0x8, 0x5, 0x8, 0x16e, 0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 
    0x3, 0x8, 0x5, 0x8, 0x173, 0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 0x5, 0x8, 0x177, 
    0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 
    0x3, 0x8, 0x5, 0x8, 0x180, 0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 
    0x8, 0x3, 0x8, 0x5, 0x8, 0x187, 0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 
    0x3, 0x8, 0x3, 0x8, 0x7, 0x8, 0x18e, 0xa, 0x8, 0xc, 0x8, 0xe, 0x8, 0x191, 
    0xb, 0x8, 0x3, 0x8, 0x3, 0x8, 0x7, 0x8, 0x195, 0xa, 0x8, 0xc, 0x8, 0xe, 
    0x8, 0x198, 0xb, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 
    0x3, 0x8, 0x7, 0x8, 0x1a0, 0xa, 0x8, 0xc, 0x8, 0xe, 0x8, 0x1a3, 0xb, 
    0x8, 0x3, 0x8, 0x3, 0x8, 0x5, 0x8, 0x1a7, 0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 
    0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 
    0x3, 0x8, 0x7, 0x8, 0x1b3, 0xa, 0x8, 0xc, 0x8, 0xe, 0x8, 0x1b6, 0xb, 
    0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 
    0xa, 0x3, 0xa, 0x3, 0xa, 0x5, 0xa, 0x1c1, 0xa, 0xa, 0x3, 0xa, 0x3, 0xa, 
    0x5, 0xa, 0x1c5, 0xa, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 
    0xa, 0x3, 0xa, 0x5, 0xa, 0x1cd, 0xa, 0xa, 0x3, 0xa, 0x5, 0xa, 0x1d0, 
    0xa, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x5, 0xb, 0x1da, 0xa, 0xb, 0x3, 0xb, 0x3, 0xb, 0x5, 
    0xb, 0x1de, 0xa, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xc, 0x5, 0xc, 0x1e7, 0xa, 0xc, 0x3, 0xc, 0x7, 0xc, 0x1ea, 
    0xa, 0xc, 0xc, 0xc, 0xe, 0xc, 0x1ed, 0xb, 0xc, 0x3, 0xc, 0x7, 0xc, 0x1f0, 
    0xa, 0xc, 0xc, 0xc, 0xe, 0xc, 0x1f3, 0xb, 0xc, 0x3, 0xc, 0x3, 0xc, 0x7, 
    0xc, 0x1f7, 0xa, 0xc, 0xc, 0xc, 0xe, 0xc, 0x1fa, 0xb, 0xc, 0x5, 0xc, 
    0x1fc, 0xa, 0xc, 0x3, 0xd, 0x5, 0xd, 0x1ff, 0xa, 0xd, 0x3, 0xd, 0x5, 
    0xd, 0x202, 0xa, 0xd, 0x3, 0xd, 0x3, 0xd, 0x5, 0xd, 0x206, 0xa, 0xd, 
    0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x7, 0xd, 
    0x20e, 0xa, 0xd, 0xc, 0xd, 0xe, 0xd, 0x211, 0xb, 0xd, 0x5, 0xd, 0x213, 
    0xa, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 0x5, 0xe, 0x219, 0xa, 
    0xe, 0x3, 0xe, 0x3, 0xe, 0x5, 0xe, 0x21d, 0xa, 0xe, 0x3, 0xf, 0x3, 0xf, 
    0x5, 0xf, 0x221, 0xa, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 
    0xf, 0x5, 0xf, 0x228, 0xa, 0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 
    0x10, 0x5, 0x10, 0x22e, 0xa, 0x10, 0x3, 0x11, 0x3, 0x11, 0x3, 0x11, 
    0x3, 0x11, 0x3, 0x11, 0x5, 0x11, 0x235, 0xa, 0x11, 0x3, 0x11, 0x3, 0x11, 
    0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 0x5, 0x12, 0x23c, 0xa, 0x12, 0x3, 0x12, 
    0x5, 0x12, 0x23f, 0xa, 0x12, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 
    0x3, 0x13, 0x7, 0x13, 0x246, 0xa, 0x13, 0xc, 0x13, 0xe, 0x13, 0x249, 
    0xb, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0x24f, 
    0xa, 0x14, 0x3, 0x14, 0x5, 0x14, 0x252, 0xa, 0x14, 0x3, 0x14, 0x3, 0x14, 
    0x5, 0x14, 0x256, 0xa, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 
    0x25b, 0xa, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 
    0x3, 0x14, 0x5, 0x14, 0x263, 0xa, 0x14, 0x3, 0x14, 0x5, 0x14, 0x266, 
    0xa, 0x14, 0x3, 0x14, 0x7, 0x14, 0x269, 0xa, 0x14, 0xc, 0x14, 0xe, 0x14, 
    0x26c, 0xb, 0x14, 0x3, 0x14, 0x5, 0x14, 0x26f, 0xa, 0x14, 0x3, 0x14, 
    0x7, 0x14, 0x272, 0xa, 0x14, 0xc, 0x14, 0xe, 0x14, 0x275, 0xb, 0x14, 
    0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0x27c, 
    0xa, 0x14, 0x3, 0x14, 0x7, 0x14, 0x27f, 0xa, 0x14, 0xc, 0x14, 0xe, 0x14, 
    0x282, 0xb, 0x14, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0x286, 0xa, 0x14, 
    0x3, 0x14, 0x3, 0x14, 0x3, 0x15, 0x3, 0x15, 0x5, 0x15, 0x28c, 0xa, 0x15, 
    0x3, 0x15, 0x3, 0x15, 0x5, 0x15, 0x290, 0xa, 0x15, 0x3, 0x15, 0x3, 0x15, 
    0x3, 0x16, 0x3, 0x16, 0x5, 0x16, 0x296, 0xa, 0x16, 0x3, 0x16, 0x3, 0x16, 
    0x5, 0x16, 0x29a, 0xa, 0x16, 0x3, 0x16, 0x3, 0x16, 0x3, 0x16, 0x7, 0x16, 
    0x29f, 0xa, 0x16, 0xc, 0x16, 0xe, 0x16, 0x2a2, 0xb, 0x16, 0x3, 0x16, 
    0x3, 0x16, 0x5, 0x16, 0x2a6, 0xa, 0x16, 0x3, 0x16, 0x3, 0x16, 0x3, 0x17, 
    0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x5, 0x17, 0x2b0, 
    0xa, 0x17, 0x5, 0x17, 0x2b2, 0xa, 0x17, 0x5, 0x17, 0x2b4, 0xa, 0x17, 
    0x3, 0x17, 0x3, 0x17, 0x3, 0x18, 0x3, 0x18, 0x7, 0x18, 0x2ba, 0xa, 0x18, 
    0xc, 0x18, 0xe, 0x18, 0x2bd, 0xb, 0x18, 0x3, 0x19, 0x3, 0x19, 0x5, 0x19, 
    0x2c1, 0xa, 0x19, 0x3, 0x19, 0x3, 0x19, 0x5, 0x19, 0x2c5, 0xa, 0x19, 
    0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x5, 0x19, 0x2ca, 0xa, 0x19, 0x3, 0x19, 
    0x5, 0x19, 0x2cd, 0xa, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 
    0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x7, 0x19, 0x2d8, 
    0xa, 0x19, 0xc, 0x19, 0xe, 0x19, 0x2db, 0xb, 0x19, 0x5, 0x19, 0x2dd, 
    0xa, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x1a, 0x3, 0x1a, 0x5, 0x1a, 0x2e3, 
    0xa, 0x1a, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 
    0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 
    0x3, 0x1b, 0x3, 0x1b, 0x5, 0x1b, 0x2f3, 0xa, 0x1b, 0x3, 0x1c, 0x3, 0x1c, 
    0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x5, 0x1c, 0x2fc, 
    0xa, 0x1c, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x5, 0x1d, 0x301, 0xa, 0x1d, 
    0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x5, 0x1d, 0x306, 0xa, 0x1d, 0x3, 0x1d, 
    0x3, 0x1d, 0x5, 0x1d, 0x30a, 0xa, 0x1d, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1f, 
    0x3, 0x1f, 0x5, 0x1f, 0x310, 0xa, 0x1f, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 
    0x3, 0x20, 0x3, 0x21, 0x3, 0x21, 0x3, 0x22, 0x3, 0x22, 0x3, 0x23, 0x3, 
    0x23, 0x3, 0x23, 0x3, 0x23, 0x7, 0x23, 0x31e, 0xa, 0x23, 0xc, 0x23, 
    0xe, 0x23, 0x321, 0xb, 0x23, 0x3, 0x23, 0x3, 0x23, 0x5, 0x23, 0x325, 
    0xa, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x5, 
    0x23, 0x32c, 0xa, 0x23, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x7, 0x24, 
    0x331, 0xa, 0x24, 0xc, 0x24, 0xe, 0x24, 0x334, 0xb, 0x24, 0x3, 0x24, 
    0x5, 0x24, 0x337, 0xa, 0x24, 0x3, 0x25, 0x3, 0x25, 0x3, 0x25, 0x3, 0x25, 
    0x7, 0x25, 0x33d, 0xa, 0x25, 0xc, 0x25, 0xe, 0x25, 0x340, 0xb, 0x25, 
    0x3, 0x25, 0x3, 0x25, 0x5, 0x25, 0x344, 0xa, 0x25, 0x3, 0x25, 0x5, 0x25, 
    0x347, 0xa, 0x25, 0x3, 0x26, 0x3, 0x26, 0x3, 0x26, 0x3, 0x26, 0x3, 0x26, 
    0x5, 0x26, 0x34e, 0xa, 0x26, 0x3, 0x26, 0x5, 0x26, 0x351, 0xa, 0x26, 
    0x3, 0x26, 0x3, 0x26, 0x3, 0x26, 0x3, 0x26, 0x5, 0x26, 0x357, 0xa, 0x26, 
    0x3, 0x26, 0x3, 0x26, 0x3, 0x26, 0x5, 0x26, 0x35c, 0xa, 0x26, 0x3, 0x27, 
    0x3, 0x27, 0x3, 0x28, 0x3, 0x28, 0x3, 0x28, 0x3, 0x28, 0x3, 0x28, 0x5, 
    0x28, 0x365, 0xa, 0x28, 0x3, 0x28, 0x3, 0x28, 0x3, 0x28, 0x3, 0x28, 
    0x3, 0x28, 0x3, 0x28, 0x3, 0x28, 0x5, 0x28, 0x36e, 0xa, 0x28, 0x3, 0x28, 
    0x3, 0x28, 0x5, 0x28, 0x372, 0xa, 0x28, 0x3, 0x29, 0x3, 0x29, 0x3, 0x29, 
    0x5, 0x29, 0x377, 0xa, 0x29, 0x3, 0x2a, 0x3, 0x2a, 0x3, 0x2a, 0x3, 0x2a, 
    0x3, 0x2a, 0x5, 0x2a, 0x37e, 0xa, 0x2a, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 
    0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x5, 0x2b, 0x388, 
    0xa, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x3, 
    0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x3, 0x2b, 0x5, 0x2b, 0x393, 0xa, 0x2b, 
    0x3, 0x2c, 0x3, 0x2c, 0x5, 0x2c, 0x397, 0xa, 0x2c, 0x3, 0x2c, 0x5, 0x2c, 
    0x39a, 0xa, 0x2c, 0x3, 0x2c, 0x3, 0x2c, 0x3, 0x2c, 0x7, 0x2c, 0x39f, 
    0xa, 0x2c, 0xc, 0x2c, 0xe, 0x2c, 0x3a2, 0xb, 0x2c, 0x3, 0x2c, 0x3, 0x2c, 
    0x5, 0x2c, 0x3a6, 0xa, 0x2c, 0x3, 0x2c, 0x3, 0x2c, 0x3, 0x2d, 0x3, 0x2d, 
    0x3, 0x2d, 0x5, 0x2d, 0x3ad, 0xa, 0x2d, 0x3, 0x2d, 0x3, 0x2d, 0x3, 0x2d, 
    0x5, 0x2d, 0x3b2, 0xa, 0x2d, 0x3, 0x2e, 0x3, 0x2e, 0x3, 0x2f, 0x3, 0x2f, 
    0x3, 0x2f, 0x3, 0x2f, 0x3, 0x2f, 0x3, 0x2f, 0x7, 0x2f, 0x3bc, 0xa, 0x2f, 
    0xc, 0x2f, 0xe, 0x2f, 0x3bf, 0xb, 0x2f, 0x3, 0x30, 0x3, 0x30, 0x3, 0x30, 
    0x3, 0x30, 0x3, 0x30, 0x7, 0x30, 0x3c6, 0xa, 0x30, 0xc, 0x30, 0xe, 0x30, 
    0x3c9, 0xb, 0x30, 0x3, 0x31, 0x3, 0x31, 0x3, 0x31, 0x5, 0x31, 0x3ce, 
    0xa, 0x31, 0x3, 0x31, 0x3, 0x31, 0x3, 0x31, 0x3, 0x31, 0x3, 0x31, 0x5, 
    0x31, 0x3d5, 0xa, 0x31, 0x5, 0x31, 0x3d7, 0xa, 0x31, 0x3, 0x31, 0x3, 
    0x31, 0x5, 0x31, 0x3db, 0xa, 0x31, 0x3, 0x32, 0x3, 0x32, 0x3, 0x32, 
    0x3, 0x32, 0x3, 0x32, 0x3, 0x33, 0x3, 0x33, 0x3, 0x33, 0x3, 0x33, 0x5, 
    0x33, 0x3e6, 0xa, 0x33, 0x3, 0x34, 0x3, 0x34, 0x3, 0x34, 0x3, 0x35, 
    0x3, 0x35, 0x5, 0x35, 0x3ed, 0xa, 0x35, 0x3, 0x35, 0x3, 0x35, 0x5, 0x35, 
    0x3f1, 0xa, 0x35, 0x5, 0x35, 0x3f3, 0xa, 0x35, 0x3, 0x36, 0x3, 0x36, 
    0x3, 0x36, 0x3, 0x36, 0x3, 0x36, 0x5, 0x36, 0x3fa, 0xa, 0x36, 0x5, 0x36, 
    0x3fc, 0xa, 0x36, 0x3, 0x36, 0x3, 0x36, 0x3, 0x36, 0x3, 0x36, 0x3, 0x36, 
    0x7, 0x36, 0x403, 0xa, 0x36, 0xc, 0x36, 0xe, 0x36, 0x406, 0xb, 0x36, 
    0x3, 0x36, 0x3, 0x36, 0x5, 0x36, 0x40a, 0xa, 0x36, 0x3, 0x37, 0x3, 0x37, 
    0x3, 0x37, 0x3, 0x38, 0x3, 0x38, 0x3, 0x38, 0x3, 0x38, 0x5, 0x38, 0x413, 
    0xa, 0x38, 0x3, 0x38, 0x3, 0x38, 0x3, 0x39, 0x5, 0x39, 0x418, 0xa, 0x39, 
    0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x5, 0x39, 0x41f, 
    0xa, 0x39, 0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x5, 0x39, 0x424, 0xa, 0x39, 
    0x5, 0x39, 0x426, 0xa, 0x39, 0x3, 0x39, 0x5, 0x39, 0x429, 0xa, 0x39, 
    0x3, 0x39, 0x3, 0x39, 0x5, 0x39, 0x42d, 0xa, 0x39, 0x3, 0x39, 0x3, 0x39, 
    0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x5, 0x39, 0x436, 
    0xa, 0x39, 0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x3, 0x39, 0x5, 0x39, 0x43c, 
    0xa, 0x39, 0x3, 0x3a, 0x3, 0x3a, 0x3, 0x3a, 0x3, 0x3a, 0x3, 0x3a, 0x7, 
    0x3a, 0x443, 0xa, 0x3a, 0xc, 0x3a, 0xe, 0x3a, 0x446, 0xb, 0x3a, 0x3, 
    0x3a, 0x3, 0x3a, 0x3, 0x3b, 0x3, 0x3b, 0x3, 0x3b, 0x5, 0x3b, 0x44d, 
    0xa, 0x3b, 0x3, 0x3c, 0x3, 0x3c, 0x3, 0x3c, 0x7, 0x3c, 0x452, 0xa, 0x3c, 
    0xc, 0x3c, 0xe, 0x3c, 0x455, 0xb, 0x3c, 0x3, 0x3d, 0x3, 0x3d, 0x3, 0x3d, 
    0x3, 0x3d, 0x3, 0x3d, 0x3, 0x3e, 0x3, 0x3e, 0x7, 0x3e, 0x45e, 0xa, 0x3e, 
    0xc, 0x3e, 0xe, 0x3e, 0x461, 0xb, 0x3e, 0x3, 0x3f, 0x3, 0x3f, 0x3, 0x3f, 
    0x3, 0x3f, 0x3, 0x3f, 0x3, 0x3f, 0x3, 0x3f, 0x3, 0x3f, 0x5, 0x3f, 0x46b, 
    0xa, 0x3f, 0x3, 0x3f, 0x3, 0x3f, 0x5, 0x3f, 0x46f, 0xa, 0x3f, 0x3, 0x3f, 
    0x3, 0x3f, 0x3, 0x3f, 0x5, 0x3f, 0x474, 0xa, 0x3f, 0x5, 0x3f, 0x476, 
    0xa, 0x3f, 0x3, 0x3f, 0x3, 0x3f, 0x3, 0x40, 0x3, 0x40, 0x3, 0x40, 0x7, 
    0x40, 0x47d, 0xa, 0x40, 0xc, 0x40, 0xe, 0x40, 0x480, 0xb, 0x40, 0x3, 
    0x40, 0x3, 0x40, 0x5, 0x40, 0x484, 0xa, 0x40, 0x3, 0x40, 0x3, 0x40, 
    0x3, 0x40, 0x3, 0x40, 0x3, 0x41, 0x3, 0x41, 0x3, 0x41, 0x3, 0x41, 0x3, 
    0x42, 0x3, 0x42, 0x3, 0x42, 0x3, 0x42, 0x3, 0x42, 0x3, 0x42, 0x3, 0x42, 
    0x3, 0x42, 0x3, 0x42, 0x7, 0x42, 0x497, 0xa, 0x42, 0xc, 0x42, 0xe, 0x42, 
    0x49a, 0xb, 0x42, 0x3, 0x43, 0x3, 0x43, 0x3, 0x43, 0x3, 0x43, 0x3, 0x43, 
    0x3, 0x43, 0x5, 0x43, 0x4a2, 0xa, 0x43, 0x3, 0x43, 0x3, 0x43, 0x5, 0x43, 
    0x4a6, 0xa, 0x43, 0x3, 0x43, 0x3, 0x43, 0x3, 0x43, 0x3, 0x43, 0x3, 0x43, 
    0x3, 0x43, 0x3, 0x43, 0x3, 0x43, 0x3, 0x43, 0x7, 0x43, 0x4b1, 0xa, 0x43, 
    0xc, 0x43, 0xe, 0x43, 0x4b4, 0xb, 0x43, 0x7, 0x43, 0x4b6, 0xa, 0x43, 
    0xc, 0x43, 0xe, 0x43, 0x4b9, 0xb, 0x43, 0x3, 0x43, 0x3, 0x43, 0x3, 0x44, 
    0x3, 0x44, 0x5, 0x44, 0x4bf, 0xa, 0x44, 0x3, 0x44, 0x3, 0x44, 0x5, 0x44, 
    0x4c3, 0xa, 0x44, 0x3, 0x44, 0x3, 0x44, 0x3, 0x45, 0x3, 0x45, 0x3, 0x45, 
    0x5, 0x45, 0x4ca, 0xa, 0x45, 0x3, 0x45, 0x3, 0x45, 0x3, 0x46, 0x3, 0x46, 
    0x5, 0x46, 0x4d0, 0xa, 0x46, 0x3, 0x46, 0x3, 0x46, 0x3, 0x46, 0x3, 0x46, 
    0x3, 0x46, 0x5, 0x46, 0x4d7, 0xa, 0x46, 0x5, 0x46, 0x4d9, 0xa, 0x46, 
    0x3, 0x46, 0x3, 0x46, 0x3, 0x47, 0x3, 0x47, 0x3, 0x47, 0x3, 0x47, 0x7, 
    0x47, 0x4e1, 0xa, 0x47, 0xc, 0x47, 0xe, 0x47, 0x4e4, 0xb, 0x47, 0x3, 
    0x47, 0x3, 0x47, 0x3, 0x48, 0x3, 0x48, 0x7, 0x48, 0x4ea, 0xa, 0x48, 
    0xc, 0x48, 0xe, 0x48, 0x4ed, 0xb, 0x48, 0x3, 0x49, 0x3, 0x49, 0x3, 0x49, 
    0x3, 0x49, 0x3, 0x49, 0x3, 0x4a, 0x3, 0x4a, 0x3, 0x4a, 0x3, 0x4a, 0x3, 
    0x4a, 0x5, 0x4a, 0x4f9, 0xa, 0x4a, 0x3, 0x4b, 0x3, 0x4b, 0x3, 0x4b, 
    0x3, 0x4b, 0x3, 0x4b, 0x5, 0x4b, 0x500, 0xa, 0x4b, 0x3, 0x4b, 0x3, 0x4b, 
    0x3, 0x4b, 0x3, 0x4c, 0x3, 0x4c, 0x3, 0x4c, 0x3, 0x4c, 0x5, 0x4c, 0x509, 
    0xa, 0x4c, 0x3, 0x4c, 0x3, 0x4c, 0x3, 0x4c, 0x3, 0x4c, 0x3, 0x4c, 0x3, 
    0x4c, 0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x5, 0x4d, 
    0x516, 0xa, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 
    0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x3, 0x4d, 0x7, 
    0x4d, 0x523, 0xa, 0x4d, 0xc, 0x4d, 0xe, 0x4d, 0x526, 0xb, 0x4d, 0x3, 
    0x4d, 0x3, 0x4d, 0x3, 0x4e, 0x3, 0x4e, 0x3, 0x4e, 0x3, 0x4e, 0x3, 0x4e, 
    0x5, 0x4e, 0x52f, 0xa, 0x4e, 0x3, 0x4e, 0x3, 0x4e, 0x3, 0x4e, 0x5, 0x4e, 
    0x534, 0xa, 0x4e, 0x3, 0x4e, 0x3, 0x4e, 0x3, 0x4e, 0x3, 0x4e, 0x5, 0x4e, 
    0x53a, 0xa, 0x4e, 0x3, 0x4e, 0x3, 0x4e, 0x5, 0x4e, 0x53e, 0xa, 0x4e, 
    0x3, 0x4e, 0x3, 0x4e, 0x3, 0x4f, 0x3, 0x4f, 0x3, 0x50, 0x3, 0x50, 0x3, 
    0x50, 0x7, 0x50, 0x547, 0xa, 0x50, 0xc, 0x50, 0xe, 0x50, 0x54a, 0xb, 
    0x50, 0x3, 0x51, 0x3, 0x51, 0x3, 0x51, 0x3, 0x51, 0x3, 0x51, 0x3, 0x52, 
    0x3, 0x52, 0x3, 0x52, 0x7, 0x52, 0x554, 0xa, 0x52, 0xc, 0x52, 0xe, 0x52, 
    0x557, 0xb, 0x52, 0x3, 0x52, 0x3, 0x52, 0x3, 0x52, 0x3, 0x52, 0x7, 0x52, 
    0x55d, 0xa, 0x52, 0xc, 0x52, 0xe, 0x52, 0x560, 0xb, 0x52, 0x3, 0x53, 
    0x3, 0x53, 0x3, 0x53, 0x3, 0x53, 0x3, 0x53, 0x3, 0x53, 0x5, 0x53, 0x568, 
    0xa, 0x53, 0x3, 0x53, 0x3, 0x53, 0x3, 0x53, 0x5, 0x53, 0x56d, 0xa, 0x53, 
    0x3, 0x53, 0x3, 0x53, 0x3, 0x53, 0x3, 0x53, 0x5, 0x53, 0x573, 0xa, 0x53, 
    0x3, 0x53, 0x3, 0x53, 0x3, 0x53, 0x3, 0x54, 0x3, 0x54, 0x3, 0x54, 0x3, 
    0x54, 0x3, 0x54, 0x7, 0x54, 0x57d, 0xa, 0x54, 0xc, 0x54, 0xe, 0x54, 
    0x580, 0xb, 0x54, 0x3, 0x54, 0x3, 0x54, 0x3, 0x54, 0x3, 0x55, 0x3, 0x55, 
    0x3, 0x56, 0x3, 0x56, 0x3, 0x56, 0x3, 0x56, 0x7, 0x56, 0x58b, 0xa, 0x56, 
    0xc, 0x56, 0xe, 0x56, 0x58e, 0xb, 0x56, 0x3, 0x57, 0x3, 0x57, 0x3, 0x57, 
    0x3, 0x57, 0x3, 0x57, 0x3, 0x57, 0x3, 0x57, 0x5, 0x57, 0x597, 0xa, 0x57, 
    0x3, 0x58, 0x3, 0x58, 0x3, 0x58, 0x3, 0x58, 0x7, 0x58, 0x59d, 0xa, 0x58, 
    0xc, 0x58, 0xe, 0x58, 0x5a0, 0xb, 0x58, 0x3, 0x59, 0x3, 0x59, 0x3, 0x59, 
    0x3, 0x59, 0x3, 0x59, 0x5, 0x59, 0x5a7, 0xa, 0x59, 0x3, 0x5a, 0x3, 0x5a, 
    0x3, 0x5a, 0x3, 0x5a, 0x5, 0x5a, 0x5ad, 0xa, 0x5a, 0x3, 0x5b, 0x3, 0x5b, 
    0x5, 0x5b, 0x5b1, 0xa, 0x5b, 0x3, 0x5b, 0x3, 0x5b, 0x3, 0x5b, 0x3, 0x5b, 
    0x3, 0x5b, 0x3, 0x5b, 0x3, 0x5b, 0x3, 0x5b, 0x3, 0x5b, 0x5, 0x5b, 0x5bc, 
    0xa, 0x5b, 0x3, 0x5c, 0x3, 0x5c, 0x3, 0x5d, 0x3, 0x5d, 0x3, 0x5d, 0x3, 
    0x5d, 0x5, 0x5d, 0x5c4, 0xa, 0x5d, 0x3, 0x5d, 0x3, 0x5d, 0x5, 0x5d, 
    0x5c8, 0xa, 0x5d, 0x3, 0x5d, 0x3, 0x5d, 0x3, 0x5d, 0x3, 0x5d, 0x5, 0x5d, 
    0x5ce, 0xa, 0x5d, 0x3, 0x5e, 0x3, 0x5e, 0x3, 0x5e, 0x7, 0x5e, 0x5d3, 
    0xa, 0x5e, 0xc, 0x5e, 0xe, 0x5e, 0x5d6, 0xb, 0x5e, 0x3, 0x5f, 0x3, 0x5f, 
    0x3, 0x60, 0x3, 0x60, 0x3, 0x60, 0x3, 0x60, 0x3, 0x60, 0x5, 0x60, 0x5df, 
    0xa, 0x60, 0x3, 0x61, 0x5, 0x61, 0x5e2, 0xa, 0x61, 0x3, 0x61, 0x3, 0x61, 
    0x3, 0x61, 0x3, 0x61, 0x3, 0x61, 0x3, 0x61, 0x5, 0x61, 0x5ea, 0xa, 0x61, 
    0x3, 0x61, 0x5, 0x61, 0x5ed, 0xa, 0x61, 0x3, 0x61, 0x5, 0x61, 0x5f0, 
    0xa, 0x61, 0x3, 0x62, 0x3, 0x62, 0x3, 0x62, 0x3, 0x62, 0x3, 0x62, 0x7, 
    0x62, 0x5f7, 0xa, 0x62, 0xc, 0x62, 0xe, 0x62, 0x5fa, 0xb, 0x62, 0x5, 
    0x62, 0x5fc, 0xa, 0x62, 0x3, 0x62, 0x3, 0x62, 0x3, 0x63, 0x3, 0x63, 
    0x3, 0x63, 0x3, 0x63, 0x3, 0x63, 0x3, 0x63, 0x3, 0x63, 0x7, 0x63, 0x607, 
    0xa, 0x63, 0xc, 0x63, 0xe, 0x63, 0x60a, 0xb, 0x63, 0x5, 0x63, 0x60c, 
    0xa, 0x63, 0x3, 0x64, 0x3, 0x64, 0x3, 0x64, 0x3, 0x64, 0x3, 0x64, 0x3, 
    0x64, 0x7, 0x64, 0x614, 0xa, 0x64, 0xc, 0x64, 0xe, 0x64, 0x617, 0xb, 
    0x64, 0x5, 0x64, 0x619, 0xa, 0x64, 0x3, 0x64, 0x3, 0x64, 0x3, 0x64, 
    0x3, 0x64, 0x5, 0x64, 0x61f, 0xa, 0x64, 0x3, 0x64, 0x3, 0x64, 0x3, 0x65, 
    0x3, 0x65, 0x3, 0x65, 0x3, 0x65, 0x3, 0x66, 0x3, 0x66, 0x3, 0x66, 0x3, 
    0x66, 0x3, 0x66, 0x5, 0x66, 0x62c, 0xa, 0x66, 0x3, 0x66, 0x3, 0x66, 
    0x5, 0x66, 0x630, 0xa, 0x66, 0x3, 0x67, 0x3, 0x67, 0x3, 0x67, 0x5, 0x67, 
    0x635, 0xa, 0x67, 0x3, 0x67, 0x5, 0x67, 0x638, 0xa, 0x67, 0x3, 0x67, 
    0x3, 0x67, 0x5, 0x67, 0x63c, 0xa, 0x67, 0x3, 0x67, 0x7, 0x67, 0x63f, 
    0xa, 0x67, 0xc, 0x67, 0xe, 0x67, 0x642, 0xb, 0x67, 0x3, 0x67, 0x7, 0x67, 
    0x645, 0xa, 0x67, 0xc, 0x67, 0xe, 0x67, 0x648, 0xb, 0x67, 0x3, 0x67, 
    0x3, 0x67, 0x5, 0x67, 0x64c, 0xa, 0x67, 0x3, 0x67, 0x7, 0x67, 0x64f, 
    0xa, 0x67, 0xc, 0x67, 0xe, 0x67, 0x652, 0xb, 0x67, 0x3, 0x67, 0x7, 0x67, 
    0x655, 0xa, 0x67, 0xc, 0x67, 0xe, 0x67, 0x658, 0xb, 0x67, 0x3, 0x67, 
    0x3, 0x67, 0x3, 0x67, 0x3, 0x67, 0x3, 0x68, 0x3, 0x68, 0x3, 0x68, 0x3, 
    0x68, 0x3, 0x68, 0x5, 0x68, 0x663, 0xa, 0x68, 0x3, 0x68, 0x3, 0x68, 
    0x3, 0x69, 0x3, 0x69, 0x3, 0x69, 0x3, 0x69, 0x3, 0x6a, 0x3, 0x6a, 0x3, 
    0x6a, 0x3, 0x6a, 0x3, 0x6a, 0x3, 0x6a, 0x3, 0x6a, 0x3, 0x6a, 0x3, 0x6a, 
    0x5, 0x6a, 0x674, 0xa, 0x6a, 0x6, 0x6a, 0x676, 0xa, 0x6a, 0xd, 0x6a, 
    0xe, 0x6a, 0x677, 0x3, 0x6b, 0x3, 0x6b, 0x3, 0x6b, 0x3, 0x6b, 0x3, 0x6b, 
    0x7, 0x6b, 0x67f, 0xa, 0x6b, 0xc, 0x6b, 0xe, 0x6b, 0x682, 0xb, 0x6b, 
    0x3, 0x6c, 0x3, 0x6c, 0x3, 0x6c, 0x3, 0x6c, 0x3, 0x6c, 0x3, 0x6c, 0x3, 
    0x6c, 0x3, 0x6c, 0x3, 0x6c, 0x5, 0x6c, 0x68d, 0xa, 0x6c, 0x3, 0x6d, 
    0x5, 0x6d, 0x690, 0xa, 0x6d, 0x3, 0x6d, 0x3, 0x6d, 0x3, 0x6d, 0x3, 0x6d, 
    0x3, 0x6d, 0x6, 0x6d, 0x697, 0xa, 0x6d, 0xd, 0x6d, 0xe, 0x6d, 0x698, 
    0x3, 0x6e, 0x3, 0x6e, 0x5, 0x6e, 0x69d, 0xa, 0x6e, 0x3, 0x6e, 0x3, 0x6e, 
    0x3, 0x6f, 0x3, 0x6f, 0x3, 0x6f, 0x3, 0x6f, 0x3, 0x6f, 0x3, 0x6f, 0x3, 
    0x6f, 0x7, 0x6f, 0x6a8, 0xa, 0x6f, 0xc, 0x6f, 0xe, 0x6f, 0x6ab, 0xb, 
    0x6f, 0x3, 0x70, 0x3, 0x70, 0x3, 0x70, 0x3, 0x70, 0x3, 0x71, 0x3, 0x71, 
    0x3, 0x71, 0x3, 0x71, 0x3, 0x71, 0x3, 0x71, 0x3, 0x71, 0x5, 0x71, 0x6b8, 
    0xa, 0x71, 0x3, 0x71, 0x3, 0x71, 0x3, 0x71, 0x3, 0x71, 0x5, 0x71, 0x6be, 
    0xa, 0x71, 0x3, 0x72, 0x3, 0x72, 0x3, 0x72, 0x5, 0x72, 0x6c3, 0xa, 0x72, 
    0x3, 0x72, 0x3, 0x72, 0x5, 0x72, 0x6c7, 0xa, 0x72, 0x3, 0x72, 0x3, 0x72, 
    0x3, 0x72, 0x5, 0x72, 0x6cc, 0xa, 0x72, 0x3, 0x72, 0x3, 0x72, 0x7, 0x72, 
    0x6d0, 0xa, 0x72, 0xc, 0x72, 0xe, 0x72, 0x6d3, 0xb, 0x72, 0x3, 0x72, 
    0x7, 0x72, 0x6d6, 0xa, 0x72, 0xc, 0x72, 0xe, 0x72, 0x6d9, 0xb, 0x72, 
    0x3, 0x72, 0x3, 0x72, 0x3, 0x72, 0x3, 0x72, 0x3, 0x73, 0x3, 0x73, 0x5, 
    0x73, 0x6e1, 0xa, 0x73, 0x3, 0x73, 0x3, 0x73, 0x5, 0x73, 0x6e5, 0xa, 
    0x73, 0x3, 0x73, 0x3, 0x73, 0x3, 0x73, 0x3, 0x73, 0x7, 0x73, 0x6eb, 
    0xa, 0x73, 0xc, 0x73, 0xe, 0x73, 0x6ee, 0xb, 0x73, 0x3, 0x73, 0x3, 0x73, 
    0x3, 0x74, 0x3, 0x74, 0x5, 0x74, 0x6f4, 0xa, 0x74, 0x3, 0x74, 0x3, 0x74, 
    0x3, 0x74, 0x3, 0x74, 0x7, 0x74, 0x6fa, 0xa, 0x74, 0xc, 0x74, 0xe, 0x74, 
    0x6fd, 0xb, 0x74, 0x3, 0x74, 0x3, 0x74, 0x3, 0x75, 0x3, 0x75, 0x3, 0x75, 
    0x3, 0x75, 0x3, 0x75, 0x3, 0x75, 0x3, 0x75, 0x3, 0x75, 0x3, 0x75, 0x3, 
    0x75, 0x3, 0x75, 0x3, 0x75, 0x3, 0x75, 0x7, 0x75, 0x70e, 0xa, 0x75, 
    0xc, 0x75, 0xe, 0x75, 0x711, 0xb, 0x75, 0x3, 0x75, 0x3, 0x75, 0x5, 0x75, 
    0x715, 0xa, 0x75, 0x3, 0x76, 0x3, 0x76, 0x3, 0x76, 0x3, 0x76, 0x3, 0x76, 
    0x5, 0x76, 0x71c, 0xa, 0x76, 0x3, 0x76, 0x3, 0x76, 0x3, 0x76, 0x3, 0x76, 
    0x3, 0x77, 0x3, 0x77, 0x3, 0x77, 0x5, 0x77, 0x725, 0xa, 0x77, 0x3, 0x77, 
    0x2, 0x3, 0x82, 0x78, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 
    0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 
    0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 
    0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 
    0x5c, 0x5e, 0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 
    0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e, 0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 
    0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e, 0xa0, 0xa2, 
    0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 
    0xbc, 0xbe, 0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 
    0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 
    0xec, 0x2, 0x11, 0x3, 0x2, 0xc0, 0xc2, 0x7, 0x2, 0xd, 0xd, 0x23, 0x25, 
    0x70, 0x70, 0x95, 0x95, 0x97, 0x97, 0x4, 0x2, 0x9, 0x9, 0xc5, 0xc5, 
    0x3, 0x2, 0x63, 0x64, 0x3, 0x2, 0x55, 0x57, 0x4, 0x2, 0x72, 0x72, 0x7a, 
    0x7a, 0x3, 0x2, 0x24, 0x25, 0x3, 0x2, 0x30, 0x32, 0x3, 0x2, 0x84, 0x85, 
    0x3, 0x2, 0x39, 0x3a, 0x4, 0x2, 0xd, 0xd, 0x95, 0x95, 0x7, 0x2, 0x22, 
    0x22, 0x27, 0x2a, 0x5b, 0x5b, 0x6b, 0x6b, 0x79, 0x7a, 0x4, 0x2, 0x41, 
    0x41, 0x45, 0x45, 0x5, 0x2, 0x40, 0x41, 0x44, 0x45, 0x49, 0x4b, 0x4, 
    0x2, 0x96, 0x96, 0xaf, 0xaf, 0x2, 0x7e7, 0x2, 0xee, 0x3, 0x2, 0x2, 0x2, 
    0x4, 0x101, 0x3, 0x2, 0x2, 0x2, 0x6, 0x103, 0x3, 0x2, 0x2, 0x2, 0x8, 
    0x114, 0x3, 0x2, 0x2, 0x2, 0xa, 0x15c, 0x3, 0x2, 0x2, 0x2, 0xc, 0x168, 
    0x3, 0x2, 0x2, 0x2, 0xe, 0x16d, 0x3, 0x2, 0x2, 0x2, 0x10, 0x1bb, 0x3, 
    0x2, 0x2, 0x2, 0x12, 0x1bd, 0x3, 0x2, 0x2, 0x2, 0x14, 0x1d6, 0x3, 0x2, 
    0x2, 0x2, 0x16, 0x1e6, 0x3, 0x2, 0x2, 0x2, 0x18, 0x201, 0x3, 0x2, 0x2, 
    0x2, 0x1a, 0x21c, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x21e, 0x3, 0x2, 0x2, 0x2, 
    0x1e, 0x22d, 0x3, 0x2, 0x2, 0x2, 0x20, 0x22f, 0x3, 0x2, 0x2, 0x2, 0x22, 
    0x23b, 0x3, 0x2, 0x2, 0x2, 0x24, 0x240, 0x3, 0x2, 0x2, 0x2, 0x26, 0x24c, 
    0x3, 0x2, 0x2, 0x2, 0x28, 0x28f, 0x3, 0x2, 0x2, 0x2, 0x2a, 0x293, 0x3, 
    0x2, 0x2, 0x2, 0x2c, 0x2a9, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x2b7, 0x3, 0x2, 
    0x2, 0x2, 0x30, 0x2be, 0x3, 0x2, 0x2, 0x2, 0x32, 0x2e2, 0x3, 0x2, 0x2, 
    0x2, 0x34, 0x2f2, 0x3, 0x2, 0x2, 0x2, 0x36, 0x2fb, 0x3, 0x2, 0x2, 0x2, 
    0x38, 0x309, 0x3, 0x2, 0x2, 0x2, 0x3a, 0x30b, 0x3, 0x2, 0x2, 0x2, 0x3c, 
    0x30d, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x311, 0x3, 0x2, 0x2, 0x2, 0x40, 0x315, 
    0x3, 0x2, 0x2, 0x2, 0x42, 0x317, 0x3, 0x2, 0x2, 0x2, 0x44, 0x32b, 0x3, 
    0x2, 0x2, 0x2, 0x46, 0x32d, 0x3, 0x2, 0x2, 0x2, 0x48, 0x338, 0x3, 0x2, 
    0x2, 0x2, 0x4a, 0x34d, 0x3, 0x2, 0x2, 0x2, 0x4c, 0x35d, 0x3, 0x2, 0x2, 
    0x2, 0x4e, 0x371, 0x3, 0x2, 0x2, 0x2, 0x50, 0x376, 0x3, 0x2, 0x2, 0x2, 
    0x52, 0x378, 0x3, 0x2, 0x2, 0x2, 0x54, 0x392, 0x3, 0x2, 0x2, 0x2, 0x56, 
    0x394, 0x3, 0x2, 0x2, 0x2, 0x58, 0x3b1, 0x3, 0x2, 0x2, 0x2, 0x5a, 0x3b3, 
    0x3, 0x2, 0x2, 0x2, 0x5c, 0x3b5, 0x3, 0x2, 0x2, 0x2, 0x5e, 0x3c0, 0x3, 
    0x2, 0x2, 0x2, 0x60, 0x3cd, 0x3, 0x2, 0x2, 0x2, 0x62, 0x3dc, 0x3, 0x2, 
    0x2, 0x2, 0x64, 0x3e1, 0x3, 0x2, 0x2, 0x2, 0x66, 0x3e7, 0x3, 0x2, 0x2, 
    0x2, 0x68, 0x3f2, 0x3, 0x2, 0x2, 0x2, 0x6a, 0x3f4, 0x3, 0x2, 0x2, 0x2, 
    0x6c, 0x40b, 0x3, 0x2, 0x2, 0x2, 0x6e, 0x40e, 0x3, 0x2, 0x2, 0x2, 0x70, 
    0x425, 0x3, 0x2, 0x2, 0x2, 0x72, 0x43d, 0x3, 0x2, 0x2, 0x2, 0x74, 0x44c, 
    0x3, 0x2, 0x2, 0x2, 0x76, 0x44e, 0x3, 0x2, 0x2, 0x2, 0x78, 0x456, 0x3, 
    0x2, 0x2, 0x2, 0x7a, 0x45b, 0x3, 0x2, 0x2, 0x2, 0x7c, 0x462, 0x3, 0x2, 
    0x2, 0x2, 0x7e, 0x483, 0x3, 0x2, 0x2, 0x2, 0x80, 0x489, 0x3, 0x2, 0x2, 
    0x2, 0x82, 0x48d, 0x3, 0x2, 0x2, 0x2, 0x84, 0x49b, 0x3, 0x2, 0x2, 0x2, 
    0x86, 0x4c2, 0x3, 0x2, 0x2, 0x2, 0x88, 0x4c9, 0x3, 0x2, 0x2, 0x2, 0x8a, 
    0x4cd, 0x3, 0x2, 0x2, 0x2, 0x8c, 0x4dc, 0x3, 0x2, 0x2, 0x2, 0x8e, 0x4e7, 
    0x3, 0x2, 0x2, 0x2, 0x90, 0x4ee, 0x3, 0x2, 0x2, 0x2, 0x92, 0x4f8, 0x3, 
    0x2, 0x2, 0x2, 0x94, 0x4fa, 0x3, 0x2, 0x2, 0x2, 0x96, 0x504, 0x3, 0x2, 
    0x2, 0x2, 0x98, 0x510, 0x3, 0x2, 0x2, 0x2, 0x9a, 0x529, 0x3, 0x2, 0x2, 
    0x2, 0x9c, 0x541, 0x3, 0x2, 0x2, 0x2, 0x9e, 0x543, 0x3, 0x2, 0x2, 0x2, 
    0xa0, 0x54b, 0x3, 0x2, 0x2, 0x2, 0xa2, 0x550, 0x3, 0x2, 0x2, 0x2, 0xa4, 
    0x561, 0x3, 0x2, 0x2, 0x2, 0xa6, 0x577, 0x3, 0x2, 0x2, 0x2, 0xa8, 0x584, 
    0x3, 0x2, 0x2, 0x2, 0xaa, 0x586, 0x3, 0x2, 0x2, 0x2, 0xac, 0x596, 0x3, 
    0x2, 0x2, 0x2, 0xae, 0x598, 0x3, 0x2, 0x2, 0x2, 0xb0, 0x5a6, 0x3, 0x2, 
    0x2, 0x2, 0xb2, 0x5a8, 0x3, 0x2, 0x2, 0x2, 0xb4, 0x5bb, 0x3, 0x2, 0x2, 
    0x2, 0xb6, 0x5bd, 0x3, 0x2, 0x2, 0x2, 0xb8, 0x5cd, 0x3, 0x2, 0x2, 0x2, 
    0xba, 0x5cf, 0x3, 0x2, 0x2, 0x2, 0xbc, 0x5d7, 0x3, 0x2, 0x2, 0x2, 0xbe, 
    0x5de, 0x3, 0x2, 0x2, 0x2, 0xc0, 0x5e1, 0x3, 0x2, 0x2, 0x2, 0xc2, 0x5f1, 
    0x3, 0x2, 0x2, 0x2, 0xc4, 0x60b, 0x3, 0x2, 0x2, 0x2, 0xc6, 0x60d, 0x3, 
    0x2, 0x2, 0x2, 0xc8, 0x622, 0x3, 0x2, 0x2, 0x2, 0xca, 0x62f, 0x3, 0x2, 
    0x2, 0x2, 0xcc, 0x631, 0x3, 0x2, 0x2, 0x2, 0xce, 0x662, 0x3, 0x2, 0x2, 
    0x2, 0xd0, 0x666, 0x3, 0x2, 0x2, 0x2, 0xd2, 0x66a, 0x3, 0x2, 0x2, 0x2, 
    0xd4, 0x679, 0x3, 0x2, 0x2, 0x2, 0xd6, 0x683, 0x3, 0x2, 0x2, 0x2, 0xd8, 
    0x68f, 0x3, 0x2, 0x2, 0x2, 0xda, 0x69c, 0x3, 0x2, 0x2, 0x2, 0xdc, 0x6a0, 
    0x3, 0x2, 0x2, 0x2, 0xde, 0x6ac, 0x3, 0x2, 0x2, 0x2, 0xe0, 0x6bd, 0x3, 
    0x2, 0x2, 0x2, 0xe2, 0x6bf, 0x3, 0x2, 0x2, 0x2, 0xe4, 0x6de, 0x3, 0x2, 
    0x2, 0x2, 0xe6, 0x6f3, 0x3, 0x2, 0x2, 0x2, 0xe8, 0x700, 0x3, 0x2, 0x2, 
    0x2, 0xea, 0x71b, 0x3, 0x2, 0x2, 0x2, 0xec, 0x721, 0x3, 0x2, 0x2, 0x2, 
    0xee, 0xef, 0x9, 0x2, 0x2, 0x2, 0xef, 0x3, 0x3, 0x2, 0x2, 0x2, 0xf0, 
    0xf1, 0x7, 0x9, 0x2, 0x2, 0xf1, 0xf3, 0x7, 0x4d, 0x2, 0x2, 0xf2, 0xf0, 
    0x3, 0x2, 0x2, 0x2, 0xf2, 0xf3, 0x3, 0x2, 0x2, 0x2, 0xf3, 0xf4, 0x3, 
    0x2, 0x2, 0x2, 0xf4, 0x102, 0x9, 0x3, 0x2, 0x2, 0xf5, 0xf6, 0x7, 0x9, 
    0x2, 0x2, 0xf6, 0xf8, 0x7, 0x4d, 0x2, 0x2, 0xf7, 0xf5, 0x3, 0x2, 0x2, 
    0x2, 0xf7, 0xf8, 0x3, 0x2, 0x2, 0x2, 0xf8, 0xf9, 0x3, 0x2, 0x2, 0x2, 
    0xf9, 0xfe, 0x7, 0xc5, 0x2, 0x2, 0xfa, 0xfb, 0x7, 0x4d, 0x2, 0x2, 0xfb, 
    0xfd, 0x7, 0xc5, 0x2, 0x2, 0xfc, 0xfa, 0x3, 0x2, 0x2, 0x2, 0xfd, 0x100, 
    0x3, 0x2, 0x2, 0x2, 0xfe, 0xfc, 0x3, 0x2, 0x2, 0x2, 0xfe, 0xff, 0x3, 
    0x2, 0x2, 0x2, 0xff, 0x102, 0x3, 0x2, 0x2, 0x2, 0x100, 0xfe, 0x3, 0x2, 
    0x2, 0x2, 0x101, 0xf2, 0x3, 0x2, 0x2, 0x2, 0x101, 0xf7, 0x3, 0x2, 0x2, 
    0x2, 0x102, 0x5, 0x3, 0x2, 0x2, 0x2, 0x103, 0x108, 0x7, 0x9, 0x2, 0x2, 
    0x104, 0x105, 0x7, 0xa, 0x2, 0x2, 0x105, 0x109, 0x8, 0x4, 0x1, 0x2, 
    0x106, 0x107, 0x7, 0xb, 0x2, 0x2, 0x107, 0x109, 0x8, 0x4, 0x1, 0x2, 
    0x108, 0x104, 0x3, 0x2, 0x2, 0x2, 0x108, 0x106, 0x3, 0x2, 0x2, 0x2, 
    0x109, 0x10a, 0x3, 0x2, 0x2, 0x2, 0x10a, 0x10e, 0x7, 0x46, 0x2, 0x2, 
    0x10b, 0x10d, 0x5, 0x8, 0x5, 0x2, 0x10c, 0x10b, 0x3, 0x2, 0x2, 0x2, 
    0x10d, 0x110, 0x3, 0x2, 0x2, 0x2, 0x10e, 0x10c, 0x3, 0x2, 0x2, 0x2, 
    0x10e, 0x10f, 0x3, 0x2, 0x2, 0x2, 0x10f, 0x111, 0x3, 0x2, 0x2, 0x2, 
    0x110, 0x10e, 0x3, 0x2, 0x2, 0x2, 0x111, 0x112, 0x7, 0x2, 0x2, 0x3, 
    0x112, 0x7, 0x3, 0x2, 0x2, 0x2, 0x113, 0x115, 0x7, 0xc, 0x2, 0x2, 0x114, 
    0x113, 0x3, 0x2, 0x2, 0x2, 0x114, 0x115, 0x3, 0x2, 0x2, 0x2, 0x115, 
    0x119, 0x3, 0x2, 0x2, 0x2, 0x116, 0x11a, 0x7, 0xd, 0x2, 0x2, 0x117, 
    0x11a, 0x7, 0xe, 0x2, 0x2, 0x118, 0x11a, 0x7, 0xf, 0x2, 0x2, 0x119, 
    0x116, 0x3, 0x2, 0x2, 0x2, 0x119, 0x117, 0x3, 0x2, 0x2, 0x2, 0x119, 
    0x118, 0x3, 0x2, 0x2, 0x2, 0x119, 0x11a, 0x3, 0x2, 0x2, 0x2, 0x11a, 
    0x11b, 0x3, 0x2, 0x2, 0x2, 0x11b, 0x11c, 0x7, 0x10, 0x2, 0x2, 0x11c, 
    0x120, 0x7, 0xc5, 0x2, 0x2, 0x11d, 0x11e, 0x7, 0x33, 0x2, 0x2, 0x11e, 
    0x11f, 0x7, 0xc5, 0x2, 0x2, 0x11f, 0x121, 0x7, 0x34, 0x2, 0x2, 0x120, 
    0x11d, 0x3, 0x2, 0x2, 0x2, 0x120, 0x121, 0x3, 0x2, 0x2, 0x2, 0x121, 
    0x124, 0x3, 0x2, 0x2, 0x2, 0x122, 0x123, 0x6, 0x5, 0x2, 0x2, 0x123, 
    0x125, 0x7, 0x13, 0x2, 0x2, 0x124, 0x122, 0x3, 0x2, 0x2, 0x2, 0x124, 
    0x125, 0x3, 0x2, 0x2, 0x2, 0x125, 0x126, 0x3, 0x2, 0x2, 0x2, 0x126, 
    0x127, 0x7, 0x16, 0x2, 0x2, 0x127, 0x128, 0x7, 0xbf, 0x2, 0x2, 0x128, 
    0x129, 0x7, 0x12, 0x2, 0x2, 0x129, 0x12b, 0x7, 0xbf, 0x2, 0x2, 0x12a, 
    0x12c, 0x7, 0xbd, 0x2, 0x2, 0x12b, 0x12a, 0x3, 0x2, 0x2, 0x2, 0x12b, 
    0x12c, 0x3, 0x2, 0x2, 0x2, 0x12c, 0x133, 0x3, 0x2, 0x2, 0x2, 0x12d, 
    0x12e, 0x7, 0x14, 0x2, 0x2, 0x12e, 0x12f, 0x7, 0x15, 0x2, 0x2, 0x12f, 
    0x130, 0x7, 0xc5, 0x2, 0x2, 0x130, 0x131, 0x7, 0x35, 0x2, 0x2, 0x131, 
    0x132, 0x7, 0xbf, 0x2, 0x2, 0x132, 0x134, 0x7, 0x36, 0x2, 0x2, 0x133, 
    0x12d, 0x3, 0x2, 0x2, 0x2, 0x133, 0x134, 0x3, 0x2, 0x2, 0x2, 0x134, 
    0x135, 0x3, 0x2, 0x2, 0x2, 0x135, 0x13a, 0x7, 0x47, 0x2, 0x2, 0x136, 
    0x137, 0x6, 0x5, 0x3, 0x2, 0x137, 0x138, 0x7, 0x17, 0x2, 0x2, 0x138, 
    0x139, 0x7, 0xbf, 0x2, 0x2, 0x139, 0x13b, 0x7, 0x46, 0x2, 0x2, 0x13a, 
    0x136, 0x3, 0x2, 0x2, 0x2, 0x13a, 0x13b, 0x3, 0x2, 0x2, 0x2, 0x13b, 
    0x140, 0x3, 0x2, 0x2, 0x2, 0x13c, 0x13d, 0x6, 0x5, 0x4, 0x2, 0x13d, 
    0x13e, 0x7, 0x18, 0x2, 0x2, 0x13e, 0x13f, 0x7, 0xbf, 0x2, 0x2, 0x13f, 
    0x141, 0x7, 0x46, 0x2, 0x2, 0x140, 0x13c, 0x3, 0x2, 0x2, 0x2, 0x140, 
    0x141, 0x3, 0x2, 0x2, 0x2, 0x141, 0x145, 0x3, 0x2, 0x2, 0x2, 0x142, 
    0x144, 0x5, 0xa, 0x6, 0x2, 0x143, 0x142, 0x3, 0x2, 0x2, 0x2, 0x144, 
    0x147, 0x3, 0x2, 0x2, 0x2, 0x145, 0x143, 0x3, 0x2, 0x2, 0x2, 0x145, 
    0x146, 0x3, 0x2, 0x2, 0x2, 0x146, 0x155, 0x3, 0x2, 0x2, 0x2, 0x147, 
    0x145, 0x3, 0x2, 0x2, 0x2, 0x148, 0x154, 0x5, 0x84, 0x43, 0x2, 0x149, 
    0x154, 0x5, 0x7a, 0x3e, 0x2, 0x14a, 0x154, 0x5, 0xc6, 0x64, 0x2, 0x14b, 
    0x154, 0x5, 0x76, 0x3c, 0x2, 0x14c, 0x154, 0x5, 0x2e, 0x18, 0x2, 0x14d, 
    0x14e, 0x6, 0x5, 0x5, 0x2, 0x14e, 0x154, 0x5, 0x5c, 0x2f, 0x2, 0x14f, 
    0x154, 0x5, 0x8e, 0x48, 0x2, 0x150, 0x154, 0x5, 0x12, 0xa, 0x2, 0x151, 
    0x154, 0x5, 0x14, 0xb, 0x2, 0x152, 0x154, 0x5, 0xe, 0x8, 0x2, 0x153, 
    0x148, 0x3, 0x2, 0x2, 0x2, 0x153, 0x149, 0x3, 0x2, 0x2, 0x2, 0x153, 
    0x14a, 0x3, 0x2, 0x2, 0x2, 0x153, 0x14b, 0x3, 0x2, 0x2, 0x2, 0x153, 
    0x14c, 0x3, 0x2, 0x2, 0x2, 0x153, 0x14d, 0x3, 0x2, 0x2, 0x2, 0x153, 
    0x14f, 0x3, 0x2, 0x2, 0x2, 0x153, 0x150, 0x3, 0x2, 0x2, 0x2, 0x153, 
    0x151, 0x3, 0x2, 0x2, 0x2, 0x153, 0x152, 0x3, 0x2, 0x2, 0x2, 0x154, 
    0x157, 0x3, 0x2, 0x2, 0x2, 0x155, 0x153, 0x3, 0x2, 0x2, 0x2, 0x155, 
    0x156, 0x3, 0x2, 0x2, 0x2, 0x156, 0x158, 0x3, 0x2, 0x2, 0x2, 0x157, 
    0x155, 0x3, 0x2, 0x2, 0x2, 0x158, 0x159, 0x7, 0x11, 0x2, 0x2, 0x159, 
    0x15a, 0x7, 0xc5, 0x2, 0x2, 0x15a, 0x15b, 0x7, 0x4d, 0x2, 0x2, 0x15b, 
    0x9, 0x3, 0x2, 0x2, 0x2, 0x15c, 0x15d, 0x7, 0x19, 0x2, 0x2, 0x15d, 0x162, 
    0x5, 0xc, 0x7, 0x2, 0x15e, 0x15f, 0x7, 0x50, 0x2, 0x2, 0x15f, 0x161, 
    0x5, 0xc, 0x7, 0x2, 0x160, 0x15e, 0x3, 0x2, 0x2, 0x2, 0x161, 0x164, 
    0x3, 0x2, 0x2, 0x2, 0x162, 0x160, 0x3, 0x2, 0x2, 0x2, 0x162, 0x163, 
    0x3, 0x2, 0x2, 0x2, 0x163, 0x165, 0x3, 0x2, 0x2, 0x2, 0x164, 0x162, 
    0x3, 0x2, 0x2, 0x2, 0x165, 0x166, 0x7, 0x46, 0x2, 0x2, 0x166, 0xb, 0x3, 
    0x2, 0x2, 0x2, 0x167, 0x169, 0x7, 0x1a, 0x2, 0x2, 0x168, 0x167, 0x3, 
    0x2, 0x2, 0x2, 0x168, 0x169, 0x3, 0x2, 0x2, 0x2, 0x169, 0x16a, 0x3, 
    0x2, 0x2, 0x2, 0x16a, 0x16b, 0x9, 0x4, 0x2, 0x2, 0x16b, 0xd, 0x3, 0x2, 
    0x2, 0x2, 0x16c, 0x16e, 0x7, 0x1f, 0x2, 0x2, 0x16d, 0x16c, 0x3, 0x2, 
    0x2, 0x2, 0x16d, 0x16e, 0x3, 0x2, 0x2, 0x2, 0x16e, 0x16f, 0x3, 0x2, 
    0x2, 0x2, 0x16f, 0x170, 0x7, 0x1b, 0x2, 0x2, 0x170, 0x172, 0x7, 0xc5, 
    0x2, 0x2, 0x171, 0x173, 0x5, 0x8c, 0x47, 0x2, 0x172, 0x171, 0x3, 0x2, 
    0x2, 0x2, 0x172, 0x173, 0x3, 0x2, 0x2, 0x2, 0x173, 0x176, 0x3, 0x2, 
    0x2, 0x2, 0x174, 0x175, 0x7, 0x20, 0x2, 0x2, 0x175, 0x177, 0x5, 0x4, 
    0x3, 0x2, 0x176, 0x174, 0x3, 0x2, 0x2, 0x2, 0x176, 0x177, 0x3, 0x2, 
    0x2, 0x2, 0x177, 0x178, 0x3, 0x2, 0x2, 0x2, 0x178, 0x17f, 0x7, 0x47, 
    0x2, 0x2, 0x179, 0x17a, 0x7, 0x21, 0x2, 0x2, 0x17a, 0x17b, 0x7, 0x22, 
    0x2, 0x2, 0x17b, 0x17c, 0x7, 0x1e, 0x2, 0x2, 0x17c, 0x17d, 0x5, 0x4, 
    0x3, 0x2, 0x17d, 0x17e, 0x7, 0x46, 0x2, 0x2, 0x17e, 0x180, 0x3, 0x2, 
    0x2, 0x2, 0x17f, 0x179, 0x3, 0x2, 0x2, 0x2, 0x17f, 0x180, 0x3, 0x2, 
    0x2, 0x2, 0x180, 0x186, 0x3, 0x2, 0x2, 0x2, 0x181, 0x182, 0x7, 0x22, 
    0x2, 0x2, 0x182, 0x183, 0x7, 0x1e, 0x2, 0x2, 0x183, 0x184, 0x5, 0x4, 
    0x3, 0x2, 0x184, 0x185, 0x7, 0x46, 0x2, 0x2, 0x185, 0x187, 0x3, 0x2, 
    0x2, 0x2, 0x186, 0x181, 0x3, 0x2, 0x2, 0x2, 0x186, 0x187, 0x3, 0x2, 
    0x2, 0x2, 0x187, 0x196, 0x3, 0x2, 0x2, 0x2, 0x188, 0x189, 0x7, 0x1c, 
    0x2, 0x2, 0x189, 0x18a, 0x7, 0x1d, 0x2, 0x2, 0x18a, 0x18f, 0x5, 0x10, 
    0x9, 0x2, 0x18b, 0x18c, 0x7, 0x50, 0x2, 0x2, 0x18c, 0x18e, 0x5, 0x10, 
    0x9, 0x2, 0x18d, 0x18b, 0x3, 0x2, 0x2, 0x2, 0x18e, 0x191, 0x3, 0x2, 
    0x2, 0x2, 0x18f, 0x18d, 0x3, 0x2, 0x2, 0x2, 0x18f, 0x190, 0x3, 0x2, 
    0x2, 0x2, 0x190, 0x192, 0x3, 0x2, 0x2, 0x2, 0x191, 0x18f, 0x3, 0x2, 
    0x2, 0x2, 0x192, 0x193, 0x7, 0x46, 0x2, 0x2, 0x193, 0x195, 0x3, 0x2, 
    0x2, 0x2, 0x194, 0x188, 0x3, 0x2, 0x2, 0x2, 0x195, 0x198, 0x3, 0x2, 
    0x2, 0x2, 0x196, 0x194, 0x3, 0x2, 0x2, 0x2, 0x196, 0x197, 0x3, 0x2, 
    0x2, 0x2, 0x197, 0x1a6, 0x3, 0x2, 0x2, 0x2, 0x198, 0x196, 0x3, 0x2, 
    0x2, 0x2, 0x199, 0x19a, 0x6, 0x8, 0x6, 0x2, 0x19a, 0x19b, 0x7, 0x5d, 
    0x2, 0x2, 0x19b, 0x19c, 0x7, 0x5c, 0x2, 0x2, 0x19c, 0x1a1, 0x5, 0x4, 
    0x3, 0x2, 0x19d, 0x19e, 0x7, 0x50, 0x2, 0x2, 0x19e, 0x1a0, 0x5, 0x4, 
    0x3, 0x2, 0x19f, 0x19d, 0x3, 0x2, 0x2, 0x2, 0x1a0, 0x1a3, 0x3, 0x2, 
    0x2, 0x2, 0x1a1, 0x19f, 0x3, 0x2, 0x2, 0x2, 0x1a1, 0x1a2, 0x3, 0x2, 
    0x2, 0x2, 0x1a2, 0x1a4, 0x3, 0x2, 0x2, 0x2, 0x1a3, 0x1a1, 0x3, 0x2, 
    0x2, 0x2, 0x1a4, 0x1a5, 0x7, 0x46, 0x2, 0x2, 0x1a5, 0x1a7, 0x3, 0x2, 
    0x2, 0x2, 0x1a6, 0x199, 0x3, 0x2, 0x2, 0x2, 0x1a6, 0x1a7, 0x3, 0x2, 
    0x2, 0x2, 0x1a7, 0x1b4, 0x3, 0x2, 0x2, 0x2, 0x1a8, 0x1b3, 0x5, 0x84, 
    0x43, 0x2, 0x1a9, 0x1b3, 0x5, 0x7a, 0x3e, 0x2, 0x1aa, 0x1b3, 0x5, 0xc6, 
    0x64, 0x2, 0x1ab, 0x1b3, 0x5, 0x2e, 0x18, 0x2, 0x1ac, 0x1b3, 0x5, 0x12, 
    0xa, 0x2, 0x1ad, 0x1b3, 0x5, 0x14, 0xb, 0x2, 0x1ae, 0x1b3, 0x5, 0x26, 
    0x14, 0x2, 0x1af, 0x1b3, 0x5, 0xa6, 0x54, 0x2, 0x1b0, 0x1b3, 0x5, 0xcc, 
    0x67, 0x2, 0x1b1, 0x1b3, 0x5, 0xe2, 0x72, 0x2, 0x1b2, 0x1a8, 0x3, 0x2, 
    0x2, 0x2, 0x1b2, 0x1a9, 0x3, 0x2, 0x2, 0x2, 0x1b2, 0x1aa, 0x3, 0x2, 
    0x2, 0x2, 0x1b2, 0x1ab, 0x3, 0x2, 0x2, 0x2, 0x1b2, 0x1ac, 0x3, 0x2, 
    0x2, 0x2, 0x1b2, 0x1ad, 0x3, 0x2, 0x2, 0x2, 0x1b2, 0x1ae, 0x3, 0x2, 
    0x2, 0x2, 0x1b2, 0x1af, 0x3, 0x2, 0x2, 0x2, 0x1b2, 0x1b0, 0x3, 0x2, 
    0x2, 0x2, 0x1b2, 0x1b1, 0x3, 0x2, 0x2, 0x2, 0x1b3, 0x1b6, 0x3, 0x2, 
    0x2, 0x2, 0x1b4, 0x1b2, 0x3, 0x2, 0x2, 0x2, 0x1b4, 0x1b5, 0x3, 0x2, 
    0x2, 0x2, 0x1b5, 0x1b7, 0x3, 0x2, 0x2, 0x2, 0x1b6, 0x1b4, 0x3, 0x2, 
    0x2, 0x2, 0x1b7, 0x1b8, 0x7, 0x11, 0x2, 0x2, 0x1b8, 0x1b9, 0x7, 0xc5, 
    0x2, 0x2, 0x1b9, 0x1ba, 0x7, 0x46, 0x2, 0x2, 0x1ba, 0xf, 0x3, 0x2, 0x2, 
    0x2, 0x1bb, 0x1bc, 0x5, 0x4, 0x3, 0x2, 0x1bc, 0x11, 0x3, 0x2, 0x2, 0x2, 
    0x1bd, 0x1be, 0x7, 0x26, 0x2, 0x2, 0x1be, 0x1c0, 0x7, 0xc5, 0x2, 0x2, 
    0x1bf, 0x1c1, 0x5, 0x8c, 0x47, 0x2, 0x1c0, 0x1bf, 0x3, 0x2, 0x2, 0x2, 
    0x1c0, 0x1c1, 0x3, 0x2, 0x2, 0x2, 0x1c1, 0x1c4, 0x3, 0x2, 0x2, 0x2, 
    0x1c2, 0x1c3, 0x7, 0x20, 0x2, 0x2, 0x1c3, 0x1c5, 0x5, 0x4, 0x3, 0x2, 
    0x1c4, 0x1c2, 0x3, 0x2, 0x2, 0x2, 0x1c4, 0x1c5, 0x3, 0x2, 0x2, 0x2, 
    0x1c5, 0x1c6, 0x3, 0x2, 0x2, 0x2, 0x1c6, 0x1cf, 0x7, 0x47, 0x2, 0x2, 
    0x1c7, 0x1c8, 0x7, 0x22, 0x2, 0x2, 0x1c8, 0x1c9, 0x7, 0x1e, 0x2, 0x2, 
    0x1c9, 0x1cd, 0x5, 0x4, 0x3, 0x2, 0x1ca, 0x1cb, 0x7, 0x2d, 0x2, 0x2, 
    0x1cb, 0x1cd, 0x7, 0x22, 0x2, 0x2, 0x1cc, 0x1c7, 0x3, 0x2, 0x2, 0x2, 
    0x1cc, 0x1ca, 0x3, 0x2, 0x2, 0x2, 0x1cd, 0x1ce, 0x3, 0x2, 0x2, 0x2, 
    0x1ce, 0x1d0, 0x7, 0x46, 0x2, 0x2, 0x1cf, 0x1cc, 0x3, 0x2, 0x2, 0x2, 
    0x1cf, 0x1d0, 0x3, 0x2, 0x2, 0x2, 0x1d0, 0x1d1, 0x3, 0x2, 0x2, 0x2, 
    0x1d1, 0x1d2, 0x5, 0x16, 0xc, 0x2, 0x1d2, 0x1d3, 0x7, 0x11, 0x2, 0x2, 
    0x1d3, 0x1d4, 0x7, 0xc5, 0x2, 0x2, 0x1d4, 0x1d5, 0x7, 0x46, 0x2, 0x2, 
    0x1d5, 0x13, 0x3, 0x2, 0x2, 0x2, 0x1d6, 0x1d7, 0x7, 0x58, 0x2, 0x2, 
    0x1d7, 0x1d9, 0x7, 0xc5, 0x2, 0x2, 0x1d8, 0x1da, 0x5, 0x8c, 0x47, 0x2, 
    0x1d9, 0x1d8, 0x3, 0x2, 0x2, 0x2, 0x1d9, 0x1da, 0x3, 0x2, 0x2, 0x2, 
    0x1da, 0x1dd, 0x3, 0x2, 0x2, 0x2, 0x1db, 0x1dc, 0x7, 0x20, 0x2, 0x2, 
    0x1dc, 0x1de, 0x5, 0x4, 0x3, 0x2, 0x1dd, 0x1db, 0x3, 0x2, 0x2, 0x2, 
    0x1dd, 0x1de, 0x3, 0x2, 0x2, 0x2, 0x1de, 0x1df, 0x3, 0x2, 0x2, 0x2, 
    0x1df, 0x1e0, 0x7, 0x47, 0x2, 0x2, 0x1e0, 0x1e1, 0x5, 0x16, 0xc, 0x2, 
    0x1e1, 0x1e2, 0x7, 0x11, 0x2, 0x2, 0x1e2, 0x1e3, 0x7, 0xc5, 0x2, 0x2, 
    0x1e3, 0x1e4, 0x7, 0x46, 0x2, 0x2, 0x1e4, 0x15, 0x3, 0x2, 0x2, 0x2, 
    0x1e5, 0x1e7, 0x7, 0x2e, 0x2, 0x2, 0x1e6, 0x1e5, 0x3, 0x2, 0x2, 0x2, 
    0x1e6, 0x1e7, 0x3, 0x2, 0x2, 0x2, 0x1e7, 0x1eb, 0x3, 0x2, 0x2, 0x2, 
    0x1e8, 0x1ea, 0x5, 0x18, 0xd, 0x2, 0x1e9, 0x1e8, 0x3, 0x2, 0x2, 0x2, 
    0x1ea, 0x1ed, 0x3, 0x2, 0x2, 0x2, 0x1eb, 0x1e9, 0x3, 0x2, 0x2, 0x2, 
    0x1eb, 0x1ec, 0x3, 0x2, 0x2, 0x2, 0x1ec, 0x1f1, 0x3, 0x2, 0x2, 0x2, 
    0x1ed, 0x1eb, 0x3, 0x2, 0x2, 0x2, 0x1ee, 0x1f0, 0x5, 0x92, 0x4a, 0x2, 
    0x1ef, 0x1ee, 0x3, 0x2, 0x2, 0x2, 0x1f0, 0x1f3, 0x3, 0x2, 0x2, 0x2, 
    0x1f1, 0x1ef, 0x3, 0x2, 0x2, 0x2, 0x1f1, 0x1f2, 0x3, 0x2, 0x2, 0x2, 
    0x1f2, 0x1fb, 0x3, 0x2, 0x2, 0x2, 0x1f3, 0x1f1, 0x3, 0x2, 0x2, 0x2, 
    0x1f4, 0x1f8, 0x7, 0x59, 0x2, 0x2, 0x1f5, 0x1f7, 0x5, 0x8a, 0x46, 0x2, 
    0x1f6, 0x1f5, 0x3, 0x2, 0x2, 0x2, 0x1f7, 0x1fa, 0x3, 0x2, 0x2, 0x2, 
    0x1f8, 0x1f6, 0x3, 0x2, 0x2, 0x2, 0x1f8, 0x1f9, 0x3, 0x2, 0x2, 0x2, 
    0x1f9, 0x1fc, 0x3, 0x2, 0x2, 0x2, 0x1fa, 0x1f8, 0x3, 0x2, 0x2, 0x2, 
    0x1fb, 0x1f4, 0x3, 0x2, 0x2, 0x2, 0x1fb, 0x1fc, 0x3, 0x2, 0x2, 0x2, 
    0x1fc, 0x17, 0x3, 0x2, 0x2, 0x2, 0x1fd, 0x1ff, 0x7, 0x2b, 0x2, 0x2, 
    0x1fe, 0x1fd, 0x3, 0x2, 0x2, 0x2, 0x1fe, 0x1ff, 0x3, 0x2, 0x2, 0x2, 
    0x1ff, 0x200, 0x3, 0x2, 0x2, 0x2, 0x200, 0x202, 0x7, 0x2c, 0x2, 0x2, 
    0x201, 0x1fe, 0x3, 0x2, 0x2, 0x2, 0x201, 0x202, 0x3, 0x2, 0x2, 0x2, 
    0x202, 0x203, 0x3, 0x2, 0x2, 0x2, 0x203, 0x205, 0x7, 0xc5, 0x2, 0x2, 
    0x204, 0x206, 0x5, 0x8c, 0x47, 0x2, 0x205, 0x204, 0x3, 0x2, 0x2, 0x2, 
    0x205, 0x206, 0x3, 0x2, 0x2, 0x2, 0x206, 0x207, 0x3, 0x2, 0x2, 0x2, 
    0x207, 0x208, 0x7, 0x4f, 0x2, 0x2, 0x208, 0x212, 0x5, 0x1a, 0xe, 0x2, 
    0x209, 0x20a, 0x7, 0x4c, 0x2, 0x2, 0x20a, 0x20f, 0x5, 0xb8, 0x5d, 0x2, 
    0x20b, 0x20c, 0x7, 0x50, 0x2, 0x2, 0x20c, 0x20e, 0x5, 0xb8, 0x5d, 0x2, 
    0x20d, 0x20b, 0x3, 0x2, 0x2, 0x2, 0x20e, 0x211, 0x3, 0x2, 0x2, 0x2, 
    0x20f, 0x20d, 0x3, 0x2, 0x2, 0x2, 0x20f, 0x210, 0x3, 0x2, 0x2, 0x2, 
    0x210, 0x213, 0x3, 0x2, 0x2, 0x2, 0x211, 0x20f, 0x3, 0x2, 0x2, 0x2, 
    0x212, 0x209, 0x3, 0x2, 0x2, 0x2, 0x212, 0x213, 0x3, 0x2, 0x2, 0x2, 
    0x213, 0x214, 0x3, 0x2, 0x2, 0x2, 0x214, 0x215, 0x7, 0x46, 0x2, 0x2, 
    0x215, 0x19, 0x3, 0x2, 0x2, 0x2, 0x216, 0x218, 0x7, 0x5a, 0x2, 0x2, 
    0x217, 0x219, 0x5, 0x1e, 0x10, 0x2, 0x218, 0x217, 0x3, 0x2, 0x2, 0x2, 
    0x218, 0x219, 0x3, 0x2, 0x2, 0x2, 0x219, 0x21d, 0x3, 0x2, 0x2, 0x2, 
    0x21a, 0x21d, 0x5, 0x1e, 0x10, 0x2, 0x21b, 0x21d, 0x5, 0x1c, 0xf, 0x2, 
    0x21c, 0x216, 0x3, 0x2, 0x2, 0x2, 0x21c, 0x21a, 0x3, 0x2, 0x2, 0x2, 
    0x21c, 0x21b, 0x3, 0x2, 0x2, 0x2, 0x21d, 0x1b, 0x3, 0x2, 0x2, 0x2, 0x21e, 
    0x220, 0x9, 0x5, 0x2, 0x2, 0x21f, 0x221, 0x5, 0x2c, 0x17, 0x2, 0x220, 
    0x21f, 0x3, 0x2, 0x2, 0x2, 0x220, 0x221, 0x3, 0x2, 0x2, 0x2, 0x221, 
    0x222, 0x3, 0x2, 0x2, 0x2, 0x222, 0x227, 0x7, 0x15, 0x2, 0x2, 0x223, 
    0x224, 0x6, 0xf, 0x7, 0x2, 0x224, 0x228, 0x5, 0x22, 0x12, 0x2, 0x225, 
    0x226, 0x6, 0xf, 0x8, 0x2, 0x226, 0x228, 0x5, 0x1e, 0x10, 0x2, 0x227, 
    0x223, 0x3, 0x2, 0x2, 0x2, 0x227, 0x225, 0x3, 0x2, 0x2, 0x2, 0x228, 
    0x1d, 0x3, 0x2, 0x2, 0x2, 0x229, 0x22e, 0x5, 0x32, 0x1a, 0x2, 0x22a, 
    0x22e, 0x5, 0x4, 0x3, 0x2, 0x22b, 0x22e, 0x5, 0x20, 0x11, 0x2, 0x22c, 
    0x22e, 0x5, 0x22, 0x12, 0x2, 0x22d, 0x229, 0x3, 0x2, 0x2, 0x2, 0x22d, 
    0x22a, 0x3, 0x2, 0x2, 0x2, 0x22d, 0x22b, 0x3, 0x2, 0x2, 0x2, 0x22d, 
    0x22c, 0x3, 0x2, 0x2, 0x2, 0x22e, 0x1f, 0x3, 0x2, 0x2, 0x2, 0x22f, 0x230, 
    0x7, 0x65, 0x2, 0x2, 0x230, 0x234, 0x7, 0x66, 0x2, 0x2, 0x231, 0x232, 
    0x7, 0x33, 0x2, 0x2, 0x232, 0x233, 0x7, 0x6b, 0x2, 0x2, 0x233, 0x235, 
    0x7, 0x34, 0x2, 0x2, 0x234, 0x231, 0x3, 0x2, 0x2, 0x2, 0x234, 0x235, 
    0x3, 0x2, 0x2, 0x2, 0x235, 0x236, 0x3, 0x2, 0x2, 0x2, 0x236, 0x237, 
    0x5, 0x22, 0x12, 0x2, 0x237, 0x21, 0x3, 0x2, 0x2, 0x2, 0x238, 0x23c, 
    0x5, 0x4, 0x3, 0x2, 0x239, 0x23c, 0x7, 0x67, 0x2, 0x2, 0x23a, 0x23c, 
    0x7, 0xbb, 0x2, 0x2, 0x23b, 0x238, 0x3, 0x2, 0x2, 0x2, 0x23b, 0x239, 
    0x3, 0x2, 0x2, 0x2, 0x23b, 0x23a, 0x3, 0x2, 0x2, 0x2, 0x23c, 0x23e, 
    0x3, 0x2, 0x2, 0x2, 0x23d, 0x23f, 0x5, 0x24, 0x13, 0x2, 0x23e, 0x23d, 
    0x3, 0x2, 0x2, 0x2, 0x23e, 0x23f, 0x3, 0x2, 0x2, 0x2, 0x23f, 0x23, 0x3, 
    0x2, 0x2, 0x2, 0x240, 0x241, 0x7, 0x68, 0x2, 0x2, 0x241, 0x242, 0x7, 
    0x33, 0x2, 0x2, 0x242, 0x247, 0x5, 0x4, 0x3, 0x2, 0x243, 0x244, 0x7, 
    0x46, 0x2, 0x2, 0x244, 0x246, 0x5, 0x4, 0x3, 0x2, 0x245, 0x243, 0x3, 
    0x2, 0x2, 0x2, 0x246, 0x249, 0x3, 0x2, 0x2, 0x2, 0x247, 0x245, 0x3, 
    0x2, 0x2, 0x2, 0x247, 0x248, 0x3, 0x2, 0x2, 0x2, 0x248, 0x24a, 0x3, 
    0x2, 0x2, 0x2, 0x249, 0x247, 0x3, 0x2, 0x2, 0x2, 0x24a, 0x24b, 0x7, 
    0x34, 0x2, 0x2, 0x24b, 0x25, 0x3, 0x2, 0x2, 0x2, 0x24c, 0x24e, 0x7, 
    0x69, 0x2, 0x2, 0x24d, 0x24f, 0x7, 0xc5, 0x2, 0x2, 0x24e, 0x24d, 0x3, 
    0x2, 0x2, 0x2, 0x24e, 0x24f, 0x3, 0x2, 0x2, 0x2, 0x24f, 0x251, 0x3, 
    0x2, 0x2, 0x2, 0x250, 0x252, 0x5, 0x8c, 0x47, 0x2, 0x251, 0x250, 0x3, 
    0x2, 0x2, 0x2, 0x251, 0x252, 0x3, 0x2, 0x2, 0x2, 0x252, 0x255, 0x3, 
    0x2, 0x2, 0x2, 0x253, 0x254, 0x7, 0x20, 0x2, 0x2, 0x254, 0x256, 0x5, 
    0x28, 0x15, 0x2, 0x255, 0x253, 0x3, 0x2, 0x2, 0x2, 0x255, 0x256, 0x3, 
    0x2, 0x2, 0x2, 0x256, 0x25a, 0x3, 0x2, 0x2, 0x2, 0x257, 0x258, 0x7, 
    0x6a, 0x2, 0x2, 0x258, 0x259, 0x7, 0x6c, 0x2, 0x2, 0x259, 0x25b, 0x5, 
    0xda, 0x6e, 0x2, 0x25a, 0x257, 0x3, 0x2, 0x2, 0x2, 0x25a, 0x25b, 0x3, 
    0x2, 0x2, 0x2, 0x25b, 0x25c, 0x3, 0x2, 0x2, 0x2, 0x25c, 0x265, 0x7, 
    0x47, 0x2, 0x2, 0x25d, 0x25e, 0x7, 0x22, 0x2, 0x2, 0x25e, 0x25f, 0x7, 
    0x1e, 0x2, 0x2, 0x25f, 0x263, 0x5, 0x4, 0x3, 0x2, 0x260, 0x261, 0x7, 
    0x2d, 0x2, 0x2, 0x261, 0x263, 0x7, 0x22, 0x2, 0x2, 0x262, 0x25d, 0x3, 
    0x2, 0x2, 0x2, 0x262, 0x260, 0x3, 0x2, 0x2, 0x2, 0x263, 0x264, 0x3, 
    0x2, 0x2, 0x2, 0x264, 0x266, 0x7, 0x46, 0x2, 0x2, 0x265, 0x262, 0x3, 
    0x2, 0x2, 0x2, 0x265, 0x266, 0x3, 0x2, 0x2, 0x2, 0x266, 0x26a, 0x3, 
    0x2, 0x2, 0x2, 0x267, 0x269, 0x5, 0x2a, 0x16, 0x2, 0x268, 0x267, 0x3, 
    0x2, 0x2, 0x2, 0x269, 0x26c, 0x3, 0x2, 0x2, 0x2, 0x26a, 0x268, 0x3, 
    0x2, 0x2, 0x2, 0x26a, 0x26b, 0x3, 0x2, 0x2, 0x2, 0x26b, 0x26e, 0x3, 
    0x2, 0x2, 0x2, 0x26c, 0x26a, 0x3, 0x2, 0x2, 0x2, 0x26d, 0x26f, 0x7, 
    0x2e, 0x2, 0x2, 0x26e, 0x26d, 0x3, 0x2, 0x2, 0x2, 0x26e, 0x26f, 0x3, 
    0x2, 0x2, 0x2, 0x26f, 0x273, 0x3, 0x2, 0x2, 0x2, 0x270, 0x272, 0x5, 
    0x18, 0xd, 0x2, 0x271, 0x270, 0x3, 0x2, 0x2, 0x2, 0x272, 0x275, 0x3, 
    0x2, 0x2, 0x2, 0x273, 0x271, 0x3, 0x2, 0x2, 0x2, 0x273, 0x274, 0x3, 
    0x2, 0x2, 0x2, 0x274, 0x27b, 0x3, 0x2, 0x2, 0x2, 0x275, 0x273, 0x3, 
    0x2, 0x2, 0x2, 0x276, 0x277, 0x7, 0x77, 0x2, 0x2, 0x277, 0x278, 0x7, 
    0x47, 0x2, 0x2, 0x278, 0x279, 0x5, 0x2c, 0x17, 0x2, 0x279, 0x27a, 0x7, 
    0x46, 0x2, 0x2, 0x27a, 0x27c, 0x3, 0x2, 0x2, 0x2, 0x27b, 0x276, 0x3, 
    0x2, 0x2, 0x2, 0x27b, 0x27c, 0x3, 0x2, 0x2, 0x2, 0x27c, 0x280, 0x3, 
    0x2, 0x2, 0x2, 0x27d, 0x27f, 0x5, 0x92, 0x4a, 0x2, 0x27e, 0x27d, 0x3, 
    0x2, 0x2, 0x2, 0x27f, 0x282, 0x3, 0x2, 0x2, 0x2, 0x280, 0x27e, 0x3, 
    0x2, 0x2, 0x2, 0x280, 0x281, 0x3, 0x2, 0x2, 0x2, 0x281, 0x283, 0x3, 
    0x2, 0x2, 0x2, 0x282, 0x280, 0x3, 0x2, 0x2, 0x2, 0x283, 0x285, 0x7, 
    0x11, 0x2, 0x2, 0x284, 0x286, 0x7, 0xc5, 0x2, 0x2, 0x285, 0x284, 0x3, 
    0x2, 0x2, 0x2, 0x285, 0x286, 0x3, 0x2, 0x2, 0x2, 0x286, 0x287, 0x3, 
    0x2, 0x2, 0x2, 0x287, 0x288, 0x7, 0x46, 0x2, 0x2, 0x288, 0x27, 0x3, 
    0x2, 0x2, 0x2, 0x289, 0x28a, 0x7, 0xc5, 0x2, 0x2, 0x28a, 0x28c, 0x7, 
    0x4d, 0x2, 0x2, 0x28b, 0x289, 0x3, 0x2, 0x2, 0x2, 0x28b, 0x28c, 0x3, 
    0x2, 0x2, 0x2, 0x28c, 0x28d, 0x3, 0x2, 0x2, 0x2, 0x28d, 0x28e, 0x7, 
    0xc5, 0x2, 0x2, 0x28e, 0x290, 0x7, 0x4d, 0x2, 0x2, 0x28f, 0x28b, 0x3, 
    0x2, 0x2, 0x2, 0x28f, 0x290, 0x3, 0x2, 0x2, 0x2, 0x290, 0x291, 0x3, 
    0x2, 0x2, 0x2, 0x291, 0x292, 0x7, 0xc5, 0x2, 0x2, 0x292, 0x29, 0x3, 
    0x2, 0x2, 0x2, 0x293, 0x295, 0x7, 0xc5, 0x2, 0x2, 0x294, 0x296, 0x5, 
    0x8c, 0x47, 0x2, 0x295, 0x294, 0x3, 0x2, 0x2, 0x2, 0x295, 0x296, 0x3, 
    0x2, 0x2, 0x2, 0x296, 0x297, 0x3, 0x2, 0x2, 0x2, 0x297, 0x299, 0x9, 
    0x6, 0x2, 0x2, 0x298, 0x29a, 0x5, 0x2c, 0x17, 0x2, 0x299, 0x298, 0x3, 
    0x2, 0x2, 0x2, 0x299, 0x29a, 0x3, 0x2, 0x2, 0x2, 0x29a, 0x29b, 0x3, 
    0x2, 0x2, 0x2, 0x29b, 0x2a0, 0x5, 0x22, 0x12, 0x2, 0x29c, 0x29d, 0x7, 
    0x78, 0x2, 0x2, 0x29d, 0x29f, 0x5, 0x22, 0x12, 0x2, 0x29e, 0x29c, 0x3, 
    0x2, 0x2, 0x2, 0x29f, 0x2a2, 0x3, 0x2, 0x2, 0x2, 0x2a0, 0x29e, 0x3, 
    0x2, 0x2, 0x2, 0x2a0, 0x2a1, 0x3, 0x2, 0x2, 0x2, 0x2a1, 0x2a5, 0x3, 
    0x2, 0x2, 0x2, 0x2a2, 0x2a0, 0x3, 0x2, 0x2, 0x2, 0x2a3, 0x2a4, 0x7, 
    0x4c, 0x2, 0x2, 0x2a4, 0x2a6, 0x5, 0xb8, 0x5d, 0x2, 0x2a5, 0x2a3, 0x3, 
    0x2, 0x2, 0x2, 0x2a5, 0x2a6, 0x3, 0x2, 0x2, 0x2, 0x2a6, 0x2a7, 0x3, 
    0x2, 0x2, 0x2, 0x2a7, 0x2a8, 0x7, 0x46, 0x2, 0x2, 0x2a8, 0x2b, 0x3, 
    0x2, 0x2, 0x2, 0x2a9, 0x2b3, 0x7, 0x37, 0x2, 0x2, 0x2aa, 0x2b4, 0x7, 
    0x39, 0x2, 0x2, 0x2ab, 0x2b1, 0x7, 0xc0, 0x2, 0x2, 0x2ac, 0x2af, 0x7, 
    0x4e, 0x2, 0x2, 0x2ad, 0x2b0, 0x7, 0xc0, 0x2, 0x2, 0x2ae, 0x2b0, 0x7, 
    0x39, 0x2, 0x2, 0x2af, 0x2ad, 0x3, 0x2, 0x2, 0x2, 0x2af, 0x2ae, 0x3, 
    0x2, 0x2, 0x2, 0x2b0, 0x2b2, 0x3, 0x2, 0x2, 0x2, 0x2b1, 0x2ac, 0x3, 
    0x2, 0x2, 0x2, 0x2b1, 0x2b2, 0x3, 0x2, 0x2, 0x2, 0x2b2, 0x2b4, 0x3, 
    0x2, 0x2, 0x2, 0x2b3, 0x2aa, 0x3, 0x2, 0x2, 0x2, 0x2b3, 0x2ab, 0x3, 
    0x2, 0x2, 0x2, 0x2b4, 0x2b5, 0x3, 0x2, 0x2, 0x2, 0x2b5, 0x2b6, 0x7, 
    0x38, 0x2, 0x2, 0x2b6, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x2b7, 0x2bb, 0x7, 
    0x7b, 0x2, 0x2, 0x2b8, 0x2ba, 0x5, 0x30, 0x19, 0x2, 0x2b9, 0x2b8, 0x3, 
    0x2, 0x2, 0x2, 0x2ba, 0x2bd, 0x3, 0x2, 0x2, 0x2, 0x2bb, 0x2b9, 0x3, 
    0x2, 0x2, 0x2, 0x2bb, 0x2bc, 0x3, 0x2, 0x2, 0x2, 0x2bc, 0x2f, 0x3, 0x2, 
    0x2, 0x2, 0x2bd, 0x2bb, 0x3, 0x2, 0x2, 0x2, 0x2be, 0x2c0, 0x7, 0xc5, 
    0x2, 0x2, 0x2bf, 0x2c1, 0x5, 0x8c, 0x47, 0x2, 0x2c0, 0x2bf, 0x3, 0x2, 
    0x2, 0x2, 0x2c0, 0x2c1, 0x3, 0x2, 0x2, 0x2, 0x2c1, 0x2c4, 0x3, 0x2, 
    0x2, 0x2, 0x2c2, 0x2c3, 0x7, 0x20, 0x2, 0x2, 0x2c3, 0x2c5, 0x5, 0x4, 
    0x3, 0x2, 0x2c4, 0x2c2, 0x3, 0x2, 0x2, 0x2, 0x2c4, 0x2c5, 0x3, 0x2, 
    0x2, 0x2, 0x2c5, 0x2c6, 0x3, 0x2, 0x2, 0x2, 0x2c6, 0x2cc, 0x7, 0x47, 
    0x2, 0x2, 0x2c7, 0x2c9, 0x7, 0x5a, 0x2, 0x2, 0x2c8, 0x2ca, 0x5, 0x32, 
    0x1a, 0x2, 0x2c9, 0x2c8, 0x3, 0x2, 0x2, 0x2, 0x2c9, 0x2ca, 0x3, 0x2, 
    0x2, 0x2, 0x2ca, 0x2cd, 0x3, 0x2, 0x2, 0x2, 0x2cb, 0x2cd, 0x5, 0x32, 
    0x1a, 0x2, 0x2cc, 0x2c7, 0x3, 0x2, 0x2, 0x2, 0x2cc, 0x2cb, 0x3, 0x2, 
    0x2, 0x2, 0x2cd, 0x2dc, 0x3, 0x2, 0x2, 0x2, 0x2ce, 0x2cf, 0x6, 0x19, 
    0x9, 0x2, 0x2cf, 0x2d0, 0x7, 0x99, 0x2, 0x2, 0x2d0, 0x2d1, 0x7, 0xc5, 
    0x2, 0x2, 0x2d1, 0x2d2, 0x7, 0x4f, 0x2, 0x2, 0x2d2, 0x2d9, 0x5, 0xa8, 
    0x55, 0x2, 0x2d3, 0x2d4, 0x7, 0x50, 0x2, 0x2, 0x2d4, 0x2d5, 0x7, 0xc5, 
    0x2, 0x2, 0x2d5, 0x2d6, 0x7, 0x4f, 0x2, 0x2, 0x2d6, 0x2d8, 0x5, 0xa8, 
    0x55, 0x2, 0x2d7, 0x2d3, 0x3, 0x2, 0x2, 0x2, 0x2d8, 0x2db, 0x3, 0x2, 
    0x2, 0x2, 0x2d9, 0x2d7, 0x3, 0x2, 0x2, 0x2, 0x2d9, 0x2da, 0x3, 0x2, 
    0x2, 0x2, 0x2da, 0x2dd, 0x3, 0x2, 0x2, 0x2, 0x2db, 0x2d9, 0x3, 0x2, 
    0x2, 0x2, 0x2dc, 0x2ce, 0x3, 0x2, 0x2, 0x2, 0x2dc, 0x2dd, 0x3, 0x2, 
    0x2, 0x2, 0x2dd, 0x2de, 0x3, 0x2, 0x2, 0x2, 0x2de, 0x2df, 0x7, 0x46, 
    0x2, 0x2, 0x2df, 0x31, 0x3, 0x2, 0x2, 0x2, 0x2e0, 0x2e3, 0x5, 0x34, 
    0x1b, 0x2, 0x2e1, 0x2e3, 0x5, 0x70, 0x39, 0x2, 0x2e2, 0x2e0, 0x3, 0x2, 
    0x2, 0x2, 0x2e2, 0x2e1, 0x3, 0x2, 0x2, 0x2, 0x2e3, 0x33, 0x3, 0x2, 0x2, 
    0x2, 0x2e4, 0x2f3, 0x5, 0x38, 0x1d, 0x2, 0x2e5, 0x2f3, 0x5, 0x3c, 0x1f, 
    0x2, 0x2e6, 0x2f3, 0x5, 0x3e, 0x20, 0x2, 0x2e7, 0x2f3, 0x5, 0x40, 0x21, 
    0x2, 0x2e8, 0x2f3, 0x5, 0x42, 0x22, 0x2, 0x2e9, 0x2f3, 0x5, 0x4a, 0x26, 
    0x2, 0x2ea, 0x2f3, 0x5, 0x54, 0x2b, 0x2, 0x2eb, 0x2ec, 0x6, 0x1b, 0xa, 
    0x2, 0x2ec, 0x2f3, 0x5, 0x4c, 0x27, 0x2, 0x2ed, 0x2f3, 0x5, 0x60, 0x31, 
    0x2, 0x2ee, 0x2f3, 0x5, 0x64, 0x33, 0x2, 0x2ef, 0x2f3, 0x5, 0x66, 0x34, 
    0x2, 0x2f0, 0x2f3, 0x5, 0x68, 0x35, 0x2, 0x2f1, 0x2f3, 0x5, 0x6a, 0x36, 
    0x2, 0x2f2, 0x2e4, 0x3, 0x2, 0x2, 0x2, 0x2f2, 0x2e5, 0x3, 0x2, 0x2, 
    0x2, 0x2f2, 0x2e6, 0x3, 0x2, 0x2, 0x2, 0x2f2, 0x2e7, 0x3, 0x2, 0x2, 
    0x2, 0x2f2, 0x2e8, 0x3, 0x2, 0x2, 0x2, 0x2f2, 0x2e9, 0x3, 0x2, 0x2, 
    0x2, 0x2f2, 0x2ea, 0x3, 0x2, 0x2, 0x2, 0x2f2, 0x2eb, 0x3, 0x2, 0x2, 
    0x2, 0x2f2, 0x2ed, 0x3, 0x2, 0x2, 0x2, 0x2f2, 0x2ee, 0x3, 0x2, 0x2, 
    0x2, 0x2f2, 0x2ef, 0x3, 0x2, 0x2, 0x2, 0x2f2, 0x2f0, 0x3, 0x2, 0x2, 
    0x2, 0x2f2, 0x2f1, 0x3, 0x2, 0x2, 0x2, 0x2f3, 0x35, 0x3, 0x2, 0x2, 0x2, 
    0x2f4, 0x2fc, 0x7, 0x6d, 0x2, 0x2, 0x2f5, 0x2fc, 0x5, 0x52, 0x2a, 0x2, 
    0x2f6, 0x2fc, 0x5, 0x3a, 0x1e, 0x2, 0x2f7, 0x2fc, 0x5, 0x5a, 0x2e, 0x2, 
    0x2f8, 0x2fc, 0x5, 0x48, 0x25, 0x2, 0x2f9, 0x2fc, 0x5, 0x6c, 0x37, 0x2, 
    0x2fa, 0x2fc, 0x5, 0x6e, 0x38, 0x2, 0x2fb, 0x2f4, 0x3, 0x2, 0x2, 0x2, 
    0x2fb, 0x2f5, 0x3, 0x2, 0x2, 0x2, 0x2fb, 0x2f6, 0x3, 0x2, 0x2, 0x2, 
    0x2fb, 0x2f7, 0x3, 0x2, 0x2, 0x2, 0x2fb, 0x2f8, 0x3, 0x2, 0x2, 0x2, 
    0x2fb, 0x2f9, 0x3, 0x2, 0x2, 0x2, 0x2fb, 0x2fa, 0x3, 0x2, 0x2, 0x2, 
    0x2fc, 0x37, 0x3, 0x2, 0x2, 0x2, 0x2fd, 0x300, 0x7, 0x6e, 0x2, 0x2, 
    0x2fe, 0x2ff, 0x7, 0x39, 0x2, 0x2, 0x2ff, 0x301, 0x7, 0xc0, 0x2, 0x2, 
    0x300, 0x2fe, 0x3, 0x2, 0x2, 0x2, 0x300, 0x301, 0x3, 0x2, 0x2, 0x2, 
    0x301, 0x30a, 0x3, 0x2, 0x2, 0x2, 0x302, 0x305, 0x7, 0x2f, 0x2, 0x2, 
    0x303, 0x304, 0x7, 0x39, 0x2, 0x2, 0x304, 0x306, 0x7, 0xc0, 0x2, 0x2, 
    0x305, 0x303, 0x3, 0x2, 0x2, 0x2, 0x305, 0x306, 0x3, 0x2, 0x2, 0x2, 
    0x306, 0x30a, 0x3, 0x2, 0x2, 0x2, 0x307, 0x30a, 0x7, 0x6f, 0x2, 0x2, 
    0x308, 0x30a, 0x7, 0x70, 0x2, 0x2, 0x309, 0x2fd, 0x3, 0x2, 0x2, 0x2, 
    0x309, 0x302, 0x3, 0x2, 0x2, 0x2, 0x309, 0x307, 0x3, 0x2, 0x2, 0x2, 
    0x309, 0x308, 0x3, 0x2, 0x2, 0x2, 0x30a, 0x39, 0x3, 0x2, 0x2, 0x2, 0x30b, 
    0x30c, 0x7, 0xbf, 0x2, 0x2, 0x30c, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x30d, 
    0x30f, 0x5, 0x44, 0x23, 0x2, 0x30e, 0x310, 0x9, 0x7, 0x2, 0x2, 0x30f, 
    0x30e, 0x3, 0x2, 0x2, 0x2, 0x30f, 0x310, 0x3, 0x2, 0x2, 0x2, 0x310, 
    0x3d, 0x3, 0x2, 0x2, 0x2, 0x311, 0x312, 0x7, 0x71, 0x2, 0x2, 0x312, 
    0x313, 0x7, 0x15, 0x2, 0x2, 0x313, 0x314, 0x5, 0x4, 0x3, 0x2, 0x314, 
    0x3f, 0x3, 0x2, 0x2, 0x2, 0x315, 0x316, 0x9, 0x8, 0x2, 0x2, 0x316, 0x41, 
    0x3, 0x2, 0x2, 0x2, 0x317, 0x318, 0x7, 0x23, 0x2, 0x2, 0x318, 0x43, 
    0x3, 0x2, 0x2, 0x2, 0x319, 0x31a, 0x7, 0x33, 0x2, 0x2, 0x31a, 0x31f, 
    0x5, 0x46, 0x24, 0x2, 0x31b, 0x31c, 0x7, 0x50, 0x2, 0x2, 0x31c, 0x31e, 
    0x5, 0x46, 0x24, 0x2, 0x31d, 0x31b, 0x3, 0x2, 0x2, 0x2, 0x31e, 0x321, 
    0x3, 0x2, 0x2, 0x2, 0x31f, 0x31d, 0x3, 0x2, 0x2, 0x2, 0x31f, 0x320, 
    0x3, 0x2, 0x2, 0x2, 0x320, 0x324, 0x3, 0x2, 0x2, 0x2, 0x321, 0x31f, 
    0x3, 0x2, 0x2, 0x2, 0x322, 0x323, 0x7, 0x4f, 0x2, 0x2, 0x323, 0x325, 
    0x7, 0x29, 0x2, 0x2, 0x324, 0x322, 0x3, 0x2, 0x2, 0x2, 0x324, 0x325, 
    0x3, 0x2, 0x2, 0x2, 0x325, 0x326, 0x3, 0x2, 0x2, 0x2, 0x326, 0x327, 
    0x7, 0x34, 0x2, 0x2, 0x327, 0x32c, 0x3, 0x2, 0x2, 0x2, 0x328, 0x329, 
    0x7, 0x33, 0x2, 0x2, 0x329, 0x32a, 0x7, 0x29, 0x2, 0x2, 0x32a, 0x32c, 
    0x7, 0x34, 0x2, 0x2, 0x32b, 0x319, 0x3, 0x2, 0x2, 0x2, 0x32b, 0x328, 
    0x3, 0x2, 0x2, 0x2, 0x32c, 0x45, 0x3, 0x2, 0x2, 0x2, 0x32d, 0x332, 0x7, 
    0xc5, 0x2, 0x2, 0x32e, 0x32f, 0x7, 0x4d, 0x2, 0x2, 0x32f, 0x331, 0x7, 
    0xc5, 0x2, 0x2, 0x330, 0x32e, 0x3, 0x2, 0x2, 0x2, 0x331, 0x334, 0x3, 
    0x2, 0x2, 0x2, 0x332, 0x330, 0x3, 0x2, 0x2, 0x2, 0x332, 0x333, 0x3, 
    0x2, 0x2, 0x2, 0x333, 0x336, 0x3, 0x2, 0x2, 0x2, 0x334, 0x332, 0x3, 
    0x2, 0x2, 0x2, 0x335, 0x337, 0x5, 0x44, 0x23, 0x2, 0x336, 0x335, 0x3, 
    0x2, 0x2, 0x2, 0x336, 0x337, 0x3, 0x2, 0x2, 0x2, 0x337, 0x47, 0x3, 0x2, 
    0x2, 0x2, 0x338, 0x346, 0x7, 0x3e, 0x2, 0x2, 0x339, 0x33e, 0x7, 0xc5, 
    0x2, 0x2, 0x33a, 0x33b, 0x7, 0x4d, 0x2, 0x2, 0x33b, 0x33d, 0x7, 0xc5, 
    0x2, 0x2, 0x33c, 0x33a, 0x3, 0x2, 0x2, 0x2, 0x33d, 0x340, 0x3, 0x2, 
    0x2, 0x2, 0x33e, 0x33c, 0x3, 0x2, 0x2, 0x2, 0x33e, 0x33f, 0x3, 0x2, 
    0x2, 0x2, 0x33f, 0x343, 0x3, 0x2, 0x2, 0x2, 0x340, 0x33e, 0x3, 0x2, 
    0x2, 0x2, 0x341, 0x342, 0x7, 0x4d, 0x2, 0x2, 0x342, 0x344, 0x7, 0x73, 
    0x2, 0x2, 0x343, 0x341, 0x3, 0x2, 0x2, 0x2, 0x343, 0x344, 0x3, 0x2, 
    0x2, 0x2, 0x344, 0x347, 0x3, 0x2, 0x2, 0x2, 0x345, 0x347, 0x7, 0x73, 
    0x2, 0x2, 0x346, 0x339, 0x3, 0x2, 0x2, 0x2, 0x346, 0x345, 0x3, 0x2, 
    0x2, 0x2, 0x347, 0x49, 0x3, 0x2, 0x2, 0x2, 0x348, 0x349, 0x5, 0x2, 0x2, 
    0x2, 0x349, 0x34a, 0x7, 0x4e, 0x2, 0x2, 0x34a, 0x34b, 0x5, 0x2, 0x2, 
    0x2, 0x34b, 0x34e, 0x3, 0x2, 0x2, 0x2, 0x34c, 0x34e, 0x7, 0x74, 0x2, 
    0x2, 0x34d, 0x348, 0x3, 0x2, 0x2, 0x2, 0x34d, 0x34c, 0x3, 0x2, 0x2, 
    0x2, 0x34e, 0x350, 0x3, 0x2, 0x2, 0x2, 0x34f, 0x351, 0x7, 0x72, 0x2, 
    0x2, 0x350, 0x34f, 0x3, 0x2, 0x2, 0x2, 0x350, 0x351, 0x3, 0x2, 0x2, 
    0x2, 0x351, 0x356, 0x3, 0x2, 0x2, 0x2, 0x352, 0x353, 0x7, 0x35, 0x2, 
    0x2, 0x353, 0x354, 0x5, 0x4, 0x3, 0x2, 0x354, 0x355, 0x7, 0x36, 0x2, 
    0x2, 0x355, 0x357, 0x3, 0x2, 0x2, 0x2, 0x356, 0x352, 0x3, 0x2, 0x2, 
    0x2, 0x356, 0x357, 0x3, 0x2, 0x2, 0x2, 0x357, 0x35b, 0x3, 0x2, 0x2, 
    0x2, 0x358, 0x35c, 0x7, 0x75, 0x2, 0x2, 0x359, 0x35c, 0x7, 0x76, 0x2, 
    0x2, 0x35a, 0x35c, 0x5, 0x4e, 0x28, 0x2, 0x35b, 0x358, 0x3, 0x2, 0x2, 
    0x2, 0x35b, 0x359, 0x3, 0x2, 0x2, 0x2, 0x35b, 0x35a, 0x3, 0x2, 0x2, 
    0x2, 0x35b, 0x35c, 0x3, 0x2, 0x2, 0x2, 0x35c, 0x4b, 0x3, 0x2, 0x2, 0x2, 
    0x35d, 0x35e, 0x9, 0x9, 0x2, 0x2, 0x35e, 0x4d, 0x3, 0x2, 0x2, 0x2, 0x35f, 
    0x360, 0x7, 0x37, 0x2, 0x2, 0x360, 0x364, 0x5, 0x88, 0x45, 0x2, 0x361, 
    0x362, 0x7, 0x35, 0x2, 0x2, 0x362, 0x363, 0x7, 0xc0, 0x2, 0x2, 0x363, 
    0x365, 0x7, 0x36, 0x2, 0x2, 0x364, 0x361, 0x3, 0x2, 0x2, 0x2, 0x364, 
    0x365, 0x3, 0x2, 0x2, 0x2, 0x365, 0x366, 0x3, 0x2, 0x2, 0x2, 0x366, 
    0x367, 0x7, 0x38, 0x2, 0x2, 0x367, 0x372, 0x3, 0x2, 0x2, 0x2, 0x368, 
    0x369, 0x7, 0x40, 0x2, 0x2, 0x369, 0x36d, 0x5, 0x4, 0x3, 0x2, 0x36a, 
    0x36b, 0x7, 0x35, 0x2, 0x2, 0x36b, 0x36c, 0x7, 0xc0, 0x2, 0x2, 0x36c, 
    0x36e, 0x7, 0x36, 0x2, 0x2, 0x36d, 0x36a, 0x3, 0x2, 0x2, 0x2, 0x36d, 
    0x36e, 0x3, 0x2, 0x2, 0x2, 0x36e, 0x36f, 0x3, 0x2, 0x2, 0x2, 0x36f, 
    0x370, 0x7, 0x44, 0x2, 0x2, 0x370, 0x372, 0x3, 0x2, 0x2, 0x2, 0x371, 
    0x35f, 0x3, 0x2, 0x2, 0x2, 0x371, 0x368, 0x3, 0x2, 0x2, 0x2, 0x372, 
    0x4f, 0x3, 0x2, 0x2, 0x2, 0x373, 0x377, 0x5, 0x2, 0x2, 0x2, 0x374, 0x377, 
    0x7, 0x7c, 0x2, 0x2, 0x375, 0x377, 0x7, 0x7d, 0x2, 0x2, 0x376, 0x373, 
    0x3, 0x2, 0x2, 0x2, 0x376, 0x374, 0x3, 0x2, 0x2, 0x2, 0x376, 0x375, 
    0x3, 0x2, 0x2, 0x2, 0x377, 0x51, 0x3, 0x2, 0x2, 0x2, 0x378, 0x37d, 0x5, 
    0x50, 0x29, 0x2, 0x379, 0x37a, 0x7, 0x35, 0x2, 0x2, 0x37a, 0x37b, 0x5, 
    0x4, 0x3, 0x2, 0x37b, 0x37c, 0x7, 0x36, 0x2, 0x2, 0x37c, 0x37e, 0x3, 
    0x2, 0x2, 0x2, 0x37d, 0x379, 0x3, 0x2, 0x2, 0x2, 0x37d, 0x37e, 0x3, 
    0x2, 0x2, 0x2, 0x37e, 0x53, 0x3, 0x2, 0x2, 0x2, 0x37f, 0x380, 0x7, 0x7e, 
    0x2, 0x2, 0x380, 0x381, 0x7, 0xa3, 0x2, 0x2, 0x381, 0x382, 0x7, 0x1d, 
    0x2, 0x2, 0x382, 0x383, 0x5, 0x4, 0x3, 0x2, 0x383, 0x387, 0x5, 0x56, 
    0x2c, 0x2, 0x384, 0x385, 0x7, 0xbf, 0x2, 0x2, 0x385, 0x386, 0x7, 0x4e, 
    0x2, 0x2, 0x386, 0x388, 0x7, 0xbf, 0x2, 0x2, 0x387, 0x384, 0x3, 0x2, 
    0x2, 0x2, 0x387, 0x388, 0x3, 0x2, 0x2, 0x2, 0x388, 0x393, 0x3, 0x2, 
    0x2, 0x2, 0x389, 0x38a, 0x7, 0x7e, 0x2, 0x2, 0x38a, 0x38b, 0x5, 0x4, 
    0x3, 0x2, 0x38b, 0x38c, 0x7, 0xbf, 0x2, 0x2, 0x38c, 0x38d, 0x7, 0x4e, 
    0x2, 0x2, 0x38d, 0x38e, 0x7, 0xbf, 0x2, 0x2, 0x38e, 0x393, 0x3, 0x2, 
    0x2, 0x2, 0x38f, 0x390, 0x7, 0xbf, 0x2, 0x2, 0x390, 0x391, 0x7, 0x4e, 
    0x2, 0x2, 0x391, 0x393, 0x7, 0xbf, 0x2, 0x2, 0x392, 0x37f, 0x3, 0x2, 
    0x2, 0x2, 0x392, 0x389, 0x3, 0x2, 0x2, 0x2, 0x392, 0x38f, 0x3, 0x2, 
    0x2, 0x2, 0x393, 0x55, 0x3, 0x2, 0x2, 0x2, 0x394, 0x396, 0x7, 0x33, 
    0x2, 0x2, 0x395, 0x397, 0x7, 0xa5, 0x2, 0x2, 0x396, 0x395, 0x3, 0x2, 
    0x2, 0x2, 0x396, 0x397, 0x3, 0x2, 0x2, 0x2, 0x397, 0x399, 0x3, 0x2, 
    0x2, 0x2, 0x398, 0x39a, 0x7, 0xbf, 0x2, 0x2, 0x399, 0x398, 0x3, 0x2, 
    0x2, 0x2, 0x399, 0x39a, 0x3, 0x2, 0x2, 0x2, 0x39a, 0x3a0, 0x3, 0x2, 
    0x2, 0x2, 0x39b, 0x39c, 0x5, 0x58, 0x2d, 0x2, 0x39c, 0x39d, 0x7, 0xbf, 
    0x2, 0x2, 0x39d, 0x39f, 0x3, 0x2, 0x2, 0x2, 0x39e, 0x39b, 0x3, 0x2, 
    0x2, 0x2, 0x39f, 0x3a2, 0x3, 0x2, 0x2, 0x2, 0x3a0, 0x39e, 0x3, 0x2, 
    0x2, 0x2, 0x3a0, 0x3a1, 0x3, 0x2, 0x2, 0x2, 0x3a1, 0x3a3, 0x3, 0x2, 
    0x2, 0x2, 0x3a2, 0x3a0, 0x3, 0x2, 0x2, 0x2, 0x3a3, 0x3a5, 0x5, 0x58, 
    0x2d, 0x2, 0x3a4, 0x3a6, 0x7, 0xbf, 0x2, 0x2, 0x3a5, 0x3a4, 0x3, 0x2, 
    0x2, 0x2, 0x3a5, 0x3a6, 0x3, 0x2, 0x2, 0x2, 0x3a6, 0x3a7, 0x3, 0x2, 
    0x2, 0x2, 0x3a7, 0x3a8, 0x7, 0x34, 0x2, 0x2, 0x3a8, 0x57, 0x3, 0x2, 
    0x2, 0x2, 0x3a9, 0x3ac, 0x7, 0xc5, 0x2, 0x2, 0x3aa, 0x3ab, 0x7, 0x3a, 
    0x2, 0x2, 0x3ab, 0x3ad, 0x7, 0xc0, 0x2, 0x2, 0x3ac, 0x3aa, 0x3, 0x2, 
    0x2, 0x2, 0x3ac, 0x3ad, 0x3, 0x2, 0x2, 0x2, 0x3ad, 0x3b2, 0x3, 0x2, 
    0x2, 0x2, 0x3ae, 0x3af, 0x7, 0xc5, 0x2, 0x2, 0x3af, 0x3b0, 0x7, 0x3a, 
    0x2, 0x2, 0x3b0, 0x3b2, 0x5, 0x4, 0x3, 0x2, 0x3b1, 0x3a9, 0x3, 0x2, 
    0x2, 0x2, 0x3b1, 0x3ae, 0x3, 0x2, 0x2, 0x2, 0x3b2, 0x59, 0x3, 0x2, 0x2, 
    0x2, 0x3b3, 0x3b4, 0x7, 0xbf, 0x2, 0x2, 0x3b4, 0x5b, 0x3, 0x2, 0x2, 
    0x2, 0x3b5, 0x3b6, 0x7, 0x5e, 0x2, 0x2, 0x3b6, 0x3b7, 0x7, 0xc5, 0x2, 
    0x2, 0x3b7, 0x3bd, 0x7, 0x47, 0x2, 0x2, 0x3b8, 0x3b9, 0x5, 0x5e, 0x30, 
    0x2, 0x3b9, 0x3ba, 0x7, 0x46, 0x2, 0x2, 0x3ba, 0x3bc, 0x3, 0x2, 0x2, 
    0x2, 0x3bb, 0x3b8, 0x3, 0x2, 0x2, 0x2, 0x3bc, 0x3bf, 0x3, 0x2, 0x2, 
    0x2, 0x3bd, 0x3bb, 0x3, 0x2, 0x2, 0x2, 0x3bd, 0x3be, 0x3, 0x2, 0x2, 
    0x2, 0x3be, 0x5d, 0x3, 0x2, 0x2, 0x2, 0x3bf, 0x3bd, 0x3, 0x2, 0x2, 0x2, 
    0x3c0, 0x3c1, 0x5, 0x4, 0x3, 0x2, 0x3c1, 0x3c2, 0x7, 0x47, 0x2, 0x2, 
    0x3c2, 0x3c7, 0x5, 0x4, 0x3, 0x2, 0x3c3, 0x3c4, 0x7, 0x78, 0x2, 0x2, 
    0x3c4, 0x3c6, 0x5, 0x4, 0x3, 0x2, 0x3c5, 0x3c3, 0x3, 0x2, 0x2, 0x2, 
    0x3c6, 0x3c9, 0x3, 0x2, 0x2, 0x2, 0x3c7, 0x3c5, 0x3, 0x2, 0x2, 0x2, 
    0x3c7, 0x3c8, 0x3, 0x2, 0x2, 0x2, 0x3c8, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x3c9, 
    0x3c7, 0x3, 0x2, 0x2, 0x2, 0x3ca, 0x3ce, 0x7, 0x80, 0x2, 0x2, 0x3cb, 
    0x3cc, 0x6, 0x31, 0xb, 0x2, 0x3cc, 0x3ce, 0x7, 0x5f, 0x2, 0x2, 0x3cd, 
    0x3ca, 0x3, 0x2, 0x2, 0x2, 0x3cd, 0x3cb, 0x3, 0x2, 0x2, 0x2, 0x3ce, 
    0x3cf, 0x3, 0x2, 0x2, 0x2, 0x3cf, 0x3d6, 0x5, 0x4a, 0x26, 0x2, 0x3d0, 
    0x3d1, 0x7, 0x50, 0x2, 0x2, 0x3d1, 0x3d4, 0x5, 0x4a, 0x26, 0x2, 0x3d2, 
    0x3d3, 0x7, 0x50, 0x2, 0x2, 0x3d3, 0x3d5, 0x5, 0x4a, 0x26, 0x2, 0x3d4, 
    0x3d2, 0x3, 0x2, 0x2, 0x2, 0x3d4, 0x3d5, 0x3, 0x2, 0x2, 0x2, 0x3d5, 
    0x3d7, 0x3, 0x2, 0x2, 0x2, 0x3d6, 0x3d0, 0x3, 0x2, 0x2, 0x2, 0x3d6, 
    0x3d7, 0x3, 0x2, 0x2, 0x2, 0x3d7, 0x3da, 0x3, 0x2, 0x2, 0x2, 0x3d8, 
    0x3d9, 0x7, 0x50, 0x2, 0x2, 0x3d9, 0x3db, 0x5, 0x62, 0x32, 0x2, 0x3da, 
    0x3d8, 0x3, 0x2, 0x2, 0x2, 0x3da, 0x3db, 0x3, 0x2, 0x2, 0x2, 0x3db, 
    0x61, 0x3, 0x2, 0x2, 0x2, 0x3dc, 0x3dd, 0x7, 0x81, 0x2, 0x2, 0x3dd, 
    0x3de, 0x7, 0xc0, 0x2, 0x2, 0x3de, 0x3df, 0x7, 0x52, 0x2, 0x2, 0x3df, 
    0x3e0, 0x7, 0xc0, 0x2, 0x2, 0x3e0, 0x63, 0x3, 0x2, 0x2, 0x2, 0x3e1, 
    0x3e5, 0x7, 0x22, 0x2, 0x2, 0x3e2, 0x3e6, 0x7, 0x82, 0x2, 0x2, 0x3e3, 
    0x3e6, 0x5, 0x4a, 0x26, 0x2, 0x3e4, 0x3e6, 0x5, 0x38, 0x1d, 0x2, 0x3e5, 
    0x3e2, 0x3, 0x2, 0x2, 0x2, 0x3e5, 0x3e3, 0x3, 0x2, 0x2, 0x2, 0x3e5, 
    0x3e4, 0x3, 0x2, 0x2, 0x2, 0x3e6, 0x65, 0x3, 0x2, 0x2, 0x2, 0x3e7, 0x3e8, 
    0x7, 0x83, 0x2, 0x2, 0x3e8, 0x3e9, 0x9, 0xa, 0x2, 0x2, 0x3e9, 0x67, 
    0x3, 0x2, 0x2, 0x2, 0x3ea, 0x3ec, 0x7, 0x26, 0x2, 0x2, 0x3eb, 0x3ed, 
    0x5, 0x24, 0x13, 0x2, 0x3ec, 0x3eb, 0x3, 0x2, 0x2, 0x2, 0x3ec, 0x3ed, 
    0x3, 0x2, 0x2, 0x2, 0x3ed, 0x3f3, 0x3, 0x2, 0x2, 0x2, 0x3ee, 0x3f0, 
    0x7, 0x58, 0x2, 0x2, 0x3ef, 0x3f1, 0x5, 0x24, 0x13, 0x2, 0x3f0, 0x3ef, 
    0x3, 0x2, 0x2, 0x2, 0x3f0, 0x3f1, 0x3, 0x2, 0x2, 0x2, 0x3f1, 0x3f3, 
    0x3, 0x2, 0x2, 0x2, 0x3f2, 0x3ea, 0x3, 0x2, 0x2, 0x2, 0x3f2, 0x3ee, 
    0x3, 0x2, 0x2, 0x2, 0x3f3, 0x69, 0x3, 0x2, 0x2, 0x2, 0x3f4, 0x3fb, 0x7, 
    0x2e, 0x2, 0x2, 0x3f5, 0x3f9, 0x7, 0x15, 0x2, 0x2, 0x3f6, 0x3fa, 0x5, 
    0xbc, 0x5f, 0x2, 0x3f7, 0x3f8, 0x7, 0x3d, 0x2, 0x2, 0x3f8, 0x3fa, 0x7, 
    0xc5, 0x2, 0x2, 0x3f9, 0x3f6, 0x3, 0x2, 0x2, 0x2, 0x3f9, 0x3f7, 0x3, 
    0x2, 0x2, 0x2, 0x3fa, 0x3fc, 0x3, 0x2, 0x2, 0x2, 0x3fb, 0x3f5, 0x3, 
    0x2, 0x2, 0x2, 0x3fb, 0x3fc, 0x3, 0x2, 0x2, 0x2, 0x3fc, 0x409, 0x3, 
    0x2, 0x2, 0x2, 0x3fd, 0x3fe, 0x7, 0x68, 0x2, 0x2, 0x3fe, 0x3ff, 0x7, 
    0x33, 0x2, 0x2, 0x3ff, 0x404, 0x5, 0x1a, 0xe, 0x2, 0x400, 0x401, 0x7, 
    0x4f, 0x2, 0x2, 0x401, 0x403, 0x5, 0x1a, 0xe, 0x2, 0x402, 0x400, 0x3, 
    0x2, 0x2, 0x2, 0x403, 0x406, 0x3, 0x2, 0x2, 0x2, 0x404, 0x402, 0x3, 
    0x2, 0x2, 0x2, 0x404, 0x405, 0x3, 0x2, 0x2, 0x2, 0x405, 0x407, 0x3, 
    0x2, 0x2, 0x2, 0x406, 0x404, 0x3, 0x2, 0x2, 0x2, 0x407, 0x408, 0x7, 
    0x34, 0x2, 0x2, 0x408, 0x40a, 0x3, 0x2, 0x2, 0x2, 0x409, 0x3fd, 0x3, 
    0x2, 0x2, 0x2, 0x409, 0x40a, 0x3, 0x2, 0x2, 0x2, 0x40a, 0x6b, 0x3, 0x2, 
    0x2, 0x2, 0x40b, 0x40c, 0x7, 0x44, 0x2, 0x2, 0x40c, 0x40d, 0x5, 0x4, 
    0x3, 0x2, 0x40d, 0x6d, 0x3, 0x2, 0x2, 0x2, 0x40e, 0x412, 0x7, 0x43, 
    0x2, 0x2, 0x40f, 0x410, 0x5, 0x4, 0x3, 0x2, 0x410, 0x411, 0x7, 0x52, 
    0x2, 0x2, 0x411, 0x413, 0x3, 0x2, 0x2, 0x2, 0x412, 0x40f, 0x3, 0x2, 
    0x2, 0x2, 0x412, 0x413, 0x3, 0x2, 0x2, 0x2, 0x413, 0x414, 0x3, 0x2, 
    0x2, 0x2, 0x414, 0x415, 0x7, 0xc5, 0x2, 0x2, 0x415, 0x6f, 0x3, 0x2, 
    0x2, 0x2, 0x416, 0x418, 0x7, 0x86, 0x2, 0x2, 0x417, 0x416, 0x3, 0x2, 
    0x2, 0x2, 0x417, 0x418, 0x3, 0x2, 0x2, 0x2, 0x418, 0x419, 0x3, 0x2, 
    0x2, 0x2, 0x419, 0x426, 0x7, 0x87, 0x2, 0x2, 0x41a, 0x426, 0x7, 0x88, 
    0x2, 0x2, 0x41b, 0x426, 0x7, 0x89, 0x2, 0x2, 0x41c, 0x41e, 0x6, 0x39, 
    0xc, 0x2, 0x41d, 0x41f, 0x7, 0x86, 0x2, 0x2, 0x41e, 0x41d, 0x3, 0x2, 
    0x2, 0x2, 0x41e, 0x41f, 0x3, 0x2, 0x2, 0x2, 0x41f, 0x420, 0x3, 0x2, 
    0x2, 0x2, 0x420, 0x424, 0x7, 0x60, 0x2, 0x2, 0x421, 0x424, 0x7, 0x61, 
    0x2, 0x2, 0x422, 0x424, 0x7, 0x62, 0x2, 0x2, 0x423, 0x41c, 0x3, 0x2, 
    0x2, 0x2, 0x423, 0x421, 0x3, 0x2, 0x2, 0x2, 0x423, 0x422, 0x3, 0x2, 
    0x2, 0x2, 0x424, 0x426, 0x3, 0x2, 0x2, 0x2, 0x425, 0x417, 0x3, 0x2, 
    0x2, 0x2, 0x425, 0x41a, 0x3, 0x2, 0x2, 0x2, 0x425, 0x41b, 0x3, 0x2, 
    0x2, 0x2, 0x425, 0x423, 0x3, 0x2, 0x2, 0x2, 0x426, 0x428, 0x3, 0x2, 
    0x2, 0x2, 0x427, 0x429, 0x5, 0x72, 0x3a, 0x2, 0x428, 0x427, 0x3, 0x2, 
    0x2, 0x2, 0x428, 0x429, 0x3, 0x2, 0x2, 0x2, 0x429, 0x42c, 0x3, 0x2, 
    0x2, 0x2, 0x42a, 0x42b, 0x7, 0x8d, 0x2, 0x2, 0x42b, 0x42d, 0x5, 0x4, 
    0x3, 0x2, 0x42c, 0x42a, 0x3, 0x2, 0x2, 0x2, 0x42c, 0x42d, 0x3, 0x2, 
    0x2, 0x2, 0x42d, 0x435, 0x3, 0x2, 0x2, 0x2, 0x42e, 0x42f, 0x7, 0x8e, 
    0x2, 0x2, 0x42f, 0x430, 0x7, 0x8f, 0x2, 0x2, 0x430, 0x431, 0x7, 0x44, 
    0x2, 0x2, 0x431, 0x436, 0x5, 0x2, 0x2, 0x2, 0x432, 0x433, 0x6, 0x39, 
    0xd, 0x2, 0x433, 0x434, 0x7, 0x8e, 0x2, 0x2, 0x434, 0x436, 0x7, 0x8f, 
    0x2, 0x2, 0x435, 0x42e, 0x3, 0x2, 0x2, 0x2, 0x435, 0x432, 0x3, 0x2, 
    0x2, 0x2, 0x435, 0x436, 0x3, 0x2, 0x2, 0x2, 0x436, 0x43b, 0x3, 0x2, 
    0x2, 0x2, 0x437, 0x438, 0x6, 0x39, 0xe, 0x2, 0x438, 0x439, 0x7, 0x90, 
    0x2, 0x2, 0x439, 0x43a, 0x7, 0x91, 0x2, 0x2, 0x43a, 0x43c, 0x5, 0x4, 
    0x3, 0x2, 0x43b, 0x437, 0x3, 0x2, 0x2, 0x2, 0x43b, 0x43c, 0x3, 0x2, 
    0x2, 0x2, 0x43c, 0x71, 0x3, 0x2, 0x2, 0x2, 0x43d, 0x43e, 0x7, 0x8a, 
    0x2, 0x2, 0x43e, 0x43f, 0x7, 0x33, 0x2, 0x2, 0x43f, 0x444, 0x5, 0x74, 
    0x3b, 0x2, 0x440, 0x441, 0x7, 0x50, 0x2, 0x2, 0x441, 0x443, 0x5, 0x74, 
    0x3b, 0x2, 0x442, 0x440, 0x3, 0x2, 0x2, 0x2, 0x443, 0x446, 0x3, 0x2, 
    0x2, 0x2, 0x444, 0x442, 0x3, 0x2, 0x2, 0x2, 0x444, 0x445, 0x3, 0x2, 
    0x2, 0x2, 0x445, 0x447, 0x3, 0x2, 0x2, 0x2, 0x446, 0x444, 0x3, 0x2, 
    0x2, 0x2, 0x447, 0x448, 0x7, 0x34, 0x2, 0x2, 0x448, 0x73, 0x3, 0x2, 
    0x2, 0x2, 0x449, 0x44d, 0x7, 0x8b, 0x2, 0x2, 0x44a, 0x44d, 0x7, 0x8c, 
    0x2, 0x2, 0x44b, 0x44d, 0x5, 0x4, 0x3, 0x2, 0x44c, 0x449, 0x3, 0x2, 
    0x2, 0x2, 0x44c, 0x44a, 0x3, 0x2, 0x2, 0x2, 0x44c, 0x44b, 0x3, 0x2, 
    0x2, 0x2, 0x44d, 0x75, 0x3, 0x2, 0x2, 0x2, 0x44e, 0x44f, 0x7, 0x90, 
    0x2, 0x2, 0x44f, 0x453, 0x7, 0x92, 0x2, 0x2, 0x450, 0x452, 0x5, 0x78, 
    0x3d, 0x2, 0x451, 0x450, 0x3, 0x2, 0x2, 0x2, 0x452, 0x455, 0x3, 0x2, 
    0x2, 0x2, 0x453, 0x451, 0x3, 0x2, 0x2, 0x2, 0x453, 0x454, 0x3, 0x2, 
    0x2, 0x2, 0x454, 0x77, 0x3, 0x2, 0x2, 0x2, 0x455, 0x453, 0x3, 0x2, 0x2, 
    0x2, 0x456, 0x457, 0x7, 0xc5, 0x2, 0x2, 0x457, 0x458, 0x7, 0x4f, 0x2, 
    0x2, 0x458, 0x459, 0x5, 0x4, 0x3, 0x2, 0x459, 0x45a, 0x7, 0x46, 0x2, 
    0x2, 0x45a, 0x79, 0x3, 0x2, 0x2, 0x2, 0x45b, 0x45f, 0x7, 0x93, 0x2, 
    0x2, 0x45c, 0x45e, 0x5, 0x7c, 0x3f, 0x2, 0x45d, 0x45c, 0x3, 0x2, 0x2, 
    0x2, 0x45e, 0x461, 0x3, 0x2, 0x2, 0x2, 0x45f, 0x45d, 0x3, 0x2, 0x2, 
    0x2, 0x45f, 0x460, 0x3, 0x2, 0x2, 0x2, 0x460, 0x7b, 0x3, 0x2, 0x2, 0x2, 
    0x461, 0x45f, 0x3, 0x2, 0x2, 0x2, 0x462, 0x46a, 0x7, 0xc5, 0x2, 0x2, 
    0x463, 0x464, 0x7, 0x33, 0x2, 0x2, 0x464, 0x465, 0x7, 0x27, 0x2, 0x2, 
    0x465, 0x46b, 0x7, 0x34, 0x2, 0x2, 0x466, 0x467, 0x7, 0x35, 0x2, 0x2, 
    0x467, 0x468, 0x5, 0x4, 0x3, 0x2, 0x468, 0x469, 0x7, 0x36, 0x2, 0x2, 
    0x469, 0x46b, 0x3, 0x2, 0x2, 0x2, 0x46a, 0x463, 0x3, 0x2, 0x2, 0x2, 
    0x46a, 0x466, 0x3, 0x2, 0x2, 0x2, 0x46a, 0x46b, 0x3, 0x2, 0x2, 0x2, 
    0x46b, 0x46e, 0x3, 0x2, 0x2, 0x2, 0x46c, 0x46d, 0x7, 0x20, 0x2, 0x2, 
    0x46d, 0x46f, 0x5, 0x4, 0x3, 0x2, 0x46e, 0x46c, 0x3, 0x2, 0x2, 0x2, 
    0x46e, 0x46f, 0x3, 0x2, 0x2, 0x2, 0x46f, 0x475, 0x3, 0x2, 0x2, 0x2, 
    0x470, 0x473, 0x7, 0x47, 0x2, 0x2, 0x471, 0x474, 0x5, 0x7e, 0x40, 0x2, 
    0x472, 0x474, 0x5, 0x80, 0x41, 0x2, 0x473, 0x471, 0x3, 0x2, 0x2, 0x2, 
    0x473, 0x472, 0x3, 0x2, 0x2, 0x2, 0x474, 0x476, 0x3, 0x2, 0x2, 0x2, 
    0x475, 0x470, 0x3, 0x2, 0x2, 0x2, 0x475, 0x476, 0x3, 0x2, 0x2, 0x2, 
    0x476, 0x477, 0x3, 0x2, 0x2, 0x2, 0x477, 0x478, 0x7, 0x46, 0x2, 0x2, 
    0x478, 0x7d, 0x3, 0x2, 0x2, 0x2, 0x479, 0x47e, 0x5, 0x50, 0x29, 0x2, 
    0x47a, 0x47b, 0x9, 0xb, 0x2, 0x2, 0x47b, 0x47d, 0x5, 0x50, 0x29, 0x2, 
    0x47c, 0x47a, 0x3, 0x2, 0x2, 0x2, 0x47d, 0x480, 0x3, 0x2, 0x2, 0x2, 
    0x47e, 0x47c, 0x3, 0x2, 0x2, 0x2, 0x47e, 0x47f, 0x3, 0x2, 0x2, 0x2, 
    0x47f, 0x484, 0x3, 0x2, 0x2, 0x2, 0x480, 0x47e, 0x3, 0x2, 0x2, 0x2, 
    0x481, 0x482, 0x7, 0x94, 0x2, 0x2, 0x482, 0x484, 0x7, 0xbd, 0x2, 0x2, 
    0x483, 0x479, 0x3, 0x2, 0x2, 0x2, 0x483, 0x481, 0x3, 0x2, 0x2, 0x2, 
    0x483, 0x484, 0x3, 0x2, 0x2, 0x2, 0x484, 0x485, 0x3, 0x2, 0x2, 0x2, 
    0x485, 0x486, 0x7, 0x35, 0x2, 0x2, 0x486, 0x487, 0x5, 0x4, 0x3, 0x2, 
    0x487, 0x488, 0x7, 0x36, 0x2, 0x2, 0x488, 0x7f, 0x3, 0x2, 0x2, 0x2, 
    0x489, 0x48a, 0x7, 0x33, 0x2, 0x2, 0x48a, 0x48b, 0x5, 0x82, 0x42, 0x2, 
    0x48b, 0x48c, 0x7, 0x34, 0x2, 0x2, 0x48c, 0x81, 0x3, 0x2, 0x2, 0x2, 
    0x48d, 0x48e, 0x8, 0x42, 0x1, 0x2, 0x48e, 0x48f, 0x5, 0x4, 0x3, 0x2, 
    0x48f, 0x498, 0x3, 0x2, 0x2, 0x2, 0x490, 0x491, 0xc, 0x4, 0x2, 0x2, 
    0x491, 0x492, 0x7, 0x39, 0x2, 0x2, 0x492, 0x497, 0x5, 0x4, 0x3, 0x2, 
    0x493, 0x494, 0xc, 0x3, 0x2, 0x2, 0x494, 0x495, 0x7, 0x3a, 0x2, 0x2, 
    0x495, 0x497, 0x5, 0x4, 0x3, 0x2, 0x496, 0x490, 0x3, 0x2, 0x2, 0x2, 
    0x496, 0x493, 0x3, 0x2, 0x2, 0x2, 0x497, 0x49a, 0x3, 0x2, 0x2, 0x2, 
    0x498, 0x496, 0x3, 0x2, 0x2, 0x2, 0x498, 0x499, 0x3, 0x2, 0x2, 0x2, 
    0x499, 0x83, 0x3, 0x2, 0x2, 0x2, 0x49a, 0x498, 0x3, 0x2, 0x2, 0x2, 0x49b, 
    0x49c, 0x9, 0xc, 0x2, 0x2, 0x49c, 0x49d, 0x7, 0x21, 0x2, 0x2, 0x49d, 
    0x4a1, 0x7, 0xc5, 0x2, 0x2, 0x49e, 0x49f, 0x7, 0x33, 0x2, 0x2, 0x49f, 
    0x4a0, 0x7, 0x29, 0x2, 0x2, 0x4a0, 0x4a2, 0x7, 0x34, 0x2, 0x2, 0x4a1, 
    0x49e, 0x3, 0x2, 0x2, 0x2, 0x4a1, 0x4a2, 0x3, 0x2, 0x2, 0x2, 0x4a2, 
    0x4a5, 0x3, 0x2, 0x2, 0x2, 0x4a3, 0x4a4, 0x7, 0x20, 0x2, 0x2, 0x4a4, 
    0x4a6, 0x5, 0x86, 0x44, 0x2, 0x4a5, 0x4a3, 0x3, 0x2, 0x2, 0x2, 0x4a5, 
    0x4a6, 0x3, 0x2, 0x2, 0x2, 0x4a6, 0x4a7, 0x3, 0x2, 0x2, 0x2, 0x4a7, 
    0x4a8, 0x7, 0x3f, 0x2, 0x2, 0x4a8, 0x4b7, 0x5, 0x4, 0x3, 0x2, 0x4a9, 
    0x4aa, 0x7, 0x96, 0x2, 0x2, 0x4aa, 0x4ab, 0x7, 0x15, 0x2, 0x2, 0x4ab, 
    0x4ac, 0x7, 0xc5, 0x2, 0x2, 0x4ac, 0x4ad, 0x7, 0x4f, 0x2, 0x2, 0x4ad, 
    0x4b2, 0x7, 0xc5, 0x2, 0x2, 0x4ae, 0x4af, 0x7, 0x50, 0x2, 0x2, 0x4af, 
    0x4b1, 0x7, 0xc5, 0x2, 0x2, 0x4b0, 0x4ae, 0x3, 0x2, 0x2, 0x2, 0x4b1, 
    0x4b4, 0x3, 0x2, 0x2, 0x2, 0x4b2, 0x4b0, 0x3, 0x2, 0x2, 0x2, 0x4b2, 
    0x4b3, 0x3, 0x2, 0x2, 0x2, 0x4b3, 0x4b6, 0x3, 0x2, 0x2, 0x2, 0x4b4, 
    0x4b2, 0x3, 0x2, 0x2, 0x2, 0x4b5, 0x4a9, 0x3, 0x2, 0x2, 0x2, 0x4b6, 
    0x4b9, 0x3, 0x2, 0x2, 0x2, 0x4b7, 0x4b5, 0x3, 0x2, 0x2, 0x2, 0x4b7, 
    0x4b8, 0x3, 0x2, 0x2, 0x2, 0x4b8, 0x4ba, 0x3, 0x2, 0x2, 0x2, 0x4b9, 
    0x4b7, 0x3, 0x2, 0x2, 0x2, 0x4ba, 0x4bb, 0x7, 0x46, 0x2, 0x2, 0x4bb, 
    0x85, 0x3, 0x2, 0x2, 0x2, 0x4bc, 0x4bd, 0x7, 0xc5, 0x2, 0x2, 0x4bd, 
    0x4bf, 0x7, 0x4d, 0x2, 0x2, 0x4be, 0x4bc, 0x3, 0x2, 0x2, 0x2, 0x4be, 
    0x4bf, 0x3, 0x2, 0x2, 0x2, 0x4bf, 0x4c0, 0x3, 0x2, 0x2, 0x2, 0x4c0, 
    0x4c1, 0x7, 0xc5, 0x2, 0x2, 0x4c1, 0x4c3, 0x7, 0x4d, 0x2, 0x2, 0x4c2, 
    0x4be, 0x3, 0x2, 0x2, 0x2, 0x4c2, 0x4c3, 0x3, 0x2, 0x2, 0x2, 0x4c3, 
    0x4c4, 0x3, 0x2, 0x2, 0x2, 0x4c4, 0x4c5, 0x7, 0xc5, 0x2, 0x2, 0x4c5, 
    0x87, 0x3, 0x2, 0x2, 0x2, 0x4c6, 0x4c7, 0x5, 0x86, 0x44, 0x2, 0x4c7, 
    0x4c8, 0x7, 0x4d, 0x2, 0x2, 0x4c8, 0x4ca, 0x3, 0x2, 0x2, 0x2, 0x4c9, 
    0x4c6, 0x3, 0x2, 0x2, 0x2, 0x4c9, 0x4ca, 0x3, 0x2, 0x2, 0x2, 0x4ca, 
    0x4cb, 0x3, 0x2, 0x2, 0x2, 0x4cb, 0x4cc, 0x7, 0xc5, 0x2, 0x2, 0x4cc, 
    0x89, 0x3, 0x2, 0x2, 0x2, 0x4cd, 0x4cf, 0x7, 0xc5, 0x2, 0x2, 0x4ce, 
    0x4d0, 0x5, 0x8c, 0x47, 0x2, 0x4cf, 0x4ce, 0x3, 0x2, 0x2, 0x2, 0x4cf, 
    0x4d0, 0x3, 0x2, 0x2, 0x2, 0x4d0, 0x4d1, 0x3, 0x2, 0x2, 0x2, 0x4d1, 
    0x4d8, 0x7, 0x4f, 0x2, 0x2, 0x4d2, 0x4d9, 0x5, 0x1a, 0xe, 0x2, 0x4d3, 
    0x4d6, 0x7, 0x97, 0x2, 0x2, 0x4d4, 0x4d5, 0x7, 0x15, 0x2, 0x2, 0x4d5, 
    0x4d7, 0x5, 0x4, 0x3, 0x2, 0x4d6, 0x4d4, 0x3, 0x2, 0x2, 0x2, 0x4d6, 
    0x4d7, 0x3, 0x2, 0x2, 0x2, 0x4d7, 0x4d9, 0x3, 0x2, 0x2, 0x2, 0x4d8, 
    0x4d2, 0x3, 0x2, 0x2, 0x2, 0x4d8, 0x4d3, 0x3, 0x2, 0x2, 0x2, 0x4d9, 
    0x4da, 0x3, 0x2, 0x2, 0x2, 0x4da, 0x4db, 0x7, 0x46, 0x2, 0x2, 0x4db, 
    0x8b, 0x3, 0x2, 0x2, 0x2, 0x4dc, 0x4dd, 0x7, 0x33, 0x2, 0x2, 0x4dd, 
    0x4e2, 0x9, 0xd, 0x2, 0x2, 0x4de, 0x4df, 0x7, 0x50, 0x2, 0x2, 0x4df, 
    0x4e1, 0x9, 0xd, 0x2, 0x2, 0x4e0, 0x4de, 0x3, 0x2, 0x2, 0x2, 0x4e1, 
    0x4e4, 0x3, 0x2, 0x2, 0x2, 0x4e2, 0x4e0, 0x3, 0x2, 0x2, 0x2, 0x4e2, 
    0x4e3, 0x3, 0x2, 0x2, 0x2, 0x4e3, 0x4e5, 0x3, 0x2, 0x2, 0x2, 0x4e4, 
    0x4e2, 0x3, 0x2, 0x2, 0x2, 0x4e5, 0x4e6, 0x7, 0x34, 0x2, 0x2, 0x4e6, 
    0x8d, 0x3, 0x2, 0x2, 0x2, 0x4e7, 0x4eb, 0x7, 0x59, 0x2, 0x2, 0x4e8, 
    0x4ea, 0x5, 0x90, 0x49, 0x2, 0x4e9, 0x4e8, 0x3, 0x2, 0x2, 0x2, 0x4ea, 
    0x4ed, 0x3, 0x2, 0x2, 0x2, 0x4eb, 0x4e9, 0x3, 0x2, 0x2, 0x2, 0x4eb, 
    0x4ec, 0x3, 0x2, 0x2, 0x2, 0x4ec, 0x8f, 0x3, 0x2, 0x2, 0x2, 0x4ed, 0x4eb, 
    0x3, 0x2, 0x2, 0x2, 0x4ee, 0x4ef, 0x7, 0xc5, 0x2, 0x2, 0x4ef, 0x4f0, 
    0x7, 0x4f, 0x2, 0x2, 0x4f0, 0x4f1, 0x5, 0x1a, 0xe, 0x2, 0x4f1, 0x4f2, 
    0x7, 0x46, 0x2, 0x2, 0x4f2, 0x91, 0x3, 0x2, 0x2, 0x2, 0x4f3, 0x4f9, 
    0x5, 0x94, 0x4b, 0x2, 0x4f4, 0x4f9, 0x5, 0x96, 0x4c, 0x2, 0x4f5, 0x4f9, 
    0x5, 0x98, 0x4d, 0x2, 0x4f6, 0x4f9, 0x5, 0x9a, 0x4e, 0x2, 0x4f7, 0x4f9, 
    0x5, 0xa4, 0x53, 0x2, 0x4f8, 0x4f3, 0x3, 0x2, 0x2, 0x2, 0x4f8, 0x4f4, 
    0x3, 0x2, 0x2, 0x2, 0x4f8, 0x4f5, 0x3, 0x2, 0x2, 0x2, 0x4f8, 0x4f6, 
    0x3, 0x2, 0x2, 0x2, 0x4f8, 0x4f7, 0x3, 0x2, 0x2, 0x2, 0x4f9, 0x93, 0x3, 
    0x2, 0x2, 0x2, 0x4fa, 0x4fb, 0x7, 0x5a, 0x2, 0x2, 0x4fb, 0x4ff, 0x7, 
    0x98, 0x2, 0x2, 0x4fc, 0x4fd, 0x6, 0x4b, 0x11, 0x2, 0x4fd, 0x4fe, 0x7, 
    0xc5, 0x2, 0x2, 0x4fe, 0x500, 0x7, 0x4f, 0x2, 0x2, 0x4ff, 0x4fc, 0x3, 
    0x2, 0x2, 0x2, 0x4ff, 0x500, 0x3, 0x2, 0x2, 0x2, 0x500, 0x501, 0x3, 
    0x2, 0x2, 0x2, 0x501, 0x502, 0x5, 0xa8, 0x55, 0x2, 0x502, 0x503, 0x7, 
    0x46, 0x2, 0x2, 0x503, 0x95, 0x3, 0x2, 0x2, 0x2, 0x504, 0x508, 0x7, 
    0x98, 0x2, 0x2, 0x505, 0x506, 0x6, 0x4c, 0x12, 0x2, 0x506, 0x507, 0x7, 
    0xc5, 0x2, 0x2, 0x507, 0x509, 0x7, 0x4f, 0x2, 0x2, 0x508, 0x505, 0x3, 
    0x2, 0x2, 0x2, 0x508, 0x509, 0x3, 0x2, 0x2, 0x2, 0x509, 0x50a, 0x3, 
    0x2, 0x2, 0x2, 0x50a, 0x50b, 0x9, 0xe, 0x2, 0x2, 0x50b, 0x50c, 0x5, 
    0x2, 0x2, 0x2, 0x50c, 0x50d, 0x7, 0x3c, 0x2, 0x2, 0x50d, 0x50e, 0x5, 
    0xa8, 0x55, 0x2, 0x50e, 0x50f, 0x7, 0x46, 0x2, 0x2, 0x50f, 0x97, 0x3, 
    0x2, 0x2, 0x2, 0x510, 0x511, 0x7, 0x9a, 0x2, 0x2, 0x511, 0x515, 0x7, 
    0x98, 0x2, 0x2, 0x512, 0x513, 0x6, 0x4d, 0x13, 0x2, 0x513, 0x514, 0x7, 
    0xc5, 0x2, 0x2, 0x514, 0x516, 0x7, 0x4f, 0x2, 0x2, 0x515, 0x512, 0x3, 
    0x2, 0x2, 0x2, 0x515, 0x516, 0x3, 0x2, 0x2, 0x2, 0x516, 0x517, 0x3, 
    0x2, 0x2, 0x2, 0x517, 0x518, 0x5, 0xbc, 0x5f, 0x2, 0x518, 0x519, 0x7, 
    0x9b, 0x2, 0x2, 0x519, 0x51a, 0x7, 0x9c, 0x2, 0x2, 0x51a, 0x51b, 0x5, 
    0x4, 0x3, 0x2, 0x51b, 0x51c, 0x7, 0x4f, 0x2, 0x2, 0x51c, 0x524, 0x5, 
    0xbc, 0x5f, 0x2, 0x51d, 0x51e, 0x7, 0x78, 0x2, 0x2, 0x51e, 0x51f, 0x5, 
    0x4, 0x3, 0x2, 0x51f, 0x520, 0x7, 0x4f, 0x2, 0x2, 0x520, 0x521, 0x5, 
    0xbc, 0x5f, 0x2, 0x521, 0x523, 0x3, 0x2, 0x2, 0x2, 0x522, 0x51d, 0x3, 
    0x2, 0x2, 0x2, 0x523, 0x526, 0x3, 0x2, 0x2, 0x2, 0x524, 0x522, 0x3, 
    0x2, 0x2, 0x2, 0x524, 0x525, 0x3, 0x2, 0x2, 0x2, 0x525, 0x527, 0x3, 
    0x2, 0x2, 0x2, 0x526, 0x524, 0x3, 0x2, 0x2, 0x2, 0x527, 0x528, 0x7, 
    0x46, 0x2, 0x2, 0x528, 0x99, 0x3, 0x2, 0x2, 0x2, 0x529, 0x52e, 0x7, 
    0x9d, 0x2, 0x2, 0x52a, 0x52b, 0x6, 0x4e, 0x14, 0x2, 0x52b, 0x52c, 0x7, 
    0x33, 0x2, 0x2, 0x52c, 0x52d, 0x7, 0x21, 0x2, 0x2, 0x52d, 0x52f, 0x7, 
    0x34, 0x2, 0x2, 0x52e, 0x52a, 0x3, 0x2, 0x2, 0x2, 0x52e, 0x52f, 0x3, 
    0x2, 0x2, 0x2, 0x52f, 0x533, 0x3, 0x2, 0x2, 0x2, 0x530, 0x531, 0x6, 
    0x4e, 0x15, 0x2, 0x531, 0x532, 0x7, 0xc5, 0x2, 0x2, 0x532, 0x534, 0x7, 
    0x4f, 0x2, 0x2, 0x533, 0x530, 0x3, 0x2, 0x2, 0x2, 0x533, 0x534, 0x3, 
    0x2, 0x2, 0x2, 0x534, 0x539, 0x3, 0x2, 0x2, 0x2, 0x535, 0x536, 0x7, 
    0x9e, 0x2, 0x2, 0x536, 0x537, 0x5, 0xa8, 0x55, 0x2, 0x537, 0x538, 0x7, 
    0x4f, 0x2, 0x2, 0x538, 0x53a, 0x3, 0x2, 0x2, 0x2, 0x539, 0x535, 0x3, 
    0x2, 0x2, 0x2, 0x539, 0x53a, 0x3, 0x2, 0x2, 0x2, 0x53a, 0x53d, 0x3, 
    0x2, 0x2, 0x2, 0x53b, 0x53e, 0x5, 0x9c, 0x4f, 0x2, 0x53c, 0x53e, 0x5, 
    0xa0, 0x51, 0x2, 0x53d, 0x53b, 0x3, 0x2, 0x2, 0x2, 0x53d, 0x53c, 0x3, 
    0x2, 0x2, 0x2, 0x53e, 0x53f, 0x3, 0x2, 0x2, 0x2, 0x53f, 0x540, 0x7, 
    0x46, 0x2, 0x2, 0x540, 0x9b, 0x3, 0x2, 0x2, 0x2, 0x541, 0x542, 0x5, 
    0x9e, 0x50, 0x2, 0x542, 0x9d, 0x3, 0x2, 0x2, 0x2, 0x543, 0x548, 0x5, 
    0xba, 0x5e, 0x2, 0x544, 0x545, 0x7, 0x50, 0x2, 0x2, 0x545, 0x547, 0x5, 
    0xba, 0x5e, 0x2, 0x546, 0x544, 0x3, 0x2, 0x2, 0x2, 0x547, 0x54a, 0x3, 
    0x2, 0x2, 0x2, 0x548, 0x546, 0x3, 0x2, 0x2, 0x2, 0x548, 0x549, 0x3, 
    0x2, 0x2, 0x2, 0x549, 0x9f, 0x3, 0x2, 0x2, 0x2, 0x54a, 0x548, 0x3, 0x2, 
    0x2, 0x2, 0x54b, 0x54c, 0x7, 0x33, 0x2, 0x2, 0x54c, 0x54d, 0x7, 0x9f, 
    0x2, 0x2, 0x54d, 0x54e, 0x7, 0x34, 0x2, 0x2, 0x54e, 0x54f, 0x5, 0xa2, 
    0x52, 0x2, 0x54f, 0xa1, 0x3, 0x2, 0x2, 0x2, 0x550, 0x555, 0x7, 0xc5, 
    0x2, 0x2, 0x551, 0x552, 0x7, 0x52, 0x2, 0x2, 0x552, 0x554, 0x7, 0xc5, 
    0x2, 0x2, 0x553, 0x551, 0x3, 0x2, 0x2, 0x2, 0x554, 0x557, 0x3, 0x2, 
    0x2, 0x2, 0x555, 0x553, 0x3, 0x2, 0x2, 0x2, 0x555, 0x556, 0x3, 0x2, 
    0x2, 0x2, 0x556, 0x558, 0x3, 0x2, 0x2, 0x2, 0x557, 0x555, 0x3, 0x2, 
    0x2, 0x2, 0x558, 0x559, 0x7, 0x4f, 0x2, 0x2, 0x559, 0x55e, 0x7, 0xc5, 
    0x2, 0x2, 0x55a, 0x55b, 0x7, 0x50, 0x2, 0x2, 0x55b, 0x55d, 0x7, 0xc5, 
    0x2, 0x2, 0x55c, 0x55a, 0x3, 0x2, 0x2, 0x2, 0x55d, 0x560, 0x3, 0x2, 
    0x2, 0x2, 0x55e, 0x55c, 0x3, 0x2, 0x2, 0x2, 0x55e, 0x55f, 0x3, 0x2, 
    0x2, 0x2, 0x55f, 0xa3, 0x3, 0x2, 0x2, 0x2, 0x560, 0x55e, 0x3, 0x2, 0x2, 
    0x2, 0x561, 0x562, 0x7, 0xa0, 0x2, 0x2, 0x562, 0x567, 0x7, 0x98, 0x2, 
    0x2, 0x563, 0x564, 0x6, 0x53, 0x16, 0x2, 0x564, 0x565, 0x7, 0x33, 0x2, 
    0x2, 0x565, 0x566, 0x7, 0x21, 0x2, 0x2, 0x566, 0x568, 0x7, 0x34, 0x2, 
    0x2, 0x567, 0x563, 0x3, 0x2, 0x2, 0x2, 0x567, 0x568, 0x3, 0x2, 0x2, 
    0x2, 0x568, 0x56c, 0x3, 0x2, 0x2, 0x2, 0x569, 0x56a, 0x6, 0x53, 0x17, 
    0x2, 0x56a, 0x56b, 0x7, 0xc5, 0x2, 0x2, 0x56b, 0x56d, 0x7, 0x4f, 0x2, 
    0x2, 0x56c, 0x569, 0x3, 0x2, 0x2, 0x2, 0x56c, 0x56d, 0x3, 0x2, 0x2, 
    0x2, 0x56d, 0x572, 0x3, 0x2, 0x2, 0x2, 0x56e, 0x56f, 0x7, 0x9e, 0x2, 
    0x2, 0x56f, 0x570, 0x5, 0xa8, 0x55, 0x2, 0x570, 0x571, 0x7, 0x4f, 0x2, 
    0x2, 0x571, 0x573, 0x3, 0x2, 0x2, 0x2, 0x572, 0x56e, 0x3, 0x2, 0x2, 
    0x2, 0x572, 0x573, 0x3, 0x2, 0x2, 0x2, 0x573, 0x574, 0x3, 0x2, 0x2, 
    0x2, 0x574, 0x575, 0x5, 0xa8, 0x55, 0x2, 0x575, 0x576, 0x7, 0x46, 0x2, 
    0x2, 0x576, 0xa5, 0x3, 0x2, 0x2, 0x2, 0x577, 0x578, 0x7, 0x99, 0x2, 
    0x2, 0x578, 0x579, 0x7, 0x15, 0x2, 0x2, 0x579, 0x57a, 0x5, 0x4, 0x3, 
    0x2, 0x57a, 0x57e, 0x7, 0x47, 0x2, 0x2, 0x57b, 0x57d, 0x5, 0x92, 0x4a, 
    0x2, 0x57c, 0x57b, 0x3, 0x2, 0x2, 0x2, 0x57d, 0x580, 0x3, 0x2, 0x2, 
    0x2, 0x57e, 0x57c, 0x3, 0x2, 0x2, 0x2, 0x57e, 0x57f, 0x3, 0x2, 0x2, 
    0x2, 0x57f, 0x581, 0x3, 0x2, 0x2, 0x2, 0x580, 0x57e, 0x3, 0x2, 0x2, 
    0x2, 0x581, 0x582, 0x7, 0x11, 0x2, 0x2, 0x582, 0x583, 0x7, 0x46, 0x2, 
    0x2, 0x583, 0xa7, 0x3, 0x2, 0x2, 0x2, 0x584, 0x585, 0x5, 0xaa, 0x56, 
    0x2, 0x585, 0xa9, 0x3, 0x2, 0x2, 0x2, 0x586, 0x58c, 0x5, 0xae, 0x58, 
    0x2, 0x587, 0x588, 0x5, 0xac, 0x57, 0x2, 0x588, 0x589, 0x5, 0xae, 0x58, 
    0x2, 0x589, 0x58b, 0x3, 0x2, 0x2, 0x2, 0x58a, 0x587, 0x3, 0x2, 0x2, 
    0x2, 0x58b, 0x58e, 0x3, 0x2, 0x2, 0x2, 0x58c, 0x58a, 0x3, 0x2, 0x2, 
    0x2, 0x58c, 0x58d, 0x3, 0x2, 0x2, 0x2, 0x58d, 0xab, 0x3, 0x2, 0x2, 0x2, 
    0x58e, 0x58c, 0x3, 0x2, 0x2, 0x2, 0x58f, 0x597, 0x7, 0x78, 0x2, 0x2, 
    0x590, 0x591, 0x6, 0x57, 0x18, 0x2, 0x591, 0x597, 0x7, 0x53, 0x2, 0x2, 
    0x592, 0x593, 0x6, 0x57, 0x19, 0x2, 0x593, 0x597, 0x7, 0x54, 0x2, 0x2, 
    0x594, 0x595, 0x6, 0x57, 0x1a, 0x2, 0x595, 0x597, 0x7, 0x42, 0x2, 0x2, 
    0x596, 0x58f, 0x3, 0x2, 0x2, 0x2, 0x596, 0x590, 0x3, 0x2, 0x2, 0x2, 
    0x596, 0x592, 0x3, 0x2, 0x2, 0x2, 0x596, 0x594, 0x3, 0x2, 0x2, 0x2, 
    0x597, 0xad, 0x3, 0x2, 0x2, 0x2, 0x598, 0x59e, 0x5, 0xb2, 0x5a, 0x2, 
    0x599, 0x59a, 0x5, 0xb0, 0x59, 0x2, 0x59a, 0x59b, 0x5, 0xb2, 0x5a, 0x2, 
    0x59b, 0x59d, 0x3, 0x2, 0x2, 0x2, 0x59c, 0x599, 0x3, 0x2, 0x2, 0x2, 
    0x59d, 0x5a0, 0x3, 0x2, 0x2, 0x2, 0x59e, 0x59c, 0x3, 0x2, 0x2, 0x2, 
    0x59e, 0x59f, 0x3, 0x2, 0x2, 0x2, 0x59f, 0xaf, 0x3, 0x2, 0x2, 0x2, 0x5a0, 
    0x59e, 0x3, 0x2, 0x2, 0x2, 0x5a1, 0x5a7, 0x7, 0xa1, 0x2, 0x2, 0x5a2, 
    0x5a3, 0x6, 0x59, 0x1b, 0x2, 0x5a3, 0x5a7, 0x7, 0x39, 0x2, 0x2, 0x5a4, 
    0x5a5, 0x6, 0x59, 0x1c, 0x2, 0x5a5, 0x5a7, 0x7, 0x3a, 0x2, 0x2, 0x5a6, 
    0x5a1, 0x3, 0x2, 0x2, 0x2, 0x5a6, 0x5a2, 0x3, 0x2, 0x2, 0x2, 0x5a6, 
    0x5a4, 0x3, 0x2, 0x2, 0x2, 0x5a7, 0xb1, 0x3, 0x2, 0x2, 0x2, 0x5a8, 0x5ac, 
    0x5, 0xb4, 0x5b, 0x2, 0x5a9, 0x5aa, 0x5, 0xb6, 0x5c, 0x2, 0x5aa, 0x5ab, 
    0x5, 0xb4, 0x5b, 0x2, 0x5ab, 0x5ad, 0x3, 0x2, 0x2, 0x2, 0x5ac, 0x5a9, 
    0x3, 0x2, 0x2, 0x2, 0x5ac, 0x5ad, 0x3, 0x2, 0x2, 0x2, 0x5ad, 0xb3, 0x3, 
    0x2, 0x2, 0x2, 0x5ae, 0x5bc, 0x5, 0xb8, 0x5d, 0x2, 0x5af, 0x5b1, 0x7, 
    0xa2, 0x2, 0x2, 0x5b0, 0x5af, 0x3, 0x2, 0x2, 0x2, 0x5b0, 0x5b1, 0x3, 
    0x2, 0x2, 0x2, 0x5b1, 0x5b2, 0x3, 0x2, 0x2, 0x2, 0x5b2, 0x5b3, 0x7, 
    0x33, 0x2, 0x2, 0x5b3, 0x5b4, 0x5, 0xa8, 0x55, 0x2, 0x5b4, 0x5b5, 0x7, 
    0x34, 0x2, 0x2, 0x5b5, 0x5bc, 0x3, 0x2, 0x2, 0x2, 0x5b6, 0x5b7, 0x7, 
    0xa6, 0x2, 0x2, 0x5b7, 0x5b8, 0x7, 0x33, 0x2, 0x2, 0x5b8, 0x5b9, 0x5, 
    0xb8, 0x5d, 0x2, 0x5b9, 0x5ba, 0x7, 0x34, 0x2, 0x2, 0x5ba, 0x5bc, 0x3, 
    0x2, 0x2, 0x2, 0x5bb, 0x5ae, 0x3, 0x2, 0x2, 0x2, 0x5bb, 0x5b0, 0x3, 
    0x2, 0x2, 0x2, 0x5bb, 0x5b6, 0x3, 0x2, 0x2, 0x2, 0x5bc, 0xb5, 0x3, 0x2, 
    0x2, 0x2, 0x5bd, 0x5be, 0x9, 0xf, 0x2, 0x2, 0x5be, 0xb7, 0x3, 0x2, 0x2, 
    0x2, 0x5bf, 0x5ce, 0x5, 0xba, 0x5e, 0x2, 0x5c0, 0x5c4, 0x5, 0xd8, 0x6d, 
    0x2, 0x5c1, 0x5c2, 0x7, 0xa7, 0x2, 0x2, 0x5c2, 0x5c4, 0x5, 0x4, 0x3, 
    0x2, 0x5c3, 0x5c0, 0x3, 0x2, 0x2, 0x2, 0x5c3, 0x5c1, 0x3, 0x2, 0x2, 
    0x2, 0x5c4, 0x5c7, 0x3, 0x2, 0x2, 0x2, 0x5c5, 0x5c6, 0x7, 0x15, 0x2, 
    0x2, 0x5c6, 0x5c8, 0x5, 0xba, 0x5e, 0x2, 0x5c7, 0x5c5, 0x3, 0x2, 0x2, 
    0x2, 0x5c7, 0x5c8, 0x3, 0x2, 0x2, 0x2, 0x5c8, 0x5ce, 0x3, 0x2, 0x2, 
    0x2, 0x5c9, 0x5ce, 0x5, 0xc2, 0x62, 0x2, 0x5ca, 0x5cb, 0x7, 0x59, 0x2, 
    0x2, 0x5cb, 0x5ce, 0x5, 0x4, 0x3, 0x2, 0x5cc, 0x5ce, 0x5, 0x36, 0x1c, 
    0x2, 0x5cd, 0x5bf, 0x3, 0x2, 0x2, 0x2, 0x5cd, 0x5c3, 0x3, 0x2, 0x2, 
    0x2, 0x5cd, 0x5c9, 0x3, 0x2, 0x2, 0x2, 0x5cd, 0x5ca, 0x3, 0x2, 0x2, 
    0x2, 0x5cd, 0x5cc, 0x3, 0x2, 0x2, 0x2, 0x5ce, 0xb9, 0x3, 0x2, 0x2, 0x2, 
    0x5cf, 0x5d4, 0x5, 0xbe, 0x60, 0x2, 0x5d0, 0x5d1, 0x7, 0x52, 0x2, 0x2, 
    0x5d1, 0x5d3, 0x5, 0xbe, 0x60, 0x2, 0x5d2, 0x5d0, 0x3, 0x2, 0x2, 0x2, 
    0x5d3, 0x5d6, 0x3, 0x2, 0x2, 0x2, 0x5d4, 0x5d2, 0x3, 0x2, 0x2, 0x2, 
    0x5d4, 0x5d5, 0x3, 0x2, 0x2, 0x2, 0x5d5, 0xbb, 0x3, 0x2, 0x2, 0x2, 0x5d6, 
    0x5d4, 0x3, 0x2, 0x2, 0x2, 0x5d7, 0x5d8, 0x5, 0xba, 0x5e, 0x2, 0x5d8, 
    0xbd, 0x3, 0x2, 0x2, 0x2, 0x5d9, 0x5df, 0x7, 0xa8, 0x2, 0x2, 0x5da, 
    0x5df, 0x7, 0xa9, 0x2, 0x2, 0x5db, 0x5df, 0x7, 0xaa, 0x2, 0x2, 0x5dc, 
    0x5df, 0x7, 0xab, 0x2, 0x2, 0x5dd, 0x5df, 0x5, 0xc0, 0x61, 0x2, 0x5de, 
    0x5d9, 0x3, 0x2, 0x2, 0x2, 0x5de, 0x5da, 0x3, 0x2, 0x2, 0x2, 0x5de, 
    0x5db, 0x3, 0x2, 0x2, 0x2, 0x5de, 0x5dc, 0x3, 0x2, 0x2, 0x2, 0x5de, 
    0x5dd, 0x3, 0x2, 0x2, 0x2, 0x5df, 0xbf, 0x3, 0x2, 0x2, 0x2, 0x5e0, 0x5e2, 
    0x7, 0x3b, 0x2, 0x2, 0x5e1, 0x5e0, 0x3, 0x2, 0x2, 0x2, 0x5e1, 0x5e2, 
    0x3, 0x2, 0x2, 0x2, 0x5e2, 0x5ef, 0x3, 0x2, 0x2, 0x2, 0x5e3, 0x5ec, 
    0x7, 0xc5, 0x2, 0x2, 0x5e4, 0x5e9, 0x7, 0x35, 0x2, 0x2, 0x5e5, 0x5ea, 
    0x7, 0xac, 0x2, 0x2, 0x5e6, 0x5ea, 0x7, 0xad, 0x2, 0x2, 0x5e7, 0x5ea, 
    0x7, 0xc0, 0x2, 0x2, 0x5e8, 0x5ea, 0x7, 0xc5, 0x2, 0x2, 0x5e9, 0x5e5, 
    0x3, 0x2, 0x2, 0x2, 0x5e9, 0x5e6, 0x3, 0x2, 0x2, 0x2, 0x5e9, 0x5e7, 
    0x3, 0x2, 0x2, 0x2, 0x5e9, 0x5e8, 0x3, 0x2, 0x2, 0x2, 0x5ea, 0x5eb, 
    0x3, 0x2, 0x2, 0x2, 0x5eb, 0x5ed, 0x7, 0x36, 0x2, 0x2, 0x5ec, 0x5e4, 
    0x3, 0x2, 0x2, 0x2, 0x5ec, 0x5ed, 0x3, 0x2, 0x2, 0x2, 0x5ed, 0x5f0, 
    0x3, 0x2, 0x2, 0x2, 0x5ee, 0x5f0, 0x7, 0xae, 0x2, 0x2, 0x5ef, 0x5e3, 
    0x3, 0x2, 0x2, 0x2, 0x5ef, 0x5ee, 0x3, 0x2, 0x2, 0x2, 0x5f0, 0xc1, 0x3, 
    0x2, 0x2, 0x2, 0x5f1, 0x5f2, 0x5, 0x4, 0x3, 0x2, 0x5f2, 0x5fb, 0x7, 
    0x33, 0x2, 0x2, 0x5f3, 0x5f8, 0x5, 0xc4, 0x63, 0x2, 0x5f4, 0x5f5, 0x7, 
    0x50, 0x2, 0x2, 0x5f5, 0x5f7, 0x5, 0xc4, 0x63, 0x2, 0x5f6, 0x5f4, 0x3, 
    0x2, 0x2, 0x2, 0x5f7, 0x5fa, 0x3, 0x2, 0x2, 0x2, 0x5f8, 0x5f6, 0x3, 
    0x2, 0x2, 0x2, 0x5f8, 0x5f9, 0x3, 0x2, 0x2, 0x2, 0x5f9, 0x5fc, 0x3, 
    0x2, 0x2, 0x2, 0x5fa, 0x5f8, 0x3, 0x2, 0x2, 0x2, 0x5fb, 0x5f3, 0x3, 
    0x2, 0x2, 0x2, 0x5fb, 0x5fc, 0x3, 0x2, 0x2, 0x2, 0x5fc, 0x5fd, 0x3, 
    0x2, 0x2, 0x2, 0x5fd, 0x5fe, 0x7, 0x34, 0x2, 0x2, 0x5fe, 0xc3, 0x3, 
    0x2, 0x2, 0x2, 0x5ff, 0x60c, 0x5, 0xa8, 0x55, 0x2, 0x600, 0x608, 0x7, 
    0x71, 0x2, 0x2, 0x601, 0x602, 0x7, 0x33, 0x2, 0x2, 0x602, 0x607, 0x5, 
    0x22, 0x12, 0x2, 0x603, 0x604, 0x5, 0x4, 0x3, 0x2, 0x604, 0x605, 0x7, 
    0x34, 0x2, 0x2, 0x605, 0x607, 0x3, 0x2, 0x2, 0x2, 0x606, 0x601, 0x3, 
    0x2, 0x2, 0x2, 0x606, 0x603, 0x3, 0x2, 0x2, 0x2, 0x607, 0x60a, 0x3, 
    0x2, 0x2, 0x2, 0x608, 0x606, 0x3, 0x2, 0x2, 0x2, 0x608, 0x609, 0x3, 
    0x2, 0x2, 0x2, 0x609, 0x60c, 0x3, 0x2, 0x2, 0x2, 0x60a, 0x608, 0x3, 
    0x2, 0x2, 0x2, 0x60b, 0x5ff, 0x3, 0x2, 0x2, 0x2, 0x60b, 0x600, 0x3, 
    0x2, 0x2, 0x2, 0x60c, 0xc5, 0x3, 0x2, 0x2, 0x2, 0x60d, 0x60e, 0x7, 0x94, 
    0x2, 0x2, 0x60e, 0x60f, 0x7, 0xc5, 0x2, 0x2, 0x60f, 0x618, 0x7, 0x33, 
    0x2, 0x2, 0x610, 0x615, 0x5, 0xc8, 0x65, 0x2, 0x611, 0x612, 0x7, 0x46, 
    0x2, 0x2, 0x612, 0x614, 0x5, 0xc8, 0x65, 0x2, 0x613, 0x611, 0x3, 0x2, 
    0x2, 0x2, 0x614, 0x617, 0x3, 0x2, 0x2, 0x2, 0x615, 0x613, 0x3, 0x2, 
    0x2, 0x2, 0x615, 0x616, 0x3, 0x2, 0x2, 0x2, 0x616, 0x619, 0x3, 0x2, 
    0x2, 0x2, 0x617, 0x615, 0x3, 0x2, 0x2, 0x2, 0x618, 0x610, 0x3, 0x2, 
    0x2, 0x2, 0x618, 0x619, 0x3, 0x2, 0x2, 0x2, 0x619, 0x61a, 0x3, 0x2, 
    0x2, 0x2, 0x61a, 0x61b, 0x7, 0x34, 0x2, 0x2, 0x61b, 0x61c, 0x7, 0x4f, 
    0x2, 0x2, 0x61c, 0x61e, 0x5, 0xca, 0x66, 0x2, 0x61d, 0x61f, 0x7, 0xbd, 
    0x2, 0x2, 0x61e, 0x61d, 0x3, 0x2, 0x2, 0x2, 0x61e, 0x61f, 0x3, 0x2, 
    0x2, 0x2, 0x61f, 0x620, 0x3, 0x2, 0x2, 0x2, 0x620, 0x621, 0x7, 0x46, 
    0x2, 0x2, 0x621, 0xc7, 0x3, 0x2, 0x2, 0x2, 0x622, 0x623, 0x7, 0xc5, 
    0x2, 0x2, 0x623, 0x624, 0x7, 0x4f, 0x2, 0x2, 0x624, 0x625, 0x5, 0xca, 
    0x66, 0x2, 0x625, 0xc9, 0x3, 0x2, 0x2, 0x2, 0x626, 0x630, 0x5, 0x1a, 
    0xe, 0x2, 0x627, 0x628, 0x9, 0x10, 0x2, 0x2, 0x628, 0x62b, 0x7, 0x15, 
    0x2, 0x2, 0x629, 0x62c, 0x5, 0x22, 0x12, 0x2, 0x62a, 0x62c, 0x5, 0x4, 
    0x3, 0x2, 0x62b, 0x629, 0x3, 0x2, 0x2, 0x2, 0x62b, 0x62a, 0x3, 0x2, 
    0x2, 0x2, 0x62c, 0x630, 0x3, 0x2, 0x2, 0x2, 0x62d, 0x630, 0x7, 0xb0, 
    0x2, 0x2, 0x62e, 0x630, 0x7, 0xb1, 0x2, 0x2, 0x62f, 0x626, 0x3, 0x2, 
    0x2, 0x2, 0x62f, 0x627, 0x3, 0x2, 0x2, 0x2, 0x62f, 0x62d, 0x3, 0x2, 
    0x2, 0x2, 0x62f, 0x62e, 0x3, 0x2, 0x2, 0x2, 0x630, 0xcb, 0x3, 0x2, 0x2, 
    0x2, 0x631, 0x632, 0x7, 0x1f, 0x2, 0x2, 0x632, 0x634, 0x7, 0xc5, 0x2, 
    0x2, 0x633, 0x635, 0x5, 0x8c, 0x47, 0x2, 0x634, 0x633, 0x3, 0x2, 0x2, 
    0x2, 0x634, 0x635, 0x3, 0x2, 0x2, 0x2, 0x635, 0x63b, 0x3, 0x2, 0x2, 
    0x2, 0x636, 0x638, 0x5, 0xce, 0x68, 0x2, 0x637, 0x636, 0x3, 0x2, 0x2, 
    0x2, 0x637, 0x638, 0x3, 0x2, 0x2, 0x2, 0x638, 0x63c, 0x3, 0x2, 0x2, 
    0x2, 0x639, 0x63a, 0x7, 0x20, 0x2, 0x2, 0x63a, 0x63c, 0x5, 0x4, 0x3, 
    0x2, 0x63b, 0x637, 0x3, 0x2, 0x2, 0x2, 0x63b, 0x639, 0x3, 0x2, 0x2, 
    0x2, 0x63c, 0x640, 0x3, 0x2, 0x2, 0x2, 0x63d, 0x63f, 0x5, 0xdc, 0x6f, 
    0x2, 0x63e, 0x63d, 0x3, 0x2, 0x2, 0x2, 0x63f, 0x642, 0x3, 0x2, 0x2, 
    0x2, 0x640, 0x63e, 0x3, 0x2, 0x2, 0x2, 0x640, 0x641, 0x3, 0x2, 0x2, 
    0x2, 0x641, 0x646, 0x3, 0x2, 0x2, 0x2, 0x642, 0x640, 0x3, 0x2, 0x2, 
    0x2, 0x643, 0x645, 0x5, 0xde, 0x70, 0x2, 0x644, 0x643, 0x3, 0x2, 0x2, 
    0x2, 0x645, 0x648, 0x3, 0x2, 0x2, 0x2, 0x646, 0x644, 0x3, 0x2, 0x2, 
    0x2, 0x646, 0x647, 0x3, 0x2, 0x2, 0x2, 0x647, 0x649, 0x3, 0x2, 0x2, 
    0x2, 0x648, 0x646, 0x3, 0x2, 0x2, 0x2, 0x649, 0x64b, 0x7, 0x47, 0x2, 
    0x2, 0x64a, 0x64c, 0x7, 0x2e, 0x2, 0x2, 0x64b, 0x64a, 0x3, 0x2, 0x2, 
    0x2, 0x64b, 0x64c, 0x3, 0x2, 0x2, 0x2, 0x64c, 0x650, 0x3, 0x2, 0x2, 
    0x2, 0x64d, 0x64f, 0x5, 0xe0, 0x71, 0x2, 0x64e, 0x64d, 0x3, 0x2, 0x2, 
    0x2, 0x64f, 0x652, 0x3, 0x2, 0x2, 0x2, 0x650, 0x64e, 0x3, 0x2, 0x2, 
    0x2, 0x650, 0x651, 0x3, 0x2, 0x2, 0x2, 0x651, 0x656, 0x3, 0x2, 0x2, 
    0x2, 0x652, 0x650, 0x3, 0x2, 0x2, 0x2, 0x653, 0x655, 0x5, 0x92, 0x4a, 
    0x2, 0x654, 0x653, 0x3, 0x2, 0x2, 0x2, 0x655, 0x658, 0x3, 0x2, 0x2, 
    0x2, 0x656, 0x654, 0x3, 0x2, 0x2, 0x2, 0x656, 0x657, 0x3, 0x2, 0x2, 
    0x2, 0x657, 0x659, 0x3, 0x2, 0x2, 0x2, 0x658, 0x656, 0x3, 0x2, 0x2, 
    0x2, 0x659, 0x65a, 0x7, 0x11, 0x2, 0x2, 0x65a, 0x65b, 0x7, 0xc5, 0x2, 
    0x2, 0x65b, 0x65c, 0x7, 0x46, 0x2, 0x2, 0x65c, 0xcd, 0x3, 0x2, 0x2, 
    0x2, 0x65d, 0x663, 0x5, 0xd0, 0x69, 0x2, 0x65e, 0x663, 0x5, 0xd2, 0x6a, 
    0x2, 0x65f, 0x663, 0x5, 0xd4, 0x6b, 0x2, 0x660, 0x663, 0x5, 0xd6, 0x6c, 
    0x2, 0x661, 0x663, 0x5, 0xd8, 0x6d, 0x2, 0x662, 0x65d, 0x3, 0x2, 0x2, 
    0x2, 0x662, 0x65e, 0x3, 0x2, 0x2, 0x2, 0x662, 0x65f, 0x3, 0x2, 0x2, 
    0x2, 0x662, 0x660, 0x3, 0x2, 0x2, 0x2, 0x662, 0x661, 0x3, 0x2, 0x2, 
    0x2, 0x663, 0x664, 0x3, 0x2, 0x2, 0x2, 0x664, 0x665, 0x7, 0x46, 0x2, 
    0x2, 0x665, 0xcf, 0x3, 0x2, 0x2, 0x2, 0x666, 0x667, 0x7, 0xb2, 0x2, 
    0x2, 0x667, 0x668, 0x7, 0x15, 0x2, 0x2, 0x668, 0x669, 0x5, 0xda, 0x6e, 
    0x2, 0x669, 0xd1, 0x3, 0x2, 0x2, 0x2, 0x66a, 0x66b, 0x7, 0xb3, 0x2, 
    0x2, 0x66b, 0x66c, 0x7, 0x15, 0x2, 0x2, 0x66c, 0x675, 0x5, 0xda, 0x6e, 
    0x2, 0x66d, 0x66e, 0x7, 0x50, 0x2, 0x2, 0x66e, 0x673, 0x5, 0xda, 0x6e, 
    0x2, 0x66f, 0x670, 0x7, 0x33, 0x2, 0x2, 0x670, 0x671, 0x7, 0x78, 0x2, 
    0x2, 0x671, 0x672, 0x7, 0xb4, 0x2, 0x2, 0x672, 0x674, 0x7, 0x34, 0x2, 
    0x2, 0x673, 0x66f, 0x3, 0x2, 0x2, 0x2, 0x673, 0x674, 0x3, 0x2, 0x2, 
    0x2, 0x674, 0x676, 0x3, 0x2, 0x2, 0x2, 0x675, 0x66d, 0x3, 0x2, 0x2, 
    0x2, 0x676, 0x677, 0x3, 0x2, 0x2, 0x2, 0x677, 0x675, 0x3, 0x2, 0x2, 
    0x2, 0x677, 0x678, 0x3, 0x2, 0x2, 0x2, 0x678, 0xd3, 0x3, 0x2, 0x2, 0x2, 
    0x679, 0x67a, 0x7, 0xb5, 0x2, 0x2, 0x67a, 0x67b, 0x7, 0x15, 0x2, 0x2, 
    0x67b, 0x680, 0x5, 0xda, 0x6e, 0x2, 0x67c, 0x67d, 0x7, 0x50, 0x2, 0x2, 
    0x67d, 0x67f, 0x5, 0xda, 0x6e, 0x2, 0x67e, 0x67c, 0x3, 0x2, 0x2, 0x2, 
    0x67f, 0x682, 0x3, 0x2, 0x2, 0x2, 0x680, 0x67e, 0x3, 0x2, 0x2, 0x2, 
    0x680, 0x681, 0x3, 0x2, 0x2, 0x2, 0x681, 0xd5, 0x3, 0x2, 0x2, 0x2, 0x682, 
    0x680, 0x3, 0x2, 0x2, 0x2, 0x683, 0x684, 0x7, 0xb6, 0x2, 0x2, 0x684, 
    0x685, 0x7, 0x15, 0x2, 0x2, 0x685, 0x68c, 0x5, 0xda, 0x6e, 0x2, 0x686, 
    0x68d, 0x7, 0x71, 0x2, 0x2, 0x687, 0x688, 0x7, 0x48, 0x2, 0x2, 0x688, 
    0x689, 0x7, 0x33, 0x2, 0x2, 0x689, 0x68a, 0x5, 0x9e, 0x50, 0x2, 0x68a, 
    0x68b, 0x7, 0x34, 0x2, 0x2, 0x68b, 0x68d, 0x3, 0x2, 0x2, 0x2, 0x68c, 
    0x686, 0x3, 0x2, 0x2, 0x2, 0x68c, 0x687, 0x3, 0x2, 0x2, 0x2, 0x68d, 
    0xd7, 0x3, 0x2, 0x2, 0x2, 0x68e, 0x690, 0x7, 0x89, 0x2, 0x2, 0x68f, 
    0x68e, 0x3, 0x2, 0x2, 0x2, 0x68f, 0x690, 0x3, 0x2, 0x2, 0x2, 0x690, 
    0x691, 0x3, 0x2, 0x2, 0x2, 0x691, 0x692, 0x7, 0xa7, 0x2, 0x2, 0x692, 
    0x693, 0x7, 0x15, 0x2, 0x2, 0x693, 0x696, 0x5, 0xda, 0x6e, 0x2, 0x694, 
    0x695, 0x7, 0x52, 0x2, 0x2, 0x695, 0x697, 0x7, 0xc5, 0x2, 0x2, 0x696, 
    0x694, 0x3, 0x2, 0x2, 0x2, 0x697, 0x698, 0x3, 0x2, 0x2, 0x2, 0x698, 
    0x696, 0x3, 0x2, 0x2, 0x2, 0x698, 0x699, 0x3, 0x2, 0x2, 0x2, 0x699, 
    0xd9, 0x3, 0x2, 0x2, 0x2, 0x69a, 0x69b, 0x7, 0xc5, 0x2, 0x2, 0x69b, 
    0x69d, 0x7, 0x3f, 0x2, 0x2, 0x69c, 0x69a, 0x3, 0x2, 0x2, 0x2, 0x69c, 
    0x69d, 0x3, 0x2, 0x2, 0x2, 0x69d, 0x69e, 0x3, 0x2, 0x2, 0x2, 0x69e, 
    0x69f, 0x5, 0x4, 0x3, 0x2, 0x69f, 0xdb, 0x3, 0x2, 0x2, 0x2, 0x6a0, 0x6a1, 
    0x7, 0xa4, 0x2, 0x2, 0x6a1, 0x6a2, 0x7, 0xc5, 0x2, 0x2, 0x6a2, 0x6a3, 
    0x7, 0x28, 0x2, 0x2, 0x6a3, 0x6a4, 0x7, 0xb7, 0x2, 0x2, 0x6a4, 0x6a9, 
    0x5, 0xda, 0x6e, 0x2, 0x6a5, 0x6a6, 0x7, 0x50, 0x2, 0x2, 0x6a6, 0x6a8, 
    0x5, 0xda, 0x6e, 0x2, 0x6a7, 0x6a5, 0x3, 0x2, 0x2, 0x2, 0x6a8, 0x6ab, 
    0x3, 0x2, 0x2, 0x2, 0x6a9, 0x6a7, 0x3, 0x2, 0x2, 0x2, 0x6a9, 0x6aa, 
    0x3, 0x2, 0x2, 0x2, 0x6aa, 0xdd, 0x3, 0x2, 0x2, 0x2, 0x6ab, 0x6a9, 0x3, 
    0x2, 0x2, 0x2, 0x6ac, 0x6ad, 0x7, 0x9e, 0x2, 0x2, 0x6ad, 0x6ae, 0x5, 
    0xa8, 0x55, 0x2, 0x6ae, 0x6af, 0x7, 0x46, 0x2, 0x2, 0x6af, 0xdf, 0x3, 
    0x2, 0x2, 0x2, 0x6b0, 0x6b1, 0x7, 0x71, 0x2, 0x2, 0x6b1, 0x6b2, 0x7, 
    0x15, 0x2, 0x2, 0x6b2, 0x6b3, 0x7, 0xc5, 0x2, 0x2, 0x6b3, 0x6be, 0x7, 
    0x46, 0x2, 0x2, 0x6b4, 0x6be, 0x5, 0x18, 0xd, 0x2, 0x6b5, 0x6b7, 0x7, 
    0xc5, 0x2, 0x2, 0x6b6, 0x6b8, 0x5, 0x8c, 0x47, 0x2, 0x6b7, 0x6b6, 0x3, 
    0x2, 0x2, 0x2, 0x6b7, 0x6b8, 0x3, 0x2, 0x2, 0x2, 0x6b8, 0x6b9, 0x3, 
    0x2, 0x2, 0x2, 0x6b9, 0x6ba, 0x7, 0x4c, 0x2, 0x2, 0x6ba, 0x6bb, 0x5, 
    0xb8, 0x5d, 0x2, 0x6bb, 0x6bc, 0x7, 0x46, 0x2, 0x2, 0x6bc, 0x6be, 0x3, 
    0x2, 0x2, 0x2, 0x6bd, 0x6b0, 0x3, 0x2, 0x2, 0x2, 0x6bd, 0x6b4, 0x3, 
    0x2, 0x2, 0x2, 0x6bd, 0x6b5, 0x3, 0x2, 0x2, 0x2, 0x6be, 0xe1, 0x3, 0x2, 
    0x2, 0x2, 0x6bf, 0x6c0, 0x7, 0xb8, 0x2, 0x2, 0x6c0, 0x6c2, 0x7, 0xc5, 
    0x2, 0x2, 0x6c1, 0x6c3, 0x5, 0x8c, 0x47, 0x2, 0x6c2, 0x6c1, 0x3, 0x2, 
    0x2, 0x2, 0x6c2, 0x6c3, 0x3, 0x2, 0x2, 0x2, 0x6c3, 0x6c6, 0x3, 0x2, 
    0x2, 0x2, 0x6c4, 0x6c5, 0x7, 0x20, 0x2, 0x2, 0x6c5, 0x6c7, 0x5, 0x4, 
    0x3, 0x2, 0x6c6, 0x6c4, 0x3, 0x2, 0x2, 0x2, 0x6c6, 0x6c7, 0x3, 0x2, 
    0x2, 0x2, 0x6c7, 0x6cb, 0x3, 0x2, 0x2, 0x2, 0x6c8, 0x6c9, 0x7, 0xa3, 
    0x2, 0x2, 0x6c9, 0x6ca, 0x7, 0x1d, 0x2, 0x2, 0x6ca, 0x6cc, 0x5, 0x4, 
    0x3, 0x2, 0x6cb, 0x6c8, 0x3, 0x2, 0x2, 0x2, 0x6cb, 0x6cc, 0x3, 0x2, 
    0x2, 0x2, 0x6cc, 0x6cd, 0x3, 0x2, 0x2, 0x2, 0x6cd, 0x6d1, 0x7, 0x47, 
    0x2, 0x2, 0x6ce, 0x6d0, 0x5, 0xde, 0x70, 0x2, 0x6cf, 0x6ce, 0x3, 0x2, 
    0x2, 0x2, 0x6d0, 0x6d3, 0x3, 0x2, 0x2, 0x2, 0x6d1, 0x6cf, 0x3, 0x2, 
    0x2, 0x2, 0x6d1, 0x6d2, 0x3, 0x2, 0x2, 0x2, 0x6d2, 0x6d7, 0x3, 0x2, 
    0x2, 0x2, 0x6d3, 0x6d1, 0x3, 0x2, 0x2, 0x2, 0x6d4, 0x6d6, 0x5, 0xe4, 
    0x73, 0x2, 0x6d5, 0x6d4, 0x3, 0x2, 0x2, 0x2, 0x6d6, 0x6d9, 0x3, 0x2, 
    0x2, 0x2, 0x6d7, 0x6d5, 0x3, 0x2, 0x2, 0x2, 0x6d7, 0x6d8, 0x3, 0x2, 
    0x2, 0x2, 0x6d8, 0x6da, 0x3, 0x2, 0x2, 0x2, 0x6d9, 0x6d7, 0x3, 0x2, 
    0x2, 0x2, 0x6da, 0x6db, 0x7, 0x11, 0x2, 0x2, 0x6db, 0x6dc, 0x7, 0xc5, 
    0x2, 0x2, 0x6dc, 0x6dd, 0x7, 0x46, 0x2, 0x2, 0x6dd, 0xe3, 0x3, 0x2, 
    0x2, 0x2, 0x6de, 0x6e0, 0x7, 0xc5, 0x2, 0x2, 0x6df, 0x6e1, 0x5, 0x8c, 
    0x47, 0x2, 0x6e0, 0x6df, 0x3, 0x2, 0x2, 0x2, 0x6e0, 0x6e1, 0x3, 0x2, 
    0x2, 0x2, 0x6e1, 0x6e4, 0x3, 0x2, 0x2, 0x2, 0x6e2, 0x6e3, 0x7, 0x15, 
    0x2, 0x2, 0x6e3, 0x6e5, 0x5, 0x4, 0x3, 0x2, 0x6e4, 0x6e2, 0x3, 0x2, 
    0x2, 0x2, 0x6e4, 0x6e5, 0x3, 0x2, 0x2, 0x2, 0x6e5, 0x6e6, 0x3, 0x2, 
    0x2, 0x2, 0x6e6, 0x6e7, 0x7, 0x4f, 0x2, 0x2, 0x6e7, 0x6ec, 0x5, 0xe6, 
    0x74, 0x2, 0x6e8, 0x6e9, 0x7, 0x46, 0x2, 0x2, 0x6e9, 0x6eb, 0x5, 0xe6, 
    0x74, 0x2, 0x6ea, 0x6e8, 0x3, 0x2, 0x2, 0x2, 0x6eb, 0x6ee, 0x3, 0x2, 
    0x2, 0x2, 0x6ec, 0x6ea, 0x3, 0x2, 0x2, 0x2, 0x6ec, 0x6ed, 0x3, 0x2, 
    0x2, 0x2, 0x6ed, 0x6ef, 0x3, 0x2, 0x2, 0x2, 0x6ee, 0x6ec, 0x3, 0x2, 
    0x2, 0x2, 0x6ef, 0x6f0, 0x7, 0x46, 0x2, 0x2, 0x6f0, 0xe5, 0x3, 0x2, 
    0x2, 0x2, 0x6f1, 0x6f2, 0x7, 0x9e, 0x2, 0x2, 0x6f2, 0x6f4, 0x5, 0xa8, 
    0x55, 0x2, 0x6f3, 0x6f1, 0x3, 0x2, 0x2, 0x2, 0x6f3, 0x6f4, 0x3, 0x2, 
    0x2, 0x2, 0x6f4, 0x6f5, 0x3, 0x2, 0x2, 0x2, 0x6f5, 0x6f6, 0x7, 0x33, 
    0x2, 0x2, 0x6f6, 0x6fb, 0x5, 0xe8, 0x75, 0x2, 0x6f7, 0x6f8, 0x7, 0x46, 
    0x2, 0x2, 0x6f8, 0x6fa, 0x5, 0xe8, 0x75, 0x2, 0x6f9, 0x6f7, 0x3, 0x2, 
    0x2, 0x2, 0x6fa, 0x6fd, 0x3, 0x2, 0x2, 0x2, 0x6fb, 0x6f9, 0x3, 0x2, 
    0x2, 0x2, 0x6fb, 0x6fc, 0x3, 0x2, 0x2, 0x2, 0x6fc, 0x6fe, 0x3, 0x2, 
    0x2, 0x2, 0x6fd, 0x6fb, 0x3, 0x2, 0x2, 0x2, 0x6fe, 0x6ff, 0x7, 0x34, 
    0x2, 0x2, 0x6ff, 0xe7, 0x3, 0x2, 0x2, 0x2, 0x700, 0x701, 0x7, 0xc5, 
    0x2, 0x2, 0x701, 0x714, 0x7, 0x4c, 0x2, 0x2, 0x702, 0x703, 0x7, 0x37, 
    0x2, 0x2, 0x703, 0x704, 0x5, 0x88, 0x45, 0x2, 0x704, 0x705, 0x7, 0x38, 
    0x2, 0x2, 0x705, 0x715, 0x3, 0x2, 0x2, 0x2, 0x706, 0x715, 0x5, 0xb8, 
    0x5d, 0x2, 0x707, 0x708, 0x7, 0xb9, 0x2, 0x2, 0x708, 0x709, 0x5, 0xbc, 
    0x5f, 0x2, 0x709, 0x70a, 0x7, 0x33, 0x2, 0x2, 0x70a, 0x70f, 0x5, 0xea, 
    0x76, 0x2, 0x70b, 0x70c, 0x7, 0x50, 0x2, 0x2, 0x70c, 0x70e, 0x5, 0xea, 
    0x76, 0x2, 0x70d, 0x70b, 0x3, 0x2, 0x2, 0x2, 0x70e, 0x711, 0x3, 0x2, 
    0x2, 0x2, 0x70f, 0x70d, 0x3, 0x2, 0x2, 0x2, 0x70f, 0x710, 0x3, 0x2, 
    0x2, 0x2, 0x710, 0x712, 0x3, 0x2, 0x2, 0x2, 0x711, 0x70f, 0x3, 0x2, 
    0x2, 0x2, 0x712, 0x713, 0x7, 0x34, 0x2, 0x2, 0x713, 0x715, 0x3, 0x2, 
    0x2, 0x2, 0x714, 0x702, 0x3, 0x2, 0x2, 0x2, 0x714, 0x706, 0x3, 0x2, 
    0x2, 0x2, 0x714, 0x707, 0x3, 0x2, 0x2, 0x2, 0x715, 0xe9, 0x3, 0x2, 0x2, 
    0x2, 0x716, 0x717, 0x7, 0x37, 0x2, 0x2, 0x717, 0x718, 0x5, 0x88, 0x45, 
    0x2, 0x718, 0x719, 0x7, 0x38, 0x2, 0x2, 0x719, 0x71c, 0x3, 0x2, 0x2, 
    0x2, 0x71a, 0x71c, 0x5, 0x36, 0x1c, 0x2, 0x71b, 0x716, 0x3, 0x2, 0x2, 
    0x2, 0x71b, 0x71a, 0x3, 0x2, 0x2, 0x2, 0x71c, 0x71d, 0x3, 0x2, 0x2, 
    0x2, 0x71d, 0x71e, 0x7, 0xba, 0x2, 0x2, 0x71e, 0x71f, 0x7, 0x9c, 0x2, 
    0x2, 0x71f, 0x720, 0x5, 0xec, 0x77, 0x2, 0x720, 0xeb, 0x3, 0x2, 0x2, 
    0x2, 0x721, 0x724, 0x5, 0x48, 0x25, 0x2, 0x722, 0x723, 0x7, 0x4e, 0x2, 
    0x2, 0x723, 0x725, 0x5, 0x48, 0x25, 0x2, 0x724, 0x722, 0x3, 0x2, 0x2, 
    0x2, 0x724, 0x725, 0x3, 0x2, 0x2, 0x2, 0x725, 0xed, 0x3, 0x2, 0x2, 0x2, 
    0xef, 0xf2, 0xf7, 0xfe, 0x101, 0x108, 0x10e, 0x114, 0x119, 0x120, 0x124, 
    0x12b, 0x133, 0x13a, 0x140, 0x145, 0x153, 0x155, 0x162, 0x168, 0x16d, 
    0x172, 0x176, 0x17f, 0x186, 0x18f, 0x196, 0x1a1, 0x1a6, 0x1b2, 0x1b4, 
    0x1c0, 0x1c4, 0x1cc, 0x1cf, 0x1d9, 0x1dd, 0x1e6, 0x1eb, 0x1f1, 0x1f8, 
    0x1fb, 0x1fe, 0x201, 0x205, 0x20f, 0x212, 0x218, 0x21c, 0x220, 0x227, 
    0x22d, 0x234, 0x23b, 0x23e, 0x247, 0x24e, 0x251, 0x255, 0x25a, 0x262, 
    0x265, 0x26a, 0x26e, 0x273, 0x27b, 0x280, 0x285, 0x28b, 0x28f, 0x295, 
    0x299, 0x2a0, 0x2a5, 0x2af, 0x2b1, 0x2b3, 0x2bb, 0x2c0, 0x2c4, 0x2c9, 
    0x2cc, 0x2d9, 0x2dc, 0x2e2, 0x2f2, 0x2fb, 0x300, 0x305, 0x309, 0x30f, 
    0x31f, 0x324, 0x32b, 0x332, 0x336, 0x33e, 0x343, 0x346, 0x34d, 0x350, 
    0x356, 0x35b, 0x364, 0x36d, 0x371, 0x376, 0x37d, 0x387, 0x392, 0x396, 
    0x399, 0x3a0, 0x3a5, 0x3ac, 0x3b1, 0x3bd, 0x3c7, 0x3cd, 0x3d4, 0x3d6, 
    0x3da, 0x3e5, 0x3ec, 0x3f0, 0x3f2, 0x3f9, 0x3fb, 0x404, 0x409, 0x412, 
    0x417, 0x41e, 0x423, 0x425, 0x428, 0x42c, 0x435, 0x43b, 0x444, 0x44c, 
    0x453, 0x45f, 0x46a, 0x46e, 0x473, 0x475, 0x47e, 0x483, 0x496, 0x498, 
    0x4a1, 0x4a5, 0x4b2, 0x4b7, 0x4be, 0x4c2, 0x4c9, 0x4cf, 0x4d6, 0x4d8, 
    0x4e2, 0x4eb, 0x4f8, 0x4ff, 0x508, 0x515, 0x524, 0x52e, 0x533, 0x539, 
    0x53d, 0x548, 0x555, 0x55e, 0x567, 0x56c, 0x572, 0x57e, 0x58c, 0x596, 
    0x59e, 0x5a6, 0x5ac, 0x5b0, 0x5bb, 0x5c3, 0x5c7, 0x5cd, 0x5d4, 0x5de, 
    0x5e1, 0x5e9, 0x5ec, 0x5ef, 0x5f8, 0x5fb, 0x606, 0x608, 0x60b, 0x615, 
    0x618, 0x61e, 0x62b, 0x62f, 0x634, 0x637, 0x63b, 0x640, 0x646, 0x64b, 
    0x650, 0x656, 0x662, 0x673, 0x677, 0x680, 0x68c, 0x68f, 0x698, 0x69c, 
    0x6a9, 0x6b7, 0x6bd, 0x6c2, 0x6c6, 0x6cb, 0x6d1, 0x6d7, 0x6e0, 0x6e4, 
    0x6ec, 0x6f3, 0x6fb, 0x70f, 0x714, 0x71b, 0x724, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

Ili2Parser::Initializer Ili2Parser::_init;
