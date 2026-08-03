#include "ParsedSourceArtifactFactory.h"
#include "EditorSnapshotProjector.h"
#include "SnapshotParserSupport.h"
#include "SyntaxSnapshotProjector.h"
#include "StrictSourceParser.h"
#include "TolerantEditorParser.h"

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
   SyntaxSnapshotProjector::project(root,parser,lexer,tokens,ranges,bundle.syntax);
   bundle.syntax.diagnostics = std::move(lexerErrors.diagnostics);
   bundle.syntax.diagnostics.insert(bundle.syntax.diagnostics.end(),parserErrors.diagnostics.begin(),parserErrors.diagnostics.end());
   sortDiagnostics(bundle.syntax.diagnostics);
   bundle.syntax.success = bundle.syntax.diagnostics.empty();
   const bool recovered =
      dynamic_cast<const EditorRecoveryTokenStream *>(&tokens) != nullptr &&
      static_cast<const EditorRecoveryTokenStream *>(&tokens)->recovered();
   bundle.editor = EditorSnapshotProjector::project(
      ranges,bundle.syntax.iliVersion,root,bundle.syntax.diagnostics,
      bundle.syntax.success,recovered);
   // The editor contract historically treated UNQUALIFIED as applying to
   // the complete comma-separated import clause. Keep that projection
   // stable while SyntaxSnapshot preserves the grammar's per-import flag.
   bool importClauseUnqualified = false;
   for (const auto &reference : bundle.editor.imports)
      importClauseUnqualified = importClauseUnqualified || reference.unqualified;
   if (importClauseUnqualified)
      for (auto &reference : bundle.editor.imports) reference.unqualified = true;

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
      if constexpr (std::is_same_v<TokenStream,EditorRecoveryTokenStream>) tokens->recoverBareModelHeaders();
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
      ? buildIli2Impl<EditorRecoveryTokenStream>(source,mode,true)
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

SnapshotBundle ParsedSourceArtifactFactory::build(const SourceManager &sources, const std::string &uri,ParseMode mode)
{
   const SourceBuffer *source = sources.get(uri);
   if (source == nullptr) {
      SnapshotBundle missing;
      missing.mode = mode;
      missing.syntax = missingSyntax(uri);
      if (mode == ParseMode::TolerantEditor) missing.editor = missingEditor(uri);
      return missing;
   }
   return build(*source,mode);
}

SnapshotBundle ParsedSourceArtifactFactory::build(const SourceBuffer &source,ParseMode mode)
{
   try {
      const auto language = detectLanguage(source);
      return mode == ParseMode::StrictCompiler
         ? StrictSourceParser::parse(source,language,buildIli1,buildIli2)
         : TolerantEditorParser::parse(source,language,buildIli1,buildIli2);
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

} // namespace ilic::detail

