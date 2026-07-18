
// Generated from Ili2Parser.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"


namespace parser {


class  Ili2Parser : public antlr4::Parser {
public:
  enum {
    SL_COMMENT = 1, ILI_OPTION = 2, ILI_BLOCKCOMMENT = 3, WS = 4, NL = 5, 
    ILI_DOC = 6, INTERLIS = 7, ILI23 = 8, ILI24 = 9, CONTRACTED = 10, REFSYSTEM = 11, 
    SYMBOLOGY = 12, TYPE = 13, MODEL = 14, END = 15, VERSION = 16, NOINCREMENTALTRANSFER = 17, 
    TRANSLATION = 18, OF = 19, ATT = 20, CHARSET = 21, XMLNS = 22, IMPORTS = 23, 
    UNQUALIFIED = 24, TOPIC = 25, DEPENDS = 26, ON = 27, AS = 28, VIEW = 29, 
    EXTENDS = 30, BASKET = 31, OID = 32, BOOLEAN = 33, HALIGNMENT = 34, 
    VALIGNMENT = 35, CLASS = 36, ABSTRACT = 37, EXTENDED = 38, FINAL = 39, 
    TRANSIENT = 40, CONTINUOUS = 41, SUBDIVISION = 42, NO = 43, ATTRIBUTE = 44, 
    TEXT = 45, DATE = 46, TIMEOFDAY = 47, DATETIME = 48, LPAREN = 49, RPAREN = 50, 
    LBRACE = 51, RBRACE = 52, LCURLY = 53, RCURLY = 54, STAR = 55, SLASH = 56, 
    BACKSLASH = 57, PERCENT = 58, AT = 59, HASH = 60, TILDE = 61, LESS = 62, 
    LESSEQUAL = 63, IMPL = 64, GREATERGREATER = 65, GREATER = 66, GREATEREQUAL = 67, 
    SEMI = 68, EQUAL = 69, EQUAL_CONST = 70, EQUALEQUAL = 71, LESSGREATER = 72, 
    NOTEQUAL = 73, COLONEQUAL = 74, DOT = 75, DOTDOT = 76, COLON = 77, COMMA = 78, 
    LARROW = 79, RARROW = 80, PLUS = 81, MINUS = 82, AGGREGATE = 83, ASSOCIATE = 84, 
    COMPOSITE = 85, STRUCTURE = 86, PARAMETER = 87, MANDATORY = 88, GENERIC = 89, 
    GENERICS = 90, DEFERRED = 91, CONTEXT = 92, MULTICOORD = 93, MULTIPOLYLINE = 94, 
    MULTISURFACE = 95, MULTIAREA = 96, BAG = 97, LIST = 98, REFERENCE = 99, 
    TO = 100, ANYCLASS = 101, RESTRICTION = 102, ASSOCIATION = 103, DERIVED = 104, 
    EXTERNAL = 105, FROM = 106, UNDEFINED = 107, MTEXT = 108, NAME_CONST = 109, 
    URI = 110, ALL = 111, CIRCULAR = 112, OTHERS = 113, NUMERIC = 114, CLOCKWISE = 115, 
    COUNTERCLOCKWISE = 116, CARDINALITY = 117, OR = 118, HIDING = 119, ORDERED = 120, 
    ILIDOMAIN = 121, PI = 122, LNBASE = 123, FORMAT = 124, INHERIT = 125, 
    COORD = 126, ROTATION = 127, ANY = 128, BLACKBOX = 129, XML = 130, BINARY = 131, 
    DIRECTED = 132, POLYLINE = 133, SURFACE = 134, AREA = 135, WITH = 136, 
    STRAIGHTS = 137, ARCS = 138, VERTEX = 139, WITHOUT = 140, OVERLAPS = 141, 
    LINE = 142, ATTRIBUTES = 143, FORM = 144, UNIT = 145, FUNCTION = 146, 
    SIGN = 147, OBJECTS = 148, METAOBJECT = 149, CONSTRAINT = 150, CONSTRAINTS = 151, 
    EXISTENCE = 152, REQUIRED = 153, IN = 154, UNIQUE = 155, WHERE = 156, 
    LOCAL = 157, SET = 158, AND = 159, NOT = 160, BASED = 161, BASE = 162, 
    INHERITANCE = 163, DEFINED = 164, INSPECTION = 165, THIS = 166, THISAREA = 167, 
    THATAREA = 168, PARENT = 169, FIRST = 170, LAST = 171, AGGREGATES = 172, 
    OBJECT = 173, ENUMVAL = 174, ENUMTREEVAL = 175, PROJECTION = 176, JOIN = 177, 
    ILINULL = 178, UNION = 179, AGGREGATION = 180, BY = 181, GRAPHIC = 182, 
    ACCORDING = 183, WHEN = 184, ANYSTRUCTURE = 185, TRANSFER = 186, EXPLANATION = 187, 
    ESC = 188, STRING = 189, POSNUMBER = 190, NUMBER = 191, DEC = 192, DECIMAL = 193, 
    HEXNUMBER = 194, NAME = 195
  };

  enum {
    RuleDecimal = 0, RulePath = 1, RuleInterlis2Def = 2, RuleModelDef = 3, 
    RuleImportDef = 4, RuleImporting = 5, RuleTopicDef = 6, RuleTopicPath = 7, 
    RuleDeferredGenerics = 8, RuleClassDef = 9, RuleStructureDef = 10, RuleClassOrStructureDef = 11, 
    RuleAttributeDef = 12, RuleAttrTypeDef = 13, RuleBagOrListType = 14, 
    RuleAttrType = 15, RuleReferenceAttr = 16, RuleRestrictedRef = 17, RuleRestriction = 18, 
    RuleAssociationDef = 19, RuleAssociationRef = 20, RuleRoleDef = 21, 
    RuleCardinality = 22, RuleDomainDef = 23, RuleDomainType = 24, RuleType = 25, 
    RuleBaseType = 26, RuleConstant = 27, RuleTextType = 28, RuleTextConst = 29, 
    RuleEnumerationType = 30, RuleEnumTreeValueType = 31, RuleAlignmentType = 32, 
    RuleBooleanType = 33, RuleEnumeration = 34, RuleEnumElement = 35, RuleEnumConst = 36, 
    RuleNumericType = 37, RuleDateTimeType = 38, RuleRefSys = 39, RuleDecConst = 40, 
    RuleNumericConst = 41, RuleFormattedType = 42, RuleFormatDef = 43, RuleBaseAttrRef = 44, 
    RuleFormattedConst = 45, RuleContextDef = 46, RuleContextBlock = 47, 
    RuleContextDecl = 48, RuleCoordinateType = 49, RuleRotationDef = 50, 
    RuleOIDType = 51, RuleBlackboxType = 52, RuleClassRefType = 53, RuleAttributePathType = 54, 
    RuleClassConst = 55, RuleAttributePathConst = 56, RuleLineType = 57, 
    RuleLineForm = 58, RuleLineFormType = 59, RuleLineFormTypeDef = 60, 
    RuleLineFormTypeDecl = 61, RuleUnitDecl = 62, RuleUnitDef = 63, RuleDerivedUnit = 64, 
    RuleComposedUnit = 65, RuleComposedUnitExpr = 66, RuleMetaDataBasketDef = 67, 
    RuleMetaDataBasketRef = 68, RuleMetaObjectRef = 69, RuleParameterDef = 70, 
    RuleProperties = 71, RuleRunTimeParameterDef = 72, RuleRunTimeParameter = 73, 
    RuleConstraintDef = 74, RuleMandatoryConstraint = 75, RulePlausibilityConstraint = 76, 
    RuleExistenceConstraint = 77, RuleUniquenessConstraint = 78, RuleGlobalUniqueness = 79, 
    RuleUniqueEl = 80, RuleLocalUniqueness = 81, RuleLocalUniqueEl = 82, 
    RuleSetConstraint = 83, RuleConstraintsDef = 84, RuleExpression = 85, 
    RuleTerm1 = 86, RuleOperator1 = 87, RuleTerm2 = 88, RuleOperator2 = 89, 
    RuleTerm3 = 90, RuleTerm = 91, RuleRelation = 92, RuleFactor = 93, RuleObjectOrAttributePath = 94, 
    RuleAttributePath = 95, RulePathEl = 96, RuleObjectRef = 97, RuleFunctionCall = 98, 
    RuleFunctionCallArgument = 99, RuleFunctionDef = 100, RuleFunctionDefParam = 101, 
    RuleArgumentType = 102, RuleViewDef = 103, RuleFormationDef = 104, RuleProjection = 105, 
    RuleJoin = 106, RuleIliunion = 107, RuleAggregation = 108, RuleInspection = 109, 
    RuleRenamedViewableRef = 110, RuleBaseExtensionDef = 111, RuleSelection = 112, 
    RuleViewAttribute = 113, RuleGraphicDef = 114, RuleDrawingRule = 115, 
    RuleCondSignParamAssignment = 116, RuleSignParamAssignment = 117, RuleEnumAssignment = 118, 
    RuleEnumRange = 119
  };

