#pragma once

#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/DiagnosticUtil.h"
#include "../../metamodel/MetaModelBuilder.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;
using namespace util;

namespace {

Type *clone_expression_type(MetaModelBuilder &builder,Factor *factor,int line)
{
   Type *source = nullptr;
   if (auto path = dynamic_cast<PathOrInspFactor *>(factor)) {
      if (!path->PathEls.empty()) {
         if (auto attribute = dynamic_cast<AttrOrParam *>(path->PathEls.back()->Ref)) {
            source = attribute->Type;
         }
         else if (auto role = dynamic_cast<Role *>(path->PathEls.back()->Ref)) {
            ObjectType *object = builder.store().make<ObjectType>();
            builder.initType(object,line);
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
      Type *type = static_cast<Type *>(builder.clone(*source));
      type->Super = source;
      type->ElementInPackage = nullptr;
      return type;
   }

   Type *type = nullptr;
   if (auto constant = dynamic_cast<Constant *>(factor)) {
      if (constant->Kind == Constant::Numeric) type = builder.store().make<NumType>();
      else if (constant->Kind == Constant::Text) type = builder.store().make<TextType>();
      else if (constant->Kind == Constant::Enumeration) type = builder.store().make<EnumType>();
   }
   else if (auto classConstant = dynamic_cast<ClassConst *>(factor)) {
      ClassRefType *reference = builder.store().make<ClassRefType>();
      reference->_baseclass = classConstant->Class;
      type = reference;
   }
   else if (dynamic_cast<AttributeConst *>(factor) != nullptr) {
      type = builder.store().make<AttributeRefType>();
   }
   if (type == nullptr) {
      type = builder.store().make<TextType>();
   }
   builder.initType(type,line);
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

   builder_.debug(ctx,">>> visitViewDef(" + name1 + ")");
   logger_.incNestLevel();

   if (name1 != name2) {
      logger_.error(DiagnosticId::NameEndMismatch,name1 + " expected",
         builder_.line(ctx->viewname2));
   }

   View* v = builder_.store().make<View>();
   v->Name = name1;
   v->Kind = Class::ViewVal;
   builder_.initClass(v,builder_.line(ctx));
   builder_.setSelectionSource(v,ctx->viewname1);
   builder_.setEndSelectionSource(v,ctx->viewname2);
   builder_.addClass(v);

   map<string,bool> properties = get_properties(logger_,ctx->properties(),vector({ABSTRACT,FINAL,TRANSIENT,EXTENDED}));
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
      builder_.setReferenceSource(v,"inheritance",ctx->viewname1);
      SubModel *topic = dynamic_cast<SubModel *>(builder_.currentPackage());
      Package *baseTopic = topic == nullptr ? nullptr : topic->_super;
      if (baseTopic == nullptr) {
         logger_.error(DiagnosticId::InheritanceExtendedTopicRequired,
            "EXTENDED can only be used in an extended topic",diagnostic_range(v));
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
            logger_.error(DiagnosticId::ViewBaseNotFound,
               "base view " + name1 + " not found",diagnostic_range(v));
         }
      }
   }

   if (ctx->EXTENDS() != nullptr) {
      builder_.setReferenceSource(v,"inheritance",ctx->viewref);
      View* vv = builder_.findView(visitPath(ctx->viewref),builder_.line(ctx->viewref));
      v->Super = vv;
   }

   /* formationDef
   : (projection | join | iliunion  | aggregation | inspection) SEMI
   */

   /* enum {Projection, Join, Union,
         Aggregation_All, Aggregation_Equal,
         Inspection_Normal, Inspection_Area} FormationKind; */

   auto *fctx = ctx->formationDef();

   builder_.pushContext(*v);

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
         int line = builder_.line(fctx->inspection());
         for (auto nameToken : fctx->inspection()->NAME()) {
            string attrname = nameToken->getText();
            v->_formationPaths.push_back(attrname);
            line = builder_.line(nameToken);
            inspectedAttribute = builder_.findAttribute(current,attrname);
            if (inspectedAttribute == nullptr) {
               logger_.error(DiagnosticId::ViewInspectionAttributeNotFound,
                  "inspection attribute " + attrname + " not found",line);
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
                  decomposedStructure = builder_.findStructure("INTERLIS.LineGeometry",line);
               }
               else {
                  decomposedStructure = builder_.findStructure("INTERLIS.SurfaceBoundary",line);
               }
            }
            else if (inspectedAttribute->Type->getClass() == "MultiValue") {
               MultiValue *mv = static_cast<MultiValue *>(inspectedAttribute->Type);
               if (mv->BaseType != nullptr && mv->BaseType->getClass() == "Class") {
                  decomposedStructure = static_cast<Class *>(mv->BaseType);
               }
            }
            else {
               logger_.error(DiagnosticId::ViewAttributeInspectionInvalid,
                  "attribute " + inspectedAttribute->Name + " can not be inspected",
                  line);
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
      CompoundExpr *e = builder_.store().make<CompoundExpr>();
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

   builder_.popContext();

   // role from ASSOCIATION BaseViewDef
   // list<RenamedBaseView *> RenamedBaseView;
   // to do !!!

   // role from ASSOCIATION DerivedAssoc
   // list <Class *> DeriAssoc;
   // to do !!!

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitViewDef(" + name1 + ")");

   return v;

}

antlrcpp::Any Ili2Input::visitRenamedViewableRef(parser::Ili2Parser::RenamedViewableRefContext *ctx)
{

   /* renamedViewableRef
   : (basename=NAME TILDE)? path
   */

   builder_.debug(nullptr,">>> visitRenamedViewableRef()");
   logger_.incNestLevel();

   Class *referencedViewable = builder_.findClassOrView(ctx->path()->getText(),builder_.line(ctx));
   string name = "";
   if (ctx->basename != nullptr) {
      name = ctx->basename->getText();
   }
   else if (referencedViewable != nullptr) {
      name = referencedViewable->Name;
   }

   ObjectType *o = builder_.store().make<ObjectType>();
   o->ElementInPackage = nullptr;
   o->Name = "TYPE";
   o->_baseclass = referencedViewable;

   AttrOrParam *a = builder_.store().make<AttrOrParam>();
   antlr4::Token *aliasToken = ctx->basename == nullptr
      ? ctx->path()->getStop() : ctx->basename;
   builder_.initExtendable(a,builder_.line(aliasToken));
   builder_.setSelectionSource(a,aliasToken);
   builder_.setReferenceSource(a,"type",ctx->path());
   builder_.setReferenceSource(a,"dependency",ctx->path());
   a->_visible = false;
   a->AttrParent = builder_.currentClass();
   a->Name = name;
   a->Type = o;
   o->LTParent = a;
   builder_.currentClass()->ClassAttribute.push_back(a);

   logger_.decNestLevel();
   builder_.debug(nullptr,"<<< visitRenamedViewableRef(" + name + ")");

   return a;

}

antlrcpp::Any Ili2Input::visitSelection(parser::Ili2Parser::SelectionContext *ctx)
{

   /* selection
   : WHERE expression SEMI
   */

   builder_.debug(ctx,">>> visitSelection()");
   logger_.incNestLevel();

   Expression *e = visitExpression(ctx->expression());

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitSelection()");

   return e;

}

antlrcpp::Any Ili2Input::visitBaseExtensionDef(parser::Ili2Parser::BaseExtensionDefContext *ctx)
{

   /* baseExtensionDef
   : BASE basename=NAME EXTENDED BY
     renamedViewableRef (COMMA renamedViewableRef)*
   */

   builder_.debug(ctx,">>> visitBaseExtensionDef()");
   logger_.incNestLevel();

   string baseName = ctx->basename->getText();
   Class *baseClass = nullptr;
   AttrOrParam *baseAlias = builder_.findAttribute(builder_.currentClass(),baseName);
   if (baseAlias != nullptr && baseAlias->Type != nullptr && baseAlias->Type->getClass() == "ObjectType") {
      baseClass = static_cast<ObjectType *>(baseAlias->Type)->_baseclass;
   }
   if (baseClass == nullptr) {
      logger_.error(DiagnosticId::ViewBaseNotFound,
         "view base " + baseName + " not found",builder_.line(ctx->basename));
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
         logger_.error(DiagnosticId::ViewBaseExtensionRequired,
            get_path(extension) + " does not extend " + get_path(baseClass),
            builder_.line(refContext),0,
            related_information(baseClass,"Expected view base is declared here"));
      }
   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitBaseExtensionDef()");
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

   builder_.debug(ctx,">>> visitViewAttribute()");
   logger_.incNestLevel();

   if (ctx->ALL() != nullptr) {
      AttrOrParam *baseattr = nullptr;
      string name = ctx->basename->getText();
      ObjectType* basetype = nullptr;;
      for (auto a : builder_.currentClass()->ClassAttribute) {
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
         logger_.error(DiagnosticId::ViewAliasNotFound,
            "alias " + name + " not found",builder_.line(ctx->ALL()));
      }
      else {
         baseattr->_visible = true;
         if (basetype != nullptr) {
            for (auto aa : basetype->_baseclass->ClassAttribute) {
               AttrOrParam *aac = static_cast<AttrOrParam *>(builder_.clone(*aa));
               builder_.setSelectionSource(aac,ctx->basename);
               aac->_visible = true; // to do !!!
               PathEl *pe = builder_.store().make<PathEl>();
               pe->Kind = PathEl::Attribute;
               pe->Ref = aa;
               PathOrInspFactor *pi = builder_.store().make<PathOrInspFactor>();
               pi->PathEls.push_back(pe);
               aac->Derivates.push_back(pi);
               builder_.currentClass()->ClassAttribute.push_back(aac);
            }
         }
      }
   }
   else if (ctx->attributeDef() != nullptr) {
      visitAttributeDef(ctx->attributeDef());
   }
   else {

      string name = ctx->attributename->getText();
   map<string,bool> properties = get_properties(logger_,ctx->properties(),vector({ABSTRACT,FINAL,TRANSIENT,EXTENDED}));
      Factor *f = visitFactor(ctx->factor());

      AttrOrParam *a = builder_.store().make<AttrOrParam>();
      builder_.initExtendable(a,builder_.line(ctx->attributename));
      builder_.setSelectionSource(a,ctx->attributename);
      a->Name = name;
      a->Abstract = properties[ABSTRACT];
      a->Final = properties[FINAL];
      a->Extended = properties[EXTENDED];
      a->Transient = properties[TRANSIENT];
      if (f != nullptr) {
         a->Derivates.push_back(f);
      }

      Type *t = clone_expression_type(builder_,f,builder_.line(ctx));
      if (properties[ABSTRACT]) {
         t->Abstract = true;
      }
      if (properties[FINAL]) {
         t->Final = true;
      }
      a->Type = t;

      a->AttrParent = builder_.currentClass();
      builder_.currentClass()->ClassAttribute.push_back(a);

   }

   logger_.decNestLevel();
   builder_.debug(ctx,"<<< visitViewAttribute()");

   return nullptr;

}
