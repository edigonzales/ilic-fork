
// Generated from Ili1Parser.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "Ili1Parser.h"


namespace parser {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by Ili1Parser.
 */
class  Ili1ParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by Ili1Parser.
   */
    virtual antlrcpp::Any visitInterlis1Def(Ili1Parser::Interlis1DefContext *context) = 0;

    virtual antlrcpp::Any visitModelDef(Ili1Parser::ModelDefContext *context) = 0;

    virtual antlrcpp::Any visitDomainDefs(Ili1Parser::DomainDefsContext *context) = 0;

    virtual antlrcpp::Any visitDomainDef(Ili1Parser::DomainDefContext *context) = 0;

    virtual antlrcpp::Any visitTopicDef(Ili1Parser::TopicDefContext *context) = 0;

    virtual antlrcpp::Any visitTableDef(Ili1Parser::TableDefContext *context) = 0;

    virtual antlrcpp::Any visitAttribute(Ili1Parser::AttributeContext *context) = 0;

    virtual antlrcpp::Any visitIdentifications(Ili1Parser::IdentificationsContext *context) = 0;

    virtual antlrcpp::Any visitIdentification(Ili1Parser::IdentificationContext *context) = 0;

    virtual antlrcpp::Any visitType(Ili1Parser::TypeContext *context) = 0;

    virtual antlrcpp::Any visitBaseType(Ili1Parser::BaseTypeContext *context) = 0;

    virtual antlrcpp::Any visitCoord2(Ili1Parser::Coord2Context *context) = 0;

    virtual antlrcpp::Any visitCoord3(Ili1Parser::Coord3Context *context) = 0;

    virtual antlrcpp::Any visitNumericRange(Ili1Parser::NumericRangeContext *context) = 0;

    virtual antlrcpp::Any visitDim1Type(Ili1Parser::Dim1TypeContext *context) = 0;

    virtual antlrcpp::Any visitDim2Type(Ili1Parser::Dim2TypeContext *context) = 0;

    virtual antlrcpp::Any visitAngleType(Ili1Parser::AngleTypeContext *context) = 0;

    virtual antlrcpp::Any visitTextType(Ili1Parser::TextTypeContext *context) = 0;

    virtual antlrcpp::Any visitDateType(Ili1Parser::DateTypeContext *context) = 0;

    virtual antlrcpp::Any visitEnumerationType(Ili1Parser::EnumerationTypeContext *context) = 0;

    virtual antlrcpp::Any visitEnumElement(Ili1Parser::EnumElementContext *context) = 0;

    virtual antlrcpp::Any visitHorizAlignment(Ili1Parser::HorizAlignmentContext *context) = 0;

    virtual antlrcpp::Any visitVertAlignment(Ili1Parser::VertAlignmentContext *context) = 0;

    virtual antlrcpp::Any visitLineType(Ili1Parser::LineTypeContext *context) = 0;

    virtual antlrcpp::Any visitAreaType(Ili1Parser::AreaTypeContext *context) = 0;

    virtual antlrcpp::Any visitForm(Ili1Parser::FormContext *context) = 0;

    virtual antlrcpp::Any visitLineForm(Ili1Parser::LineFormContext *context) = 0;

    virtual antlrcpp::Any visitIntersectionDef(Ili1Parser::IntersectionDefContext *context) = 0;

    virtual antlrcpp::Any visitControlPoints(Ili1Parser::ControlPointsContext *context) = 0;

    virtual antlrcpp::Any visitLineAttributes(Ili1Parser::LineAttributesContext *context) = 0;

    virtual antlrcpp::Any visitDerivatives(Ili1Parser::DerivativesContext *context) = 0;

    virtual antlrcpp::Any visitView(Ili1Parser::ViewContext *context) = 0;

    virtual antlrcpp::Any visitViewDef(Ili1Parser::ViewDefContext *context) = 0;

    virtual antlrcpp::Any visitFormatEncoding(Ili1Parser::FormatEncodingContext *context) = 0;

    virtual antlrcpp::Any visitFont(Ili1Parser::FontContext *context) = 0;

    virtual antlrcpp::Any visitCode(Ili1Parser::CodeContext *context) = 0;

    virtual antlrcpp::Any visitDecimal(Ili1Parser::DecimalContext *context) = 0;


};

}  // namespace parser
