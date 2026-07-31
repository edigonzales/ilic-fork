#pragma once

#include "Ili1Input.h"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

antlrcpp::Any Ili1Input::visitDomainDefs(parser::Ili1Parser::DomainDefsContext *ctx)
{

   /* domainDefs
   : ILIDOMAIN domainDef+
   */
  
   builder_.debug(ctx,">>> visitDomainDefs()");
   logger_.incNestLevel();
   
   for (auto dctx : ctx->domainDef()) {
      visitDomainDef(dctx);
   }
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitDomainDefs()");

   return nullptr;
   
}

antlrcpp::Any Ili1Input::visitDomainDef(parser::Ili1Parser::DomainDefContext *ctx)
{

   /* domainDef
   : domainname=NAME EQUAL type SEMI
   */
   
   string domainname = ctx->domainname->getText();
   if (builder_.isReservedName(domainname)) {
      domainname += "_ILI1";
   }

   builder_.debug(ctx,">>> visitDomainDef(" + domainname + ")");
   logger_.incNestLevel();

   // init DomainType
   Type *t = visitType(ctx->type());
   if (t != nullptr) {
      t->Name = domainname;
      builder_.addType(t);
   }
         
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitDomainDef(" + domainname + ")");
   
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

   builder_.debug(ctx,">>> visitType()");
   logger_.incNestLevel();
   
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
      if (builder_.isReservedName(name)) {
         name += "_ILI1";
      }
      Type *base = builder_.findType(name,builder_.line(ctx));
      if (base != nullptr) {
         t = static_cast<Type *>(builder_.clone(*base));
         builder_.initType(t,builder_.line(ctx->name));
         t->Super = base;
      }
   }
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitType()");

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
   
   builder_.debug(ctx,">>> visitBaseType()");
   logger_.incNestLevel();
   
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
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitBaseType()");

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
   
   builder_.debug(ctx,">>> visitCoord2()");
   logger_.incNestLevel();
   
   CoordType *t = builder_.store().make<CoordType>();
   builder_.initDomainType(t,ctx->start->getLine());

   t->NullAxis = 2;
   t->PiHalfAxis = 1;

   // C1
   NumType *n = builder_.store().make<NumType>();
   builder_.initDomainType(n,builder_.line(ctx->emin));
   n->Min = ctx->emin->getText();
   n->Max = ctx->emax->getText();
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
   n = builder_.store().make<NumType>();
   builder_.initDomainType(n,builder_.line(ctx->emin));
   n->Min = ctx->nmin->getText();
   n->Max = ctx->nmax->getText();
   n->Name = "C2";
   n->ElementInPackage = nullptr;
   n->_other_type = t;
   t->Axis.push_back(n);

   as = builder_.store().make<AxisSpec>();
   builder_.initObject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   builder_.addAxisSpec(as);

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitCoord2()");
   
   return t;
   
}

