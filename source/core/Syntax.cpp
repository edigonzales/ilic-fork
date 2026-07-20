#include "../../include/ilic/Syntax.h"

#include "../input/parser/generated/Ili1Lexer.h"
#include "../input/parser/generated/Ili1Parser.h"
#include "../input/parser/generated/Ili2Lexer.h"
#include "../input/parser/generated/Ili2Parser.h"

#include "BaseErrorListener.h"
#include "CommonTokenStream.h"
#include "ParserRuleContext.h"
#include "tree/ParseTree.h"

#include <algorithm>
#include <cctype>
#include <set>

namespace ilic {
namespace {

std::size_t utf8ByteOffset(const std::string &text,std::size_t codepointOffset)
{
   std::size_t byteOffset = 0;
   for (std::size_t index = 0; index < codepointOffset && byteOffset < text.size(); ++index) {
      const unsigned char lead = static_cast<unsigned char>(text[byteOffset]);
      std::size_t width = 1;
      if ((lead & 0xe0) == 0xc0) width = 2;
      else if ((lead & 0xf0) == 0xe0) width = 3;
      else if ((lead & 0xf8) == 0xf0) width = 4;
      byteOffset = std::min(text.size(),byteOffset + width);
   }
   return byteOffset;
}

SourceRange rangeAt(const SourceManager &sources,const std::string &uri,
   std::size_t start,std::size_t end)
{
   const SourceBuffer *source = sources.get(uri);
   SourceRange value;
   if (source == nullptr) return value;
   const std::size_t startCodepoint = start;
   start = utf8ByteOffset(source->text,startCodepoint);
   end = utf8ByteOffset(source->text,std::max(end,startCodepoint));
   const SourcePosition startPosition = sources.position(uri,start);
   const SourcePosition endPosition = sources.position(uri,end);
   value.uri = uri;
   value.start = {startPosition.line,startPosition.utf16Column,startPosition.offset};
   value.end = {endPosition.line,endPosition.utf16Column,endPosition.offset};
   value.valid = true;
   return value;
}

class SnapshotErrorListener : public antlr4::BaseErrorListener {
public:
   SnapshotErrorListener(const SourceManager &sources,std::string uri)
      : sources_(sources),uri_(std::move(uri)) {}

   void syntaxError(antlr4::Recognizer *,antlr4::Token *offendingSymbol,
      std::size_t,std::size_t,const std::string &message,std::exception_ptr) override
   {
      std::size_t start = 0;
      std::size_t end = 0;
      if (offendingSymbol != nullptr && offendingSymbol->getType() != antlr4::Token::EOF) {
         start = offendingSymbol->getStartIndex();
         end = offendingSymbol->getStopIndex() + 1;
      }
      else if (const SourceBuffer *source = sources_.get(uri_)) {
         start = source->text.size();
         end = start;
      }
      Diagnostic diagnostic;
      diagnostic.severity = DiagnosticSeverity::Error;
      diagnostic.code = "ILIC-SYNTAX";
      diagnostic.message = message;
      diagnostic.range = rangeAt(sources_,uri_,start,end);
      diagnostics.push_back(std::move(diagnostic));
   }

