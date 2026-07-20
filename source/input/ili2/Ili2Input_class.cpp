#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/StringUtil.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

namespace {

antlr4::Token *restricted_ref_token(Ili2Parser::RestrictedRefContext *ctx)
{
   if (ctx == nullptr) return nullptr;
   if (ctx->typeref != nullptr) return ctx->typeref->getStop();
   if (ctx->ANYCLASS() != nullptr) return ctx->ANYCLASS()->getSymbol();
   return ctx->ANYSTRUCTURE() == nullptr ? nullptr : ctx->ANYSTRUCTURE()->getSymbol();
}

antlr4::Token *attr_type_token(Ili2Parser::AttrTypeContext *ctx)
{
   if (ctx == nullptr) return nullptr;
   if (ctx->path() != nullptr) return ctx->path()->getStop();
   if (ctx->referenceAttr() != nullptr)
      return restricted_ref_token(ctx->referenceAttr()->restrictedRef());
   if (ctx->restrictedRef() != nullptr) return restricted_ref_token(ctx->restrictedRef());
   return nullptr;
}

antlr4::Token *attr_type_def_token(Ili2Parser::AttrTypeDefContext *ctx)
{
   if (ctx == nullptr) return nullptr;
   if (ctx->attrType() != nullptr) return attr_type_token(ctx->attrType());
   if (ctx->bagOrListType() == nullptr) return nullptr;
   if (ctx->bagOrListType()->restrictedRef() != nullptr)
      return restricted_ref_token(ctx->bagOrListType()->restrictedRef());
   return attr_type_token(ctx->bagOrListType()->attrType());
}

}

