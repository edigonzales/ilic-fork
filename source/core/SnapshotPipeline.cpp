#include "SnapshotPipeline.h"

#include "SourceRangeMapper.h"

#include "../../include/ilic/Compiler.h"
#include "../input/ili1/Ili1Input.h"
#include "../input/ili2/Ili2Input.h"
#include "../input/parser/generated/Ili1Lexer.h"
#include "../input/parser/generated/Ili1Parser.h"
#include "../input/parser/generated/Ili2Lexer.h"
#include "../input/parser/generated/Ili2Parser.h"

#include "BaseErrorListener.h"
#include "CommonTokenStream.h"
#include "ParserRuleContext.h"
#include "tree/ParseTree.h"
#include "tree/TerminalNode.h"

#include "../util/Logger.h"
#include "../input/parser/ParserDiagnosticTranslator.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ilic::detail {

const char *toString(ParseMode mode) noexcept
{
   switch (mode) {
      case ParseMode::StrictCompiler: return "strict-compiler";
      case ParseMode::TolerantEditor: return "tolerant-editor";
   }
   return "unknown";
}

namespace {

enum class DetectedLanguage { Unknown,Ili1,Ili2 };

DetectedLanguage detectLanguage(const SourceBuffer &source)
{
   const std::string &text = source.text;
   std::size_t offset = 0;
   while (offset < text.size()) {
      while (offset < text.size() && std::isspace(static_cast<unsigned char>(text[offset]))) ++offset;
      if (offset + 1 < text.size() && text[offset] == '!' && text[offset + 1] == '!') {
         offset += 2;
         while (offset < text.size() && text[offset] != '\n') ++offset;
         continue;
      }
      if (offset + 1 < text.size() && text[offset] == '/' && text[offset + 1] == '*') {
         offset += 2;
         std::size_t depth = 1;
         while (offset < text.size() && depth != 0) {
            if (offset + 1 < text.size() && text[offset] == '/' && text[offset + 1] == '*') {
               ++depth;
               offset += 2;
            }
            else if (offset + 1 < text.size() && text[offset] == '*' && text[offset + 1] == '/') {
               --depth;
               offset += 2;
            }
            else ++offset;
         }
         continue;
      }
      const std::size_t begin = offset;
      while (offset < text.size()
         && (std::isalpha(static_cast<unsigned char>(text[offset])) || text[offset] == '_')) ++offset;
      if (begin == offset) return DetectedLanguage::Unknown;
      const std::string_view word(text.data() + begin,offset - begin);
      if (word == "TRANSFER") return DetectedLanguage::Ili1;
      if (word == "INTERLIS") return DetectedLanguage::Ili2;
      return DetectedLanguage::Unknown;
   }
   return DetectedLanguage::Unknown;
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

class EditorTokenStream final : public antlr4::CommonTokenStream {
public:
   explicit EditorTokenStream(antlr4::TokenSource *source)
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

bool isSyntaxContextRule(const std::string &kind)
{
   static const std::set<std::string> rules = {
      "modelDef","topicDef","classDef","structureDef","associationDef","viewDef",
      "graphicDef","attributeDef","domainDef","unitDef","importDef","importing",
      "textType","numericType","properties","path"
   };
   return rules.find(kind) != rules.end();
}

template<class Parser>
void appendSyntaxNodes(antlr4::tree::ParseTree *tree,const Parser &parser,
   const SourceRangeMapper &ranges,SyntaxSnapshot &snapshot,std::size_t parent,bool hasParent)
{
   if (tree == nullptr) return;
   auto *rule = dynamic_cast<antlr4::ParserRuleContext *>(tree);
   std::size_t currentParent = parent;
   bool currentHasParent = hasParent;
   if (rule != nullptr) {
      const auto &ruleNames = parser.getRuleNames();
      const std::size_t ruleIndex = rule->getRuleIndex();
      const std::string kind = ruleIndex < ruleNames.size() ? ruleNames[ruleIndex] : "unknown";
      SyntaxNode node;
      node.id = snapshot.nodes.size();
      node.parent = parent;
      node.hasParent = hasParent;
      node.kind = kind;
      node.range = ranges.context(rule);
      snapshot.nodes.push_back(node);
      if (isSyntaxContextRule(kind)) snapshot.contexts.push_back({kind,node.range});
      currentParent = node.id;
      currentHasParent = true;
   }
   for (auto *child : tree->children)
      appendSyntaxNodes(child,parser,ranges,snapshot,currentParent,currentHasParent);
}

template<class Lexer>
void appendSyntaxTokens(antlr4::CommonTokenStream &tokens,const Lexer &lexer,
   const SourceRangeMapper &ranges,SyntaxSnapshot &snapshot)
{
   tokens.fill();
   for (auto *token : tokens.getTokens()) {
      if (token == nullptr || token->getType() == antlr4::Token::EOF) continue;
      std::string kind = lexer.getVocabulary().getSymbolicName(token->getType());
      if (kind.empty()) kind = lexer.getVocabulary().getLiteralName(token->getType());
      snapshot.tokens.push_back({kind,token->getText(),token->getChannel(),ranges.token(token)});
   }
}

void sortDiagnostics(std::vector<Diagnostic> &diagnostics)
{
   std::stable_sort(diagnostics.begin(),diagnostics.end(),[](const Diagnostic &left,const Diagnostic &right) {
      const std::size_t leftStart = left.range.valid ? left.range.start.byteOffset : 0;
      const std::size_t rightStart = right.range.valid ? right.range.start.byteOffset : 0;
      const std::size_t leftEnd = left.range.valid ? left.range.end.byteOffset : 0;
      const std::size_t rightEnd = right.range.valid ? right.range.end.byteOffset : 0;
      if (left.range.uri != right.range.uri) return left.range.uri < right.range.uri;
      if (leftStart != rightStart) return leftStart < rightStart;
      if (leftEnd != rightEnd) return leftEnd < rightEnd;
      if (left.severity != right.severity) return static_cast<int>(left.severity) < static_cast<int>(right.severity);
      if (left.code != right.code) return left.code < right.code;
      return left.message < right.message;
   });
   diagnostics.erase(std::unique(diagnostics.begin(),diagnostics.end(),[](const Diagnostic &left,const Diagnostic &right) {
      return left.code == right.code && left.severity == right.severity
         && left.message == right.message && left.range.valid == right.range.valid
         && (!left.range.valid || (left.range.uri == right.range.uri
            && left.range.start.byteOffset == right.range.start.byteOffset
            && left.range.end.byteOffset == right.range.end.byteOffset));
   }),diagnostics.end());
}

bool isContainer(EditorSymbolKind kind)
{
   return kind == EditorSymbolKind::Model || kind == EditorSymbolKind::Topic
      || kind == EditorSymbolKind::Class || kind == EditorSymbolKind::Structure
      || kind == EditorSymbolKind::Association || kind == EditorSymbolKind::View
      || kind == EditorSymbolKind::Graphic;
}

const char *symbolKind(EditorSymbolKind kind)
{
   switch (kind) {
      case EditorSymbolKind::Model: return "model";
      case EditorSymbolKind::Topic: return "topic";
      case EditorSymbolKind::Class: return "class";
      case EditorSymbolKind::Structure: return "structure";
      case EditorSymbolKind::Association: return "association";
      case EditorSymbolKind::View: return "view";
      case EditorSymbolKind::Graphic: return "graphic";
      case EditorSymbolKind::Domain: return "domain";
      case EditorSymbolKind::Unit: return "unit";
      case EditorSymbolKind::Attribute: return "attribute";
   }
   return "attribute";
}

const char *referenceKind(EditorReferenceKind kind)
{
   switch (kind) {
      case EditorReferenceKind::Extends: return "extends";
      case EditorReferenceKind::Type: return "type";
      case EditorReferenceKind::Collection: return "collection";
      case EditorReferenceKind::Reference: return "reference";
      case EditorReferenceKind::Unit: return "unit";
   }
   return "type";
}

class EditorSnapshotAccumulator final {
public:
   static constexpr std::size_t noDeclaration = static_cast<std::size_t>(-1);

   EditorSnapshotAccumulator(const SourceRangeMapper &ranges,std::string iliVersion)
      : ranges_(ranges)
   {
      snapshot_.uri = ranges.source().uri;
      snapshot_.documentVersion = ranges.source().version;
      snapshot_.iliVersion = std::move(iliVersion);
   }

   std::size_t addDeclaration(const antlr4::ParserRuleContext *context,EditorSymbolKind kind,
      const antlr4::Token *name,const antlr4::Token *endName,std::size_t containerIndex)
   {
      if (context == nullptr || name == nullptr || name->getStartIndex() == INVALID_INDEX)
         return noDeclaration;
      const SourceRange selection = ranges_.token(name);
      if (!selection.valid) return noDeclaration;
      EditorDeclaration declaration;
      declaration.name = name->getText();
      declaration.kind = kind;
      declaration.id = std::string(symbolKind(kind)) + ":"
         + std::to_string(selection.start.byteOffset) + ":" + declaration.name;
      declaration.range = ranges_.context(context);
      declaration.selectionRange = selection;
      if (endName != nullptr && endName->getStartIndex() != INVALID_INDEX)
         declaration.endRange = ranges_.token(endName);
      if (containerIndex != noDeclaration) {
         const EditorDeclaration &container = snapshot_.declarations[containerIndex];
         declaration.qualifiedName = container.qualifiedName + "." + declaration.name;
         declaration.containerId = container.id;
         declaration.hasContainer = true;
      }
      if (declaration.qualifiedName.empty()) declaration.qualifiedName = declaration.name;
      const std::size_t index = snapshot_.declarations.size();
      snapshot_.declarations.push_back(std::move(declaration));
      if (isContainer(kind)) snapshot_.contexts.push_back({containerRule(kind),snapshot_.declarations.back().range});
      if (isContainer(kind) && endName == nullptr) recovered_ = true;
      if (endName != nullptr && endName->getStartIndex() != INVALID_INDEX
         && !sameName(snapshot_.declarations.back().name,endName->getText())) {
         Diagnostic diagnostic;
         diagnostic.severity = DiagnosticSeverity::Error;
         diagnostic.code = "ILIC-LIVE-END-NAME";
         diagnostic.message = "Expected END " + snapshot_.declarations.back().name;
         diagnostic.range = ranges_.token(endName);
         diagnostic.relatedInformation.push_back({snapshot_.declarations.back().selectionRange,"Declaration"});
         diagnostic.source = "live";
         diagnostic.phase = DiagnosticPhase::EditorRecovery;
         diagnostic.tags.push_back(DiagnosticTag::Recovery);
         snapshot_.diagnostics.push_back(std::move(diagnostic));
         recovered_ = true;
      }
      return index;
   }

   void addReference(const antlr4::ParserRuleContext *context,EditorReferenceKind kind,
      std::string text,std::size_t declarationIndex)
   {
      if (context == nullptr || text.empty()) return;
      const SourceRange valueRange = ranges_.context(context);
      if (!valueRange.valid) return;
      const std::string sourceId = declarationIndex == noDeclaration
         ? std::string() : snapshot_.declarations[declarationIndex].id;
      const std::string key = std::string(referenceKind(kind)) + ":"
         + std::to_string(valueRange.start.byteOffset) + ":"
         + std::to_string(valueRange.end.byteOffset) + ":" + text + ":" + sourceId;
      if (!references_.insert(key).second) return;
      EditorReference reference;
      reference.text = std::move(text);
      reference.kind = kind;
      reference.range = valueRange;
      if (declarationIndex != noDeclaration) {
         reference.sourceId = sourceId;
         reference.hasSource = true;
      }
      snapshot_.references.push_back(std::move(reference));
   }

   void addImport(SyntaxImportReference reference)
   {
      if (!reference.model.empty()) snapshot_.imports.push_back(std::move(reference));
   }

   void appendSyntaxDiagnostics(const std::vector<Diagnostic> &diagnostics)
   {
      snapshot_.diagnostics.insert(snapshot_.diagnostics.end(),diagnostics.begin(),diagnostics.end());
      if (!diagnostics.empty()) recovered_ = true;
   }

   EditorSnapshot finish(bool parserComplete)
   {
      std::map<std::string,const EditorDeclaration *> previousDeclarations;
      for (const auto &declaration : snapshot_.declarations) {
         std::string key = declaration.hasContainer ? declaration.containerId : std::string();
         key.push_back(':');
         for (const char value : declaration.name)
            key.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(value))));
         const auto previous = previousDeclarations.find(key);
         if (previous != previousDeclarations.end()) {
            Diagnostic diagnostic;
            diagnostic.severity = DiagnosticSeverity::Error;
            diagnostic.code = "ILIC-LIVE-DUPLICATE";
            diagnostic.message = "Duplicate declaration '" + declaration.name + "'";
            diagnostic.range = declaration.selectionRange;
            diagnostic.relatedInformation.push_back({previous->second->selectionRange,"First declaration"});
            diagnostic.source = "live";
            diagnostic.phase = DiagnosticPhase::EditorRecovery;
            diagnostic.tags.push_back(DiagnosticTag::Recovery);
            snapshot_.diagnostics.push_back(std::move(diagnostic));
            recovered_ = true;
         }
         else previousDeclarations.emplace(std::move(key),&declaration);
      }
      std::sort(snapshot_.declarations.begin(),snapshot_.declarations.end(),[](const auto &left,const auto &right) {
         if (left.selectionRange.start.byteOffset != right.selectionRange.start.byteOffset)
            return left.selectionRange.start.byteOffset < right.selectionRange.start.byteOffset;
         if (left.selectionRange.end.byteOffset != right.selectionRange.end.byteOffset)
            return left.selectionRange.end.byteOffset < right.selectionRange.end.byteOffset;
         if (left.kind != right.kind) return static_cast<int>(left.kind) < static_cast<int>(right.kind);
         return left.name < right.name;
      });
      std::sort(snapshot_.references.begin(),snapshot_.references.end(),[](const auto &left,const auto &right) {
         if (left.range.start.byteOffset != right.range.start.byteOffset)
            return left.range.start.byteOffset < right.range.start.byteOffset;
         if (left.range.end.byteOffset != right.range.end.byteOffset)
            return left.range.end.byteOffset < right.range.end.byteOffset;
         return static_cast<int>(left.kind) < static_cast<int>(right.kind);
      });
      std::stable_sort(snapshot_.imports.begin(),snapshot_.imports.end(),[](const auto &left,const auto &right) {
         return left.range.start.byteOffset < right.range.start.byteOffset;
      });
      std::sort(snapshot_.contexts.begin(),snapshot_.contexts.end(),[](const auto &left,const auto &right) {
         return left.range.start.byteOffset < right.range.start.byteOffset;
      });
      sortDiagnostics(snapshot_.diagnostics);
      bool hasError = false;
      for (const auto &diagnostic : snapshot_.diagnostics)
         hasError = hasError || diagnostic.treatedAsError || diagnostic.severity == DiagnosticSeverity::Error;
      snapshot_.success = !hasError;
      snapshot_.recovered = recovered_ || hasError;
      snapshot_.complete = parserComplete && !snapshot_.recovered;
      return std::move(snapshot_);
   }