antlrcpp::Any Ili1Input::visitCoord3(parser::Ili1Parser::Coord3Context *ctx)
{
   
   /* coord3
   : COORD3 
     emin=decimal nmin=decimal hmin = decimal
     emax=decimal nmax=decimal hmax=decimal
   */

   builder_.debug(ctx,">>> visitCoord3()");
   logger_.incNestLevel();
   
   CoordType *t = builder_.store().make<CoordType>();
   builder_.initDomainType(t,ctx->start->getLine());

   t->NullAxis = 2;
   t->PiHalfAxis = 1;

   // C1
   NumType *n = builder_.store().make<NumType>();
   builder_.initDomainType(n,builder_.line(ctx->emin));
   n->Min = ctx->emin->getText();
   n->Max = ctx->emax->getText();
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
   n = builder_.store().make<NumType>();
   builder_.initDomainType(n,builder_.line(ctx->emin));
   n->Min = ctx->nmin->getText();
   n->Max = ctx->nmax->getText();
   n->Name = "C2";
   n->ElementInPackage = nullptr;
   n->_other_type = t;
   t->Axis.push_back(n);

   as = builder_.store().make<AxisSpec>();
   builder_.initObject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   builder_.addAxisSpec(as);

   // C3
   n = builder_.store().make<NumType>();
   builder_.initDomainType(n,builder_.line(ctx->emin));
   n->Min = ctx->hmin->getText();
   n->Max = ctx->hmax->getText();
   n->Name = "C3";
   n->ElementInPackage = nullptr;
   n->_other_type = t;
   t->Axis.push_back(n);

   as = builder_.store().make<AxisSpec>();
   builder_.initObject(as,n->_line);
   as->CoordType = t;
   as->Axis = n;
   builder_.addAxisSpec(as);

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitCoord3()");
   
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

   builder_.debug(ctx,">>> visitNumericRange()");
   
   NumType *t = builder_.store().make<NumType>();
   builder_.initDomainType(t,ctx->start->getLine());
   
   t->Min = ctx->min->getText();
   t->Max = ctx->max->getText();
   
   builder_.debug(ctx,"<<< visitNumericRange()");
   
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

   builder_.debug(ctx,">>> visitDim1Context()");

   NumType *t = builder_.store().make<NumType>();
   builder_.initDomainType(t,ctx->start->getLine());
   
   t->Min = ctx->min->getText();
   t->Max = ctx->max->getText();
   t->Unit = builder_.findUnit("INTERLIS.m",builder_.line(ctx));
   
   builder_.debug(ctx,"<<< visitDim1Context()");

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
   
   builder_.debug(ctx,">>> visitDim2Type()");

   NumType *t = builder_.store().make<NumType>();
   builder_.initDomainType(t,ctx->start->getLine());
   
   t->Min = ctx->min->getText();
   t->Max = ctx->max->getText();
   t->Unit = builder_.findUnit(builder_.currentModel()->Name + ".m2",builder_.line(ctx));
   
   builder_.debug(ctx,"<<< visitDim2Type()");

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
   
   builder_.debug(ctx,">>> visitAngleType()");

   NumType *t = builder_.store().make<NumType>();
   builder_.initDomainType(t,builder_.line(ctx));
   
   t->Min = ctx->min->getText();
   t->Max = ctx->max->getText();

   if (ctx->RADIANS() != nullptr) {
      t->Unit = builder_.findUnit("INTERLIS.rad",builder_.line(ctx));
   }
   else if (ctx->DEGREES() != nullptr) {
      t->Unit = builder_.findUnit(builder_.currentModel()->Name + ".dgr",builder_.line(ctx));
   }
   else if (ctx->GRADS() != nullptr) {
      t->Unit = builder_.findUnit(builder_.currentModel()->Name + ".grd",builder_.line(ctx));
   }
   
   builder_.debug(ctx,"<<< visitAngleType()");

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

   builder_.debug(ctx,">>> visitTextType()");
   
   TextType *t = builder_.store().make<TextType>();
   builder_.initDomainType(t,ctx->start->getLine());

   t->Kind = TextType::Text;
   
   t->MaxLength = atoi(ctx->numchars->getText().c_str());
   
   builder_.debug(ctx,"<<< visitAngleType()");

   return t;
   
}

antlrcpp::Any Ili1Input::visitDateType(parser::Ili1Parser::DateTypeContext *ctx)
{
   
   /* dateType
   : date=DATE
   */

   builder_.debug(ctx,">>> visitDateType()");

   Type *base = builder_.findType("INTERLIS.INTERLIS_1_DATE",builder_.line(ctx));

   TextType *t = static_cast<TextType *>(builder_.clone(*base));
   builder_.initDomainType(t,builder_.line(ctx));
   t->Super = base;

   builder_.debug(ctx,"<<< visitDateType()");

   return t;
   
}

antlrcpp::Any Ili1Input::visitEnumerationType(parser::Ili1Parser::EnumerationTypeContext *ctx)
{
   
   /* enumerationType
   : LPAREN enumElement (COMMA enumElement)* RPAREN
   */
   
   builder_.debug(ctx,">>> visitEnumerationType()");
   logger_.incNestLevel();

   EnumType *t = builder_.store().make<EnumType>();
   
   builder_.initDomainType(t,ctx->start->getLine());
   t->Order = EnumType::Unordered;
   
   // TopNode
   EnumNode* tn = builder_.store().make<EnumNode>();
   tn->Name = "TOP";
   tn->EnumType = t;
   tn->Final = false;
   t->TopNode = tn;
   
   // role from ASSOCIATION TopNode
   builder_.pushContext(*t);
   for (auto ectx : ctx->enumElement()) {
      EnumNode *nn = visitEnumElement(ectx);
      tn->Node.push_back(nn);
      nn->ParentNode = tn;
   }
   builder_.popContext();

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitEnumerationType()");
   return t;
   
}

antlrcpp::Any Ili1Input::visitEnumElement(parser::Ili1Parser::EnumElementContext *ctx)
{
   
   /* enumElement
   : enumelement=NAME enumerationType?
   */

   EnumNode *n = builder_.store().make<EnumNode>();
   n->Name = ctx->enumelement->getText();

   builder_.debug(ctx,">>> visitEnumElement(" + n->Name + ")");

   // list <EnumNode *> Node;
   if (ctx->enumerationType() != nullptr) {
      logger_.incNestLevel();
      for (auto ectx : ctx->enumerationType()->enumElement()) {
         EnumNode *nn = visitEnumElement(ectx);
         n->Node.push_back(nn);
         nn->ParentNode = n;
      }
      logger_.decNestLevel();
   }

   builder_.debug(ctx,"<<< visitEnumElement(" + n->Name + ")");

   return n;
   
}

antlrcpp::Any Ili1Input::visitHorizAlignment(parser::Ili1Parser::HorizAlignmentContext *ctx)
{
   
   /* horizAlignment
   : HALIGNMENT
   */
   
   builder_.debug(ctx,">>> visitHorizAlignment()");

   Type *base = builder_.findType("INTERLIS.HALIGNMENT",builder_.line(ctx));

   EnumType *t = static_cast<EnumType *>(builder_.clone(*base));
   builder_.initDomainType(t,builder_.line(ctx));
   t->Super = base;

   builder_.debug(ctx,"<<< visitHorizAlignment()");

   return t;
   
}

