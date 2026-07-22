#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

/*

   struct SignParamAssignment : public MMObject {
   public:
      AttrOrParam *Param;
      Expression *Assignment;
      virtual string getClass() { return "SignParamAssignment"; }
   };

   struct CondSignParamAssignment : public MMObject {
   public:
      Expression *Where;
      list<SignParamAssignment *> Assignments;
      virtual string getClass() { return "CondSignParamAssignment"; }
   };

   class DrawingRule : public ExtendableME {
      // MetaElement.Name := Name as defined in the INTERLIS-Model
   public:
      list<CondSignParamAssignment *> Rule;
      // role from ASSOCIATION GraphicRule
      Graphic *Graphic;
      // role from ASSOCIATION SignClass
      Class *Class;
      virtual string getClass() { return "DrawingRule"; }
   };

*/

extern Class* pathel_context;

antlrcpp::Any Ili2Input::visitGraphicDef(parser::Ili2Parser::GraphicDefContext *ctx)
{

   /* graphicDef
   : GRAPHIC graphicname1=NAME properties? // ABSTRACT|FINAL
     (EXTENDS expath=path)?
     (BASED ON bpath=path)? EQUAL
     (selection)*
     (drawingRule)*
     END graphicname2=NAME SEMI
   */

   /* class Graphic : public ExtendableME {
      // MetaElement.Name := Name as defined in the INTERLIS-Model
   public:
      Expression *Where;
      // role from ASSOCIATION GraphicBase
      Class *Base;
      // role from ASSOCIATION GraphicRule
      list<DrawingRule *> DrawingRule;
      virtual string getClass() { return "Graphic"; }
   */

   string name1 = ctx->graphicname1->getText();
   string name2 = ctx->graphicname2->getText();
   
   debug(ctx,">>> visitGrahicDef(" + name1 + ")");
   
   if (name1 != name2) {
      Log.warning(name2 + " does not match " + name1,ctx->graphicname2->getLine());
   }
   
   Graphic *g = new Graphic();
   init_graphic(g,get_line(ctx));
   set_selection_source(g,ctx->graphicname1);
   set_end_selection_source(g,ctx->graphicname2);
   g->Name = name1;
   add_graphic(g);
   push_context(g);
   
   map<string,bool> properties = get_properties(ctx->properties(),vector({ABSTRACT,FINAL}));
   if (properties[ABSTRACT]) {
      g->Abstract = true;
   }
   if (properties[FINAL]) {
      g->Final = true;
   }
   
   g->Base = nullptr;
   if (ctx->EXTENDS() != nullptr) {
      set_reference_source(g,"inheritance",ctx->expath);
      Graphic *s = find_graphic(ctx->expath->getText(),get_line(ctx->expath));
      g->Super = s;
      if (s != nullptr) {
         g->Base = s->Base;
      }
   }
   
   if (ctx->BASED() != nullptr) {
      if (g->Base != nullptr) {
         Log.error("graphic definition is already based on " + get_path(g->Base),get_line(ctx->BASED()));
      }
      else {
         g->Base = find_class_or_view(ctx->bpath->getText(),get_line(ctx->bpath));
      }
   }
   
   for (auto s : ctx->selection()) {
      if (g->Where == nullptr) {
         g->Where = visitSelection(s);
      }
      else {

         /* struct CompoundExpr : public Expression {
         public:
            CompoundExpr_OperationType Operation = Relation_Equal;
            list <Expression *> SubExpressions; // LIST
         */

         if (g->Where->getClass() != "CompoundExpression") {
            CompoundExpr *e = new CompoundExpr();
            e->Operation = CompoundExpr_OperationType::And;
            e->SubExpressions.push_back(g->Where);
            Expression* ee = visitSelection(s);
            e->SubExpressions.push_back(ee);
         }
         else {
            CompoundExpr *e = static_cast<CompoundExpr *>(g->Where);
            Expression* ee = visitSelection(s);
            e->SubExpressions.push_back(ee);
         }
         
      }
   }
   
   for (auto r : ctx->drawingRule()) {
      DrawingRule *dr = visitDrawingRule(r);
      dr->Graphic = g;
      g->DrawingRule.push_back(dr);
   }
   
   pop_context();
   
   return g;

}
             
