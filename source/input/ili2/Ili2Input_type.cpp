#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;
using namespace util;

// general

antlrcpp::Any Ili2Input::visitDecimal(parser::Ili2Parser::DecimalContext *ctx)
{

   /* decimal
   : DEC
   | POSNUMBER
   | NUMBER
   */

   string value;

   builder_.debug(ctx,">>> visitDecimal()");

   if (ctx->DEC() != nullptr) {
      value = ctx->DEC()->getText();
   }
   else if (ctx->POSNUMBER() != nullptr) {
      value = ctx->POSNUMBER()->getText();
   }
   else {
      value = ctx->NUMBER()->getText();
   }
   
   builder_.debug(ctx,"<<< visitDecimal(" + value + ")");
   return value;
   
}

   
antlrcpp::Any Ili2Input::visitDomainType(parser::Ili2Parser::DomainTypeContext *ctx)
{

   /* typeDef
   : tname=NAME properties? // ABSTRACT|GENERIC|FINAL
     (EXTENDS basetype_=path)? EQUAL
     (MANDATORY type? | type) 
     ({ili24}? CONSTRAINTS NAME COLON expression (COMMA NAME COLON expression)*)?
     SEMI
   */
   
   /* class DomainType : public Type { // ABSTRACT
      // MetaElement.Name :=
      // DomainName if defined explicitly as a domain,
      // "Type" if defined within an attribute definition
   public:
      bool Mandatory = false;
      // role from ASSOCIATION BasketOID
      list <DataUnit *> ForDataUnit;
      // role form ASSOCIATION DomainConstraint
      list <Constraint *> Constraint;
      // role form ASSOCIATION ConcreteForGeneric
      GenericDef *GenericDef = nullptr;
   */

   string name = ctx->domainname->getText();

   builder_.debug(ctx,">>> visitDomainType(" + name + ")");
   logger_.incNestLevel();
      
   if (ctx->EXTENDS() != nullptr) {
      basetype_ = builder_.findType(visitPath(ctx->basedomain),builder_.line(ctx));
   }

   DomainType *t = nullptr;
   if (ctx->type() != nullptr) {
      Type *tt = visitType(ctx->type());

      if (tt == nullptr) {
         basetype_ = nullptr;
         logger_.decNestLevel();
         builder_.debug(ctx,"<<< visitDomainType(" + name + ") unresolved type");
         return nullptr;
      }
      
      if (tt->getClass() == "Class") {
         Class* ct = static_cast<Class*>(tt);
         ct->isDomainType = true;
      }

      t = dynamic_cast<DomainType *>(tt);
      if (t == nullptr) {
         logger_.error(DiagnosticId::TypeDomainRequired,
            "domain " + name + " requires a domain type",builder_.line(ctx));
         basetype_ = nullptr;
         logger_.decNestLevel();
         return static_cast<DomainType *>(nullptr);
      }
   }
   else {
      t = static_cast<DomainType *>(builder_.clone(*basetype_));
      t->Mandatory = true;
   }
   
   t->Name = name;
   t->Super = basetype_;
   builder_.setSelectionSource(t,ctx->domainname);
   if (ctx->basedomain != nullptr)
      builder_.setReferenceSource(t,"inheritance",ctx->basedomain);
      
   map<string,bool> properties = get_properties(logger_,ctx->properties(),vector<string>({ABSTRACT,GENERIC,FINAL}));
   t->Abstract = properties[ABSTRACT];
   t->Generic = properties[GENERIC];
   t->Final = properties[FINAL];
   if (ctx->MANDATORY() != nullptr) {
      t->Mandatory = true;
   }

   // INTERLIS 2.4 domain constraints are semantic model elements. Keeping the
   // lexical domain on both the constraint and its expression makes THIS
   // resolvable after all models have been loaded.
   auto domainExpressions = ctx->expression();
   auto domainNames = ctx->NAME();
   if (!domainExpressions.empty()) {
      builder_.pushContext(*t);
      for (size_t index = 0; index < domainExpressions.size(); ++index) {
         SimpleConstraint *constraint = builder_.store().make<SimpleConstraint>();
         builder_.initConstraint(constraint,builder_.line(domainExpressions[index]));
         constraint->Kind = SimpleConstraint::MandC;
         constraint->toDomain = t;
         if (index + 1 < domainNames.size()) {
            constraint->Name = domainNames[index + 1]->getText();
         }
         constraint->LogicalExpression = visitExpression(domainExpressions[index]);
         t->Constraint.push_back(constraint);
      }
      builder_.popContext();
   }

   builder_.addType(t);

   basetype_ = nullptr;
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitDomainType(" + name + ")");

   return t;

}

antlrcpp::Any Ili2Input::visitContextDef(parser::Ili2Parser::ContextDefContext *ctx)
{
   return visitChildren(ctx);
}

antlrcpp::Any Ili2Input::visitContextBlock(parser::Ili2Parser::ContextBlockContext *ctx)
{
   Context *context = builder_.store().make<Context>();
   builder_.initMetaElement(context,builder_.line(ctx->name));
   builder_.setSelectionSource(context,ctx->name);
   context->Name = ctx->name->getText();

   builder_.pushContext(*context);
   for (auto declaration : ctx->contextDecl()) {
      visitContextDecl(declaration);
   }
   builder_.popContext();
   return context;
}

