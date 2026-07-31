#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/DiagnosticUtil.h"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/StringUtil.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;
using namespace util;

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
      logger_.error(DiagnosticId::NameEndMismatch,
         "classname " + name2 + " must match " + name1,
         builder_.line(ctx->classname2)
      );
   }

   builder_.debug(ctx,">>> visitClassDef(" + name1 + ")");
   logger_.incNestLevel();

   // init Class
   Class *c = builder_.store().make<Class>();
   c->Kind = Class::ClassVal;
   builder_.initType(c,builder_.line(ctx->classname1));
   builder_.setSelectionSource(c,ctx->classname1);
   builder_.setEndSelectionSource(c,ctx->classname2);

   // MetaElement Attributes
   c->Name = name1;

   // ExtendableME Attributes
   map<string,bool> properties = get_properties(logger_,ctx->properties(),vector<string>({ABSTRACT,FINAL,EXTENDED}));
   c->Abstract = properties[ABSTRACT];
   if (builder_.currentPackage()->getClass() == "Model" && !c->Abstract) {
      logger_.error(DiagnosticId::ClassAbstractModelContextRequired,
         "a class in model context has to be defined ABSTRACT",builder_.line(ctx));
   }
   c->Final = properties[FINAL];
   c->Extended = properties[EXTENDED];

   // EXTENDED
   if (c->Extended) {
      builder_.setReferenceSource(c,"inheritance",ctx->classname1);
      DataUnit* u = builder_.findDataUnit(get_path(builder_.currentPackage()),c->_line);
      if (u->Super == nullptr) {
         logger_.error(DiagnosticId::InheritanceExtendedTopicRequired,
            "EXTENDED can only by used in extended topics",diagnostic_range(c));
      }
      else {
         Class *s = builder_.findClassOrStructure(name1,c->_line);
         c->Super = s;
         if (s != nullptr) {
            s->Sub.push_back(c);
            if (s->Final) {
               logger_.error(DiagnosticId::InheritanceFinalBase,
                  "class " + name1 + " can not extend FINAL base class " +
                     get_path(s),diagnostic_range(c));
            }
         }
      }
   }

   // EXTENDS
   if (ctx->classbase != nullptr) {
      builder_.setReferenceSource(c,"inheritance",ctx->classbase);
      Class *s = builder_.findClassOrStructure(ctx->classbase->getText(),builder_.line(ctx->classbase));
      c->Super = s;
      if (s != nullptr) {
         s->Sub.push_back(c);
         if (s->Final) {
            logger_.error(DiagnosticId::InheritanceFinalBase,
               "class " + name1 + " can not extend FINAL base class " +
                  get_path(s),diagnostic_range(c));
         }
      }
   }

   // role from ASSOCIATION LocalType
   // metamodel::AttrOrParam *LTParent;

   if (ctx->classoid != nullptr) {
      c->Oid = builder_.findDomainType(ctx->classoid->getText(),builder_.line(ctx->classoid));
      // DomainType *Oid; // RESTRICTION(TextType; NumType; AnyOIDType), to do !!!
   }
   else if (ctx->NO()) {
      c->NoOid = true;
   }

   // role from ASSOCIATION DerivedAssoc
   // View *View;

   builder_.addClass(c);
   builder_.pushContext(*c);

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

   check_references(builder_,logger_,c,"",0);
   builder_.popContext();

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitClassDef(" + name1 + ")");

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

   builder_.debug(ctx,">>> visitStructureDef(" + name1 + ")");
   logger_.incNestLevel();

   if (name1 != name2) {
      logger_.error(DiagnosticId::NameEndMismatch,
         "structurename " + name2 + " must match " + name1,
         ctx->structurename2->getLine()
      );
   }

   // init Class
   Class *c = builder_.store().make<Class>();
   c->Kind = Class::Structure;
   builder_.initType(c,ctx->structurename1->getLine());
   builder_.setSelectionSource(c,ctx->structurename1);
   builder_.setEndSelectionSource(c,ctx->structurename2);

   // MetaElement Attributes
   c->Name = name1;

   // ExtendableME Attributes
   map<string,bool> properties = get_properties(logger_,ctx->properties(),vector<string>({ABSTRACT,FINAL,EXTENDED}));
   c->Abstract = properties[ABSTRACT];
   c->Final = properties[FINAL];
   c->Extended = properties[EXTENDED];

   // EXTENDED
   if (c->Extended) {
      builder_.setReferenceSource(c,"inheritance",ctx->structurename1);
      DataUnit* u = builder_.findDataUnit(get_path(builder_.currentPackage()),c->_line);
      if (u->Super == nullptr) {
         logger_.error(DiagnosticId::InheritanceExtendedTopicRequired,
            "EXTENDED can only by used in extended topics",diagnostic_range(c));
      }
      else {
         Class *s = builder_.findStructure(name1,c->_line);
         c->Super = s;
         if (s != nullptr) {
            s->Sub.push_back(c);
            if (s->Final) {
               logger_.error(DiagnosticId::InheritanceFinalBase,
                  "structure " + name1 +
                     " can not extend FINAL base structure " + get_path(s),
                  diagnostic_range(c));
            }
         }
      }
   }

   // EXTENDS
   if (ctx->structurebase != nullptr) {
      builder_.setReferenceSource(c,"inheritance",ctx->structurebase);
      Class *s = builder_.findStructure(ctx->structurebase->getText(),builder_.line(ctx->structurebase));
      c->Super = s;
      if (s != nullptr) {
         s->Sub.push_back(c);
         if (s->Final) {
            logger_.error(DiagnosticId::InheritanceFinalBase,
               "structure " + name1 +
                  " can not extend FINAL base structure " + get_path(s),
               diagnostic_range(c));
         }
      }
   }

   // role from ASSOCIATION LocalType
   // metamodel::AttrOrParam *LTParent;

   // role from ASSOCIATION DerivedAssoc
   // View *View;

   builder_.addClass(c);
   builder_.pushContext(*c);

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

   check_references(builder_,logger_,c,"",0);
   builder_.popContext();

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitStructureDef(" + name1 + ")");

   return c;

}

