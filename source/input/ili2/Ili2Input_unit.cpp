#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
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
   debug(ctx,">>> visitUnitDef(" + name + ")");
   Log.incNestLevel();

   // init Unit
   Unit *u = new Unit;
   init_extendableme(u,ctx->unitname->getLine());
   set_selection_source(u,ctx->unitshort == nullptr
      ? ctx->unitname : ctx->unitshort->getStop());

   // MetaElement Attributes
   u->_unitname = name;
   if (ctx->unitshort != nullptr) {
      u->Name = ctx->unitshort->getText();
   }
   else {
      u->Name = name;
   }
   add_unit(u);

   // ExtendableME Attributes
   if (ctx->ABSTRACT() != nullptr) {
      u->Abstract = true;
   }
   if (ctx->super != nullptr) {
      set_reference_source(u,"inheritance",ctx->super);
      u->Super = find_unit(ctx->super->getText(),ctx->super->start->getLine());
      if (u->Super != nullptr) {
         u->Super->Sub.push_back(u);
      }
   }

   // Unit Attributes
   u->Kind = Unit::BaseU;
   if (ctx->derivedUnit() != nullptr) {
      u->Kind = Unit::DerivedU;
      push_context(u);
      u->Definition = visitDerivedUnit(ctx->derivedUnit());
      pop_context();
   }
   else if (ctx->composedUnit() != nullptr) {
      u->Kind = Unit::ComposedU;
      Expression *e = visitComposedUnit(ctx->composedUnit());
      u->Definition = e;
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitUnitDef(" + name + ")");
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

   debug(ctx,">>> visitDerivedUnit()");
   Log.incNestLevel();
   
   Expression *e = nullptr;
   
   if (ctx->decConst().size() > 0) {

      for (auto d : ctx->decConst()) {
         if (e == nullptr) {
            Constant *c = visitDecConst(d);
            e = c;
         }
         else {

            CompoundExpr *ce = new CompoundExpr();
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
            CompoundExpression *c = new CompoundExpression();
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
      Unit *u = static_cast<Unit *>(get_context());
      u->Super = find_unit(ctx->unitref->getText(),ctx->unitref->start->getLine());
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitDerivedUnit()");

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

   debug(ctx,">>> visitComposedUnit()");
   Log.incNestLevel();
   
   Expression* e = visitComposedUnitExpr(ctx->composedUnitExpr());
      
   Log.decNestLevel();
   debug(ctx,"<<< visitComposedUnit()");

   return e;
   
}

antlrcpp::Any Ili2Input::visitComposedUnitExpr(parser::Ili2Parser::ComposedUnitExprContext* ctx)
{

   /* composedUnitExpr
      : path
      | composedUnitExpr STAR path
      | composedUnitExpr SLASH path
   */

   debug(ctx, ">>> visitComposedUnitExpr()");

   Expression* e = nullptr;

   if (ctx->STAR() != nullptr) {
      CompoundExpr* ce = new CompoundExpr();
      ce->Operation = CompoundExpr_OperationType::Mult;
      ce->SubExpressions.push_back(visitComposedUnitExpr(ctx->composedUnitExpr()));
      UnitRef* r = new UnitRef;
      r->Unit = find_unit(visitPath(ctx->path()), get_line(ctx));
      ce->SubExpressions.push_back(r);
      e = ce;
   }
   else if (ctx->SLASH() != nullptr) {
      CompoundExpr* ce = new CompoundExpr();
      ce->Operation = CompoundExpr_OperationType::Div;
      ce->SubExpressions.push_back(visitComposedUnitExpr(ctx->composedUnitExpr()));
      UnitRef * r = new UnitRef;
      r->Unit = find_unit(visitPath(ctx->path()), get_line(ctx));
      ce->SubExpressions.push_back(r);
      e = ce;
   }
   else {
      UnitRef* r = new UnitRef;
      r->Unit = find_unit(visitPath(ctx->path()), get_line(ctx));
      e = r;
   }

   debug(ctx, "<<< visitComposedUnitExpr()");

   return e;

}
