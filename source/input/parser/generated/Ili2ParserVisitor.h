
// Generated from Ili2Parser.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "Ili2Parser.h"


namespace parser {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by Ili2Parser.
 */
class  Ili2ParserVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by Ili2Parser.
   */
    virtual antlrcpp::Any visitDecimal(Ili2Parser::DecimalContext *context) = 0;

    virtual antlrcpp::Any visitPath(Ili2Parser::PathContext *context) = 0;

    virtual antlrcpp::Any visitInterlis2Def(Ili2Parser::Interlis2DefContext *context) = 0;

    virtual antlrcpp::Any visitModelDef(Ili2Parser::ModelDefContext *context) = 0;

    virtual antlrcpp::Any visitImportDef(Ili2Parser::ImportDefContext *context) = 0;

    virtual antlrcpp::Any visitImporting(Ili2Parser::ImportingContext *context) = 0;

    virtual antlrcpp::Any visitTopicDef(Ili2Parser::TopicDefContext *context) = 0;

    virtual antlrcpp::Any visitTopicPath(Ili2Parser::TopicPathContext *context) = 0;

    virtual antlrcpp::Any visitClassDef(Ili2Parser::ClassDefContext *context) = 0;

    virtual antlrcpp::Any visitStructureDef(Ili2Parser::StructureDefContext *context) = 0;

    virtual antlrcpp::Any visitClassOrStructureDef(Ili2Parser::ClassOrStructureDefContext *context) = 0;

    virtual antlrcpp::Any visitAttributeDef(Ili2Parser::AttributeDefContext *context) = 0;

    virtual antlrcpp::Any visitAttrTypeDef(Ili2Parser::AttrTypeDefContext *context) = 0;

    virtual antlrcpp::Any visitBagOrListType(Ili2Parser::BagOrListTypeContext *context) = 0;

    virtual antlrcpp::Any visitAttrType(Ili2Parser::AttrTypeContext *context) = 0;

    virtual antlrcpp::Any visitReferenceAttr(Ili2Parser::ReferenceAttrContext *context) = 0;

    virtual antlrcpp::Any visitRestrictedRef(Ili2Parser::RestrictedRefContext *context) = 0;

    virtual antlrcpp::Any visitRestriction(Ili2Parser::RestrictionContext *context) = 0;

    virtual antlrcpp::Any visitAssociationDef(Ili2Parser::AssociationDefContext *context) = 0;

    virtual antlrcpp::Any visitAssociationRef(Ili2Parser::AssociationRefContext *context) = 0;

    virtual antlrcpp::Any visitRoleDef(Ili2Parser::RoleDefContext *context) = 0;

    virtual antlrcpp::Any visitCardinality(Ili2Parser::CardinalityContext *context) = 0;

    virtual antlrcpp::Any visitDomainDef(Ili2Parser::DomainDefContext *context) = 0;

    virtual antlrcpp::Any visitDomainType(Ili2Parser::DomainTypeContext *context) = 0;

    virtual antlrcpp::Any visitType(Ili2Parser::TypeContext *context) = 0;

    virtual antlrcpp::Any visitBaseType(Ili2Parser::BaseTypeContext *context) = 0;

    virtual antlrcpp::Any visitConstant(Ili2Parser::ConstantContext *context) = 0;

    virtual antlrcpp::Any visitTextType(Ili2Parser::TextTypeContext *context) = 0;

    virtual antlrcpp::Any visitTextConst(Ili2Parser::TextConstContext *context) = 0;

    virtual antlrcpp::Any visitEnumerationType(Ili2Parser::EnumerationTypeContext *context) = 0;

    virtual antlrcpp::Any visitEnumTreeValueType(Ili2Parser::EnumTreeValueTypeContext *context) = 0;

    virtual antlrcpp::Any visitAlignmentType(Ili2Parser::AlignmentTypeContext *context) = 0;

    virtual antlrcpp::Any visitBooleanType(Ili2Parser::BooleanTypeContext *context) = 0;

    virtual antlrcpp::Any visitEnumeration(Ili2Parser::EnumerationContext *context) = 0;

    virtual antlrcpp::Any visitEnumElement(Ili2Parser::EnumElementContext *context) = 0;

    virtual antlrcpp::Any visitEnumConst(Ili2Parser::EnumConstContext *context) = 0;

    virtual antlrcpp::Any visitNumericType(Ili2Parser::NumericTypeContext *context) = 0;

    virtual antlrcpp::Any visitDateTimeType(Ili2Parser::DateTimeTypeContext *context) = 0;

    virtual antlrcpp::Any visitRefSys(Ili2Parser::RefSysContext *context) = 0;

