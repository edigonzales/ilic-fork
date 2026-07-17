
// Generated from Ili2Parser.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "Ili2ParserVisitor.h"


namespace parser {

/**
 * This class provides an empty implementation of Ili2ParserVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  Ili2ParserBaseVisitor : public Ili2ParserVisitor {
public:

  virtual antlrcpp::Any visitDecimal(Ili2Parser::DecimalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPath(Ili2Parser::PathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitInterlis2Def(Ili2Parser::Interlis2DefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitModelDef(Ili2Parser::ModelDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitImportDef(Ili2Parser::ImportDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitImporting(Ili2Parser::ImportingContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTopicDef(Ili2Parser::TopicDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTopicPath(Ili2Parser::TopicPathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitClassDef(Ili2Parser::ClassDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitStructureDef(Ili2Parser::StructureDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitClassOrStructureDef(Ili2Parser::ClassOrStructureDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAttributeDef(Ili2Parser::AttributeDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAttrTypeDef(Ili2Parser::AttrTypeDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBagOrListType(Ili2Parser::BagOrListTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAttrType(Ili2Parser::AttrTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitReferenceAttr(Ili2Parser::ReferenceAttrContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRestrictedRef(Ili2Parser::RestrictedRefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRestriction(Ili2Parser::RestrictionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAssociationDef(Ili2Parser::AssociationDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAssociationRef(Ili2Parser::AssociationRefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRoleDef(Ili2Parser::RoleDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCardinality(Ili2Parser::CardinalityContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDomainDef(Ili2Parser::DomainDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDomainType(Ili2Parser::DomainTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitType(Ili2Parser::TypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBaseType(Ili2Parser::BaseTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstant(Ili2Parser::ConstantContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTextType(Ili2Parser::TextTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTextConst(Ili2Parser::TextConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEnumerationType(Ili2Parser::EnumerationTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEnumTreeValueType(Ili2Parser::EnumTreeValueTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAlignmentType(Ili2Parser::AlignmentTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBooleanType(Ili2Parser::BooleanTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEnumeration(Ili2Parser::EnumerationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEnumElement(Ili2Parser::EnumElementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEnumConst(Ili2Parser::EnumConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNumericType(Ili2Parser::NumericTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDateTimeType(Ili2Parser::DateTimeTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRefSys(Ili2Parser::RefSysContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDecConst(Ili2Parser::DecConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitNumericConst(Ili2Parser::NumericConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFormattedType(Ili2Parser::FormattedTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFormatDef(Ili2Parser::FormatDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBaseAttrRef(Ili2Parser::BaseAttrRefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFormattedConst(Ili2Parser::FormattedConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitContextDef(Ili2Parser::ContextDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitContextDecl(Ili2Parser::ContextDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCoordinateType(Ili2Parser::CoordinateTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRotationDef(Ili2Parser::RotationDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitOIDType(Ili2Parser::OIDTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBlackboxType(Ili2Parser::BlackboxTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitClassRefType(Ili2Parser::ClassRefTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAttributePathType(Ili2Parser::AttributePathTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitClassConst(Ili2Parser::ClassConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAttributePathConst(Ili2Parser::AttributePathConstContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLineType(Ili2Parser::LineTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLineForm(Ili2Parser::LineFormContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLineFormType(Ili2Parser::LineFormTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLineFormTypeDef(Ili2Parser::LineFormTypeDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLineFormTypeDecl(Ili2Parser::LineFormTypeDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnitDecl(Ili2Parser::UnitDeclContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUnitDef(Ili2Parser::UnitDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDerivedUnit(Ili2Parser::DerivedUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitComposedUnit(Ili2Parser::ComposedUnitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitComposedUnitExpr(Ili2Parser::ComposedUnitExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMetaDataBasketDef(Ili2Parser::MetaDataBasketDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMetaDataBasketRef(Ili2Parser::MetaDataBasketRefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMetaObjectRef(Ili2Parser::MetaObjectRefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitParameterDef(Ili2Parser::ParameterDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitProperties(Ili2Parser::PropertiesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRunTimeParameterDef(Ili2Parser::RunTimeParameterDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRunTimeParameter(Ili2Parser::RunTimeParameterContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstraintDef(Ili2Parser::ConstraintDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitMandatoryConstraint(Ili2Parser::MandatoryConstraintContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPlausibilityConstraint(Ili2Parser::PlausibilityConstraintContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExistenceConstraint(Ili2Parser::ExistenceConstraintContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUniquenessConstraint(Ili2Parser::UniquenessConstraintContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitGlobalUniqueness(Ili2Parser::GlobalUniquenessContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitUniqueEl(Ili2Parser::UniqueElContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLocalUniqueness(Ili2Parser::LocalUniquenessContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitLocalUniqueEl(Ili2Parser::LocalUniqueElContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSetConstraint(Ili2Parser::SetConstraintContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitConstraintsDef(Ili2Parser::ConstraintsDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitExpression(Ili2Parser::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTerm1(Ili2Parser::Term1Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitOperator1(Ili2Parser::Operator1Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTerm2(Ili2Parser::Term2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitOperator2(Ili2Parser::Operator2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTerm3(Ili2Parser::Term3Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitTerm(Ili2Parser::TermContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRelation(Ili2Parser::RelationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFactor(Ili2Parser::FactorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitObjectOrAttributePath(Ili2Parser::ObjectOrAttributePathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAttributePath(Ili2Parser::AttributePathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPathEl(Ili2Parser::PathElContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitObjectRef(Ili2Parser::ObjectRefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFunctionCall(Ili2Parser::FunctionCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFunctionCallArgument(Ili2Parser::FunctionCallArgumentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFunctionDef(Ili2Parser::FunctionDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFunctionDefParam(Ili2Parser::FunctionDefParamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitArgumentType(Ili2Parser::ArgumentTypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitViewDef(Ili2Parser::ViewDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFormationDef(Ili2Parser::FormationDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitProjection(Ili2Parser::ProjectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitJoin(Ili2Parser::JoinContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitIliunion(Ili2Parser::IliunionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitAggregation(Ili2Parser::AggregationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitInspection(Ili2Parser::InspectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitRenamedViewableRef(Ili2Parser::RenamedViewableRefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBaseExtensionDef(Ili2Parser::BaseExtensionDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSelection(Ili2Parser::SelectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitViewAttribute(Ili2Parser::ViewAttributeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitGraphicDef(Ili2Parser::GraphicDefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitDrawingRule(Ili2Parser::DrawingRuleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCondSignParamAssignment(Ili2Parser::CondSignParamAssignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSignParamAssignment(Ili2Parser::SignParamAssignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEnumAssignment(Ili2Parser::EnumAssignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitEnumRange(Ili2Parser::EnumRangeContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace parser
