#pragma once

#include "Ili1Input.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

antlrcpp::Any Ili1Input::visitDomainDefs(parser::Ili1Parser::DomainDefsContext *ctx)
{

   /* domainDefs
   : ILIDOMAIN domainDef+
   */
  
   debug(ctx,">>> visitDomainDefs()");
   Log.incNestLevel();
   
   for (auto dctx : ctx->domainDef()) {
      visitDomainDef(dctx);
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitDomainDefs()");

   return nullptr;
   
}

antlrcpp::Any Ili1Input::visitDomainDef(parser::Ili1Parser::DomainDefContext *ctx)
{

   /* domainDef
   : domainname=NAME EQUAL type SEMI
   */
   
   string domainname = ctx->domainname->getText();
   if (is_reserved_name(domainname)) {
      domainname += "_ILI1";
   }

   debug(ctx,">>> visitDomainDef(" + domainname + ")");
   Log.incNestLevel();

   // init DomainType
   Type *t = visitType(ctx->type());
   if (t != nullptr) {
      t->Name = domainname;
      add_type(t);
   }
         
   Log.decNestLevel();
   debug(ctx,"<<< visitDomainDef(" + domainname + ")");
   
   return t;

}

antlrcpp::Any Ili1Input::visitType(parser::Ili1Parser::TypeContext *ctx)
{
   
   /* type
   : baseType
   | lineType
   | areaType
   | name=NAME
   */

   debug(ctx,">>> visitType()");
   Log.incNestLevel();
   
   Type *t = nullptr;

   if (ctx->baseType() != nullptr) {
      t = visitBaseType(ctx->baseType());
   }
   else if (ctx->lineType() != nullptr) {
      LineType *l = visitLineType(ctx->lineType());
      t = l;
   }
   else if (ctx->areaType() != nullptr) {
      LineType *l = visitAreaType(ctx->areaType());
      t = l;
   }
   else {
      string name = ctx->name->getText();
      if (is_reserved_name(name)) {
         name += "_ILI1";
      }
      Type *base = find_type(name,get_line(ctx));
      if (base != nullptr) {
         t = static_cast<Type *>(base->clone());
         init_type(t,get_line(ctx->name));
         t->Super = base;
      }
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitType()");

   return t;

}

antlrcpp::Any Ili1Input::visitBaseType(parser::Ili1Parser::BaseTypeContext *ctx)
{

   /* baseType
   : coord2
   | coord3
   | dim1Type
   | dim2Type
   | angleType
   | numericRange
   | textType
   | dateType
   | enumerationType
   | horizAlignment
   | vertAlignment
   */
   
   debug(ctx,">>> visitBaseType()");
   Log.incNestLevel();
   
   Type *t = nullptr;

   if (ctx->coord2() != nullptr) {
      CoordType *c = visitCoord2(ctx->coord2());
      t = c;
   }
   else if (ctx->coord3() != nullptr) {
      CoordType *c = visitCoord3(ctx->coord3());
      t = c;
   }
   else if (ctx->dim1Type() != nullptr) {
      NumType *n = visitDim1Type(ctx->dim1Type());
      t = n;
   }
   else if (ctx->dim2Type() != nullptr) {
      NumType *n = visitDim2Type(ctx->dim2Type());
      t = n;
   }
   else if (ctx->angleType() != nullptr) {
      NumType *n = visitAngleType(ctx->angleType());
      t = n;
   }
   else if (ctx->numericRange() != nullptr) {
      NumType *n = visitNumericRange(ctx->numericRange());
      t = n;
   }
   else if (ctx->textType() != nullptr) {
      TextType *tt = visitTextType(ctx->textType());
      t = tt;
   }
   else if (ctx->dateType() != nullptr) {
      TextType *tt = visitDateType(ctx->dateType());
      t = tt;
   }
   else if (ctx->enumerationType() != nullptr) {
      EnumType *e = visitEnumerationType(ctx->enumerationType());
      t = e;
   }
   else if (ctx->horizAlignment() != nullptr) {
      EnumType *e = visitHorizAlignment(ctx->horizAlignment());
      t = e;
   }
   else {
      EnumType *e = visitVertAlignment(ctx->vertAlignment());
      t = e;
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitBaseType()");

   return t;

}

antlrcpp::Any Ili1Input::visitCoord2(parser::Ili1Parser::Coord2Context *ctx)
{
   
   /* class CoordType : public DomainType {
      public:
      int NullAxis;
      int PiHalfAxis;
      // role from ASSOCIATION LineCoord
      list <LineType *> LineType;
   */
   
   /* coord2
   : COORD2 
     emin=decimal nmin=decimal
     emax=decimal nmax=decimal
   */
   
   debug(ctx,">>> visitCoord2()");
   Log.incNestLevel();
   
   CoordType *t = make_mmobject<CoordType>();
   init_domaintype(t,ctx->start->getLine());

   t->NullAxis = 2;
   t->PiHalfAxis = 1;

   // C1
   NumType *n = make_mmobject<NumType>();
   init_domaintype(n,get_line(ctx->emin));
   n->Min = ctx->emin->getText();
   n->Max = ctx->emax->getText();
   n->Name = "C1";
   n->ElementInPackage = nullptr;
   n->_other_type = t;
   t->Axis.push_back(n);

   AxisSpec *as = make_mmobject<AxisSpec>();
   init_mmobject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   add_axisspec(as);

   // C2
   n = make_mmobject<NumType>();
   init_domaintype(n,get_line(ctx->emin));
   n->Min = ctx->nmin->getText();
   n->Max = ctx->nmax->getText();
   n->Name = "C2";
   n->ElementInPackage = nullptr;
   n->_other_type = t;
   t->Axis.push_back(n);

   as = make_mmobject<AxisSpec>();
   init_mmobject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   add_axisspec(as);

   Log.decNestLevel();
   debug(ctx,"<<< visitCoord2()");
   
   return t;
   
}

antlrcpp::Any Ili1Input::visitCoord3(parser::Ili1Parser::Coord3Context *ctx)
{
   
   /* coord3
   : COORD3 
     emin=decimal nmin=decimal hmin = decimal
     emax=decimal nmax=decimal hmax=decimal
   */

   debug(ctx,">>> visitCoord3()");
   Log.incNestLevel();
   
   CoordType *t = make_mmobject<CoordType>();
   init_domaintype(t,ctx->start->getLine());

   t->NullAxis = 2;
   t->PiHalfAxis = 1;

   // C1
   NumType *n = make_mmobject<NumType>();
   init_domaintype(n,get_line(ctx->emin));
   n->Min = ctx->emin->getText();
   n->Max = ctx->emax->getText();
   n->Name = "C1";
   n->ElementInPackage = nullptr;
   n->_other_type = t;
   t->Axis.push_back(n);

   AxisSpec *as = make_mmobject<AxisSpec>();
   init_mmobject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   add_axisspec(as);

   // C2
   n = make_mmobject<NumType>();
   init_domaintype(n,get_line(ctx->emin));
   n->Min = ctx->nmin->getText();
   n->Max = ctx->nmax->getText();
   n->Name = "C2";
   n->ElementInPackage = nullptr;
   n->_other_type = t;
   t->Axis.push_back(n);

   as = make_mmobject<AxisSpec>();
   init_mmobject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   add_axisspec(as);

   // C3
   n = make_mmobject<NumType>();
   init_domaintype(n,get_line(ctx->emin));
   n->Min = ctx->hmin->getText();
   n->Max = ctx->hmax->getText();
   n->Name = "C3";
   n->ElementInPackage = nullptr;
   n->_other_type = t;
   t->Axis.push_back(n);

   as = make_mmobject<AxisSpec>();
   init_mmobject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   add_axisspec(as);

   Log.decNestLevel();
   debug(ctx,"<<< visitCoord3()");
   
   return t;
   
}

antlrcpp::Any Ili1Input::visitNumericRange(parser::Ili1Parser::NumericRangeContext *ctx)
{

   /*
   class NumType : public DomainType {
   public:
      string Min;
      string Max;
      bool Circular = false;
      bool Clockwise = false;
      // frole from ASSOCIATION NumUnit
      Unit *Unit;
   */
   
   /* numericRange
   : LBRACE
     min = decimal
     DOTDOT
     max = decimal
     RBRACE
   */

   debug(ctx,">>> visitNumericRange()");
   
   NumType *t = make_mmobject<NumType>();
   init_domaintype(t,ctx->start->getLine());
   
   t->Min = ctx->min->getText();
   t->Max = ctx->max->getText();
   
   debug(ctx,"<<< visitNumericRange()");
   
   return t;
   
}

antlrcpp::Any Ili1Input::visitDim1Type(parser::Ili1Parser::Dim1TypeContext *ctx)
{
   
   /*
   class NumType : public DomainType {
   public:
      string Min;
      string Max;
      bool Circular = false;
      bool Clockwise = false;
      // frole from ASSOCIATION NumUnit
      Unit *Unit;
   */
   
   /* dim1Type
   : DIM1 min=decimal max=decimal
   */

   debug(ctx,">>> visitDim1Context()");

   NumType *t = make_mmobject<NumType>();
   init_domaintype(t,ctx->start->getLine());
   
   t->Min = ctx->min->getText();
   t->Max = ctx->max->getText();
   t->Unit = find_unit("INTERLIS.m",get_line(ctx));
   
   debug(ctx,"<<< visitDim1Context()");

   return t;
   
}

antlrcpp::Any Ili1Input::visitDim2Type(parser::Ili1Parser::Dim2TypeContext *ctx)
{
   
   /*
   class NumType : public DomainType {
   public:
      string Min;
      string Max;
      bool Circular = false;
      bool Clockwise = false;
      // frole from ASSOCIATION NumUnit
      Unit *Unit;
   */
   
   /* dim2Type
   : DIM2 min = decimal max=decimal
   */
   
   debug(ctx,">>> visitDim2Type()");

   NumType *t = make_mmobject<NumType>();
   init_domaintype(t,ctx->start->getLine());
   
   t->Min = ctx->min->getText();
   t->Max = ctx->max->getText();
   t->Unit = find_unit(get_model_context()->Name + ".m2",get_line(ctx));
   
   debug(ctx,"<<< visitDim2Type()");

   return t;
      
}

antlrcpp::Any Ili1Input::visitAngleType(parser::Ili1Parser::AngleTypeContext *ctx)
{
   
   /*
   class NumType : public DomainType {
   public:
      string Min;
      string Max;
      bool Circular = false;
      bool Clockwise = false;
      // frole from ASSOCIATION NumUnit
      Unit *Unit;
   */
      
   /* angleType 
   : (RADIANS | DEGREES | GRADS) min=decimal max=decimal
   */
   
   debug(ctx,">>> visitAngleType()");

   NumType *t = make_mmobject<NumType>();
   init_domaintype(t,get_line(ctx));
   
   t->Min = ctx->min->getText();
   t->Max = ctx->max->getText();

   if (ctx->RADIANS() != nullptr) {
      t->Unit = find_unit("INTERLIS.rad",get_line(ctx));
   }
   else if (ctx->DEGREES() != nullptr) {
      t->Unit = find_unit(get_model_context()->Name + ".dgr",get_line(ctx));
   }
   else if (ctx->GRADS() != nullptr) {
      t->Unit = find_unit(get_model_context()->Name + ".grd",get_line(ctx));
   }
   
   debug(ctx,"<<< visitAngleType()");

   return t;
   
}

antlrcpp::Any Ili1Input::visitTextType(parser::Ili1Parser::TextTypeContext *ctx)
{
   
   /* class TextType : public DomainType {
   public:
      enum { MText, Text, Name, Uri } Kind;
      int MaxLength = 0;
   */

   /* textType
   : TEXT STAR numchars=POSNUMBER
   */

   debug(ctx,">>> visitTextType()");
   
   TextType *t = make_mmobject<TextType>();
   init_domaintype(t,ctx->start->getLine());

   t->Kind = TextType::Text;
   
   t->MaxLength = atoi(ctx->numchars->getText().c_str());
   
   debug(ctx,"<<< visitAngleType()");

   return t;
   
}

antlrcpp::Any Ili1Input::visitDateType(parser::Ili1Parser::DateTypeContext *ctx)
{
   
   /* dateType
   : date=DATE
   */

   debug(ctx,">>> visitDateType()");

   Type *base = find_type("INTERLIS.INTERLIS_1_DATE",get_line(ctx));

   TextType *t = static_cast<TextType *>(base->clone());
   init_domaintype(t,get_line(ctx));
   t->Super = base;

   debug(ctx,"<<< visitDateType()");

   return t;
   
}

antlrcpp::Any Ili1Input::visitEnumerationType(parser::Ili1Parser::EnumerationTypeContext *ctx)
{
   
   /* enumerationType
   : LPAREN enumElement (COMMA enumElement)* RPAREN
   */
   
   debug(ctx,">>> visitEnumerationType()");
   Log.incNestLevel();

   EnumType *t = make_mmobject<EnumType>();
   
   init_domaintype(t,ctx->start->getLine());
   t->Order = EnumType::Unordered;
   
   // TopNode
   EnumNode* tn = make_mmobject<EnumNode>();
   tn->Name = "TOP";
   tn->EnumType = t;
   tn->Final = false;
   t->TopNode = tn;
   
   // role from ASSOCIATION TopNode
   push_context(t);
   for (auto ectx : ctx->enumElement()) {
      EnumNode *nn = visitEnumElement(ectx);
      tn->Node.push_back(nn);
      nn->ParentNode = tn;
   }
   pop_context();

   Log.decNestLevel();
   debug(ctx,"<<< visitEnumerationType()");
   return t;
   
}

antlrcpp::Any Ili1Input::visitEnumElement(parser::Ili1Parser::EnumElementContext *ctx)
{
   
   /* enumElement
   : enumelement=NAME enumerationType?
   */

   EnumNode *n = make_mmobject<EnumNode>();
   n->Name = ctx->enumelement->getText();

   debug(ctx,">>> visitEnumElement(" + n->Name + ")");

   // list <EnumNode *> Node;
   if (ctx->enumerationType() != nullptr) {
      Log.incNestLevel();
      for (auto ectx : ctx->enumerationType()->enumElement()) {
         EnumNode *nn = visitEnumElement(ectx);
         n->Node.push_back(nn);
         nn->ParentNode = n;
      }
      Log.decNestLevel();
   }

   debug(ctx,"<<< visitEnumElement(" + n->Name + ")");

   return n;
   
}

antlrcpp::Any Ili1Input::visitHorizAlignment(parser::Ili1Parser::HorizAlignmentContext *ctx)
{
   
   /* horizAlignment
   : HALIGNMENT
   */
   
   debug(ctx,">>> visitHorizAlignment()");

   Type *base = find_type("INTERLIS.HALIGNMENT",get_line(ctx));

   EnumType *t = static_cast<EnumType *>(base->clone());
   init_domaintype(t,get_line(ctx));
   t->Super = base;

   debug(ctx,"<<< visitHorizAlignment()");

   return t;
   
}

antlrcpp::Any Ili1Input::visitVertAlignment(parser::Ili1Parser::VertAlignmentContext *ctx)
{
   
   /* vertAlignment
   : VALIGNMENT
   */

   debug(ctx,">>> visitVertAlignment()");

   Type *base = find_type("INTERLIS.VALIGNMENT",get_line(ctx));

   EnumType *t = static_cast<EnumType *>(base->clone());
   init_domaintype(t,get_line(ctx));
   t->Super = base;

   debug(ctx,"<<< visitVertAlignment()");

   return t;
   
}

antlrcpp::Any Ili1Input::visitLineType(parser::Ili1Parser::LineTypeContext *ctx)
{
   
   /* lineType
   : POLYLINE
     form
     controlPoints
     intersectionDef?
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
   
   LineType *t = make_mmobject<LineType>();
   init_domaintype(t,ctx->start->getLine());
   t->Kind = LineType::Polyline;

   if (ctx->form() != nullptr) {
      list<LineForm *> f = visitForm(ctx->form());
      t->LineForm = f;
   }
   
   push_context(t);
   t->CoordType = visitControlPoints(ctx->controlPoints());
   pop_context();
   
   debug(ctx,"<<< visitLineType()");
   return t;
   
}

antlrcpp::Any Ili1Input::visitForm(parser::Ili1Parser::FormContext *ctx)
{
   
   /* form
   : WITH LPAREN
    lineForm (COMMA lineForm)*
    RPAREN
   */
   
   list<LineForm *> lf;
   
   for (auto *t : ctx->lineForm()) {
      LineForm *f = make_mmobject<LineForm>();
      f->Name = t->getText();
      lf.push_back(f);
   }
   
   return lf;
   
}

antlrcpp::Any Ili1Input::visitAreaType(parser::Ili1Parser::AreaTypeContext *ctx)
{
   
   /* areaType
   : ( SURFACE form controlPoints intersectionDef?
     | AREA form controlPoints intersectionDef
     )
     lineAttributes?
   */

   debug(ctx,">>> visitAreaType()");

   LineType *t = make_mmobject<LineType>();
   init_domaintype(t,ctx->start->getLine());
   if (ctx->SURFACE() != nullptr) {
      t->Kind = LineType::Surface;
   }
   else {
      t->Kind = LineType::Area;
   }
   
   if (ctx->form() != nullptr) {
      list<LineForm *> f = visitForm(ctx->form());
      t->LineForm = f;
   }
   
   if (ctx->intersectionDef() != nullptr) {
      string overlap = visitIntersectionDef(ctx->intersectionDef());
      t->MaxOverlap = overlap;
   }
   else {
      t->MaxOverlap = "0.1";
   }

   push_context(t);
   t->CoordType = visitControlPoints(ctx->controlPoints());
   pop_context();

   if (ctx->lineAttributes() != nullptr) {
      t->LAStructure = visitLineAttributes(ctx->lineAttributes());
   }

   debug(ctx,"<<< visitAreaType()");
   return t;
   
}

antlrcpp::Any Ili1Input::visitIntersectionDef(parser::Ili1Parser::IntersectionDefContext *ctx)
{
   
   /* intersectionDef
   : WITHOUT OVERLAPS GREATER maxoverlap=decimal
   */
   
   debug(ctx,">>> visitIntersectionDef()");
   
   string maxoverlap = ctx->maxoverlap->getText();
   
   debug(ctx,"<<< visitIntersectionDef(" + maxoverlap + ")");
   
   return maxoverlap;
   
}

static int control_point_counter = 0;

antlrcpp::Any Ili1Input::visitControlPoints(parser::Ili1Parser::ControlPointsContext *ctx)
{
   
   /* controlPoints
   : VERTEX
     (coord2 | coord3 | NAME)
     (BASE EXPLANATION)?
   */
   
   debug(ctx,">>> visitControlPoints()");
   Log.incNestLevel();
   
   CoordType *c = nullptr;
   
   if (ctx->coord2() != nullptr) {
      c = visitCoord2(ctx->coord2());
      c->Name = "ControlPoints" + to_string(++control_point_counter);
      Package* p = get_package_context();
      c->ElementInPackage = p;
      p->Element.push_front(c);
   }
   else if (ctx->coord3() != nullptr) {
      c = visitCoord3(ctx->coord3());
      c->Name = "ControlPoints" + to_string(++control_point_counter);
      Package* p = get_package_context();
      c->ElementInPackage = p;
      p->Element.push_front(c);
   }
   else {
      string name = ctx->NAME()->getText();
      if (is_reserved_name(name)) {
         name += "_ILI1";
      }
      c = dynamic_cast<CoordType *>(find_type(name,get_line(ctx)));
   }
      
   Log.decNestLevel();
   debug(ctx,"<<< visitControlPoints()");
   return c;
   
}

antlrcpp::Any Ili1Input::visitLineAttributes(parser::Ili1Parser::LineAttributesContext *ctx)
{
   
   /* lineAttributes
   : LINEATTR EQUAL
     attribute+
     identifications?
     END
   */

   string name = get_class_context()->Name + "_" + get_context()->Name + "_LineAttrib";
   debug(ctx,">>> visitLineAttributes(" + name + ")");
   
   Log.incNestLevel();

   Class *c = make_mmobject<Class>();

   // Class Attributes
   c->Name = name;
   c->Kind = Class::Structure;
   init_type(c,get_line(ctx));
   add_class(c);

   // because we are in class context, we have to set ElementInPackage manually
   c->ElementInPackage = get_package_context();
   c->ElementInPackage->Element.insert(get_package_context()->Element.begin(),c);

   push_context(c);

   for (auto actx : ctx->attribute()) {
      visitAttribute(actx);
   }
   
   if (ctx->identifications() != nullptr) {
      visitIdentifications(ctx->identifications());
   }

   pop_context();
   Log.decNestLevel();
   
   debug(ctx,"<<< visitLineAttributes()");

   return c;
   
}
