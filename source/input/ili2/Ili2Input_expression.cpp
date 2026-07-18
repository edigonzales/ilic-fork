#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

/*

   class Expression : public MMObject { // ABSTRACT
   };

   struct UnaryExpr : public Expression {
   public:
      enum {Not, Defined} Operation;
      Expression *SubExpression;
   };

   struct CompoundExpr : public Expression {
   public:
      enum {And, Or, Mult, Div,
            Relation_Equal, Relation_NotEqual,
            Relation_LessOrEqual, Relation_GreaterOrEqual,
            Relation_Less, Relation_Greater} Operation;
      list <Expression *> SubExpressions;
   };

   struct Factor : public Expression { // ABSTRACT
   };

   struct PathEl : public MMObject {
   public:
      enum {This, ThisArea, ThatArea, Parent,
            ReferenceAttr, AssocPath, Role, ViewBase,
            Attribute, MetaObject} Kind;
      MetaElement *Ref;
      int NumIndex;
      enum {First, Last} SpecIndex;
      virtual string getClass() { return "PathEl"; }
   };

   struct PathOrInspFactor : public Factor {
   public:
      list <PathEl *> PathEls;
      View *Inspection;
      virtual string getClass() { return "PathOrInspFactor"; }
   };

   struct EnumAssignment : public MMObject {
   public:
      Expression *ValueToAssign;
      EnumNode *MinEnumValue;
      EnumNode *MaxEnumValue;
      virtual string getClass() { return "EnumAssignment"; }
   };

   struct EnumMapping : public Factor {
   public:
      PathOrInspFactor *EnumValue;
      list <EnumAssignment *> Cases;
      virtual string getClass() { return "EnumMapping"; }
   };

*/

antlrcpp::Any Ili2Input::visitExpression(parser::Ili2Parser::ExpressionContext* ctx)
{

   /* expression
   : term1 ({ili24}? IMPL term1)?
   */

   debug(ctx, ">>> visitExpression()");
   Log.incNestLevel();

   Expression* e = visitTerm1(ctx->term1());

   Log.decNestLevel();
   if (e != nullptr) {
      debug(ctx, "<<< visitExpression(" + e->_type + ")");
   }
   else {
      debug(ctx, "<<< visitExpression(???)");
   }
   
   return e;

}

antlrcpp::Any Ili2Input::visitTerm1(parser::Ili2Parser::Term1Context *ctx)
{

   /* term1
   : term2 (operator1 term2)*
   */

   /* operator1
   : OR
   | {ili24}? PLUS
   | {ili24}? MINUS
   | {ili24}? IMPL
   */

   /* struct CompoundExpr : public Expression {
   public:
      enum {
         Implication, And, Or, Mult, Div, // 2.4 Implication
         Relation_Equal, Relation_NotEqual,
         Relation_LessOrEqual, Relation_GreaterOrEqual,
         Relation_Less, Relation_Greater
      } Operation;
      list <Expression*> SubExpressions;
   */

   debug(ctx,">>> visitTerm1()");
   Log.incNestLevel();
   
   Expression *e = nullptr;

   if (ctx->term2().size() == 1) {
      e = visitTerm2(ctx->term2().front());
   }
   else {
      CompoundExpr *ce = new CompoundExpr();
      init_expression(ce,get_line(ctx));
      if (ctx->operator1().front()->OR() != nullptr) {
         ce->Operation = CompoundExpr_OperationType::Or;
         ce->_type = "BooleanType";
      }
      else if (ctx->operator1().front()->PLUS() != nullptr) {
         ce->Operation = CompoundExpr_OperationType::Plus;
         ce->_type = "NumType";
      }
      else if (ctx->operator1().front()->MINUS() != nullptr) {
         ce->Operation = CompoundExpr_OperationType::Minus;
         ce->_type = "NumType";
      }
      else if (ctx->operator1().front()->IMPL() != nullptr) {
         ce->Operation = CompoundExpr_OperationType::Implication;
         ce->_type = "BooleanType";
      }
      for (auto t : ctx->term2()) {
         Expression *e2 = visitTerm2(t);
         ce->SubExpressions.push_back(e2);
      }
      e = ce;
   }
      
   Log.decNestLevel();
   debug(ctx, "<<< visitTerm1(" + e->_type + ")");
   return e;
   
}

