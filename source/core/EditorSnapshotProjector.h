#pragma once

#include "SnapshotPipeline.h"
#include "SnapshotParserSupport.h"
#include "SourceRangeMapper.h"
#include "../input/parser/generated/Ili1Parser.h"
#include "../input/parser/generated/Ili2Parser.h"
#include "tree/ParseTree.h"
#include "Ili1EditorProjector.h"
#include "Ili2EditorProjector.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace ilic::detail {

namespace editor_snapshot_detail {

inline bool isContainer(EditorSymbolKind kind)
{
   return kind == EditorSymbolKind::Model || kind == EditorSymbolKind::Topic
      || kind == EditorSymbolKind::Class || kind == EditorSymbolKind::Structure
      || kind == EditorSymbolKind::Association || kind == EditorSymbolKind::View
      || kind == EditorSymbolKind::Graphic;
}

inline const char *symbolKind(EditorSymbolKind kind)
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

inline const char *referenceKind(EditorReferenceKind kind)
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
inline std::string pathText(const Context *context)
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

inline EditorDeclarationMatch ili2DeclarationMatch(antlr4::tree::ParseTree *tree)
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

inline EditorDeclarationMatch ili1DeclarationMatch(antlr4::tree::ParseTree *tree)
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

inline EditorReferenceKind ili2ReferenceKind(const parser::Ili2Parser::PathContext *path)
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

inline void collectIli2Editor(EditorSnapshotAccumulator &output,const SourceRangeMapper &ranges,
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

inline void collectIli1Editor(EditorSnapshotAccumulator &output,
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



} // namespace editor_snapshot_detail

class EditorSnapshotProjector final {
public:
   template<class Root>
   static EditorSnapshot project(
      const SourceRangeMapper &ranges,
      std::string iliVersion,
      Root *root,
      const std::vector<Diagnostic> &syntaxDiagnostics,
      bool parserComplete,
      bool recovered)
   {
      editor_snapshot_detail::EditorSnapshotAccumulator editor(ranges,std::move(iliVersion));
      if constexpr (std::is_same_v<Root,parser::Ili2Parser::Interlis2DefContext>) {
         Ili2EditorProjector::project(editor,ranges,root);
      }
      else {
         Ili1EditorProjector::project(editor,root);
      }
      if (recovered) editor.markRecovered();
      editor.appendSyntaxDiagnostics(syntaxDiagnostics);
      return editor.finish(parserComplete);
   }
};

} // namespace ilic::detail
