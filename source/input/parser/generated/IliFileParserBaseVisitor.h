
// Generated from IliFileParser.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "IliFileParserVisitor.h"


namespace parser {

/**
 * This class provides an empty implementation of IliFileParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  IliFileParserBaseVisitor : public IliFileParserVisitor {
public:

  virtual antlrcpp::Any visitIliFile(IliFileParser::IliFileContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitVersion(IliFileParser::VersionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitModelName(IliFileParser::ModelNameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTranslationOf(IliFileParser::TranslationOfContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitImports(IliFileParser::ImportsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitModelImport(IliFileParser::ModelImportContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace parser
