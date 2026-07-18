
// Generated from Ili1Parser.g4 by ANTLR 4.7.1


#include "Ili1ParserVisitor.h"

#include "Ili1Parser.h"


using namespace antlrcpp;
using namespace parser;
using namespace antlr4;

Ili1Parser::Ili1Parser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

Ili1Parser::~Ili1Parser() {
  delete _interpreter;
}

std::string Ili1Parser::getGrammarFileName() const {
  return "Ili1Parser.g4";
}

const std::vector<std::string>& Ili1Parser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& Ili1Parser::getVocabulary() const {
  return _vocabulary;
}


//----------------- Interlis1DefContext ------------------------------------------------------------------

Ili1Parser::Interlis1DefContext::Interlis1DefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::Interlis1DefContext::TRANSFER() {
  return getToken(Ili1Parser::TRANSFER, 0);
}

tree::TerminalNode* Ili1Parser::Interlis1DefContext::SEMI() {
  return getToken(Ili1Parser::SEMI, 0);
}

Ili1Parser::ModelDefContext* Ili1Parser::Interlis1DefContext::modelDef() {
  return getRuleContext<Ili1Parser::ModelDefContext>(0);
}

Ili1Parser::FormatEncodingContext* Ili1Parser::Interlis1DefContext::formatEncoding() {
  return getRuleContext<Ili1Parser::FormatEncodingContext>(0);
}

tree::TerminalNode* Ili1Parser::Interlis1DefContext::EOF() {
  return getToken(Ili1Parser::EOF, 0);
}

tree::TerminalNode* Ili1Parser::Interlis1DefContext::NAME() {
  return getToken(Ili1Parser::NAME, 0);
}

Ili1Parser::DomainDefsContext* Ili1Parser::Interlis1DefContext::domainDefs() {
  return getRuleContext<Ili1Parser::DomainDefsContext>(0);
}

Ili1Parser::DerivativesContext* Ili1Parser::Interlis1DefContext::derivatives() {
  return getRuleContext<Ili1Parser::DerivativesContext>(0);
}

std::vector<Ili1Parser::ViewContext *> Ili1Parser::Interlis1DefContext::view() {
  return getRuleContexts<Ili1Parser::ViewContext>();
}

Ili1Parser::ViewContext* Ili1Parser::Interlis1DefContext::view(size_t i) {
  return getRuleContext<Ili1Parser::ViewContext>(i);
}


size_t Ili1Parser::Interlis1DefContext::getRuleIndex() const {
  return Ili1Parser::RuleInterlis1Def;
}

