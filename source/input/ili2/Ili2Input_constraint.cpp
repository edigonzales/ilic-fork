#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;
using namespace util;

antlrcpp::Any Ili2Input::visitConstraintDef(parser::Ili2Parser::ConstraintDefContext *ctx)
{

   /* constraintDef
   : mandatoryConstraint
   | plausibilityConstraint
   | existenceConstraint
   | uniquenessConstraint
   | setConstraint 
   */

   /* class Constraint : public MetaElement { // ABSTRACT, 2.4
   public:
      // role from ASSOCIATION DomainConstraint
      DomainType *toDomain = nullptr; // 2.4
      // role from ASSOCIATION ClassConstraint
      Class *ToClass = nullptr; // 2.4
      virtual string getClass() { return "Constraint"; }
      virtual string getBaseClass() { return "MetaElement"; };
      virtual bool isAbstract() { return true; }
   */
   
   builder_.debug(ctx,">>> visitConstraintDef()");
   logger_.incNestLevel();

   Constraint *c = nullptr;
   
   if (ctx->mandatoryConstraint() != nullptr) {
      SimpleConstraint *cc = visitMandatoryConstraint(ctx->mandatoryConstraint());
      c = cc;
   }
   else if (ctx->plausibilityConstraint() != nullptr) {
      SimpleConstraint *cc = visitPlausibilityConstraint(ctx->plausibilityConstraint());
      c = cc;
   }
   else if (ctx->existenceConstraint() != nullptr) {
      ExistenceConstraint *cc = visitExistenceConstraint(ctx->existenceConstraint());
      c = cc;
   }
   else if (ctx->uniquenessConstraint() != nullptr) {
      UniqueConstraint *cc = visitUniquenessConstraint(ctx->uniquenessConstraint());
      c = cc;
   }
   else if (ctx->setConstraint() != nullptr) {
      SetConstraint *cc = visitSetConstraint(ctx->setConstraint());
      c = cc;
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitConstraintDef()");

   return c;

}

static bool is_boolean_expression(Expression *e)
{
   return e->_type == "BooleanType" || e->_type == "???";
}

antlrcpp::Any Ili2Input::visitMandatoryConstraint(parser::Ili2Parser::MandatoryConstraintContext *ctx)
{

   /* mandatoryConstraint
   : MANDATORY CONSTRAINT booleanexp=expression SEMI
   */

   /* class SimpleConstraint : public Constraint { // 2.4 struct -> class
   public:
      enum {MandC, LowPercC, HighPercC} Kind;
      double Percentage = 100.0;
      Expression *LogicalExpression;
   */

   builder_.debug(ctx,">>> visitMandatoryConstraint()");
   logger_.incNestLevel();
   
   SimpleConstraint *c = builder_.store().make<SimpleConstraint>();
   builder_.initConstraint(c,builder_.line(ctx));
   if (ctx->name != nullptr) {
      c->Name = ctx->name->getText();
      builder_.setSelectionSource(c,ctx->name);
   }
   c->Kind = SimpleConstraint::MandC;
   c->LogicalExpression = visitExpression(ctx->expression());
   if (c->LogicalExpression != nullptr && !is_boolean_expression(c->LogicalExpression)) {
      logger_.error(DiagnosticId::ExpressionBooleanRequired,
         "expression must return a boolean value",ctx->expression()->start->getLine());
   }
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitMandatoryConstraint()");

   return c;

}

antlrcpp::Any Ili2Input::visitPlausibilityConstraint(parser::Ili2Parser::PlausibilityConstraintContext *ctx)
{

   /* plausibilityConstraint
   : CONSTRAINT
     (LESSEQUAL | GREATEREQUAL) percentage=decimal PERCENT
     expression SEMI
   */

   /* class SimpleConstraint : public Constraint { // 2.4 struct -> class
   public:
      enum {MandC, LowPercC, HighPercC} Kind;
      double Percentage = 100.0;
      Expression *LogicalExpression;
   */

   builder_.debug(ctx,"visitPlausibilityConstraint()");

   SimpleConstraint *c = builder_.store().make<SimpleConstraint>();
   builder_.initConstraint(c,builder_.line(ctx));
   if (ctx->name != nullptr) {
      c->Name = ctx->name->getText();
      builder_.setSelectionSource(c,ctx->name);
   }
   c->Kind = SimpleConstraint::MandC;
   c->LogicalExpression = visitExpression(ctx->expression());
   if (!is_boolean_expression(c->LogicalExpression)) {
      logger_.error(DiagnosticId::ExpressionBooleanRequired,
         "expression must return a boolean value",ctx->expression()->start->getLine());
   }
   if (ctx->LESSEQUAL() != nullptr) {
      c->_percentage_operation = SimpleConstraint::LessEqual;
   }
   else if (ctx->GREATEREQUAL() != nullptr) {
      c->_percentage_operation = SimpleConstraint::GreaterEqual;
   }
   if (ctx->percentage != nullptr) {
      c->Percentage = stod(ctx->percentage->getText());
   }
      
   return c;

}

antlrcpp::Any Ili2Input::visitExistenceConstraint(parser::Ili2Parser::ExistenceConstraintContext *ctx)
{
   
   /* existenceConstraint
   : EXISTENCE CONSTRAINT attributePath REQUIRED IN
     path COLON attributePath
     (OR path COLON attributePath )* SEMI
   ;
   */

   /* struct PathOrInspFactor : public Factor {
   public:
      list <PathEl *> PathEls;
      View *Inspection = nullptr;
   };

   class ExistenceDef : public PathOrInspFactor { // 2.4 struct -> class
   public:
      Class *Viewable = nullptr;
   };

   class ExistenceConstraint : public Constraint { // 2.4 struct -> class
   public:
      PathOrInspFactor *Attr;
      list<ExistenceDef *> ExistsIn;
   */

   builder_.debug(ctx,">>> visitExistenceConstraint()");
   logger_.incNestLevel();

   ExistenceConstraint *c = builder_.store().make<ExistenceConstraint>();
   builder_.initConstraint(c,builder_.line(ctx));
   if (ctx->name != nullptr) {
      c->Name = ctx->name->getText();
      builder_.setSelectionSource(c,ctx->name);
   }
   auto attributes = ctx->attributePath();
   auto viewables = ctx->path();
   if (!attributes.empty()) {
      antlrcpp::Any attribute = visitAttributePath(attributes.front());
      c->Attr = attribute.as<PathOrInspFactor *>();
   }
   for (size_t i = 0; i < viewables.size() && i + 1 < attributes.size(); ++i) {
      ExistenceDef *definition = builder_.store().make<ExistenceDef>();
      builder_.initFactor(definition,builder_.line(viewables[i]));
      definition->Viewable = builder_.findClassOrView(visitPath(viewables[i]),builder_.line(viewables[i]));
      if (definition->Viewable != nullptr) {
         builder_.pushContext(*definition->Viewable);
         antlrcpp::Any requiredFactor = visitAttributePath(attributes[i + 1]);
         PathOrInspFactor *required = requiredFactor.as<PathOrInspFactor *>();
         builder_.popContext();
         if (required != nullptr) {
            definition->PathEls = required->PathEls;
            definition->_path = required->_path;
         }
      }
      c->ExistsIn.push_back(definition);
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitExistenceConstraint()");

   return c;

}

antlrcpp::Any Ili2Input::visitUniquenessConstraint(parser::Ili2Parser::UniquenessConstraintContext *ctx)
{

   /* uniquenessConstraint
   : UNIQUE 
     (WHERE expression COLON)?
     (globalUniqueness | localUniqueness) SEMI
   */

   /* class UniqueConstraint : public Constraint { // 2.4 struct -> class
   public:
      list<Expression *> Where;
      enum {GlobalU, LocalU} Kind;
      list<PathOrInspFactor *> UniqueDef;
   */

   builder_.debug(ctx,"visitUniquenessConstraint()");
   
   UniqueConstraint *c = builder_.store().make<UniqueConstraint>();
   builder_.initConstraint(c,builder_.line(ctx));
   c->PerBasket = ctx->BASKET() != nullptr;
   if (ctx->name != nullptr) {
      c->Name = ctx->name->getText();
      builder_.setSelectionSource(c,ctx->name);
   }
   
   if (ctx->WHERE() != nullptr) {
      c->Where.push_back(visitExpression(ctx->expression()));
   }
   
   if (ctx->globalUniqueness() != nullptr) {
      /* globalUniqueness
      : uniqueEl
      */
      c->Kind = UniqueConstraint::GlobalU;
      for (auto p: ctx->globalUniqueness()->uniqueEl()->objectOrAttributePath()) {
         PathOrInspFactor *pf = visitObjectOrAttributePath(p);
         c->UniqueDef.push_back(pf);
      }
   }
   else {
      /* localUniqueness
      : LPAREN LOCAL RPAREN localUniqueEl
      */
      /* localUniqueEl
      : structureattributename=NAME
      (RARROW structureattributename=NAME)* COLON
      attributename=NAME (COMMA attributename=NAME)*
      */
      c->Kind = UniqueConstraint::LocalU;
      PathOrInspFactor * pf = builder_.store().make<PathOrInspFactor>();
      pf->_path = ctx->localUniqueness()->localUniqueEl()->getText();
      c->UniqueDef.push_back(pf);
   }
   
   return c;

}

antlrcpp::Any Ili2Input::visitGlobalUniqueness(parser::Ili2Parser::GlobalUniquenessContext *ctx)
{

   /* globalUniqueness
   : uniqueEl
   */

   /* uniqueEl
   : objectOrAttributePath (COMMA objectOrAttributePath)*
   */

   /* struct PathOrInspFactor : public Factor {
   public:
      list <PathEl *> PathEls; // LIST
      View *Inspection = nullptr;
      string _path = "";
   */

   builder_.debug(ctx,">>> visitGlobalUniqueness()");
   logger_.incNestLevel();
   
   list<PathOrInspFactor *> result;
   // to do !!!

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitGlobalUniqueness()");
   
   return result;

}

antlrcpp::Any Ili2Input::visitUniqueEl(parser::Ili2Parser::UniqueElContext *ctx)
{

   /* uniqueEl
   : objectOrAttributePath (COMMA objectOrAttributePath)*
   */

   builder_.debug(ctx,"visitUniqueEl()");
   
   string path = "";
   for (auto p : ctx->objectOrAttributePath()) {
      /*string ps = visitPath(p);
      if (path == "") {
         path = ps;
      }
      else {
         path = path + "." + ps;
      }*/
   }
   
   return path;

}

antlrcpp::Any Ili2Input::visitLocalUniqueness(parser::Ili2Parser::LocalUniquenessContext *ctx)
{
   
   /* localUniqueness
   : LPAREN LOCAL RPAREN structureattributename=NAME
     (RARROW structureattributename=NAME)* COLON
     attributename=NAME (COMMA attributename=NAME)*
   */

   /* struct PathOrInspFactor : public Factor {
   public:
      list <PathEl *> PathEls; // LIST
      View *Inspection = nullptr;
      string _path = "";
   */

   builder_.debug(ctx,">>> visitLocalUniqueness()");
   logger_.incNestLevel();
   
   list<PathOrInspFactor *> result;
   // to do !!!

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitLocalUniqueness()");
   
   return result;

}

antlrcpp::Any Ili2Input::visitSetConstraint(parser::Ili2Parser::SetConstraintContext *ctx)
{

   /* setConstraint
   : SET CONSTRAINT (WHERE logical=expression COLON)? expression SEMI
   */

   /* class Constraint : public MetaElement { // ABSTRACT, 2.4
   public:
      // role from ASSOCIATION DomainConstraint
      DomainType *toDomain = nullptr; // 2.4
      // role from ASSOCIATION ClassConstraint
      Class *ToClass = nullptr; // 2.4
   */

   /* class SetConstraint : public Constraint { // 2.4 struct -> class
   public:
      list<Expression *> Where;
      Expression *Constraint = nullptr;
   */

   builder_.debug(ctx,">>> visitSetConstraint()");
   logger_.incNestLevel();
   
   SetConstraint *c = builder_.store().make<SetConstraint>();
   builder_.initConstraint(c,builder_.line(ctx));
   c->PerBasket = ctx->BASKET() != nullptr;
   if (ctx->name != nullptr) {
      c->Name = ctx->name->getText();
      builder_.setSelectionSource(c,ctx->name);
   }
   auto expressions = ctx->expression();
   if (ctx->logical != nullptr) {
      c->Where.push_back(visitExpression(ctx->logical));
   }
   if (!expressions.empty()) {
      c->Constraint = visitExpression(expressions.back());
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitSetConstraint()");
         
   return c;

}

antlrcpp::Any Ili2Input::visitConstraintsDef(parser::Ili2Parser::ConstraintsDefContext *ctx)
{

   /* constraintsDef
   : CONSTRAINTS OF path EQUAL
     (constraintDef)*
     END SEMI
   */

   builder_.debug(ctx,">>> visitConstraintsDef()");
   logger_.incNestLevel();
   
   Class *c = builder_.findClass(visit(ctx->path()),builder_.line(ctx));
   if (c != nullptr) {
      builder_.pushContext(*c);
      for (auto cctx : ctx->constraintDef()) { // ???, to do !!!
         Constraint *cc = visitConstraintDef(cctx);
         c->Constraint.push_back(cc);
      }
      builder_.popContext();
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitConstraintsDef()");
   
   return nullptr;

}
