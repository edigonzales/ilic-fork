#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

namespace {

Type *clone_expression_type(Factor *factor,int line)
{
   Type *source = nullptr;
   if (auto path = dynamic_cast<PathOrInspFactor *>(factor)) {
      if (!path->PathEls.empty()) {
         if (auto attribute = dynamic_cast<AttrOrParam *>(path->PathEls.back()->Ref)) {
            source = attribute->Type;
         }
         else if (auto role = dynamic_cast<Role *>(path->PathEls.back()->Ref)) {
            ObjectType *object = new ObjectType();
            init_type(object,line);
            object->_baseclass = role->_baseclass;
            return object;
         }
      }
   }
   else if (auto function = dynamic_cast<FunctionCall *>(factor)) {
      source = function->Function == nullptr ? nullptr : function->Function->ResultType;
   }
   else if (auto parameter = dynamic_cast<RuntimeParamRef *>(factor)) {
      source = parameter->RuntimeParam == nullptr ? nullptr : parameter->RuntimeParam->Type;
   }
   if (source != nullptr) {
      Type *type = static_cast<Type *>(source->clone());
      type->Super = source;
      type->ElementInPackage = nullptr;
      return type;
   }

   Type *type = nullptr;
   if (auto constant = dynamic_cast<Constant *>(factor)) {
      if (constant->Kind == Constant::Numeric) type = new NumType();
      else if (constant->Kind == Constant::Text) type = new TextType();
      else if (constant->Kind == Constant::Enumeration) type = new EnumType();
   }
   else if (auto classConstant = dynamic_cast<ClassConst *>(factor)) {
      ClassRefType *reference = new ClassRefType();
      reference->_baseclass = classConstant->Class;
      type = reference;
   }
   else if (dynamic_cast<AttributeConst *>(factor) != nullptr) {
      type = new AttributeRefType();
   }
   if (type == nullptr) {
      type = new TextType();
   }
   init_type(type,line);
   return type;
}

}

