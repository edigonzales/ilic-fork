#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;
using namespace util;

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
   builder_.debug(ctx,">>> visitFunctionDef(" + name + ")");
   logger_.incNestLevel();

   FunctionDef *f = builder_.store().make<FunctionDef>();
   builder_.initMetaElement(f,ctx->start->getLine());
   builder_.setSelectionSource(f,ctx->functioname);

   // MetaElement attributes
   f->Name = name;
   builder_.addFunction(f);
   
   if (builder_.isIli23() && !builder_.currentModel()->Contracted) {
      logger_.error(DiagnosticId::FunctionContractedModelRequired,
         "functions can only be defined in contracted models",0);
   }
   
   // FunctionDef attributes
   if (ctx->EXPLANATION() != nullptr) {
      f->Explanation = ctx->EXPLANATION()->getText();
   }
   
   builder_.pushContext(*f);

   for (auto pctx : ctx->functionDefParam()) {
      Argument *a = builder_.store().make<Argument>();
      builder_.initMetaElement(a,ctx->start->getLine());
      builder_.setSelectionSource(a,pctx->NAME()->getSymbol());
      a->Name = pctx->NAME()->getText();
      builder_.debug(ctx,">>> visitArgument " + a->Name);
      logger_.incNestLevel();
      a->Kind = Argument::TypeVal; // to do !!!
      a->Type = visitArgumentType(pctx->argumentType());
      a->Type->Name = a->Name;
      if (a->Type != nullptr) {
         a->Type->LFTParent = f;
      }
      a->Function = f;
      f->Argument.push_back(a);
      logger_.decNestLevel();
      builder_.debug(ctx,"<<< visitArgument " + a->Name);
   }

   if (builder_.isIli23() && f->Argument.size() == 0) {
      logger_.error(DiagnosticId::FunctionDefinitionArgumentRequired,
         "function definition of " + name + "() needs at least one argument",
         builder_.line(ctx));
   }

   // f->LocalType ???
   builder_.debug(ctx,">>> visitResultType");
   logger_.incNestLevel();
   Type *t = visitArgumentType(ctx->result);
   f->ResultType = t;
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitResultType");

   builder_.popContext();

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitFunctionDef(" + name + ")");
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
   builder_.debug(ctx,">>> visitFunctionCall(" + name + ")");
   logger_.incNestLevel();

   FunctionCall *c = builder_.store().make<FunctionCall>();
   builder_.initFactor(c,builder_.line(ctx));
   
   c->Function = builder_.findFunction(name,builder_.line(ctx));
   if (c->Function != nullptr && c->Function->ResultType != nullptr) {
      c->_type = builder_.typeString(c->Function->ResultType);
   }
   else {
      c->_type = "???";
   }
   
   for (auto actx : ctx->functionCallArgument()) {
      c->Arguments.push_back(visitFunctionCallArgument(actx));
   }
   
   if (c->Function != nullptr) {
      if (c->Arguments.size() != c->Function->Argument.size()) {
         logger_.error(DiagnosticId::FunctionCallArgumentCount,
            "function call of " + name + "() needs " +
               to_string(c->Function->Argument.size()) + " arguments",builder_.line(ctx));
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
               logger_.error(DiagnosticId::FunctionArgumentTypeMismatch,
                  "incompatible type for " + name + "() argument " + formal->Name +
                  " (" + a->Expression->_type + "<>" + formal->Type->getClass() + ")",
                  builder_.line(ctx)
               );
            }
         }
      }
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitFunctionCall(" + name + ":" + c->_type + ")");
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

   builder_.debug(ctx,"visitFunctionCallArgument()");

   ActualArgument *a = builder_.store().make<ActualArgument>();
   builder_.initObject(a,ctx->start->getLine());

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

   builder_.debug(ctx,">>> visitArgumentType()");
   logger_.incNestLevel();
   
   Type *t = nullptr;

   if (ctx->attrTypeDef() != nullptr) {
      t = visitAttrTypeDef(ctx->attrTypeDef());
   }
   else if (ctx->OBJECT() != nullptr) {
      if (ctx->restrictedRef() != nullptr) {
         RestrictedRef *r = visitRestrictedRef(ctx->restrictedRef());
         ObjectType *o = builder_.store().make<ObjectType>();
         o->Multiple = false;
         o->_baseclass = r->_baseclass;
         t = o;
      }
      else {
         // viewRef, to do !!!
         logger_.internal_error("visitArgumentType(): viewRef not implemented",1);
      }
   }
   else if (ctx->OBJECTS() != nullptr) {
      if (ctx->restrictedRef() != nullptr) {
         RestrictedRef* r = visitRestrictedRef(ctx->restrictedRef());
         ObjectType* o = builder_.store().make<ObjectType>();
         o->Multiple = true;
         o->_baseclass = r->_baseclass;
         t = o;
      }
      else {
         // viewRef, to do !!!
         logger_.internal_error("visitArgumentType(): viewRef not implemented",1);
      }
   }
   else if (ctx->ENUMVAL() != nullptr) {
      EnumTreeValueType *tt = builder_.store().make<EnumTreeValueType>();
      // tt->ET = find_enumtreevalue(); to do !!!
      t = tt;
   }
   else if (ctx->ENUMTREEVAL() != nullptr) {
      EnumTreeValueType *tt = builder_.store().make<EnumTreeValueType>();
      // tt->ET = find_enumtreevalue(); to do !!!
      t = tt;
   }
   
   //t->LFTParent = dynamic_cast<FunctionDef *>(builder_.current());

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitArgumentType()");
   return t;

}
