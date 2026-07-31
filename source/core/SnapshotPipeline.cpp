#include "SnapshotPipeline.h"

#include "SourceRangeMapper.h"

#include "../../include/ilic/Compiler.h"
#include "../input/parser/generated/Ili1Lexer.h"
#include "../input/parser/generated/Ili1Parser.h"
#include "../input/parser/generated/Ili2Lexer.h"
#include "../input/parser/generated/Ili2Parser.h"

#include "BaseErrorListener.h"
#include "CommonTokenStream.h"
#include "ParserRuleContext.h"
#include "tree/ParseTree.h"
#include "tree/TerminalNode.h"

#include <algorithm>
#include <cctype>
#include <exception>
#include <map>
#include <set>
#include <string_view>
#include <type_traits>
#include <utility>

namespace ilic::detail {
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
      diagnostic.code = "ILIC-SYNTAX";
      diagnostic.message = message;
      diagnostic.range = offendingSymbol == nullptr ? ranges_.eof() : ranges_.token(offendingSymbol);
      if (!diagnostic.range.valid) diagnostic.range = ranges_.eof();
      diagnostic.source = source_;
      diagnostics.push_back(std::move(diagnostic));
   }

   std::vector<Diagnostic> diagnostics;

private:
   const SourceRangeMapper &ranges_;
   const char *source_;
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
   EditorSnapshotAccumulator(const SourceRangeMapper &ranges,std::string iliVersion)
      : ranges_(ranges)
   {
      snapshot_.uri = ranges.source().uri;
      snapshot_.documentVersion = ranges.source().version;
      snapshot_.iliVersion = std::move(iliVersion);
   }

   void addDeclaration(const antlr4::ParserRuleContext *context,EditorSymbolKind kind,
      const antlr4::Token *name,const antlr4::Token *endName)
   {
      if (context == nullptr || name == nullptr || name->getStartIndex() == INVALID_INDEX) return;
      const SourceRange selection = ranges_.token(name);
      if (!selection.valid) return;
      const EditorDeclaration *owner = nearestContainer(context->parent);
      EditorDeclaration declaration;
      declaration.name = name->getText();
      declaration.kind = kind;
      declaration.id = std::string(symbolKind(kind)) + ":"
         + std::to_string(selection.start.byteOffset) + ":" + declaration.name;
      declaration.range = ranges_.context(context);
      declaration.selectionRange = selection;
      if (endName != nullptr && endName->getStartIndex() != INVALID_INDEX)
         declaration.endRange = ranges_.token(endName);
      if (owner != nullptr) {
         declaration.qualifiedName = owner->qualifiedName + "." + declaration.name;
         if (isContainer(owner->kind)) {
            declaration.containerId = owner->id;
            declaration.hasContainer = true;
         }
      }
      if (declaration.qualifiedName.empty()) declaration.qualifiedName = declaration.name;
      const std::size_t index = snapshot_.declarations.size();
      snapshot_.declarations.push_back(std::move(declaration));
      declarationByContext_[context] = index;
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
         snapshot_.diagnostics.push_back(std::move(diagnostic));
         recovered_ = true;
      }
   }

   void addReference(const antlr4::ParserRuleContext *context,EditorReferenceKind kind,
      std::string text)
   {
      if (context == nullptr || text.empty()) return;
      const SourceRange valueRange = ranges_.context(context);
      if (!valueRange.valid) return;
      const EditorDeclaration *owner = nearestDeclaration(context->parent);
      const std::string sourceId = owner == nullptr ? std::string() : owner->id;
      const std::string key = std::string(referenceKind(kind)) + ":"
         + std::to_string(valueRange.start.byteOffset) + ":"
         + std::to_string(valueRange.end.byteOffset) + ":" + text + ":" + sourceId;
      if (!references_.insert(key).second) return;
      EditorReference reference;
      reference.text = std::move(text);
      reference.kind = kind;
      reference.range = valueRange;
      if (owner != nullptr) {
         reference.sourceId = owner->id;
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

   const EditorDeclaration *nearestDeclaration(const antlr4::tree::ParseTree *node) const
   {
      for (auto *current = node; current != nullptr;) {
         auto *context = dynamic_cast<const antlr4::ParserRuleContext *>(current);
         if (context != nullptr) {
            auto found = declarationByContext_.find(context);
            if (found != declarationByContext_.end()) return &snapshot_.declarations[found->second];
         }
         current = current->parent;
      }
      return nullptr;
   }

   const EditorDeclaration *nearestContainer(const antlr4::tree::ParseTree *node) const
   {
      const EditorDeclaration *declaration = nearestDeclaration(node);
      return declaration != nullptr && isContainer(declaration->kind) ? declaration : nullptr;
   }

   EditorSnapshot &snapshot() noexcept { return snapshot_; }

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
   std::map<const antlr4::ParserRuleContext *,std::size_t> declarationByContext_;
   std::set<std::string> references_;
   bool recovered_ = false;
};

template<class Context>
std::string pathText(const Context *context)
{
   return context == nullptr ? std::string()
      : const_cast<Context *>(context)->getText();
}

void collectIli2Declarations(EditorSnapshotAccumulator &output,antlr4::tree::ParseTree *tree)
{
   if (tree == nullptr) return;
   if (auto *context = dynamic_cast<parser::Ili2Parser::ModelDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Model,context->modelname1,context->modelname2);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::TopicDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Topic,context->topicname1,context->topicname2);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::ClassDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Class,context->classname1,context->classname2);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::StructureDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Structure,context->structurename1,context->structurename2);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::AssociationDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Association,context->associationname1,context->associationname2);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::ViewDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::View,context->viewname1,context->viewname2);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::GraphicDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Graphic,context->graphicname1,context->graphicname2);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::DomainTypeContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Domain,context->domainname,nullptr);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::UnitDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Unit,context->unitname,nullptr);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::AttributeDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Attribute,context->attributname,nullptr);
   else if (auto *context = dynamic_cast<parser::Ili2Parser::ViewAttributeContext *>(tree)) {
      if (context->attributeDef() == nullptr)
         output.addDeclaration(context,EditorSymbolKind::Attribute,context->attributename,nullptr);
   }
   for (auto *child : tree->children) collectIli2Declarations(output,child);
}

