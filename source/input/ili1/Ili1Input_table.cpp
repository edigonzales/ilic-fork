#include "Ili1Input.h"
#include "../../metamodel/MetaModelBuilder.h"
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

   builder_.debug(ctx,">>> visitTableDef(" + name1 + ")");
   logger_.incNestLevel();

   if (name1 != name2) {
      logger_.error(
         "classname " + name2 + " must match " + name1,
         ctx->tablename2->getLine()
      );
   }

   Class *c = builder_.store().make<Class>();

   // Class Attributes
   c->Name = name1;
   c->Kind = Class::ClassVal;
   builder_.initType(c,builder_.line(ctx));
   if (ctx->OPTIONAL() != nullptr) {
      c->ili1OptionalTable = true;
   }
   list<metamodel::AttrOrParam *> classattribute;
   c->ClassAttribute = classattribute;
   builder_.addClass(c);

   builder_.pushContext(*c);

   for (auto actx : ctx->attribute()) {
      visitAttribute(actx);
   }
   visitIdentifications(ctx->identifications());

   builder_.popContext();
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitTableDef(" + name1 + ")");
   
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
   if (builder_.isReservedName(name)) {
      name += "_ILI1";
   }

   builder_.debug(ctx,">>> visitAttribute(" + name + ")");
   logger_.incNestLevel();

   AttrOrParam *a = builder_.store().make<AttrOrParam>();
   builder_.initExtendable(a,ctx->attributename->getLine());
   a->Name = name;
   
   builder_.pushContext(*a);
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
      ReferenceType *rt = builder_.store().make<ReferenceType>();
      builder_.initDomainType(rt,ctx->start->getLine());
      dt = static_cast<DomainType*>(rt);
      rt->External = false;
      rt->_baseclass = builder_.findClass(ctx->tablename->getText(),builder_.line(ctx));
      a->Type = dt;
   }
   builder_.popContext();

   if (ctx->OPTIONAL() == nullptr && dt != nullptr) {
      if (dt != nullptr) {
         dt->Mandatory = true;
      }
   }
   
   if (ctx->EXPLANATION() != nullptr) {
      // can not be mapped to metamodel, ignored
   }

   // ASSOCIATION ClassAttr
   a->AttrParent = builder_.currentClass();
   builder_.currentClass()->ClassAttribute.push_back(a);

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitAttribute(" + name + ")");

   return a;

}

antlrcpp::Any Ili1Input::visitIdentifications(Ili1Parser::IdentificationsContext *ctx)
{

   /* identifications
   : NO IDENT
   | IDENT identification+
   */

   builder_.debug(ctx,">>> visitIdentifications()");
   logger_.incNestLevel();

   if (ctx->NO() == nullptr) {
      for (auto ictx : ctx->identification()) {
         vector<string> attr_names = visitIdentification(ictx);
         /* class UniqueConstraint : public Constraint {
         public:
         list<Expression *> Where;
         enum {GlobalU, LocalU} Kind;
         list<PathOrInspFactor *> UniqueDef;
         */
         UniqueConstraint *c = builder_.store().make<UniqueConstraint>();
         builder_.initConstraint(c,builder_.line(ctx));
         for (auto a : attr_names) {
            PathEl *pl = builder_.store().make<PathEl>();
            pl->Kind = PathEl::Attribute;
            pl->Ref = builder_.findAttribute(builder_.currentClass(),a);
            PathOrInspFactor * pf = builder_.store().make<PathOrInspFactor>();
            pf->PathEls.push_back(pl);
            pf->_path = a;
            c->UniqueDef.push_back(pf);
         }
         builder_.currentClass()->Constraints.push_back(c);
      }
   }
   
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitIdentifications()");
   return nullptr;

}

antlrcpp::Any Ili1Input::visitIdentification(Ili1Parser::IdentificationContext *ctx)
{

   /* identification
   : NAME (COMMA NAME)* SEMI
   */

   builder_.debug(ctx,">>> visitIdentification()");
   logger_.incNestLevel();

   vector<string> attr_names;
   for (auto n : ctx->NAME()) {
      attr_names.push_back(n->getText());
   }
  
   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitIdentification()");

   return attr_names;

}