antlrcpp::Any Ili2Input::visitTerm2(parser::Ili2Parser::Term2Context *ctx)
{

   /* term2
   : term3 (operator2 term3)*
   */
   
   /* operator2
   : AND
   | {ili24}? STAR
   | {ili24}? SLASH
   */

   debug(ctx,">>> visitTerm2()");
   Log.incNestLevel();

   Expression *e = nullptr;

   if (ctx->term3().size() == 1) {
      e = visitTerm3(ctx->term3().front());
   }
   else {
      CompoundExpr *ce = new CompoundExpr();
      init_expression(ce,get_line(ctx));
      if (ctx->operator2().front()->AND() != nullptr) {
         ce->Operation = CompoundExpr_OperationType::And;
         ce->_type = "BooleanType";
      }
      else if (ctx->operator2().front()->STAR() != nullptr) {
         ce->Operation = CompoundExpr_OperationType::Mult;
         ce->_type = "NumType";
      }
      else if (ctx->operator2().front()->SLASH() != nullptr) {
         ce->Operation = CompoundExpr_OperationType::Div;
         ce->_type = "NumType";
      }
      for (auto t : ctx->term3()) {
         Expression *e2 = visitTerm3(t);
         if (e2 != nullptr) {
            ce->SubExpressions.push_back(e2);
         }
      }
      e = ce;
   }

   Log.decNestLevel();
   if (e != nullptr) {
      debug(ctx, "<<< visitTerm2(" + e->_type + ")");
   }
   else {
      debug(ctx, "<<< visitTerm2(???)");
   }

   return e;
   
}

static string get_operation_type(int operation,Expression *e1,Expression *e2)
{

   /* enum {And, Or, Mult, Div,
            Relation_Equal, Relation_NotEqual,
            Relation_LessOrEqual, Relation_GreaterOrEqual,
            Relation_Less, Relation_Greater} Operation;
   */

   if (operation == CompoundExpr_OperationType::And) {
      if (e1->_type != "BooleanType") {
         Log.error("and: term is not of boolean type",e1->_line);
      }
      if (e2->_type != "BooleanType") {
         Log.error("and: term is not of boolean type",e1->_line);
      }
      return "BooleanType";
   }
   else if (operation == CompoundExpr_OperationType::Or) {
      if (e1->_type != "BooleanType") {
         Log.error("or: term is not of boolean type",e1->_line);
      }
      if (e2->_type != "BooleanType") {
         Log.error("or: term is not of boolean type",e1->_line);
      }
      return "BooleanType";
   }
   else if (operation == CompoundExpr_OperationType::Mult) {
      if (e1->_type != "NumericType") {
         Log.error("*: term is not of numeric type",e1->_line);
      }
      if (e2->_type != "NumericType") {
         Log.error("*: term is not of numeric type",e1->_line);
      }
      return "NumericType";
   }
   else if (operation == CompoundExpr_OperationType::Div) {
      if (e1->_type != "NumericType") {
         Log.error("/: term is not of numeric type",e1->_line);
      }
      if (e2->_type != "NumericType") {
         Log.error("/: term is not of numeric type",e1->_line);
      }
      return "NumericType";
   }
   else if (operation == CompoundExpr_OperationType::Relation_Equal) {
      if (e1->_type == "???") {
      }
      else if (e2->_type == "???") {
      }
      else if (e1->_type != e2->_type) {
         Log.error("==: term1 and term2 have incompatible datatypes (" + e1->_type + "<>" + e2->_type + ")",e1->_line);
      }
      return "BooleanType";
   }
   else if (operation == CompoundExpr_OperationType::Relation_NotEqual) {
      if (e1->_type == "???") {
      }
      else if (e2->_type == "???") {
      }
      else if (e1->_type != e2->_type) {
         Log.error("!=: term1 and term2 have incompatible datatypes (" + e1->_type + "<>" + e2->_type + ")",e1->_line);
      }
      return "BooleanType";
   }

   // other operations, to do !!!
   
   return "???";

}
   