antlrcpp::Any Ili2Input::visitDrawingRule(parser::Ili2Parser::DrawingRuleContext *ctx)
{

   /* drawingRule
   : drawingrulename=NAME properties? // ABSTRACT|EXTENDED|FINAL
     (OF path)?
     COLON condSignParamAssignment (COMMA condSignParamAssignment)* SEMI
   */

   /* class DrawingRule : public ExtendableME {
      // MetaElement.Name := Name as defined in the INTERLIS-Model
   public:
      list<CondSignParamAssignment *> Rule; // LIST
      // role from ASSOCIATION GraphicRule
      Graphic *Graphic = nullptr;
      // role from ASSOCIATION SignClass
      Class *Class = nullptr;
   */
   
   string name = ctx->drawingrulename->getText();
   
   debug(ctx,">>> visitDrawingRule(" + name + ")");
   Log.incNestLevel();

   DrawingRule *r = new DrawingRule;
   init_extendableme(r,get_line(ctx));
   r->Name = name;

   map<string,bool> properties = get_properties(ctx->properties(),vector({ABSTRACT,EXTENDED,FINAL}));
   if (properties[ABSTRACT]) {
      r->Abstract = true;
   }
   if (properties[FINAL]) {
      r->Final = true;
   }
   if (properties[EXTENDED]) {
      // g->Super, to do !!!
   }

   for (auto actx: ctx->condSignParamAssignment()) {
      r->Rule.push_back(visitCondSignParamAssignment(actx));
   }

   Log.decNestLevel();
   debug(ctx,"<<< visitDrawingRule(" + name + ")");
   return r;

}
             
antlrcpp::Any Ili2Input::visitCondSignParamAssignment(parser::Ili2Parser::CondSignParamAssignmentContext *ctx)
{

   /* condSignParamAssignment
   : (WHERE logical=expression)?
     LPAREN signParamAssignment (COMMA signParamAssignment)* RPAREN
   */

   /* struct CondSignParamAssignment : public MMObject {
   public:
      Expression *Where = nullptr;
      list<SignParamAssignment *> Assignments; // LIST
   */
   
   debug(ctx,">>> visitCondSingAssignment()");
   Log.incNestLevel();

   CondSignParamAssignment *a = new CondSignParamAssignment;
   init_mmobject(a,get_line(ctx));

   if (ctx->expression() != nullptr) {
      a->Where = visitExpression(ctx->expression());
   }

   for (auto actx: ctx->signParamAssignment()) {
      a->Assignments.push_back(visitSignParamAssignment(actx));
   }

   Log.decNestLevel();
   debug(ctx,">>> visitCondSingAssignment()");

   return a;

}
             
antlrcpp::Any Ili2Input::visitSignParamAssignment(parser::Ili2Parser::SignParamAssignmentContext *ctx)
{

   /* signParamAssignment
   : signparametername=NAME
     COLONEQUAL ( 
       LCURLY metaObjectRef RCURLY
       | factor
       | ACCORDING enumpath=attributePath
         LPAREN enumAssignment (COMMA enumAssignment)* RPAREN
     )
   */

   /* struct SignParamAssignment : public MMObject {
   public:
      AttrOrParam *Param = nullptr;
      Expression *Assignment = nullptr;
   */

   string name = ctx->signparametername->getText();
   debug(ctx,">>> visitSignParamAssignment(" + name + ")");

   SignParamAssignment *a = new SignParamAssignment;
   init_mmobject(a,get_line(ctx));
   // to do !!!

   debug(ctx,"<<< visitSignParamAssignment(" + name + ")");

   return a;

}