static Type* anyToType(antlrcpp::Any any)
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

   return nullptr;
}

static void check_type_restriction(Type *base_type,Type *extended_type, string name, int line)
{

   if (!extended_type->isSubClassOf(base_type->getClass())) {
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
      }
      else if (max_e > max_b) {
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

   builder_.debug(ctx,">>> visitAttributeDef(" + name + ")");
   logger_.incNestLevel();

   if (builder_.findRole(builder_.currentClass(),name) != nullptr) {
      logger_.error(DiagnosticId::AssociationAttributeNameConflict,
         "there is already a role with name " + name,builder_.line(ctx));
   }
   AttrOrParam *aa = builder_.findAttribute(builder_.currentClass(),name);
   if (aa != nullptr && aa->AttrParent == builder_.currentClass()) {
      logger_.error(DiagnosticId::AttributeDuplicate,
         "there is already an attribute with name " + name,builder_.line(ctx));
   }

   // init AttrOrParam
   AttrOrParam *a = builder_.store().make<AttrOrParam>();
   builder_.initExtendable(a, ctx->attributname->getLine());
   builder_.setSelectionSource(a,ctx->attributname);

   // MetaElement attributes
   a->Name = name;

   // ExtendableME attributes

   map <string,bool> properties;
   properties = get_properties(logger_,ctx->properties(),vector<string>({ABSTRACT,FINAL,EXTENDED,TRANSIENT}));
   a->Abstract = properties[ABSTRACT];
   a->Final = properties[FINAL];
   a->Extended = properties[EXTENDED];
   if (a->Extended) {
      builder_.setReferenceSource(a,"inheritance",ctx->attributname);
      Class *c = builder_.currentClass();
      if (c->Super == nullptr) {
         logger_.error(DiagnosticId::InheritanceExtendedDeclarationRequired,
            "EXTENDED can only be used in extended classes, structures, or associations",
            diagnostic_range(a));
      }
      else {
         Class* s = static_cast<Class*>(c->Super);
         aa = builder_.findAttribute(s, a->Name);
         if (aa == nullptr) {
            logger_.error(DiagnosticId::AttributeBaseNotFound,
               "base attribute of " + name + " not found in " + get_path(s),
               diagnostic_range(a));
         }
         else if (aa->Final) {
            logger_.error(DiagnosticId::AttributeBaseFinal,
               "base attribute of " + name + " is FINAL",diagnostic_range(a),
               related_information(aa,"Final base attribute is declared here"));
         }
         a->Extending = aa;
         //check_type_restriction(aa->Type, a->Type, name, ctx->attributname->getLine());
      }
   }

   builder_.pushContext(*a);
   a->Type = visitAttrTypeDef(ctx->attrTypeDef());
   builder_.setReferenceSource(a,"type",attr_type_def_token(ctx->attrTypeDef()));
   a->TypeExplicitlyDefined = ctx->attrTypeDef()->attrType() != nullptr ||
                              ctx->attrTypeDef()->bagOrListType() != nullptr;
   builder_.popContext();

   if (aa != nullptr && aa->AttrParent != builder_.currentClass()) {
      // Type compatibility belongs to the semantic checker, which can report
      // the named declarations and their exact source ranges.
      if (!properties[EXTENDED]) {
         logger_.error(DiagnosticId::AttributeExtendedRequired,
            "attribute " + a->Name + " must be declared EXTENDED",diagnostic_range(a),
            related_information(aa,"Inherited attribute is declared here"));
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
   a->AttrParent = builder_.currentClass();
   builder_.currentClass()->ClassAttribute.push_back(a);

   // RefHB 2.3 3.8: an abstract transient attribute may defer its factor to a
   // concrete extension. This matches ili2c's attributeDef validation.
   if (a->Transient && !a->Abstract && ctx->factor().empty()) {
      logger_.error(DiagnosticId::TransientAttributeFactorRequired,
         "TRANSIENT attribute " + name + " requires an assignment of a factor",
         diagnostic_range(a));
   }
   for (auto fctx : ctx->factor()) {
      Factor *factor = visitFactor(fctx);
      if (factor != nullptr) {
         a->Derivates.push_back(factor);
      }
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitAttributeDef(" + name + ")");

   return a;

}

antlrcpp::Any Ili2Input::visitAttrTypeDef(parser::Ili2Parser::AttrTypeDefContext *ctx)
{

   /* attrTypeDef
   : MANDATORY attrType?
   | attrType
   | bagOrListType
   */

   builder_.debug(ctx,">>> visitAttrTypeDef()");
   logger_.incNestLevel();

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
               logger_.internal_error("unable to cast to DomainType, line=" + to_string(ctx->start->getLine()), e, 1);
            }
         }
      }
   }
   else if (ctx->MANDATORY() != nullptr) {
      AttrOrParam *a = static_cast<AttrOrParam *>(builder_.current());
      if (a->Extending != nullptr) {
         DomainType* dt = static_cast<DomainType*>(builder_.clone(*a->Extending->Type));
         dt->Mandatory = true;
         t = dt;
      }
      else {
         logger_.error(DiagnosticId::AttributeMandatoryExtensionRequired,
            "MANDATORY restriction only allowed on EXTENDED attributes",builder_.line(ctx));
      }
   }
   else {
      MultiValue *m = visitBagOrListType(ctx->bagOrListType());
      t = m;
   }

   if (t != nullptr) {
      t->Name = "TYPE";
      //t->LTParent = dynamic_cast<AttrOrParam *>(builder_.current());
      t->_attr = dynamic_cast<AttrOrParam *>(builder_.current());
   }

   logger_.decNestLevel();
   if (t != nullptr) {
      builder_.debug(ctx,"<<< visitAttrTypeDef() " + t->Name + ":" + t->getClass());
   }
   else {
      builder_.debug(ctx,"<<< visitAttrTypeDef() ???");
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
   builder_.debug(ctx,">>> visitParameterDef(" + name + ")");
   logger_.incNestLevel();

   // init AttrOrParam
   AttrOrParam *a = builder_.store().make<AttrOrParam>();
   builder_.initExtendable(a, ctx->parameterName->getLine());
   builder_.setSelectionSource(a,ctx->parameterName);

   // MetaElement attributes
   a->Name = name;

   // ExtendableME attributes
   if (ctx->properties() != nullptr) {
      map <string,bool> properties = get_properties(logger_,ctx->properties(),vector<string>({ABSTRACT,FINAL,EXTENDED,TRANSIENT}));
      a->Abstract = properties[ABSTRACT];
      a->Final = properties[FINAL];
      a->Extended = properties[EXTENDED];
      if (properties[EXTENDED]) {
         builder_.setReferenceSource(a,"inheritance",ctx->parameterName);
         Class *c = builder_.currentClass();
         if (c->Super == nullptr) {
            logger_.error(DiagnosticId::InheritanceExtendedDeclarationRequired,
               "EXTENDED can only be used in extended classes, structures, or associations",
               diagnostic_range(a));
         }
         else {
            Class* s = static_cast<Class*>(c->Super);
            AttrOrParam *aa = builder_.findParameter(s,name,builder_.line(ctx));
            if (aa != nullptr) {
               //check_type_restriction(aa->Type, a->Type, name, ctx->attributname->getLine());
               a->Extending = aa;
            }
         }
      }
   }

   builder_.pushContext(*a);
   a->Type = visitAttrTypeDef(ctx->attrTypeDef());
   builder_.setReferenceSource(a,"type",attr_type_def_token(ctx->attrTypeDef()));
   builder_.popContext();

   // ASSOCIATION ClassParam
   a->ParamParent = builder_.currentClass();
   builder_.currentClass()->ClassParameter.push_back(a);

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitParameterDef(" + name + ")");

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

   builder_.debug(ctx,">>> visitAttrType()");
   logger_.incNestLevel();

   Type *t = nullptr;

   if (ctx->path()) {
      Type *tt = builder_.findType(visitPath(ctx->path()),builder_.line(ctx));
      if (tt != nullptr) {
         if (tt->getClass() == "Class") {
            MultiValue *mv = builder_.store().make<MultiValue>();
            builder_.initType(mv,builder_.line(ctx));
            mv->Multiplicity.Min = 0;
            mv->Multiplicity.Max = 1;
            mv->BaseType = tt;
            mv->Super = nullptr;
            t = mv;
         }
         else {
            t = static_cast<Type *>(builder_.clone(*tt));
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
      MultiValue* mv = builder_.store().make<MultiValue>();
      builder_.initType(mv, builder_.line(ctx));
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
         if (builder_.current()->getClass() == "FunctionDef") {
            t->LFTParent = dynamic_cast<FunctionDef*>(builder_.current());
         }
         else {
            t->LTParent = dynamic_cast<AttrOrParam*>(builder_.current());
         }
      }
      catch (exception e) {
         logger_.internal_error("LTParent: " + string(e.what()),1);
      }
      t->ElementInPackage = nullptr;
      logger_.decNestLevel();
      builder_.debug(ctx,"<<< visitAttrType() " + t->Name + ":" + t->getClass());
   }
   else {
      logger_.decNestLevel();
      builder_.debug(ctx,"<<< visitAttrType() ???");
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

   builder_.debug(ctx,">>> visitReferenceAttr()");
   logger_.incNestLevel();

   ReferenceType *t = builder_.store().make<ReferenceType>();
   builder_.initDomainType(t,ctx->start->getLine());

   if (ctx->EXTERNAL() != nullptr) {
      t->External = true;
   }

   Class *c = static_cast<Class *>(builder_.currentClass());
   if (c->Kind != Class::Structure) {
      if (builder_.isIli23()) {
         logger_.error(DiagnosticId::ReferenceStructureContextRequired,
            "reference to is only allowed in structures",builder_.line(ctx));
      }
   }

   RestrictedRef *r = visitRestrictedRef(ctx->restrictedRef());

   if (r != nullptr && r->_baseclass != nullptr) {
      t->_classrestriction = r->_classrestriction;
      if (r->_baseclass != nullptr) {
         t->_baseclass = r->_baseclass;
         if (t->_baseclass->Kind == Class::Structure) {
            logger_.error(DiagnosticId::ReferenceClassOrAssociationRequired,
               "target of reference type must be a class or association, found structure",
               builder_.line(ctx));
         }
      }
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitReferenceAttr()");
   return t;

}