antlrcpp::Any Ili2Input::visitClassDef(Ili2Parser::ClassDefContext *ctx)
{

   /* classDef
   : CLASS classname1=NAME properties? // ABSTRACT|EXTENDED|FINAL
     (EXTENDS classbase=path)? EQUAL
     ((OID AS classoid=path | NO OID) SEMI )?
     classOrStructureDef
     END classname2=NAME SEMI
   */
   
   /*
   class Class : public Type {
      // MetaElement.Name := StructureName, ClassName,
      //                     AssociationName, ViewName
      //                     as defined in the INTERLIS-Model
   public:
      enum { Structure, ClassVal, ViewVal, Association } Kind;
      Multiplicity Multiplicity; // for associations only
      list<Constraint*> Constraints;
      bool EmbeddedRoleTransfer = false;
      bool ili1OptionalTable = false;
      // role from ASSOCIATION ClassAttr
      list<metamodel::AttrOrParam*> ClassAttribute;
      // role from ASSOCIATION AssocRole
      list<Role*> Role;
      // role from ExplicitAssocAcc
      list<ExplicitAssocAccess*> ExplicitAssocAccess;
      // role from ASSOCIATION MetaObjectClass
      list<MetaObjectDef*> MetaObjectDef;
      // role from ASSOCIATION StructOfFormat
      list<FormattedType*> FormattedType;
      // role form ASSOCIATION ObjectOID
      DomainType* Oid = nullptr; // RESTRICTION(TextType; NumType; AnyOIDType)
      // role from ASSOCIATION ARefOf
      list<AttributeRefType*> ForARef;
      // role from ASSOCIATION LineFormStructure
      list<LineForm*> LineForm;
      // role from ASSOCIATION LineAttr
      list<LineType*> LineType;
      // role from ASSOCIATION BaseViewRef
      list<RenamedBaseView*> RenamedBaseView;
      // role from ASSOCIATION DerivedAssoc
      View* View = nullptr;
      // role from ASSOCIATION GraphicBase
      //list<Graphic *> Graphic;
      // role from ASSOCIATION SignClass
      list <DrawingRule*> DrawingRule;
      // from from ASSOCIATION ClassConstraint
      list<Constraint*> Constraint;
   */

   string name1 = ctx->classname1->getText();
   if (util::starts_with(name1,"ILIC_")) {
      name1 = name1.substr(5);
   }

   string name2 = ctx->classname2->getText();
   if (util::starts_with(name2,"ILIC_")) {
      name2 = name2.substr(5);
   }

   if (name1 != name2) {
      Log.error(
         "classname " + name2 + " must match " + name1,
         get_line(ctx->classname2)
      );
   }

   debug(ctx,">>> visitClassDef(" + name1 + ")");
   Log.incNestLevel();

   // init Class
   Class *c = new Class();
   c->Kind = Class::ClassVal;
   init_type(c,get_line(ctx->classname1));
   set_selection_source(c,ctx->classname1);

   // MetaElement Attributes
   c->Name = name1;

   // ExtendableME Attributes
   map<string,bool> properties = get_properties(ctx->properties(),vector<string>({ABSTRACT,FINAL,EXTENDED}));
   c->Abstract = properties[ABSTRACT];
   if (get_package_context()->getClass() == "Model" && !c->Abstract) {
      Log.error("a class in model context has to be defined ABSTRACT",get_line(ctx));
   }
   c->Final = properties[FINAL];
   c->Extended = properties[EXTENDED];

   // EXTENDED
   if (c->Extended) {
      set_reference_source(c,"inheritance",ctx->classname1);
      DataUnit* u = find_dataunit(get_path(get_package_context()),c->_line);
      if (u->Super == nullptr) {
         Log.error(string("EXTENDED can only by used in extended topics"),c->_line);
      }
      else {
         Class *s = find_class_or_structure(name1,c->_line);
         c->Super = s;
         if (s != nullptr) {
            s->Sub.push_back(c);
            if (s->Final) {
               Log.error("class " + name1 + " can not extend FINAL base class " + get_path(s),c->_line);
            }
         }
      }
   }

   // EXTENDS
   if (ctx->classbase != nullptr) {
      set_reference_source(c,"inheritance",ctx->classbase);
      Class *s = find_class_or_structure(ctx->classbase->getText(),get_line(ctx->classbase));
      c->Super = s;
      if (s != nullptr) {
         s->Sub.push_back(c);
         if (s->Final) {
            Log.error("class " + name1 + " can not extend FINAL base class " + get_path(s),c->_line);
         }
      }
   }

   // role from ASSOCIATION LocalType
   // metamodel::AttrOrParam *LTParent;

   if (ctx->classoid != nullptr) {
      c->Oid = find_domaintype(ctx->classoid->getText(),get_line(ctx->classoid));
      // DomainType *Oid; // RESTRICTION(TextType; NumType; AnyOIDType), to do !!!
   }
   else if (ctx->NO()) {
      c->NoOid = true;
   }

   // role from ASSOCIATION DerivedAssoc
   // View *View;

   add_class(c);
   push_context(c);

   /* classOrStructureDef
   : ATTRIBUTE? 
     attributeDef*
     constraintDef*
     (PARAMETER parameterDef*)?
   */

   for (auto *actx : ctx->classOrStructureDef()->attributeDef()) {
      visitAttributeDef(actx);
   }

   for (auto cctx : ctx->classOrStructureDef()->constraintDef()) {
      Constraint *cc = visitConstraintDef(cctx);
      c->Constraints.push_back(cc);
   }

   for (auto pctx : ctx->classOrStructureDef()->parameterDef()) {
      visitParameterDef(pctx);
   }

   check_references(c,"",0);
   pop_context();

   Log.decNestLevel();
   debug(ctx,"<<< visitClassDef(" + name1 + ")");
   
   return c;

}