antlrcpp::Any Ili2Input::visitTerm3(parser::Ili2Parser::Term3Context *ctx)
{

   /* term3
   : t1=term (relation t2=term)?
   */

   debug(ctx,">>> visitTerm3()");
   Log.incNestLevel();

   Expression *e = nullptr;
   
   if (ctx->relation() == nullptr) {
      /* struct UnaryExpr : public Expression {
      public:
         enum {Not, Defined, None} Operation;
         Expression *SubExpression = nullptr;
      */
      UnaryExpr *u = new UnaryExpr();
      init_expression(u,get_line(ctx));
      u->Operation = UnaryExpr::None;
      u->SubExpression = visitTerm(ctx->t1);
      if (u->SubExpression != nullptr) {
         u->_type = u->SubExpression->_type;
      }
      else {
         u->_type = "???";
      }
      e = u;
   }
   else {
      /*
      struct CompoundExpr : public Expression {
      public:
         enum {And, Or, Mult, Div,
               Relation_Equal, Relation_NotEqual,
               Relation_LessOrEqual, Relation_GreaterOrEqual,
               Relation_Less, Relation_Greater} Operation;
         list <Expression *> SubExpressions;
         virtual string getClass() { return "CompoundExpr"; }
      };
      */
      CompoundExpr *c = new CompoundExpr();
      init_expression(c,get_line(ctx));
      c->Operation = static_cast<CompoundExpr_OperationType>(visitRelation(ctx->relation()));
      Expression *e1 = visitTerm(ctx->t1);
      if (e1 != nullptr) {
         c->SubExpressions.push_back(e1);
      }
      Expression *e2 = visitTerm(ctx->t2);
      if (e2 != nullptr) {
         c->SubExpressions.push_back(e2);
      }
      if (c->Operation == Mult) {
         c->_type = "NumType";
      }
      else if (c->Operation == Div) {
         c->_type = "NumType";
      }
      else {
         c->_type = "BooleanType";
      }
      e = c;
   }

   Log.decNestLevel();

   if (e != nullptr) {
      debug(ctx, "<<< visitTerm3(" + e->_type + ")");
   }
   else {
      debug(ctx, "<<< visitTerm3(???)");
   }

   return e;
   
}
   
antlrcpp::Any Ili2Input::visitTerm(parser::Ili2Parser::TermContext *ctx)
{

   /* term
   : factor                       
   | NOT? LPAREN expression RPAREN // predicate
   | DEFINED LPAREN factor RPAREN
   */
   
   debug(ctx,">>> visitTerm()");
   Log.incNestLevel();
   
   Expression *e = nullptr;

   if (ctx->expression() != nullptr) {
      /* struct UnaryExpr : public Expression {
      public:
         enum {Not, Defined, None} Operation;
         Expression *SubExpression;
      };
      */
      UnaryExpr *u = new UnaryExpr();
      init_expression(u,get_line(ctx));
      if (ctx->NOT() != nullptr) {
         u->Operation = UnaryExpr::Not;
      }
      else {
         u->Operation = UnaryExpr::None;
      }
      u->SubExpression = visitExpression(ctx->expression());
      if (u->SubExpression != nullptr) {
         u->_type = u->SubExpression->_type;
      }
      else {
         u->_type = "???";
      }
      e = u;
   }
   else {
      /* struct Factor : public Expression { // ABSTRACT
      public:
      */
      UnaryExpr *u = new UnaryExpr();
      init_expression(u,get_line(ctx));
      u->Operation = UnaryExpr::Defined;
      Factor *f = visitFactor(ctx->factor());
      if (ctx->DEFINED() != nullptr) {
         u->Operation = UnaryExpr::Defined;
         u->_type = "BooleanType";
      }
      else {
         u->Operation = UnaryExpr::None;
         u->_type = f->_type;
      }
      u->SubExpression = f;
      e = u;
   }

   Log.decNestLevel();

   if (e != nullptr) {
      debug(ctx, "<<< visitTerm(" + e->_type + ")");
   }
   else {
      debug(ctx, "<<< visitTerm(???)");
   }

   return e;
   
}
   
antlrcpp::Any Ili2Input::visitRelation(parser::Ili2Parser::RelationContext *ctx)
{

   /* CompoundExpr_OperationType::enum {
      And, Or, Mult, Div,
      Relation_Equal, Relation_NotEqual,
      Relation_LessOrEqual, Relation_GreaterOrEqual,
      Relation_Less, Relation_Greater
   */

   /* relation
   : EQUALEQUAL 
   | NOTEQUAL    
   | LESSGREATER
   | LESSEQUAL  
   | GREATEREQUAL
   | LESS        
   | GREATER 
   */
   
   debug(ctx,">>> visitRelation()");

   CompoundExpr_OperationType relation;

   if (ctx->EQUALEQUAL() != nullptr) {
      relation = Relation_Equal;
   }
   else if (ctx->NOTEQUAL() != nullptr) {
      relation = Relation_NotEqual;
   }
   else if (ctx->LESSGREATER() != nullptr) {
      relation = Relation_NotEqual; // ???
   }
   else if (ctx->LESSEQUAL() != nullptr) {
      relation = Relation_LessOrEqual;
   }
   else if (ctx->GREATEREQUAL() != nullptr) {
      relation = Relation_GreaterOrEqual;
   }
   else if (ctx->LESS() != nullptr) {
      relation = Relation_Less;
   }
   else { // greater
      relation = Relation_Greater;
   }
   
   debug(ctx, "<<< visitRelation() " + to_string(relation));
   return relation;
   
}