   EditorSnapshot &snapshot() noexcept { return snapshot_; }
   void markRecovered() noexcept { recovered_ = true; }

private:
   static bool sameName(const std::string &left,const std::string &right)
   {
      if (left.size() != right.size()) return false;
      for (std::size_t i = 0; i < left.size(); ++i)
         if (std::toupper(static_cast<unsigned char>(left[i]))
            != std::toupper(static_cast<unsigned char>(right[i]))) return false;
      return true;
   }

   static std::string containerRule(EditorSymbolKind kind)
   {
      return std::string(symbolKind(kind)) + "Def";
   }

   const SourceRangeMapper &ranges_;
   EditorSnapshot snapshot_;
   std::set<std::string> references_;
   bool recovered_ = false;
};

template<class Context>
std::string pathText(const Context *context)
{
   return context == nullptr ? std::string()
      : const_cast<Context *>(context)->getText();
}

struct EditorDeclarationMatch {
   EditorSymbolKind kind = EditorSymbolKind::Attribute;
   const antlr4::ParserRuleContext *context = nullptr;
   const antlr4::Token *name = nullptr;
   const antlr4::Token *endName = nullptr;
};

EditorDeclarationMatch ili2DeclarationMatch(antlr4::tree::ParseTree *tree)
{
   if (auto *context = dynamic_cast<parser::Ili2Parser::ModelDefContext *>(tree))
      return {EditorSymbolKind::Model,context,context->modelname1,context->modelname2};
   if (auto *context = dynamic_cast<parser::Ili2Parser::TopicDefContext *>(tree))
      return {EditorSymbolKind::Topic,context,context->topicname1,context->topicname2};
   if (auto *context = dynamic_cast<parser::Ili2Parser::ClassDefContext *>(tree))
      return {EditorSymbolKind::Class,context,context->classname1,context->classname2};
   if (auto *context = dynamic_cast<parser::Ili2Parser::StructureDefContext *>(tree))
      return {EditorSymbolKind::Structure,context,context->structurename1,context->structurename2};
   if (auto *context = dynamic_cast<parser::Ili2Parser::AssociationDefContext *>(tree))
      return {EditorSymbolKind::Association,context,context->associationname1,context->associationname2};
   if (auto *context = dynamic_cast<parser::Ili2Parser::ViewDefContext *>(tree))
      return {EditorSymbolKind::View,context,context->viewname1,context->viewname2};
   if (auto *context = dynamic_cast<parser::Ili2Parser::GraphicDefContext *>(tree))
      return {EditorSymbolKind::Graphic,context,context->graphicname1,context->graphicname2};
   if (auto *context = dynamic_cast<parser::Ili2Parser::DomainTypeContext *>(tree))
      return {EditorSymbolKind::Domain,context,context->domainname,nullptr};
   if (auto *context = dynamic_cast<parser::Ili2Parser::UnitDefContext *>(tree))
      return {EditorSymbolKind::Unit,context,context->unitname,nullptr};
   if (auto *context = dynamic_cast<parser::Ili2Parser::AttributeDefContext *>(tree))
      return {EditorSymbolKind::Attribute,context,context->attributname,nullptr};
   if (auto *context = dynamic_cast<parser::Ili2Parser::ViewAttributeContext *>(tree))
      if (context->attributeDef() == nullptr)
         return {EditorSymbolKind::Attribute,context,context->attributename,nullptr};
   return {};
}

