
// Generated from IliFileParser.g4 by ANTLR 4.7.1


#include "IliFileParserVisitor.h"

#include "IliFileParser.h"


using namespace antlrcpp;
using namespace parser;
using namespace antlr4;

IliFileParser::IliFileParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

IliFileParser::~IliFileParser() {
  delete _interpreter;
}

std::string IliFileParser::getGrammarFileName() const {
  return "IliFileParser.g4";
}

const std::vector<std::string>& IliFileParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& IliFileParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- IliFileContext ------------------------------------------------------------------

IliFileParser::IliFileContext::IliFileContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

IliFileParser::VersionContext* IliFileParser::IliFileContext::version() {
  return getRuleContext<IliFileParser::VersionContext>(0);
}

std::vector<IliFileParser::ModelNameContext *> IliFileParser::IliFileContext::modelName() {
  return getRuleContexts<IliFileParser::ModelNameContext>();
}

IliFileParser::ModelNameContext* IliFileParser::IliFileContext::modelName(size_t i) {
  return getRuleContext<IliFileParser::ModelNameContext>(i);
}

std::vector<IliFileParser::TranslationOfContext *> IliFileParser::IliFileContext::translationOf() {
  return getRuleContexts<IliFileParser::TranslationOfContext>();
}

IliFileParser::TranslationOfContext* IliFileParser::IliFileContext::translationOf(size_t i) {
  return getRuleContext<IliFileParser::TranslationOfContext>(i);
}

std::vector<IliFileParser::ImportsContext *> IliFileParser::IliFileContext::imports() {
  return getRuleContexts<IliFileParser::ImportsContext>();
}

IliFileParser::ImportsContext* IliFileParser::IliFileContext::imports(size_t i) {
  return getRuleContext<IliFileParser::ImportsContext>(i);
}


size_t IliFileParser::IliFileContext::getRuleIndex() const {
  return IliFileParser::RuleIliFile;
}

antlrcpp::Any IliFileParser::IliFileContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<IliFileParserVisitor*>(visitor))
    return parserVisitor->visitIliFile(this);
  else
    return visitor->visitChildren(this);
}

IliFileParser::IliFileContext* IliFileParser::iliFile() {
  IliFileContext *_localctx = _tracker.createInstance<IliFileContext>(_ctx, getState());
  enterRule(_localctx, 0, IliFileParser::RuleIliFile);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(12);
    version();
    setState(19);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx);
    while (alt != 1 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1 + 1) {
        setState(17);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx)) {
        case 1: {
          setState(13);
          modelName();
          break;
        }

        case 2: {
          setState(14);
          translationOf();
          break;
        }

        case 3: {
          setState(15);
          imports();
          break;
        }

        case 4: {
          setState(16);
          matchWildcard();
          break;
        }

        } 
      }
      setState(21);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VersionContext ------------------------------------------------------------------

IliFileParser::VersionContext::VersionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* IliFileParser::VersionContext::INTERLIS() {
  return getToken(IliFileParser::INTERLIS, 0);
}

tree::TerminalNode* IliFileParser::VersionContext::DOT() {
  return getToken(IliFileParser::DOT, 0);
}

std::vector<tree::TerminalNode *> IliFileParser::VersionContext::DIGIT() {
  return getTokens(IliFileParser::DIGIT);
}

tree::TerminalNode* IliFileParser::VersionContext::DIGIT(size_t i) {
  return getToken(IliFileParser::DIGIT, i);
}

tree::TerminalNode* IliFileParser::VersionContext::TRANSFER() {
  return getToken(IliFileParser::TRANSFER, 0);
}

tree::TerminalNode* IliFileParser::VersionContext::SEMI() {
  return getToken(IliFileParser::SEMI, 0);
}

tree::TerminalNode* IliFileParser::VersionContext::NAME() {
  return getToken(IliFileParser::NAME, 0);
}


size_t IliFileParser::VersionContext::getRuleIndex() const {
  return IliFileParser::RuleVersion;
}

antlrcpp::Any IliFileParser::VersionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<IliFileParserVisitor*>(visitor))
    return parserVisitor->visitVersion(this);
  else
    return visitor->visitChildren(this);
}

