#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

// general

antlrcpp::Any Ili2Input::visitDecimal(parser::Ili2Parser::DecimalContext *ctx)
{

   /* decimal
   : DEC
   | POSNUMBER
   | NUMBER
   */

   string value;

   debug(ctx,">>> visitDecimal()");

   if (ctx->DEC() != nullptr) {
      value = ctx->DEC()->getText();
   }
   else if (ctx->POSNUMBER() != nullptr) {
      value = ctx->POSNUMBER()->getText();
   }
   else {
      value = ctx->NUMBER()->getText();
   }
   
   debug(ctx,"<<< visitDecimal(" + value + ")");
   return value;
   
}

static Type* basetype;
   
antlrcpp::Any Ili2Input::visitDomainType(parser::Ili2Parser::DomainTypeContext *ctx)
{

   /* typeDef
   : tname=NAME properties? // ABSTRACT|GENERIC|FINAL
     (EXTENDS basetype=path)? EQUAL
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

   debug(ctx,">>> visitDomainType(" + name + ")");
   Log.incNestLevel();
      
   if (ctx->EXTENDS() != nullptr) {
      basetype = find_type(visitPath(ctx->basedomain),get_line(ctx));
   }

   DomainType *t = nullptr;
   if (ctx->type() != nullptr) {
      Type *tt = visitType(ctx->type());

      if (tt == nullptr) {
         basetype = nullptr;
         Log.decNestLevel();
         debug(ctx,"<<< visitDomainType(" + name + ") unresolved type");
         return nullptr;
      }
      
      if (tt->getClass() == "Class") {
         Class* ct = static_cast<Class*>(tt);
         ct->isDomainType = true;
      }

      t = static_cast<DomainType *>(tt);
   }
   else {
      t = static_cast<DomainType *>(basetype->clone());
      t->Mandatory = true;
   }
   
   t->Name = name;
   t->Super = basetype;
      
   map<string,bool> properties = get_properties(ctx->properties(),vector<string>({ABSTRACT,FINAL}));
   t->Abstract = properties[ABSTRACT];
   t->Final = properties[FINAL];
   if (ctx->MANDATORY() != nullptr) {
      t->Mandatory = true;
   }

   add_type(t);

   basetype = nullptr;
   Log.decNestLevel();
   debug(ctx,"<<< visitDomainType(" + name + ")");

   return t;

}

antlrcpp::Any Ili2Input::visitType(parser::Ili2Parser::TypeContext *ctx)
{
   
   /* type
   : baseType 
   | lineType
   */

   debug(ctx,">>> visitType()");
   Log.incNestLevel();
   
   Type *t = nullptr;

   if (ctx->baseType() != nullptr) {
      t = visitBaseType(ctx->baseType());
   }
   else if (ctx->lineType() != nullptr) {
      LineType *tt = visitLineType(ctx->lineType());
      t = tt;
   }

   Log.decNestLevel();
   if (t == nullptr) {
      debug(ctx,"<<< visitType() nullptr");
   }
   else {
      debug(ctx,"<<< visitType() " + t->getClass());
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

   debug(ctx,">>> visitBaseType()");
   Log.incNestLevel();
   
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
      Log.internal_error("visitBaseType(): unable to cast to Type",1);
   }
  
   Log.decNestLevel();
   if (t == nullptr) {
      debug(ctx,"<<< visitBaseType() nullptr");
   }
   else {
      debug(ctx,"<<< visitBaseType() " + t->getClass());
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

   Type *base = find_type(predefined,get_line(ctx));
   if (base == nullptr) {
      return static_cast<Type *>(nullptr);
   }
   Type *type = static_cast<Type *>(base->clone());
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

   debug(ctx,">>> visitTextType()");

   TextType *t = new TextType;
   init_domaintype(t,ctx->start->getLine());

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
      
   debug(ctx,"<<< visitTextType()");
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

   debug(ctx,">>> visitEnumerationType()");
   Log.incNestLevel();

   EnumType *t = new EnumType;
   
   init_domaintype(t,ctx->start->getLine());

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
   EnumNode* tn = new EnumNode;
   tn->Name = "TOP";
   tn->EnumType = t;
   t->TopNode = tn;
   
   // role from ASSOCIATION TopNode
   push_context(t);
   for (auto ectx : ctx->enumeration()->enumElement()) {
      EnumNode *nn = visitEnumElement(ectx);
      tn->Node.push_back(nn);
      nn->ParentNode = tn;
   }
   pop_context();
   if (ctx->enumeration()->COLON() != nullptr) {
      tn->Final = true;
   }
   else if (ctx->enumeration()->FINAL() != nullptr) {
      tn->Final = true;
   }

   if (basetype != nullptr) {
      if (basetype->getClass() != "EnumType") {
         Log.error("incompatible base type " + basetype->getClass(),get_line(ctx));
      }
   }

   // role from ASSOCIATION TreeValueTypeOf
   // list <EnumTreeValueType *> ETVT;

   Log.decNestLevel();
   debug(ctx,"<<< visitEnumerationType()");
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

   EnumNode *n = new EnumNode();
   init_extendableme(n,ctx->start->getLine());
   n->Name = "";
   for (auto nn : ctx->NAME()) {
      if (n->Name == "") {
         n->Name = nn->getText();
      }
      else {
         n->Name = n->Name + "." + nn->getText();
      }
   }

   debug(ctx,">>> visitEnumElement(" + n->Name + ")");

   // list <EnumNode *> Node;
   if (ctx->enumeration() != nullptr) {
      Log.incNestLevel();
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
      Log.decNestLevel();
   }

   debug(ctx,"<<< visitEnumElement(" + n->Name + ")");

   return n;

}

antlrcpp::Any Ili2Input::visitBooleanType(parser::Ili2Parser::BooleanTypeContext *ctx)
{

   /* booleanType
   : BOOLEAN
   */

   /* class BooleanType : public DomainType {
   */

   debug(ctx,">>> visitBooleanType()");
   Type *t = new BooleanType;
   init_type(t,get_line(ctx));
   debug(ctx,"<<< visitBooleanType() " + t->Name);

   return t;

}

antlrcpp::Any Ili2Input::visitAlignmentType(parser::Ili2Parser::AlignmentTypeContext *ctx)
{

   /* alignmentType
   : HALIGNMENT | VALIGNMENT
   */

   debug(ctx,">>> visitAlignementType()");
   
   Type *t = nullptr;
   if (ctx->HALIGNMENT() != nullptr) {
      t = find_type("INTERLIS.HALIGNMENT",get_line(ctx));
   }
   else {
      t = find_type("INTERLIS.VALIGNMENT",get_line(ctx));
   }
   
   Type *tt = static_cast<Type *>(t->clone());
   tt->Super = t;
   
   debug(ctx,"<<< visitAlignmentType() " + t->Name);

   return tt;

}

antlrcpp::Any Ili2Input::visitEnumTreeValueType(parser::Ili2Parser::EnumTreeValueTypeContext *ctx)
{

   /* enumTreeValueType
   : ALL OF typeref=path
   */

   debug(ctx,">>> visitEnumTreeValueType()");
   Log.incNestLevel();

   EnumTreeValueType *t = new EnumTreeValueType();
   init_domaintype(t,ctx->start->getLine());

   t->ET = static_cast<EnumType *>(find_type(visitPath(ctx->path()),get_line(ctx)));

   Log.decNestLevel();
   debug(ctx,"<<< visitEnumTreeValueType()");

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

   debug(ctx,">>> visitAttributePathType()");
   AttributeRefType *t = new AttributeRefType();
   init_domaintype(t,ctx->start->getLine());
   if (ctx->attributePath() != nullptr) {
      PathOrInspFactor *restriction = visitAttributePath(ctx->attributePath());
      t->AttrRestriction = restriction;

      AttrOrParam *terminal = nullptr;
      if (restriction != nullptr && !restriction->PathEls.empty()) {
         terminal = dynamic_cast<AttrOrParam *>(restriction->PathEls.back()->Ref);
      }
      if (terminal == nullptr || terminal->Type == nullptr ||
          terminal->Type->getClass() != "ClassRefType") {
         Log.error("ATTRIBUTE OF restriction must end at a CLASS attribute",get_line(ctx->attributePath()));
      }
   }
   for (auto restriction : ctx->attrTypeDef()) {
      Type *type = visitAttrTypeDef(restriction);
      if (type != nullptr) {
         t->TypeRestriction.push_back(type);
      }
   }
   debug(ctx,"<<< visitAttributePathType()");

   return t;

}

antlrcpp::Any Ili2Input::visitEnumAssignment(parser::Ili2Parser::EnumAssignmentContext *ctx)
{

   /* enumAssignment
   : (LCURLY metaObjectRef RCURLY | constant) 
     WHEN IN enumRange
   */

   debug(ctx,"visitEnumAssignment()");
   return nullptr;

}

antlrcpp::Any Ili2Input::visitEnumRange(parser::Ili2Parser::EnumRangeContext *ctx)
{

   /* enumRange
   : enumConst (DOTDOT enumConst)?
   */

   debug(ctx,"visitEnumRange()");
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

   debug(ctx,">>> visitNumericType()");
   Log.incNestLevel();

   NumType *t = new NumType;
   init_type(t,ctx->start->getLine());
   
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
      t->Unit = find_unit(visitPath(ctx->unitref),ctx->unitref->start->getLine());
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

   Log.decNestLevel();
   debug(ctx,"<<< visitNumericType()");
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

   debug(ctx,">>> visitFormattedType()");
   Log.incNestLevel();

   FormattedType *t = new FormattedType();
   init_domaintype(t,get_line(ctx));

   t->Name = "TYPE";

   // ASSOCIATION PackageElements
   if (ctx->BASED() != nullptr) {
      Type *tt = find_type(ctx->structref->getText(),get_line(ctx));
      if (tt->getClass() == "Class") {
         t->Struct = static_cast<Class*>(tt);
      }
      else {
         Log.error(ctx->structref->getText() + " must be a structure", get_line(ctx->structref));
      }
      string format = visitFormatDef(ctx->formatDef());
      t->Format = format;
      if (ctx->min != nullptr) {
         t->Min = visitString(ctx->min);
         t->Max = visitString(ctx->max);
      }
   }
   else if (ctx->FORMAT() != nullptr) {
      Type *f = find_type(ctx->formatref->getText(),get_line(ctx->formatref));
      if (f->getClass() == "FormattedType") {
         t->BaseFormattedType = static_cast<FormattedType*>(f);
         t->Min = visitString(ctx->min);
         t->Max = visitString(ctx->max);
      }
      else {
         Log.error(ctx->formatref->getText() + " must be a formatted type",get_line(ctx));
      }
   }
   else {
      t->Min = visitString(ctx->min);
      t->Max = visitString(ctx->max);
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitFormattedType()");
   return t;

}

antlrcpp::Any Ili2Input::visitFormatDef(parser::Ili2Parser::FormatDefContext *ctx)
{

   /* formatDef 
   : LPAREN INHERITANCE?
     nonnumeric=STRING? (baseAttrRef nonnumeric=STRING)*
     baseAttrRef nonnumeric=STRING? RPAREN
   */

   debug(ctx,">>> visitFormatDef()");

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
   
   debug(ctx,"<<< visitFormatDef(" + format + ")");
   
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
   
   debug(ctx,">>> visitCoordinateType()");
   Log.incNestLevel();

   CoordType *t = new CoordType();
   init_domaintype(t,ctx->start->getLine());

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

   AxisSpec *as = new AxisSpec();
   init_mmobject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   add_axisspec(as);

   // C2
   if (ctx->numtype2 != nullptr) {

      n = visitNumericType(ctx->numtype2);
      n->Name = "C2";
      n->ElementInPackage = nullptr;
      n->_other_type = t;
      t->Axis.push_back(n);

      AxisSpec *as = new AxisSpec();
      init_mmobject(as,n->_line);
      as->CoordType = t;
      as->Axis = n;
      add_axisspec(as);

   }
   
   // C3
   if (ctx->numtype3 != nullptr) {

      n = visitNumericType(ctx->numtype2);
      n->Name = "C3";
      n->ElementInPackage = nullptr;
      n->_other_type = t;
      t->Axis.push_back(n);

      AxisSpec *as = new AxisSpec();
      init_mmobject(as,n->_line);
      as->CoordType = t;
      as->Axis = n;
      add_axisspec(as);

   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitCoordinateType()");
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

   debug(ctx, ">>> visitBagOrListType()");
   Log.incNestLevel();

   MultiValue* m = new MultiValue();
   init_domaintype(m, ctx->start->getLine());

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
         Log.decNestLevel();
         debug(ctx,"<<< visitBagOrListType() unresolved base type");
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

   Log.decNestLevel();
   debug(ctx,message);
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

   debug(ctx,">>> visitLineType()");
   
   LineType *t = new LineType();
   init_domaintype(t,ctx->start->getLine());
   
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
         t->CoordType = dynamic_cast<CoordType *>(find_domaintype(visitPath(ctx->coordref),get_line(ctx)));
      }
      catch (exception e) {
         string path = visitPath(ctx->coordref);
         Log.error(path + " is no coord type");
      }
   }

   if (ctx->overlap != nullptr) {
      t->MaxOverlap = ctx->overlap->getText();
   }
   
   if (ctx->lineattrstruct != nullptr) {
      string path = visitPath(ctx->lineattrstruct);
      try {
         t->LAStructure = find_structure(path,get_line(ctx->lineattrstruct));
      }
      catch (exception e) {
         Log.error(path + " is no structure type");
      }
   }
   
   if (ctx->lineattrstruct != nullptr) {
      //string overlap = ctx->lineattrstruct->getText();
      t->LAStructure = find_structure(ctx->lineattrstruct->getText(),get_line(ctx->lineattrstruct));
      // assign, to do !!!
   }

   switch (t->Kind) {
      case LineType::Polyline:
         debug(ctx,"<<< visitLineType() Polyline");
         break;
      case LineType::DirectedPolyline:
         debug(ctx,"<<< visitLineType() DirectedPolyline");
         break;
      case LineType::Surface:
         debug(ctx,"<<< visitLineType() Surface");
         break;
      case LineType::Area:
         debug(ctx,"<<< visitLineType() Area");
         break;
      default:
         debug(ctx,"<<< visitLineType() unknown");
         break;
   }
   
   return t;

}

antlrcpp::Any Ili2Input::visitLineForm(parser::Ili2Parser::LineFormContext *ctx)
{

   /* lineForm 
   : WITH LPAREN lineFormType (COMMA lineFormType)* RPAREN
   */

   debug(ctx,"visitLineForm()");
      
   list<LineForm *> lf;
   
   for (parser::Ili2Parser::LineFormTypeContext *t : ctx->lineFormType()) {
      string lineform = visitLineFormType(t);
		if (lineform == "STRAIGHTS") {
         LineForm *f = new LineForm();
         f->Name = lineform;
         lf.push_back(f);
		}
		else if (lineform == "ARCS") {
         LineForm *f = new LineForm();
         f->Name = lineform;
         lf.push_back(f);
	   }
		else {
         LineForm *f = find_lineform(lineform,get_line(t));
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

   debug(ctx,">>> visitLineFormType()");
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

   debug(ctx,"<<< visitLineFormType() " + form);
   return form;

}

antlrcpp::Any Ili2Input::visitLineFormTypeDef(parser::Ili2Parser::LineFormTypeDefContext *ctx)
{
   
   /* lineFormTypeDef
   : LINE FORM (lineFormTypeDecl)*
   */
   
   debug(ctx,">>> visitLineFormTypeDef()");
   Log.incNestLevel();
   
   for (auto d : ctx->lineFormTypeDecl()) {
      add_lineform(visitLineFormTypeDecl(d));
   }

   Log.decNestLevel();
   debug(ctx,">>> visitLineFormTypeDef()");

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

   debug(ctx,">>> visitLineFormTypeDecl()");
   Log.incNestLevel();
   
   LineForm *f = new LineForm();
   init_lineform(f,get_line(ctx));
   
   f->Name = ctx->lineformname->getText();
   f->Structure = find_structure(visitPath(ctx->path()),get_line(ctx->structureref));

   Log.decNestLevel();
   debug(ctx,"<<< visitLineFormTypeDecl()");
   
   return f;
   
}

antlrcpp::Any Ili2Input::visitOIDType(parser::Ili2Parser::OIDTypeContext* ctx)
{

   /* oIDType
   : OID (ANY | numericType | textType)
   */

   DomainType* t;

   debug(ctx, ">>> visitOIDType()");
   Log.incNestLevel();

   if (ctx->ANY() != nullptr) {
      t = new AnyOIDType();
      init_domaintype(t, ctx->ANY()->getSymbol()->getLine());
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

   Log.decNestLevel();
   debug(ctx, "<<< visitOIDType()");
   return t;

}

antlrcpp::Any Ili2Input::visitBlackboxType(parser::Ili2Parser::BlackboxTypeContext *ctx)
{
   
   /* blackboxType
   : BLACKBOX (XML | BINARY)
   */
   
   debug(ctx,">>> visitBlackboxType()");
   Log.incNestLevel();

   BlackboxType *t = new BlackboxType();
   init_domaintype(t,ctx->start->getLine());

   // MetaElement
   t->Name = "BLACKBOX";

   // BlackboxType
   if (ctx->XML() != nullptr) {
      t->Kind = BlackboxType::Xml;
   }
   else {
      t->Kind = BlackboxType::Binary;
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitBlackboxType()");

   return t;

}

antlrcpp::Any Ili2Input::visitClassRefType(parser::Ili2Parser::ClassRefTypeContext *ctx)
{

   /* classRefType
   : CLASS restriction?
   | STRUCTURE restriction?
   */

   debug(ctx,">>> visitClassRefType()");
   Log.incNestLevel();
   
   ClassRefType *r = new ClassRefType;
   init_type(r,get_line(ctx));
   
   Class* c = nullptr;
   if (ctx->CLASS() != nullptr) {
      c = find_class("ANYCLASS",get_line(ctx->CLASS()->getSymbol())); // restriction, to do !!!
   }
   else {
      c = find_class("ANYSTRUCTURE", get_line(ctx->STRUCTURE()->getSymbol())); // restriction, to do !!!
   }
   
   r->_baseclass = c;
   if (ctx->restriction() != nullptr) {
      list<Class *> restrictions = visitRestriction(ctx->restriction());
      r->_classrestriction = restrictions;
   }

   Log.decNestLevel();
   debug(ctx,"<<< visitClassRefType()");

   return r;

}

antlrcpp::Any Ili2Input::visitRefSys(parser::Ili2Parser::RefSysContext *ctx)
{

   /* refSys
   : (LCURLY refsys=metaObjectRef (LBRACE axis=POSNUMBER RBRACE)? RCURLY
   | LESS coord=path (LBRACE axis=POSNUMBER RBRACE)? GREATER)
   */

   debug(ctx,">>> visitRefSys()");
   NumsRefSys *result = new NumsRefSys();
   init_mmobject(result,get_line(ctx));
   if (ctx->coord != nullptr) {
      result->RefSys = dynamic_cast<MetaElement *>(find_type(visitPath(ctx->coord),get_line(ctx->coord)));
   }
   if (ctx->axis != nullptr) {
      result->AxisInd = stoi(ctx->axis->getText());
   }
   debug(ctx,"<<< visitRefSys()");
   return result;

}