EditorDeclarationMatch ili1DeclarationMatch(antlr4::tree::ParseTree *tree)
{
   if (auto *context = dynamic_cast<parser::Ili1Parser::ModelDefContext *>(tree))
      return {EditorSymbolKind::Model,context,context->modelname1,context->modelname2};
   if (auto *context = dynamic_cast<parser::Ili1Parser::TopicDefContext *>(tree))
      return {EditorSymbolKind::Topic,context,context->topicname1,context->topicname2};
   if (auto *context = dynamic_cast<parser::Ili1Parser::TableDefContext *>(tree))
      return {EditorSymbolKind::Class,context,context->tablename1,context->tablename2};
   if (auto *context = dynamic_cast<parser::Ili1Parser::DomainDefContext *>(tree))
      return {EditorSymbolKind::Domain,context,context->domainname,nullptr};
   if (auto *context = dynamic_cast<parser::Ili1Parser::AttributeContext *>(tree))
      return {EditorSymbolKind::Attribute,context,context->attributename,nullptr};
   if (auto *context = dynamic_cast<parser::Ili1Parser::ViewContext *>(tree))
      return {EditorSymbolKind::View,context,context->modelname1,context->modelname2};
   return {};
}

EditorReferenceKind ili2ReferenceKind(const parser::Ili2Parser::PathContext *path)
{
   for (auto *parent = path == nullptr ? nullptr : path->parent; parent != nullptr; parent = parent->parent) {
      if (dynamic_cast<parser::Ili2Parser::BagOrListTypeContext *>(parent) != nullptr)
         return EditorReferenceKind::Collection;
      if (dynamic_cast<parser::Ili2Parser::ReferenceAttrContext *>(parent) != nullptr
         || dynamic_cast<parser::Ili2Parser::RoleDefContext *>(parent) != nullptr)
         return EditorReferenceKind::Reference;
      if (dynamic_cast<parser::Ili2Parser::UnitDefContext *>(parent) != nullptr
         || dynamic_cast<parser::Ili2Parser::DerivedUnitContext *>(parent) != nullptr
         || dynamic_cast<parser::Ili2Parser::ComposedUnitContext *>(parent) != nullptr)
         return EditorReferenceKind::Unit;
      if (auto *context = dynamic_cast<parser::Ili2Parser::ClassDefContext *>(parent))
         if (context->classbase == path) return EditorReferenceKind::Extends;
      if (auto *context = dynamic_cast<parser::Ili2Parser::StructureDefContext *>(parent))
         if (context->structurebase == path) return EditorReferenceKind::Extends;
      if (auto *context = dynamic_cast<parser::Ili2Parser::TopicDefContext *>(parent))
         if (context->topicbase == path) return EditorReferenceKind::Extends;
      if (auto *context = dynamic_cast<parser::Ili2Parser::ViewDefContext *>(parent))
         if (context->viewref == path) return EditorReferenceKind::Extends;
      if (auto *context = dynamic_cast<parser::Ili2Parser::GraphicDefContext *>(parent))
         if (context->expath == path) return EditorReferenceKind::Extends;
      if (auto *context = dynamic_cast<parser::Ili2Parser::DomainTypeContext *>(parent))
         if (context->basedomain == path) return EditorReferenceKind::Extends;
   }
   return EditorReferenceKind::Type;
}