antlrcpp::Any Ili2Input::visitStructureDef(Ili2Parser::StructureDefContext *ctx)
{

   /* structureDef
   : STRUCTURE structurename1=NAME properties? // ABSTRACT|EXTENDED|FINAL
     (EXTENDS structurebase=path)? EQUAL
     classOrStructureDef
     END structurename2=NAME SEMI
   */

   string name1 = ctx->structurename1->getText();
   string name2 = ctx->structurename2->getText();

   debug(ctx,">>> visitStructureDef(" + name1 + ")");
   Log.incNestLevel();

   if (name1 != name2) {
      Log.error(
         "structurename " + name2 + " must match " + name1,
         ctx->structurename2->getLine()
      );
   }

   // init Class
   Class *c = new Class();
   c->Kind = Class::Structure;
   init_type(c,ctx->structurename1->getLine());
   set_selection_source(c,ctx->structurename1);

   // MetaElement Attributes
   c->Name = name1;

   // ExtendableME Attributes
   map<string,bool> properties = get_properties(ctx->properties(),vector<string>({ABSTRACT,FINAL,EXTENDED}));
   c->Abstract = properties[ABSTRACT];
   c->Final = properties[FINAL];
   c->Extended = properties[EXTENDED];

   // EXTENDED
   if (c->Extended) {
      set_reference_source(c,"inheritance",ctx->structurename1);
      DataUnit* u = find_dataunit(get_path(get_package_context()),c->_line);
      if (u->Super == nullptr) {
         Log.error(string("EXTENDED can only by used in extended topics"),c->_line);
      }
      else {
         Class *s = find_structure(name1,c->_line);
         c->Super = s;
         if (s != nullptr) {
            s->Sub.push_back(c);
            if (s->Final) {
               Log.error("structure " + name1 + " can not extend FINAL base structure " + get_path(s),c->_line);
            }
         }
      }
   }

   // EXTENDS
   if (ctx->structurebase != nullptr) {
      set_reference_source(c,"inheritance",ctx->structurebase);
      Class *s = find_structure(ctx->structurebase->getText(),get_line(ctx->structurebase));
      c->Super = s;
      if (s != nullptr) {
         s->Sub.push_back(c);
         if (s->Final) {
            Log.error("structure " + name1 + " can not extend FINAL base structure " + get_path(s),c->_line);
         }
      }
   }

   // role from ASSOCIATION LocalType
   // metamodel::AttrOrParam *LTParent;

   // role from ASSOCIATION DerivedAssoc
   // View *View;

   add_class(c);
   push_context(c);

   /* classOrStructureDef
   : ATTRIBUTE? 
     attributeDef*
     constraintDef*
     (PARAMETER parameterDef*)?
   */

   for (auto *actx : ctx->classOrStructureDef()->attributeDef()) {
      visitAttributeDef(actx);
   }

   for (auto cctx : ctx->classOrStructureDef()->constraintDef()) {
      Constraint *constraint = visitConstraintDef(cctx);
      c->Constraints.push_back(constraint);
   }

   for (auto pctx : ctx->classOrStructureDef()->parameterDef()) {
      visitParameterDef(pctx);
   }

   check_references(c,"",0);
   pop_context();

   Log.decNestLevel();
   debug(ctx,"<<< visitStructureDef(" + name1 + ")");

   return c;

}

static Type* any_to_type(antlrcpp::Any any) 
{
   
   try {
      return any.as<TextType *>();
   }
   catch (exception e) {
   }
   
   try {
      return any.as<NumType *>();
   }
   catch (exception e) {
   }
   
   try {
      return any.as<BooleanType *>();
   }
   catch (exception e) {
   }

   try {
      return any.as<EnumType *>();
   }
   catch (exception e) {
   }

   try {
      return any.as<CoordType *>();
   }
   catch (exception e) {
   }

   try {
      return any.as<BlackboxType *>();
   }
   catch (exception e) {
   }

   try {
      return any.as<AnyOIDType *>();
   }
   catch (exception e) {
   }

   try {
      return any.as<LineType *>();
   }
   catch (exception e) {
   }

   try {
      DomainType * t = any.as<DomainType *>();
      return nullptr;
   }
   catch (exception e) {
   }

   try {
      Type * t = any.as<Type *>();
      return nullptr;
   }
   catch (exception e) {
   }

   Log.error("any_to_type: unsupported type");
   return nullptr;

}

static void check_type_restriction(Type *base_type,Type *extended_type, string name, int line)
{

   if (!extended_type->isSubClassOf(base_type->getClass())) {
      Log.error("type of attribute " + name + " must have same type as baseclass attr",line);
      return;
   }

   if (base_type->getClass() == "NumType") {
      NumType* b = static_cast<NumType*>(base_type);
      double min_b = atoi(b->Min.c_str());
      double max_b = atoi(b->Max.c_str());
      NumType* e = static_cast<NumType*>(extended_type);
      double min_e = atoi(e->Min.c_str());
      double max_e = atoi(e->Max.c_str());
      if (min_e < min_b) {
         Log.error("numeric range of " + name + " must be a subrange of baseclass type");
      }
      else if (max_e > max_b) {
         Log.error("numeric range of " + name + " must be a subrange of baseclass type");
      }
      return;
   }

   // other type, to do !!!

}
   
