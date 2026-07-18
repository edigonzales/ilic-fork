#include "Ili2Input.h"
#include "Ili2Input_helper.h"
#include "../../metamodel/MetaModelInput.h"
#include "../../util/Logger.h"

using namespace input;
using namespace parser;
using namespace metamodel;

antlrcpp::Any Ili2Input::visitPath(parser::Ili2Parser::PathContext * ctx)
{

   /* path
   : INTERLIS DOT (SIGN | URI | REFSYSTEM | BOOLEAN | HALIGNMENT | VALIGNMENT)
   | (INTERLIS DOT)? NAME (DOT NAME)*
   */
            
   string path = "";
   
   if (ctx->SIGN() != nullptr) {
      path = "INTERLIS.SIGN";
   }
   else if (ctx->URI() != nullptr) {
      path = "INTERLIS.URI";
   }
   else if (ctx->REFSYSTEM() != nullptr) {
      path = "INTERLIS.REFSYSTEM";
   }
   else if (ctx->BOOLEAN() != nullptr) {
      path = "INTERLIS.BOOLEAN";
   }
   else if (ctx->HALIGNMENT() != nullptr) {
      path = "INTERLIS.HALIGNMENT";
   }
   else if (ctx->VALIGNMENT() != nullptr) {
      path = "INTERLIS.VALIGNMENT";
   }
   else {

      if (ctx->INTERLIS() != nullptr) {
         path = "INTERLIS";
      }
   
      for (auto n : ctx->NAME()) {
         string name = n->getSymbol()->getText();
         if (path == "") {
            path = name;
         }
         else {
            path = path + "." + name;
         }
      }
      
   }

   debug(ctx,"visitPath(" + path + ")");

   return path;

}

antlrcpp::Any Ili2Input::visitRestrictedRef(parser::Ili2Parser::RestrictedRefContext * ctx)
{

   /* restrictedRef
   : (typeref=path | ANYCLASS | ANYSTRUCTURE)
     restriction?
   */

   /* class RestricedRef : public MMObject {
   // for ilic internal purposes only
   public:
      Type *BaseType = nullptr;
      list<Type *> TypeRestriction;
   */

   debug(ctx,">>> visitRestrictedRef()");
   Log.incNestLevel();
   
   RestrictedRef *r = new RestrictedRef();
   init_mmobject(r,ctx->start->getLine());
   
   if (ctx->typeref != nullptr) {
      string path = visitPath(ctx->typeref);
      r->_baseclass = find_class_type(path,get_line(ctx->typeref));
   }
   else if (ctx->ANYCLASS() != nullptr) {
      // returns Class / Type
      r->_baseclass = find_class("ANYCLASS",get_line(ctx->ANYCLASS()->getSymbol()));
   }
   else {
      // returns Class / Type
      r->_baseclass = find_class("ANYSTRUCTURE", get_line(ctx->ANYSTRUCTURE()->getSymbol()));
   }
   
   if (ctx->restriction() != nullptr) {
      list<Class *> t = visitRestriction(ctx->restriction());
      r->_classrestriction = t;
   }
   
   for (auto c : r->_classrestriction) {
      if (!c->isSubClassOf(r->_baseclass->getClass())) {
         Log.error(c->getClass() + " is no extension of " + r->_baseclass->getClass(),c->_line);
      }
   }
      
   Log.decNestLevel();
   if (r == nullptr) {
      debug(ctx,"<<< visitRestrictedRef() returns nullptr");
   }
   else if (r->_baseclass == nullptr) {
      debug(ctx,"<<< visitRestrictedRef() BaseClass is nullptr");
   }
   else {
      debug(ctx,"<<< visitRestrictedRef() returns " + r->_baseclass->getClass());
   }
   return r;

}

antlrcpp::Any Ili2Input::visitRestriction(parser::Ili2Parser::RestrictionContext * ctx)
{

   /* restriction
   :  RESTRICTION LPAREN path (COMMA path)* RPAREN)?
   */

   debug(ctx,">>> visitRestriction()");
   Log.incNestLevel();
   
   list<Class *> r;
   for (auto p : ctx->path()) {
      Class *c = find_class_or_structure(visitPath(p),get_line(ctx));
      if (c != nullptr) {
         r.push_back(c);
      }
   }

   Log.decNestLevel();
   debug(ctx,"<<< visitRestriction(" + to_string(r.size()) + ")");

   return r;

}