IliFileParser::VersionContext* IliFileParser::version() {
  VersionContext *_localctx = _tracker.createInstance<VersionContext>(_ctx, getState());
  enterRule(_localctx, 2, IliFileParser::RuleVersion);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(29);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case IliFileParser::INTERLIS: {
        enterOuterAlt(_localctx, 1);
        setState(22);
        match(IliFileParser::INTERLIS);
        setState(23);
        dynamic_cast<VersionContext *>(_localctx)->majorversion = match(IliFileParser::DIGIT);
        setState(24);
        match(IliFileParser::DOT);
        setState(25);
        dynamic_cast<VersionContext *>(_localctx)->minorversion = match(IliFileParser::DIGIT);
        break;
      }

      case IliFileParser::TRANSFER: {
        enterOuterAlt(_localctx, 2);
        setState(26);
        match(IliFileParser::TRANSFER);
        setState(27);
        dynamic_cast<VersionContext *>(_localctx)->transfername = match(IliFileParser::NAME);
        setState(28);
        match(IliFileParser::SEMI);
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

//----------------- ModelNameContext ------------------------------------------------------------------

IliFileParser::ModelNameContext::ModelNameContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* IliFileParser::ModelNameContext::MODEL() {
  return getToken(IliFileParser::MODEL, 0);
}

tree::TerminalNode* IliFileParser::ModelNameContext::NAME() {
  return getToken(IliFileParser::NAME, 0);
}


size_t IliFileParser::ModelNameContext::getRuleIndex() const {
  return IliFileParser::RuleModelName;
}

antlrcpp::Any IliFileParser::ModelNameContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<IliFileParserVisitor*>(visitor))
    return parserVisitor->visitModelName(this);
  else
    return visitor->visitChildren(this);
}

IliFileParser::ModelNameContext* IliFileParser::modelName() {
  ModelNameContext *_localctx = _tracker.createInstance<ModelNameContext>(_ctx, getState());
  enterRule(_localctx, 4, IliFileParser::RuleModelName);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(31);
    match(IliFileParser::MODEL);
    setState(32);
    dynamic_cast<ModelNameContext *>(_localctx)->modelname = match(IliFileParser::NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TranslationOfContext ------------------------------------------------------------------

IliFileParser::TranslationOfContext::TranslationOfContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* IliFileParser::TranslationOfContext::TRANSLATION() {
  return getToken(IliFileParser::TRANSLATION, 0);
}

tree::TerminalNode* IliFileParser::TranslationOfContext::OF() {
  return getToken(IliFileParser::OF, 0);
}

tree::TerminalNode* IliFileParser::TranslationOfContext::NAME() {
  return getToken(IliFileParser::NAME, 0);
}


size_t IliFileParser::TranslationOfContext::getRuleIndex() const {
  return IliFileParser::RuleTranslationOf;
}

antlrcpp::Any IliFileParser::TranslationOfContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<IliFileParserVisitor*>(visitor))
    return parserVisitor->visitTranslationOf(this);
  else
    return visitor->visitChildren(this);
}

IliFileParser::TranslationOfContext* IliFileParser::translationOf() {
  TranslationOfContext *_localctx = _tracker.createInstance<TranslationOfContext>(_ctx, getState());
  enterRule(_localctx, 6, IliFileParser::RuleTranslationOf);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(34);
    match(IliFileParser::TRANSLATION);
    setState(35);
    match(IliFileParser::OF);
    setState(36);
    dynamic_cast<TranslationOfContext *>(_localctx)->modelname = match(IliFileParser::NAME);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImportsContext ------------------------------------------------------------------

IliFileParser::ImportsContext::ImportsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* IliFileParser::ImportsContext::IMPORTS() {
  return getToken(IliFileParser::IMPORTS, 0);
}

std::vector<IliFileParser::ModelImportContext *> IliFileParser::ImportsContext::modelImport() {
  return getRuleContexts<IliFileParser::ModelImportContext>();
}

IliFileParser::ModelImportContext* IliFileParser::ImportsContext::modelImport(size_t i) {
  return getRuleContext<IliFileParser::ModelImportContext>(i);
}

std::vector<tree::TerminalNode *> IliFileParser::ImportsContext::COMMA() {
  return getTokens(IliFileParser::COMMA);
}

tree::TerminalNode* IliFileParser::ImportsContext::COMMA(size_t i) {
  return getToken(IliFileParser::COMMA, i);
}


size_t IliFileParser::ImportsContext::getRuleIndex() const {
  return IliFileParser::RuleImports;
}

antlrcpp::Any IliFileParser::ImportsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<IliFileParserVisitor*>(visitor))
    return parserVisitor->visitImports(this);
  else
    return visitor->visitChildren(this);
}