  Ili2Parser(antlr4::TokenStream *input);
  ~Ili2Parser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


     bool ili23=false; 
     bool ili24=false;


  class DecimalContext;
  class PathContext;
  class Interlis2DefContext;
  class ModelDefContext;
  class ImportDefContext;
  class ImportingContext;
  class TopicDefContext;
  class TopicPathContext;
  class DeferredGenericsContext;
  class ClassDefContext;
  class StructureDefContext;
  class ClassOrStructureDefContext;
  class AttributeDefContext;
  class AttrTypeDefContext;
  class BagOrListTypeContext;
  class AttrTypeContext;
  class ReferenceAttrContext;
  class RestrictedRefContext;
  class RestrictionContext;
  class AssociationDefContext;
  class AssociationRefContext;
  class RoleDefContext;
  class CardinalityContext;
  class DomainDefContext;
  class DomainTypeContext;
  class TypeContext;
  class BaseTypeContext;
  class ConstantContext;
  class TextTypeContext;
  class TextConstContext;
  class EnumerationTypeContext;
  class EnumTreeValueTypeContext;
  class AlignmentTypeContext;
  class BooleanTypeContext;
  class EnumerationContext;
  class EnumElementContext;
  class EnumConstContext;
  class NumericTypeContext;
  class DateTimeTypeContext;
  class RefSysContext;
  class DecConstContext;
  class NumericConstContext;
  class FormattedTypeContext;
  class FormatDefContext;
  class BaseAttrRefContext;
  class FormattedConstContext;
  class ContextDefContext;
  class ContextBlockContext;
  class ContextDeclContext;
  class CoordinateTypeContext;
  class RotationDefContext;
  class OIDTypeContext;
  class BlackboxTypeContext;
  class ClassRefTypeContext;
  class AttributePathTypeContext;
  class ClassConstContext;
  class AttributePathConstContext;
  class LineTypeContext;
  class LineFormContext;
  class LineFormTypeContext;
  class LineFormTypeDefContext;
  class LineFormTypeDeclContext;
  class UnitDeclContext;
  class UnitDefContext;
  class DerivedUnitContext;
  class ComposedUnitContext;
  class ComposedUnitExprContext;
  class MetaDataBasketDefContext;
  class MetaDataBasketRefContext;
  class MetaObjectRefContext;
  class ParameterDefContext;
  class PropertiesContext;
  class RunTimeParameterDefContext;
  class RunTimeParameterContext;
  class ConstraintDefContext;
  class MandatoryConstraintContext;
  class PlausibilityConstraintContext;
  class ExistenceConstraintContext;
  class UniquenessConstraintContext;
  class GlobalUniquenessContext;
  class UniqueElContext;
  class LocalUniquenessContext;
  class LocalUniqueElContext;
  class SetConstraintContext;
  class ConstraintsDefContext;
  class ExpressionContext;
  class Term1Context;
  class Operator1Context;
  class Term2Context;
  class Operator2Context;
  class Term3Context;
  class TermContext;
  class RelationContext;
  class FactorContext;
  class ObjectOrAttributePathContext;
  class AttributePathContext;
  class PathElContext;
  class ObjectRefContext;
  class FunctionCallContext;
  class FunctionCallArgumentContext;
  class FunctionDefContext;
  class FunctionDefParamContext;
  class ArgumentTypeContext;
  class ViewDefContext;
  class FormationDefContext;
  class ProjectionContext;
  class JoinContext;
  class IliunionContext;
  class AggregationContext;
  class InspectionContext;
  class RenamedViewableRefContext;
  class BaseExtensionDefContext;
  class SelectionContext;
  class ViewAttributeContext;
  class GraphicDefContext;
  class DrawingRuleContext;
  class CondSignParamAssignmentContext;
  class SignParamAssignmentContext;
  class EnumAssignmentContext;
  class EnumRangeContext; 

  class  DecimalContext : public antlr4::ParserRuleContext {
  public:
    DecimalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DEC();
    antlr4::tree::TerminalNode *POSNUMBER();
    antlr4::tree::TerminalNode *NUMBER();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DecimalContext* decimal();

  class  PathContext : public antlr4::ParserRuleContext {
  public:
    PathContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SIGN();
    antlr4::tree::TerminalNode *URI();
    antlr4::tree::TerminalNode *REFSYSTEM();
    antlr4::tree::TerminalNode *BOOLEAN();
    antlr4::tree::TerminalNode *HALIGNMENT();
    antlr4::tree::TerminalNode *VALIGNMENT();
    antlr4::tree::TerminalNode *METAOBJECT();
    antlr4::tree::TerminalNode *INTERLIS();
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PathContext* path();