antlrcpp::Any Ili2Input::visitFactor(parser::Ili2Parser::FactorContext *ctx)
{

   /* factor
      : objectpath=objectOrAttributePath
      | (inspection | INSPECTION path) (OF inspectionpath = objectOrAttributePath)?
      | functionCall
      | PARAMETER parampath = path
      | constant
   */

   /* struct Expression : public MMObject { // ABSTRACT
   public:
      string _type;
   */

   /* struct Factor : public Expression { // ABSTRACT
   */

   debug(ctx,">>> visitFactor()");
   Log.incNestLevel();
   
   Factor *f = nullptr;

   if (ctx->objectOrAttributePath() != nullptr) {
      PathOrInspFactor *pif = visitObjectOrAttributePath(ctx->objectpath);
      f = pif;
   }
   else if (ctx->inspection() != nullptr) {
      f = visitInspection(ctx->inspection()); // ???, to do !!!
   }
   else if (ctx->INSPECTION() != nullptr) {
      // tod do !!!
   }
   else if (ctx->functionCall() != nullptr) {
      FunctionCall *fc = visitFunctionCall(ctx->functionCall());
      f = fc;
   }
   else if (ctx->PARAMETER() != nullptr) {
      RuntimeParamRef *r = new RuntimeParamRef;
      init_factor(r,get_line(ctx->parampath));
      r->RuntimeParam = nullptr;
      string name = ctx->parampath->getText();
      for (auto p : get_model_context()->_runtimeparameter) {
         if (name == p->Name) {
            r->RuntimeParam = p;
            break;
         }
      }
      if (r->RuntimeParam == nullptr) {
         Log.error("unknown runtime parameter " + name,get_line(ctx));
      }
      f = r;
   }
   else if (ctx->constant() != nullptr) {
      Factor *c = visitConstant(ctx->constant());
      f = c;
   }

   Log.decNestLevel();
   debug(ctx, "<<< visitFactor(" + f->_type + ")");
   return f;

}