antlrcpp::Any Ili2Input::visitContextDecl(parser::Ili2Parser::ContextDeclContext *ctx)
{
   Context *context = dynamic_cast<Context *>(builder_.current());
   if (context == nullptr) {
      return static_cast<GenericDef *>(nullptr);
   }

   vector<parser::Ili2Parser::PathContext *> paths = ctx->path();
   if (paths.size() < 2) {
      return static_cast<GenericDef *>(nullptr);
   }

   GenericDef *definition = builder_.store().make<GenericDef>();
   builder_.initObject(definition,builder_.line(ctx));
   definition->Context = context;

   DomainType *generic = builder_.findDomainType(visitPath(paths.front()),builder_.line(paths.front()));
   if (generic != nullptr) {
      definition->GenericDomain.push_back(generic);
      if (generic->GenericDef == nullptr) {
         generic->GenericDef = definition;
      }
   }
   for (size_t index = 1; index < paths.size(); ++index) {
      DomainType *concrete = builder_.findDomainType(visitPath(paths[index]),builder_.line(paths[index]));
      if (concrete != nullptr) {
         definition->ConcreteDomain.push_back(concrete);
      }
   }
   context->GenericDefinitions.push_back(definition);
   return definition;
}

antlrcpp::Any Ili2Input::visitType(parser::Ili2Parser::TypeContext *ctx)
{
   
   /* type
   : baseType 
   | lineType
   */

   builder_.debug(ctx,">>> visitType()");
   logger_.incNestLevel();
   
   Type *t = nullptr;

   if (ctx->baseType() != nullptr) {
      t = visitBaseType(ctx->baseType());
   }
   else if (ctx->lineType() != nullptr) {
      LineType *tt = visitLineType(ctx->lineType());
      t = tt;
   }

   logger_.decNestLevel();
   if (t == nullptr) {
      builder_.debug(ctx,"<<< visitType() nullptr");
   }
   else {
      builder_.debug(ctx,"<<< visitType() " + t->getClass());
   }
   return t;

}

// BaseType
   
antlrcpp::Any Ili2Input::visitBaseType(parser::Ili2Parser::BaseTypeContext *ctx)
{

   /* baseType
   : textType
   | enumerationType
   | alignmentType
   | enumTreeValueType
   | booleanType
   | alignmentType
   | numericType
   | formattedType
   | coordinateType
   | oIDType
   | blackboxType
   | classRefType
   | attributePathType
   */

   builder_.debug(ctx,">>> visitBaseType()");
   logger_.incNestLevel();
   
   Type *t = nullptr;

   try {
      if (ctx->textType() != nullptr) {
         TextType *tt = visitTextType(ctx->textType());
         t = tt;
      }
      else if (ctx->enumerationType() != nullptr) {
         EnumType *tt = visitEnumerationType(ctx->enumerationType());
         t = tt;
      }
      else if (ctx->enumTreeValueType() != nullptr) {
         EnumTreeValueType *tt = visitEnumTreeValueType(ctx->enumTreeValueType());
         t = tt;
      }
      else if (ctx->booleanType() != nullptr) {
         t = visitBooleanType(ctx->booleanType());
      }
      else if (ctx->alignmentType() != nullptr) {
         t = visitAlignmentType(ctx->alignmentType());
      }
      else if (ctx->numericType() != nullptr) {
         NumType *tt = visitNumericType(ctx->numericType());
         t = tt;
      }
      else if (ctx->formattedType() != nullptr) {
         FormattedType *tt = visitFormattedType(ctx->formattedType());
         t = tt;
      }
      else if (ctx->dateTimeType() != nullptr) {
         t = visitDateTimeType(ctx->dateTimeType());
      }
      else if (ctx->coordinateType() != nullptr) {
         CoordType *tt = visitCoordinateType(ctx->coordinateType());
         t = tt;
      }
      else if (ctx->oIDType() != nullptr) {
         DomainType *tt = visitOIDType(ctx->oIDType());
         t = tt;
      }
      else if (ctx->blackboxType() != nullptr) {
         BlackboxType *tt = visitBlackboxType(ctx->blackboxType());
         t = tt;
      }
      else if (ctx->classRefType() != nullptr) {
         ClassRefType *tt = visitClassRefType(ctx->classRefType());      
         t = tt;
      }
      else if (ctx->attributePathType() != nullptr) {
         AttributeRefType *tt = visitAttributePathType(ctx->attributePathType());
         t = tt;
      }
   }
   catch (exception e) {
      logger_.internal_error("visitBaseType(): unable to cast to Type",1);
   }
  
   logger_.decNestLevel();
   if (t == nullptr) {
      builder_.debug(ctx,"<<< visitBaseType() nullptr");
   }
   else {
      builder_.debug(ctx,"<<< visitBaseType() " + t->getClass());
   }
   return t;

}

antlrcpp::Any Ili2Input::visitDateTimeType(parser::Ili2Parser::DateTimeTypeContext *ctx)
{
   string predefined;
   if (ctx->DATE() != nullptr) {
      predefined = "INTERLIS.XMLDate";
   }
   else if (ctx->TIMEOFDAY() != nullptr) {
      predefined = "INTERLIS.XMLTime";
   }
   else {
      predefined = "INTERLIS.XMLDateTime";
   }

   Type *base = builder_.findType(predefined,builder_.line(ctx));
   if (base == nullptr) {
      return static_cast<Type *>(nullptr);
   }
   Type *type = static_cast<Type *>(builder_.clone(*base));
   type->Super = base;
   return type;
}