    virtual antlrcpp::Any visitDecConst(Ili2Parser::DecConstContext *context) = 0;

    virtual antlrcpp::Any visitNumericConst(Ili2Parser::NumericConstContext *context) = 0;

    virtual antlrcpp::Any visitFormattedType(Ili2Parser::FormattedTypeContext *context) = 0;

    virtual antlrcpp::Any visitFormatDef(Ili2Parser::FormatDefContext *context) = 0;

    virtual antlrcpp::Any visitBaseAttrRef(Ili2Parser::BaseAttrRefContext *context) = 0;

    virtual antlrcpp::Any visitFormattedConst(Ili2Parser::FormattedConstContext *context) = 0;

    virtual antlrcpp::Any visitContextDef(Ili2Parser::ContextDefContext *context) = 0;

    virtual antlrcpp::Any visitContextDecl(Ili2Parser::ContextDeclContext *context) = 0;

    virtual antlrcpp::Any visitCoordinateType(Ili2Parser::CoordinateTypeContext *context) = 0;

    virtual antlrcpp::Any visitRotationDef(Ili2Parser::RotationDefContext *context) = 0;

    virtual antlrcpp::Any visitOIDType(Ili2Parser::OIDTypeContext *context) = 0;

    virtual antlrcpp::Any visitBlackboxType(Ili2Parser::BlackboxTypeContext *context) = 0;

    virtual antlrcpp::Any visitClassRefType(Ili2Parser::ClassRefTypeContext *context) = 0;

    virtual antlrcpp::Any visitAttributePathType(Ili2Parser::AttributePathTypeContext *context) = 0;

    virtual antlrcpp::Any visitClassConst(Ili2Parser::ClassConstContext *context) = 0;

    virtual antlrcpp::Any visitAttributePathConst(Ili2Parser::AttributePathConstContext *context) = 0;

    virtual antlrcpp::Any visitLineType(Ili2Parser::LineTypeContext *context) = 0;

    virtual antlrcpp::Any visitLineForm(Ili2Parser::LineFormContext *context) = 0;

    virtual antlrcpp::Any visitLineFormType(Ili2Parser::LineFormTypeContext *context) = 0;

    virtual antlrcpp::Any visitLineFormTypeDef(Ili2Parser::LineFormTypeDefContext *context) = 0;

    virtual antlrcpp::Any visitLineFormTypeDecl(Ili2Parser::LineFormTypeDeclContext *context) = 0;

    virtual antlrcpp::Any visitUnitDecl(Ili2Parser::UnitDeclContext *context) = 0;

    virtual antlrcpp::Any visitUnitDef(Ili2Parser::UnitDefContext *context) = 0;

    virtual antlrcpp::Any visitDerivedUnit(Ili2Parser::DerivedUnitContext *context) = 0;

    virtual antlrcpp::Any visitComposedUnit(Ili2Parser::ComposedUnitContext *context) = 0;

    virtual antlrcpp::Any visitComposedUnitExpr(Ili2Parser::ComposedUnitExprContext *context) = 0;

    virtual antlrcpp::Any visitMetaDataBasketDef(Ili2Parser::MetaDataBasketDefContext *context) = 0;

    virtual antlrcpp::Any visitMetaDataBasketRef(Ili2Parser::MetaDataBasketRefContext *context) = 0;

    virtual antlrcpp::Any visitMetaObjectRef(Ili2Parser::MetaObjectRefContext *context) = 0;

    virtual antlrcpp::Any visitParameterDef(Ili2Parser::ParameterDefContext *context) = 0;

    virtual antlrcpp::Any visitProperties(Ili2Parser::PropertiesContext *context) = 0;

    virtual antlrcpp::Any visitRunTimeParameterDef(Ili2Parser::RunTimeParameterDefContext *context) = 0;

    virtual antlrcpp::Any visitRunTimeParameter(Ili2Parser::RunTimeParameterContext *context) = 0;

    virtual antlrcpp::Any visitConstraintDef(Ili2Parser::ConstraintDefContext *context) = 0;

    virtual antlrcpp::Any visitMandatoryConstraint(Ili2Parser::MandatoryConstraintContext *context) = 0;

    virtual antlrcpp::Any visitPlausibilityConstraint(Ili2Parser::PlausibilityConstraintContext *context) = 0;

    virtual antlrcpp::Any visitExistenceConstraint(Ili2Parser::ExistenceConstraintContext *context) = 0;

    virtual antlrcpp::Any visitUniquenessConstraint(Ili2Parser::UniquenessConstraintContext *context) = 0;

    virtual antlrcpp::Any visitGlobalUniqueness(Ili2Parser::GlobalUniquenessContext *context) = 0;

