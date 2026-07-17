#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

antlrcpp::Any Ili2Input::visitAssociationDef(parser::Ili2Parser::AssociationDefContext *ctx)
{

   /* associationDef
   : ASSOCIATION (associationname1=NAME)? properties? // ABSTRACT|EXTENDED|FINAL|OID +++
     (EXTENDS associationRef)?
     (DERIVED FROM renamedViewableRef)? +++
     EQUAL
     ((OID AS assocoid=path | NO OID) SEMI)?
     (roleDef)* +++
     ATTRIBUTE? (attributeDef)*
     (CARDINALITY EQUAL cardinality SEMI)? +++
     (constraintDef)*
     END (associationname2=NAME)? SEMI
   */

   /* class Class : public Type {
      // MetaElement.Name := StructureName, ClassName,
      //                     AssociationName, ViewName
      //                     as defined in the INTERLIS-Model
   public:
      enum {Structure,ClassVal,ViewVal,Association} Kind;
      Multiplicity Multiplicity; // for associations only
      list<Constraint *> Constraints;
      bool EmbeddedRoleTransfer = false;
      bool ili1OptionalTable = false;
      // role from ASSOCIATION LocalType
      metamodel::AttrOrParam *LTParent = nullptr;
      // role from ASSOCIATION ClassAttr
      list<metamodel::AttrOrParam *> ClassAttribute;
      // role from ASSOCIATION AssocRole
      list<Role *> Role;
      // role from ExplicitAssocAcc
      list<ExplicitAssocAccess *> ExplicitAssocAccess;
      // role from ASSOCIATION MetaObjectClass
      list<MetaObjectDef *> MetaObjectDef;
      // role from ASSOCIATION StructOfFormat
      list<FormattedType *> FormattedType;
      // role form ASSOCIATION ObjectOID
      DomainType *Oid = nullptr; // RESTRICTION(TextType; NumType; AnyOIDType)
      // role from ASSOCIATION ARefOf
      list<AttributeRefType *> ForARef;
      // role from ASSOCIATION LineFormStructure
      list<LineForm *> LineForm;
      // role from ASSOCIATION LineAttr
      list<LineType *> LineType;
      // role from ASSOCIATION BaseViewRef
      list<RenamedBaseView *> RenamedBaseView;
      // role from ASSOCIATION DerivedAssoc
      View *View = nullptr;
      // role from ASSOCIATION GraphicBase
      //list<Graphic *> Graphic;
      // role from ASSOCIATION SignClass
      list <DrawingRule *> DrawingRule;
      // from from ASSOCIATION ClassConstraint
      list<Constraint *> Constraint;
   */
   
   string name1 = "???";
   if (ctx->associationname1 != nullptr) {
      name1 = ctx->associationname1->getText();
   }
   
   string name2 = "???";
   if (ctx->associationname2 != nullptr) {
      name2 = ctx->associationname2->getText();
   }
   
   debug(ctx,">>> visitAssociationDef(" + name1 + ")");
   Log.incNestLevel();
   
   if (name1 != name2) {
      Log.error(name2 + " does not match " + name1,ctx->END()->getSymbol()->getLine());
   }

   // init Class
   Class *c = new Class();
   c->Kind = Class::Association;
   init_type(c,get_line(ctx));

   // MetaElement Attributes
   c->Name = name1;

   // ExtendableME Attributes
   map<string,bool> properties = get_properties(ctx->properties(),vector<string>({ABSTRACT,EXTENDED,FINAL,OID}));
   c->Abstract = properties[ABSTRACT];
   c->Final = properties[FINAL];
   c->Extended = properties[EXTENDED];

   // EXTENDED
   if (c->Extended) {
      DataUnit* u = find_dataunit(get_path(get_package_context()),c->_line);
      if (u->Super == nullptr) {
         Log.error(string("EXTENDED can only by used in extended topics"),c->_line);
      }
      else {
         Class *s = find_association(name1,c->_line);
         c->Super = s;
         if (s != nullptr) {
            s->Sub.push_back(c);
            if (s->Final) {
               Log.error("association " + name1 + " can not extend FINAL base association " + get_path(s),c->_line);
            }
         }
      }
   }
   
   // EXTENDS
   if (ctx->associationRef() != nullptr) {
      Class *s = find_association(ctx->associationRef()->getText(),c->_line);
      c->Super = s;
      if (s != nullptr) {
         s->Sub.push_back(c);
         if (s->Final) {
            Log.error("association " + name1 + " can not extend FINAL base association " + get_path(s),c->_line);
         }
      }
   }
   
   if (properties[OID]) {
      c->OidProperty = true;
   }
   
   if (ctx->DERIVED() != nullptr) {
      c->View = find_view(visitPath(ctx->renamedViewableRef()->path()),get_line(ctx->renamedViewableRef()));
   }

   // role from ASSOCIATION LocalType
   // metamodel::AttrOrParam *LTParent;

   if (ctx->assocoid != nullptr) {
      DomainType *t = find_domaintype(ctx->assocoid->getText(),get_line(ctx->assocoid));
      c->Oid = t;
   }
   c->NoOid = ctx->NO() != nullptr;

   // role from ASSOCIATION DerivedAssoc
   // View *View;
   
   add_class(c);
   push_context(c);

   for (auto rctx : ctx->roleDef()) {
      Role *r = visitRoleDef(rctx);
      if (r->Abstract && !c->Abstract) {
         Log.error("concrete association " + get_path(c) + " can not contain abstract role " + r->Name,get_line(rctx));
      }
      if (c->Super != nullptr && !r->Extended) {
         Log.error("can not add role " + r->Name + " in extended association",get_line(rctx));
      }
      else {
         r->Association = c;
         c->Role.push_back(r);
      }
   }

   int rolecount = 0;
   Class *cc = c;
   while (cc != nullptr) {
      rolecount += cc->Role.size();
      cc = static_cast<Class *>(cc->Super);
   }
   if (rolecount < 2) {
      Log.error("an association requires at least two roles",get_line(ctx));
   }
   
   if (ctx->CARDINALITY() != nullptr) {
      c->Multiplicity = visitCardinality(ctx->cardinality());
   }

   for (auto *actx : ctx->attributeDef()) {
      AttrOrParam *a = visitAttributeDef(actx);
      a->AttrParent = c;
      //c->ClassAttribute.push_back(a);
   }

   for (auto r1: c->Role) {
      for (auto r2: c->Role) {
         if (r1 != r2 && r1->_baseclass != nullptr) {
            if (r1->External) {
               continue;
            }
            else if (r2->Extended) {
               continue;
            }
            if (find_attribute(r1->_baseclass,r2->Name)) {
               Log.error("attribute with name " + r2->Name + " already exists in " + get_path(r1->_baseclass),r2->_line); 
            }
            else if (find_role(r1->_baseclass,r2->Name)) {
               Log.error("role or roleaccess with name " + r2->Name + " already exists in " + get_path(r1->_baseclass),r2->_line); 
            }
            else {
               r1->_baseclass->_roleaccess.push_back(r2);
            }
         }
      }
   }
   
   for (auto cctx : ctx->constraintDef()) {
      Constraint *cc = visitConstraintDef(cctx);
      c->Constraints.push_back(cc);
   }

   check_references(c,"",0);
   pop_context();

   Log.decNestLevel();
   debug(ctx,"<<< visitAssociationDef(" + name1 + ")");
   
   return c;

}