void collectIli1Declarations(EditorSnapshotAccumulator &output,antlr4::tree::ParseTree *tree)
{
   if (tree == nullptr) return;
   if (auto *context = dynamic_cast<parser::Ili1Parser::ModelDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Model,context->modelname1,context->modelname2);
   else if (auto *context = dynamic_cast<parser::Ili1Parser::TopicDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Topic,context->topicname1,context->topicname2);
   else if (auto *context = dynamic_cast<parser::Ili1Parser::TableDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Class,context->tablename1,context->tablename2);
   else if (auto *context = dynamic_cast<parser::Ili1Parser::DomainDefContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Domain,context->domainname,nullptr);
   else if (auto *context = dynamic_cast<parser::Ili1Parser::AttributeContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::Attribute,context->attributename,nullptr);
   else if (auto *context = dynamic_cast<parser::Ili1Parser::ViewContext *>(tree))
      output.addDeclaration(context,EditorSymbolKind::View,context->modelname1,context->modelname2);
   for (auto *child : tree->children) collectIli1Declarations(output,child);
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

void collectIli2ReferencesAndImports(EditorSnapshotAccumulator &output,antlr4::tree::ParseTree *tree)
{
   if (tree == nullptr) return;
   if (auto *importing = dynamic_cast<parser::Ili2Parser::ImportingContext *>(tree)) {
      const antlr4::Token *name = nullptr;
      if (auto *node = importing->INTERLIS()) name = node->getSymbol();
      if (name == nullptr) if (auto *node = importing->NAME()) name = node->getSymbol();
      if (name != nullptr) output.addImport({name->getText(),importing->UNQUALIFIED() != nullptr,
         SourceRange{}});
      else {
         for (auto *child : tree->children) {
            auto *path = dynamic_cast<parser::Ili2Parser::PathContext *>(child);
            if (path == nullptr) continue;
            output.addImport({pathText(path),importing->UNQUALIFIED() != nullptr,SourceRange{}});
         }
      }
   }
   if (auto *path = dynamic_cast<parser::Ili2Parser::PathContext *>(tree)) {
      bool imported = false;
      for (auto *parent = path->parent; parent != nullptr; parent = parent->parent)
         if (dynamic_cast<parser::Ili2Parser::ImportingContext *>(parent) != nullptr) imported = true;
      if (!imported) output.addReference(path,ili2ReferenceKind(path),pathText(path));
   }
   for (auto *child : tree->children) collectIli2ReferencesAndImports(output,child);
}

void collectIli1References(EditorSnapshotAccumulator &output,antlr4::tree::ParseTree *tree)
{
   if (tree == nullptr) return;
   if (auto *type = dynamic_cast<parser::Ili1Parser::TypeContext *>(tree)) {
      if (type->name != nullptr) output.addReference(type,EditorReferenceKind::Type,type->name->getText());
   }
   if (auto *attribute = dynamic_cast<parser::Ili1Parser::AttributeContext *>(tree)) {
      if (attribute->tablename != nullptr)
         output.addReference(attribute,EditorReferenceKind::Reference,attribute->tablename->getText());
   }
   for (auto *child : tree->children) collectIli1References(output,child);
}

template<class Parser,class Lexer,class Root>
SnapshotBundle buildParsed(const SourceBuffer &source,bool includeEditor,Root *root,
   Parser &parser,Lexer &lexer,antlr4::CommonTokenStream &tokens,
   const SourceRangeMapper &ranges,SnapshotErrorCollector &lexerErrors,
   SnapshotErrorCollector &parserErrors)
{
   SnapshotBundle bundle;
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
   // The editor product needs only the parser tree and diagnostics. Avoid
   // materializing the much larger token/node syntax product on this path;
   // both products still come from the same lexer, token stream and parser.
   if (!includeEditor) {
      appendSyntaxTokens(tokens,lexer,ranges,bundle.syntax);
      if (root != nullptr) appendSyntaxNodes(root,parser,ranges,bundle.syntax,0,false);
   }
   bundle.syntax.diagnostics = std::move(lexerErrors.diagnostics);
   bundle.syntax.diagnostics.insert(bundle.syntax.diagnostics.end(),parserErrors.diagnostics.begin(),parserErrors.diagnostics.end());
   sortDiagnostics(bundle.syntax.diagnostics);
   bundle.syntax.success = bundle.syntax.diagnostics.empty();
   if (includeEditor) {
      EditorSnapshotAccumulator editor(ranges,bundle.syntax.iliVersion);
      if constexpr (std::is_same_v<Root,parser::Ili2Parser::Interlis2DefContext>) {
         collectIli2Declarations(editor,root);
         collectIli2ReferencesAndImports(editor,root);
      }
      else {
         collectIli1Declarations(editor,root);
         collectIli1References(editor,root);
      }
      editor.appendSyntaxDiagnostics(bundle.syntax.diagnostics);
      bundle.editor = editor.finish(bundle.syntax.success);
      for (auto &reference : bundle.editor.imports) {
         if (!reference.range.valid) {
            for (const auto &syntaxImport : bundle.syntax.importReferences)
               if (syntaxImport.model == reference.model && syntaxImport.unqualified == reference.unqualified) {
                  reference.range = syntaxImport.range;
                  break;
               }
         }
      }
      // The collector uses parse-tree ranges for imports; make sure the public
      // import contract is identical to the syntax product.
      bundle.editor.imports = bundle.syntax.importReferences;
      // The editor contract historically treated UNQUALIFIED as applying to
      // the complete comma-separated import clause. Keep that projection
      // stable while SyntaxSnapshot preserves the grammar's per-import flag.
      bool importClauseUnqualified = false;
      for (const auto &reference : bundle.editor.imports)
         importClauseUnqualified = importClauseUnqualified || reference.unqualified;
      if (importClauseUnqualified)
         for (auto &reference : bundle.editor.imports) reference.unqualified = true;
   }
   return bundle;
}

SnapshotBundle buildIli2(const SourceBuffer &source,bool includeEditor)
{
   SourceRangeMapper ranges(source);
   antlr4::ANTLRInputStream input(ranges.normalizedUtf8());
   lexer::Ili2Lexer lexer(&input);
   SnapshotErrorCollector lexerErrors(ranges,"compiler");
   lexer.removeErrorListeners();
   lexer.addErrorListener(&lexerErrors);
   antlr4::CommonTokenStream tokens(&lexer);
   parser::Ili2Parser parser(&tokens);
   SnapshotErrorCollector parserErrors(ranges,"compiler");
   parser.removeErrorListeners();
   parser.addErrorListener(&parserErrors);
   auto *root = parser.interlis2Def();
   return buildParsed(source,includeEditor,root,parser,lexer,tokens,ranges,lexerErrors,parserErrors);
}

SnapshotBundle buildIli1(const SourceBuffer &source,bool includeEditor)
{
   SourceRangeMapper ranges(source);
   antlr4::ANTLRInputStream input(ranges.normalizedUtf8());
   lexer::Ili1Lexer lexer(&input);
   SnapshotErrorCollector lexerErrors(ranges,"compiler");
   lexer.removeErrorListeners();
   lexer.addErrorListener(&lexerErrors);
   antlr4::CommonTokenStream tokens(&lexer);
   parser::Ili1Parser parser(&tokens);
   SnapshotErrorCollector parserErrors(ranges,"compiler");
   parser.removeErrorListeners();
   parser.addErrorListener(&parserErrors);
   auto *root = parser.interlis1Def();
   return buildParsed(source,includeEditor,root,parser,lexer,tokens,ranges,lexerErrors,parserErrors);
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

SnapshotBundle SnapshotPipeline::build(const std::string &uri,bool includeEditor) const
{
   const SourceBuffer *source = sources_.get(uri);
   if (source == nullptr) {
      SnapshotBundle missing;
      missing.syntax = missingSyntax(uri);
      if (includeEditor) missing.editor = missingEditor(uri);
      return missing;
   }
   try {
      return detectLanguage(*source) == DetectedLanguage::Ili1
         ? buildIli1(*source,includeEditor) : buildIli2(*source,includeEditor);
   }
   catch (const std::exception &error) {
      SnapshotBundle failure;
      failure.syntax.uri = source->uri;
      failure.syntax.documentVersion = source->version;
      failure.syntax.iliVersion = "unknown";
      Diagnostic diagnostic;
      diagnostic.severity = DiagnosticSeverity::Error;
      diagnostic.code = "ILIC-SYNTAX";
      diagnostic.message = std::string("snapshot parser failure: ") + error.what();
      diagnostic.range = SourceRangeMapper(*source).eof();
      diagnostic.source = "compiler";
      failure.syntax.diagnostics.push_back(std::move(diagnostic));
      if (includeEditor) {
         failure.editor.uri = source->uri;
         failure.editor.documentVersion = source->version;
         failure.editor.iliVersion = "unknown";
         failure.editor.diagnostics = failure.syntax.diagnostics;
         failure.editor.recovered = true;
      }
      return failure;
   }
   catch (...) {
      SnapshotBundle failure;
      failure.syntax.uri = source->uri;
      failure.syntax.documentVersion = source->version;
      failure.syntax.iliVersion = "unknown";
      Diagnostic diagnostic;
      diagnostic.severity = DiagnosticSeverity::Error;
      diagnostic.code = "ILIC-SYNTAX";
      diagnostic.message = "snapshot parser failure: unknown exception";
      diagnostic.range = SourceRangeMapper(*source).eof();
      diagnostic.source = "compiler";
      failure.syntax.diagnostics.push_back(std::move(diagnostic));
      if (includeEditor) {
         failure.editor.uri = source->uri;
         failure.editor.documentVersion = source->version;
         failure.editor.iliVersion = "unknown";
         failure.editor.diagnostics = failure.syntax.diagnostics;
         failure.editor.recovered = true;
      }
      return failure;
   }
}

SyntaxSnapshot SnapshotPipeline::syntax(const std::string &uri) const
{
   return build(uri,false).syntax;
}

EditorSnapshot SnapshotPipeline::editor(const std::string &uri) const
{
   return build(uri,true).editor;
}

} // namespace ilic::detail
