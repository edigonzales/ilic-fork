#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

antlrcpp::Any Ili2Input::visitUnitDef(parser::Ili2Parser::UnitDefContext *ctx)
{

   /* unitDef
   : unitname=NAME 
     (LPAREN ABSTRACT RPAREN | LBRACE unitshort=path RBRACE)?
     (EXTENDS super=path)?
     (EQUAL (derivedUnit | composedUnit))? 
     SEMI 
   */

   /* class Unit : public ExtendableME {
      // MetaElement.Name := ShortName as defined in the INTERLIS-Model
   public:
      enum { BaseU, DerivedU, ComposedU } Kind;
      Expression *Definition;
      // role from ASSOCIATION NumUnit
      list <NumType *> Num;
      string _unitname;
   */

   string name = ctx->unitname->getText();
   builder_.debug(ctx,">>> visitUnitDef(" + name + ")");
   logger_.incNestLevel();

   // init Unit
   Unit *u = builder_.store().make<Unit>();
   builder_.initExtendable(u,ctx->unitname->getLine());
   builder_.setSelectionSource(u,ctx->unitshort == nullptr
      ? ctx->unitname : ctx->unitshort->getStop());

   // MetaElement Attributes
   u->_unitname = name;
   if (ctx->unitshort != nullptr) {
      u->Name = ctx->unitshort->getText();
   }
   else {
      u->Name = name;
   }
   builder_.addUnit(u);

   // ExtendableME Attributes
   if (ctx->ABSTRACT() != nullptr) {
      u->Abstract = true;
   }
   if (ctx->super != nullptr) {
      builder_.setReferenceSource(u,"inheritance",ctx->super);
      u->Super = builder_.findUnit(ctx->super->getText(),ctx->super->start->getLine());
      if (u->Super != nullptr) {
         u->Super->Sub.push_back(u);
      }
   }

   // Unit Attributes
   u->Kind = Unit::BaseU;
   if (ctx->derivedUnit() != nullptr) {
      u->Kind = Unit::DerivedU;
      builder_.pushContext(*u);
      u->Definition = visitDerivedUnit(ctx->derivedUnit());
      builder_.popContext();
   }
   else if (ctx->composedUnit() != nullptr) {
      u->Kind = Unit::ComposedU;
      Expression *e = visitComposedUnit(ctx->composedUnit());
      u->Definition = e;
   }
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitUnitDef(" + name + ")");
   return u;

}

antlrcpp::Any Ili2Input::visitDerivedUnit(parser::Ili2Parser::DerivedUnitContext *ctx)
{

   /* derivedUnit 
   : (decConst (op=(STAR | SLASH) decConst)* | FUNCTION EXPLANATION)? 
     LBRACE unitref=path RBRACE
   */

   /* struct Expression : public MMObject { // ABSTRACT
   public:
      string _type;
   */

   builder_.debug(ctx,">>> visitDerivedUnit()");
   logger_.incNestLevel();
   
   Expression *e = nullptr;
   
   if (ctx->decConst().size() > 0) {

      for (auto d : ctx->decConst()) {
         if (e == nullptr) {
            Constant *c = visitDecConst(d);
            e = c;
         }
         else {

            CompoundExpr *ce = builder_.store().make<CompoundExpr>();
            ce->_type = "NumType";
            if (ctx->op->getText() == "*") {
               ce->Operation = CompoundExpr_OperationType::Mult;
            }
            else {
               ce->Operation = CompoundExpr_OperationType::Div;
            }
            ce->SubExpressions.push_back(e);
            Constant *c = visitDecConst(d);
            ce->SubExpressions.push_back(c);
            e = ce;

            /*
            Token *op = ctx->op().begin();
            CompoundExpression *c = builder_.store().make<CompoundExpression>();
            c->SubExpressions.push_back(e);
            c->SubExpressions.push_back(visitDecConst(d));
            c->_type = "NumType";
            if (op.getText() == "*") {
               c->Operation = CompoundExpression::Mult;
            }
            else {
               c->Operation = CompoundExpression::Div;
            }
            e = c;
            ++op;
            */

         }
      }

   }
   else {
      // function
      // to do !!!
   }

   if (ctx->unitref != nullptr) {
      Unit *u = static_cast<Unit *>(builder_.current());
      u->Super = builder_.findUnit(ctx->unitref->getText(),ctx->unitref->start->getLine());
   }
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitDerivedUnit()");

   return e;
   
}

antlrcpp::Any Ili2Input::visitComposedUnit(parser::Ili2Parser::ComposedUnitContext *ctx)
{

   /*
   composedUnit
      : LPAREN composedUnitExpr RPAREN
   */

   /* struct Expression : public MMObject { // ABSTRACT
   public:
      string _type;
   */

   builder_.debug(ctx,">>> visitComposedUnit()");
   logger_.incNestLevel();
   
   Expression* e = visitComposedUnitExpr(ctx->composedUnitExpr());
      
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitComposedUnit()");

   return e;
   
}

antlrcpp::Any Ili2Input::visitComposedUnitExpr(parser::Ili2Parser::ComposedUnitExprContext* ctx)
{

   /* composedUnitExpr
      : path
      | composedUnitExpr STAR path
      | composedUnitExpr SLASH path
   */

   builder_.debug(ctx, ">>> visitComposedUnitExpr()");

   Expression* e = nullptr;

   if (ctx->STAR() != nullptr) {
      CompoundExpr* ce = builder_.store().make<CompoundExpr>();
      ce->Operation = CompoundExpr_OperationType::Mult;
      ce->SubExpressions.push_back(visitComposedUnitExpr(ctx->composedUnitExpr()));
      UnitRef* r = builder_.store().make<UnitRef>();
      r->Unit = builder_.findUnit(visitPath(ctx->path()), builder_.line(ctx));
      ce->SubExpressions.push_back(r);
      e = ce;
   }
   else if (ctx->SLASH() != nullptr) {
      CompoundExpr* ce = builder_.store().make<CompoundExpr>();
      ce->Operation = CompoundExpr_OperationType::Div;
      ce->SubExpressions.push_back(visitComposedUnitExpr(ctx->composedUnitExpr()));
      UnitRef * r = builder_.store().make<UnitRef>();
      r->Unit = builder_.findUnit(visitPath(ctx->path()), builder_.line(ctx));
      ce->SubExpressions.push_back(r);
      e = ce;
   }
   else {
      UnitRef* r = builder_.store().make<UnitRef>();
      r->Unit = builder_.findUnit(visitPath(ctx->path()), builder_.line(ctx));
      e = r;
   }

   builder_.debug(ctx, "<<< visitComposedUnitExpr()");

   return e;

}