antlrcpp::Any Ili2Input::visitTextType(parser::Ili2Parser::TextTypeContext *ctx)
{

   /* textType
   : MTEXT (STAR maxLength=POSNUMBER)?
   | TEXT (STAR maxLength=POSNUMBER)?
   | NAME_CONST
   | URI
   */

   builder_.debug(ctx,">>> visitTextType()");

   TextType *t = builder_.store().make<TextType>();
   builder_.initDomainType(t,ctx->start->getLine());

   // MetaElement
   t->Name = "TEXT";

   // TextType
   if (ctx->MTEXT() != nullptr) {
      t->Kind = TextType::MText;
      if (ctx->maxlength != nullptr) {
         t->MaxLength = stoi(ctx->maxlength->getText());
      }
   }
   else if (ctx->TEXT() != nullptr) {
      t->Kind = TextType::Text;
      if (ctx->maxlength != nullptr) {
         t->MaxLength = stoi(ctx->maxlength->getText());
      }
   }
   else if (ctx->NAME_CONST() != nullptr) {
      t->Kind = TextType::NameVal;
   }
   else {
      t->Kind = TextType::Uri;
   }
      
   builder_.debug(ctx,"<<< visitTextType()");
   return t;

}

antlrcpp::Any Ili2Input::visitEnumerationType(parser::Ili2Parser::EnumerationTypeContext *ctx)
{

   /* enumerationType
   : enumeration (ORDERED | CIRCULAR)?
   */

   /* enumeration
   : LPAREN enumElement (COMMA enumElement)* COLON FINAL RPAREN 
   | LPAREN FINAL RPAREN 
   */

   /* class EnumType : public DomainType {
   public:
      enum {Unordered, Ordered, Circular} Order;
      // role from ASSOCIATION TopNode
      list <EnumNode *> TopNode;
      // role from ASSOCIATION TreeValueTypeOf
      list <EnumTreeValueType *> ETVT;
   */

   builder_.debug(ctx,">>> visitEnumerationType()");
   logger_.incNestLevel();

   EnumType *t = builder_.store().make<EnumType>();
   
   builder_.initDomainType(t,ctx->start->getLine());

   // EnumType
   if (ctx->ORDERED() != nullptr) {
      t->Order = EnumType::Ordered;
   }
   else if (ctx->CIRCULAR() != nullptr) {
      t->Order = EnumType::Circular;
   }
   else {
      t->Order = EnumType::Unordered;
   }
   
   // TopNode
   EnumNode* tn = builder_.store().make<EnumNode>();
   tn->Name = "TOP";
   tn->EnumType = t;
   t->TopNode = tn;
   
   // role from ASSOCIATION TopNode
   builder_.pushContext(*t);
   for (auto ectx : ctx->enumeration()->enumElement()) {
      EnumNode *nn = visitEnumElement(ectx);
      tn->Node.push_back(nn);
      nn->ParentNode = tn;
   }
   builder_.popContext();
   if (ctx->enumeration()->COLON() != nullptr) {
      tn->Final = true;
   }
   else if (ctx->enumeration()->FINAL() != nullptr) {
      tn->Final = true;
   }

   if (basetype_ != nullptr) {
      if (basetype_->getClass() != "EnumType") {
         logger_.error(DiagnosticId::TypeEnumBaseRequired,
            "incompatible base type " + basetype_->getClass(),builder_.line(ctx));
      }
   }

   // role from ASSOCIATION TreeValueTypeOf
   // list <EnumTreeValueType *> ETVT;

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitEnumerationType()");
   return t;

}

antlrcpp::Any Ili2Input::visitEnumElement(parser::Ili2Parser::EnumElementContext *ctx)
{
   
   /* enumElement
   : NAME (DOT NAME)* (sub=enumeration)?
   */
   
   /* class EnumNode : public ExtendableME {
      // MetaElement.Name := "TOP" for topnode,
      //                     enumeration value (without constant prefix #)
      //                     for all real nodes
   public:
      // role from ASSOCIATION TopNode
      EnumType *EnumType = nullptr;
      // roles from ASSOCIATION SubNode
      EnumNode *ParentNode = nullptr;
      list <EnumNode *> Node;
   */

   EnumNode *n = builder_.store().make<EnumNode>();
   builder_.initExtendable(n,ctx->start->getLine());
   n->Name = "";
   for (auto nn : ctx->NAME()) {
      if (n->Name == "") {
         n->Name = nn->getText();
      }
      else {
         n->Name = n->Name + "." + nn->getText();
      }
   }

   builder_.debug(ctx,">>> visitEnumElement(" + n->Name + ")");

   // list <EnumNode *> Node;
   if (ctx->enumeration() != nullptr) {
      logger_.incNestLevel();
      for (auto ectx : ctx->enumeration()->enumElement()) {
         EnumNode *nn = visitEnumElement(ectx);
         n->Node.push_back(nn);
         nn->ParentNode = n;
      }
      if (ctx->enumeration()->COLON() != nullptr) {
         n->Final = true;
      }
      else if (ctx->enumeration()->FINAL() != nullptr) {
         n->Final = true;
      }
      logger_.decNestLevel();
   }

   builder_.debug(ctx,"<<< visitEnumElement(" + n->Name + ")");

   return n;

}