void collectIli2Editor(EditorSnapshotAccumulator &output,const SourceRangeMapper &ranges,
   antlr4::tree::ParseTree *tree,std::size_t containerIndex,std::size_t declarationIndex)
{
   if (tree == nullptr) return;
   const EditorDeclarationMatch match = ili2DeclarationMatch(tree);
   std::size_t currentDeclaration = declarationIndex;
   std::size_t currentContainer = containerIndex;
   if (match.name != nullptr) {
      currentDeclaration = output.addDeclaration(match.context,match.kind,match.name,
         match.endName,containerIndex);
      if (currentDeclaration != EditorSnapshotAccumulator::noDeclaration
         && isContainer(match.kind)) currentContainer = currentDeclaration;
   }
   if (auto *importing = dynamic_cast<parser::Ili2Parser::ImportingContext *>(tree)) {
      const antlr4::Token *name = nullptr;
      if (auto *node = importing->INTERLIS()) name = node->getSymbol();
      if (name == nullptr) if (auto *node = importing->NAME()) name = node->getSymbol();
      if (name != nullptr) output.addImport({name->getText(),importing->UNQUALIFIED() != nullptr,
         ranges.token(name)});
      else {
         for (auto *child : tree->children) {
            auto *path = dynamic_cast<parser::Ili2Parser::PathContext *>(child);
            if (path == nullptr) continue;
            output.addImport({pathText(path),importing->UNQUALIFIED() != nullptr,
               ranges.context(path)});
         }
      }
   }
   if (auto *path = dynamic_cast<parser::Ili2Parser::PathContext *>(tree)) {
      bool imported = false;
      for (auto *parent = path->parent; parent != nullptr; parent = parent->parent)
         if (dynamic_cast<parser::Ili2Parser::ImportingContext *>(parent) != nullptr) imported = true;
      if (!imported)
         output.addReference(path,ili2ReferenceKind(path),pathText(path),currentDeclaration);
   }
   for (auto *child : tree->children)
      collectIli2Editor(output,ranges,child,currentContainer,currentDeclaration);
}