IliFileParser::ImportsContext* IliFileParser::imports() {
  ImportsContext *_localctx = _tracker.createInstance<ImportsContext>(_ctx, getState());
  enterRule(_localctx, 8, IliFileParser::RuleImports);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(38);
    match(IliFileParser::IMPORTS);
    setState(39);
    modelImport();
    setState(44);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(40);
        match(IliFileParser::COMMA);
        setState(41);
        modelImport(); 
      }
      setState(46);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ModelImportContext ------------------------------------------------------------------

IliFileParser::ModelImportContext::ModelImportContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* IliFileParser::ModelImportContext::UNQUALIFIED() {
  return getToken(IliFileParser::UNQUALIFIED, 0);
}

tree::TerminalNode* IliFileParser::ModelImportContext::NAME() {
  return getToken(IliFileParser::NAME, 0);
}

tree::TerminalNode* IliFileParser::ModelImportContext::INTERLIS() {
  return getToken(IliFileParser::INTERLIS, 0);
}


size_t IliFileParser::ModelImportContext::getRuleIndex() const {
  return IliFileParser::RuleModelImport;
}

antlrcpp::Any IliFileParser::ModelImportContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<IliFileParserVisitor*>(visitor))
    return parserVisitor->visitModelImport(this);
  else
    return visitor->visitChildren(this);
}