antlrcpp::Any Ili2Input::visitBooleanType(parser::Ili2Parser::BooleanTypeContext *ctx)
{

   /* booleanType
   : BOOLEAN
   */

   /* class BooleanType : public DomainType {
   */

   builder_.debug(ctx,">>> visitBooleanType()");
   Type *t = builder_.store().make<BooleanType>();
   builder_.initType(t,builder_.line(ctx));
   builder_.debug(ctx,"<<< visitBooleanType() " + t->Name);

   return t;

}

antlrcpp::Any Ili2Input::visitAlignmentType(parser::Ili2Parser::AlignmentTypeContext *ctx)
{

   /* alignmentType
   : HALIGNMENT | VALIGNMENT
   */

   builder_.debug(ctx,">>> visitAlignementType()");
   
   Type *t = nullptr;
   if (ctx->HALIGNMENT() != nullptr) {
      t = builder_.findType("INTERLIS.HALIGNMENT",builder_.line(ctx));
   }
   else {
      t = builder_.findType("INTERLIS.VALIGNMENT",builder_.line(ctx));
   }
   
   Type *tt = static_cast<Type *>(builder_.clone(*t));
   tt->Super = t;
   
   builder_.debug(ctx,"<<< visitAlignmentType() " + t->Name);

   return tt;

}

antlrcpp::Any Ili2Input::visitEnumTreeValueType(parser::Ili2Parser::EnumTreeValueTypeContext *ctx)
{

   /* enumTreeValueType
   : ALL OF typeref=path
   */

   builder_.debug(ctx,">>> visitEnumTreeValueType()");
   logger_.incNestLevel();

   EnumTreeValueType *t = builder_.store().make<EnumTreeValueType>();
   builder_.initDomainType(t,ctx->start->getLine());

   t->ET = static_cast<EnumType *>(builder_.findType(visitPath(ctx->path()),builder_.line(ctx)));

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitEnumTreeValueType()");

   return t;

}

antlrcpp::Any Ili2Input::visitAttributePathType(parser::Ili2Parser::AttributePathTypeContext *ctx)
{
   /* attributePathType
   : ATTRIBUTE (OF (attributePath | AT NAME))?
     (RESTRICTION LPAREN attrTypeDef (COLON attrTypeDef)* RPAREN)? 
   */

   /*
   class AttributeRefType : public DomainType {
   public:
      // role from ASSOCIATION ARefOf
      Class *Of = nullptr; // Class OR AttrOrParam OR Argument, to do !!!
   */

   builder_.debug(ctx,">>> visitAttributePathType()");
   AttributeRefType *t = builder_.store().make<AttributeRefType>();
   builder_.initDomainType(t,ctx->start->getLine());
   if (ctx->attributePath() != nullptr) {
      PathOrInspFactor *restriction = visitAttributePath(ctx->attributePath());
      t->AttrRestriction = restriction;

      AttrOrParam *terminal = nullptr;
      if (restriction != nullptr && !restriction->PathEls.empty()) {
         terminal = dynamic_cast<AttrOrParam *>(restriction->PathEls.back()->Ref);
      }
      if (terminal == nullptr || terminal->Type == nullptr ||
          terminal->Type->getClass() != "ClassRefType") {
         logger_.error(DiagnosticId::ReferenceAttributeOfClassRequired,
            "ATTRIBUTE OF restriction must end at a CLASS attribute",
            builder_.line(ctx->attributePath()));
      }
   }
   for (auto restriction : ctx->attrTypeDef()) {
      Type *type = visitAttrTypeDef(restriction);
      if (type != nullptr) {
         t->TypeRestriction.push_back(type);
      }
   }
   builder_.debug(ctx,"<<< visitAttributePathType()");

   return t;

}

antlrcpp::Any Ili2Input::visitEnumAssignment(parser::Ili2Parser::EnumAssignmentContext *ctx)
{

   /* enumAssignment
   : (LCURLY metaObjectRef RCURLY | constant) 
     WHEN IN enumRange
   */

   builder_.debug(ctx,"visitEnumAssignment()");
   return nullptr;

}

antlrcpp::Any Ili2Input::visitEnumRange(parser::Ili2Parser::EnumRangeContext *ctx)
{

   /* enumRange
   : enumConst (DOTDOT enumConst)?
   */

   builder_.debug(ctx,"visitEnumRange()");
   return nullptr;

}

