#include "antlr4-runtime.h"
#include "../parser/generated/Ili1ParserBaseVisitor.h"

#include <string>

using namespace std;

namespace input {
   
   void parseIli1(string input);

   class Ili1Input : public parser::Ili1ParserBaseVisitor {
   public:

      antlrcpp::Any visitInterlis1Def(parser::Ili1Parser::Interlis1DefContext *ctx) override;
      antlrcpp::Any visitModelDef(parser::Ili1Parser::ModelDefContext *ctx) override;
      antlrcpp::Any visitTopicDef(parser::Ili1Parser::TopicDefContext *ctx) override;
      antlrcpp::Any visitTableDef(parser::Ili1Parser::TableDefContext *ctx) override;
      antlrcpp::Any visitDecimal(parser::Ili1Parser::DecimalContext *ctx) override;
      antlrcpp::Any visitDerivatives(parser::Ili1Parser::DerivativesContext *ctx) override;
      antlrcpp::Any visitFormatEncoding(parser::Ili1Parser::FormatEncodingContext *ctx) override;
      antlrcpp::Any visitFont(parser::Ili1Parser::FontContext *ctx) override;
      antlrcpp::Any visitCode(parser::Ili1Parser::CodeContext *ctx) override;
      antlrcpp::Any visitAttribute(parser::Ili1Parser::AttributeContext *ctx) override;
      antlrcpp::Any visitIdentifications(parser::Ili1Parser::IdentificationsContext *ctx) override;
      antlrcpp::Any visitIdentification(parser::Ili1Parser::IdentificationContext *ctx) override;
      antlrcpp::Any visitDomainDefs(parser::Ili1Parser::DomainDefsContext *ctx) override;
      antlrcpp::Any visitDomainDef(parser::Ili1Parser::DomainDefContext *ctx) override;
      antlrcpp::Any visitType(parser::Ili1Parser::TypeContext *ctx) override;
      antlrcpp::Any visitBaseType(parser::Ili1Parser::BaseTypeContext *ctx) override;
      antlrcpp::Any visitCoord2(parser::Ili1Parser::Coord2Context *ctx) override;
      antlrcpp::Any visitCoord3(parser::Ili1Parser::Coord3Context *ctx) override;
      antlrcpp::Any visitNumericRange(parser::Ili1Parser::NumericRangeContext *ctx) override;
      antlrcpp::Any visitDim1Type(parser::Ili1Parser::Dim1TypeContext *ctx) override;
      antlrcpp::Any visitDim2Type(parser::Ili1Parser::Dim2TypeContext *ctx) override;
      antlrcpp::Any visitAngleType(parser::Ili1Parser::AngleTypeContext *ctx) override;
      antlrcpp::Any visitTextType(parser::Ili1Parser::TextTypeContext *ctx) override;
      antlrcpp::Any visitDateType(parser::Ili1Parser::DateTypeContext *ctx) override;
      antlrcpp::Any visitEnumerationType(parser::Ili1Parser::EnumerationTypeContext *ctx) override;
      antlrcpp::Any visitEnumElement(parser::Ili1Parser::EnumElementContext *ctx) override;
      antlrcpp::Any visitHorizAlignment(parser::Ili1Parser::HorizAlignmentContext *ctx) override;
      antlrcpp::Any visitVertAlignment(parser::Ili1Parser::VertAlignmentContext *ctx) override;
      antlrcpp::Any visitLineType(parser::Ili1Parser::LineTypeContext *ctx) override;
      antlrcpp::Any visitAreaType(parser::Ili1Parser::AreaTypeContext *ctx) override;
      antlrcpp::Any visitForm(parser::Ili1Parser::FormContext *ctx) override;
      //antlrcpp::Any Ili1Input::visitLineForm(parser::Ili1Parser::LineFormContext *ctx) override;
      antlrcpp::Any visitIntersectionDef(parser::Ili1Parser::IntersectionDefContext *ctx) override;
      antlrcpp::Any visitControlPoints(parser::Ili1Parser::ControlPointsContext *ctx) override;
      antlrcpp::Any visitLineAttributes(parser::Ili1Parser::LineAttributesContext *ctx) override;

   };

};