void collectIli1Editor(EditorSnapshotAccumulator &output,
   antlr4::tree::ParseTree *tree,std::size_t containerIndex,std::size_t declarationIndex)
{
   if (tree == nullptr) return;
   const EditorDeclarationMatch match = ili1DeclarationMatch(tree);
   std::size_t currentDeclaration = declarationIndex;
   std::size_t currentContainer = containerIndex;
   if (match.name != nullptr) {
      currentDeclaration = output.addDeclaration(match.context,match.kind,match.name,
         match.endName,containerIndex);
      if (currentDeclaration != EditorSnapshotAccumulator::noDeclaration
         && isContainer(match.kind)) currentContainer = currentDeclaration;
   }
   if (auto *type = dynamic_cast<parser::Ili1Parser::TypeContext *>(tree)) {
      if (type->name != nullptr)
         output.addReference(type,EditorReferenceKind::Type,type->name->getText(),currentDeclaration);
   }
   if (auto *attribute = dynamic_cast<parser::Ili1Parser::AttributeContext *>(tree)) {
      if (attribute->tablename != nullptr)
         output.addReference(attribute,EditorReferenceKind::Reference,
            attribute->tablename->getText(),currentDeclaration);
   }
   for (auto *child : tree->children)
      collectIli1Editor(output,child,currentContainer,currentDeclaration);
}

template<class Parser,class Lexer,class TokenStream,class Root>
class ParsedSourceArtifactImpl final : public ParsedSourceArtifact {
public:
   using VisitFunction = void (*)(const SourceBuffer &,Root *,
      metamodel::MetaModelBuilder &,util::Logger &);
   using HeaderFunction = ParsedSourceHeader (*)(Root *);

   ParsedSourceArtifactImpl(SourceBuffer source,
      std::unique_ptr<antlr4::ANTLRInputStream> input,
      std::unique_ptr<Lexer> lexer,std::unique_ptr<TokenStream> tokens,
      std::unique_ptr<Parser> parser,Root *root,bool valid,VisitFunction visit,
      std::vector<Diagnostic> parserDiagnostics,ParseMode mode)
      : source_(std::move(source)),input_(std::move(input)),lexer_(std::move(lexer)),
        tokens_(std::move(tokens)),parser_(std::move(parser)),root_(root),valid_(valid),
        visit_(visit),parserDiagnostics_(std::move(parserDiagnostics)),mode_(mode) {}

   void setHeader(HeaderFunction header) noexcept { header_ = header; }

   ParsedSourceHeader header() const override
   {
      return header_ == nullptr ? ParsedSourceHeader{} : header_(root_);
   }

   const std::vector<Diagnostic> &parserDiagnostics() const noexcept override
   {
      return parserDiagnostics_;
   }

   ParseMode mode() const noexcept override { return mode_; }

   bool supportsMetaModelBuild() const noexcept override
   {
      return mode_ == ParseMode::StrictCompiler;
   }

   std::size_t tokenCount() const noexcept override { return tokenCount_; }

   std::size_t parseTreeNodeCount() const noexcept override { return parseTreeNodeCount_; }