antlrcpp::Any Ili2Input::visitAttributeDef(parser::Ili2Parser::AttributeDefContext * ctx)
{

   /* attributeDef
   : (CONTINUOUS? SUBDIVISION)? attributname=NAME
     properties? // ABSTRACT|EXTENDED|FINAL|TRANSIENT
     COLON attrTypeDef
     (COLONEQUAL factor (COMMA factor)*)? SEMI
   */

   /* class AttrOrParam : public ExtendableME {
      // MetaElement.Name := AttributeName, ParameterName
      //                    as defined in the INTERLIS-Model
   public:
      enum { NoSubDiv, SubDiv, ContSubDiv } SubdivisionKind;
      bool Transient = false;
      list<Expression *> Derivates;
      // ROLE from ASSOCIATION LocalType
      list<Type *> LocalType;
      // ROLE from ASSOCIATION AttrOrParamType
      Type *Type;
      // ROLE from ASSOCIATION ClassAttr
      Class *AttrParent;
   */

   string name = ctx->attributname->getText();
      
   debug(ctx,">>> visitAttributeDef(" + name + ")");
   Log.incNestLevel();

   if (find_role(get_class_context(),name) != nullptr) {
      Log.error("there is already a role with name " + name,get_line(ctx));
   }
   AttrOrParam *aa = find_attribute(get_class_context(),name);
   if (aa != nullptr && aa->AttrParent == get_class_context()) {
      Log.error("there is already an attribute with name " + name,get_line(ctx));
   }

   // init AttrOrParam
   AttrOrParam *a = new AttrOrParam();
   init_extendableme(a, ctx->attributname->getLine());
   set_selection_source(a,ctx->attributname);

   // MetaElement attributes
   a->Name = name;
   
   // ExtendableME attributes
   
   map <string,bool> properties;
   properties = get_properties(ctx->properties(),vector<string>({ABSTRACT,FINAL,EXTENDED,TRANSIENT}));
   a->Abstract = properties[ABSTRACT];
   a->Final = properties[FINAL];
   a->Extended = properties[EXTENDED];
   if (a->Extended) {
      set_reference_source(a,"inheritance",ctx->attributname);
      Class *c = get_class_context();
      if (c->Super == nullptr) {
         Log.error(string("EXTENDED can only by used in extended classes / structures / associations"),ctx->attributname->getLine());
      }
      else {
         Class* s = static_cast<Class*>(c->Super);
         aa = find_attribute(s, a->Name);
         if (aa == nullptr) {
            Log.error("base attribute of " + name + " not found in " + get_path(s), a->_line);
         }
         else if (aa->Final) {
            Log.error("base attribute of " + name + " is FINAL", a->_line);
         }
         a->Extending = aa;
         //check_type_restriction(aa->Type, a->Type, name, ctx->attributname->getLine());
      }
   }

   push_context(a);
   a->Type = visitAttrTypeDef(ctx->attrTypeDef());
   set_reference_source(a,"type",attr_type_def_token(ctx->attrTypeDef()));
   a->TypeExplicitlyDefined = ctx->attrTypeDef()->attrType() != nullptr ||
                              ctx->attrTypeDef()->bagOrListType() != nullptr;
   pop_context();

   if (aa != nullptr && aa->AttrParent != get_class_context()) {
      if (properties[EXTENDED]) {
         check_type_extendability(aa->Type,a->Type,get_line(ctx));
      }
      else {
         Log.error("attribute " + a->Name + " must be declared EXTENDED",get_line(ctx));
      }
   }

   // AttrOrParam Attributes
   if (ctx->SUBDIVISION() != nullptr) {
      if (ctx->CONTINUOUS() != nullptr) {
         a->SubdivisionKind = AttrOrParam::ContSubDiv;
      }
      else {
         a->SubdivisionKind = AttrOrParam::SubDiv;
      }
   }
   else {
      a->SubdivisionKind = AttrOrParam::NoSubDiv;
   }

   a->Transient = properties[TRANSIENT];

   // ASSOCIATION ClassAttr
   a->AttrParent = get_class_context();
   get_class_context()->ClassAttribute.push_back(a);

   // RefHB 2.3 3.8: an abstract transient attribute may defer its factor to a
   // concrete extension. This matches ili2c's attributeDef validation.
   if (a->Transient && !a->Abstract && ctx->factor().empty()) {
      Log.error("TRANSIENT attribute " + name + " requires an assignment of a factor",get_line(ctx));
   }
   for (auto fctx : ctx->factor()) {
      Factor *factor = visitFactor(fctx);
      if (factor != nullptr) {
         a->Derivates.push_back(factor);
      }
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitAttributeDef(" + name + ")");

   return a;

}

antlrcpp::Any Ili2Input::visitAttrTypeDef(parser::Ili2Parser::AttrTypeDefContext *ctx)
{
   
   /* attrTypeDef
   : MANDATORY attrType?
   | attrType
   | bagOrListType
   */

   debug(ctx,">>> visitAttrTypeDef()");
   Log.incNestLevel();

   Type *t = nullptr;
   
   if (ctx->attrType() != nullptr) {
      t = visitAttrType(ctx->attrType());
      if (t != nullptr && ctx->MANDATORY() != nullptr) {
         Class* ct = dynamic_cast<Class*>(t);
         if (ct != nullptr) {
            ct->Mandatory = true;
         }
         else {
            try {
               DomainType* dt = static_cast<DomainType*>(t);
               dt->Mandatory = true;
               t = dt;
            }
            catch (exception e) {
               Log.internal_error("unable to cast to DomainType, line=" + to_string(ctx->start->getLine()), e, 1);
            }
         }
      }
   }
   else if (ctx->MANDATORY() != nullptr) {
      AttrOrParam *a = static_cast<AttrOrParam *>(get_context());
      if (a->Extending != nullptr) {
         DomainType* dt = static_cast<DomainType*>((a->Extending->Type)->clone());
         dt->Mandatory = true;
         t = dt;
      }
      else {
         Log.error("MANDATORY restriction only allowed on EXTENDED attributes",get_line(ctx));
      }
   }
   else {
      MultiValue *m = visitBagOrListType(ctx->bagOrListType());
      t = m;
   }
   
   if (t != nullptr) {
      t->Name = "TYPE";
      //t->LTParent = dynamic_cast<AttrOrParam *>(get_context());
      t->_attr = dynamic_cast<AttrOrParam *>(get_context());
   }

   Log.decNestLevel();
   if (t != nullptr) {
      debug(ctx,"<<< visitAttrTypeDef() " + t->Name + ":" + t->getClass());
   }
   else {
      debug(ctx,"<<< visitAttrTypeDef() ???");
   }

   return t;

}

antlrcpp::Any Ili2Input::visitParameterDef(parser::Ili2Parser::ParameterDefContext *ctx)
{
   
   /* parameterDef 
   : parameterName=NAME
     properties? // ABSTRACT,EXTENDED,FINAL
     COLON (attrTypeDef | METAOBJECT (OF path)?) SEMI
   */
   
   string name = ctx->parameterName->getText();
   debug(ctx,">>> visitParameterDef(" + name + ")");
   Log.incNestLevel();

   // init AttrOrParam
   AttrOrParam *a = new AttrOrParam();
   init_extendableme(a, ctx->parameterName->getLine());
   set_selection_source(a,ctx->parameterName);

   // MetaElement attributes
   a->Name = name;
   
   // ExtendableME attributes
   if (ctx->properties() != nullptr) {
      map <string,bool> properties = get_properties(ctx->properties(),vector<string>({ABSTRACT,FINAL,EXTENDED,TRANSIENT}));
      a->Abstract = properties[ABSTRACT];
      a->Final = properties[FINAL];
      a->Extended = properties[EXTENDED];
      if (properties[EXTENDED]) {
         set_reference_source(a,"inheritance",ctx->parameterName);
         Class *c = get_class_context();
         if (c->Super == nullptr) {
            Log.error(string("EXTENDED can only by used in extended classes / structures / associations"),ctx->parameterName->getLine());
         }
         else {
            Class* s = static_cast<Class*>(c->Super);
            AttrOrParam *aa = metamodel::find_parameter(s,name,get_line(ctx));
            if (aa != nullptr) {
               //check_type_restriction(aa->Type, a->Type, name, ctx->attributname->getLine());
               a->Extending = aa;
            }
         }
      }
   }

   push_context(a);
   a->Type = visitAttrTypeDef(ctx->attrTypeDef());
   set_reference_source(a,"type",attr_type_def_token(ctx->attrTypeDef()));
   pop_context();

   // ASSOCIATION ClassParam
   a->ParamParent = get_class_context();
   get_class_context()->ClassParameter.push_back(a);
   
   Log.decNestLevel();
   debug(ctx,"<<< visitParameterDef(" + name + ")");

   return nullptr;

}

antlrcpp::Any Ili2Input::visitAttrType(parser::Ili2Parser::AttrTypeContext * ctx)
{

   /* attrType
   : type
   | path
   | referenceAttr
   | restrictedRef
   */

   /* class Type : public ExtendableME { // ABSTRACT
   public:
      // role from ASSOCIATION LocalType
      metamodel::AttrOrParam *LTParent = nullptr;
      // role from ASSOCIATION AttrOrParamType
      list<AttrOrParam *> AttrOrParam;
      // role form ASSOCIATION BaseType
      list<TypeRelatedType *> TRT;
      // role from ASSOCIATION LocalFType
      FunctionDef *LFTParent = nullptr;
      // role from ASSOCIATION ResultType
      list <FunctionDef *> Function;
      // role form ASSOCIATION ArgumentType
      list<Argument *> Argument;
      Type *_other_type = nullptr; // AttrType by reference, CoordType axis types
      metamodel::AttrOrParam *_attr = nullptr;
   */

   debug(ctx,">>> visitAttrType()");
   Log.incNestLevel();

   Type *t = nullptr;

   if (ctx->path()) {
      Type *tt = find_type(visitPath(ctx->path()),get_line(ctx));
      if (tt != nullptr) {
         if (tt->getClass() == "Class") {
            MultiValue *mv = new MultiValue;
            init_type(mv,get_line(ctx));
            mv->Multiplicity.Min = 0;
            mv->Multiplicity.Max = 1;
            mv->BaseType = tt;
            mv->Super = nullptr;
            t = mv;
         }
         else {
            t = static_cast<Type *>(tt->clone());
            t->Super = tt;
         }
      }
   }
   else if (ctx->type() != nullptr) {
      t = visitType(ctx->type());
      t->Name = "TYPE";
   }
   else if (ctx->referenceAttr() != nullptr) {
      ReferenceType *rt = visitReferenceAttr(ctx->referenceAttr());
      t = rt;
   }
   else if (ctx->restrictedRef() != nullptr) {
      RestrictedRef* r = visitRestrictedRef(ctx->restrictedRef());
      MultiValue* mv = new MultiValue;
      init_type(mv, get_line(ctx));
      mv->Multiplicity.Min = 0;
      mv->Multiplicity.Max = 1;
      mv->BaseType = r->_baseclass;
      for (auto r : r->_classrestriction) {
         mv->TypeRestriction.push_back(r);
      }
      mv->Super = nullptr;
      t = mv;
   }

   if (t != nullptr) {
      try {
         if (get_context()->getClass() == "FunctionDef") {
            t->LFTParent = dynamic_cast<FunctionDef*>(get_context());
         }
         else {
            t->LTParent = dynamic_cast<AttrOrParam*>(get_context());
         }
      }
      catch (exception e) {
         Log.internal_error("LTParent: " + string(e.what()),1);
      }
      t->ElementInPackage = nullptr;
      Log.decNestLevel();
      debug(ctx,"<<< visitAttrType() " + t->Name + ":" + t->getClass());
   }
   else {
      Log.decNestLevel();
      debug(ctx,"<<< visitAttrType() ???");
   }

   return t;

}

antlrcpp::Any Ili2Input::visitReferenceAttr(parser::Ili2Parser::ReferenceAttrContext * ctx)
{

   /* referenceAttr
   : REFERENCE TO (LPAREN EXTERNAL RPAREN)? restrictedRef
   */
   
   /* class ClassRelatedType : public DomainType { // ABSTRACT
   */

   /* class ReferenceType : public ClassRelatedType {
   public:
      bool External = false;
      virtual string getClass() { return "ReferenceType"; }
      virtual string getBaseClass() { return "ClassRelatedType"; };
   */

   debug(ctx,">>> visitReferenceAttr()");
   Log.incNestLevel();
      
   ReferenceType *t = new ReferenceType();
   init_domaintype(t,ctx->start->getLine());
   
   if (ctx->EXTERNAL() != nullptr) {
      t->External = true;
   }
   
   Class *c = static_cast<Class *>(get_class_context());
   if (c->Kind != Class::Structure) {
      if (ili23) {
         Log.error("reference to is only allowed in structures",get_line(ctx));
      }
   }
   
   RestrictedRef *r = visitRestrictedRef(ctx->restrictedRef());
   
   if (r != nullptr && r->_baseclass != nullptr) {
      t->_classrestriction = r->_classrestriction;
      if (r->_baseclass != nullptr) {
         t->_baseclass = r->_baseclass;
         if (t->_baseclass->Kind == Class::Structure) {
            Log.error("target of reference type must be a class or association, found structure",get_line(ctx));
         }
      }
   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitReferenceAttr()");
   return t;

}
