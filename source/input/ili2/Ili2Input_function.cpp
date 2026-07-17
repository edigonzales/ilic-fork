#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

antlrcpp::Any Ili2Input::visitFunctionDef(parser::Ili2Parser::FunctionDefContext *ctx)
{

   /*
   class FunctionDef : public MetaElement {
      // MetaElement.Name := FunctionName as defined in the INTERLIS-Model
   public:
      string Explanation;
      // role from ASSOCIATION LocalFType
      Type *LocalType; // ???, to do !!!
      Type *ResultType;
      // role from ASSOCIATION FormalArgument
      list <Argument *> Argument;
   };
   
   class Argument : public MetaElement {
      // MetaElement.Name := ArgumentName as defined in the INTERLIS-Model
   public:
      enum {TypeVal, EnumVal, EnumTreeVal} Kind;
      // role from ASSOCIATION FormalArgument =
      FunctionDef *Function;
      // role from ASSOCIATION ArgumentType
      Type *Type;
   };
   */

   /* functionDef
   : FUNCTION functioname=NAME
     LPAREN functionDefParam (SEMI functionDefParam)* RPAREN
     COLON result=argumentType 
     EXPLANATION? 
     SEMI
   */

   /* functionDefParam
   : argumentname=NAME COLON argumentType
   */

   /* functionArgument
   : argumentname=NAME COLON argumentType
   */
   
   string name = ctx->functioname->getText();
   debug(ctx,">>> visitFunctionDef(" + name + ")");
   Log.incNestLevel();

   FunctionDef *f = new FunctionDef;
   init_metaelement(f,ctx->start->getLine());

   // MetaElement attributes
   f->Name = name;
   add_function(f);
   
   if (ili23 && !get_model_context()->Contracted) {
      Log.error("functions can only be defined in contracted models");
   }
   
   // FunctionDef attributes
   if (ctx->EXPLANATION() != nullptr) {
      f->Explanation = ctx->EXPLANATION()->getText();
   }
   
   push_context(f);

   for (auto pctx : ctx->functionDefParam()) {
      Argument *a = new Argument();
      init_metaelement(a,ctx->start->getLine());
      a->Name = pctx->NAME()->getText();
      debug(ctx,">>> visitArgument " + a->Name);
      Log.incNestLevel();
      a->Kind = Argument::TypeVal; // to do !!!
      a->Type = visitArgumentType(pctx->argumentType());
      a->Type->Name = a->Name;
      if (a->Type != nullptr) {
         a->Type->LFTParent = f;
      }
      a->Function = f;
      f->Argument.push_back(a);
      Log.decNestLevel();
      debug(ctx,"<<< visitArgument " + a->Name);
   }

   if (ili23 && f->Argument.size() == 0) {
      Log.error("function definition of " + name + "() needs at least one argument",get_line(ctx));
   }

   // f->LocalType ???
   debug(ctx,">>> visitResultType");
   Log.incNestLevel();
   Type *t = visitArgumentType(ctx->result);
   f->ResultType = t;
   Log.decNestLevel();
   debug(ctx,"<<< visitResultType");

   pop_context();

   Log.decNestLevel();
   debug(ctx,"<<< visitFunctionDef(" + name + ")");
   return f;

}