    virtual antlrcpp::Any visitUniqueEl(Ili2Parser::UniqueElContext *context) = 0;

    virtual antlrcpp::Any visitLocalUniqueness(Ili2Parser::LocalUniquenessContext *context) = 0;

    virtual antlrcpp::Any visitLocalUniqueEl(Ili2Parser::LocalUniqueElContext *context) = 0;

    virtual antlrcpp::Any visitSetConstraint(Ili2Parser::SetConstraintContext *context) = 0;

    virtual antlrcpp::Any visitConstraintsDef(Ili2Parser::ConstraintsDefContext *context) = 0;

    virtual antlrcpp::Any visitExpression(Ili2Parser::ExpressionContext *context) = 0;

    virtual antlrcpp::Any visitTerm1(Ili2Parser::Term1Context *context) = 0;

    virtual antlrcpp::Any visitOperator1(Ili2Parser::Operator1Context *context) = 0;

    virtual antlrcpp::Any visitTerm2(Ili2Parser::Term2Context *context) = 0;

    virtual antlrcpp::Any visitOperator2(Ili2Parser::Operator2Context *context) = 0;

    virtual antlrcpp::Any visitTerm3(Ili2Parser::Term3Context *context) = 0;

    virtual antlrcpp::Any visitTerm(Ili2Parser::TermContext *context) = 0;

    virtual antlrcpp::Any visitRelation(Ili2Parser::RelationContext *context) = 0;

    virtual antlrcpp::Any visitFactor(Ili2Parser::FactorContext *context) = 0;

    virtual antlrcpp::Any visitObjectOrAttributePath(Ili2Parser::ObjectOrAttributePathContext *context) = 0;

    virtual antlrcpp::Any visitAttributePath(Ili2Parser::AttributePathContext *context) = 0;

    virtual antlrcpp::Any visitPathEl(Ili2Parser::PathElContext *context) = 0;

    virtual antlrcpp::Any visitObjectRef(Ili2Parser::ObjectRefContext *context) = 0;

    virtual antlrcpp::Any visitFunctionCall(Ili2Parser::FunctionCallContext *context) = 0;

    virtual antlrcpp::Any visitFunctionCallArgument(Ili2Parser::FunctionCallArgumentContext *context) = 0;

    virtual antlrcpp::Any visitFunctionDef(Ili2Parser::FunctionDefContext *context) = 0;

    virtual antlrcpp::Any visitFunctionDefParam(Ili2Parser::FunctionDefParamContext *context) = 0;

    virtual antlrcpp::Any visitArgumentType(Ili2Parser::ArgumentTypeContext *context) = 0;

    virtual antlrcpp::Any visitViewDef(Ili2Parser::ViewDefContext *context) = 0;

    virtual antlrcpp::Any visitFormationDef(Ili2Parser::FormationDefContext *context) = 0;

    virtual antlrcpp::Any visitProjection(Ili2Parser::ProjectionContext *context) = 0;

    virtual antlrcpp::Any visitJoin(Ili2Parser::JoinContext *context) = 0;

    virtual antlrcpp::Any visitIliunion(Ili2Parser::IliunionContext *context) = 0;

    virtual antlrcpp::Any visitAggregation(Ili2Parser::AggregationContext *context) = 0;

    virtual antlrcpp::Any visitInspection(Ili2Parser::InspectionContext *context) = 0;

    virtual antlrcpp::Any visitRenamedViewableRef(Ili2Parser::RenamedViewableRefContext *context) = 0;

    virtual antlrcpp::Any visitBaseExtensionDef(Ili2Parser::BaseExtensionDefContext *context) = 0;

    virtual antlrcpp::Any visitSelection(Ili2Parser::SelectionContext *context) = 0;

    virtual antlrcpp::Any visitViewAttribute(Ili2Parser::ViewAttributeContext *context) = 0;

    virtual antlrcpp::Any visitGraphicDef(Ili2Parser::GraphicDefContext *context) = 0;

    virtual antlrcpp::Any visitDrawingRule(Ili2Parser::DrawingRuleContext *context) = 0;

    virtual antlrcpp::Any visitCondSignParamAssignment(Ili2Parser::CondSignParamAssignmentContext *context) = 0;

    virtual antlrcpp::Any visitSignParamAssignment(Ili2Parser::SignParamAssignmentContext *context) = 0;

    virtual antlrcpp::Any visitEnumAssignment(Ili2Parser::EnumAssignmentContext *context) = 0;

    virtual antlrcpp::Any visitEnumRange(Ili2Parser::EnumRangeContext *context) = 0;


};

}  // namespace parser