antlrcpp::Any Ili2Input::visitViewDef(parser::Ili2Parser::ViewDefContext *ctx)
{

   /* viewDef
   : VIEW viewname1=NAME
     properties? // ABSTRACT|EXTENDED|FINAL|TRANSIENT
     (formationDef | EXTENDS viewref=path)
     (baseExtensionDef)*
     (selection)*
     EQUAL
     ATTRIBUTES?
     viewAttribute*
     constraintDef*
     END viewname2=NAME SEMI
   */

   /* formationDef
   : (projection | join | iliunion  | aggregation | inspection) SEMI
   */

   /* class View : public Class {
   public:
      enum {Projection, Join, Union,
            Aggregation_All, Aggregation_Equal,
            Inspection_Normal, Inspection_Area} FormationKind;

      list<Expression *> FormationParameter; // PathOrInspFactor only
                          // Aggr.Equal: UniqueEl
                          // Inspection: Attributepath
      Expression *Where;
      bool Transient = false;
      // role from ASSOCIATION BaseViewDef
      list<RenamedBaseView *> RenamedBaseView;
      // role from ASSOCIATION DerivedAssoc
      list <Class *> DeriAssoc;
   */

   string name1 = ctx->viewname1->getText();
   string name2 = ctx->viewname2->getText();

   debug(ctx,">>> visitViewDef(" + name1 + ")");
   Log.incNestLevel();
   
   if (name1 != name2) {
      Log.error(name1 + " expected",get_line(ctx->viewname2));
   }

   View* v = new View;
   v->Name = name1;
   v->Kind = Class::ViewVal;
   init_class(v,get_line(ctx));
   set_selection_source(v,ctx->viewname1);
   set_end_selection_source(v,ctx->viewname2);
   add_class(v); 

   map<string,bool> properties = get_properties(ctx->properties(),vector({ABSTRACT,FINAL,TRANSIENT,EXTENDED}));
   if (properties[ABSTRACT]) {
      v->Abstract = true;
   }
   if (properties[FINAL]) {
      v->Final = true;
   }
   if (properties[TRANSIENT]) {
      v->Transient = true;
   }
   
   if (properties[EXTENDED]) {
      v->Extended = true;
      set_reference_source(v,"inheritance",ctx->viewname1);
      SubModel *topic = dynamic_cast<SubModel *>(get_package_context());
      Package *baseTopic = topic == nullptr ? nullptr : topic->_super;
      if (baseTopic == nullptr) {
         Log.error("EXTENDED can only be used in an extended topic",get_line(ctx));
      }
      else {
         for (MetaElement *element : baseTopic->Element) {
            if (element->Name == name1 && element->getClass() == "View") {
               v->Super = static_cast<View *>(element);
               v->Super->Sub.push_back(v);
               break;
            }
         }
         if (v->Super == nullptr) {
            Log.error("base view " + name1 + " not found",get_line(ctx));
         }
      }
   }

   if (ctx->EXTENDS() != nullptr) {
      set_reference_source(v,"inheritance",ctx->viewref);
      View* vv = find_view(visitPath(ctx->viewref),get_line(ctx->viewref));
      v->Super = vv;
   }

   /* formationDef
   : (projection | join | iliunion  | aggregation | inspection) SEMI
   */
   
   /* enum {Projection, Join, Union,
         Aggregation_All, Aggregation_Equal,
         Inspection_Normal, Inspection_Area} FormationKind; */

   auto *fctx = ctx->formationDef();
   
   push_context(v);

   if (fctx != nullptr) {

      /* renamedViewableRef
      : (basename=NAME TILDE)? path
      */
      
      if (fctx->projection() != nullptr) {
         /* PROJECTION OF renamedViewableRef
         */
         v->FormationKind = View::Projection;
         v->_formationPaths.push_back(fctx->projection()->renamedViewableRef()->path()->getText());
         visitRenamedViewableRef(fctx->projection()->renamedViewableRef());
      }
      else if (fctx->join() != nullptr) {
         /* JOIN OF renamedViewableRef
            (COMMA renamedViewableRef (LPAREN OR ILINULL RPAREN)?)+
         */
         v->FormationKind = View::Join;
         for (auto vr: fctx->join()->renamedViewableRef()) {
            v->_formationPaths.push_back(vr->path()->getText());
            visitRenamedViewableRef(vr);
         }
         v->_orNullCount = static_cast<int>(fctx->join()->ILINULL().size());
      }
      else if (fctx->iliunion() != nullptr) {
         /* UNION OF renamedViewableRef
            (COMMA renamedViewableRef)*
         */
         v->FormationKind = View::Union;
         for (auto vr: fctx->iliunion()->renamedViewableRef()) {
            v->_formationPaths.push_back(vr->path()->getText());
            visitRenamedViewableRef(vr);
         }
      }
      else if (fctx->aggregation() != nullptr) {
         /* AGGREGATION OF renamedViewableRef
            (ALL | EQUAL LPAREN uniqueEl RPAREN)
         */
         v->FormationKind = fctx->aggregation()->ALL() != nullptr
            ? View::Aggregation_All : View::Aggregation_Equal;
         v->_formationPaths.push_back(fctx->aggregation()->renamedViewableRef()->path()->getText());
         if (fctx->aggregation()->uniqueEl() != nullptr) {
            for (auto path : fctx->aggregation()->uniqueEl()->objectOrAttributePath()) {
               v->_formationPaths.push_back(path->getText());
            }
         }
         visitRenamedViewableRef(fctx->aggregation()->renamedViewableRef());
      }
      else if (fctx->inspection() != nullptr) {
         /* inspection
         : AREA? INSPECTION OF renamedViewableRef
         (RARROW structureorlineattributename=NAME)+
         */
         if (fctx->inspection()->AREA() != nullptr) {
            v->FormationKind = View::Inspection_Area;
         }
         else {
            v->FormationKind = View::Inspection_Normal;
         }
         AttrOrParam *baseAlias = visitRenamedViewableRef(fctx->inspection()->renamedViewableRef());
         v->_formationPaths.push_back(fctx->inspection()->renamedViewableRef()->path()->getText());
         ObjectType *baseAliasType = baseAlias == nullptr
            ? nullptr : dynamic_cast<ObjectType *>(baseAlias->Type);
         Class *inspectionRoot = baseAliasType == nullptr ? nullptr : baseAliasType->_baseclass;
         v->_inspectionParent = inspectionRoot;

         Class *current = inspectionRoot;
         AttrOrParam *inspectedAttribute = nullptr;
         int line = get_line(fctx->inspection());
         for (auto nameToken : fctx->inspection()->NAME()) {
            string attrname = nameToken->getText();
            v->_formationPaths.push_back(attrname);
            line = get_line(nameToken);
            inspectedAttribute = find_attribute(current,attrname);
            if (inspectedAttribute == nullptr) {
               Log.error("inspection attribute " + attrname + " not found",line);
               current = nullptr;
               break;
            }
            if (inspectedAttribute->Type != nullptr && inspectedAttribute->Type->getClass() == "MultiValue") {
               MultiValue *mv = static_cast<MultiValue *>(inspectedAttribute->Type);
               current = mv->BaseType != nullptr && mv->BaseType->getClass() == "Class"
                  ? static_cast<Class *>(mv->BaseType) : nullptr;
            }
         }

         Class *decomposedStructure = nullptr;
         if (inspectedAttribute != nullptr && inspectedAttribute->Type != nullptr) {
            if (inspectedAttribute->Type->getClass() == "LineType") {
               LineType *lineType = static_cast<LineType *>(inspectedAttribute->Type);
               if (lineType->Kind == LineType::Polyline || lineType->Kind == LineType::DirectedPolyline) {
                  decomposedStructure = find_structure("INTERLIS.LineGeometry",line);
               }
               else {
                  decomposedStructure = find_structure("INTERLIS.SurfaceBoundary",line);
               }
            }
            else if (inspectedAttribute->Type->getClass() == "MultiValue") {
               MultiValue *mv = static_cast<MultiValue *>(inspectedAttribute->Type);
               if (mv->BaseType != nullptr && mv->BaseType->getClass() == "Class") {
                  decomposedStructure = static_cast<Class *>(mv->BaseType);
               }
            }
            else {
               Log.error("attribute " + inspectedAttribute->Name + " can not be inspected",line);
            }
         }
         if (baseAliasType != nullptr && decomposedStructure != nullptr) {
            baseAliasType->_baseclass = decomposedStructure;
         }
      }
   }

   for (auto baseExtension : ctx->baseExtensionDef()) {
      visitBaseExtensionDef(baseExtension);
   }
   
   if (ctx->selection().size() == 1) {
      v->Where = visitSelection(ctx->selection().front());
   }
   else if (ctx->selection().size() > 1) {
      CompoundExpr *e = new CompoundExpr();
      e->Operation = CompoundExpr_OperationType::And;
      for (auto sctx : ctx->selection()) {
         e->SubExpressions.push_back(visitSelection(sctx));
      }
      v->Where = e;
   }

   // bool Transient = false;
   
   for (auto actx : ctx->viewAttribute()) {
      visitViewAttribute(actx);
   }
   
   for (auto cctx : ctx->constraintDef()) {
      v->Constraints.push_back(visitConstraintDef(cctx));
   }
   
   pop_context();

   // role from ASSOCIATION BaseViewDef
   // list<RenamedBaseView *> RenamedBaseView;
   // to do !!!

   // role from ASSOCIATION DerivedAssoc
   // list <Class *> DeriAssoc;
   // to do !!!

   Log.decNestLevel();
   debug(ctx,"<<< visitViewDef(" + name1 + ")");
   
   return v;
   
}