antlrcpp::Any Ili2Input::visitBaseAttrRef(parser::Ili2Parser::BaseAttrRefContext *ctx)
{
   
   /* baseAttrRef
   : NAME (SLASH POSNUMBER)?
   | NAME SLASH baseattr=path
   */

   debug(ctx,"visitBaseAttrRef()");
   return nullptr;

}

antlrcpp::Any Ili2Input::visitMetaObjectRef(parser::Ili2Parser::MetaObjectRefContext *ctx)
{

   /* metaObjectRef
   : (metaDataBasketRef DOT)? metaobjectname=NAME
   */

   debug(ctx,"visitMetaObjectRef()");
   return nullptr;

}

namespace {

struct PathResolutionState {
   Class *current = nullptr;
   View *enclosingView = nullptr;
   Package *scope = nullptr;
};

Class *path_target(Type *type)
{
   if (type == nullptr) {
      return nullptr;
   }
   if (type->getClass() == "Class") {
      return static_cast<Class *>(type);
   }
   if (type->isSubClassOf("ClassRelatedType")) {
      return static_cast<ClassRelatedType *>(type)->_baseclass;
   }
   if (type->isSubClassOf("TypeRelatedType")) {
      Type *base = static_cast<TypeRelatedType *>(type)->BaseType;
      return base != type ? path_target(base) : nullptr;
   }
   return nullptr;
}

AttrOrParam *view_alias(View *view,string name = "")
{
   if (view == nullptr) {
      return nullptr;
   }
   for (auto attribute : view->ClassAttribute) {
      if (attribute == nullptr || attribute->Type == nullptr ||
          attribute->Type->getClass() != "ObjectType") {
         continue;
      }
      if (name.empty() || attribute->Name == name) {
         return attribute;
      }
   }
   return nullptr;
}

bool is_defined_in_scope(Class *candidate,Package *scope)
{
   if (candidate == nullptr || scope == nullptr) {
      return false;
   }
   if (candidate->ElementInPackage == scope) {
      return true;
   }
   // A topic is also allowed to see contextual extensions defined at model
   // level, matching ili2c's Viewable.isDefinedIn behaviour.
   return scope->getClass() == "SubModel" &&
          candidate->ElementInPackage == scope->ElementInPackage;
}

void find_contextual_attributes(Class *base,Package *scope,const string &name,
                                list<AttrOrParam *> &matches)
{
   if (base == nullptr) {
      return;
   }
   for (ExtendableME *extension : base->Sub) {
      Class *extendedClass = dynamic_cast<Class *>(extension);
      if (extendedClass == nullptr) {
         continue;
      }
      if (is_defined_in_scope(extendedClass,scope)) {
         AttrOrParam *attribute = find_attribute(extendedClass,name);
         if (attribute != nullptr && attribute->AttrParent == extendedClass) {
            matches.push_back(attribute);
         }
      }
      find_contextual_attributes(extendedClass,scope,name,matches);
   }
}

AttrOrParam *find_contextual_attribute(Class *base,Package *scope,const string &name,int line)
{
   AttrOrParam *attribute = find_attribute(base,name);
   if (attribute != nullptr) {
      return attribute;
   }

   list<AttrOrParam *> matches;
   find_contextual_attributes(base,scope,name,matches);
   if (matches.size() > 1) {
      Log.error("ambiguous contextual attribute " + name + " from " + get_path(base),line);
      return nullptr;
   }
   return matches.empty() ? nullptr : matches.front();
}

MetaElement *resolve_through_view_bases(View *view,Package *scope,string name,Class *&target,int line)
{
   MetaElement *found = nullptr;
   target = nullptr;
   for (auto alias : view->ClassAttribute) {
      if (alias == nullptr || alias->Type == nullptr || alias->Type->getClass() != "ObjectType") {
         continue;
      }
      Class *base = path_target(alias->Type);
      if (base == nullptr) {
         continue;
      }

      MetaElement *candidate = find_role(base,name);
      if (candidate == nullptr) {
         candidate = find_contextual_attribute(base,scope,name,line);
      }
      if (candidate == nullptr) {
         continue;
      }
      if (found != nullptr && found != candidate) {
         Log.error("ambiguous path element " + name + " in view " + get_path(view),line);
         return nullptr;
      }
      found = candidate;
      if (candidate->getClass() == "Role") {
         target = static_cast<Role *>(candidate)->_baseclass;
      }
      else {
         target = path_target(static_cast<AttrOrParam *>(candidate)->Type);
      }
   }
   return found;
}

PathEl *resolve_path_element(parser::Ili2Parser::PathElContext *ctx,PathResolutionState &state)
{
   PathEl *element = new PathEl();
   init_mmobject(element,ctx->start->getLine());

   if (ctx->THIS() != nullptr) {
      element->Kind = PathEl::This;
      element->Ref = state.current;
      return element;
   }

   if (ctx->PARENT() != nullptr) {
      element->Kind = PathEl::Parent;
      element->Ref = state.enclosingView;
      if (state.enclosingView == nullptr ||
          state.enclosingView->FormationKind != View::Inspection_Normal ||
          state.enclosingView->_inspectionParent == nullptr) {
         Log.error("PARENT is only valid in a normal inspection view",get_line(ctx));
         state.current = nullptr;
      }
      else {
         state.current = state.enclosingView->_inspectionParent;
      }
      return element;
   }

   if (ctx->THISAREA() != nullptr || ctx->THATAREA() != nullptr) {
      bool thatArea = ctx->THATAREA() != nullptr;
      element->Kind = thatArea ? PathEl::ThatArea : PathEl::ThisArea;
      element->Ref = state.enclosingView;
      if (state.enclosingView == nullptr ||
          state.enclosingView->FormationKind != View::Inspection_Area ||
          state.enclosingView->_inspectionParent == nullptr) {
         Log.error(string(thatArea ? "THATAREA" : "THISAREA") +
                   " is only valid in an area inspection view",get_line(ctx));
         state.current = nullptr;
      }
      else {
         state.current = state.enclosingView->_inspectionParent;
      }
      return element;
   }

   auto object = ctx->objectRef();
   if (object == nullptr) {
      state.current = nullptr;
      return element;
   }

   if (object->FIRST() != nullptr) {
      element->SpecIndex = PathEl::First;
   }
   else if (object->LAST() != nullptr) {
      element->SpecIndex = PathEl::Last;
   }
   else if (object->axislistindex != nullptr) {
      element->NumIndex = atoi(object->axislistindex->getText().c_str());
   }

   if (object->AGGREGATES() != nullptr) {
      element->Kind = PathEl::ViewBase;
      View *view = dynamic_cast<View *>(state.current);
      AttrOrParam *alias = view_alias(view);
      element->Ref = alias;
      state.current = alias == nullptr ? nullptr : path_target(alias->Type);
      if (alias == nullptr) {
         Log.error("AGGREGATES has no aggregation base",get_line(ctx));
      }
      return element;
   }

   if (object->name == nullptr) {
      Log.error("path element has no name",get_line(ctx));
      state.current = nullptr;
      return element;
   }

   string name = object->name->getText();
   if (state.current == nullptr) {
      Log.error("path element " + name + " has no viewable context",get_line(ctx));
      return element;
   }

   View *currentView = dynamic_cast<View *>(state.current);
   AttrOrParam *attribute = find_contextual_attribute(state.current,state.scope,name,get_line(ctx));
   Role *role = find_role(state.current,name);

   if (currentView != nullptr && attribute != nullptr &&
       attribute->Type != nullptr && attribute->Type->getClass() == "ObjectType") {
      element->Kind = PathEl::ViewBase;
      element->Ref = attribute;
      state.current = path_target(attribute->Type);
      return element;
   }

   if (role != nullptr) {
      bool associationPath = object->BACKSLASH() != nullptr;
      element->Kind = associationPath ? PathEl::AssocPath : PathEl::Role;
      element->Ref = role;
      state.current = associationPath ? role->Association : role->_baseclass;
      return element;
   }

   if (attribute != nullptr) {
      element->Kind = attribute->Type != nullptr && attribute->Type->getClass() == "ReferenceType"
         ? PathEl::ReferenceAttr : PathEl::Attribute;
      element->Ref = attribute;
      state.current = path_target(attribute->Type);
      return element;
   }

   if (currentView != nullptr) {
      Class *target = nullptr;
      MetaElement *throughBase = resolve_through_view_bases(currentView,state.scope,name,target,get_line(ctx));
      if (throughBase != nullptr) {
         element->Ref = throughBase;
         element->Kind = throughBase->getClass() == "Role" ? PathEl::Role : PathEl::Attribute;
         state.current = target;
         return element;
      }
   }

   Log.error("path element " + name + " not found in " + get_path(state.current),get_line(ctx));
   state.current = nullptr;
   return element;
}

}