antlrcpp::Any Ili2Input::visitConstant(parser::Ili2Parser::ConstantContext *ctx)
{

   /* struct Constant : public Factor {
   public:
      string Value;
      enum {Undefined, Numeric, Text, Enumeration} Kind;
   */

   /* constant
   : UNDEFINED
   | numericConst
   | textConst
   | formattedConst
   | enumConst
   | classConst
   | attributePathConst
   */

   debug(ctx,">>> visitConstant()");
   Log.incNestLevel();
   
   Factor *c = nullptr;
   
   if (ctx->UNDEFINED() != nullptr) {
      c = new Constant();
      init_factor(c,get_line(ctx));
      static_cast<Constant *>(c)->Kind = Constant::Undefined;
      c->_type = "UNDEFINED";
   }
   else if (ctx->numericConst() != nullptr) {
      Constant *constant = visitNumericConst(ctx->numericConst());
      c = constant;
   }
   else if (ctx->textConst() != nullptr) {
      Constant *constant = visitTextConst(ctx->textConst());
      c = constant;
   }
   else if (ctx->formattedConst() != nullptr) {
      Constant *constant = visitFormattedConst(ctx->formattedConst());
      c = constant;
   }
   else if (ctx->enumConst() != nullptr) {
      Constant *constant = visitEnumConst(ctx->enumConst());
      c = constant;
   }
   else if (ctx->classConst() != nullptr) {
      ClassConst *constant = visitClassConst(ctx->classConst());
      c = constant;
   }
   else if (ctx->attributePathConst() != nullptr) {
      AttributeConst *constant = visitAttributePathConst(ctx->attributePathConst());
      c = constant;
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitConstant(" + c->_type + ")");
   
   return c;
   
}

antlrcpp::Any Ili2Input::visitDecConst(parser::Ili2Parser::DecConstContext *ctx)
{

   /* decConst
   : dec=decimal
   | PI 
   | LNBASE
   */

   debug(ctx,">>> visitDecConst()");
   Log.incNestLevel();
   
   Constant *c = new Constant();
   init_factor(c,get_line(ctx));
   c->Kind = Constant::Numeric;
   c->_type = "NumType";
   
   if (ctx->PI() != nullptr) {
      c->Value = "PI";
   }
   else if (ctx->LNBASE() != nullptr) {
      c->Value = "e";
   }
   else {
      // c->Value = visitDecimal(ctx->dec); does not compile, why?
      string val = visitDecimal(ctx->dec);
      c->Value = val;
   }

   Log.decNestLevel();
   debug(ctx,"<<< visitDecConst(" + c->Value + ")");
   return c;
   
}

antlrcpp::Any Ili2Input::visitNumericConst(parser::Ili2Parser::NumericConstContext *ctx)
{

   /* numericConst
   : decConst (LBRACE unitref=path RBRACE)?
   */

   debug(ctx,">>> visitNumericConst()");
   Log.incNestLevel();

   Constant *c = visitDecConst(ctx->decConst());
   
   if (ctx->path() != nullptr) {
      Unit *u = find_unit(ctx->unitref->getText(),ctx->unitref->start->getLine());
      // unit assignment, to do !!!
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitNumericConst(" + c->Value + ")");
   
   return c;
   
}

antlrcpp::Any Ili2Input::visitTextConst(parser::Ili2Parser::TextConstContext *ctx)
{

   /* textConst
   : textconst=STRING
   */

   debug(ctx,">>> visitTextConst()");
   Log.incNestLevel();

   Constant *c = new Constant();
   init_factor(c,get_line(ctx));
   c->Kind = Constant::Text;
   c->_type = "TextType";
   c->Value = visitString(ctx->textconst);
   
   Log.decNestLevel();
   debug(ctx,"<<< visitTextConst(" + c->Value + ")");
   return c;
   
}

antlrcpp::Any Ili2Input::visitEnumConst(parser::Ili2Parser::EnumConstContext *ctx)
{

   /* enumConst
   : HASH (NAME (DOT NAME)* (DOT OTHERS)?
   | OTHERS)
   */
   
   debug(ctx,">>> visitEnumConst()");
   Log.incNestLevel();

   Constant *c = new Constant();
   init_factor(c,get_line(ctx));
   c->Kind = Constant::Enumeration;
   c->_type = "EnumType";
   c->Value = "";
   for (auto n : ctx->NAME()) {
      if (c->Value == "") {
         c->Value = n->getText();
      }
      else {
         c->Value = c->Value + "." + n->getText();
      }
   }
   
   if (c->Value == "true" || c->Value == "false" ||
       c->Value == "INTERLIS.true" || c->Value == "INTERLIS.false") {
      c->_type = "BooleanType";
   }

   Log.decNestLevel();
   debug(ctx,"<<< visitEnumConst(" + c->Value + ")");
   return c;
   
}

antlrcpp::Any Ili2Input::visitClassConst(parser::Ili2Parser::ClassConstContext *ctx)
{

   /* classConst
   : GREATER classref=path
   */

   debug(ctx,">>> visitClassConst()");
   Log.incNestLevel();

   ClassConst *c = new ClassConst();
   init_factor(c,get_line(ctx));
   c->_type = "ClassRefType";
   c->Class = find_class(visitPath(ctx->path()),get_line(ctx));

   Log.decNestLevel();
   debug(ctx,"<<< visitClassConst()");
   return c;
   
}

antlrcpp::Any Ili2Input::visitAttributePathConst(parser::Ili2Parser::AttributePathConstContext *ctx)
{
   
   /* attributePathConst
   : GREATERGREATER (classref=path RARROW)? attribute=NAME
   */

   debug(ctx,">>> visitAttributePathConst()");
   Log.incNestLevel();

   AttributeConst *c = new AttributeConst();
   init_factor(c,get_line(ctx));
   c->_type = "AttributeRefType";
   if (ctx->path() != nullptr) {
      string path = visitPath(ctx->path());
      Class *owner = find_class(path,get_line(ctx));
      c->Attribute = owner == nullptr ? nullptr : find_attribute(owner,ctx->NAME()->getText());
   }
   else {
      c->Attribute = find_attribute(get_class_context(),ctx->NAME()->getText());
   }
   if (c->Attribute == nullptr) {
      Log.error("attribute " + ctx->NAME()->getText() + " not found",get_line(ctx));
   }

   Log.decNestLevel();
   debug(ctx,"<<< visitAttributePathConst()");
   return c;
   
}

antlrcpp::Any Ili2Input::visitFormattedConst(parser::Ili2Parser::FormattedConstContext *ctx)
{

   /* formattedConst
   : formattedconst=STRING
   */

   debug(ctx,">>> visitFormattedConst()");
   Log.incNestLevel();

   Constant *c = new Constant();
   init_factor(c,get_line(ctx));
   c->Kind = Constant::Text;
   c->_type = "TextType";
   c->Value = visitString(ctx->formattedconst);

   Log.decNestLevel();
   debug(ctx,"visitFormattedConst(" + c->Value + ")");
   return c;
   
}