   std::size_t estimatedRetainedBytes() const noexcept override
   {
      std::size_t bytes = sizeof(*this) + source_.text.capacity();
      bytes += tokenCount_ * (sizeof(void *) * 8);
      bytes += parseTreeNodeCount_ * (sizeof(void *) * 6);
      for (const auto &diagnostic : parserDiagnostics_) {
         bytes += sizeof(Diagnostic) + diagnostic.code.capacity()
            + diagnostic.message.capacity() + diagnostic.source.capacity();
         bytes += diagnostic.relatedInformation.capacity() * sizeof(RelatedInformation);
         bytes += diagnostic.notes.capacity() * sizeof(std::string);
      }
      return bytes;
   }

   void reportParserDiagnostics(util::Logger &logger) const override
   {
      for (const auto &diagnostic : parserDiagnostics_)
         logger.error(diagnostic.message,diagnostic.range,diagnostic.code,
            diagnostic.relatedInformation,diagnostic.notes);
   }

   void buildMetaModel(metamodel::MetaModelBuilder &builder,
      util::Logger &logger) const override
   {
      if (supportsMetaModelBuild() && valid_ && root_ != nullptr && visit_ != nullptr)
         visit_(source_,root_,builder,logger);
   }

   void setProjectionCounts(std::size_t tokenCount,std::size_t parseTreeNodeCount) noexcept
   {
      tokenCount_ = tokenCount;
      parseTreeNodeCount_ = parseTreeNodeCount;
   }

   Parser *parser() noexcept { return parser_.get(); }
   Lexer *lexer() noexcept { return lexer_.get(); }
   TokenStream *tokens() noexcept { return tokens_.get(); }

private:
   SourceBuffer source_;
   std::unique_ptr<antlr4::ANTLRInputStream> input_;
   std::unique_ptr<Lexer> lexer_;
   std::unique_ptr<TokenStream> tokens_;
   std::unique_ptr<Parser> parser_;
   Root *root_ = nullptr;
   bool valid_ = false;
   VisitFunction visit_ = nullptr;
   HeaderFunction header_ = nullptr;
   std::vector<Diagnostic> parserDiagnostics_;
   ParseMode mode_ = ParseMode::StrictCompiler;
   std::size_t tokenCount_ = 0;
   std::size_t parseTreeNodeCount_ = 0;
};

ParsedSourceHeader ili2Header(parser::Ili2Parser::Interlis2DefContext *root)
{
   ParsedSourceHeader header;
   if (root == nullptr) return header;
   if (root->iliversion != nullptr) header.iliVersion = root->iliversion->getText();
   for (auto *model : root->modelDef()) {
      if (model == nullptr) continue;
      if (model->modelname1 != nullptr) header.models.push_back(model->modelname1->getText());
      if (model->translationOf != nullptr)
         header.requiredModels.push_back(model->translationOf->getText());
      for (auto *definition : model->importDef()) {
         if (definition == nullptr) continue;
         for (auto *importing : definition->importing()) {
            if (importing == nullptr) continue;
            auto *name = importing->INTERLIS();
            if (name == nullptr) name = importing->NAME();
            if (name != nullptr) {
               header.imports.push_back(name->getText());
               header.requiredModels.push_back(name->getText());
            }
         }
      }
   }
   return header;
}

ParsedSourceHeader ili1Header(parser::Ili1Parser::Interlis1DefContext *root)
{
   ParsedSourceHeader header;
   header.iliVersion = "1.0";
   if (root != nullptr && root->modelDef() != nullptr
      && root->modelDef()->modelname1 != nullptr)
      header.models.push_back(root->modelDef()->modelname1->getText());
   return header;
}

std::size_t countParseTreeNodes(const antlr4::tree::ParseTree *tree)
{
   if (tree == nullptr) return 0;
   std::size_t count = 1;
   for (const auto *child : tree->children) count += countParseTreeNodes(child);
   return count;
}