   std::vector<Diagnostic> diagnostics;

private:
   const SourceManager &sources_;
   std::string uri_;
};

bool isEditorContext(const std::string &kind)
{
   static const std::set<std::string> contexts = {
      "modelDef","topicDef","classDef","structureDef","associationDef","viewDef",
      "graphicDef","attributeDef","domainDef","unitDef","importDef","importing",
      "textType","numericType","properties","path"
   };
   return contexts.find(kind) != contexts.end();
}

template<typename Parser>
void appendNodes(antlr4::tree::ParseTree *tree,const Parser &parser,
   const SourceManager &sources,const std::string &uri,SyntaxSnapshot &snapshot,
   std::size_t parent,bool hasParent)
{
   auto *rule = dynamic_cast<antlr4::ParserRuleContext *>(tree);
   std::size_t currentParent = parent;
   bool currentHasParent = hasParent;
   if (rule != nullptr) {
      const auto &ruleNames = parser.getRuleNames();
      const std::size_t ruleIndex = rule->getRuleIndex();
      const std::string kind = ruleIndex < ruleNames.size() ? ruleNames[ruleIndex] : "unknown";
      antlr4::Token *startToken = rule->getStart();
      antlr4::Token *stopToken = rule->getStop();
      const std::size_t start = startToken != nullptr && startToken->getType() != antlr4::Token::EOF
         ? startToken->getStartIndex() : 0;
      const std::size_t end = stopToken != nullptr && stopToken->getType() != antlr4::Token::EOF
         ? stopToken->getStopIndex() + 1 : start;
      const std::size_t id = snapshot.nodes.size();
      SyntaxNode node;
      node.id = id;
      node.parent = parent;
      node.hasParent = hasParent;
      node.kind = kind;
      node.range = rangeAt(sources,uri,start,end);
      snapshot.nodes.push_back(node);
      if (isEditorContext(kind)) snapshot.contexts.push_back({kind,node.range});
      currentParent = id;
      currentHasParent = true;
   }
   for (auto *child : tree->children) {
      if (child != nullptr)
         appendNodes(child,parser,sources,uri,snapshot,currentParent,currentHasParent);
   }
}

template<typename Lexer>
void appendTokens(antlr4::CommonTokenStream &stream,Lexer &lexer,
   const SourceManager &sources,const std::string &uri,SyntaxSnapshot &snapshot)
{
   stream.fill();
   for (auto *token : stream.getTokens()) {
      if (token == nullptr || token->getType() == antlr4::Token::EOF) continue;
      const std::size_t start = token->getStartIndex();
      const std::size_t end = token->getStopIndex() + 1;
      std::string kind = lexer.getVocabulary().getSymbolicName(token->getType());
      if (kind.empty()) kind = lexer.getVocabulary().getLiteralName(token->getType());
      snapshot.tokens.push_back({kind,token->getText(),token->getChannel(),
         rangeAt(sources,uri,start,end)});
   }
}

SyntaxSnapshot parseIli2(const SourceManager &sources,const SourceBuffer &source)
{
   SyntaxSnapshot snapshot;
   snapshot.uri = source.uri;
   snapshot.documentVersion = source.version;
   snapshot.iliVersion = source.text.find("2.4") != std::string::npos ? "2.4" : "2.3";

   antlr4::ANTLRInputStream input(source.text);
   lexer::Ili2Lexer lexer(&input);
   SnapshotErrorListener lexerErrors(sources,source.uri);
   lexer.removeErrorListeners();
   lexer.addErrorListener(&lexerErrors);
   antlr4::CommonTokenStream tokens(&lexer);
   parser::Ili2Parser parser(&tokens);
   SnapshotErrorListener parserErrors(sources,source.uri);
   parser.removeErrorListeners();
   parser.addErrorListener(&parserErrors);
   auto *root = parser.interlis2Def();

   appendTokens(tokens,lexer,sources,source.uri,snapshot);
   if (root != nullptr) {
      appendNodes(root,parser,sources,source.uri,snapshot,0,false);
      for (auto *model : root->modelDef()) {
         for (auto *definition : model->importDef()) {
            for (auto *importing : definition->importing()) {
               antlr4::tree::TerminalNode *name = importing->INTERLIS();
               if (name == nullptr) name = importing->NAME();
               if (name == nullptr) continue;
               const std::string modelName = name->getText();
               snapshot.imports.push_back(modelName);
               antlr4::Token *token = name->getSymbol();
               const SourceRange importRange = token == nullptr
                  ? SourceRange{}
                  : rangeAt(sources,source.uri,token->getStartIndex(),token->getStopIndex() + 1);
               snapshot.importReferences.push_back({modelName,
                  importing->UNQUALIFIED() != nullptr,importRange});
            }
         }
      }
   }
   snapshot.diagnostics = std::move(lexerErrors.diagnostics);
   snapshot.diagnostics.insert(snapshot.diagnostics.end(),parserErrors.diagnostics.begin(),parserErrors.diagnostics.end());
   snapshot.success = snapshot.diagnostics.empty();
   return snapshot;
}

SyntaxSnapshot parseIli1(const SourceManager &sources,const SourceBuffer &source)
{
   SyntaxSnapshot snapshot;
   snapshot.uri = source.uri;
   snapshot.documentVersion = source.version;
   snapshot.iliVersion = "1.0";

   antlr4::ANTLRInputStream input(source.text);
   lexer::Ili1Lexer lexer(&input);
   SnapshotErrorListener lexerErrors(sources,source.uri);
   lexer.removeErrorListeners();
   lexer.addErrorListener(&lexerErrors);
   antlr4::CommonTokenStream tokens(&lexer);
   parser::Ili1Parser parser(&tokens);
   SnapshotErrorListener parserErrors(sources,source.uri);
   parser.removeErrorListeners();
   parser.addErrorListener(&parserErrors);
   auto *root = parser.interlis1Def();

   appendTokens(tokens,lexer,sources,source.uri,snapshot);
   if (root != nullptr) appendNodes(root,parser,sources,source.uri,snapshot,0,false);
   snapshot.diagnostics = std::move(lexerErrors.diagnostics);
   snapshot.diagnostics.insert(snapshot.diagnostics.end(),parserErrors.diagnostics.begin(),parserErrors.diagnostics.end());
   snapshot.success = snapshot.diagnostics.empty();
   return snapshot;
}

} // namespace

SyntaxSnapshot parseSyntax(const SourceManager &sources,const std::string &uri)
{
   const SourceBuffer *source = sources.get(uri);
   if (source == nullptr) {
      SyntaxSnapshot missing;
      missing.uri = uri;
      Diagnostic diagnostic;
      diagnostic.severity = DiagnosticSeverity::Error;
      diagnostic.code = "ILIC-SOURCE-NOT-FOUND";
      diagnostic.message = "syntax source is not registered";
      missing.diagnostics.push_back(std::move(diagnostic));
      return missing;
   }

   auto first = std::find_if_not(source->text.begin(),source->text.end(),
      [](unsigned char character) { return std::isspace(character) != 0; });
   const std::string prefix(first,source->text.end());
   return prefix.rfind("TRANSFER",0) == 0 ? parseIli1(sources,*source) : parseIli2(sources,*source);
}

} // namespace ilic
