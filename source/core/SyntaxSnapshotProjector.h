#pragma once

#include "SnapshotPipeline.h"
#include "SourceRangeMapper.h"
#include "ParserRuleContext.h"
#include "CommonTokenStream.h"
#include "tree/ParseTree.h"

#include <cstddef>
#include <set>
#include <string>
#include <vector>

namespace ilic::detail {

class SyntaxSnapshotProjector final {
public:
   template<class Parser,class Lexer,class Root>
   static void project(
      Root *root,
      const Parser &parser,
      Lexer &lexer,
      antlr4::CommonTokenStream &tokens,
      const SourceRangeMapper &ranges,
      SyntaxSnapshot &snapshot)
   {
      appendSyntaxTokens(tokens,lexer,ranges,snapshot);
      if (root != nullptr)
         appendSyntaxNodes(root,parser,ranges,snapshot,0,false);
   }

private:
   static bool isSyntaxContextRule(const std::string &kind)
   {
      static const std::set<std::string> rules = {
         "modelDef","topicDef","classDef","structureDef","associationDef","viewDef",
         "graphicDef","attributeDef","domainDef","unitDef","importDef","importing",
         "textType","numericType","properties","path"
      };
      return rules.find(kind) != rules.end();
   }
   
   template<class Parser>
   static void appendSyntaxNodes(antlr4::tree::ParseTree *tree,const Parser &parser,
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
   static void appendSyntaxTokens(antlr4::CommonTokenStream &tokens,const Lexer &lexer,
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
   
   
};

} // namespace ilic::detail