antlrcpp::Any Ili1Input::visitVertAlignment(parser::Ili1Parser::VertAlignmentContext *ctx)
{
   
   /* vertAlignment
   : VALIGNMENT
   */

   builder_.debug(ctx,">>> visitVertAlignment()");

   Type *base = builder_.findType("INTERLIS.VALIGNMENT",builder_.line(ctx));

   EnumType *t = static_cast<EnumType *>(builder_.clone(*base));
   builder_.initDomainType(t,builder_.line(ctx));
   t->Super = base;

   builder_.debug(ctx,"<<< visitVertAlignment()");

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

   builder_.debug(ctx,">>> visitLineType()");
   
   LineType *t = builder_.store().make<LineType>();
   builder_.initDomainType(t,ctx->start->getLine());
   t->Kind = LineType::Polyline;

   if (ctx->form() != nullptr) {
      list<LineForm *> f = visitForm(ctx->form());
      t->LineForm = f;
   }
   
   builder_.pushContext(*t);
   t->CoordType = visitControlPoints(ctx->controlPoints());
   builder_.popContext();
   
   builder_.debug(ctx,"<<< visitLineType()");
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
      LineForm *f = builder_.store().make<LineForm>();
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

   builder_.debug(ctx,">>> visitAreaType()");

   LineType *t = builder_.store().make<LineType>();
   builder_.initDomainType(t,ctx->start->getLine());
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

   builder_.pushContext(*t);
   t->CoordType = visitControlPoints(ctx->controlPoints());
   builder_.popContext();

   if (ctx->lineAttributes() != nullptr) {
      t->LAStructure = visitLineAttributes(ctx->lineAttributes());
   }

   builder_.debug(ctx,"<<< visitAreaType()");
   return t;
   
}

antlrcpp::Any Ili1Input::visitIntersectionDef(parser::Ili1Parser::IntersectionDefContext *ctx)
{
   
   /* intersectionDef
   : WITHOUT OVERLAPS GREATER maxoverlap=decimal
   */
   
   builder_.debug(ctx,">>> visitIntersectionDef()");
   
   string maxoverlap = ctx->maxoverlap->getText();
   
   builder_.debug(ctx,"<<< visitIntersectionDef(" + maxoverlap + ")");
   
   return maxoverlap;
   
}

antlrcpp::Any Ili1Input::visitControlPoints(parser::Ili1Parser::ControlPointsContext *ctx)
{
   
   /* controlPoints
   : VERTEX
     (coord2 | coord3 | NAME)
     (BASE EXPLANATION)?
   */
   
   builder_.debug(ctx,">>> visitControlPoints()");
   logger_.incNestLevel();
   
   CoordType *c = nullptr;
   
   if (ctx->coord2() != nullptr) {
      c = visitCoord2(ctx->coord2());
      c->Name = "ControlPoints" + to_string(++controlPointCounter_);
      Package* p = builder_.currentPackage();
      c->ElementInPackage = p;
      p->Element.push_front(c);
   }
   else if (ctx->coord3() != nullptr) {
      c = visitCoord3(ctx->coord3());
      c->Name = "ControlPoints" + to_string(++controlPointCounter_);
      Package* p = builder_.currentPackage();
      c->ElementInPackage = p;
      p->Element.push_front(c);
   }
   else {
      string name = ctx->NAME()->getText();
      if (builder_.isReservedName(name)) {
         name += "_ILI1";
      }
      c = dynamic_cast<CoordType *>(builder_.findType(name,builder_.line(ctx)));
   }
      
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitControlPoints()");
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

   auto *current = dynamic_cast<MetaElement *>(builder_.current());
   string name = builder_.currentClass()->Name + "_" +
      (current == nullptr ? string() : current->Name) + "_LineAttrib";
   builder_.debug(ctx,">>> visitLineAttributes(" + name + ")");
   
   logger_.incNestLevel();

   Class *c = builder_.store().make<Class>();

   // Class Attributes
   c->Name = name;
   c->Kind = Class::Structure;
   builder_.initType(c,builder_.line(ctx));
   builder_.addClass(c);

   // because we are in class context, we have to set ElementInPackage manually
   c->ElementInPackage = builder_.currentPackage();
   c->ElementInPackage->Element.insert(builder_.currentPackage()->Element.begin(),c);

   builder_.pushContext(*c);

   for (auto actx : ctx->attribute()) {
      visitAttribute(actx);
   }
   
   if (ctx->identifications() != nullptr) {
      visitIdentifications(ctx->identifications());
   }

   builder_.popContext();
   logger_.decNestLevel();
   
   builder_.debug(ctx,"<<< visitLineAttributes()");

   return c;
   
}