antlrcpp::Any Ili2Input::visitRenamedViewableRef(parser::Ili2Parser::RenamedViewableRefContext *ctx)
{

   /* renamedViewableRef
   : (basename=NAME TILDE)? path
   */

   Log.debug(">>> visitRenamedViewableRef()");
   Log.incNestLevel();

   Class *referencedViewable = find_class_or_view(ctx->path()->getText(),get_line(ctx));
   string name = "";
   if (ctx->basename != nullptr) {
      name = ctx->basename->getText();
   }
   else if (referencedViewable != nullptr) {
      name = referencedViewable->Name;
   }

   ObjectType *o = new ObjectType;
   o->ElementInPackage = nullptr;
   o->Name = "TYPE";
   o->_baseclass = referencedViewable;

   AttrOrParam *a = new AttrOrParam;
   antlr4::Token *aliasToken = ctx->basename == nullptr
      ? ctx->path()->getStop() : ctx->basename;
   init_extendableme(a,get_line(aliasToken));
   set_selection_source(a,aliasToken);
   set_reference_source(a,"type",ctx->path());
   a->_visible = false;
   a->AttrParent = get_class_context();
   a->Name = name;
   a->Type = o;
   o->LTParent = a;
   get_class_context()->ClassAttribute.push_back(a);

   Log.decNestLevel();
   Log.debug("<<< visitRenamedViewableRef(" + name + ")");
   
   return a;

}

antlrcpp::Any Ili2Input::visitSelection(parser::Ili2Parser::SelectionContext *ctx)
{

   /* selection
   : WHERE expression SEMI
   */

   debug(ctx,">>> visitSelection()");
   Log.incNestLevel();

   Expression *e = visitExpression(ctx->expression());

   Log.decNestLevel();
   debug(ctx,"<<< visitSelection()");

   return e;
   
}