antlrcpp::Any Ili2Input::visitFunctionCall(parser::Ili2Parser::FunctionCallContext *ctx)
{

   /*
   struct FunctionCall : public Factor {
   public:
      FunctionDef *Function;
      list <ActualArgument *> Arguments;
   };
   */

   /* functionCall
   : functionname=path LPAREN functionCallArgument (COMMA functionCallArgument)* RPAREN
   */

   string name = visitPath(ctx->path());
   debug(ctx,">>> visitFunctionCall(" + name + ")");
   Log.incNestLevel();

   FunctionCall *c = new FunctionCall();
   init_factor(c,get_line(ctx));
   
   c->Function = find_function(name,get_line(ctx));
   if (c->Function != nullptr && c->Function->ResultType != nullptr) {
      c->_type = get_type_string(c->Function->ResultType);
   }
   else {
      c->_type = "???";
   }
   
   for (auto actx : ctx->functionCallArgument()) {
      c->Arguments.push_back(visitFunctionCallArgument(actx));
   }
   
   if (c->Function != nullptr) {
      if (c->Arguments.size() != c->Function->Argument.size()) {
         Log.error("function call of " + name + "() needs " + to_string(c->Function->Argument.size()) + " arguments",get_line(ctx));
      }
      else {
         auto argp = c->Function->Argument.begin();
         for (auto a: c->Arguments) {
            Argument *formal = *argp;
            ++argp;
            a->FormalArgument = formal;
            if (formal->Type == nullptr || a->Kind == ActualArgument::AllOf || a->Expression == nullptr) {
               continue;
            }
            if (a->Expression->_type == "???") {
               continue;
            }
            if (!check_type_compatibility(formal->Type->getClass(),a->Expression->_type)) {
               Log.error(
                  "incompatible type for " + name + "() argument " + formal->Name +
                  " (" + a->Expression->_type + "<>" + formal->Type->getClass() + ")",
                  get_line(ctx)
               );
            }
         }
      }
   }

   Log.decNestLevel();
   debug(ctx,"<<< visitFunctionCall(" + name + ":" + c->_type + ")");
   return c;

}

antlrcpp::Any Ili2Input::visitFunctionCallArgument(parser::Ili2Parser::FunctionCallArgumentContext *ctx)
{

   /*struct ActualArgument : public MMObject {
   public:
      Argument *FormalArgument;
      enum {ExpressionVal, AllOf} Kind;
      list <Expression *> Expression;
      list <ClassRef *> ObjectClasses;
   };
   */

   /* functionCallArgument
   : (expression
     | ALL (LPAREN restrictedRef | path RPAREN)* )
   */

   debug(ctx,"visitFunctionCallArgument()");

   ActualArgument *a = new ActualArgument();
   init_mmobject(a,ctx->start->getLine());

   if (ctx->expression() != nullptr) {
      a->Kind = ActualArgument::ExpressionVal;
      a->Expression = visitExpression(ctx->expression());
   }
   else {
      a->Kind = ActualArgument::AllOf;
      // a->ObjectClasses, to do !!!
   }

   return a;

}

antlrcpp::Any Ili2Input::visitArgumentType(parser::Ili2Parser::ArgumentTypeContext *ctx)
{

   /* argumentType
   : attrTypeDef
   | (OBJECT | OBJECTS) OF (restrictedRef | viewref=path)
   | ENUMVAL 
	| ENUMTREEVAL
   */

   debug(ctx,">>> visitArgumentType()");
   Log.incNestLevel();
   
   Type *t = nullptr;

   if (ctx->attrTypeDef() != nullptr) {
      t = visitAttrTypeDef(ctx->attrTypeDef());
   }
   else if (ctx->OBJECT() != nullptr) {
      if (ctx->restrictedRef() != nullptr) {
         RestrictedRef *r = visitRestrictedRef(ctx->restrictedRef());
         ObjectType *o = new ObjectType;
         o->Multiple = false;
         o->_baseclass = r->_baseclass;
         t = o;
      }
      else {
         // viewRef, to do !!!
         Log.internal_error("visitArgumentType(): viewRef not implemented",1);
      }
   }
   else if (ctx->OBJECTS() != nullptr) {
      if (ctx->restrictedRef() != nullptr) {
         RestrictedRef* r = visitRestrictedRef(ctx->restrictedRef());
         ObjectType* o = new ObjectType;
         o->Multiple = true;
         o->_baseclass = r->_baseclass;
         t = o;
      }
      else {
         // viewRef, to do !!!
         Log.internal_error("visitArgumentType(): viewRef not implemented",1);
      }
   }
   else if (ctx->ENUMVAL() != nullptr) {
      EnumTreeValueType *tt = new EnumTreeValueType();
      // tt->ET = find_enumtreevalue(); to do !!!
      t = tt;
   }
   else if (ctx->ENUMTREEVAL() != nullptr) {
      EnumTreeValueType *tt = new EnumTreeValueType();
      // tt->ET = find_enumtreevalue(); to do !!!
      t = tt;
   }
   
   //t->LFTParent = dynamic_cast<FunctionDef *>(get_context());

   Log.decNestLevel();
   debug(ctx,"<<< visitArgumentType()");
   return t;

}