  class  Interlis2DefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *iliversion = nullptr;;
    Interlis2DefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTERLIS();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *EOF();
    antlr4::tree::TerminalNode *ILI23();
    antlr4::tree::TerminalNode *ILI24();
    std::vector<ModelDefContext *> modelDef();
    ModelDefContext* modelDef(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interlis2DefContext* interlis2Def();

  class  ModelDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *contracted = nullptr;;
    antlr4::Token *refsystemmodel = nullptr;;
    antlr4::Token *symbologymodel = nullptr;;
    antlr4::Token *typemodel = nullptr;;
    antlr4::Token *modelname1 = nullptr;;
    antlr4::Token *language = nullptr;;
    antlr4::Token *issuerurl = nullptr;;
    antlr4::Token *modelversion = nullptr;;
    antlr4::Token *modelversion_expl = nullptr;;
    antlr4::Token *translationOf = nullptr;;
    antlr4::Token *translationOfVersion = nullptr;;
    antlr4::Token *iananame = nullptr;;
    antlr4::Token *xmlns = nullptr;;
    antlr4::Token *modelname2 = nullptr;;
    ModelDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MODEL();
    antlr4::tree::TerminalNode *ATT();
    antlr4::tree::TerminalNode *VERSION();
    antlr4::tree::TerminalNode *EQUAL();
    antlr4::tree::TerminalNode *END();
    antlr4::tree::TerminalNode *DOT();
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *NOINCREMENTALTRANSFER();
    antlr4::tree::TerminalNode *TRANSLATION();
    antlr4::tree::TerminalNode *OF();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *CHARSET();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    antlr4::tree::TerminalNode *XMLNS();
    std::vector<ImportDefContext *> importDef();
    ImportDefContext* importDef(size_t i);
    std::vector<MetaDataBasketDefContext *> metaDataBasketDef();
    MetaDataBasketDefContext* metaDataBasketDef(size_t i);
    std::vector<UnitDeclContext *> unitDecl();
    UnitDeclContext* unitDecl(size_t i);
    std::vector<FunctionDefContext *> functionDef();
    FunctionDefContext* functionDef(size_t i);
    std::vector<LineFormTypeDefContext *> lineFormTypeDef();
    LineFormTypeDefContext* lineFormTypeDef(size_t i);
    std::vector<DomainDefContext *> domainDef();
    DomainDefContext* domainDef(size_t i);
    std::vector<ContextDefContext *> contextDef();
    ContextDefContext* contextDef(size_t i);
    std::vector<RunTimeParameterDefContext *> runTimeParameterDef();
    RunTimeParameterDefContext* runTimeParameterDef(size_t i);
    std::vector<ClassDefContext *> classDef();
    ClassDefContext* classDef(size_t i);
    std::vector<StructureDefContext *> structureDef();
    StructureDefContext* structureDef(size_t i);
    std::vector<TopicDefContext *> topicDef();
    TopicDefContext* topicDef(size_t i);
    antlr4::tree::TerminalNode *CONTRACTED();
    antlr4::tree::TerminalNode *REFSYSTEM();
    antlr4::tree::TerminalNode *SYMBOLOGY();
    antlr4::tree::TerminalNode *TYPE();
    antlr4::tree::TerminalNode *EXPLANATION();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ModelDefContext* modelDef();

  class  ImportDefContext : public antlr4::ParserRuleContext {
  public:
    ImportDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IMPORTS();
    std::vector<ImportingContext *> importing();
    ImportingContext* importing(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportDefContext* importDef();

  class  ImportingContext : public antlr4::ParserRuleContext {
  public:
    ImportingContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INTERLIS();
    antlr4::tree::TerminalNode *NAME();
    antlr4::tree::TerminalNode *UNQUALIFIED();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportingContext* importing();

  class  TopicDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *topicname1 = nullptr;;
    Ili2Parser::PathContext *topicbase = nullptr;;
    Ili2Parser::PathContext *basketOid = nullptr;;
    Ili2Parser::PathContext *topicOid = nullptr;;
    antlr4::Token *topicname2 = nullptr;;
    TopicDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *TOPIC();
    antlr4::tree::TerminalNode *EQUAL();
    antlr4::tree::TerminalNode *END();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    antlr4::tree::TerminalNode *VIEW();
    PropertiesContext *properties();
    antlr4::tree::TerminalNode *EXTENDS();
    antlr4::tree::TerminalNode *BASKET();
    std::vector<antlr4::tree::TerminalNode *> OID();
    antlr4::tree::TerminalNode* OID(size_t i);
    std::vector<antlr4::tree::TerminalNode *> AS();
    antlr4::tree::TerminalNode* AS(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DEPENDS();
    antlr4::tree::TerminalNode* DEPENDS(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ON();
    antlr4::tree::TerminalNode* ON(size_t i);
    std::vector<TopicPathContext *> topicPath();
    TopicPathContext* topicPath(size_t i);
    DeferredGenericsContext *deferredGenerics();
    std::vector<MetaDataBasketDefContext *> metaDataBasketDef();
    MetaDataBasketDefContext* metaDataBasketDef(size_t i);
    std::vector<UnitDeclContext *> unitDecl();
    UnitDeclContext* unitDecl(size_t i);
    std::vector<FunctionDefContext *> functionDef();
    FunctionDefContext* functionDef(size_t i);
    std::vector<DomainDefContext *> domainDef();
    DomainDefContext* domainDef(size_t i);
    std::vector<ClassDefContext *> classDef();
    ClassDefContext* classDef(size_t i);
    std::vector<StructureDefContext *> structureDef();
    StructureDefContext* structureDef(size_t i);
    std::vector<AssociationDefContext *> associationDef();
    AssociationDefContext* associationDef(size_t i);
    std::vector<ConstraintsDefContext *> constraintsDef();
    ConstraintsDefContext* constraintsDef(size_t i);
    std::vector<ViewDefContext *> viewDef();
    ViewDefContext* viewDef(size_t i);
    std::vector<GraphicDefContext *> graphicDef();
    GraphicDefContext* graphicDef(size_t i);
    std::vector<PathContext *> path();
    PathContext* path(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TopicDefContext* topicDef();

  class  TopicPathContext : public antlr4::ParserRuleContext {
  public:
    TopicPathContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TopicPathContext* topicPath();

  class  DeferredGenericsContext : public antlr4::ParserRuleContext {
  public:
    DeferredGenericsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DEFERRED();
    antlr4::tree::TerminalNode *GENERICS();
    std::vector<PathContext *> path();
    PathContext* path(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DeferredGenericsContext* deferredGenerics();

  class  ClassDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *classname1 = nullptr;;
    Ili2Parser::PathContext *classbase = nullptr;;
    Ili2Parser::PathContext *classoid = nullptr;;
    antlr4::Token *classname2 = nullptr;;
    ClassDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CLASS();
    antlr4::tree::TerminalNode *EQUAL();
    ClassOrStructureDefContext *classOrStructureDef();
    antlr4::tree::TerminalNode *END();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    PropertiesContext *properties();
    antlr4::tree::TerminalNode *EXTENDS();
    std::vector<PathContext *> path();
    PathContext* path(size_t i);
    antlr4::tree::TerminalNode *OID();
    antlr4::tree::TerminalNode *AS();
    antlr4::tree::TerminalNode *NO();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ClassDefContext* classDef();

  class  StructureDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *structurename1 = nullptr;;
    Ili2Parser::PathContext *structurebase = nullptr;;
    antlr4::Token *structurename2 = nullptr;;
    StructureDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRUCTURE();
    antlr4::tree::TerminalNode *EQUAL();
    ClassOrStructureDefContext *classOrStructureDef();
    antlr4::tree::TerminalNode *END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    PropertiesContext *properties();
    antlr4::tree::TerminalNode *EXTENDS();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  StructureDefContext* structureDef();

  class  ClassOrStructureDefContext : public antlr4::ParserRuleContext {
  public:
    ClassOrStructureDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ATTRIBUTE();
    std::vector<AttributeDefContext *> attributeDef();
    AttributeDefContext* attributeDef(size_t i);
    std::vector<ConstraintDefContext *> constraintDef();
    ConstraintDefContext* constraintDef(size_t i);
    antlr4::tree::TerminalNode *PARAMETER();
    std::vector<ParameterDefContext *> parameterDef();
    ParameterDefContext* parameterDef(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ClassOrStructureDefContext* classOrStructureDef();

  class  AttributeDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *attributname = nullptr;;
    AttributeDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    AttrTypeDefContext *attrTypeDef();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *NAME();
    antlr4::tree::TerminalNode *SUBDIVISION();
    PropertiesContext *properties();
    antlr4::tree::TerminalNode *COLONEQUAL();
    std::vector<FactorContext *> factor();
    FactorContext* factor(size_t i);
    antlr4::tree::TerminalNode *CONTINUOUS();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttributeDefContext* attributeDef();

  class  AttrTypeDefContext : public antlr4::ParserRuleContext {
  public:
    AttrTypeDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MANDATORY();
    AttrTypeContext *attrType();
    BagOrListTypeContext *bagOrListType();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttrTypeDefContext* attrTypeDef();

  class  BagOrListTypeContext : public antlr4::ParserRuleContext {
  public:
    BagOrListTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OF();
    antlr4::tree::TerminalNode *BAG();
    antlr4::tree::TerminalNode *LIST();
    RestrictedRefContext *restrictedRef();
    AttrTypeContext *attrType();
    CardinalityContext *cardinality();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BagOrListTypeContext* bagOrListType();

  class  AttrTypeContext : public antlr4::ParserRuleContext {
  public:
    AttrTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TypeContext *type();
    PathContext *path();
    ReferenceAttrContext *referenceAttr();
    RestrictedRefContext *restrictedRef();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttrTypeContext* attrType();

  class  ReferenceAttrContext : public antlr4::ParserRuleContext {
  public:
    ReferenceAttrContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *REFERENCE();
    antlr4::tree::TerminalNode *TO();
    RestrictedRefContext *restrictedRef();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *EXTERNAL();
    antlr4::tree::TerminalNode *RPAREN();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ReferenceAttrContext* referenceAttr();

  class  RestrictedRefContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *typeref = nullptr;;
    RestrictedRefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ANYCLASS();
    antlr4::tree::TerminalNode *ANYSTRUCTURE();
    PathContext *path();
    RestrictionContext *restriction();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RestrictedRefContext* restrictedRef();

  class  RestrictionContext : public antlr4::ParserRuleContext {
  public:
    RestrictionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *RESTRICTION();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<PathContext *> path();
    PathContext* path(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RestrictionContext* restriction();

  class  AssociationDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *associationname1 = nullptr;;
    Ili2Parser::PathContext *assocoid = nullptr;;
    antlr4::Token *associationname2 = nullptr;;
    AssociationDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ASSOCIATION();
    std::vector<antlr4::tree::TerminalNode *> EQUAL();
    antlr4::tree::TerminalNode* EQUAL(size_t i);
    antlr4::tree::TerminalNode *END();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    PropertiesContext *properties();
    antlr4::tree::TerminalNode *EXTENDS();
    AssociationRefContext *associationRef();
    antlr4::tree::TerminalNode *DERIVED();
    antlr4::tree::TerminalNode *FROM();
    RenamedViewableRefContext *renamedViewableRef();
    std::vector<RoleDefContext *> roleDef();
    RoleDefContext* roleDef(size_t i);
    antlr4::tree::TerminalNode *ATTRIBUTE();
    std::vector<AttributeDefContext *> attributeDef();
    AttributeDefContext* attributeDef(size_t i);
    antlr4::tree::TerminalNode *CARDINALITY();
    CardinalityContext *cardinality();
    std::vector<ConstraintDefContext *> constraintDef();
    ConstraintDefContext* constraintDef(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    antlr4::tree::TerminalNode *OID();
    antlr4::tree::TerminalNode *AS();
    antlr4::tree::TerminalNode *NO();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AssociationDefContext* associationDef();

  class  AssociationRefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *modelname = nullptr;;
    antlr4::Token *topicname = nullptr;;
    antlr4::Token *associationname = nullptr;;
    AssociationRefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AssociationRefContext* associationRef();

  class  RoleDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *rolename = nullptr;;
    Ili2Parser::FactorContext *role = nullptr;;
    RoleDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<RestrictedRefContext *> restrictedRef();
    RestrictedRefContext* restrictedRef(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *NAME();
    antlr4::tree::TerminalNode *ASSOCIATE();
    antlr4::tree::TerminalNode *AGGREGATE();
    antlr4::tree::TerminalNode *COMPOSITE();
    PropertiesContext *properties();
    CardinalityContext *cardinality();
    std::vector<antlr4::tree::TerminalNode *> OR();
    antlr4::tree::TerminalNode* OR(size_t i);
    antlr4::tree::TerminalNode *COLONEQUAL();
    FactorContext *factor();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RoleDefContext* roleDef();

  class  CardinalityContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *min = nullptr;;
    antlr4::Token *max = nullptr;;
    CardinalityContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LCURLY();
    antlr4::tree::TerminalNode *RCURLY();
    antlr4::tree::TerminalNode *STAR();
    std::vector<antlr4::tree::TerminalNode *> POSNUMBER();
    antlr4::tree::TerminalNode* POSNUMBER(size_t i);
    antlr4::tree::TerminalNode *DOTDOT();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CardinalityContext* cardinality();

  class  DomainDefContext : public antlr4::ParserRuleContext {
  public:
    DomainDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ILIDOMAIN();
    std::vector<DomainTypeContext *> domainType();
    DomainTypeContext* domainType(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DomainDefContext* domainDef();

  class  DomainTypeContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *domainname = nullptr;;
    Ili2Parser::PathContext *basedomain = nullptr;;
    DomainTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQUAL();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    antlr4::tree::TerminalNode *MANDATORY();
    TypeContext *type();
    PropertiesContext *properties();
    antlr4::tree::TerminalNode *EXTENDS();
    antlr4::tree::TerminalNode *CONSTRAINTS();
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    PathContext *path();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DomainTypeContext* domainType();

  class  TypeContext : public antlr4::ParserRuleContext {
  public:
    TypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    BaseTypeContext *baseType();
    LineTypeContext *lineType();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TypeContext* type();

  class  BaseTypeContext : public antlr4::ParserRuleContext {
  public:
    BaseTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TextTypeContext *textType();
    EnumerationTypeContext *enumerationType();
    EnumTreeValueTypeContext *enumTreeValueType();
    AlignmentTypeContext *alignmentType();
    BooleanTypeContext *booleanType();
    NumericTypeContext *numericType();
    FormattedTypeContext *formattedType();
    DateTimeTypeContext *dateTimeType();
    CoordinateTypeContext *coordinateType();
    OIDTypeContext *oIDType();
    BlackboxTypeContext *blackboxType();
    ClassRefTypeContext *classRefType();
    AttributePathTypeContext *attributePathType();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BaseTypeContext* baseType();

  class  ConstantContext : public antlr4::ParserRuleContext {
  public:
    ConstantContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UNDEFINED();
    NumericConstContext *numericConst();
    TextConstContext *textConst();
    FormattedConstContext *formattedConst();
    EnumConstContext *enumConst();
    ClassConstContext *classConst();
    AttributePathConstContext *attributePathConst();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstantContext* constant();

  class  TextTypeContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *maxlength = nullptr;;
    TextTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MTEXT();
    antlr4::tree::TerminalNode *STAR();
    antlr4::tree::TerminalNode *POSNUMBER();
    antlr4::tree::TerminalNode *TEXT();
    antlr4::tree::TerminalNode *NAME_CONST();
    antlr4::tree::TerminalNode *URI();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TextTypeContext* textType();

  class  TextConstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *textconst = nullptr;;
    TextConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TextConstContext* textConst();

  class  EnumerationTypeContext : public antlr4::ParserRuleContext {
  public:
    EnumerationTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    EnumerationContext *enumeration();
    antlr4::tree::TerminalNode *ORDERED();
    antlr4::tree::TerminalNode *CIRCULAR();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumerationTypeContext* enumerationType();

  class  EnumTreeValueTypeContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *typeref = nullptr;;
    EnumTreeValueTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ALL();
    antlr4::tree::TerminalNode *OF();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumTreeValueTypeContext* enumTreeValueType();

  class  AlignmentTypeContext : public antlr4::ParserRuleContext {
  public:
    AlignmentTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HALIGNMENT();
    antlr4::tree::TerminalNode *VALIGNMENT();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AlignmentTypeContext* alignmentType();

  class  BooleanTypeContext : public antlr4::ParserRuleContext {
  public:
    BooleanTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BOOLEAN();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BooleanTypeContext* booleanType();

  class  EnumerationContext : public antlr4::ParserRuleContext {
  public:
    EnumerationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<EnumElementContext *> enumElement();
    EnumElementContext* enumElement(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *FINAL();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumerationContext* enumeration();

  class  EnumElementContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::EnumerationContext *sub = nullptr;;
    EnumElementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    EnumerationContext *enumeration();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumElementContext* enumElement();

  class  EnumConstContext : public antlr4::ParserRuleContext {
  public:
    EnumConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *HASH();
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    antlr4::tree::TerminalNode *OTHERS();
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumConstContext* enumConst();

  class  NumericTypeContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::DecimalContext *min = nullptr;;
    Ili2Parser::DecimalContext *max = nullptr;;
    Ili2Parser::PathContext *unitref = nullptr;;
    NumericTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOTDOT();
    antlr4::tree::TerminalNode *NUMERIC();
    std::vector<DecimalContext *> decimal();
    DecimalContext* decimal(size_t i);
    antlr4::tree::TerminalNode *CIRCULAR();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *CLOCKWISE();
    antlr4::tree::TerminalNode *COUNTERCLOCKWISE();
    RefSysContext *refSys();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NumericTypeContext* numericType();

  class  DateTimeTypeContext : public antlr4::ParserRuleContext {
  public:
    DateTimeTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DATE();
    antlr4::tree::TerminalNode *TIMEOFDAY();
    antlr4::tree::TerminalNode *DATETIME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DateTimeTypeContext* dateTimeType();

  class  RefSysContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::MetaObjectRefContext *refsys = nullptr;;
    antlr4::Token *axis = nullptr;;
    Ili2Parser::PathContext *coord = nullptr;;
    RefSysContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LCURLY();
    antlr4::tree::TerminalNode *RCURLY();
    antlr4::tree::TerminalNode *LESS();
    antlr4::tree::TerminalNode *GREATER();
    MetaObjectRefContext *metaObjectRef();
    PathContext *path();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *POSNUMBER();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RefSysContext* refSys();

  class  DecConstContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::DecimalContext *dec = nullptr;;
    DecConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    DecimalContext *decimal();
    antlr4::tree::TerminalNode *PI();
    antlr4::tree::TerminalNode *LNBASE();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DecConstContext* decConst();

  class  NumericConstContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *unitref = nullptr;;
    NumericConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    DecConstContext *decConst();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NumericConstContext* numericConst();

  class  FormattedTypeContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *structref = nullptr;;
    antlr4::Token *min = nullptr;;
    antlr4::Token *max = nullptr;;
    Ili2Parser::PathContext *formatref = nullptr;;
    FormattedTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FORMAT();
    antlr4::tree::TerminalNode *BASED();
    antlr4::tree::TerminalNode *ON();
    FormatDefContext *formatDef();
    PathContext *path();
    antlr4::tree::TerminalNode *DOTDOT();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FormattedTypeContext* formattedType();

  class  FormatDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *nonnumeric = nullptr;;
    FormatDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<BaseAttrRefContext *> baseAttrRef();
    BaseAttrRefContext* baseAttrRef(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *INHERITANCE();
    std::vector<antlr4::tree::TerminalNode *> STRING();
    antlr4::tree::TerminalNode* STRING(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FormatDefContext* formatDef();

  class  BaseAttrRefContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *baseattr = nullptr;;
    BaseAttrRefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NAME();
    antlr4::tree::TerminalNode *SLASH();
    antlr4::tree::TerminalNode *POSNUMBER();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BaseAttrRefContext* baseAttrRef();

  class  FormattedConstContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *formattedconst = nullptr;;
    FormattedConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FormattedConstContext* formattedConst();

  class  ContextDefContext : public antlr4::ParserRuleContext {
  public:
    ContextDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONTEXT();
    std::vector<ContextBlockContext *> contextBlock();
    ContextBlockContext* contextBlock(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ContextDefContext* contextDef();

  class  ContextBlockContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *name = nullptr;;
    ContextBlockContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQUAL();
    antlr4::tree::TerminalNode *NAME();
    std::vector<ContextDeclContext *> contextDecl();
    ContextDeclContext* contextDecl(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ContextBlockContext* contextBlock();

  class  ContextDeclContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *generic = nullptr;;
    ContextDeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQUAL();
    std::vector<PathContext *> path();
    PathContext* path(size_t i);
    std::vector<antlr4::tree::TerminalNode *> OR();
    antlr4::tree::TerminalNode* OR(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ContextDeclContext* contextDecl();

  class  CoordinateTypeContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::NumericTypeContext *numtype1 = nullptr;;
    Ili2Parser::NumericTypeContext *numtype2 = nullptr;;
    Ili2Parser::NumericTypeContext *numtype3 = nullptr;;
    CoordinateTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<NumericTypeContext *> numericType();
    NumericTypeContext* numericType(size_t i);
    antlr4::tree::TerminalNode *COORD();
    antlr4::tree::TerminalNode *MULTICOORD();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    RotationDefContext *rotationDef();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CoordinateTypeContext* coordinateType();

  class  RotationDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *nullaxis = nullptr;;
    antlr4::Token *pihalfaxis = nullptr;;
    RotationDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ROTATION();
    antlr4::tree::TerminalNode *RARROW();
    std::vector<antlr4::tree::TerminalNode *> POSNUMBER();
    antlr4::tree::TerminalNode* POSNUMBER(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RotationDefContext* rotationDef();

  class  OIDTypeContext : public antlr4::ParserRuleContext {
  public:
    OIDTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OID();
    antlr4::tree::TerminalNode *ANY();
    NumericTypeContext *numericType();
    TextTypeContext *textType();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  OIDTypeContext* oIDType();

  class  BlackboxTypeContext : public antlr4::ParserRuleContext {
  public:
    BlackboxTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BLACKBOX();
    antlr4::tree::TerminalNode *XML();
    antlr4::tree::TerminalNode *BINARY();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BlackboxTypeContext* blackboxType();

  class  ClassRefTypeContext : public antlr4::ParserRuleContext {
  public:
    ClassRefTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CLASS();
    RestrictionContext *restriction();
    antlr4::tree::TerminalNode *STRUCTURE();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ClassRefTypeContext* classRefType();

  class  AttributePathTypeContext : public antlr4::ParserRuleContext {
  public:
    AttributePathTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ATTRIBUTE();
    antlr4::tree::TerminalNode *OF();
    antlr4::tree::TerminalNode *RESTRICTION();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<AttrTypeDefContext *> attrTypeDef();
    AttrTypeDefContext* attrTypeDef(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    AttributePathContext *attributePath();
    antlr4::tree::TerminalNode *AT();
    antlr4::tree::TerminalNode *NAME();
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttributePathTypeContext* attributePathType();

  class  ClassConstContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *classref = nullptr;;
    ClassConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GREATER();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ClassConstContext* classConst();

  class  AttributePathConstContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *classref = nullptr;;
    antlr4::Token *attribute = nullptr;;
    AttributePathConstContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GREATERGREATER();
    antlr4::tree::TerminalNode *NAME();
    antlr4::tree::TerminalNode *RARROW();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttributePathConstContext* attributePathConst();

  class  LineTypeContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *directed = nullptr;;
    antlr4::Token *multdir = nullptr;;
    Ili2Parser::PathContext *coordref = nullptr;;
    Ili2Parser::DecimalContext *overlap = nullptr;;
    Ili2Parser::PathContext *lineattrstruct = nullptr;;
    LineTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *POLYLINE();
    antlr4::tree::TerminalNode *SURFACE();
    antlr4::tree::TerminalNode *AREA();
    LineFormContext *lineForm();
    antlr4::tree::TerminalNode *VERTEX();
    antlr4::tree::TerminalNode *WITHOUT();
    antlr4::tree::TerminalNode *OVERLAPS();
    antlr4::tree::TerminalNode *GREATER();
    antlr4::tree::TerminalNode *LINE();
    antlr4::tree::TerminalNode *ATTRIBUTES();
    antlr4::tree::TerminalNode *MULTISURFACE();
    antlr4::tree::TerminalNode *MULTIAREA();
    std::vector<PathContext *> path();
    PathContext* path(size_t i);
    DecimalContext *decimal();
    antlr4::tree::TerminalNode *DIRECTED();
    antlr4::tree::TerminalNode *MULTIPOLYLINE();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LineTypeContext* lineType();

  class  LineFormContext : public antlr4::ParserRuleContext {
  public:
    LineFormContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WITH();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<LineFormTypeContext *> lineFormType();
    LineFormTypeContext* lineFormType(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LineFormContext* lineForm();

  class  LineFormTypeContext : public antlr4::ParserRuleContext {
  public:
    LineFormTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRAIGHTS();
    antlr4::tree::TerminalNode *ARCS();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LineFormTypeContext* lineFormType();

  class  LineFormTypeDefContext : public antlr4::ParserRuleContext {
  public:
    LineFormTypeDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LINE();
    antlr4::tree::TerminalNode *FORM();
    std::vector<LineFormTypeDeclContext *> lineFormTypeDecl();
    LineFormTypeDeclContext* lineFormTypeDecl(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LineFormTypeDefContext* lineFormTypeDef();

  class  LineFormTypeDeclContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *lineformname = nullptr;;
    Ili2Parser::PathContext *structureref = nullptr;;
    LineFormTypeDeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *NAME();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LineFormTypeDeclContext* lineFormTypeDecl();

  class  UnitDeclContext : public antlr4::ParserRuleContext {
  public:
    UnitDeclContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UNIT();
    std::vector<UnitDefContext *> unitDef();
    UnitDefContext* unitDef(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UnitDeclContext* unitDecl();

  class  UnitDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *unitname = nullptr;;
    Ili2Parser::PathContext *unitshort = nullptr;;
    Ili2Parser::PathContext *super = nullptr;;
    UnitDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *NAME();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *ABSTRACT();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *EXTENDS();
    antlr4::tree::TerminalNode *EQUAL();
    std::vector<PathContext *> path();
    PathContext* path(size_t i);
    DerivedUnitContext *derivedUnit();
    ComposedUnitContext *composedUnit();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UnitDefContext* unitDef();

  class  DerivedUnitContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *op = nullptr;;
    Ili2Parser::PathContext *unitref = nullptr;;
    DerivedUnitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    PathContext *path();
    std::vector<DecConstContext *> decConst();
    DecConstContext* decConst(size_t i);
    antlr4::tree::TerminalNode *FUNCTION();
    antlr4::tree::TerminalNode *EXPLANATION();
    std::vector<antlr4::tree::TerminalNode *> STAR();
    antlr4::tree::TerminalNode* STAR(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SLASH();
    antlr4::tree::TerminalNode* SLASH(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DerivedUnitContext* derivedUnit();

  class  ComposedUnitContext : public antlr4::ParserRuleContext {
  public:
    ComposedUnitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    ComposedUnitExprContext *composedUnitExpr();
    antlr4::tree::TerminalNode *RPAREN();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ComposedUnitContext* composedUnit();

  class  ComposedUnitExprContext : public antlr4::ParserRuleContext {
  public:
    ComposedUnitExprContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PathContext *path();
    ComposedUnitExprContext *composedUnitExpr();
    antlr4::tree::TerminalNode *STAR();
    antlr4::tree::TerminalNode *SLASH();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ComposedUnitExprContext* composedUnitExpr();
  ComposedUnitExprContext* composedUnitExpr(int precedence);
  class  MetaDataBasketDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *basketname = nullptr;;
    antlr4::Token *classname = nullptr;;
    antlr4::Token *metabjectname = nullptr;;
    antlr4::Token *metaobjectname = nullptr;;
    MetaDataBasketDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BASKET();
    antlr4::tree::TerminalNode *TILDE();
    PathContext *path();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *SIGN();
    antlr4::tree::TerminalNode *REFSYSTEM();
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *FINAL();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *EXTENDS();
    MetaDataBasketRefContext *metaDataBasketRef();
    std::vector<antlr4::tree::TerminalNode *> OBJECTS();
    antlr4::tree::TerminalNode* OBJECTS(size_t i);
    std::vector<antlr4::tree::TerminalNode *> OF();
    antlr4::tree::TerminalNode* OF(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MetaDataBasketDefContext* metaDataBasketDef();

  class  MetaDataBasketRefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *modelname = nullptr;;
    antlr4::Token *topicname = nullptr;;
    antlr4::Token *basketname = nullptr;;
    MetaDataBasketRefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MetaDataBasketRefContext* metaDataBasketRef();

  class  MetaObjectRefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *metaobjectname = nullptr;;
    MetaObjectRefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *NAME();
    MetaDataBasketRefContext *metaDataBasketRef();
    antlr4::tree::TerminalNode *DOT();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MetaObjectRefContext* metaObjectRef();

  class  ParameterDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *parameterName = nullptr;;
    ParameterDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *NAME();
    AttrTypeDefContext *attrTypeDef();
    antlr4::tree::TerminalNode *METAOBJECT();
    PropertiesContext *properties();
    antlr4::tree::TerminalNode *OF();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ParameterDefContext* parameterDef();

  class  PropertiesContext : public antlr4::ParserRuleContext {
  public:
    PropertiesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> ABSTRACT();
    antlr4::tree::TerminalNode* ABSTRACT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> EXTENDED();
    antlr4::tree::TerminalNode* EXTENDED(size_t i);
    std::vector<antlr4::tree::TerminalNode *> FINAL();
    antlr4::tree::TerminalNode* FINAL(size_t i);
    std::vector<antlr4::tree::TerminalNode *> TRANSIENT();
    antlr4::tree::TerminalNode* TRANSIENT(size_t i);
    std::vector<antlr4::tree::TerminalNode *> OID();
    antlr4::tree::TerminalNode* OID(size_t i);
    std::vector<antlr4::tree::TerminalNode *> HIDING();
    antlr4::tree::TerminalNode* HIDING(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ORDERED();
    antlr4::tree::TerminalNode* ORDERED(size_t i);
    std::vector<antlr4::tree::TerminalNode *> EXTERNAL();
    antlr4::tree::TerminalNode* EXTERNAL(size_t i);
    std::vector<antlr4::tree::TerminalNode *> GENERIC();
    antlr4::tree::TerminalNode* GENERIC(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PropertiesContext* properties();

  class  RunTimeParameterDefContext : public antlr4::ParserRuleContext {
  public:
    RunTimeParameterDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PARAMETER();
    std::vector<RunTimeParameterContext *> runTimeParameter();
    RunTimeParameterContext* runTimeParameter(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RunTimeParameterDefContext* runTimeParameterDef();

  class  RunTimeParameterContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *runtimeparametername = nullptr;;
    RunTimeParameterContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    AttrTypeDefContext *attrTypeDef();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RunTimeParameterContext* runTimeParameter();

  class  ConstraintDefContext : public antlr4::ParserRuleContext {
  public:
    ConstraintDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    MandatoryConstraintContext *mandatoryConstraint();
    PlausibilityConstraintContext *plausibilityConstraint();
    ExistenceConstraintContext *existenceConstraint();
    UniquenessConstraintContext *uniquenessConstraint();
    SetConstraintContext *setConstraint();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstraintDefContext* constraintDef();

  class  MandatoryConstraintContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *name = nullptr;;
    Ili2Parser::ExpressionContext *booleanexp = nullptr;;
    MandatoryConstraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MANDATORY();
    antlr4::tree::TerminalNode *CONSTRAINT();
    antlr4::tree::TerminalNode *SEMI();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  MandatoryConstraintContext* mandatoryConstraint();

  class  PlausibilityConstraintContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *name = nullptr;;
    Ili2Parser::DecimalContext *percentage = nullptr;;
    PlausibilityConstraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONSTRAINT();
    antlr4::tree::TerminalNode *PERCENT();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *LESSEQUAL();
    antlr4::tree::TerminalNode *GREATEREQUAL();
    DecimalContext *decimal();
    antlr4::tree::TerminalNode *COLON();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PlausibilityConstraintContext* plausibilityConstraint();

  class  ExistenceConstraintContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *name = nullptr;;
    ExistenceConstraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EXISTENCE();
    antlr4::tree::TerminalNode *CONSTRAINT();
    std::vector<AttributePathContext *> attributePath();
    AttributePathContext* attributePath(size_t i);
    antlr4::tree::TerminalNode *REQUIRED();
    antlr4::tree::TerminalNode *IN();
    std::vector<PathContext *> path();
    PathContext* path(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> OR();
    antlr4::tree::TerminalNode* OR(size_t i);
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExistenceConstraintContext* existenceConstraint();

  class  UniquenessConstraintContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *name = nullptr;;
    UniquenessConstraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UNIQUE();
    antlr4::tree::TerminalNode *SEMI();
    GlobalUniquenessContext *globalUniqueness();
    LocalUniquenessContext *localUniqueness();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *BASKET();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    antlr4::tree::TerminalNode *WHERE();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UniquenessConstraintContext* uniquenessConstraint();

  class  GlobalUniquenessContext : public antlr4::ParserRuleContext {
  public:
    GlobalUniquenessContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    UniqueElContext *uniqueEl();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GlobalUniquenessContext* globalUniqueness();

  class  UniqueElContext : public antlr4::ParserRuleContext {
  public:
    UniqueElContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ObjectOrAttributePathContext *> objectOrAttributePath();
    ObjectOrAttributePathContext* objectOrAttributePath(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  UniqueElContext* uniqueEl();

  class  LocalUniquenessContext : public antlr4::ParserRuleContext {
  public:
    LocalUniquenessContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *LOCAL();
    antlr4::tree::TerminalNode *RPAREN();
    LocalUniqueElContext *localUniqueEl();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LocalUniquenessContext* localUniqueness();

  class  LocalUniqueElContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *structureattributename = nullptr;;
    antlr4::Token *attributename = nullptr;;
    LocalUniqueElContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RARROW();
    antlr4::tree::TerminalNode* RARROW(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LocalUniqueElContext* localUniqueEl();

  class  SetConstraintContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *name = nullptr;;
    Ili2Parser::ExpressionContext *logical = nullptr;;
    SetConstraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SET();
    antlr4::tree::TerminalNode *CONSTRAINT();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *BASKET();
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    antlr4::tree::TerminalNode *WHERE();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SetConstraintContext* setConstraint();

  class  ConstraintsDefContext : public antlr4::ParserRuleContext {
  public:
    ConstraintsDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *CONSTRAINTS();
    antlr4::tree::TerminalNode *OF();
    PathContext *path();
    antlr4::tree::TerminalNode *EQUAL();
    antlr4::tree::TerminalNode *END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<ConstraintDefContext *> constraintDef();
    ConstraintDefContext* constraintDef(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstraintsDefContext* constraintsDef();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Term1Context *term1();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionContext* expression();

  class  Term1Context : public antlr4::ParserRuleContext {
  public:
    Term1Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Term2Context *> term2();
    Term2Context* term2(size_t i);
    std::vector<Operator1Context *> operator1();
    Operator1Context* operator1(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Term1Context* term1();

  class  Operator1Context : public antlr4::ParserRuleContext {
  public:
    Operator1Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *OR();
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *IMPL();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator1Context* operator1();

  class  Term2Context : public antlr4::ParserRuleContext {
  public:
    Term2Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Term3Context *> term3();
    Term3Context* term3(size_t i);
    std::vector<Operator2Context *> operator2();
    Operator2Context* operator2(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Term2Context* term2();

  class  Operator2Context : public antlr4::ParserRuleContext {
  public:
    Operator2Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AND();
    antlr4::tree::TerminalNode *STAR();
    antlr4::tree::TerminalNode *SLASH();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator2Context* operator2();

  class  Term3Context : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::TermContext *t1 = nullptr;;
    Ili2Parser::TermContext *t2 = nullptr;;
    Term3Context(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<TermContext *> term();
    TermContext* term(size_t i);
    RelationContext *relation();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Term3Context* term3();

  class  TermContext : public antlr4::ParserRuleContext {
  public:
    TermContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    FactorContext *factor();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *NOT();
    antlr4::tree::TerminalNode *DEFINED();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TermContext* term();

  class  RelationContext : public antlr4::ParserRuleContext {
  public:
    RelationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQUALEQUAL();
    antlr4::tree::TerminalNode *NOTEQUAL();
    antlr4::tree::TerminalNode *LESSGREATER();
    antlr4::tree::TerminalNode *LESSEQUAL();
    antlr4::tree::TerminalNode *GREATEREQUAL();
    antlr4::tree::TerminalNode *LESS();
    antlr4::tree::TerminalNode *GREATER();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RelationContext* relation();

  class  FactorContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::ObjectOrAttributePathContext *objectpath = nullptr;;
    Ili2Parser::ObjectOrAttributePathContext *inspaectionpath = nullptr;;
    Ili2Parser::PathContext *parampath = nullptr;;
    FactorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ObjectOrAttributePathContext *objectOrAttributePath();
    InspectionContext *inspection();
    antlr4::tree::TerminalNode *INSPECTION();
    PathContext *path();
    antlr4::tree::TerminalNode *OF();
    FunctionCallContext *functionCall();
    antlr4::tree::TerminalNode *PARAMETER();
    ConstantContext *constant();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FactorContext* factor();

  class  ObjectOrAttributePathContext : public antlr4::ParserRuleContext {
  public:
    ObjectOrAttributePathContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<PathElContext *> pathEl();
    PathElContext* pathEl(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RARROW();
    antlr4::tree::TerminalNode* RARROW(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ObjectOrAttributePathContext* objectOrAttributePath();

  class  AttributePathContext : public antlr4::ParserRuleContext {
  public:
    AttributePathContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ObjectOrAttributePathContext *objectOrAttributePath();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AttributePathContext* attributePath();

  class  PathElContext : public antlr4::ParserRuleContext {
  public:
    PathElContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *THIS();
    antlr4::tree::TerminalNode *THISAREA();
    antlr4::tree::TerminalNode *THATAREA();
    antlr4::tree::TerminalNode *PARENT();
    ObjectRefContext *objectRef();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PathElContext* pathEl();

  class  ObjectRefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *name = nullptr;;
    antlr4::Token *axislistindex = nullptr;;
    antlr4::Token *associationname = nullptr;;
    ObjectRefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AGGREGATES();
    antlr4::tree::TerminalNode *BACKSLASH();
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    antlr4::tree::TerminalNode *LBRACE();
    antlr4::tree::TerminalNode *RBRACE();
    antlr4::tree::TerminalNode *FIRST();
    antlr4::tree::TerminalNode *LAST();
    antlr4::tree::TerminalNode *POSNUMBER();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ObjectRefContext* objectRef();

  class  FunctionCallContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *functionname = nullptr;;
    FunctionCallContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    PathContext *path();
    std::vector<FunctionCallArgumentContext *> functionCallArgument();
    FunctionCallArgumentContext* functionCallArgument(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FunctionCallContext* functionCall();

  class  FunctionCallArgumentContext : public antlr4::ParserRuleContext {
  public:
    FunctionCallArgumentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *ALL();
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    std::vector<RestrictedRefContext *> restrictedRef();
    RestrictedRefContext* restrictedRef(size_t i);
    std::vector<PathContext *> path();
    PathContext* path(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FunctionCallArgumentContext* functionCallArgument();

  class  FunctionDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *functioname = nullptr;;
    Ili2Parser::ArgumentTypeContext *result = nullptr;;
    FunctionDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *FUNCTION();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *COLON();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    antlr4::tree::TerminalNode *NAME();
    ArgumentTypeContext *argumentType();
    std::vector<FunctionDefParamContext *> functionDefParam();
    FunctionDefParamContext* functionDefParam(size_t i);
    antlr4::tree::TerminalNode *EXPLANATION();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FunctionDefContext* functionDef();

  class  FunctionDefParamContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *argumentname = nullptr;;
    FunctionDefParamContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    ArgumentTypeContext *argumentType();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FunctionDefParamContext* functionDefParam();

  class  ArgumentTypeContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::PathContext *viewref = nullptr;;
    ArgumentTypeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AttrTypeDefContext *attrTypeDef();
    antlr4::tree::TerminalNode *OF();
    antlr4::tree::TerminalNode *OBJECT();
    antlr4::tree::TerminalNode *OBJECTS();
    RestrictedRefContext *restrictedRef();
    PathContext *path();
    antlr4::tree::TerminalNode *ENUMVAL();
    antlr4::tree::TerminalNode *ENUMTREEVAL();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ArgumentTypeContext* argumentType();

  class  ViewDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *viewname1 = nullptr;;
    Ili2Parser::PathContext *viewref = nullptr;;
    antlr4::Token *viewname2 = nullptr;;
    ViewDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *VIEW();
    antlr4::tree::TerminalNode *EQUAL();
    antlr4::tree::TerminalNode *END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    antlr4::tree::TerminalNode *EXTENDS();
    PropertiesContext *properties();
    PathContext *path();
    std::vector<BaseExtensionDefContext *> baseExtensionDef();
    BaseExtensionDefContext* baseExtensionDef(size_t i);
    std::vector<SelectionContext *> selection();
    SelectionContext* selection(size_t i);
    antlr4::tree::TerminalNode *ATTRIBUTE();
    std::vector<ViewAttributeContext *> viewAttribute();
    ViewAttributeContext* viewAttribute(size_t i);
    std::vector<ConstraintDefContext *> constraintDef();
    ConstraintDefContext* constraintDef(size_t i);
    FormationDefContext *formationDef();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ViewDefContext* viewDef();

  class  FormationDefContext : public antlr4::ParserRuleContext {
  public:
    FormationDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SEMI();
    ProjectionContext *projection();
    JoinContext *join();
    IliunionContext *iliunion();
    AggregationContext *aggregation();
    InspectionContext *inspection();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  FormationDefContext* formationDef();

  class  ProjectionContext : public antlr4::ParserRuleContext {
  public:
    ProjectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PROJECTION();
    antlr4::tree::TerminalNode *OF();
    RenamedViewableRefContext *renamedViewableRef();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProjectionContext* projection();

  class  JoinContext : public antlr4::ParserRuleContext {
  public:
    JoinContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *JOIN();
    antlr4::tree::TerminalNode *OF();
    std::vector<RenamedViewableRefContext *> renamedViewableRef();
    RenamedViewableRefContext* renamedViewableRef(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    std::vector<antlr4::tree::TerminalNode *> LPAREN();
    antlr4::tree::TerminalNode* LPAREN(size_t i);
    std::vector<antlr4::tree::TerminalNode *> OR();
    antlr4::tree::TerminalNode* OR(size_t i);
    std::vector<antlr4::tree::TerminalNode *> ILINULL();
    antlr4::tree::TerminalNode* ILINULL(size_t i);
    std::vector<antlr4::tree::TerminalNode *> RPAREN();
    antlr4::tree::TerminalNode* RPAREN(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  JoinContext* join();

  class  IliunionContext : public antlr4::ParserRuleContext {
  public:
    IliunionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *UNION();
    antlr4::tree::TerminalNode *OF();
    std::vector<RenamedViewableRefContext *> renamedViewableRef();
    RenamedViewableRefContext* renamedViewableRef(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IliunionContext* iliunion();

  class  AggregationContext : public antlr4::ParserRuleContext {
  public:
    AggregationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AGGREGATION();
    antlr4::tree::TerminalNode *OF();
    RenamedViewableRefContext *renamedViewableRef();
    antlr4::tree::TerminalNode *ALL();
    antlr4::tree::TerminalNode *EQUAL_CONST();
    antlr4::tree::TerminalNode *LPAREN();
    UniqueElContext *uniqueEl();
    antlr4::tree::TerminalNode *RPAREN();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AggregationContext* aggregation();

  class  InspectionContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *structureorlineattributename = nullptr;;
    InspectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *INSPECTION();
    antlr4::tree::TerminalNode *OF();
    RenamedViewableRefContext *renamedViewableRef();
    antlr4::tree::TerminalNode *AREA();
    std::vector<antlr4::tree::TerminalNode *> RARROW();
    antlr4::tree::TerminalNode* RARROW(size_t i);
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  InspectionContext* inspection();

  class  RenamedViewableRefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *basename = nullptr;;
    RenamedViewableRefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    PathContext *path();
    antlr4::tree::TerminalNode *TILDE();
    antlr4::tree::TerminalNode *NAME();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RenamedViewableRefContext* renamedViewableRef();

  class  BaseExtensionDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *basename = nullptr;;
    BaseExtensionDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BASE();
    antlr4::tree::TerminalNode *EXTENDED();
    antlr4::tree::TerminalNode *BY();
    std::vector<RenamedViewableRefContext *> renamedViewableRef();
    RenamedViewableRefContext* renamedViewableRef(size_t i);
    antlr4::tree::TerminalNode *NAME();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  BaseExtensionDefContext* baseExtensionDef();

  class  SelectionContext : public antlr4::ParserRuleContext {
  public:
    SelectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WHERE();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *SEMI();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SelectionContext* selection();

  class  ViewAttributeContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *basename = nullptr;;
    antlr4::Token *attributename = nullptr;;
    ViewAttributeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ALL();
    antlr4::tree::TerminalNode *OF();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *NAME();
    AttributeDefContext *attributeDef();
    antlr4::tree::TerminalNode *COLONEQUAL();
    FactorContext *factor();
    PropertiesContext *properties();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ViewAttributeContext* viewAttribute();

  class  GraphicDefContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *graphicname1 = nullptr;;
    Ili2Parser::PathContext *expath = nullptr;;
    Ili2Parser::PathContext *bpath = nullptr;;
    antlr4::Token *graphicname2 = nullptr;;
    GraphicDefContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *GRAPHIC();
    antlr4::tree::TerminalNode *EQUAL();
    antlr4::tree::TerminalNode *END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> NAME();
    antlr4::tree::TerminalNode* NAME(size_t i);
    PropertiesContext *properties();
    antlr4::tree::TerminalNode *EXTENDS();
    antlr4::tree::TerminalNode *BASED();
    antlr4::tree::TerminalNode *ON();
    std::vector<SelectionContext *> selection();
    SelectionContext* selection(size_t i);
    std::vector<DrawingRuleContext *> drawingRule();
    DrawingRuleContext* drawingRule(size_t i);
    std::vector<PathContext *> path();
    PathContext* path(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  GraphicDefContext* graphicDef();

  class  DrawingRuleContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *drawingrulename = nullptr;;
    DrawingRuleContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLON();
    std::vector<CondSignParamAssignmentContext *> condSignParamAssignment();
    CondSignParamAssignmentContext* condSignParamAssignment(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    antlr4::tree::TerminalNode *NAME();
    PropertiesContext *properties();
    antlr4::tree::TerminalNode *OF();
    PathContext *path();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DrawingRuleContext* drawingRule();

  class  CondSignParamAssignmentContext : public antlr4::ParserRuleContext {
  public:
    Ili2Parser::ExpressionContext *logical = nullptr;;
    CondSignParamAssignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<SignParamAssignmentContext *> signParamAssignment();
    SignParamAssignmentContext* signParamAssignment(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *WHERE();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    ExpressionContext *expression();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  CondSignParamAssignmentContext* condSignParamAssignment();

  class  SignParamAssignmentContext : public antlr4::ParserRuleContext {
  public:
    antlr4::Token *signparametername = nullptr;;
    Ili2Parser::AttributePathContext *enumpath = nullptr;;
    SignParamAssignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COLONEQUAL();
    antlr4::tree::TerminalNode *NAME();
    antlr4::tree::TerminalNode *LCURLY();
    MetaObjectRefContext *metaObjectRef();
    antlr4::tree::TerminalNode *RCURLY();
    FactorContext *factor();
    antlr4::tree::TerminalNode *ACCORDING();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<EnumAssignmentContext *> enumAssignment();
    EnumAssignmentContext* enumAssignment(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    AttributePathContext *attributePath();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SignParamAssignmentContext* signParamAssignment();

  class  EnumAssignmentContext : public antlr4::ParserRuleContext {
  public:
    EnumAssignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *WHEN();
    antlr4::tree::TerminalNode *IN();
    EnumRangeContext *enumRange();
    antlr4::tree::TerminalNode *LCURLY();
    MetaObjectRefContext *metaObjectRef();
    antlr4::tree::TerminalNode *RCURLY();
    ConstantContext *constant();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumAssignmentContext* enumAssignment();

  class  EnumRangeContext : public antlr4::ParserRuleContext {
  public:
    EnumRangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<EnumConstContext *> enumConst();
    EnumConstContext* enumConst(size_t i);
    antlr4::tree::TerminalNode *DOTDOT();

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  EnumRangeContext* enumRange();


  virtual bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;
  bool modelDefSempred(ModelDefContext *_localctx, size_t predicateIndex);
  bool topicDefSempred(TopicDefContext *_localctx, size_t predicateIndex);
  bool bagOrListTypeSempred(BagOrListTypeContext *_localctx, size_t predicateIndex);
  bool domainTypeSempred(DomainTypeContext *_localctx, size_t predicateIndex);
  bool baseTypeSempred(BaseTypeContext *_localctx, size_t predicateIndex);
  bool coordinateTypeSempred(CoordinateTypeContext *_localctx, size_t predicateIndex);
  bool lineTypeSempred(LineTypeContext *_localctx, size_t predicateIndex);
  bool composedUnitExprSempred(ComposedUnitExprContext *_localctx, size_t predicateIndex);
  bool mandatoryConstraintSempred(MandatoryConstraintContext *_localctx, size_t predicateIndex);
  bool plausibilityConstraintSempred(PlausibilityConstraintContext *_localctx, size_t predicateIndex);
  bool existenceConstraintSempred(ExistenceConstraintContext *_localctx, size_t predicateIndex);
  bool uniquenessConstraintSempred(UniquenessConstraintContext *_localctx, size_t predicateIndex);
  bool setConstraintSempred(SetConstraintContext *_localctx, size_t predicateIndex);
  bool operator1Sempred(Operator1Context *_localctx, size_t predicateIndex);
  bool operator2Sempred(Operator2Context *_localctx, size_t predicateIndex);

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

}  // namespace parser