antlrcpp::Any Ili2Input::visitObjectOrAttributePath(parser::Ili2Parser::ObjectOrAttributePathContext *ctx)
{

   /* objectOrAttributePath
   : pathEl (RARROW pathEl)*
   */
  
   /* struct PathEl : public MMObject {
   public:
      enum {
         This, ThisArea, ThatArea, Parent,
         ReferenceAttr, AssocPath, Role, ViewBase,
         Attribute, MetaObject
      } Kind;
      MetaElement* Ref = nullptr;
      int NumIndex;
      enum { First, Last } SpecIndex;

   struct PathOrInspFactor : public Factor {
   public:
      list <PathEl *> PathEls;
      View *Inspection = nullptr;
      string _path = "";
   */

   debug(ctx, ">>> visitObjectOrAttributePath()");
   Log.incNestLevel();
   
   PathOrInspFactor *f = new PathOrInspFactor();
   init_factor(f,ctx->start->getLine());

   PathResolutionState state;
   if (get_context() != nullptr && get_context()->getClass() == "Graphic") {
      Graphic *g = static_cast<Graphic *>(get_context());
      state.current = g->Base;
   }
   else {
      state.current = get_class_context();
   }
   state.enclosingView = dynamic_cast<View *>(state.current);
   state.scope = get_package_context();

   for (auto p : ctx->pathEl()) {
      f->PathEls.push_back(resolve_path_element(p,state));
      if (f->_path != "") {
         f->_path += "->";
      }
      f->_path += p->getText();
   }

   f->_type = "???";
   if (!f->PathEls.empty() && f->PathEls.back()->Ref != nullptr) {
      if (f->PathEls.back()->Ref->getClass() == "AttrOrParam") {
         AttrOrParam *a = static_cast<AttrOrParam*>(f->PathEls.back()->Ref);
         if (a->Type != nullptr) {
            f->_type = a->Type->getClass();
         }
      }
   }

   Log.decNestLevel();
   debug(ctx, "<<< visitObjectOrAttributePath(" + f->_path + ")");
   return f;

}