antlrcpp::Any Ili2Input::visitBaseExtensionDef(parser::Ili2Parser::BaseExtensionDefContext *ctx)
{

   /* baseExtensionDef
   : BASE basename=NAME EXTENDED BY
     renamedViewableRef (COMMA renamedViewableRef)*
   */

   debug(ctx,">>> visitBaseExtensionDef()");
   Log.incNestLevel();

   string baseName = ctx->basename->getText();
   Class *baseClass = nullptr;
   AttrOrParam *baseAlias = find_attribute(get_class_context(),baseName);
   if (baseAlias != nullptr && baseAlias->Type != nullptr && baseAlias->Type->getClass() == "ObjectType") {
      baseClass = static_cast<ObjectType *>(baseAlias->Type)->_baseclass;
   }
   if (baseClass == nullptr) {
      Log.error("view base " + baseName + " not found",get_line(ctx->basename));
   }

   for (auto refContext : ctx->renamedViewableRef()) {
      AttrOrParam *extensionAlias = visitRenamedViewableRef(refContext);
      ObjectType *extensionType = extensionAlias == nullptr
         ? nullptr : dynamic_cast<ObjectType *>(extensionAlias->Type);
      Class *extension = extensionType == nullptr ? nullptr : extensionType->_baseclass;
      bool extendsBase = extension != nullptr && baseClass != nullptr;
      Class *candidate = extension;
      while (extendsBase && candidate != baseClass) {
         if (candidate->Super == nullptr || candidate->Super->getClass() != "Class") {
            extendsBase = false;
            break;
         }
         candidate = static_cast<Class *>(candidate->Super);
      }
      if (extension != nullptr && baseClass != nullptr && !extendsBase) {
         Log.error(get_path(extension) + " does not extend " + get_path(baseClass),get_line(refContext));
      }
   }

   Log.decNestLevel();
   debug(ctx,"<<< visitBaseExtensionDef()");
   return nullptr;
   
}

antlrcpp::Any Ili2Input::visitViewAttribute(parser::Ili2Parser::ViewAttributeContext *ctx)
{

   /* viewAttribute
   : ALL OF basename=NAME SEMI
   | attributeDef
   | attributename=NAME properties? // ABSTRACT|EXTENDED|FINAL|TRANSIENT
        COLONEQUAL factor SEMI
   */

   debug(ctx,">>> visitViewAttribute()");
   Log.incNestLevel();

   if (ctx->ALL() != nullptr) {
      AttrOrParam *baseattr = nullptr;
      string name = ctx->basename->getText();
      ObjectType* basetype = nullptr;;
      for (auto a : get_class_context()->ClassAttribute) {
         if (a->Type == nullptr) {
            continue;
         }
         if (a->Type->getClass() != "ObjectType") {
            continue;
         }
         if (a->Name == name) {
            baseattr = a;
            basetype = static_cast<ObjectType *>(a->Type);
            break;
         }
      }
      if (baseattr == nullptr) {
         Log.error("alias " + name + " not found",get_line(ctx->ALL()));
      }
      else {
         baseattr->_visible = true;
         if (basetype != nullptr) {
            for (auto aa : basetype->_baseclass->ClassAttribute) {
               AttrOrParam *aac = static_cast<AttrOrParam *>(aa->clone());
               aac->_visible = true; // to do !!!
               PathEl *pe = new PathEl;
               pe->Kind = PathEl::Attribute;
               pe->Ref = aa;
               PathOrInspFactor *pi = new PathOrInspFactor;
               pi->PathEls.push_back(pe);
               aac->Derivates.push_back(pi);
               get_class_context()->ClassAttribute.push_back(aac);
            }
         }
      }
   }
   else if (ctx->attributeDef() != nullptr) {
      visitAttributeDef(ctx->attributeDef());
   }
   else {

      string name = ctx->attributename->getText();
      map<string,bool> properties = get_properties(ctx->properties(),vector({ABSTRACT,FINAL,TRANSIENT,EXTENDED}));
      Factor *f = visitFactor(ctx->factor());
      
      AttrOrParam *a = new AttrOrParam;
      init_extendableme(a,get_line(ctx->attributename));
      set_selection_source(a,ctx->attributename);
      a->Name = name;
      a->Abstract = properties[ABSTRACT];
      a->Final = properties[FINAL];
      a->Extended = properties[EXTENDED];
      a->Transient = properties[TRANSIENT];
      if (f != nullptr) {
         a->Derivates.push_back(f);
      }
      
      Type *t = clone_expression_type(f,get_line(ctx));
      if (properties[ABSTRACT]) {
         t->Abstract = true;
      }
      if (properties[FINAL]) {
         t->Final = true;
      }
      a->Type = t;

      a->AttrParent = get_class_context();
      get_class_context()->ClassAttribute.push_back(a);

   }
   
   Log.decNestLevel();
   debug(ctx,"<<< visitViewAttribute()");

   return nullptr;

}