antlrcpp::Any Ili1Parser::Interlis1DefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitInterlis1Def(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::Interlis1DefContext* Ili1Parser::interlis1Def() {
  Interlis1DefContext *_localctx = _tracker.createInstance<Interlis1DefContext>(_ctx, getState());
  enterRule(_localctx, 0, Ili1Parser::RuleInterlis1Def);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(74);
    match(Ili1Parser::TRANSFER);
    setState(75);
    dynamic_cast<Interlis1DefContext *>(_localctx)->transfername = match(Ili1Parser::NAME);
    setState(76);
    match(Ili1Parser::SEMI);
    setState(78);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::ILIDOMAIN) {
      setState(77);
      domainDefs();
    }
    setState(81);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::DERIVATIVES) {
      setState(80);
      derivatives();
    }
    setState(83);
    modelDef();
    setState(87);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili1Parser::VIEW) {
      setState(84);
      view();
      setState(89);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(90);
    formatEncoding();
    setState(91);
    match(Ili1Parser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ModelDefContext ------------------------------------------------------------------

Ili1Parser::ModelDefContext::ModelDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::ModelDefContext::MODEL() {
  return getToken(Ili1Parser::MODEL, 0);
}

tree::TerminalNode* Ili1Parser::ModelDefContext::END() {
  return getToken(Ili1Parser::END, 0);
}

tree::TerminalNode* Ili1Parser::ModelDefContext::DOT() {
  return getToken(Ili1Parser::DOT, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::ModelDefContext::NAME() {
  return getTokens(Ili1Parser::NAME);
}

tree::TerminalNode* Ili1Parser::ModelDefContext::NAME(size_t i) {
  return getToken(Ili1Parser::NAME, i);
}

Ili1Parser::DomainDefsContext* Ili1Parser::ModelDefContext::domainDefs() {
  return getRuleContext<Ili1Parser::DomainDefsContext>(0);
}

std::vector<Ili1Parser::TopicDefContext *> Ili1Parser::ModelDefContext::topicDef() {
  return getRuleContexts<Ili1Parser::TopicDefContext>();
}

Ili1Parser::TopicDefContext* Ili1Parser::ModelDefContext::topicDef(size_t i) {
  return getRuleContext<Ili1Parser::TopicDefContext>(i);
}


size_t Ili1Parser::ModelDefContext::getRuleIndex() const {
  return Ili1Parser::RuleModelDef;
}

antlrcpp::Any Ili1Parser::ModelDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitModelDef(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::ModelDefContext* Ili1Parser::modelDef() {
  ModelDefContext *_localctx = _tracker.createInstance<ModelDefContext>(_ctx, getState());
  enterRule(_localctx, 2, Ili1Parser::RuleModelDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(93);
    match(Ili1Parser::MODEL);
    setState(94);
    dynamic_cast<ModelDefContext *>(_localctx)->modelname1 = match(Ili1Parser::NAME);
    setState(96);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::ILIDOMAIN) {
      setState(95);
      domainDefs();
    }
    setState(99); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(98);
      topicDef();
      setState(101); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == Ili1Parser::TOPIC);
    setState(103);
    match(Ili1Parser::END);
    setState(104);
    dynamic_cast<ModelDefContext *>(_localctx)->modelname2 = match(Ili1Parser::NAME);
    setState(105);
    match(Ili1Parser::DOT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DomainDefsContext ------------------------------------------------------------------

Ili1Parser::DomainDefsContext::DomainDefsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::DomainDefsContext::ILIDOMAIN() {
  return getToken(Ili1Parser::ILIDOMAIN, 0);
}

std::vector<Ili1Parser::DomainDefContext *> Ili1Parser::DomainDefsContext::domainDef() {
  return getRuleContexts<Ili1Parser::DomainDefContext>();
}

Ili1Parser::DomainDefContext* Ili1Parser::DomainDefsContext::domainDef(size_t i) {
  return getRuleContext<Ili1Parser::DomainDefContext>(i);
}


size_t Ili1Parser::DomainDefsContext::getRuleIndex() const {
  return Ili1Parser::RuleDomainDefs;
}

antlrcpp::Any Ili1Parser::DomainDefsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitDomainDefs(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::DomainDefsContext* Ili1Parser::domainDefs() {
  DomainDefsContext *_localctx = _tracker.createInstance<DomainDefsContext>(_ctx, getState());
  enterRule(_localctx, 4, Ili1Parser::RuleDomainDefs);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(107);
    match(Ili1Parser::ILIDOMAIN);
    setState(109); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(108);
      domainDef();
      setState(111); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == Ili1Parser::NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DomainDefContext ------------------------------------------------------------------

Ili1Parser::DomainDefContext::DomainDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::DomainDefContext::EQUAL() {
  return getToken(Ili1Parser::EQUAL, 0);
}

Ili1Parser::TypeContext* Ili1Parser::DomainDefContext::type() {
  return getRuleContext<Ili1Parser::TypeContext>(0);
}

tree::TerminalNode* Ili1Parser::DomainDefContext::SEMI() {
  return getToken(Ili1Parser::SEMI, 0);
}

tree::TerminalNode* Ili1Parser::DomainDefContext::NAME() {
  return getToken(Ili1Parser::NAME, 0);
}


size_t Ili1Parser::DomainDefContext::getRuleIndex() const {
  return Ili1Parser::RuleDomainDef;
}

antlrcpp::Any Ili1Parser::DomainDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitDomainDef(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::DomainDefContext* Ili1Parser::domainDef() {
  DomainDefContext *_localctx = _tracker.createInstance<DomainDefContext>(_ctx, getState());
  enterRule(_localctx, 6, Ili1Parser::RuleDomainDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(113);
    dynamic_cast<DomainDefContext *>(_localctx)->domainname = match(Ili1Parser::NAME);
    setState(114);
    match(Ili1Parser::EQUAL);
    setState(115);
    type();
    setState(116);
    match(Ili1Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TopicDefContext ------------------------------------------------------------------

Ili1Parser::TopicDefContext::TopicDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::TopicDefContext::TOPIC() {
  return getToken(Ili1Parser::TOPIC, 0);
}

tree::TerminalNode* Ili1Parser::TopicDefContext::EQUAL() {
  return getToken(Ili1Parser::EQUAL, 0);
}

tree::TerminalNode* Ili1Parser::TopicDefContext::END() {
  return getToken(Ili1Parser::END, 0);
}

tree::TerminalNode* Ili1Parser::TopicDefContext::DOT() {
  return getToken(Ili1Parser::DOT, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::TopicDefContext::NAME() {
  return getTokens(Ili1Parser::NAME);
}

tree::TerminalNode* Ili1Parser::TopicDefContext::NAME(size_t i) {
  return getToken(Ili1Parser::NAME, i);
}

std::vector<Ili1Parser::TableDefContext *> Ili1Parser::TopicDefContext::tableDef() {
  return getRuleContexts<Ili1Parser::TableDefContext>();
}

Ili1Parser::TableDefContext* Ili1Parser::TopicDefContext::tableDef(size_t i) {
  return getRuleContext<Ili1Parser::TableDefContext>(i);
}

std::vector<Ili1Parser::DomainDefsContext *> Ili1Parser::TopicDefContext::domainDefs() {
  return getRuleContexts<Ili1Parser::DomainDefsContext>();
}

Ili1Parser::DomainDefsContext* Ili1Parser::TopicDefContext::domainDefs(size_t i) {
  return getRuleContext<Ili1Parser::DomainDefsContext>(i);
}


size_t Ili1Parser::TopicDefContext::getRuleIndex() const {
  return Ili1Parser::RuleTopicDef;
}

antlrcpp::Any Ili1Parser::TopicDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitTopicDef(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::TopicDefContext* Ili1Parser::topicDef() {
  TopicDefContext *_localctx = _tracker.createInstance<TopicDefContext>(_ctx, getState());
  enterRule(_localctx, 8, Ili1Parser::RuleTopicDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(118);
    match(Ili1Parser::TOPIC);
    setState(119);
    dynamic_cast<TopicDefContext *>(_localctx)->topicname1 = match(Ili1Parser::NAME);
    setState(120);
    match(Ili1Parser::EQUAL);
    setState(123); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(123);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case Ili1Parser::OPTIONAL:
        case Ili1Parser::TABLE: {
          setState(121);
          tableDef();
          break;
        }

        case Ili1Parser::ILIDOMAIN: {
          setState(122);
          domainDefs();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(125); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << Ili1Parser::ILIDOMAIN)
      | (1ULL << Ili1Parser::OPTIONAL)
      | (1ULL << Ili1Parser::TABLE))) != 0));
    setState(127);
    match(Ili1Parser::END);
    setState(128);
    dynamic_cast<TopicDefContext *>(_localctx)->topicname2 = match(Ili1Parser::NAME);
    setState(129);
    match(Ili1Parser::DOT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TableDefContext ------------------------------------------------------------------

Ili1Parser::TableDefContext::TableDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::TableDefContext::TABLE() {
  return getToken(Ili1Parser::TABLE, 0);
}

tree::TerminalNode* Ili1Parser::TableDefContext::EQUAL() {
  return getToken(Ili1Parser::EQUAL, 0);
}

Ili1Parser::IdentificationsContext* Ili1Parser::TableDefContext::identifications() {
  return getRuleContext<Ili1Parser::IdentificationsContext>(0);
}

tree::TerminalNode* Ili1Parser::TableDefContext::END() {
  return getToken(Ili1Parser::END, 0);
}

tree::TerminalNode* Ili1Parser::TableDefContext::SEMI() {
  return getToken(Ili1Parser::SEMI, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::TableDefContext::NAME() {
  return getTokens(Ili1Parser::NAME);
}

tree::TerminalNode* Ili1Parser::TableDefContext::NAME(size_t i) {
  return getToken(Ili1Parser::NAME, i);
}

tree::TerminalNode* Ili1Parser::TableDefContext::OPTIONAL() {
  return getToken(Ili1Parser::OPTIONAL, 0);
}

std::vector<Ili1Parser::AttributeContext *> Ili1Parser::TableDefContext::attribute() {
  return getRuleContexts<Ili1Parser::AttributeContext>();
}

Ili1Parser::AttributeContext* Ili1Parser::TableDefContext::attribute(size_t i) {
  return getRuleContext<Ili1Parser::AttributeContext>(i);
}


size_t Ili1Parser::TableDefContext::getRuleIndex() const {
  return Ili1Parser::RuleTableDef;
}

antlrcpp::Any Ili1Parser::TableDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitTableDef(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::TableDefContext* Ili1Parser::tableDef() {
  TableDefContext *_localctx = _tracker.createInstance<TableDefContext>(_ctx, getState());
  enterRule(_localctx, 10, Ili1Parser::RuleTableDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(132);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::OPTIONAL) {
      setState(131);
      match(Ili1Parser::OPTIONAL);
    }
    setState(134);
    match(Ili1Parser::TABLE);
    setState(135);
    dynamic_cast<TableDefContext *>(_localctx)->tablename1 = match(Ili1Parser::NAME);
    setState(136);
    match(Ili1Parser::EQUAL);
    setState(138); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(137);
      attribute();
      setState(140); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == Ili1Parser::NAME);
    setState(142);
    identifications();
    setState(143);
    match(Ili1Parser::END);
    setState(144);
    dynamic_cast<TableDefContext *>(_localctx)->tablename2 = match(Ili1Parser::NAME);
    setState(145);
    match(Ili1Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AttributeContext ------------------------------------------------------------------

Ili1Parser::AttributeContext::AttributeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::AttributeContext::COLON() {
  return getToken(Ili1Parser::COLON, 0);
}

tree::TerminalNode* Ili1Parser::AttributeContext::SEMI() {
  return getToken(Ili1Parser::SEMI, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::AttributeContext::NAME() {
  return getTokens(Ili1Parser::NAME);
}

tree::TerminalNode* Ili1Parser::AttributeContext::NAME(size_t i) {
  return getToken(Ili1Parser::NAME, i);
}

Ili1Parser::TypeContext* Ili1Parser::AttributeContext::type() {
  return getRuleContext<Ili1Parser::TypeContext>(0);
}

tree::TerminalNode* Ili1Parser::AttributeContext::RARROW() {
  return getToken(Ili1Parser::RARROW, 0);
}

tree::TerminalNode* Ili1Parser::AttributeContext::OPTIONAL() {
  return getToken(Ili1Parser::OPTIONAL, 0);
}

tree::TerminalNode* Ili1Parser::AttributeContext::EXPLANATION() {
  return getToken(Ili1Parser::EXPLANATION, 0);
}


size_t Ili1Parser::AttributeContext::getRuleIndex() const {
  return Ili1Parser::RuleAttribute;
}

antlrcpp::Any Ili1Parser::AttributeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitAttribute(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::AttributeContext* Ili1Parser::attribute() {
  AttributeContext *_localctx = _tracker.createInstance<AttributeContext>(_ctx, getState());
  enterRule(_localctx, 12, Ili1Parser::RuleAttribute);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(147);
    dynamic_cast<AttributeContext *>(_localctx)->attributename = match(Ili1Parser::NAME);
    setState(148);
    match(Ili1Parser::COLON);
    setState(150);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::OPTIONAL) {
      setState(149);
      match(Ili1Parser::OPTIONAL);
    }
    setState(155);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::DATE:
      case Ili1Parser::TEXT:
      case Ili1Parser::LPAREN:
      case Ili1Parser::LBRACE:
      case Ili1Parser::HALIGNMENT:
      case Ili1Parser::VALIGNMENT:
      case Ili1Parser::POLYLINE:
      case Ili1Parser::SURFACE:
      case Ili1Parser::AREA:
      case Ili1Parser::COORD2:
      case Ili1Parser::COORD3:
      case Ili1Parser::DIM1:
      case Ili1Parser::DIM2:
      case Ili1Parser::RADIANS:
      case Ili1Parser::DEGREES:
      case Ili1Parser::GRADS:
      case Ili1Parser::NAME: {
        setState(152);
        type();
        break;
      }

      case Ili1Parser::RARROW: {
        setState(153);
        match(Ili1Parser::RARROW);
        setState(154);
        dynamic_cast<AttributeContext *>(_localctx)->tablename = match(Ili1Parser::NAME);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(158);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::EXPLANATION) {
      setState(157);
      dynamic_cast<AttributeContext *>(_localctx)->explanation = match(Ili1Parser::EXPLANATION);
    }
    setState(160);
    match(Ili1Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IdentificationsContext ------------------------------------------------------------------

Ili1Parser::IdentificationsContext::IdentificationsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::IdentificationsContext::NO() {
  return getToken(Ili1Parser::NO, 0);
}

tree::TerminalNode* Ili1Parser::IdentificationsContext::IDENT() {
  return getToken(Ili1Parser::IDENT, 0);
}

std::vector<Ili1Parser::IdentificationContext *> Ili1Parser::IdentificationsContext::identification() {
  return getRuleContexts<Ili1Parser::IdentificationContext>();
}

Ili1Parser::IdentificationContext* Ili1Parser::IdentificationsContext::identification(size_t i) {
  return getRuleContext<Ili1Parser::IdentificationContext>(i);
}


size_t Ili1Parser::IdentificationsContext::getRuleIndex() const {
  return Ili1Parser::RuleIdentifications;
}

antlrcpp::Any Ili1Parser::IdentificationsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitIdentifications(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::IdentificationsContext* Ili1Parser::identifications() {
  IdentificationsContext *_localctx = _tracker.createInstance<IdentificationsContext>(_ctx, getState());
  enterRule(_localctx, 14, Ili1Parser::RuleIdentifications);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(170);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::NO: {
        enterOuterAlt(_localctx, 1);
        setState(162);
        match(Ili1Parser::NO);
        setState(163);
        match(Ili1Parser::IDENT);
        break;
      }

      case Ili1Parser::IDENT: {
        enterOuterAlt(_localctx, 2);
        setState(164);
        match(Ili1Parser::IDENT);
        setState(166); 
        _errHandler->sync(this);
        _la = _input->LA(1);
        do {
          setState(165);
          identification();
          setState(168); 
          _errHandler->sync(this);
          _la = _input->LA(1);
        } while (_la == Ili1Parser::NAME);
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

//----------------- IdentificationContext ------------------------------------------------------------------

Ili1Parser::IdentificationContext::IdentificationContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> Ili1Parser::IdentificationContext::NAME() {
  return getTokens(Ili1Parser::NAME);
}

tree::TerminalNode* Ili1Parser::IdentificationContext::NAME(size_t i) {
  return getToken(Ili1Parser::NAME, i);
}

tree::TerminalNode* Ili1Parser::IdentificationContext::SEMI() {
  return getToken(Ili1Parser::SEMI, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::IdentificationContext::COMMA() {
  return getTokens(Ili1Parser::COMMA);
}

tree::TerminalNode* Ili1Parser::IdentificationContext::COMMA(size_t i) {
  return getToken(Ili1Parser::COMMA, i);
}


size_t Ili1Parser::IdentificationContext::getRuleIndex() const {
  return Ili1Parser::RuleIdentification;
}

antlrcpp::Any Ili1Parser::IdentificationContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitIdentification(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::IdentificationContext* Ili1Parser::identification() {
  IdentificationContext *_localctx = _tracker.createInstance<IdentificationContext>(_ctx, getState());
  enterRule(_localctx, 16, Ili1Parser::RuleIdentification);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(172);
    match(Ili1Parser::NAME);
    setState(177);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili1Parser::COMMA) {
      setState(173);
      match(Ili1Parser::COMMA);
      setState(174);
      match(Ili1Parser::NAME);
      setState(179);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(180);
    match(Ili1Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TypeContext ------------------------------------------------------------------

Ili1Parser::TypeContext::TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili1Parser::BaseTypeContext* Ili1Parser::TypeContext::baseType() {
  return getRuleContext<Ili1Parser::BaseTypeContext>(0);
}

Ili1Parser::LineTypeContext* Ili1Parser::TypeContext::lineType() {
  return getRuleContext<Ili1Parser::LineTypeContext>(0);
}

Ili1Parser::AreaTypeContext* Ili1Parser::TypeContext::areaType() {
  return getRuleContext<Ili1Parser::AreaTypeContext>(0);
}

tree::TerminalNode* Ili1Parser::TypeContext::NAME() {
  return getToken(Ili1Parser::NAME, 0);
}


size_t Ili1Parser::TypeContext::getRuleIndex() const {
  return Ili1Parser::RuleType;
}

antlrcpp::Any Ili1Parser::TypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitType(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::TypeContext* Ili1Parser::type() {
  TypeContext *_localctx = _tracker.createInstance<TypeContext>(_ctx, getState());
  enterRule(_localctx, 18, Ili1Parser::RuleType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(186);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::DATE:
      case Ili1Parser::TEXT:
      case Ili1Parser::LPAREN:
      case Ili1Parser::LBRACE:
      case Ili1Parser::HALIGNMENT:
      case Ili1Parser::VALIGNMENT:
      case Ili1Parser::COORD2:
      case Ili1Parser::COORD3:
      case Ili1Parser::DIM1:
      case Ili1Parser::DIM2:
      case Ili1Parser::RADIANS:
      case Ili1Parser::DEGREES:
      case Ili1Parser::GRADS: {
        enterOuterAlt(_localctx, 1);
        setState(182);
        baseType();
        break;
      }

      case Ili1Parser::POLYLINE: {
        enterOuterAlt(_localctx, 2);
        setState(183);
        lineType();
        break;
      }

      case Ili1Parser::SURFACE:
      case Ili1Parser::AREA: {
        enterOuterAlt(_localctx, 3);
        setState(184);
        areaType();
        break;
      }

      case Ili1Parser::NAME: {
        enterOuterAlt(_localctx, 4);
        setState(185);
        dynamic_cast<TypeContext *>(_localctx)->name = match(Ili1Parser::NAME);
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

//----------------- BaseTypeContext ------------------------------------------------------------------

Ili1Parser::BaseTypeContext::BaseTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

Ili1Parser::Coord2Context* Ili1Parser::BaseTypeContext::coord2() {
  return getRuleContext<Ili1Parser::Coord2Context>(0);
}

Ili1Parser::Coord3Context* Ili1Parser::BaseTypeContext::coord3() {
  return getRuleContext<Ili1Parser::Coord3Context>(0);
}

Ili1Parser::Dim1TypeContext* Ili1Parser::BaseTypeContext::dim1Type() {
  return getRuleContext<Ili1Parser::Dim1TypeContext>(0);
}

Ili1Parser::Dim2TypeContext* Ili1Parser::BaseTypeContext::dim2Type() {
  return getRuleContext<Ili1Parser::Dim2TypeContext>(0);
}

Ili1Parser::AngleTypeContext* Ili1Parser::BaseTypeContext::angleType() {
  return getRuleContext<Ili1Parser::AngleTypeContext>(0);
}

Ili1Parser::NumericRangeContext* Ili1Parser::BaseTypeContext::numericRange() {
  return getRuleContext<Ili1Parser::NumericRangeContext>(0);
}

Ili1Parser::TextTypeContext* Ili1Parser::BaseTypeContext::textType() {
  return getRuleContext<Ili1Parser::TextTypeContext>(0);
}

Ili1Parser::DateTypeContext* Ili1Parser::BaseTypeContext::dateType() {
  return getRuleContext<Ili1Parser::DateTypeContext>(0);
}

Ili1Parser::EnumerationTypeContext* Ili1Parser::BaseTypeContext::enumerationType() {
  return getRuleContext<Ili1Parser::EnumerationTypeContext>(0);
}

Ili1Parser::HorizAlignmentContext* Ili1Parser::BaseTypeContext::horizAlignment() {
  return getRuleContext<Ili1Parser::HorizAlignmentContext>(0);
}

Ili1Parser::VertAlignmentContext* Ili1Parser::BaseTypeContext::vertAlignment() {
  return getRuleContext<Ili1Parser::VertAlignmentContext>(0);
}


size_t Ili1Parser::BaseTypeContext::getRuleIndex() const {
  return Ili1Parser::RuleBaseType;
}

antlrcpp::Any Ili1Parser::BaseTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitBaseType(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::BaseTypeContext* Ili1Parser::baseType() {
  BaseTypeContext *_localctx = _tracker.createInstance<BaseTypeContext>(_ctx, getState());
  enterRule(_localctx, 20, Ili1Parser::RuleBaseType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(199);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::COORD2: {
        enterOuterAlt(_localctx, 1);
        setState(188);
        coord2();
        break;
      }

      case Ili1Parser::COORD3: {
        enterOuterAlt(_localctx, 2);
        setState(189);
        coord3();
        break;
      }

      case Ili1Parser::DIM1: {
        enterOuterAlt(_localctx, 3);
        setState(190);
        dim1Type();
        break;
      }

      case Ili1Parser::DIM2: {
        enterOuterAlt(_localctx, 4);
        setState(191);
        dim2Type();
        break;
      }

      case Ili1Parser::RADIANS:
      case Ili1Parser::DEGREES:
      case Ili1Parser::GRADS: {
        enterOuterAlt(_localctx, 5);
        setState(192);
        angleType();
        break;
      }

      case Ili1Parser::LBRACE: {
        enterOuterAlt(_localctx, 6);
        setState(193);
        numericRange();
        break;
      }

      case Ili1Parser::TEXT: {
        enterOuterAlt(_localctx, 7);
        setState(194);
        textType();
        break;
      }

      case Ili1Parser::DATE: {
        enterOuterAlt(_localctx, 8);
        setState(195);
        dateType();
        break;
      }

      case Ili1Parser::LPAREN: {
        enterOuterAlt(_localctx, 9);
        setState(196);
        enumerationType();
        break;
      }

      case Ili1Parser::HALIGNMENT: {
        enterOuterAlt(_localctx, 10);
        setState(197);
        horizAlignment();
        break;
      }

      case Ili1Parser::VALIGNMENT: {
        enterOuterAlt(_localctx, 11);
        setState(198);
        vertAlignment();
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

//----------------- Coord2Context ------------------------------------------------------------------

Ili1Parser::Coord2Context::Coord2Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::Coord2Context::COORD2() {
  return getToken(Ili1Parser::COORD2, 0);
}

std::vector<Ili1Parser::DecimalContext *> Ili1Parser::Coord2Context::decimal() {
  return getRuleContexts<Ili1Parser::DecimalContext>();
}

Ili1Parser::DecimalContext* Ili1Parser::Coord2Context::decimal(size_t i) {
  return getRuleContext<Ili1Parser::DecimalContext>(i);
}


size_t Ili1Parser::Coord2Context::getRuleIndex() const {
  return Ili1Parser::RuleCoord2;
}

antlrcpp::Any Ili1Parser::Coord2Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitCoord2(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::Coord2Context* Ili1Parser::coord2() {
  Coord2Context *_localctx = _tracker.createInstance<Coord2Context>(_ctx, getState());
  enterRule(_localctx, 22, Ili1Parser::RuleCoord2);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(201);
    match(Ili1Parser::COORD2);
    setState(202);
    dynamic_cast<Coord2Context *>(_localctx)->emin = decimal();
    setState(203);
    dynamic_cast<Coord2Context *>(_localctx)->nmin = decimal();
    setState(204);
    dynamic_cast<Coord2Context *>(_localctx)->emax = decimal();
    setState(205);
    dynamic_cast<Coord2Context *>(_localctx)->nmax = decimal();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Coord3Context ------------------------------------------------------------------

Ili1Parser::Coord3Context::Coord3Context(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::Coord3Context::COORD3() {
  return getToken(Ili1Parser::COORD3, 0);
}

std::vector<Ili1Parser::DecimalContext *> Ili1Parser::Coord3Context::decimal() {
  return getRuleContexts<Ili1Parser::DecimalContext>();
}

Ili1Parser::DecimalContext* Ili1Parser::Coord3Context::decimal(size_t i) {
  return getRuleContext<Ili1Parser::DecimalContext>(i);
}


size_t Ili1Parser::Coord3Context::getRuleIndex() const {
  return Ili1Parser::RuleCoord3;
}

antlrcpp::Any Ili1Parser::Coord3Context::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitCoord3(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::Coord3Context* Ili1Parser::coord3() {
  Coord3Context *_localctx = _tracker.createInstance<Coord3Context>(_ctx, getState());
  enterRule(_localctx, 24, Ili1Parser::RuleCoord3);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(207);
    match(Ili1Parser::COORD3);
    setState(208);
    dynamic_cast<Coord3Context *>(_localctx)->emin = decimal();
    setState(209);
    dynamic_cast<Coord3Context *>(_localctx)->nmin = decimal();
    setState(210);
    dynamic_cast<Coord3Context *>(_localctx)->hmin = decimal();
    setState(211);
    dynamic_cast<Coord3Context *>(_localctx)->emax = decimal();
    setState(212);
    dynamic_cast<Coord3Context *>(_localctx)->nmax = decimal();
    setState(213);
    dynamic_cast<Coord3Context *>(_localctx)->hmax = decimal();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumericRangeContext ------------------------------------------------------------------

Ili1Parser::NumericRangeContext::NumericRangeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::NumericRangeContext::LBRACE() {
  return getToken(Ili1Parser::LBRACE, 0);
}

tree::TerminalNode* Ili1Parser::NumericRangeContext::DOTDOT() {
  return getToken(Ili1Parser::DOTDOT, 0);
}

tree::TerminalNode* Ili1Parser::NumericRangeContext::RBRACE() {
  return getToken(Ili1Parser::RBRACE, 0);
}

std::vector<Ili1Parser::DecimalContext *> Ili1Parser::NumericRangeContext::decimal() {
  return getRuleContexts<Ili1Parser::DecimalContext>();
}

Ili1Parser::DecimalContext* Ili1Parser::NumericRangeContext::decimal(size_t i) {
  return getRuleContext<Ili1Parser::DecimalContext>(i);
}


size_t Ili1Parser::NumericRangeContext::getRuleIndex() const {
  return Ili1Parser::RuleNumericRange;
}

antlrcpp::Any Ili1Parser::NumericRangeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitNumericRange(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::NumericRangeContext* Ili1Parser::numericRange() {
  NumericRangeContext *_localctx = _tracker.createInstance<NumericRangeContext>(_ctx, getState());
  enterRule(_localctx, 26, Ili1Parser::RuleNumericRange);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(215);
    match(Ili1Parser::LBRACE);
    setState(216);
    dynamic_cast<NumericRangeContext *>(_localctx)->min = decimal();
    setState(217);
    match(Ili1Parser::DOTDOT);
    setState(218);
    dynamic_cast<NumericRangeContext *>(_localctx)->max = decimal();
    setState(219);
    match(Ili1Parser::RBRACE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Dim1TypeContext ------------------------------------------------------------------

Ili1Parser::Dim1TypeContext::Dim1TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::Dim1TypeContext::DIM1() {
  return getToken(Ili1Parser::DIM1, 0);
}

std::vector<Ili1Parser::DecimalContext *> Ili1Parser::Dim1TypeContext::decimal() {
  return getRuleContexts<Ili1Parser::DecimalContext>();
}

Ili1Parser::DecimalContext* Ili1Parser::Dim1TypeContext::decimal(size_t i) {
  return getRuleContext<Ili1Parser::DecimalContext>(i);
}


size_t Ili1Parser::Dim1TypeContext::getRuleIndex() const {
  return Ili1Parser::RuleDim1Type;
}

antlrcpp::Any Ili1Parser::Dim1TypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitDim1Type(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::Dim1TypeContext* Ili1Parser::dim1Type() {
  Dim1TypeContext *_localctx = _tracker.createInstance<Dim1TypeContext>(_ctx, getState());
  enterRule(_localctx, 28, Ili1Parser::RuleDim1Type);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(221);
    match(Ili1Parser::DIM1);
    setState(222);
    dynamic_cast<Dim1TypeContext *>(_localctx)->min = decimal();
    setState(223);
    dynamic_cast<Dim1TypeContext *>(_localctx)->max = decimal();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Dim2TypeContext ------------------------------------------------------------------

Ili1Parser::Dim2TypeContext::Dim2TypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::Dim2TypeContext::DIM2() {
  return getToken(Ili1Parser::DIM2, 0);
}

std::vector<Ili1Parser::DecimalContext *> Ili1Parser::Dim2TypeContext::decimal() {
  return getRuleContexts<Ili1Parser::DecimalContext>();
}

Ili1Parser::DecimalContext* Ili1Parser::Dim2TypeContext::decimal(size_t i) {
  return getRuleContext<Ili1Parser::DecimalContext>(i);
}


size_t Ili1Parser::Dim2TypeContext::getRuleIndex() const {
  return Ili1Parser::RuleDim2Type;
}

antlrcpp::Any Ili1Parser::Dim2TypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitDim2Type(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::Dim2TypeContext* Ili1Parser::dim2Type() {
  Dim2TypeContext *_localctx = _tracker.createInstance<Dim2TypeContext>(_ctx, getState());
  enterRule(_localctx, 30, Ili1Parser::RuleDim2Type);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(225);
    match(Ili1Parser::DIM2);
    setState(226);
    dynamic_cast<Dim2TypeContext *>(_localctx)->min = decimal();
    setState(227);
    dynamic_cast<Dim2TypeContext *>(_localctx)->max = decimal();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AngleTypeContext ------------------------------------------------------------------

Ili1Parser::AngleTypeContext::AngleTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::AngleTypeContext::RADIANS() {
  return getToken(Ili1Parser::RADIANS, 0);
}

tree::TerminalNode* Ili1Parser::AngleTypeContext::DEGREES() {
  return getToken(Ili1Parser::DEGREES, 0);
}

tree::TerminalNode* Ili1Parser::AngleTypeContext::GRADS() {
  return getToken(Ili1Parser::GRADS, 0);
}

std::vector<Ili1Parser::DecimalContext *> Ili1Parser::AngleTypeContext::decimal() {
  return getRuleContexts<Ili1Parser::DecimalContext>();
}

Ili1Parser::DecimalContext* Ili1Parser::AngleTypeContext::decimal(size_t i) {
  return getRuleContext<Ili1Parser::DecimalContext>(i);
}


size_t Ili1Parser::AngleTypeContext::getRuleIndex() const {
  return Ili1Parser::RuleAngleType;
}

antlrcpp::Any Ili1Parser::AngleTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitAngleType(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::AngleTypeContext* Ili1Parser::angleType() {
  AngleTypeContext *_localctx = _tracker.createInstance<AngleTypeContext>(_ctx, getState());
  enterRule(_localctx, 32, Ili1Parser::RuleAngleType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(229);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << Ili1Parser::RADIANS)
      | (1ULL << Ili1Parser::DEGREES)
      | (1ULL << Ili1Parser::GRADS))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(230);
    dynamic_cast<AngleTypeContext *>(_localctx)->min = decimal();
    setState(231);
    dynamic_cast<AngleTypeContext *>(_localctx)->max = decimal();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TextTypeContext ------------------------------------------------------------------

Ili1Parser::TextTypeContext::TextTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::TextTypeContext::TEXT() {
  return getToken(Ili1Parser::TEXT, 0);
}

tree::TerminalNode* Ili1Parser::TextTypeContext::STAR() {
  return getToken(Ili1Parser::STAR, 0);
}

tree::TerminalNode* Ili1Parser::TextTypeContext::POSNUMBER() {
  return getToken(Ili1Parser::POSNUMBER, 0);
}


size_t Ili1Parser::TextTypeContext::getRuleIndex() const {
  return Ili1Parser::RuleTextType;
}

antlrcpp::Any Ili1Parser::TextTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitTextType(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::TextTypeContext* Ili1Parser::textType() {
  TextTypeContext *_localctx = _tracker.createInstance<TextTypeContext>(_ctx, getState());
  enterRule(_localctx, 34, Ili1Parser::RuleTextType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(233);
    match(Ili1Parser::TEXT);
    setState(234);
    match(Ili1Parser::STAR);
    setState(235);
    dynamic_cast<TextTypeContext *>(_localctx)->numchars = match(Ili1Parser::POSNUMBER);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DateTypeContext ------------------------------------------------------------------

Ili1Parser::DateTypeContext::DateTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::DateTypeContext::DATE() {
  return getToken(Ili1Parser::DATE, 0);
}


size_t Ili1Parser::DateTypeContext::getRuleIndex() const {
  return Ili1Parser::RuleDateType;
}

antlrcpp::Any Ili1Parser::DateTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitDateType(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::DateTypeContext* Ili1Parser::dateType() {
  DateTypeContext *_localctx = _tracker.createInstance<DateTypeContext>(_ctx, getState());
  enterRule(_localctx, 36, Ili1Parser::RuleDateType);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(237);
    dynamic_cast<DateTypeContext *>(_localctx)->date = match(Ili1Parser::DATE);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumerationTypeContext ------------------------------------------------------------------

Ili1Parser::EnumerationTypeContext::EnumerationTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::EnumerationTypeContext::LPAREN() {
  return getToken(Ili1Parser::LPAREN, 0);
}

std::vector<Ili1Parser::EnumElementContext *> Ili1Parser::EnumerationTypeContext::enumElement() {
  return getRuleContexts<Ili1Parser::EnumElementContext>();
}

Ili1Parser::EnumElementContext* Ili1Parser::EnumerationTypeContext::enumElement(size_t i) {
  return getRuleContext<Ili1Parser::EnumElementContext>(i);
}

tree::TerminalNode* Ili1Parser::EnumerationTypeContext::RPAREN() {
  return getToken(Ili1Parser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::EnumerationTypeContext::COMMA() {
  return getTokens(Ili1Parser::COMMA);
}

tree::TerminalNode* Ili1Parser::EnumerationTypeContext::COMMA(size_t i) {
  return getToken(Ili1Parser::COMMA, i);
}


size_t Ili1Parser::EnumerationTypeContext::getRuleIndex() const {
  return Ili1Parser::RuleEnumerationType;
}

antlrcpp::Any Ili1Parser::EnumerationTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitEnumerationType(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::EnumerationTypeContext* Ili1Parser::enumerationType() {
  EnumerationTypeContext *_localctx = _tracker.createInstance<EnumerationTypeContext>(_ctx, getState());
  enterRule(_localctx, 38, Ili1Parser::RuleEnumerationType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(239);
    match(Ili1Parser::LPAREN);
    setState(240);
    enumElement();
    setState(245);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili1Parser::COMMA) {
      setState(241);
      match(Ili1Parser::COMMA);
      setState(242);
      enumElement();
      setState(247);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(248);
    match(Ili1Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- EnumElementContext ------------------------------------------------------------------

Ili1Parser::EnumElementContext::EnumElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::EnumElementContext::NAME() {
  return getToken(Ili1Parser::NAME, 0);
}

Ili1Parser::EnumerationTypeContext* Ili1Parser::EnumElementContext::enumerationType() {
  return getRuleContext<Ili1Parser::EnumerationTypeContext>(0);
}


size_t Ili1Parser::EnumElementContext::getRuleIndex() const {
  return Ili1Parser::RuleEnumElement;
}

antlrcpp::Any Ili1Parser::EnumElementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitEnumElement(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::EnumElementContext* Ili1Parser::enumElement() {
  EnumElementContext *_localctx = _tracker.createInstance<EnumElementContext>(_ctx, getState());
  enterRule(_localctx, 40, Ili1Parser::RuleEnumElement);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(250);
    dynamic_cast<EnumElementContext *>(_localctx)->enumelement = match(Ili1Parser::NAME);
    setState(252);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::LPAREN) {
      setState(251);
      enumerationType();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- HorizAlignmentContext ------------------------------------------------------------------

Ili1Parser::HorizAlignmentContext::HorizAlignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::HorizAlignmentContext::HALIGNMENT() {
  return getToken(Ili1Parser::HALIGNMENT, 0);
}


size_t Ili1Parser::HorizAlignmentContext::getRuleIndex() const {
  return Ili1Parser::RuleHorizAlignment;
}

antlrcpp::Any Ili1Parser::HorizAlignmentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitHorizAlignment(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::HorizAlignmentContext* Ili1Parser::horizAlignment() {
  HorizAlignmentContext *_localctx = _tracker.createInstance<HorizAlignmentContext>(_ctx, getState());
  enterRule(_localctx, 42, Ili1Parser::RuleHorizAlignment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(254);
    match(Ili1Parser::HALIGNMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VertAlignmentContext ------------------------------------------------------------------

Ili1Parser::VertAlignmentContext::VertAlignmentContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::VertAlignmentContext::VALIGNMENT() {
  return getToken(Ili1Parser::VALIGNMENT, 0);
}


size_t Ili1Parser::VertAlignmentContext::getRuleIndex() const {
  return Ili1Parser::RuleVertAlignment;
}

antlrcpp::Any Ili1Parser::VertAlignmentContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitVertAlignment(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::VertAlignmentContext* Ili1Parser::vertAlignment() {
  VertAlignmentContext *_localctx = _tracker.createInstance<VertAlignmentContext>(_ctx, getState());
  enterRule(_localctx, 44, Ili1Parser::RuleVertAlignment);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(256);
    match(Ili1Parser::VALIGNMENT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineTypeContext ------------------------------------------------------------------

Ili1Parser::LineTypeContext::LineTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::LineTypeContext::POLYLINE() {
  return getToken(Ili1Parser::POLYLINE, 0);
}

Ili1Parser::FormContext* Ili1Parser::LineTypeContext::form() {
  return getRuleContext<Ili1Parser::FormContext>(0);
}

Ili1Parser::ControlPointsContext* Ili1Parser::LineTypeContext::controlPoints() {
  return getRuleContext<Ili1Parser::ControlPointsContext>(0);
}

Ili1Parser::IntersectionDefContext* Ili1Parser::LineTypeContext::intersectionDef() {
  return getRuleContext<Ili1Parser::IntersectionDefContext>(0);
}


size_t Ili1Parser::LineTypeContext::getRuleIndex() const {
  return Ili1Parser::RuleLineType;
}

antlrcpp::Any Ili1Parser::LineTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitLineType(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::LineTypeContext* Ili1Parser::lineType() {
  LineTypeContext *_localctx = _tracker.createInstance<LineTypeContext>(_ctx, getState());
  enterRule(_localctx, 46, Ili1Parser::RuleLineType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(258);
    match(Ili1Parser::POLYLINE);
    setState(259);
    form();
    setState(260);
    controlPoints();
    setState(262);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::WITHOUT) {
      setState(261);
      intersectionDef();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AreaTypeContext ------------------------------------------------------------------

Ili1Parser::AreaTypeContext::AreaTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::AreaTypeContext::SURFACE() {
  return getToken(Ili1Parser::SURFACE, 0);
}

Ili1Parser::FormContext* Ili1Parser::AreaTypeContext::form() {
  return getRuleContext<Ili1Parser::FormContext>(0);
}

Ili1Parser::ControlPointsContext* Ili1Parser::AreaTypeContext::controlPoints() {
  return getRuleContext<Ili1Parser::ControlPointsContext>(0);
}

tree::TerminalNode* Ili1Parser::AreaTypeContext::AREA() {
  return getToken(Ili1Parser::AREA, 0);
}

Ili1Parser::IntersectionDefContext* Ili1Parser::AreaTypeContext::intersectionDef() {
  return getRuleContext<Ili1Parser::IntersectionDefContext>(0);
}

Ili1Parser::LineAttributesContext* Ili1Parser::AreaTypeContext::lineAttributes() {
  return getRuleContext<Ili1Parser::LineAttributesContext>(0);
}


size_t Ili1Parser::AreaTypeContext::getRuleIndex() const {
  return Ili1Parser::RuleAreaType;
}

antlrcpp::Any Ili1Parser::AreaTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitAreaType(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::AreaTypeContext* Ili1Parser::areaType() {
  AreaTypeContext *_localctx = _tracker.createInstance<AreaTypeContext>(_ctx, getState());
  enterRule(_localctx, 48, Ili1Parser::RuleAreaType);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(275);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::SURFACE: {
        setState(264);
        match(Ili1Parser::SURFACE);
        setState(265);
        form();
        setState(266);
        controlPoints();
        setState(268);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == Ili1Parser::WITHOUT) {
          setState(267);
          intersectionDef();
        }
        break;
      }

      case Ili1Parser::AREA: {
        setState(270);
        match(Ili1Parser::AREA);
        setState(271);
        form();
        setState(272);
        controlPoints();
        setState(273);
        intersectionDef();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(278);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::LINEATTR) {
      setState(277);
      lineAttributes();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FormContext ------------------------------------------------------------------

Ili1Parser::FormContext::FormContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::FormContext::WITH() {
  return getToken(Ili1Parser::WITH, 0);
}

tree::TerminalNode* Ili1Parser::FormContext::LPAREN() {
  return getToken(Ili1Parser::LPAREN, 0);
}

std::vector<Ili1Parser::LineFormContext *> Ili1Parser::FormContext::lineForm() {
  return getRuleContexts<Ili1Parser::LineFormContext>();
}

Ili1Parser::LineFormContext* Ili1Parser::FormContext::lineForm(size_t i) {
  return getRuleContext<Ili1Parser::LineFormContext>(i);
}

tree::TerminalNode* Ili1Parser::FormContext::RPAREN() {
  return getToken(Ili1Parser::RPAREN, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::FormContext::COMMA() {
  return getTokens(Ili1Parser::COMMA);
}

tree::TerminalNode* Ili1Parser::FormContext::COMMA(size_t i) {
  return getToken(Ili1Parser::COMMA, i);
}


size_t Ili1Parser::FormContext::getRuleIndex() const {
  return Ili1Parser::RuleForm;
}

antlrcpp::Any Ili1Parser::FormContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitForm(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::FormContext* Ili1Parser::form() {
  FormContext *_localctx = _tracker.createInstance<FormContext>(_ctx, getState());
  enterRule(_localctx, 50, Ili1Parser::RuleForm);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(280);
    match(Ili1Parser::WITH);
    setState(281);
    match(Ili1Parser::LPAREN);
    setState(282);
    lineForm();
    setState(287);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili1Parser::COMMA) {
      setState(283);
      match(Ili1Parser::COMMA);
      setState(284);
      lineForm();
      setState(289);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(290);
    match(Ili1Parser::RPAREN);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineFormContext ------------------------------------------------------------------

Ili1Parser::LineFormContext::LineFormContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::LineFormContext::STRAIGHTS() {
  return getToken(Ili1Parser::STRAIGHTS, 0);
}

tree::TerminalNode* Ili1Parser::LineFormContext::ARCS() {
  return getToken(Ili1Parser::ARCS, 0);
}

tree::TerminalNode* Ili1Parser::LineFormContext::EXPLANATION() {
  return getToken(Ili1Parser::EXPLANATION, 0);
}


size_t Ili1Parser::LineFormContext::getRuleIndex() const {
  return Ili1Parser::RuleLineForm;
}

antlrcpp::Any Ili1Parser::LineFormContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitLineForm(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::LineFormContext* Ili1Parser::lineForm() {
  LineFormContext *_localctx = _tracker.createInstance<LineFormContext>(_ctx, getState());
  enterRule(_localctx, 52, Ili1Parser::RuleLineForm);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(295);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::STRAIGHTS: {
        enterOuterAlt(_localctx, 1);
        setState(292);
        match(Ili1Parser::STRAIGHTS);
        break;
      }

      case Ili1Parser::ARCS: {
        enterOuterAlt(_localctx, 2);
        setState(293);
        match(Ili1Parser::ARCS);
        break;
      }

      case Ili1Parser::EXPLANATION: {
        enterOuterAlt(_localctx, 3);
        setState(294);
        dynamic_cast<LineFormContext *>(_localctx)->explanation = match(Ili1Parser::EXPLANATION);
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

//----------------- IntersectionDefContext ------------------------------------------------------------------

Ili1Parser::IntersectionDefContext::IntersectionDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::IntersectionDefContext::WITHOUT() {
  return getToken(Ili1Parser::WITHOUT, 0);
}

tree::TerminalNode* Ili1Parser::IntersectionDefContext::OVERLAPS() {
  return getToken(Ili1Parser::OVERLAPS, 0);
}

tree::TerminalNode* Ili1Parser::IntersectionDefContext::GREATER() {
  return getToken(Ili1Parser::GREATER, 0);
}

Ili1Parser::DecimalContext* Ili1Parser::IntersectionDefContext::decimal() {
  return getRuleContext<Ili1Parser::DecimalContext>(0);
}


size_t Ili1Parser::IntersectionDefContext::getRuleIndex() const {
  return Ili1Parser::RuleIntersectionDef;
}

antlrcpp::Any Ili1Parser::IntersectionDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitIntersectionDef(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::IntersectionDefContext* Ili1Parser::intersectionDef() {
  IntersectionDefContext *_localctx = _tracker.createInstance<IntersectionDefContext>(_ctx, getState());
  enterRule(_localctx, 54, Ili1Parser::RuleIntersectionDef);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(297);
    match(Ili1Parser::WITHOUT);
    setState(298);
    match(Ili1Parser::OVERLAPS);
    setState(299);
    match(Ili1Parser::GREATER);
    setState(300);
    dynamic_cast<IntersectionDefContext *>(_localctx)->maxoverlap = decimal();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ControlPointsContext ------------------------------------------------------------------

Ili1Parser::ControlPointsContext::ControlPointsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::ControlPointsContext::VERTEX() {
  return getToken(Ili1Parser::VERTEX, 0);
}

Ili1Parser::Coord2Context* Ili1Parser::ControlPointsContext::coord2() {
  return getRuleContext<Ili1Parser::Coord2Context>(0);
}

Ili1Parser::Coord3Context* Ili1Parser::ControlPointsContext::coord3() {
  return getRuleContext<Ili1Parser::Coord3Context>(0);
}

tree::TerminalNode* Ili1Parser::ControlPointsContext::NAME() {
  return getToken(Ili1Parser::NAME, 0);
}

tree::TerminalNode* Ili1Parser::ControlPointsContext::BASE() {
  return getToken(Ili1Parser::BASE, 0);
}

tree::TerminalNode* Ili1Parser::ControlPointsContext::EXPLANATION() {
  return getToken(Ili1Parser::EXPLANATION, 0);
}


size_t Ili1Parser::ControlPointsContext::getRuleIndex() const {
  return Ili1Parser::RuleControlPoints;
}

antlrcpp::Any Ili1Parser::ControlPointsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitControlPoints(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::ControlPointsContext* Ili1Parser::controlPoints() {
  ControlPointsContext *_localctx = _tracker.createInstance<ControlPointsContext>(_ctx, getState());
  enterRule(_localctx, 56, Ili1Parser::RuleControlPoints);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(302);
    match(Ili1Parser::VERTEX);
    setState(306);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::COORD2: {
        setState(303);
        coord2();
        break;
      }

      case Ili1Parser::COORD3: {
        setState(304);
        coord3();
        break;
      }

      case Ili1Parser::NAME: {
        setState(305);
        match(Ili1Parser::NAME);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(310);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::BASE) {
      setState(308);
      match(Ili1Parser::BASE);
      setState(309);
      match(Ili1Parser::EXPLANATION);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineAttributesContext ------------------------------------------------------------------

Ili1Parser::LineAttributesContext::LineAttributesContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::LineAttributesContext::LINEATTR() {
  return getToken(Ili1Parser::LINEATTR, 0);
}

tree::TerminalNode* Ili1Parser::LineAttributesContext::EQUAL() {
  return getToken(Ili1Parser::EQUAL, 0);
}

tree::TerminalNode* Ili1Parser::LineAttributesContext::END() {
  return getToken(Ili1Parser::END, 0);
}

std::vector<Ili1Parser::AttributeContext *> Ili1Parser::LineAttributesContext::attribute() {
  return getRuleContexts<Ili1Parser::AttributeContext>();
}

Ili1Parser::AttributeContext* Ili1Parser::LineAttributesContext::attribute(size_t i) {
  return getRuleContext<Ili1Parser::AttributeContext>(i);
}

Ili1Parser::IdentificationsContext* Ili1Parser::LineAttributesContext::identifications() {
  return getRuleContext<Ili1Parser::IdentificationsContext>(0);
}


size_t Ili1Parser::LineAttributesContext::getRuleIndex() const {
  return Ili1Parser::RuleLineAttributes;
}

antlrcpp::Any Ili1Parser::LineAttributesContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitLineAttributes(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::LineAttributesContext* Ili1Parser::lineAttributes() {
  LineAttributesContext *_localctx = _tracker.createInstance<LineAttributesContext>(_ctx, getState());
  enterRule(_localctx, 58, Ili1Parser::RuleLineAttributes);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(312);
    match(Ili1Parser::LINEATTR);
    setState(313);
    match(Ili1Parser::EQUAL);
    setState(315); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(314);
      attribute();
      setState(317); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == Ili1Parser::NAME);
    setState(320);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::NO

    || _la == Ili1Parser::IDENT) {
      setState(319);
      identifications();
    }
    setState(322);
    match(Ili1Parser::END);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DerivativesContext ------------------------------------------------------------------

Ili1Parser::DerivativesContext::DerivativesContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::DerivativesContext::DERIVATIVES() {
  return getToken(Ili1Parser::DERIVATIVES, 0);
}

tree::TerminalNode* Ili1Parser::DerivativesContext::END() {
  return getToken(Ili1Parser::END, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::DerivativesContext::NAME() {
  return getTokens(Ili1Parser::NAME);
}

tree::TerminalNode* Ili1Parser::DerivativesContext::NAME(size_t i) {
  return getToken(Ili1Parser::NAME, i);
}

Ili1Parser::DomainDefsContext* Ili1Parser::DerivativesContext::domainDefs() {
  return getRuleContext<Ili1Parser::DomainDefsContext>(0);
}

std::vector<Ili1Parser::TopicDefContext *> Ili1Parser::DerivativesContext::topicDef() {
  return getRuleContexts<Ili1Parser::TopicDefContext>();
}

Ili1Parser::TopicDefContext* Ili1Parser::DerivativesContext::topicDef(size_t i) {
  return getRuleContext<Ili1Parser::TopicDefContext>(i);
}


size_t Ili1Parser::DerivativesContext::getRuleIndex() const {
  return Ili1Parser::RuleDerivatives;
}

antlrcpp::Any Ili1Parser::DerivativesContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitDerivatives(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::DerivativesContext* Ili1Parser::derivatives() {
  DerivativesContext *_localctx = _tracker.createInstance<DerivativesContext>(_ctx, getState());
  enterRule(_localctx, 60, Ili1Parser::RuleDerivatives);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(324);
    match(Ili1Parser::DERIVATIVES);
    setState(325);
    dynamic_cast<DerivativesContext *>(_localctx)->derivativename1 = match(Ili1Parser::NAME);
    setState(327);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::ILIDOMAIN) {
      setState(326);
      domainDefs();
    }
    setState(330); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(329);
      topicDef();
      setState(332); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while (_la == Ili1Parser::TOPIC);
    setState(334);
    match(Ili1Parser::END);
    setState(335);
    dynamic_cast<DerivativesContext *>(_localctx)->derivativename2 = match(Ili1Parser::NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ViewContext ------------------------------------------------------------------

Ili1Parser::ViewContext::ViewContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::ViewContext::VIEW() {
  return getToken(Ili1Parser::VIEW, 0);
}

tree::TerminalNode* Ili1Parser::ViewContext::END() {
  return getToken(Ili1Parser::END, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::ViewContext::DOT() {
  return getTokens(Ili1Parser::DOT);
}

tree::TerminalNode* Ili1Parser::ViewContext::DOT(size_t i) {
  return getToken(Ili1Parser::DOT, i);
}

std::vector<tree::TerminalNode *> Ili1Parser::ViewContext::NAME() {
  return getTokens(Ili1Parser::NAME);
}

tree::TerminalNode* Ili1Parser::ViewContext::NAME(size_t i) {
  return getToken(Ili1Parser::NAME, i);
}

std::vector<tree::TerminalNode *> Ili1Parser::ViewContext::COLON() {
  return getTokens(Ili1Parser::COLON);
}

tree::TerminalNode* Ili1Parser::ViewContext::COLON(size_t i) {
  return getToken(Ili1Parser::COLON, i);
}

std::vector<Ili1Parser::ViewDefContext *> Ili1Parser::ViewContext::viewDef() {
  return getRuleContexts<Ili1Parser::ViewDefContext>();
}

Ili1Parser::ViewDefContext* Ili1Parser::ViewContext::viewDef(size_t i) {
  return getRuleContext<Ili1Parser::ViewDefContext>(i);
}

std::vector<tree::TerminalNode *> Ili1Parser::ViewContext::SEMI() {
  return getTokens(Ili1Parser::SEMI);
}

tree::TerminalNode* Ili1Parser::ViewContext::SEMI(size_t i) {
  return getToken(Ili1Parser::SEMI, i);
}

std::vector<tree::TerminalNode *> Ili1Parser::ViewContext::COMMA() {
  return getTokens(Ili1Parser::COMMA);
}

tree::TerminalNode* Ili1Parser::ViewContext::COMMA(size_t i) {
  return getToken(Ili1Parser::COMMA, i);
}


size_t Ili1Parser::ViewContext::getRuleIndex() const {
  return Ili1Parser::RuleView;
}

antlrcpp::Any Ili1Parser::ViewContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitView(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::ViewContext* Ili1Parser::view() {
  ViewContext *_localctx = _tracker.createInstance<ViewContext>(_ctx, getState());
  enterRule(_localctx, 62, Ili1Parser::RuleView);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(337);
    match(Ili1Parser::VIEW);
    setState(338);
    dynamic_cast<ViewContext *>(_localctx)->modelname1 = match(Ili1Parser::NAME);
    setState(355);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == Ili1Parser::NAME) {
      setState(339);
      dynamic_cast<ViewContext *>(_localctx)->topicname = match(Ili1Parser::NAME);
      setState(340);
      match(Ili1Parser::DOT);
      setState(341);
      dynamic_cast<ViewContext *>(_localctx)->tablename = match(Ili1Parser::NAME);
      setState(342);
      match(Ili1Parser::COLON);
      setState(343);
      viewDef();
      setState(348);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == Ili1Parser::COMMA) {
        setState(344);
        match(Ili1Parser::COMMA);
        setState(345);
        viewDef();
        setState(350);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(351);
      match(Ili1Parser::SEMI);
      setState(357);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(358);
    match(Ili1Parser::END);
    setState(359);
    dynamic_cast<ViewContext *>(_localctx)->modelname2 = match(Ili1Parser::NAME);
    setState(360);
    match(Ili1Parser::DOT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ViewDefContext ------------------------------------------------------------------

Ili1Parser::ViewDefContext::ViewDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::ViewDefContext::VERTEXINFO() {
  return getToken(Ili1Parser::VERTEXINFO, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::ViewDefContext::NAME() {
  return getTokens(Ili1Parser::NAME);
}

tree::TerminalNode* Ili1Parser::ViewDefContext::NAME(size_t i) {
  return getToken(Ili1Parser::NAME, i);
}

tree::TerminalNode* Ili1Parser::ViewDefContext::EXPLANATION() {
  return getToken(Ili1Parser::EXPLANATION, 0);
}

tree::TerminalNode* Ili1Parser::ViewDefContext::WITH() {
  return getToken(Ili1Parser::WITH, 0);
}

tree::TerminalNode* Ili1Parser::ViewDefContext::PERIPHERY() {
  return getToken(Ili1Parser::PERIPHERY, 0);
}

tree::TerminalNode* Ili1Parser::ViewDefContext::CONTOUR() {
  return getToken(Ili1Parser::CONTOUR, 0);
}

tree::TerminalNode* Ili1Parser::ViewDefContext::LARROW() {
  return getToken(Ili1Parser::LARROW, 0);
}

tree::TerminalNode* Ili1Parser::ViewDefContext::DOT() {
  return getToken(Ili1Parser::DOT, 0);
}


size_t Ili1Parser::ViewDefContext::getRuleIndex() const {
  return Ili1Parser::RuleViewDef;
}

antlrcpp::Any Ili1Parser::ViewDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitViewDef(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::ViewDefContext* Ili1Parser::viewDef() {
  ViewDefContext *_localctx = _tracker.createInstance<ViewDefContext>(_ctx, getState());
  enterRule(_localctx, 64, Ili1Parser::RuleViewDef);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(378);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::VERTEXINFO: {
        enterOuterAlt(_localctx, 1);
        setState(362);
        match(Ili1Parser::VERTEXINFO);
        setState(363);
        dynamic_cast<ViewDefContext *>(_localctx)->vertexinfo = match(Ili1Parser::NAME);
        setState(364);
        dynamic_cast<ViewDefContext *>(_localctx)->explanation = match(Ili1Parser::EXPLANATION);
        break;
      }

      case Ili1Parser::WITH: {
        enterOuterAlt(_localctx, 2);
        setState(365);
        match(Ili1Parser::WITH);
        setState(366);
        match(Ili1Parser::PERIPHERY);
        setState(367);
        dynamic_cast<ViewDefContext *>(_localctx)->periphery = match(Ili1Parser::NAME);
        break;
      }

      case Ili1Parser::CONTOUR: {
        enterOuterAlt(_localctx, 3);
        setState(368);
        match(Ili1Parser::CONTOUR);
        setState(369);
        dynamic_cast<ViewDefContext *>(_localctx)->contour = match(Ili1Parser::NAME);
        setState(372);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if (_la == Ili1Parser::WITH) {
          setState(370);
          match(Ili1Parser::WITH);
          setState(371);
          match(Ili1Parser::PERIPHERY);
        }
        break;
      }

      case Ili1Parser::LARROW: {
        enterOuterAlt(_localctx, 4);
        setState(374);
        match(Ili1Parser::LARROW);
        setState(375);
        dynamic_cast<ViewDefContext *>(_localctx)->table = match(Ili1Parser::NAME);
        setState(376);
        match(Ili1Parser::DOT);
        setState(377);
        dynamic_cast<ViewDefContext *>(_localctx)->attr = match(Ili1Parser::NAME);
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

//----------------- FormatEncodingContext ------------------------------------------------------------------

Ili1Parser::FormatEncodingContext::FormatEncodingContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::FORMAT() {
  return getToken(Ili1Parser::FORMAT, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::FormatEncodingContext::SEMI() {
  return getTokens(Ili1Parser::SEMI);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::SEMI(size_t i) {
  return getToken(Ili1Parser::SEMI, i);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::CODE() {
  return getToken(Ili1Parser::CODE, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::BLANK() {
  return getToken(Ili1Parser::BLANK, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::FormatEncodingContext::EQUAL() {
  return getTokens(Ili1Parser::EQUAL);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::EQUAL(size_t i) {
  return getToken(Ili1Parser::EQUAL, i);
}

std::vector<tree::TerminalNode *> Ili1Parser::FormatEncodingContext::COMMA() {
  return getTokens(Ili1Parser::COMMA);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::COMMA(size_t i) {
  return getToken(Ili1Parser::COMMA, i);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::UNDEFINED() {
  return getToken(Ili1Parser::UNDEFINED, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::CONTINUE() {
  return getToken(Ili1Parser::CONTINUE, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::TID() {
  return getToken(Ili1Parser::TID, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::END() {
  return getToken(Ili1Parser::END, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::DOT() {
  return getToken(Ili1Parser::DOT, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::I16() {
  return getToken(Ili1Parser::I16, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::I32() {
  return getToken(Ili1Parser::I32, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::ANY() {
  return getToken(Ili1Parser::ANY, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::EXPLANATION() {
  return getToken(Ili1Parser::EXPLANATION, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::FREE() {
  return getToken(Ili1Parser::FREE, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::FIX() {
  return getToken(Ili1Parser::FIX, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::WITH() {
  return getToken(Ili1Parser::WITH, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::LINESIZE() {
  return getToken(Ili1Parser::LINESIZE, 0);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::TIDSIZE() {
  return getToken(Ili1Parser::TIDSIZE, 0);
}

std::vector<tree::TerminalNode *> Ili1Parser::FormatEncodingContext::DEFAULT() {
  return getTokens(Ili1Parser::DEFAULT);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::DEFAULT(size_t i) {
  return getToken(Ili1Parser::DEFAULT, i);
}

std::vector<tree::TerminalNode *> Ili1Parser::FormatEncodingContext::POSNUMBER() {
  return getTokens(Ili1Parser::POSNUMBER);
}

tree::TerminalNode* Ili1Parser::FormatEncodingContext::POSNUMBER(size_t i) {
  return getToken(Ili1Parser::POSNUMBER, i);
}

Ili1Parser::FontContext* Ili1Parser::FormatEncodingContext::font() {
  return getRuleContext<Ili1Parser::FontContext>(0);
}

std::vector<Ili1Parser::CodeContext *> Ili1Parser::FormatEncodingContext::code() {
  return getRuleContexts<Ili1Parser::CodeContext>();
}

Ili1Parser::CodeContext* Ili1Parser::FormatEncodingContext::code(size_t i) {
  return getRuleContext<Ili1Parser::CodeContext>(i);
}


size_t Ili1Parser::FormatEncodingContext::getRuleIndex() const {
  return Ili1Parser::RuleFormatEncoding;
}

antlrcpp::Any Ili1Parser::FormatEncodingContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitFormatEncoding(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::FormatEncodingContext* Ili1Parser::formatEncoding() {
  FormatEncodingContext *_localctx = _tracker.createInstance<FormatEncodingContext>(_ctx, getState());
  enterRule(_localctx, 66, Ili1Parser::RuleFormatEncoding);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(380);
    match(Ili1Parser::FORMAT);
    setState(391);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::FREE: {
        setState(381);
        match(Ili1Parser::FREE);
        break;
      }

      case Ili1Parser::FIX: {
        setState(382);
        match(Ili1Parser::FIX);
        setState(383);
        match(Ili1Parser::WITH);
        setState(384);
        match(Ili1Parser::LINESIZE);
        setState(385);
        match(Ili1Parser::EQUAL);
        setState(386);
        dynamic_cast<FormatEncodingContext *>(_localctx)->lineSize = match(Ili1Parser::POSNUMBER);
        setState(387);
        match(Ili1Parser::COMMA);
        setState(388);
        match(Ili1Parser::TIDSIZE);
        setState(389);
        match(Ili1Parser::EQUAL);
        setState(390);
        dynamic_cast<FormatEncodingContext *>(_localctx)->tidSize = match(Ili1Parser::POSNUMBER);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(393);
    match(Ili1Parser::SEMI);
    setState(394);
    match(Ili1Parser::CODE);
    setState(396);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == Ili1Parser::FONT) {
      setState(395);
      font();
    }
    setState(398);
    match(Ili1Parser::BLANK);
    setState(399);
    match(Ili1Parser::EQUAL);
    setState(402);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::DEFAULT: {
        setState(400);
        match(Ili1Parser::DEFAULT);
        break;
      }

      case Ili1Parser::POSNUMBER:
      case Ili1Parser::HEXNUMBER: {
        setState(401);
        dynamic_cast<FormatEncodingContext *>(_localctx)->blankcode = code();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(404);
    match(Ili1Parser::COMMA);
    setState(405);
    match(Ili1Parser::UNDEFINED);
    setState(406);
    match(Ili1Parser::EQUAL);
    setState(409);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::DEFAULT: {
        setState(407);
        match(Ili1Parser::DEFAULT);
        break;
      }

      case Ili1Parser::POSNUMBER:
      case Ili1Parser::HEXNUMBER: {
        setState(408);
        dynamic_cast<FormatEncodingContext *>(_localctx)->undefinedcode = code();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(411);
    match(Ili1Parser::COMMA);
    setState(412);
    match(Ili1Parser::CONTINUE);
    setState(413);
    match(Ili1Parser::EQUAL);
    setState(416);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case Ili1Parser::DEFAULT: {
        setState(414);
        match(Ili1Parser::DEFAULT);
        break;
      }

      case Ili1Parser::POSNUMBER:
      case Ili1Parser::HEXNUMBER: {
        setState(415);
        dynamic_cast<FormatEncodingContext *>(_localctx)->continuecode = code();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(418);
    match(Ili1Parser::SEMI);
    setState(419);
    match(Ili1Parser::TID);
    setState(420);
    match(Ili1Parser::EQUAL);
    setState(421);
    _la = _input->LA(1);
    if (!(((((_la - 33) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 33)) & ((1ULL << (Ili1Parser::ANY - 33))
      | (1ULL << (Ili1Parser::I16 - 33))
      | (1ULL << (Ili1Parser::I32 - 33))
      | (1ULL << (Ili1Parser::EXPLANATION - 33)))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(422);
    match(Ili1Parser::SEMI);
    setState(423);
    match(Ili1Parser::END);
    setState(424);
    match(Ili1Parser::DOT);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FontContext ------------------------------------------------------------------

Ili1Parser::FontContext::FontContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::FontContext::FONT() {
  return getToken(Ili1Parser::FONT, 0);
}

tree::TerminalNode* Ili1Parser::FontContext::EQUAL() {
  return getToken(Ili1Parser::EQUAL, 0);
}

tree::TerminalNode* Ili1Parser::FontContext::SEMI() {
  return getToken(Ili1Parser::SEMI, 0);
}

tree::TerminalNode* Ili1Parser::FontContext::EXPLANATION() {
  return getToken(Ili1Parser::EXPLANATION, 0);
}


size_t Ili1Parser::FontContext::getRuleIndex() const {
  return Ili1Parser::RuleFont;
}

antlrcpp::Any Ili1Parser::FontContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitFont(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::FontContext* Ili1Parser::font() {
  FontContext *_localctx = _tracker.createInstance<FontContext>(_ctx, getState());
  enterRule(_localctx, 68, Ili1Parser::RuleFont);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(426);
    match(Ili1Parser::FONT);
    setState(427);
    match(Ili1Parser::EQUAL);
    setState(428);
    dynamic_cast<FontContext *>(_localctx)->expl = match(Ili1Parser::EXPLANATION);
    setState(429);
    match(Ili1Parser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CodeContext ------------------------------------------------------------------

Ili1Parser::CodeContext::CodeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::CodeContext::POSNUMBER() {
  return getToken(Ili1Parser::POSNUMBER, 0);
}

tree::TerminalNode* Ili1Parser::CodeContext::HEXNUMBER() {
  return getToken(Ili1Parser::HEXNUMBER, 0);
}


size_t Ili1Parser::CodeContext::getRuleIndex() const {
  return Ili1Parser::RuleCode;
}

antlrcpp::Any Ili1Parser::CodeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitCode(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::CodeContext* Ili1Parser::code() {
  CodeContext *_localctx = _tracker.createInstance<CodeContext>(_ctx, getState());
  enterRule(_localctx, 70, Ili1Parser::RuleCode);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(431);
    _la = _input->LA(1);
    if (!(_la == Ili1Parser::POSNUMBER

    || _la == Ili1Parser::HEXNUMBER)) {
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

//----------------- DecimalContext ------------------------------------------------------------------

Ili1Parser::DecimalContext::DecimalContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* Ili1Parser::DecimalContext::DEC() {
  return getToken(Ili1Parser::DEC, 0);
}

tree::TerminalNode* Ili1Parser::DecimalContext::POSNUMBER() {
  return getToken(Ili1Parser::POSNUMBER, 0);
}

tree::TerminalNode* Ili1Parser::DecimalContext::NUMBER() {
  return getToken(Ili1Parser::NUMBER, 0);
}


size_t Ili1Parser::DecimalContext::getRuleIndex() const {
  return Ili1Parser::RuleDecimal;
}

antlrcpp::Any Ili1Parser::DecimalContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<Ili1ParserVisitor*>(visitor))
    return parserVisitor->visitDecimal(this);
  else
    return visitor->visitChildren(this);
}

Ili1Parser::DecimalContext* Ili1Parser::decimal() {
  DecimalContext *_localctx = _tracker.createInstance<DecimalContext>(_ctx, getState());
  enterRule(_localctx, 72, Ili1Parser::RuleDecimal);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(433);
    _la = _input->LA(1);
    if (!(((((_la - 76) & ~ 0x3fULL) == 0) &&
      ((1ULL << (_la - 76)) & ((1ULL << (Ili1Parser::POSNUMBER - 76))
      | (1ULL << (Ili1Parser::NUMBER - 76))
      | (1ULL << (Ili1Parser::DEC - 76)))) != 0))) {
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

// Static vars and initialization.
std::vector<dfa::DFA> Ili1Parser::_decisionToDFA;
atn::PredictionContextCache Ili1Parser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN Ili1Parser::_atn;
std::vector<uint16_t> Ili1Parser::_serializedATN;

std::vector<std::string> Ili1Parser::_ruleNames = {
  "interlis1Def", "modelDef", "domainDefs", "domainDef", "topicDef", "tableDef", 
  "attribute", "identifications", "identification", "type", "baseType", 
  "coord2", "coord3", "numericRange", "dim1Type", "dim2Type", "angleType", 
  "textType", "dateType", "enumerationType", "enumElement", "horizAlignment", 
  "vertAlignment", "lineType", "areaType", "form", "lineForm", "intersectionDef", 
  "controlPoints", "lineAttributes", "derivatives", "view", "viewDef", "formatEncoding", 
  "font", "code", "decimal"
};

std::vector<std::string> Ili1Parser::_literalNames = {
  "", "", "", "", "", "", "'MODEL'", "'END'", "'TOPIC'", "'VIEW'", "'DATE'", 
  "'NO'", "'TEXT'", "'('", "')'", "'['", "']'", "'*'", "'>'", "';'", "'='", 
  "'.'", "'..'", "':'", "','", "'<-'", "'->'", "'HALIGNMENT'", "'VALIGNMENT'", 
  "'UNDEFINED'", "'DOMAIN'", "'FORMAT'", "'COORD'", "'ANY'", "'POLYLINE'", 
  "'SURFACE'", "'AREA'", "'WITH'", "'STRAIGHTS'", "'ARCS'", "'VERTEX'", 
  "'WITHOUT'", "'OVERLAPS'", "'FORM'", "'BASE'", "'TRANSFER'", "'OPTIONAL'", 
  "'TABLE'", "'IDENT'", "'COORD2'", "'COORD3'", "'DIM1'", "'DIM2'", "'RADIANS'", 
  "'DEGREES'", "'GRADS'", "'LINEATTR'", "'DERIVATIVES'", "'VERTEXINFO'", 
  "'PERIPHERY'", "'CONTOUR'", "'FREE'", "'FIX'", "'LINESIZE'", "'TIDSIZE'", 
  "'CODE'", "'FONT'", "'BLANK'", "'DEFAULT'", "'CONTINUE'", "'TID'", "'I16'", 
  "'I32'"
};

std::vector<std::string> Ili1Parser::_symbolicNames = {
  "", "WS", "SL_COMMENT", "ILI_DOC", "ILI_OPTION", "ILI_BLOCKCOMMENT", "MODEL", 
  "END", "TOPIC", "VIEW", "DATE", "NO", "TEXT", "LPAREN", "RPAREN", "LBRACE", 
  "RBRACE", "STAR", "GREATER", "SEMI", "EQUAL", "DOT", "DOTDOT", "COLON", 
  "COMMA", "LARROW", "RARROW", "HALIGNMENT", "VALIGNMENT", "UNDEFINED", 
  "ILIDOMAIN", "FORMAT", "COORD", "ANY", "POLYLINE", "SURFACE", "AREA", 
  "WITH", "STRAIGHTS", "ARCS", "VERTEX", "WITHOUT", "OVERLAPS", "FORM", 
  "BASE", "TRANSFER", "OPTIONAL", "TABLE", "IDENT", "COORD2", "COORD3", 
  "DIM1", "DIM2", "RADIANS", "DEGREES", "GRADS", "LINEATTR", "DERIVATIVES", 
  "VERTEXINFO", "PERIPHERY", "CONTOUR", "FREE", "FIX", "LINESIZE", "TIDSIZE", 
  "CODE", "FONT", "BLANK", "DEFAULT", "CONTINUE", "TID", "I16", "I32", "EXPLANATION", 
  "ESC", "STRING", "POSNUMBER", "NUMBER", "DEC", "SCALING", "DECIMAL", "HEXNUMBER", 
  "NAME", "ERRORCHAR"
};

dfa::Vocabulary Ili1Parser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> Ili1Parser::_tokenNames;

Ili1Parser::Initializer::Initializer() {
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
    0x3, 0x55, 0x1b6, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
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
    0x25, 0x4, 0x26, 0x9, 0x26, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 
    0x5, 0x2, 0x51, 0xa, 0x2, 0x3, 0x2, 0x5, 0x2, 0x54, 0xa, 0x2, 0x3, 0x2, 
    0x3, 0x2, 0x7, 0x2, 0x58, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x5b, 0xb, 0x2, 
    0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 
    0x63, 0xa, 0x3, 0x3, 0x3, 0x6, 0x3, 0x66, 0xa, 0x3, 0xd, 0x3, 0xe, 0x3, 
    0x67, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x4, 0x6, 
    0x4, 0x70, 0xa, 0x4, 0xd, 0x4, 0xe, 0x4, 0x71, 0x3, 0x5, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 
    0x6, 0x6, 0x6, 0x7e, 0xa, 0x6, 0xd, 0x6, 0xe, 0x6, 0x7f, 0x3, 0x6, 0x3, 
    0x6, 0x3, 0x6, 0x3, 0x6, 0x3, 0x7, 0x5, 0x7, 0x87, 0xa, 0x7, 0x3, 0x7, 
    0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x6, 0x7, 0x8d, 0xa, 0x7, 0xd, 0x7, 0xe, 
    0x7, 0x8e, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x8, 
    0x3, 0x8, 0x3, 0x8, 0x5, 0x8, 0x99, 0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 
    0x8, 0x5, 0x8, 0x9e, 0xa, 0x8, 0x3, 0x8, 0x5, 0x8, 0xa1, 0xa, 0x8, 0x3, 
    0x8, 0x3, 0x8, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x6, 0x9, 0xa9, 
    0xa, 0x9, 0xd, 0x9, 0xe, 0x9, 0xaa, 0x5, 0x9, 0xad, 0xa, 0x9, 0x3, 0xa, 
    0x3, 0xa, 0x3, 0xa, 0x7, 0xa, 0xb2, 0xa, 0xa, 0xc, 0xa, 0xe, 0xa, 0xb5, 
    0xb, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x5, 0xb, 0xbd, 0xa, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 
    0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x5, 
    0xc, 0xca, 0xa, 0xc, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 0x3, 0xd, 
    0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 0x3, 0xe, 
    0x3, 0xe, 0x3, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 
    0x3, 0xf, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x11, 0x3, 
    0x11, 0x3, 0x11, 0x3, 0x11, 0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 0x3, 0x12, 
    0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x14, 0x3, 0x14, 0x3, 
    0x15, 0x3, 0x15, 0x3, 0x15, 0x3, 0x15, 0x7, 0x15, 0xf6, 0xa, 0x15, 0xc, 
    0x15, 0xe, 0x15, 0xf9, 0xb, 0x15, 0x3, 0x15, 0x3, 0x15, 0x3, 0x16, 0x3, 
    0x16, 0x5, 0x16, 0xff, 0xa, 0x16, 0x3, 0x17, 0x3, 0x17, 0x3, 0x18, 0x3, 
    0x18, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x3, 0x19, 0x5, 0x19, 0x109, 
    0xa, 0x19, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x5, 0x1a, 0x10f, 
    0xa, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x5, 
    0x1a, 0x116, 0xa, 0x1a, 0x3, 0x1a, 0x5, 0x1a, 0x119, 0xa, 0x1a, 0x3, 
    0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 0x7, 0x1b, 0x120, 
    0xa, 0x1b, 0xc, 0x1b, 0xe, 0x1b, 0x123, 0xb, 0x1b, 0x3, 0x1b, 0x3, 0x1b, 
    0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x5, 0x1c, 0x12a, 0xa, 0x1c, 0x3, 0x1d, 
    0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1e, 0x3, 0x1e, 0x3, 
    0x1e, 0x3, 0x1e, 0x5, 0x1e, 0x135, 0xa, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 
    0x5, 0x1e, 0x139, 0xa, 0x1e, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x6, 0x1f, 
    0x13e, 0xa, 0x1f, 0xd, 0x1f, 0xe, 0x1f, 0x13f, 0x3, 0x1f, 0x5, 0x1f, 
    0x143, 0xa, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 
    0x5, 0x20, 0x14a, 0xa, 0x20, 0x3, 0x20, 0x6, 0x20, 0x14d, 0xa, 0x20, 
    0xd, 0x20, 0xe, 0x20, 0x14e, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x21, 
    0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 
    0x21, 0x3, 0x21, 0x7, 0x21, 0x15d, 0xa, 0x21, 0xc, 0x21, 0xe, 0x21, 
    0x160, 0xb, 0x21, 0x3, 0x21, 0x3, 0x21, 0x7, 0x21, 0x164, 0xa, 0x21, 
    0xc, 0x21, 0xe, 0x21, 0x167, 0xb, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 
    0x3, 0x21, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x3, 
    0x22, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x5, 0x22, 0x177, 
    0xa, 0x22, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x5, 0x22, 0x17d, 
    0xa, 0x22, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 
    0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x5, 0x23, 
    0x18a, 0xa, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x5, 0x23, 0x18f, 
    0xa, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x5, 0x23, 0x195, 
    0xa, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x5, 
    0x23, 0x19c, 0xa, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 
    0x3, 0x23, 0x5, 0x23, 0x1a3, 0xa, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 
    0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x23, 0x3, 0x24, 0x3, 
    0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x24, 0x3, 0x25, 0x3, 0x25, 0x3, 0x26, 
    0x3, 0x26, 0x3, 0x26, 0x2, 0x2, 0x27, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 
    0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 
    0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 
    0x3e, 0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x2, 0x6, 0x3, 0x2, 0x37, 
    0x39, 0x4, 0x2, 0x23, 0x23, 0x49, 0x4b, 0x4, 0x2, 0x4e, 0x4e, 0x53, 
    0x53, 0x3, 0x2, 0x4e, 0x50, 0x2, 0x1c8, 0x2, 0x4c, 0x3, 0x2, 0x2, 0x2, 
    0x4, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x6, 0x6d, 0x3, 0x2, 0x2, 0x2, 0x8, 0x73, 
    0x3, 0x2, 0x2, 0x2, 0xa, 0x78, 0x3, 0x2, 0x2, 0x2, 0xc, 0x86, 0x3, 0x2, 
    0x2, 0x2, 0xe, 0x95, 0x3, 0x2, 0x2, 0x2, 0x10, 0xac, 0x3, 0x2, 0x2, 
    0x2, 0x12, 0xae, 0x3, 0x2, 0x2, 0x2, 0x14, 0xbc, 0x3, 0x2, 0x2, 0x2, 
    0x16, 0xc9, 0x3, 0x2, 0x2, 0x2, 0x18, 0xcb, 0x3, 0x2, 0x2, 0x2, 0x1a, 
    0xd1, 0x3, 0x2, 0x2, 0x2, 0x1c, 0xd9, 0x3, 0x2, 0x2, 0x2, 0x1e, 0xdf, 
    0x3, 0x2, 0x2, 0x2, 0x20, 0xe3, 0x3, 0x2, 0x2, 0x2, 0x22, 0xe7, 0x3, 
    0x2, 0x2, 0x2, 0x24, 0xeb, 0x3, 0x2, 0x2, 0x2, 0x26, 0xef, 0x3, 0x2, 
    0x2, 0x2, 0x28, 0xf1, 0x3, 0x2, 0x2, 0x2, 0x2a, 0xfc, 0x3, 0x2, 0x2, 
    0x2, 0x2c, 0x100, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x102, 0x3, 0x2, 0x2, 0x2, 
    0x30, 0x104, 0x3, 0x2, 0x2, 0x2, 0x32, 0x115, 0x3, 0x2, 0x2, 0x2, 0x34, 
    0x11a, 0x3, 0x2, 0x2, 0x2, 0x36, 0x129, 0x3, 0x2, 0x2, 0x2, 0x38, 0x12b, 
    0x3, 0x2, 0x2, 0x2, 0x3a, 0x130, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x13a, 0x3, 
    0x2, 0x2, 0x2, 0x3e, 0x146, 0x3, 0x2, 0x2, 0x2, 0x40, 0x153, 0x3, 0x2, 
    0x2, 0x2, 0x42, 0x17c, 0x3, 0x2, 0x2, 0x2, 0x44, 0x17e, 0x3, 0x2, 0x2, 
    0x2, 0x46, 0x1ac, 0x3, 0x2, 0x2, 0x2, 0x48, 0x1b1, 0x3, 0x2, 0x2, 0x2, 
    0x4a, 0x1b3, 0x3, 0x2, 0x2, 0x2, 0x4c, 0x4d, 0x7, 0x2f, 0x2, 0x2, 0x4d, 
    0x4e, 0x7, 0x54, 0x2, 0x2, 0x4e, 0x50, 0x7, 0x15, 0x2, 0x2, 0x4f, 0x51, 
    0x5, 0x6, 0x4, 0x2, 0x50, 0x4f, 0x3, 0x2, 0x2, 0x2, 0x50, 0x51, 0x3, 
    0x2, 0x2, 0x2, 0x51, 0x53, 0x3, 0x2, 0x2, 0x2, 0x52, 0x54, 0x5, 0x3e, 
    0x20, 0x2, 0x53, 0x52, 0x3, 0x2, 0x2, 0x2, 0x53, 0x54, 0x3, 0x2, 0x2, 
    0x2, 0x54, 0x55, 0x3, 0x2, 0x2, 0x2, 0x55, 0x59, 0x5, 0x4, 0x3, 0x2, 
    0x56, 0x58, 0x5, 0x40, 0x21, 0x2, 0x57, 0x56, 0x3, 0x2, 0x2, 0x2, 0x58, 
    0x5b, 0x3, 0x2, 0x2, 0x2, 0x59, 0x57, 0x3, 0x2, 0x2, 0x2, 0x59, 0x5a, 
    0x3, 0x2, 0x2, 0x2, 0x5a, 0x5c, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x59, 0x3, 
    0x2, 0x2, 0x2, 0x5c, 0x5d, 0x5, 0x44, 0x23, 0x2, 0x5d, 0x5e, 0x7, 0x2, 
    0x2, 0x3, 0x5e, 0x3, 0x3, 0x2, 0x2, 0x2, 0x5f, 0x60, 0x7, 0x8, 0x2, 
    0x2, 0x60, 0x62, 0x7, 0x54, 0x2, 0x2, 0x61, 0x63, 0x5, 0x6, 0x4, 0x2, 
    0x62, 0x61, 0x3, 0x2, 0x2, 0x2, 0x62, 0x63, 0x3, 0x2, 0x2, 0x2, 0x63, 
    0x65, 0x3, 0x2, 0x2, 0x2, 0x64, 0x66, 0x5, 0xa, 0x6, 0x2, 0x65, 0x64, 
    0x3, 0x2, 0x2, 0x2, 0x66, 0x67, 0x3, 0x2, 0x2, 0x2, 0x67, 0x65, 0x3, 
    0x2, 0x2, 0x2, 0x67, 0x68, 0x3, 0x2, 0x2, 0x2, 0x68, 0x69, 0x3, 0x2, 
    0x2, 0x2, 0x69, 0x6a, 0x7, 0x9, 0x2, 0x2, 0x6a, 0x6b, 0x7, 0x54, 0x2, 
    0x2, 0x6b, 0x6c, 0x7, 0x17, 0x2, 0x2, 0x6c, 0x5, 0x3, 0x2, 0x2, 0x2, 
    0x6d, 0x6f, 0x7, 0x20, 0x2, 0x2, 0x6e, 0x70, 0x5, 0x8, 0x5, 0x2, 0x6f, 
    0x6e, 0x3, 0x2, 0x2, 0x2, 0x70, 0x71, 0x3, 0x2, 0x2, 0x2, 0x71, 0x6f, 
    0x3, 0x2, 0x2, 0x2, 0x71, 0x72, 0x3, 0x2, 0x2, 0x2, 0x72, 0x7, 0x3, 
    0x2, 0x2, 0x2, 0x73, 0x74, 0x7, 0x54, 0x2, 0x2, 0x74, 0x75, 0x7, 0x16, 
    0x2, 0x2, 0x75, 0x76, 0x5, 0x14, 0xb, 0x2, 0x76, 0x77, 0x7, 0x15, 0x2, 
    0x2, 0x77, 0x9, 0x3, 0x2, 0x2, 0x2, 0x78, 0x79, 0x7, 0xa, 0x2, 0x2, 
    0x79, 0x7a, 0x7, 0x54, 0x2, 0x2, 0x7a, 0x7d, 0x7, 0x16, 0x2, 0x2, 0x7b, 
    0x7e, 0x5, 0xc, 0x7, 0x2, 0x7c, 0x7e, 0x5, 0x6, 0x4, 0x2, 0x7d, 0x7b, 
    0x3, 0x2, 0x2, 0x2, 0x7d, 0x7c, 0x3, 0x2, 0x2, 0x2, 0x7e, 0x7f, 0x3, 
    0x2, 0x2, 0x2, 0x7f, 0x7d, 0x3, 0x2, 0x2, 0x2, 0x7f, 0x80, 0x3, 0x2, 
    0x2, 0x2, 0x80, 0x81, 0x3, 0x2, 0x2, 0x2, 0x81, 0x82, 0x7, 0x9, 0x2, 
    0x2, 0x82, 0x83, 0x7, 0x54, 0x2, 0x2, 0x83, 0x84, 0x7, 0x17, 0x2, 0x2, 
    0x84, 0xb, 0x3, 0x2, 0x2, 0x2, 0x85, 0x87, 0x7, 0x30, 0x2, 0x2, 0x86, 
    0x85, 0x3, 0x2, 0x2, 0x2, 0x86, 0x87, 0x3, 0x2, 0x2, 0x2, 0x87, 0x88, 
    0x3, 0x2, 0x2, 0x2, 0x88, 0x89, 0x7, 0x31, 0x2, 0x2, 0x89, 0x8a, 0x7, 
    0x54, 0x2, 0x2, 0x8a, 0x8c, 0x7, 0x16, 0x2, 0x2, 0x8b, 0x8d, 0x5, 0xe, 
    0x8, 0x2, 0x8c, 0x8b, 0x3, 0x2, 0x2, 0x2, 0x8d, 0x8e, 0x3, 0x2, 0x2, 
    0x2, 0x8e, 0x8c, 0x3, 0x2, 0x2, 0x2, 0x8e, 0x8f, 0x3, 0x2, 0x2, 0x2, 
    0x8f, 0x90, 0x3, 0x2, 0x2, 0x2, 0x90, 0x91, 0x5, 0x10, 0x9, 0x2, 0x91, 
    0x92, 0x7, 0x9, 0x2, 0x2, 0x92, 0x93, 0x7, 0x54, 0x2, 0x2, 0x93, 0x94, 
    0x7, 0x15, 0x2, 0x2, 0x94, 0xd, 0x3, 0x2, 0x2, 0x2, 0x95, 0x96, 0x7, 
    0x54, 0x2, 0x2, 0x96, 0x98, 0x7, 0x19, 0x2, 0x2, 0x97, 0x99, 0x7, 0x30, 
    0x2, 0x2, 0x98, 0x97, 0x3, 0x2, 0x2, 0x2, 0x98, 0x99, 0x3, 0x2, 0x2, 
    0x2, 0x99, 0x9d, 0x3, 0x2, 0x2, 0x2, 0x9a, 0x9e, 0x5, 0x14, 0xb, 0x2, 
    0x9b, 0x9c, 0x7, 0x1c, 0x2, 0x2, 0x9c, 0x9e, 0x7, 0x54, 0x2, 0x2, 0x9d, 
    0x9a, 0x3, 0x2, 0x2, 0x2, 0x9d, 0x9b, 0x3, 0x2, 0x2, 0x2, 0x9e, 0xa0, 
    0x3, 0x2, 0x2, 0x2, 0x9f, 0xa1, 0x7, 0x4b, 0x2, 0x2, 0xa0, 0x9f, 0x3, 
    0x2, 0x2, 0x2, 0xa0, 0xa1, 0x3, 0x2, 0x2, 0x2, 0xa1, 0xa2, 0x3, 0x2, 
    0x2, 0x2, 0xa2, 0xa3, 0x7, 0x15, 0x2, 0x2, 0xa3, 0xf, 0x3, 0x2, 0x2, 
    0x2, 0xa4, 0xa5, 0x7, 0xd, 0x2, 0x2, 0xa5, 0xad, 0x7, 0x32, 0x2, 0x2, 
    0xa6, 0xa8, 0x7, 0x32, 0x2, 0x2, 0xa7, 0xa9, 0x5, 0x12, 0xa, 0x2, 0xa8, 
    0xa7, 0x3, 0x2, 0x2, 0x2, 0xa9, 0xaa, 0x3, 0x2, 0x2, 0x2, 0xaa, 0xa8, 
    0x3, 0x2, 0x2, 0x2, 0xaa, 0xab, 0x3, 0x2, 0x2, 0x2, 0xab, 0xad, 0x3, 
    0x2, 0x2, 0x2, 0xac, 0xa4, 0x3, 0x2, 0x2, 0x2, 0xac, 0xa6, 0x3, 0x2, 
    0x2, 0x2, 0xad, 0x11, 0x3, 0x2, 0x2, 0x2, 0xae, 0xb3, 0x7, 0x54, 0x2, 
    0x2, 0xaf, 0xb0, 0x7, 0x1a, 0x2, 0x2, 0xb0, 0xb2, 0x7, 0x54, 0x2, 0x2, 
    0xb1, 0xaf, 0x3, 0x2, 0x2, 0x2, 0xb2, 0xb5, 0x3, 0x2, 0x2, 0x2, 0xb3, 
    0xb1, 0x3, 0x2, 0x2, 0x2, 0xb3, 0xb4, 0x3, 0x2, 0x2, 0x2, 0xb4, 0xb6, 
    0x3, 0x2, 0x2, 0x2, 0xb5, 0xb3, 0x3, 0x2, 0x2, 0x2, 0xb6, 0xb7, 0x7, 
    0x15, 0x2, 0x2, 0xb7, 0x13, 0x3, 0x2, 0x2, 0x2, 0xb8, 0xbd, 0x5, 0x16, 
    0xc, 0x2, 0xb9, 0xbd, 0x5, 0x30, 0x19, 0x2, 0xba, 0xbd, 0x5, 0x32, 0x1a, 
    0x2, 0xbb, 0xbd, 0x7, 0x54, 0x2, 0x2, 0xbc, 0xb8, 0x3, 0x2, 0x2, 0x2, 
    0xbc, 0xb9, 0x3, 0x2, 0x2, 0x2, 0xbc, 0xba, 0x3, 0x2, 0x2, 0x2, 0xbc, 
    0xbb, 0x3, 0x2, 0x2, 0x2, 0xbd, 0x15, 0x3, 0x2, 0x2, 0x2, 0xbe, 0xca, 
    0x5, 0x18, 0xd, 0x2, 0xbf, 0xca, 0x5, 0x1a, 0xe, 0x2, 0xc0, 0xca, 0x5, 
    0x1e, 0x10, 0x2, 0xc1, 0xca, 0x5, 0x20, 0x11, 0x2, 0xc2, 0xca, 0x5, 
    0x22, 0x12, 0x2, 0xc3, 0xca, 0x5, 0x1c, 0xf, 0x2, 0xc4, 0xca, 0x5, 0x24, 
    0x13, 0x2, 0xc5, 0xca, 0x5, 0x26, 0x14, 0x2, 0xc6, 0xca, 0x5, 0x28, 
    0x15, 0x2, 0xc7, 0xca, 0x5, 0x2c, 0x17, 0x2, 0xc8, 0xca, 0x5, 0x2e, 
    0x18, 0x2, 0xc9, 0xbe, 0x3, 0x2, 0x2, 0x2, 0xc9, 0xbf, 0x3, 0x2, 0x2, 
    0x2, 0xc9, 0xc0, 0x3, 0x2, 0x2, 0x2, 0xc9, 0xc1, 0x3, 0x2, 0x2, 0x2, 
    0xc9, 0xc2, 0x3, 0x2, 0x2, 0x2, 0xc9, 0xc3, 0x3, 0x2, 0x2, 0x2, 0xc9, 
    0xc4, 0x3, 0x2, 0x2, 0x2, 0xc9, 0xc5, 0x3, 0x2, 0x2, 0x2, 0xc9, 0xc6, 
    0x3, 0x2, 0x2, 0x2, 0xc9, 0xc7, 0x3, 0x2, 0x2, 0x2, 0xc9, 0xc8, 0x3, 
    0x2, 0x2, 0x2, 0xca, 0x17, 0x3, 0x2, 0x2, 0x2, 0xcb, 0xcc, 0x7, 0x33, 
    0x2, 0x2, 0xcc, 0xcd, 0x5, 0x4a, 0x26, 0x2, 0xcd, 0xce, 0x5, 0x4a, 0x26, 
    0x2, 0xce, 0xcf, 0x5, 0x4a, 0x26, 0x2, 0xcf, 0xd0, 0x5, 0x4a, 0x26, 
    0x2, 0xd0, 0x19, 0x3, 0x2, 0x2, 0x2, 0xd1, 0xd2, 0x7, 0x34, 0x2, 0x2, 
    0xd2, 0xd3, 0x5, 0x4a, 0x26, 0x2, 0xd3, 0xd4, 0x5, 0x4a, 0x26, 0x2, 
    0xd4, 0xd5, 0x5, 0x4a, 0x26, 0x2, 0xd5, 0xd6, 0x5, 0x4a, 0x26, 0x2, 
    0xd6, 0xd7, 0x5, 0x4a, 0x26, 0x2, 0xd7, 0xd8, 0x5, 0x4a, 0x26, 0x2, 
    0xd8, 0x1b, 0x3, 0x2, 0x2, 0x2, 0xd9, 0xda, 0x7, 0x11, 0x2, 0x2, 0xda, 
    0xdb, 0x5, 0x4a, 0x26, 0x2, 0xdb, 0xdc, 0x7, 0x18, 0x2, 0x2, 0xdc, 0xdd, 
    0x5, 0x4a, 0x26, 0x2, 0xdd, 0xde, 0x7, 0x12, 0x2, 0x2, 0xde, 0x1d, 0x3, 
    0x2, 0x2, 0x2, 0xdf, 0xe0, 0x7, 0x35, 0x2, 0x2, 0xe0, 0xe1, 0x5, 0x4a, 
    0x26, 0x2, 0xe1, 0xe2, 0x5, 0x4a, 0x26, 0x2, 0xe2, 0x1f, 0x3, 0x2, 0x2, 
    0x2, 0xe3, 0xe4, 0x7, 0x36, 0x2, 0x2, 0xe4, 0xe5, 0x5, 0x4a, 0x26, 0x2, 
    0xe5, 0xe6, 0x5, 0x4a, 0x26, 0x2, 0xe6, 0x21, 0x3, 0x2, 0x2, 0x2, 0xe7, 
    0xe8, 0x9, 0x2, 0x2, 0x2, 0xe8, 0xe9, 0x5, 0x4a, 0x26, 0x2, 0xe9, 0xea, 
    0x5, 0x4a, 0x26, 0x2, 0xea, 0x23, 0x3, 0x2, 0x2, 0x2, 0xeb, 0xec, 0x7, 
    0xe, 0x2, 0x2, 0xec, 0xed, 0x7, 0x13, 0x2, 0x2, 0xed, 0xee, 0x7, 0x4e, 
    0x2, 0x2, 0xee, 0x25, 0x3, 0x2, 0x2, 0x2, 0xef, 0xf0, 0x7, 0xc, 0x2, 
    0x2, 0xf0, 0x27, 0x3, 0x2, 0x2, 0x2, 0xf1, 0xf2, 0x7, 0xf, 0x2, 0x2, 
    0xf2, 0xf7, 0x5, 0x2a, 0x16, 0x2, 0xf3, 0xf4, 0x7, 0x1a, 0x2, 0x2, 0xf4, 
    0xf6, 0x5, 0x2a, 0x16, 0x2, 0xf5, 0xf3, 0x3, 0x2, 0x2, 0x2, 0xf6, 0xf9, 
    0x3, 0x2, 0x2, 0x2, 0xf7, 0xf5, 0x3, 0x2, 0x2, 0x2, 0xf7, 0xf8, 0x3, 
    0x2, 0x2, 0x2, 0xf8, 0xfa, 0x3, 0x2, 0x2, 0x2, 0xf9, 0xf7, 0x3, 0x2, 
    0x2, 0x2, 0xfa, 0xfb, 0x7, 0x10, 0x2, 0x2, 0xfb, 0x29, 0x3, 0x2, 0x2, 
    0x2, 0xfc, 0xfe, 0x7, 0x54, 0x2, 0x2, 0xfd, 0xff, 0x5, 0x28, 0x15, 0x2, 
    0xfe, 0xfd, 0x3, 0x2, 0x2, 0x2, 0xfe, 0xff, 0x3, 0x2, 0x2, 0x2, 0xff, 
    0x2b, 0x3, 0x2, 0x2, 0x2, 0x100, 0x101, 0x7, 0x1d, 0x2, 0x2, 0x101, 
    0x2d, 0x3, 0x2, 0x2, 0x2, 0x102, 0x103, 0x7, 0x1e, 0x2, 0x2, 0x103, 
    0x2f, 0x3, 0x2, 0x2, 0x2, 0x104, 0x105, 0x7, 0x24, 0x2, 0x2, 0x105, 
    0x106, 0x5, 0x34, 0x1b, 0x2, 0x106, 0x108, 0x5, 0x3a, 0x1e, 0x2, 0x107, 
    0x109, 0x5, 0x38, 0x1d, 0x2, 0x108, 0x107, 0x3, 0x2, 0x2, 0x2, 0x108, 
    0x109, 0x3, 0x2, 0x2, 0x2, 0x109, 0x31, 0x3, 0x2, 0x2, 0x2, 0x10a, 0x10b, 
    0x7, 0x25, 0x2, 0x2, 0x10b, 0x10c, 0x5, 0x34, 0x1b, 0x2, 0x10c, 0x10e, 
    0x5, 0x3a, 0x1e, 0x2, 0x10d, 0x10f, 0x5, 0x38, 0x1d, 0x2, 0x10e, 0x10d, 
    0x3, 0x2, 0x2, 0x2, 0x10e, 0x10f, 0x3, 0x2, 0x2, 0x2, 0x10f, 0x116, 
    0x3, 0x2, 0x2, 0x2, 0x110, 0x111, 0x7, 0x26, 0x2, 0x2, 0x111, 0x112, 
    0x5, 0x34, 0x1b, 0x2, 0x112, 0x113, 0x5, 0x3a, 0x1e, 0x2, 0x113, 0x114, 
    0x5, 0x38, 0x1d, 0x2, 0x114, 0x116, 0x3, 0x2, 0x2, 0x2, 0x115, 0x10a, 
    0x3, 0x2, 0x2, 0x2, 0x115, 0x110, 0x3, 0x2, 0x2, 0x2, 0x116, 0x118, 
    0x3, 0x2, 0x2, 0x2, 0x117, 0x119, 0x5, 0x3c, 0x1f, 0x2, 0x118, 0x117, 
    0x3, 0x2, 0x2, 0x2, 0x118, 0x119, 0x3, 0x2, 0x2, 0x2, 0x119, 0x33, 0x3, 
    0x2, 0x2, 0x2, 0x11a, 0x11b, 0x7, 0x27, 0x2, 0x2, 0x11b, 0x11c, 0x7, 
    0xf, 0x2, 0x2, 0x11c, 0x121, 0x5, 0x36, 0x1c, 0x2, 0x11d, 0x11e, 0x7, 
    0x1a, 0x2, 0x2, 0x11e, 0x120, 0x5, 0x36, 0x1c, 0x2, 0x11f, 0x11d, 0x3, 
    0x2, 0x2, 0x2, 0x120, 0x123, 0x3, 0x2, 0x2, 0x2, 0x121, 0x11f, 0x3, 
    0x2, 0x2, 0x2, 0x121, 0x122, 0x3, 0x2, 0x2, 0x2, 0x122, 0x124, 0x3, 
    0x2, 0x2, 0x2, 0x123, 0x121, 0x3, 0x2, 0x2, 0x2, 0x124, 0x125, 0x7, 
    0x10, 0x2, 0x2, 0x125, 0x35, 0x3, 0x2, 0x2, 0x2, 0x126, 0x12a, 0x7, 
    0x28, 0x2, 0x2, 0x127, 0x12a, 0x7, 0x29, 0x2, 0x2, 0x128, 0x12a, 0x7, 
    0x4b, 0x2, 0x2, 0x129, 0x126, 0x3, 0x2, 0x2, 0x2, 0x129, 0x127, 0x3, 
    0x2, 0x2, 0x2, 0x129, 0x128, 0x3, 0x2, 0x2, 0x2, 0x12a, 0x37, 0x3, 0x2, 
    0x2, 0x2, 0x12b, 0x12c, 0x7, 0x2b, 0x2, 0x2, 0x12c, 0x12d, 0x7, 0x2c, 
    0x2, 0x2, 0x12d, 0x12e, 0x7, 0x14, 0x2, 0x2, 0x12e, 0x12f, 0x5, 0x4a, 
    0x26, 0x2, 0x12f, 0x39, 0x3, 0x2, 0x2, 0x2, 0x130, 0x134, 0x7, 0x2a, 
    0x2, 0x2, 0x131, 0x135, 0x5, 0x18, 0xd, 0x2, 0x132, 0x135, 0x5, 0x1a, 
    0xe, 0x2, 0x133, 0x135, 0x7, 0x54, 0x2, 0x2, 0x134, 0x131, 0x3, 0x2, 
    0x2, 0x2, 0x134, 0x132, 0x3, 0x2, 0x2, 0x2, 0x134, 0x133, 0x3, 0x2, 
    0x2, 0x2, 0x135, 0x138, 0x3, 0x2, 0x2, 0x2, 0x136, 0x137, 0x7, 0x2e, 
    0x2, 0x2, 0x137, 0x139, 0x7, 0x4b, 0x2, 0x2, 0x138, 0x136, 0x3, 0x2, 
    0x2, 0x2, 0x138, 0x139, 0x3, 0x2, 0x2, 0x2, 0x139, 0x3b, 0x3, 0x2, 0x2, 
    0x2, 0x13a, 0x13b, 0x7, 0x3a, 0x2, 0x2, 0x13b, 0x13d, 0x7, 0x16, 0x2, 
    0x2, 0x13c, 0x13e, 0x5, 0xe, 0x8, 0x2, 0x13d, 0x13c, 0x3, 0x2, 0x2, 
    0x2, 0x13e, 0x13f, 0x3, 0x2, 0x2, 0x2, 0x13f, 0x13d, 0x3, 0x2, 0x2, 
    0x2, 0x13f, 0x140, 0x3, 0x2, 0x2, 0x2, 0x140, 0x142, 0x3, 0x2, 0x2, 
    0x2, 0x141, 0x143, 0x5, 0x10, 0x9, 0x2, 0x142, 0x141, 0x3, 0x2, 0x2, 
    0x2, 0x142, 0x143, 0x3, 0x2, 0x2, 0x2, 0x143, 0x144, 0x3, 0x2, 0x2, 
    0x2, 0x144, 0x145, 0x7, 0x9, 0x2, 0x2, 0x145, 0x3d, 0x3, 0x2, 0x2, 0x2, 
    0x146, 0x147, 0x7, 0x3b, 0x2, 0x2, 0x147, 0x149, 0x7, 0x54, 0x2, 0x2, 
    0x148, 0x14a, 0x5, 0x6, 0x4, 0x2, 0x149, 0x148, 0x3, 0x2, 0x2, 0x2, 
    0x149, 0x14a, 0x3, 0x2, 0x2, 0x2, 0x14a, 0x14c, 0x3, 0x2, 0x2, 0x2, 
    0x14b, 0x14d, 0x5, 0xa, 0x6, 0x2, 0x14c, 0x14b, 0x3, 0x2, 0x2, 0x2, 
    0x14d, 0x14e, 0x3, 0x2, 0x2, 0x2, 0x14e, 0x14c, 0x3, 0x2, 0x2, 0x2, 
    0x14e, 0x14f, 0x3, 0x2, 0x2, 0x2, 0x14f, 0x150, 0x3, 0x2, 0x2, 0x2, 
    0x150, 0x151, 0x7, 0x9, 0x2, 0x2, 0x151, 0x152, 0x7, 0x54, 0x2, 0x2, 
    0x152, 0x3f, 0x3, 0x2, 0x2, 0x2, 0x153, 0x154, 0x7, 0xb, 0x2, 0x2, 0x154, 
    0x165, 0x7, 0x54, 0x2, 0x2, 0x155, 0x156, 0x7, 0x54, 0x2, 0x2, 0x156, 
    0x157, 0x7, 0x17, 0x2, 0x2, 0x157, 0x158, 0x7, 0x54, 0x2, 0x2, 0x158, 
    0x159, 0x7, 0x19, 0x2, 0x2, 0x159, 0x15e, 0x5, 0x42, 0x22, 0x2, 0x15a, 
    0x15b, 0x7, 0x1a, 0x2, 0x2, 0x15b, 0x15d, 0x5, 0x42, 0x22, 0x2, 0x15c, 
    0x15a, 0x3, 0x2, 0x2, 0x2, 0x15d, 0x160, 0x3, 0x2, 0x2, 0x2, 0x15e, 
    0x15c, 0x3, 0x2, 0x2, 0x2, 0x15e, 0x15f, 0x3, 0x2, 0x2, 0x2, 0x15f, 
    0x161, 0x3, 0x2, 0x2, 0x2, 0x160, 0x15e, 0x3, 0x2, 0x2, 0x2, 0x161, 
    0x162, 0x7, 0x15, 0x2, 0x2, 0x162, 0x164, 0x3, 0x2, 0x2, 0x2, 0x163, 
    0x155, 0x3, 0x2, 0x2, 0x2, 0x164, 0x167, 0x3, 0x2, 0x2, 0x2, 0x165, 
    0x163, 0x3, 0x2, 0x2, 0x2, 0x165, 0x166, 0x3, 0x2, 0x2, 0x2, 0x166, 
    0x168, 0x3, 0x2, 0x2, 0x2, 0x167, 0x165, 0x3, 0x2, 0x2, 0x2, 0x168, 
    0x169, 0x7, 0x9, 0x2, 0x2, 0x169, 0x16a, 0x7, 0x54, 0x2, 0x2, 0x16a, 
    0x16b, 0x7, 0x17, 0x2, 0x2, 0x16b, 0x41, 0x3, 0x2, 0x2, 0x2, 0x16c, 
    0x16d, 0x7, 0x3c, 0x2, 0x2, 0x16d, 0x16e, 0x7, 0x54, 0x2, 0x2, 0x16e, 
    0x17d, 0x7, 0x4b, 0x2, 0x2, 0x16f, 0x170, 0x7, 0x27, 0x2, 0x2, 0x170, 
    0x171, 0x7, 0x3d, 0x2, 0x2, 0x171, 0x17d, 0x7, 0x54, 0x2, 0x2, 0x172, 
    0x173, 0x7, 0x3e, 0x2, 0x2, 0x173, 0x176, 0x7, 0x54, 0x2, 0x2, 0x174, 
    0x175, 0x7, 0x27, 0x2, 0x2, 0x175, 0x177, 0x7, 0x3d, 0x2, 0x2, 0x176, 
    0x174, 0x3, 0x2, 0x2, 0x2, 0x176, 0x177, 0x3, 0x2, 0x2, 0x2, 0x177, 
    0x17d, 0x3, 0x2, 0x2, 0x2, 0x178, 0x179, 0x7, 0x1b, 0x2, 0x2, 0x179, 
    0x17a, 0x7, 0x54, 0x2, 0x2, 0x17a, 0x17b, 0x7, 0x17, 0x2, 0x2, 0x17b, 
    0x17d, 0x7, 0x54, 0x2, 0x2, 0x17c, 0x16c, 0x3, 0x2, 0x2, 0x2, 0x17c, 
    0x16f, 0x3, 0x2, 0x2, 0x2, 0x17c, 0x172, 0x3, 0x2, 0x2, 0x2, 0x17c, 
    0x178, 0x3, 0x2, 0x2, 0x2, 0x17d, 0x43, 0x3, 0x2, 0x2, 0x2, 0x17e, 0x189, 
    0x7, 0x21, 0x2, 0x2, 0x17f, 0x18a, 0x7, 0x3f, 0x2, 0x2, 0x180, 0x181, 
    0x7, 0x40, 0x2, 0x2, 0x181, 0x182, 0x7, 0x27, 0x2, 0x2, 0x182, 0x183, 
    0x7, 0x41, 0x2, 0x2, 0x183, 0x184, 0x7, 0x16, 0x2, 0x2, 0x184, 0x185, 
    0x7, 0x4e, 0x2, 0x2, 0x185, 0x186, 0x7, 0x1a, 0x2, 0x2, 0x186, 0x187, 
    0x7, 0x42, 0x2, 0x2, 0x187, 0x188, 0x7, 0x16, 0x2, 0x2, 0x188, 0x18a, 
    0x7, 0x4e, 0x2, 0x2, 0x189, 0x17f, 0x3, 0x2, 0x2, 0x2, 0x189, 0x180, 
    0x3, 0x2, 0x2, 0x2, 0x18a, 0x18b, 0x3, 0x2, 0x2, 0x2, 0x18b, 0x18c, 
    0x7, 0x15, 0x2, 0x2, 0x18c, 0x18e, 0x7, 0x43, 0x2, 0x2, 0x18d, 0x18f, 
    0x5, 0x46, 0x24, 0x2, 0x18e, 0x18d, 0x3, 0x2, 0x2, 0x2, 0x18e, 0x18f, 
    0x3, 0x2, 0x2, 0x2, 0x18f, 0x190, 0x3, 0x2, 0x2, 0x2, 0x190, 0x191, 
    0x7, 0x45, 0x2, 0x2, 0x191, 0x194, 0x7, 0x16, 0x2, 0x2, 0x192, 0x195, 
    0x7, 0x46, 0x2, 0x2, 0x193, 0x195, 0x5, 0x48, 0x25, 0x2, 0x194, 0x192, 
    0x3, 0x2, 0x2, 0x2, 0x194, 0x193, 0x3, 0x2, 0x2, 0x2, 0x195, 0x196, 
    0x3, 0x2, 0x2, 0x2, 0x196, 0x197, 0x7, 0x1a, 0x2, 0x2, 0x197, 0x198, 
    0x7, 0x1f, 0x2, 0x2, 0x198, 0x19b, 0x7, 0x16, 0x2, 0x2, 0x199, 0x19c, 
    0x7, 0x46, 0x2, 0x2, 0x19a, 0x19c, 0x5, 0x48, 0x25, 0x2, 0x19b, 0x199, 
    0x3, 0x2, 0x2, 0x2, 0x19b, 0x19a, 0x3, 0x2, 0x2, 0x2, 0x19c, 0x19d, 
    0x3, 0x2, 0x2, 0x2, 0x19d, 0x19e, 0x7, 0x1a, 0x2, 0x2, 0x19e, 0x19f, 
    0x7, 0x47, 0x2, 0x2, 0x19f, 0x1a2, 0x7, 0x16, 0x2, 0x2, 0x1a0, 0x1a3, 
    0x7, 0x46, 0x2, 0x2, 0x1a1, 0x1a3, 0x5, 0x48, 0x25, 0x2, 0x1a2, 0x1a0, 
    0x3, 0x2, 0x2, 0x2, 0x1a2, 0x1a1, 0x3, 0x2, 0x2, 0x2, 0x1a3, 0x1a4, 
    0x3, 0x2, 0x2, 0x2, 0x1a4, 0x1a5, 0x7, 0x15, 0x2, 0x2, 0x1a5, 0x1a6, 
    0x7, 0x48, 0x2, 0x2, 0x1a6, 0x1a7, 0x7, 0x16, 0x2, 0x2, 0x1a7, 0x1a8, 
    0x9, 0x3, 0x2, 0x2, 0x1a8, 0x1a9, 0x7, 0x15, 0x2, 0x2, 0x1a9, 0x1aa, 
    0x7, 0x9, 0x2, 0x2, 0x1aa, 0x1ab, 0x7, 0x17, 0x2, 0x2, 0x1ab, 0x45, 
    0x3, 0x2, 0x2, 0x2, 0x1ac, 0x1ad, 0x7, 0x44, 0x2, 0x2, 0x1ad, 0x1ae, 
    0x7, 0x16, 0x2, 0x2, 0x1ae, 0x1af, 0x7, 0x4b, 0x2, 0x2, 0x1af, 0x1b0, 
    0x7, 0x15, 0x2, 0x2, 0x1b0, 0x47, 0x3, 0x2, 0x2, 0x2, 0x1b1, 0x1b2, 
    0x9, 0x4, 0x2, 0x2, 0x1b2, 0x49, 0x3, 0x2, 0x2, 0x2, 0x1b3, 0x1b4, 0x9, 
    0x5, 0x2, 0x2, 0x1b4, 0x4b, 0x3, 0x2, 0x2, 0x2, 0x2b, 0x50, 0x53, 0x59, 
    0x62, 0x67, 0x71, 0x7d, 0x7f, 0x86, 0x8e, 0x98, 0x9d, 0xa0, 0xaa, 0xac, 
    0xb3, 0xbc, 0xc9, 0xf7, 0xfe, 0x108, 0x10e, 0x115, 0x118, 0x121, 0x129, 
    0x134, 0x138, 0x13f, 0x142, 0x149, 0x14e, 0x15e, 0x165, 0x176, 0x17c, 
    0x189, 0x18e, 0x194, 0x19b, 0x1a2, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

Ili1Parser::Initializer Ili1Parser::_init;