antlrcpp::Any Ili2Input::visitAttributePath(parser::Ili2Parser::AttributePathContext *ctx)
{
   debug(ctx, ">>> visitAttributePath()");
   PathOrInspFactor *f = visitObjectOrAttributePath(ctx->objectOrAttributePath());
   debug(ctx, "<<< visitAttributePath()");
   return f;
}

antlrcpp::Any Ili2Input::visitPathEl(parser::Ili2Parser::PathElContext *ctx)
{
   
   /* pathEl
   : THIS
   | THISAREA 
   | THATAREA
   | PARENT
   | objectRef 
   */

   /* objectRef 
   : (BACKSLASH)? (name=NAME (LBRACE (FIRST | LAST | axislistindex=POSNUMBER | associationname=NAME) RBRACE)? | AGGREGATES)
   */

   /* struct PathEl : public MMObject {
   public:
      enum {
         This, ThisArea, ThatArea, Parent,
         ReferenceAttr, AssocPath, Role, ViewBase,
         Attribute, MetaObject
      } Kind;
      MetaElement* Ref = nullptr;
      int NumIndex;
      enum { First, Last } SpecIndex;
   */

   PathResolutionState state;
   state.current = get_class_context();
   state.enclosingView = dynamic_cast<View *>(state.current);
   state.scope = get_package_context();
   return resolve_path_element(ctx,state);

}
