#pragma once

#include "Ili1Input.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

antlrcpp::Any Ili1Input::visitTableDef(Ili1Parser::TableDefContext *ctx)
{

   /* tableDef
   : OPTIONAL?
     TABLE
     tablename1=NAME EQUAL
     attribute+
     identifications
     END tablename2=NAME SEMI
   */

   string name1 = ctx->tablename1->getText();
   string name2 = ctx->tablename2->getText();

   debug(ctx,">>> visitTableDef(" + name1 + ")");
   Log.incNestLevel();

   if (name1 != name2) {
      Log.error(
         "classname " + name2 + " must match " + name1,
         ctx->tablename2->getLine()
      );
   }

   Class *c = make_mmobject<Class>();

   // Class Attributes
   c->Name = name1;
   c->Kind = Class::ClassVal;
   init_type(c,get_line(ctx));
   if (ctx->OPTIONAL() != nullptr) {
      c->ili1OptionalTable = true;
   }
   list<metamodel::AttrOrParam *> classattribute;
   c->ClassAttribute = classattribute;
   add_class(c);

   push_context(c);

   for (auto actx : ctx->attribute()) {
      visitAttribute(actx);
   }
   visitIdentifications(ctx->identifications());

   pop_context();
   Log.decNestLevel();
   debug(ctx,"<<< visitTableDef(" + name1 + ")");
   
   return c;

}

antlrcpp::Any Ili1Input::visitAttribute(Ili1Parser::AttributeContext *ctx)
{

   /* attribute
   : attributename=NAME
     COLON
     OPTIONAL?
     (type | RARROW tablename=NAME)
     (explanation=EXPLANATION)?
     SEMI
   */

   string name = ctx->attributename->getText();
   if (is_reserved_name(name)) {
      name += "_ILI1";
   }

   debug(ctx,">>> visitAttribute(" + name + ")");
   Log.incNestLevel();

   AttrOrParam *a = make_mmobject<AttrOrParam>();
   init_extendableme(a,ctx->attributename->getLine());
   a->Name = name;
   
   push_context(a);
   DomainType *dt = nullptr;
   if (ctx->type() != nullptr) {
      Type *t = visitType(ctx->type());
      dt = static_cast<DomainType*>(t);
      if (t != nullptr) {
         a->Type = dt;
      }
   }
   else {
      // reference attribute
      ReferenceType *rt = make_mmobject<ReferenceType>();
      init_domaintype(rt,ctx->start->getLine());
      dt = static_cast<DomainType*>(rt);
      rt->External = false;
      rt->_baseclass = find_class(ctx->tablename->getText(),get_line(ctx));
      a->Type = dt;
   }
   pop_context();

   if (ctx->OPTIONAL() == nullptr && dt != nullptr) {
      if (dt != nullptr) {
         dt->Mandatory = true;
      }
   }
   
   if (ctx->EXPLANATION() != nullptr) {
      // can not be mapped to metamodel, ignored
   }

   // ASSOCIATION ClassAttr
   a->AttrParent = get_class_context();
   get_class_context()->ClassAttribute.push_back(a);

   Log.decNestLevel();
   debug(ctx,"<<< visitAttribute(" + name + ")");

   return a;

}

antlrcpp::Any Ili1Input::visitIdentifications(Ili1Parser::IdentificationsContext *ctx)
{

   /* identifications
   : NO IDENT
   | IDENT identification+
   */

   debug(ctx,">>> visitIdentifications()");
   Log.incNestLevel();

   if (ctx->NO() == nullptr) {
      for (auto ictx : ctx->identification()) {
         vector<string> attr_names = visitIdentification(ictx);
         /* class UniqueConstraint : public Constraint {
         public:
         list<Expression *> Where;
         enum {GlobalU, LocalU} Kind;
         list<PathOrInspFactor *> UniqueDef;
         */
         UniqueConstraint *c = make_mmobject<UniqueConstraint>();
         init_constraint(c,get_line(ctx));
         for (auto a : attr_names) {
            PathEl *pl = make_mmobject<PathEl>();
            pl->Kind = PathEl::Attribute;
            pl->Ref = find_attribute(get_class_context(),a);
            PathOrInspFactor * pf = make_mmobject<PathOrInspFactor>();
            pf->PathEls.push_back(pl);
            pf->_path = a;
            c->UniqueDef.push_back(pf);
         }
         get_class_context()->Constraints.push_back(c);
      }
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitIdentifications()");
   return nullptr;

}

antlrcpp::Any Ili1Input::visitIdentification(Ili1Parser::IdentificationContext *ctx)
{

   /* identification
   : NAME (COMMA NAME)* SEMI
   */

   debug(ctx,">>> visitIdentification()");
   Log.incNestLevel();

   vector<string> attr_names;
   for (auto n : ctx->NAME()) {
      attr_names.push_back(n->getText());
   }
  
   Log.decNestLevel();
   debug(ctx,"<<< visitIdentification()");

   return attr_names;

}