antlrcpp::Any Ili2Input::visitRoleDef(parser::Ili2Parser::RoleDefContext *ctx)
{
   
   /* roleDef
   : rolename=NAME 
     properties? // ABSTRACT|EXTENDED|FINAL|HIDING|ORDERED|EXTERNAL
     (ASSOCIATE | AGGREGATE | COMPOSITE ) cardinality?
     restrictedRef (OR restrictedRef)*
     (COLONEQUAL role=factor)? SEMI
   */
   
   /*
   class Role : public ReferenceType {
      // MetaElement.Name := RoleName as defined in the INTERLIS-Model
   public:
      enum {Assoc, Aggr, Comp} Strongness;
      bool Ordered = false;
      Multiplicity Multiplicity;
      list<Expression *> Derivates;
      bool EmbeddedTransfer = false;
      // role from ASSOCIATION AssocRole
      Class *Association;
      // role from ASSOCIATION AssocAccOrign
      list<ExplicitAssocAccess *> UseAsOrigin;
      // role from ASSOCIATION AssocAccTarget
      list<ExplicitAssocAccess *> UseAsTarget;

   class ExplicitAssocAccess : public ExtendableME {
   public:
      // role from ASSOCIATION ExplicitAssocAcc
      Class* AssocAccOf;
      // role from ASSOCIATION AssocAccOrign
      Role* OriginRole;
      // role from ASSOCIATION AssocAccTarget
      Role* TargetRole;
   */
   
   /* struct RestrictedRef : public MMObject {
   // for ilic internal purposes only
   public:
      Type *BaseType = nullptr;
      list<Type *> TypeRestriction;
   */

   /* class BaseClass : public MMObject { // ASSOCIATION
   public:
      ClassRelatedType *CRT = nullptr;
      Class *BaseClass = nullptr;
   */

   string name = ctx->rolename->getText();

   debug(ctx,">>> visitRoleDef(" + name + ")");
   Log.incNestLevel();

   if (find_attribute(get_class_context(),name)) {
      Log.error("there is already an attribute with name " + name,get_line(ctx));
   }

   Role *r = new Role;
   init_domaintype(r,ctx->start->getLine());

   r->Name = name;
   r->ElementInPackage = nullptr;

   map<string,bool> properties = get_properties(ctx->properties(),vector<string>({ABSTRACT,EXTENDED,FINAL,HIDING,ORDERED,EXTERNAL}));
   r->Abstract = properties[ABSTRACT];
   r->Final = properties[FINAL];
   r->Ordered = properties[ORDERED];
   r->Extended = properties[EXTENDED];
   r->External = properties[EXTERNAL];
   r->Hiding = properties[HIDING];

   if (r->Extended) {
      Class* c = get_class_context();
      if (c->Super == nullptr) {
         Log.error("EXTENDED can only be used in extended associations",r->_line);
      }
      else {
         Class* s = static_cast<Class*>(c->Super);
         Role *rr = find_role(s,r->Name);
         if (rr == nullptr) {
            Log.error("base of role " + r->Name + " not found in " + get_path(s),r->_line);
         }
         else if (rr->Final) {
            Log.error("base of role " + r->Name + " is FINAL",r->_line);
         }
         r->Super = rr;
      }
   }

   r->Association = get_class_context();

   if (ctx->ASSOCIATE() != nullptr) {
      r->Strongness = Role::Assoc;
   }
   else if (ctx->AGGREGATE() != nullptr) {
      r->Strongness = Role::Aggr;
   }
   else {
      r->Strongness = Role::Comp;
   }
   
   if (ctx->cardinality() != nullptr) {
      r->Multiplicity = visitCardinality(ctx->cardinality());
   }

   for (auto rr : ctx->restrictedRef()) {
      RestrictedRef *rrr = visitRestrictedRef(rr);
      if (rrr == nullptr) {
         continue;
      }
      for (auto t : rrr->_classrestriction) {
         ExplicitAssocAccess *a = new ExplicitAssocAccess();
         a->AssocAccOf = t;
         // Role* OriginRole; to do !!!
         // Role* TargetRole; to do !!!
         r->UseAsTarget.push_back(a);
      }
      r->_baseclass = rrr->_baseclass;
      r->_classrestriction = rrr->_classrestriction;
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitRoleDef(" + name + ")");

   return r;
   
}

antlrcpp::Any Ili2Input::visitCardinality(parser::Ili2Parser::CardinalityContext *ctx)
{

   /* 
   struct Multiplicity : public MMObject {
   public:
      int Min;
      int Max;
   };
   */

   /* cardinality
   : LCURLY (STAR | min=POSNUMBER (DOTDOT (max=POSNUMBER | STAR))?) RCURLY
   */

   debug(ctx,">>> visitCardinality()");
   
   Multiplicity m;
   m.Min = -1;
   m.Max = -1;
   
   if (ctx->min != nullptr) {
      m.Min = stoi(ctx->min->getText());
   }
   
   if (ctx->max != nullptr) {
      m.Max = stoi(ctx->max->getText());
   }
   else {
      m.Max = m.Min;
   }
   
   if (ctx->STAR() != nullptr) {
      m.Max = -1;
   }

   debug(ctx,"<<< visitCardinality(" + to_string(m.Min) + ".." + to_string(m.Max) + ")");

   return m;
   
}