antlrcpp::Any Ili2Input::visitNumericType(parser::Ili2Parser::NumericTypeContext *ctx)
{

   /* numericType
   : (min=decimal DOTDOT max=decimal | NUMERIC ) CIRCULAR?
     (LBRACE unitref=path RBRACE)?
     (CLOCKWISE | COUNTERCLOCKWISE | refSys)?
   */

   /* class NumType : public DomainType {
      // MetaElement.Name :=
      // DomainName if defined explicitly as a domain,
      // "Type" if defined within an attribute definition,
      // "C1", "C2", "C3" if defined within a coordinate type
   public:
      string Min;
      string Max;
      bool Circular = false;
      bool Clockwise = false;
      // frole from ASSOCIATION NumUnit
      Unit *Unit;
   */

   builder_.debug(ctx,">>> visitNumericType()");
   logger_.incNestLevel();

   NumType *t = builder_.store().make<NumType>();
   builder_.initType(t,ctx->start->getLine());
   
   // MetaElement
   t->Name = "TYPE"; // not set here

   // NumType
   if (ctx->NUMERIC() == nullptr) {
      string min = visitDecimal(ctx->min); t->Min = min;
      string max = visitDecimal(ctx->max); t->Max = max;
   }
   else {
      t->Min = "";
      t->Max = "";
   }
   if (ctx->CIRCULAR() != nullptr) {
      t->Circular = true;
   }
   if (ctx->unitref != nullptr) {
      t->Unit = builder_.findUnit(visitPath(ctx->unitref),ctx->unitref->start->getLine());
   }
   if (ctx->CLOCKWISE() != nullptr) {
      t->Clockwise = true;
      t->Direction = NumType::ClockwiseDirection;
   }
   if (ctx->COUNTERCLOCKWISE() != nullptr) {
      t->Direction = NumType::CounterclockwiseDirection;
   }
   if (ctx->refSys() != nullptr) {
      NumsRefSys *ref = visitRefSys(ctx->refSys());
      if (ref != nullptr) {
         t->RefSys = ref->RefSys;
         t->RefSysName = ctx->refSys()->refsys == nullptr ? "" : ctx->refSys()->refsys->getText();
         t->RefSysAxis = ref->AxisInd;
      }
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitNumericType()");
   return t;

}

antlrcpp::Any Ili2Input::visitFormattedType(parser::Ili2Parser::FormattedTypeContext *ctx)
{

   /* formattedType
   : (FORMAT BASED ON structref=path formatDef)? (min=STRING DOTDOT max=STRING)?
	| FORMAT formatref=path min=STRING DOTDOT max=STRING
   */

   /* class NumType : public DomainType {
      // MetaElement.Name :=
      // DomainName if defined explicitly as a domain,
      // "Type" if defined within an attribute definition,
      // "C1", "C2", "C3" if defined within a coordinate type
   public:
      string Min = "";
      string Max = "";
      bool Circular = false;
      bool Clockwise = false;
      // frole from ASSOCIATION NumUnit
      Unit *Unit = nullptr;
   */

   /* class FormattedType : public NumType {
   public:
      string Format;
      // role from ASSOCIATION
      Class *Struct = nullptr;
   */

   builder_.debug(ctx,">>> visitFormattedType()");
   logger_.incNestLevel();

   FormattedType *t = builder_.store().make<FormattedType>();
   builder_.initDomainType(t,builder_.line(ctx));

   t->Name = "TYPE";

   // ASSOCIATION PackageElements
   if (ctx->BASED() != nullptr) {
      Type *tt = builder_.findType(ctx->structref->getText(),builder_.line(ctx));
      if (tt->getClass() == "Class") {
         t->Struct = static_cast<Class*>(tt);
      }
      else {
         logger_.error(DiagnosticId::ReferenceStructureRequired,
            ctx->structref->getText() + " must be a structure",builder_.line(ctx->structref));
      }
      string format = visitFormatDef(ctx->formatDef());
      t->Format = format;
      if (ctx->min != nullptr) {
         t->Min = visitString(ctx->min);
         t->Max = visitString(ctx->max);
      }
   }
   else if (ctx->FORMAT() != nullptr) {
      Type *f = builder_.findType(ctx->formatref->getText(),builder_.line(ctx->formatref));
      if (f->getClass() == "FormattedType") {
         t->BaseFormattedType = static_cast<FormattedType*>(f);
         t->Min = visitString(ctx->min);
         t->Max = visitString(ctx->max);
      }
      else {
         logger_.error(DiagnosticId::TypeFormattedRequired,
            ctx->formatref->getText() + " must be a formatted type",builder_.line(ctx));
      }
   }
   else {
      t->Min = visitString(ctx->min);
      t->Max = visitString(ctx->max);
   }
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitFormattedType()");
   return t;

}

antlrcpp::Any Ili2Input::visitFormatDef(parser::Ili2Parser::FormatDefContext *ctx)
{

   /* formatDef 
   : LPAREN INHERITANCE?
     nonnumeric=STRING? (baseAttrRef nonnumeric=STRING)*
     baseAttrRef nonnumeric=STRING? RPAREN
   */

   builder_.debug(ctx,">>> visitFormatDef()");

   string format = "";
   for (auto c : ctx->children) {
      string tag = c->getText();
      if (tag == "INHERITANCE") {
         continue;
      }
      else if (tag == "(") {
         continue;
      }
      else if (tag == ")") {
         continue;
      }
      if (format == "") {
         format += tag;
      }
      else {
         format += " " + tag;
      }
   }
   
   builder_.debug(ctx,"<<< visitFormatDef(" + format + ")");
   
   return format;

}
   
antlrcpp::Any Ili2Input::visitCoordinateType(parser::Ili2Parser::CoordinateTypeContext *ctx)
{

   /* coordinateType
   : ({ili23}?COORD | {ili24}?MULTICOORD) numtype1=numericType (COMMA numtype2=numericType (COMMA numtype3=numericType)?)? (COMMA rotationDef)?
   */

   /* class CoordType : public DomainType {
   public:
      int NullAxis = 1;
      int PiHalfAxis = 2;
      bool Multi = false; // 2.4
      // role from ASSOCIATION LineCoord
      list<NumType *> Axis;
   */
   
   builder_.debug(ctx,">>> visitCoordinateType()");
   logger_.incNestLevel();

   CoordType *t = builder_.store().make<CoordType>();
   builder_.initDomainType(t,ctx->start->getLine());

   // ASSOCIATION PackageElements
   if (ctx->rotationDef() != nullptr) {
      /* rotationDef
      : ROTATION nullaxis=POSNUMBER RARROW pihalfaxis=POSNUMBER
      */
      t->NullAxis = atoi(ctx->rotationDef()->nullaxis->getText().c_str());
      t->PiHalfAxis = atoi(ctx->rotationDef()->pihalfaxis->getText().c_str());
   }
      
   if (ctx->MULTICOORD() != nullptr) {
      t->Multi = true;
   }

   // C1
   NumType *n = visitNumericType(ctx->numtype1);
   n->Name = "C1";
   n->ElementInPackage = nullptr;
   n->_other_type = t;
   t->Axis.push_back(n);

   AxisSpec *as = builder_.store().make<AxisSpec>();
   builder_.initObject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   builder_.addAxisSpec(as);

   // C2
   if (ctx->numtype2 != nullptr) {

      n = visitNumericType(ctx->numtype2);
      n->Name = "C2";
      n->ElementInPackage = nullptr;
      n->_other_type = t;
      t->Axis.push_back(n);

      AxisSpec *as = builder_.store().make<AxisSpec>();
      builder_.initObject(as,n->_line);
      as->CoordType = t;
      as->Axis = n;
      builder_.addAxisSpec(as);

   }
   
   // C3
   if (ctx->numtype3 != nullptr) {

      n = visitNumericType(ctx->numtype3);
      n->Name = "C3";
      n->ElementInPackage = nullptr;
      n->_other_type = t;
      t->Axis.push_back(n);

      AxisSpec *as = builder_.store().make<AxisSpec>();
      builder_.initObject(as,n->_line);
      as->CoordType = t;
      as->Axis = n;
      builder_.addAxisSpec(as);

   }
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitCoordinateType()");
   return t;

}

// BagOrListType

antlrcpp::Any Ili2Input::visitBagOrListType(parser::Ili2Parser::BagOrListTypeContext* ctx)
{

   /* bagOrListType
   : (BAG | LIST) cardinality? OF ({ili23}? restrictedRef | {ili24}? attrType)
   */

   /* class TypeRelatedType : public DomainType { // ABSTRACT
   public:
      // Role from ASSOCIATION BaseType
      Type *BaseType = nullptr;
   */

   /* class MultiValue : public TypeRelatedType {
      // MetaElement.Name := "Type" because always defined
      //                     within an attribute definition
   public:
      bool Ordered = false;
      Multiplicity Multiplicity;
      list<Type *> TypeRestriction;
   */

   builder_.debug(ctx, ">>> visitBagOrListType()");
   logger_.incNestLevel();

   MultiValue* m = builder_.store().make<MultiValue>();
   builder_.initDomainType(m, ctx->start->getLine());

   if (ctx->LIST() != nullptr) {
      m->Ordered = true;
   }

   if (ctx->cardinality() != nullptr) {
      Multiplicity mm = visitCardinality(ctx->cardinality());
      m->Multiplicity = mm;
   }

   if (ctx->restrictedRef() != nullptr) {
      RestrictedRef* r = visitRestrictedRef(ctx->restrictedRef());
      m->BaseType = r->_baseclass;
      for (auto cr : r->_classrestriction) {
         m->TypeRestriction.push_back(cr);
      }
   }

   if (ctx->attrType() != nullptr) {
      Type *t = visitAttrType(ctx->attrType());
      if (t == nullptr) {
         logger_.decNestLevel();
         builder_.debug(ctx,"<<< visitBagOrListType() unresolved base type");
         return m;
      }
      if (t->getClass() == "RestrictedRef") {
         RestrictedRef* rr = dynamic_cast<RestrictedRef *>(t);
         m->BaseType = rr->_baseclass;
         for (auto cr : rr->_classrestriction) {
            m->TypeRestriction.push_back(cr);
         }
      }
      else if (t->getClass() == "MultiValue") {
         MultiValue* mv = dynamic_cast<MultiValue *>(t);
         m->BaseType = mv->BaseType;
         m->TypeRestriction = mv->TypeRestriction;
      }
      else {
         m->BaseType = t;
      }
      
   }
   
   string message = "<<< visitBagOrListType()";
   if (m->Ordered) {
      message += " LIST";
   }
   else {
      message += " BAG";
   }
   message += " {" + to_string(m->Multiplicity.Min) + ".." + to_string(m->Multiplicity.Max) + "}";
   message += " OF ";
   if (m->BaseType != nullptr) {
      message += m->BaseType->Name;
   }
   else {
      message += "???"; 
   }

   logger_.decNestLevel();
   builder_.debug(ctx,message);
   return m;

}

// geometry

antlrcpp::Any Ili2Input::visitLineType(parser::Ili2Parser::LineTypeContext *ctx)
{

   /* lineType
   : DIRECTED? (POLYLINE|SURFACE|AREA)
     lineForm? 
     (VERTEX coordref=path)? 
     (WITHOUT OVERLAPS GREATER overlap=decimal)? 
     (LINE ATTRIBUTES lineattrstruct=path)?
   */

   /* class LineType : public DomainType {
   public:
      enum {Polyline, DirectedPolyline, Surface, Area} Kind;
      string MaxOverlap;
      bool Multi = false; // 2.4
      // role from ASSOCIATION LineCoord
      CoordType *CoordType;
      // role from ASSOCIATION LineAttr
      Class *LAStructure;
   */

   builder_.debug(ctx,">>> visitLineType()");
   
   LineType *t = builder_.store().make<LineType>();
   builder_.initDomainType(t,ctx->start->getLine());
   
   if (ctx->directed != nullptr) {
      t->Kind = LineType::DirectedPolyline;
   }
   else if (ctx->POLYLINE() != nullptr) {
      t->Kind = LineType::Polyline;
   }
   else if (ctx->SURFACE() != nullptr) {
      t->Kind = LineType::Surface;
   }
   else if (ctx->AREA() != nullptr) {
      t->Kind = LineType::Area;
   }
   else if (ctx->multdir != nullptr) {
      t->Kind = LineType::DirectedMultiPolyline;
   }
   else if (ctx->MULTIPOLYLINE() != nullptr) {
      t->Kind = LineType::MultiPolyline;
   }
   else if (ctx->MULTISURFACE() != nullptr) {
      t->Kind = LineType::MultiSurface;
   }
   else if (ctx->MULTIAREA() != nullptr) {
      t->Kind = LineType::MultiArea;
   }

   if (ctx->lineForm() != nullptr) {
      list<LineForm *> f = visitLineForm(ctx->lineForm());
      t->LineForm = f;
   }
   
   if (ctx->coordref != nullptr) {
      try {
         t->CoordType = dynamic_cast<CoordType *>(builder_.findDomainType(visitPath(ctx->coordref),builder_.line(ctx)));
      }
      catch (exception e) {
         string path = visitPath(ctx->coordref);
         logger_.error(DiagnosticId::TypeCoordinateRequired,path + " is no coord type",0);
      }
   }

   if (ctx->overlap != nullptr) {
      t->MaxOverlap = ctx->overlap->getText();
   }
   
   if (ctx->lineattrstruct != nullptr) {
      string path = visitPath(ctx->lineattrstruct);
      try {
         t->LAStructure = builder_.findStructure(path,builder_.line(ctx->lineattrstruct));
      }
      catch (exception e) {
         logger_.error(DiagnosticId::ReferenceStructureRequired,
            path + " is no structure type",0);
      }
   }
   
   if (ctx->lineattrstruct != nullptr) {
      //string overlap = ctx->lineattrstruct->getText();
      t->LAStructure = builder_.findStructure(ctx->lineattrstruct->getText(),builder_.line(ctx->lineattrstruct));
      // assign, to do !!!
   }

   switch (t->Kind) {
      case LineType::Polyline:
         builder_.debug(ctx,"<<< visitLineType() Polyline");
         break;
      case LineType::DirectedPolyline:
         builder_.debug(ctx,"<<< visitLineType() DirectedPolyline");
         break;
      case LineType::Surface:
         builder_.debug(ctx,"<<< visitLineType() Surface");
         break;
      case LineType::Area:
         builder_.debug(ctx,"<<< visitLineType() Area");
         break;
      default:
         builder_.debug(ctx,"<<< visitLineType() unknown");
         break;
   }
   
   return t;

}

antlrcpp::Any Ili2Input::visitLineForm(parser::Ili2Parser::LineFormContext *ctx)
{

   /* lineForm 
   : WITH LPAREN lineFormType (COMMA lineFormType)* RPAREN
   */

   builder_.debug(ctx,"visitLineForm()");
      
   list<LineForm *> lf;
   
   for (parser::Ili2Parser::LineFormTypeContext *t : ctx->lineFormType()) {
      string lineform = visitLineFormType(t);
		if (lineform == "STRAIGHTS") {
         LineForm *f = builder_.store().make<LineForm>();
         f->Name = lineform;
         lf.push_back(f);
		}
		else if (lineform == "ARCS") {
         LineForm *f = builder_.store().make<LineForm>();
         f->Name = lineform;
         lf.push_back(f);
	   }
		else {
         LineForm *f = builder_.findLineForm(lineform,builder_.line(t));
         f->Name = lineform;
         lf.push_back(f);
		}
		// assignment, to do !!!
   }
   
   return lf;

}

antlrcpp::Any Ili2Input::visitLineFormType(parser::Ili2Parser::LineFormTypeContext *ctx)
{

   /* lineFormType
   : STRAIGHTS
   | ARCS
   | path
   */

   builder_.debug(ctx,">>> visitLineFormType()");
   string form;
   
   if (ctx->STRAIGHTS() != nullptr) {
      form = "STRAIGHTS";
   }
   else if (ctx->ARCS() != nullptr) {
      form = "ARCS";
   }
   else {
      form = ctx->path()->getText();
   }

   builder_.debug(ctx,"<<< visitLineFormType() " + form);
   return form;

}

antlrcpp::Any Ili2Input::visitLineFormTypeDef(parser::Ili2Parser::LineFormTypeDefContext *ctx)
{
   
   /* lineFormTypeDef
   : LINE FORM (lineFormTypeDecl)*
   */
   
   builder_.debug(ctx,">>> visitLineFormTypeDef()");
   logger_.incNestLevel();
   
   for (auto d : ctx->lineFormTypeDecl()) {
      builder_.addLineForm(visitLineFormTypeDecl(d));
   }

   logger_.decNestLevel();
   builder_.debug(ctx,">>> visitLineFormTypeDef()");

   return nullptr;

}

antlrcpp::Any Ili2Input::visitLineFormTypeDecl(parser::Ili2Parser::LineFormTypeDeclContext *ctx)
{

   /* lineFormTypeDecl
   : lineformname=NAME COLON structureref=path SEMI
   */   
   
   /* class LineForm : public MetaElement {
      // MetaElement.Name := LineFormName as defined in the INTERLIS-Model
   public:
      // role from ASSOCIATION LineFormStructure
      Class *Structure = nullptr;
   */

   builder_.debug(ctx,">>> visitLineFormTypeDecl()");
   logger_.incNestLevel();
   
   LineForm *f = builder_.store().make<LineForm>();
   builder_.initLineForm(f,builder_.line(ctx));
   
   f->Name = ctx->lineformname->getText();
   f->Structure = builder_.findStructure(visitPath(ctx->path()),builder_.line(ctx->structureref));

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitLineFormTypeDecl()");
   
   return f;
   
}

antlrcpp::Any Ili2Input::visitOIDType(parser::Ili2Parser::OIDTypeContext* ctx)
{

   /* oIDType
   : OID (ANY | numericType | textType)
   */

   DomainType* t;

   builder_.debug(ctx, ">>> visitOIDType()");
   logger_.incNestLevel();

   if (ctx->ANY() != nullptr) {
      t = builder_.store().make<AnyOIDType>();
      builder_.initDomainType(t, ctx->ANY()->getSymbol()->getLine());
   }
   else if (ctx->numericType() != nullptr) {
      NumType* nt = visitNumericType(ctx->numericType());
      nt->OIDType = true;
      t = nt;
   }
   else {
      TextType* tt = visitTextType(ctx->textType());
      tt->OIDType = true;
      t = tt;
   }

   logger_.decNestLevel();
   builder_.debug(ctx, "<<< visitOIDType()");
   return t;

}

antlrcpp::Any Ili2Input::visitBlackboxType(parser::Ili2Parser::BlackboxTypeContext *ctx)
{
   
   /* blackboxType
   : BLACKBOX (XML | BINARY)
   */
   
   builder_.debug(ctx,">>> visitBlackboxType()");
   logger_.incNestLevel();

   BlackboxType *t = builder_.store().make<BlackboxType>();
   builder_.initDomainType(t,ctx->start->getLine());

   // MetaElement
   t->Name = "BLACKBOX";

   // BlackboxType
   if (ctx->XML() != nullptr) {
      t->Kind = BlackboxType::Xml;
   }
   else {
      t->Kind = BlackboxType::Binary;
   }
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitBlackboxType()");

   return t;

}

antlrcpp::Any Ili2Input::visitClassRefType(parser::Ili2Parser::ClassRefTypeContext *ctx)
{

   /* classRefType
   : CLASS restriction?
   | STRUCTURE restriction?
   */

   builder_.debug(ctx,">>> visitClassRefType()");
   logger_.incNestLevel();
   
   ClassRefType *r = builder_.store().make<ClassRefType>();
   builder_.initType(r,builder_.line(ctx));
   
   Class* c = nullptr;
   if (ctx->CLASS() != nullptr) {
      c = builder_.findClass("ANYCLASS",builder_.line(ctx->CLASS()->getSymbol())); // restriction, to do !!!
   }
   else {
      c = builder_.findClass("ANYSTRUCTURE", builder_.line(ctx->STRUCTURE()->getSymbol())); // restriction, to do !!!
   }
   
   r->_baseclass = c;
   if (ctx->restriction() != nullptr) {
      list<Class *> restrictions = visitRestriction(ctx->restriction());
      r->_classrestriction = restrictions;
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitClassRefType()");

   return r;

}

antlrcpp::Any Ili2Input::visitRefSys(parser::Ili2Parser::RefSysContext *ctx)
{

   /* refSys
   : (LCURLY refsys=metaObjectRef (LBRACE axis=POSNUMBER RBRACE)? RCURLY
   | LESS coord=path (LBRACE axis=POSNUMBER RBRACE)? GREATER)
   */

   builder_.debug(ctx,">>> visitRefSys()");
   NumsRefSys *result = builder_.store().make<NumsRefSys>();
   builder_.initObject(result,builder_.line(ctx));
   if (ctx->coord != nullptr) {
      result->RefSys = dynamic_cast<MetaElement *>(builder_.findType(visitPath(ctx->coord),builder_.line(ctx->coord)));
   }
   if (ctx->axis != nullptr) {
      result->AxisInd = stoi(ctx->axis->getText());
   }
   builder_.debug(ctx,"<<< visitRefSys()");
   return result;

}