IliFileParser::ModelImportContext* IliFileParser::modelImport() {
  ModelImportContext *_localctx = _tracker.createInstance<ModelImportContext>(_ctx, getState());
  enterRule(_localctx, 10, IliFileParser::RuleModelImport);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(48);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == IliFileParser::UNQUALIFIED) {
      setState(47);
      match(IliFileParser::UNQUALIFIED);
    }
    setState(52);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case IliFileParser::NAME: {
        setState(50);
        dynamic_cast<ModelImportContext *>(_localctx)->modelname = match(IliFileParser::NAME);
        break;
      }

      case IliFileParser::INTERLIS: {
        setState(51);
        dynamic_cast<ModelImportContext *>(_localctx)->modelname = match(IliFileParser::INTERLIS);
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

// Static vars and initialization.
std::vector<dfa::DFA> IliFileParser::_decisionToDFA;
atn::PredictionContextCache IliFileParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN IliFileParser::_atn;
std::vector<uint16_t> IliFileParser::_serializedATN;

std::vector<std::string> IliFileParser::_ruleNames = {
  "iliFile", "version", "modelName", "translationOf", "imports", "modelImport"
};

std::vector<std::string> IliFileParser::_literalNames = {
  "", "'INTERLIS'", "'.'", "','", "';'", "'TRANSFER'", "'MODEL'", "'TRANSLATION'", 
  "'OF'", "'IMPORTS'", "'UNQUALIFIED'"
};

std::vector<std::string> IliFileParser::_symbolicNames = {
  "", "INTERLIS", "DOT", "COMMA", "SEMI", "TRANSFER", "MODEL", "TRANSLATION", 
  "OF", "IMPORTS", "UNQUALIFIED", "NAME", "DIGIT", "LETTER", "WS", "SL_COMMENT", 
  "ILI_DOC", "ILI_BLOCKCOMMENT", "ANYCHAR"
};

dfa::Vocabulary IliFileParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> IliFileParser::_tokenNames;

IliFileParser::Initializer::Initializer() {
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
    0x3, 0x14, 0x39, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x3, 
    0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x2, 0x7, 0x2, 0x14, 0xa, 0x2, 
    0xc, 0x2, 0xe, 0x2, 0x17, 0xb, 0x2, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0x20, 0xa, 0x3, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x7, 0x6, 0x2d, 0xa, 0x6, 0xc, 0x6, 0xe, 
    0x6, 0x30, 0xb, 0x6, 0x3, 0x7, 0x5, 0x7, 0x33, 0xa, 0x7, 0x3, 0x7, 0x3, 
    0x7, 0x5, 0x7, 0x37, 0xa, 0x7, 0x3, 0x7, 0x3, 0x15, 0x2, 0x8, 0x2, 0x4, 
    0x6, 0x8, 0xa, 0xc, 0x2, 0x2, 0x2, 0x3a, 0x2, 0xe, 0x3, 0x2, 0x2, 0x2, 
    0x4, 0x1f, 0x3, 0x2, 0x2, 0x2, 0x6, 0x21, 0x3, 0x2, 0x2, 0x2, 0x8, 0x24, 
    0x3, 0x2, 0x2, 0x2, 0xa, 0x28, 0x3, 0x2, 0x2, 0x2, 0xc, 0x32, 0x3, 0x2, 
    0x2, 0x2, 0xe, 0x15, 0x5, 0x4, 0x3, 0x2, 0xf, 0x14, 0x5, 0x6, 0x4, 0x2, 
    0x10, 0x14, 0x5, 0x8, 0x5, 0x2, 0x11, 0x14, 0x5, 0xa, 0x6, 0x2, 0x12, 
    0x14, 0xb, 0x2, 0x2, 0x2, 0x13, 0xf, 0x3, 0x2, 0x2, 0x2, 0x13, 0x10, 
    0x3, 0x2, 0x2, 0x2, 0x13, 0x11, 0x3, 0x2, 0x2, 0x2, 0x13, 0x12, 0x3, 
    0x2, 0x2, 0x2, 0x14, 0x17, 0x3, 0x2, 0x2, 0x2, 0x15, 0x16, 0x3, 0x2, 
    0x2, 0x2, 0x15, 0x13, 0x3, 0x2, 0x2, 0x2, 0x16, 0x3, 0x3, 0x2, 0x2, 
    0x2, 0x17, 0x15, 0x3, 0x2, 0x2, 0x2, 0x18, 0x19, 0x7, 0x3, 0x2, 0x2, 
    0x19, 0x1a, 0x7, 0xe, 0x2, 0x2, 0x1a, 0x1b, 0x7, 0x4, 0x2, 0x2, 0x1b, 
    0x20, 0x7, 0xe, 0x2, 0x2, 0x1c, 0x1d, 0x7, 0x7, 0x2, 0x2, 0x1d, 0x1e, 
    0x7, 0xd, 0x2, 0x2, 0x1e, 0x20, 0x7, 0x6, 0x2, 0x2, 0x1f, 0x18, 0x3, 
    0x2, 0x2, 0x2, 0x1f, 0x1c, 0x3, 0x2, 0x2, 0x2, 0x20, 0x5, 0x3, 0x2, 
    0x2, 0x2, 0x21, 0x22, 0x7, 0x8, 0x2, 0x2, 0x22, 0x23, 0x7, 0xd, 0x2, 
    0x2, 0x23, 0x7, 0x3, 0x2, 0x2, 0x2, 0x24, 0x25, 0x7, 0x9, 0x2, 0x2, 
    0x25, 0x26, 0x7, 0xa, 0x2, 0x2, 0x26, 0x27, 0x7, 0xd, 0x2, 0x2, 0x27, 
    0x9, 0x3, 0x2, 0x2, 0x2, 0x28, 0x29, 0x7, 0xb, 0x2, 0x2, 0x29, 0x2e, 
    0x5, 0xc, 0x7, 0x2, 0x2a, 0x2b, 0x7, 0x5, 0x2, 0x2, 0x2b, 0x2d, 0x5, 
    0xc, 0x7, 0x2, 0x2c, 0x2a, 0x3, 0x2, 0x2, 0x2, 0x2d, 0x30, 0x3, 0x2, 
    0x2, 0x2, 0x2e, 0x2c, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x2f, 0x3, 0x2, 0x2, 
    0x2, 0x2f, 0xb, 0x3, 0x2, 0x2, 0x2, 0x30, 0x2e, 0x3, 0x2, 0x2, 0x2, 
    0x31, 0x33, 0x7, 0xc, 0x2, 0x2, 0x32, 0x31, 0x3, 0x2, 0x2, 0x2, 0x32, 
    0x33, 0x3, 0x2, 0x2, 0x2, 0x33, 0x36, 0x3, 0x2, 0x2, 0x2, 0x34, 0x37, 
    0x7, 0xd, 0x2, 0x2, 0x35, 0x37, 0x7, 0x3, 0x2, 0x2, 0x36, 0x34, 0x3, 
    0x2, 0x2, 0x2, 0x36, 0x35, 0x3, 0x2, 0x2, 0x2, 0x37, 0xd, 0x3, 0x2, 
    0x2, 0x2, 0x8, 0x13, 0x15, 0x1f, 0x2e, 0x32, 0x36, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

IliFileParser::Initializer IliFileParser::_init;