template<class Parser,class Lexer,class Root,class TokenStream>
SnapshotBundle buildParsed(const SourceBuffer &source,ParseMode mode,Root *root,
   Parser &parser,Lexer &lexer,antlr4::CommonTokenStream &tokens,
   const SourceRangeMapper &ranges,SnapshotErrorCollector &lexerErrors,
   SnapshotErrorCollector &parserErrors,ParsedSourceArtifactPtr artifact)
{
   SnapshotBundle bundle;
   bundle.mode = mode;
   bundle.artifact = std::move(artifact);
   bundle.syntax.uri = source.uri;
   bundle.syntax.documentVersion = source.version;
   bundle.syntax.iliVersion = "unknown";
   if (root != nullptr && root->getStart() != nullptr) {
      if constexpr (std::is_same_v<Root,parser::Ili2Parser::Interlis2DefContext>) {
         if (root->iliversion != nullptr) bundle.syntax.iliVersion = root->iliversion->getText();
      }
      else bundle.syntax.iliVersion = "1.0";
   }
   if constexpr (std::is_same_v<Root,parser::Ili2Parser::Interlis2DefContext>) {
      auto *editorRoot = root;
      if (editorRoot != nullptr)
         for (auto *model : editorRoot->modelDef())
            for (auto *definition : model->importDef())
               for (auto *importing : definition->importing()) {
                  antlr4::Token *name = nullptr;
                  if (auto *node = importing->INTERLIS()) name = node->getSymbol();
                  if (name == nullptr) if (auto *node = importing->NAME()) name = node->getSymbol();
                  if (name == nullptr) continue;
                  const SourceRange valueRange = ranges.token(name);
                  bundle.syntax.imports.push_back(name->getText());
                  bundle.syntax.importReferences.push_back({name->getText(),
                     importing->UNQUALIFIED() != nullptr,valueRange});
               }
   }
   // Both public projections are materialized from this one parser tree. The
   // caller may discard either projection, but must not cause a second parser
   // invocation merely because it asks for syntax and editor data in a
   // different order.
   appendSyntaxTokens(tokens,lexer,ranges,bundle.syntax);
   if (root != nullptr) appendSyntaxNodes(root,parser,ranges,bundle.syntax,0,false);
   bundle.syntax.diagnostics = std::move(lexerErrors.diagnostics);
   bundle.syntax.diagnostics.insert(bundle.syntax.diagnostics.end(),parserErrors.diagnostics.begin(),parserErrors.diagnostics.end());
   sortDiagnostics(bundle.syntax.diagnostics);
   bundle.syntax.success = bundle.syntax.diagnostics.empty();
   {
      EditorSnapshotAccumulator editor(ranges,bundle.syntax.iliVersion);
      if constexpr (std::is_same_v<Root,parser::Ili2Parser::Interlis2DefContext>) {
         collectIli2Editor(editor,ranges,root,EditorSnapshotAccumulator::noDeclaration,
            EditorSnapshotAccumulator::noDeclaration);
      }
      else {
         collectIli1Editor(editor,root,EditorSnapshotAccumulator::noDeclaration,
            EditorSnapshotAccumulator::noDeclaration);
      }
      if (const auto *editorTokens = dynamic_cast<const EditorTokenStream *>(&tokens);
         editorTokens != nullptr && editorTokens->recovered())
         editor.markRecovered();
      editor.appendSyntaxDiagnostics(bundle.syntax.diagnostics);
      bundle.editor = editor.finish(bundle.syntax.success);
      // The editor contract historically treated UNQUALIFIED as applying to
      // the complete comma-separated import clause. Keep that projection
      // stable while SyntaxSnapshot preserves the grammar's per-import flag.
      bool importClauseUnqualified = false;
      for (const auto &reference : bundle.editor.imports)
         importClauseUnqualified = importClauseUnqualified || reference.unqualified;
      if (importClauseUnqualified)
         for (auto &reference : bundle.editor.imports) reference.unqualified = true;
   }
   auto concrete = std::dynamic_pointer_cast<const ParsedSourceArtifactImpl<Parser,Lexer,
      TokenStream,Root>>(bundle.artifact);
   if (concrete != nullptr)
      const_cast<ParsedSourceArtifactImpl<Parser,Lexer,TokenStream,Root> *>(concrete.get())
         ->setProjectionCounts(bundle.syntax.tokens.size(),countParseTreeNodes(root));
   return bundle;
}

template<class TokenStream>
SnapshotBundle buildIli2Impl(const SourceBuffer &source,ParseMode mode,bool recover)
{
   SourceRangeMapper ranges(source);
   auto input = std::make_unique<antlr4::ANTLRInputStream>(ranges.normalizedUtf8());
   auto lexer = std::make_unique<lexer::Ili2Lexer>(input.get());
   SnapshotErrorCollector lexerErrors(ranges,"compiler");
   lexer->removeErrorListeners();
   lexer->addErrorListener(&lexerErrors);
   auto tokens = std::make_unique<TokenStream>(lexer.get());
   if (recover) {
      if constexpr (std::is_same_v<TokenStream,EditorTokenStream>) tokens->recoverBareModelHeaders();
   }
   auto parser = std::make_unique<parser::Ili2Parser>(tokens.get());
   SnapshotErrorCollector parserErrors(ranges,"compiler");
   parser->removeErrorListeners();
   parser->addErrorListener(&parserErrors);
   auto *root = parser->interlis2Def();
   using Artifact = ParsedSourceArtifactImpl<parser::Ili2Parser,lexer::Ili2Lexer,
      TokenStream,parser::Ili2Parser::Interlis2DefContext>;
   auto artifact = std::make_shared<Artifact>(source,std::move(input),std::move(lexer),
      std::move(tokens),std::move(parser),root,parserErrors.diagnostics.empty()
         && lexerErrors.diagnostics.empty(),input::visitIli2,
      [&]() {
         std::vector<Diagnostic> diagnostics = lexerErrors.diagnostics;
         diagnostics.insert(diagnostics.end(),parserErrors.diagnostics.begin(),
            parserErrors.diagnostics.end());
         return diagnostics;
      }(),mode);
   artifact->setHeader(ili2Header);
   auto *artifactParser = artifact->parser();
   auto *artifactLexer = artifact->lexer();
   auto *artifactTokens = artifact->tokens();
   return buildParsed<parser::Ili2Parser,lexer::Ili2Lexer,
      parser::Ili2Parser::Interlis2DefContext,TokenStream>(source,mode,root,*artifactParser,*artifactLexer,
      *artifactTokens,ranges,lexerErrors,parserErrors,std::move(artifact));
}

SnapshotBundle buildIli2(const SourceBuffer &source,ParseMode mode)
{
   return mode == ParseMode::TolerantEditor
      ? buildIli2Impl<EditorTokenStream>(source,mode,true)
      : buildIli2Impl<antlr4::CommonTokenStream>(source,mode,false);
}

