#pragma once

#include "antlr4-runtime.h"
#include "../parser/generated/Ili2ParserBaseVisitor.h"

#include <string>

using namespace std;

namespace input {
   
   void parseIli2(string ilifile); 

   class Ili2Input : public parser::Ili2ParserBaseVisitor {
   public:
   
      antlrcpp::Any visitInterlis2Def(parser::Ili2Parser::Interlis2DefContext *ctx) override;
      antlrcpp::Any visitModelDef(parser::Ili2Parser::ModelDefContext *ctx) override;
      antlrcpp::Any visitImporting(parser::Ili2Parser::ImportingContext *ctx) override;
      antlrcpp::Any visitDomainType(parser::Ili2Parser::DomainTypeContext *ctx) override;
      antlrcpp::Any visitTextType(parser::Ili2Parser::TextTypeContext *ctx) override;
      antlrcpp::Any visitEnumerationType(parser::Ili2Parser::EnumerationTypeContext *ctx) override;
      antlrcpp::Any visitEnumTreeValueType(parser::Ili2Parser::EnumTreeValueTypeContext *ctx) override;
      antlrcpp::Any visitBooleanType(parser::Ili2Parser::BooleanTypeContext *ctx) override;
      antlrcpp::Any visitAlignmentType(parser::Ili2Parser::AlignmentTypeContext *ctx) override;
      antlrcpp::Any visitNumericType(parser::Ili2Parser::NumericTypeContext *ctx) override;
      antlrcpp::Any visitFormattedType(parser::Ili2Parser::FormattedTypeContext *ctx) override;
      antlrcpp::Any visitDateTimeType(parser::Ili2Parser::DateTimeTypeContext *ctx) override;
      antlrcpp::Any visitCoordinateType(parser::Ili2Parser::CoordinateTypeContext *ctx) override;
      antlrcpp::Any visitTopicDef(parser::Ili2Parser::TopicDefContext *ctx) override;
      antlrcpp::Any visitClassDef(parser::Ili2Parser::ClassDefContext *ctx) override;
      antlrcpp::Any visitStructureDef(parser::Ili2Parser::StructureDefContext *ctx) override;
      antlrcpp::Any visitAttributeDef(parser::Ili2Parser::AttributeDefContext *ctx) override;
      antlrcpp::Any visitFunctionDef(parser::Ili2Parser::FunctionDefContext *ctx) override;
      antlrcpp::Any visitLineFormType(parser::Ili2Parser::LineFormTypeContext *ctx) override;
      antlrcpp::Any visitLineFormTypeDef(parser::Ili2Parser::LineFormTypeDefContext *ctx) override;
      antlrcpp::Any visitMetaDataBasketDef(parser::Ili2Parser::MetaDataBasketDefContext *ctx) override;
      antlrcpp::Any visitRunTimeParameterDef(parser::Ili2Parser::RunTimeParameterDefContext *ctx) override;
      antlrcpp::Any visitLineFormTypeDecl(parser::Ili2Parser::LineFormTypeDeclContext * ctx) override;
      antlrcpp::Any visitAttrTypeDef(parser::Ili2Parser::AttrTypeDefContext *ctx) override;
      antlrcpp::Any visitAttrType(parser::Ili2Parser::AttrTypeContext *ctx) override;
      antlrcpp::Any visitBagOrListType(parser::Ili2Parser::BagOrListTypeContext *ctx) override;
      antlrcpp::Any visitReferenceAttr(parser::Ili2Parser::ReferenceAttrContext *ctx) override;
      antlrcpp::Any visitType(parser::Ili2Parser::TypeContext *ctx) override;
      antlrcpp::Any visitBaseType(parser::Ili2Parser::BaseTypeContext *ctx) override;
      antlrcpp::Any visitLineType(parser::Ili2Parser::LineTypeContext *ctx) override;
      antlrcpp::Any visitPath(parser::Ili2Parser::PathContext * ctx);
      antlrcpp::Any visitRestriction(parser::Ili2Parser::RestrictionContext * ctx);
      antlrcpp::Any visitRestrictedRef(parser::Ili2Parser::RestrictedRefContext * ctx);
      antlrcpp::Any visitLineForm(parser::Ili2Parser::LineFormContext *ctx) override;
      antlrcpp::Any visitOIDType(parser::Ili2Parser::OIDTypeContext *ctx) override;
      antlrcpp::Any visitBlackboxType(parser::Ili2Parser::BlackboxTypeContext *ctx) override;
      antlrcpp::Any visitClassRefType(parser::Ili2Parser::ClassRefTypeContext *ctx) override;
      antlrcpp::Any visitExpression(parser::Ili2Parser::ExpressionContext *ctx) override;
      antlrcpp::Any visitTerm1(parser::Ili2Parser::Term1Context *ctx) override;
      antlrcpp::Any visitTerm2(parser::Ili2Parser::Term2Context *ctx) override;
      antlrcpp::Any visitTerm3(parser::Ili2Parser::Term3Context *ctx) override;
      antlrcpp::Any visitTerm(parser::Ili2Parser::TermContext *ctx) override;
      antlrcpp::Any visitRelation(parser::Ili2Parser::RelationContext *ctx) override;
      antlrcpp::Any visitFactor(parser::Ili2Parser::FactorContext *ctx) override;
      antlrcpp::Any visitViewDef(parser::Ili2Parser::ViewDefContext *ctx) override;
      antlrcpp::Any visitRenamedViewableRef(parser::Ili2Parser::RenamedViewableRefContext *ctx) override;
      antlrcpp::Any visitSelection(parser::Ili2Parser::SelectionContext *ctx) override;
      antlrcpp::Any visitViewAttribute(parser::Ili2Parser::ViewAttributeContext *ctx) override;
      antlrcpp::Any visitConstraintDef(parser::Ili2Parser::ConstraintDefContext *ctx) override;
      antlrcpp::Any visitConstraintsDef(parser::Ili2Parser::ConstraintsDefContext *ctx) override;
      antlrcpp::Any visitMandatoryConstraint(parser::Ili2Parser::MandatoryConstraintContext *ctx) override;
      antlrcpp::Any visitPlausibilityConstraint(parser::Ili2Parser::PlausibilityConstraintContext *ctx) override;
      antlrcpp::Any visitExistenceConstraint(parser::Ili2Parser::ExistenceConstraintContext *ctx) override;
      antlrcpp::Any visitUniquenessConstraint(parser::Ili2Parser::UniquenessConstraintContext *ctx) override;
      antlrcpp::Any visitGlobalUniqueness(parser::Ili2Parser::GlobalUniquenessContext *ctx) override;
      antlrcpp::Any visitUniqueEl(parser::Ili2Parser::UniqueElContext *ctx) override;
      antlrcpp::Any visitLocalUniqueness(parser::Ili2Parser::LocalUniquenessContext *ctx) override;
      antlrcpp::Any visitSetConstraint(parser::Ili2Parser::SetConstraintContext *ctx) override;
      antlrcpp::Any visitBaseExtensionDef(parser::Ili2Parser::BaseExtensionDefContext *ctx) override;
      antlrcpp::Any visitGraphicDef(parser::Ili2Parser::GraphicDefContext *ctx) override;
      antlrcpp::Any visitDrawingRule(parser::Ili2Parser::DrawingRuleContext *ctx) override;
      antlrcpp::Any visitSignParamAssignment(parser::Ili2Parser::SignParamAssignmentContext *ctx) override;
      antlrcpp::Any visitCondSignParamAssignment(parser::Ili2Parser::CondSignParamAssignmentContext *ctx) override;
      antlrcpp::Any visitDecimal(parser::Ili2Parser::DecimalContext *ctx) override;
      antlrcpp::Any visitEnumElement(parser::Ili2Parser::EnumElementContext *ctx) override;
      antlrcpp::Any visitAssociationDef(parser::Ili2Parser::AssociationDefContext *ctx) override;
      antlrcpp::Any visitRoleDef(parser::Ili2Parser::RoleDefContext *ctx) override;
      antlrcpp::Any visitCardinality(parser::Ili2Parser::CardinalityContext *ctx) override;
      antlrcpp::Any visitConstant(parser::Ili2Parser::ConstantContext *ctx) override;
      antlrcpp::Any visitTextConst(parser::Ili2Parser::TextConstContext *ctx) override;
      antlrcpp::Any visitEnumConst(parser::Ili2Parser::EnumConstContext *ctx) override;
      antlrcpp::Any visitNumericConst(parser::Ili2Parser::NumericConstContext *ctx) override;
      antlrcpp::Any visitClassConst(parser::Ili2Parser::ClassConstContext *ctx) override;
      antlrcpp::Any visitAttributePathConst(parser::Ili2Parser::AttributePathConstContext *ctx) override;
      antlrcpp::Any visitDecConst(parser::Ili2Parser::DecConstContext *ctx) override;
      antlrcpp::Any visitFormattedConst(parser::Ili2Parser::FormattedConstContext *ctx) override;
      antlrcpp::Any visitUnitDef(parser::Ili2Parser::UnitDefContext *ctx) override;
      antlrcpp::Any visitDerivedUnit(parser::Ili2Parser::DerivedUnitContext *ctx) override;
      antlrcpp::Any visitComposedUnit(parser::Ili2Parser::ComposedUnitContext* ctx) override;
      antlrcpp::Any visitComposedUnitExpr(parser::Ili2Parser::ComposedUnitExprContext* ctx) override;
      antlrcpp::Any visitBaseAttrRef(parser::Ili2Parser::BaseAttrRefContext *ctx) override;
      antlrcpp::Any visitMetaObjectRef(parser::Ili2Parser::MetaObjectRefContext *ctx) override;
      antlrcpp::Any visitRefSys(parser::Ili2Parser::RefSysContext *ctx) override;
      antlrcpp::Any visitFormatDef(parser::Ili2Parser::FormatDefContext *ctx) override;
      antlrcpp::Any visitAttributePath(parser::Ili2Parser::AttributePathContext *ctx) override;
      antlrcpp::Any visitObjectOrAttributePath(parser::Ili2Parser::ObjectOrAttributePathContext *ctx) override;
      antlrcpp::Any visitPathEl(parser::Ili2Parser::PathElContext *ctx) override;
      antlrcpp::Any visitAttributePathType(parser::Ili2Parser::AttributePathTypeContext *ctx) override;
      antlrcpp::Any visitEnumAssignment(parser::Ili2Parser::EnumAssignmentContext *ctx) override;
      antlrcpp::Any visitEnumRange(parser::Ili2Parser::EnumRangeContext *ctx) override;
      antlrcpp::Any visitMetaDataBasketRef(parser::Ili2Parser::MetaDataBasketRefContext *ctx) override;
      antlrcpp::Any visitParameterDef(parser::Ili2Parser::ParameterDefContext *ctx) override;
      antlrcpp::Any visitFunctionCall(parser::Ili2Parser::FunctionCallContext *ctx) override;
      antlrcpp::Any visitFunctionCallArgument(parser::Ili2Parser::FunctionCallArgumentContext *ctx) override;
      antlrcpp::Any visitArgumentType(parser::Ili2Parser::ArgumentTypeContext *ctx) override;
      
   };

};
