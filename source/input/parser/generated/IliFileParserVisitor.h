
// Generated from IliFileParser.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "IliFileParser.h"


namespace parser {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by IliFileParser.
 */
class  IliFileParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by IliFileParser.
   */
    virtual antlrcpp::Any visitIliFile(IliFileParser::IliFileContext *context) = 0;

    virtual antlrcpp::Any visitVersion(IliFileParser::VersionContext *context) = 0;

    virtual antlrcpp::Any visitModelName(IliFileParser::ModelNameContext *context) = 0;

    virtual antlrcpp::Any visitTranslationOf(IliFileParser::TranslationOfContext *context) = 0;

    virtual antlrcpp::Any visitImports(IliFileParser::ImportsContext *context) = 0;

    virtual antlrcpp::Any visitModelImport(IliFileParser::ModelImportContext *context) = 0;


};

}  // namespace parser