SnapshotBundle buildIli1(const SourceBuffer &source,ParseMode mode)
{
   SourceRangeMapper ranges(source);
   auto input = std::make_unique<antlr4::ANTLRInputStream>(ranges.normalizedUtf8());
   auto lexer = std::make_unique<lexer::Ili1Lexer>(input.get());
   SnapshotErrorCollector lexerErrors(ranges,"compiler");
   lexer->removeErrorListeners();
   lexer->addErrorListener(&lexerErrors);
   auto tokens = std::make_unique<antlr4::CommonTokenStream>(lexer.get());
   auto parser = std::make_unique<parser::Ili1Parser>(tokens.get());
   SnapshotErrorCollector parserErrors(ranges,"compiler");
   parser->removeErrorListeners();
   parser->addErrorListener(&parserErrors);
   auto *root = parser->interlis1Def();
   using Artifact = ParsedSourceArtifactImpl<parser::Ili1Parser,lexer::Ili1Lexer,
      antlr4::CommonTokenStream,parser::Ili1Parser::Interlis1DefContext>;
   auto artifact = std::make_shared<Artifact>(source,std::move(input),std::move(lexer),
      std::move(tokens),std::move(parser),root,parserErrors.diagnostics.empty()
         && lexerErrors.diagnostics.empty(),input::visitIli1,
      [&]() {
         std::vector<Diagnostic> diagnostics = lexerErrors.diagnostics;
         diagnostics.insert(diagnostics.end(),parserErrors.diagnostics.begin(),
            parserErrors.diagnostics.end());
         return diagnostics;
      }(),mode);
   artifact->setHeader(ili1Header);
   auto *artifactParser = artifact->parser();
   auto *artifactLexer = artifact->lexer();
   auto *artifactTokens = artifact->tokens();
   return buildParsed<parser::Ili1Parser,lexer::Ili1Lexer,
      parser::Ili1Parser::Interlis1DefContext,antlr4::CommonTokenStream>(source,mode,root,*artifactParser,*artifactLexer,
      *artifactTokens,ranges,lexerErrors,parserErrors,std::move(artifact));
}

EditorSnapshot missingEditor(const std::string &uri)
{
   EditorSnapshot result;
   result.uri = uri;
   Diagnostic diagnostic;
   diagnostic.severity = DiagnosticSeverity::Error;
   diagnostic.code = "ILIC-SOURCE-NOT-FOUND";
   diagnostic.message = "editor source is not registered";
   result.diagnostics.push_back(std::move(diagnostic));
   return result;
}

SyntaxSnapshot missingSyntax(const std::string &uri)
{
   SyntaxSnapshot result;
   result.uri = uri;
   Diagnostic diagnostic;
   diagnostic.severity = DiagnosticSeverity::Error;
   diagnostic.code = "ILIC-SOURCE-NOT-FOUND";
   diagnostic.message = "syntax source is not registered";
   result.diagnostics.push_back(std::move(diagnostic));
   return result;
}

} // namespace

SnapshotBundle SnapshotPipeline::build(const std::string &uri,ParseMode mode) const
{
   const SourceBuffer *source = sources_.get(uri);
   if (source == nullptr) {
      SnapshotBundle missing;
      missing.mode = mode;
      missing.syntax = missingSyntax(uri);
      if (mode == ParseMode::TolerantEditor) missing.editor = missingEditor(uri);
      return missing;
   }
   return build(*source,mode);
}

SnapshotBundle SnapshotPipeline::build(const SourceBuffer &source,ParseMode mode) const
{
   try {
      return detectLanguage(source) == DetectedLanguage::Ili1
         ? buildIli1(source,mode) : buildIli2(source,mode);
   }
   catch (const std::exception &error) {
      SnapshotBundle failure;
      failure.mode = mode;
      failure.syntax.uri = source.uri;
      failure.syntax.documentVersion = source.version;
      failure.syntax.iliVersion = "unknown";
      Diagnostic diagnostic;
      diagnostic.severity = DiagnosticSeverity::Error;
      diagnostic.code = "ILIC-SYNTAX";
      diagnostic.message = std::string("snapshot parser failure: ") + error.what();
      diagnostic.range = SourceRangeMapper(source).eof();
      diagnostic.source = "compiler";
      failure.syntax.diagnostics.push_back(std::move(diagnostic));
      if (mode == ParseMode::TolerantEditor) {
         failure.editor.uri = source.uri;
         failure.editor.documentVersion = source.version;
         failure.editor.iliVersion = "unknown";
         failure.editor.diagnostics = failure.syntax.diagnostics;
         failure.editor.recovered = true;
      }
      return failure;
   }
   catch (...) {
      SnapshotBundle failure;
      failure.mode = mode;
      failure.syntax.uri = source.uri;
      failure.syntax.documentVersion = source.version;
      failure.syntax.iliVersion = "unknown";
      Diagnostic diagnostic;
      diagnostic.severity = DiagnosticSeverity::Error;
      diagnostic.code = "ILIC-SYNTAX";
      diagnostic.message = "snapshot parser failure: unknown exception";
      diagnostic.range = SourceRangeMapper(source).eof();
      diagnostic.source = "compiler";
      failure.syntax.diagnostics.push_back(std::move(diagnostic));
      if (mode == ParseMode::TolerantEditor) {
         failure.editor.uri = source.uri;
         failure.editor.documentVersion = source.version;
         failure.editor.iliVersion = "unknown";
         failure.editor.diagnostics = failure.syntax.diagnostics;
         failure.editor.recovered = true;
      }
      return failure;
   }
}

SyntaxSnapshot SnapshotPipeline::syntax(const std::string &uri) const
{
   return build(uri,ParseMode::StrictCompiler).syntax;
}

EditorSnapshot SnapshotPipeline::editor(const std::string &uri) const
{
   return build(uri,ParseMode::TolerantEditor).editor;
}

} // namespace ilic::detail
