#include "TranslationChecker.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "MetaModel.h"
#include "../util/Logger.h"

using namespace std;
using namespace util;

namespace metamodel {

namespace {

template<typename T> vector<T *> as_vector(const list<T *> &values)
{
   return vector<T *>(values.begin(),values.end());
}

class Checker {
public:
   void run()
   {
      validate_model_graph();
      for (Model *model : get_all_models()) {
         if (!model->_translationOfName.empty() && model->_translationOf == nullptr) {
            Model *base = find_model(model->_translationOfName);
            if (base != nullptr && base != model && !in_cycle(model)) {
               link_element(model,base);
            }
         }
      }
      for (Model *model : get_all_models()) {
         if (model->_translationOf != nullptr) {
            compare_element(model,model->_translationOf);
         }
      }
   }

private:
   set<Model *> cyclic_models;
   map<string,string> translated_names;

   Model *find_model(const string &name)
   {
      for (Model *model : get_all_models()) {
         if (model->Name == name) {
            return model;
         }
      }
      return nullptr;
   }

   bool in_cycle(Model *model) const
   {
      return cyclic_models.find(model) != cyclic_models.end();
   }

   void validate_model_graph()
   {
      for (Model *model : get_all_models()) {
         if (model->_translationOfName.empty()) {
            continue;
         }
         Model *base = find_model(model->_translationOfName);
         if (base == nullptr) {
            Log.error("translation base model " + model->_translationOfName + " not found",model->_line);
         }
         else if (base == model) {
            Log.error("model " + model->Name + " cannot be a translation of itself",model->_line);
            cyclic_models.insert(model);
         }
      }

      for (Model *start : get_all_models()) {
         vector<Model *> chain;
         Model *current = start;
         while (current != nullptr && !current->_translationOfName.empty()) {
            auto found = find(chain.begin(),chain.end(),current);
            if (found != chain.end()) {
               for (auto it = found; it != chain.end(); ++it) {
                  cyclic_models.insert(*it);
               }
               Log.error("cycle in TRANSLATION OF chain at model " + current->Name,current->_line);
               break;
            }
            chain.push_back(current);
            current = find_model(current->_translationOfName);
         }
      }
   }

   string label(MetaElement *element) const
   {
      return element == nullptr ? "<missing>" : get_path(element);
   }

   void mismatch(MetaElement *translated,MetaElement *base,const string &property)
   {
      Log.error("translation mismatch for " + label(translated) + " against "
         + label(base) + ": " + property,translated == nullptr ? -1 : translated->_line);
   }

   string canonical_text(const string &text) const
   {
      string result;
      bool quoted = false;
      for (size_t i = 0; i < text.size();) {
         if (text[i] == '"') {
            quoted = !quoted;
            result += text[i++];
         }
         else if (!quoted && (isalpha(static_cast<unsigned char>(text[i])) || text[i] == '_')) {
            size_t end = i + 1;
            while (end < text.size() && (isalnum(static_cast<unsigned char>(text[end])) || text[end] == '_')) ++end;
            string token = text.substr(i,end - i);
            auto found = translated_names.find(token);
            result += found != translated_names.end() && !found->second.empty() ? found->second : token;
            i = end;
         }
         else {
            result += text[i++];
         }
      }
      return result;
   }

   template<typename T,typename U>
   void value(MetaElement *translated,MetaElement *base,const string &property,const T &actual,const U &expected)
   {
      if (actual != expected) {
         mismatch(translated,base,property);
      }
   }

   bool same_ref(MetaElement *actual,MetaElement *expected) const
   {
      if (actual == nullptr || expected == nullptr) {
         return actual == expected;
      }
      return actual->getTranslationOfRoot() == expected->getTranslationOfRoot();
   }

   void ref(MetaElement *translated,MetaElement *base,const string &property,
      MetaElement *actual,MetaElement *expected)
   {
      if (!same_ref(actual,expected)) {
         mismatch(translated,base,property);
      }
   }

   template<typename T>
   void link_list(const list<T *> &translated,const list<T *> &base,MetaElement *owner,const string &property)
   {
      vector<T *> translated_values = as_vector(translated);
      vector<T *> base_values = as_vector(base);
      if (translated_values.size() != base_values.size()) {
         mismatch(owner,owner->_translationOf,property + " count");
      }
      size_t count = min(translated_values.size(),base_values.size());
      for (size_t i = 0; i < count; ++i) {
         link_element(translated_values[i],base_values[i]);
      }
   }

   void link_type(Type *translated,Type *base)
   {
      if (translated == nullptr || base == nullptr) {
         return;
      }
      link_element(translated,base);
   }

   void link_element(MetaElement *translated,MetaElement *base)
   {
      if (translated == nullptr || base == nullptr) {
         return;
      }
      if (translated->_translationOf != nullptr) {
         return;
      }
      if (translated->getClass() != base->getClass()) {
         mismatch(translated,base,"element kind");
         return;
      }
      translated->_translationOf = base;
      if (!translated->Name.empty() && translated->Name != base->Name) {
         auto existing = translated_names.find(translated->Name);
         if (existing == translated_names.end()) translated_names[translated->Name] = base->Name;
         else if (existing->second != base->Name) existing->second.clear();
      }

      if (auto package = dynamic_cast<Package *>(translated)) {
         link_list(package->Element,static_cast<Package *>(base)->Element,translated,"elements");
      }
      if (auto model = dynamic_cast<Model *>(translated)) {
         link_list(model->_runtimeparameter,static_cast<Model *>(base)->_runtimeparameter,translated,"runtime parameters");
      }
      if (auto cls = dynamic_cast<Class *>(translated)) {
         Class *base_class = static_cast<Class *>(base);
         link_list(cls->ClassAttribute,base_class->ClassAttribute,translated,"attributes");
         link_list(cls->ClassParameter,base_class->ClassParameter,translated,"parameters");
         link_list(cls->Role,base_class->Role,translated,"roles");
         link_list(cls->Constraints,base_class->Constraints,translated,"constraints");
         link_list(cls->Constraint,base_class->Constraint,translated,"external constraints");
      }
      if (auto domain = dynamic_cast<DomainType *>(translated)) {
         link_list(domain->Constraint,static_cast<DomainType *>(base)->Constraint,
            translated,"domain constraints");
      }
      if (auto attribute = dynamic_cast<AttrOrParam *>(translated)) {
         link_type(attribute->Type,static_cast<AttrOrParam *>(base)->Type);
      }
      if (auto function = dynamic_cast<FunctionDef *>(translated)) {
         FunctionDef *base_function = static_cast<FunctionDef *>(base);
         link_list(function->Argument,base_function->Argument,translated,"arguments");
         link_type(function->ResultType,base_function->ResultType);
      }
      if (auto argument = dynamic_cast<Argument *>(translated)) {
         link_type(argument->Type,static_cast<Argument *>(base)->Type);
      }
      if (auto multi = dynamic_cast<MultiValue *>(translated)) {
         MultiValue *base_multi = static_cast<MultiValue *>(base);
         if (multi->BaseType != nullptr && base_multi->BaseType != nullptr
            && multi->BaseType->getClass() != "Class" && base_multi->BaseType->getClass() != "Class") {
            link_type(multi->BaseType,base_multi->BaseType);
         }
      }
      if (auto attribute_ref = dynamic_cast<AttributeRefType *>(translated)) {
         link_list(attribute_ref->TypeRestriction,static_cast<AttributeRefType *>(base)->TypeRestriction,
            translated,"attribute path restrictions");
      }
      if (auto coord = dynamic_cast<CoordType *>(translated)) {
         link_list(coord->Axis,static_cast<CoordType *>(base)->Axis,translated,"coordinate dimensions");
      }
      if (auto enumeration = dynamic_cast<EnumType *>(translated)) {
         link_element(enumeration->TopNode,static_cast<EnumType *>(base)->TopNode);
      }
      if (auto node = dynamic_cast<EnumNode *>(translated)) {
         link_list(node->Node,static_cast<EnumNode *>(base)->Node,translated,"enumeration elements");
      }
      if (auto basket = dynamic_cast<MetaBasketDef *>(translated)) {
         link_list(basket->Members,static_cast<MetaBasketDef *>(base)->Members,translated,"metadata objects");
      }
   }

   void compare_multiplicity(MetaElement *translated,MetaElement *base,const string &property,
      const Multiplicity &actual,const Multiplicity &expected)
   {
      value(translated,base,property + " minimum",actual.Min,expected.Min);
      value(translated,base,property + " maximum",actual.Max,expected.Max);
   }

   template<typename T>
   void compare_ref_list(MetaElement *translated,MetaElement *base,const string &property,
      const list<T *> &actual,const list<T *> &expected,bool ordered = true)
   {
      vector<MetaElement *> actual_roots;
      vector<MetaElement *> expected_roots;
      for (T *element : actual) {
         actual_roots.push_back(element == nullptr ? nullptr : element->getTranslationOfRoot());
      }
      for (T *element : expected) {
         expected_roots.push_back(element == nullptr ? nullptr : element->getTranslationOfRoot());
      }
      if (!ordered) {
         sort(actual_roots.begin(),actual_roots.end());
         sort(expected_roots.begin(),expected_roots.end());
      }
      if (actual_roots != expected_roots) {
         mismatch(translated,base,property);
      }
   }

   void compare_expression_list(MetaElement *translated,MetaElement *base,const string &property,
      const list<Expression *> &actual,const list<Expression *> &expected)
   {
      vector<Expression *> left = as_vector(actual);
      vector<Expression *> right = as_vector(expected);
      if (left.size() != right.size()) {
         mismatch(translated,base,property + " count");
      }
      for (size_t i = 0; i < min(left.size(),right.size()); ++i) {
         if (!same_expression(left[i],right[i])) {
            mismatch(translated,base,property);
            return;
         }
      }
   }

   bool same_path(PathOrInspFactor *actual,PathOrInspFactor *expected)
   {
      if (actual == nullptr || expected == nullptr) {
         return actual == expected;
      }
      if (actual->PathEls.empty() && expected->PathEls.empty()) {
         return canonical_text(actual->_path) == canonical_text(expected->_path);
      }
      vector<PathEl *> left = as_vector(actual->PathEls);
      vector<PathEl *> right = as_vector(expected->PathEls);
      if (left.size() != right.size()) {
         return false;
      }
      for (size_t i = 0; i < left.size(); ++i) {
         if (left[i]->Kind != right[i]->Kind || left[i]->NumIndex != right[i]->NumIndex
            || left[i]->SpecIndex != right[i]->SpecIndex || !same_ref(left[i]->Ref,right[i]->Ref)) {
            return false;
         }
      }
      return same_ref(actual->Inspection,expected->Inspection);
   }

   bool same_expression(Expression *actual,Expression *expected)
   {
      if (actual == nullptr || expected == nullptr) {
         return actual == expected;
      }
      if (actual->getClass() != expected->getClass()) {
         return false;
      }
      if (auto left = dynamic_cast<UnaryExpr *>(actual)) {
         auto right = static_cast<UnaryExpr *>(expected);
         return left->Operation == right->Operation && same_expression(left->SubExpression,right->SubExpression);
      }
      if (auto left = dynamic_cast<CompoundExpr *>(actual)) {
         auto right = static_cast<CompoundExpr *>(expected);
         vector<Expression *> lv = as_vector(left->SubExpressions);
         vector<Expression *> rv = as_vector(right->SubExpressions);
         if (left->Operation != right->Operation || lv.size() != rv.size()) {
            return false;
         }
         for (size_t i = 0; i < lv.size(); ++i) {
            if (!same_expression(lv[i],rv[i])) {
               return false;
            }
         }
         return true;
      }
      if (auto left = dynamic_cast<PathOrInspFactor *>(actual)) {
         return same_path(left,static_cast<PathOrInspFactor *>(expected));
      }
      if (auto left = dynamic_cast<FunctionCall *>(actual)) {
         auto right = static_cast<FunctionCall *>(expected);
         vector<ActualArgument *> lv = as_vector(left->Arguments);
         vector<ActualArgument *> rv = as_vector(right->Arguments);
         if (!same_ref(left->Function,right->Function) || lv.size() != rv.size()) {
            return false;
         }
         for (size_t i = 0; i < lv.size(); ++i) {
            if (lv[i]->Kind != rv[i]->Kind || !same_ref(lv[i]->FormalArgument,rv[i]->FormalArgument)
               || !same_expression(lv[i]->Expression,rv[i]->Expression)) {
               return false;
            }
            vector<ClassRef *> lc = as_vector(lv[i]->ObjectClasses);
            vector<ClassRef *> rc = as_vector(rv[i]->ObjectClasses);
            if (lc.size() != rc.size()) return false;
            for (size_t j = 0; j < lc.size(); ++j) {
               if (!same_ref(lc[j]->Ref,rc[j]->Ref)) return false;
            }
         }
         return true;
      }
      if (auto left = dynamic_cast<RuntimeParamRef *>(actual)) {
         return same_ref(left->RuntimeParam,static_cast<RuntimeParamRef *>(expected)->RuntimeParam);
      }
      if (auto left = dynamic_cast<Constant *>(actual)) {
         auto right = static_cast<Constant *>(expected);
         return left->Kind == right->Kind
            && (left->Kind == Constant::Enumeration
               ? canonical_text(left->Value) == canonical_text(right->Value)
               : left->Value == right->Value);
      }
      if (auto left = dynamic_cast<ClassConst *>(actual)) {
         return same_ref(left->Class,static_cast<ClassConst *>(expected)->Class);
      }
      if (auto left = dynamic_cast<AttributeConst *>(actual)) {
         return same_ref(left->Attribute,static_cast<AttributeConst *>(expected)->Attribute);
      }
      if (auto left = dynamic_cast<UnitRef *>(actual)) {
         return same_ref(left->Unit,static_cast<UnitRef *>(expected)->Unit);
      }
      if (auto left = dynamic_cast<UnitFunction *>(actual)) {
         return left->Explanation == static_cast<UnitFunction *>(expected)->Explanation;
      }
      return true;
   }

   void compare_type(Type *translated,Type *base)
   {
      if (translated == nullptr || base == nullptr) {
         return;
      }
      if (auto domain = dynamic_cast<DomainType *>(translated)) {
         value(translated,base,"mandatory",domain->Mandatory,static_cast<DomainType *>(base)->Mandatory);
      }
      if (auto related = dynamic_cast<TypeRelatedType *>(translated)) {
         TypeRelatedType *base_related = static_cast<TypeRelatedType *>(base);
         if (related->BaseType != nullptr && base_related->BaseType != nullptr
            && related->BaseType->_translationOf == base_related->BaseType) {
            compare_element(related->BaseType,base_related->BaseType);
         }
         else {
            ref(translated,base,"base type",related->BaseType,base_related->BaseType);
         }
      }
      if (auto multi = dynamic_cast<MultiValue *>(translated)) {
         MultiValue *base_multi = static_cast<MultiValue *>(base);
         value(translated,base,"LIST/BAG ordering",multi->Ordered,base_multi->Ordered);
         compare_multiplicity(translated,base,"LIST/BAG cardinality",multi->Multiplicity,base_multi->Multiplicity);
         compare_ref_list(translated,base,"LIST/BAG restrictions",multi->TypeRestriction,base_multi->TypeRestriction);
      }
      if (auto related = dynamic_cast<ClassRelatedType *>(translated)) {
         ClassRelatedType *base_related = static_cast<ClassRelatedType *>(base);
         ref(translated,base,"class reference",related->_baseclass,base_related->_baseclass);
         compare_ref_list(translated,base,"class restrictions",related->_classrestriction,base_related->_classrestriction);
      }
      if (auto reference = dynamic_cast<ReferenceType *>(translated)) {
         value(translated,base,"external",reference->External,static_cast<ReferenceType *>(base)->External);
      }
      if (auto text = dynamic_cast<TextType *>(translated)) {
         TextType *base_text = static_cast<TextType *>(base);
         value(translated,base,"text kind",text->Kind,base_text->Kind);
         value(translated,base,"text length",text->MaxLength,base_text->MaxLength);
      }
      if (auto blackbox = dynamic_cast<BlackboxType *>(translated)) {
         value(translated,base,"blackbox kind",blackbox->Kind,static_cast<BlackboxType *>(base)->Kind);
      }
      if (auto num = dynamic_cast<NumType *>(translated)) {
         NumType *base_num = static_cast<NumType *>(base);
         value(translated,base,"minimum",num->Min,base_num->Min);
         value(translated,base,"maximum",num->Max,base_num->Max);
         value(translated,base,"circular",num->Circular,base_num->Circular);
         value(translated,base,"clockwise",num->Clockwise,base_num->Clockwise);
         value(translated,base,"direction",num->Direction,base_num->Direction);
         ref(translated,base,"unit",num->Unit,base_num->Unit);
         ref(translated,base,"reference system",num->RefSys,base_num->RefSys);
         value(translated,base,"reference system name",canonical_text(num->RefSysName),canonical_text(base_num->RefSysName));
         value(translated,base,"reference system axis",num->RefSysAxis,base_num->RefSysAxis);
      }
      if (auto coord = dynamic_cast<CoordType *>(translated)) {
         CoordType *base_coord = static_cast<CoordType *>(base);
         value(translated,base,"null axis",coord->NullAxis,base_coord->NullAxis);
         value(translated,base,"pi-half axis",coord->PiHalfAxis,base_coord->PiHalfAxis);
      }
      if (auto formatted = dynamic_cast<FormattedType *>(translated)) {
         FormattedType *base_formatted = static_cast<FormattedType *>(base);
         value(translated,base,"format",canonical_text(formatted->Format),canonical_text(base_formatted->Format));
         ref(translated,base,"format structure",formatted->Struct,base_formatted->Struct);
         ref(translated,base,"base format",formatted->BaseFormattedType,base_formatted->BaseFormattedType);
      }
      if (auto object = dynamic_cast<ObjectType *>(translated)) {
         value(translated,base,"multiple objects",object->Multiple,static_cast<ObjectType *>(base)->Multiple);
      }
      if (auto aref = dynamic_cast<AttributeRefType *>(translated)) {
         AttributeRefType *base_aref = static_cast<AttributeRefType *>(base);
         ref(translated,base,"attribute path restriction",aref->Of,base_aref->Of);
         if (!same_expression(aref->AttrRestriction,base_aref->AttrRestriction)) {
            mismatch(translated,base,"ATTRIBUTE OF restriction");
         }
         vector<Type *> actual = as_vector(aref->TypeRestriction);
         vector<Type *> expected = as_vector(base_aref->TypeRestriction);
         for (size_t i = 0; i < min(actual.size(),expected.size()); ++i) {
            compare_element(actual[i],expected[i]);
         }
      }
      if (auto enumeration = dynamic_cast<EnumType *>(translated)) {
         value(translated,base,"enumeration ordering",enumeration->Order,static_cast<EnumType *>(base)->Order);
      }
      if (auto tree = dynamic_cast<EnumTreeValueType *>(translated)) {
         ref(translated,base,"enumeration reference",tree->ET,static_cast<EnumTreeValueType *>(base)->ET);
      }
      if (auto line = dynamic_cast<LineType *>(translated)) {
         LineType *base_line = static_cast<LineType *>(base);
         value(translated,base,"line kind",line->Kind,base_line->Kind);
         value(translated,base,"maximum overlap",line->MaxOverlap,base_line->MaxOverlap);
         ref(translated,base,"vertex type",line->CoordType,base_line->CoordType);
         ref(translated,base,"line attributes",line->LAStructure,base_line->LAStructure);
         compare_line_forms(translated,base,line->LineForm,base_line->LineForm);
      }
   }

   void compare_element(MetaElement *translated,MetaElement *base)
   {
      if (translated == nullptr || base == nullptr || translated->getClass() != base->getClass()) {
         return;
      }
      if (auto extendable = dynamic_cast<ExtendableME *>(translated)) {
         ExtendableME *base_extendable = static_cast<ExtendableME *>(base);
         value(translated,base,"abstract",extendable->Abstract,base_extendable->Abstract);
         value(translated,base,"generic",extendable->Generic,base_extendable->Generic);
         value(translated,base,"final",extendable->Final,base_extendable->Final);
         value(translated,base,"extended",extendable->Extended,base_extendable->Extended);
         ref(translated,base,"extends",extendable->Super,base_extendable->Super);
      }
      if (auto model = dynamic_cast<Model *>(translated)) {
         Model *base_model = static_cast<Model *>(base);
         value(translated,base,"contracted",model->Contracted,base_model->Contracted);
         value(translated,base,"model kind",model->Kind,base_model->Kind);
         compare_imports(model,base_model);
      }
      compare_metadata(translated,base);
      if (auto topic = dynamic_cast<SubModel *>(translated)) {
         SubModel *base_topic = static_cast<SubModel *>(base);
         if (topic->_dataunit != nullptr && base_topic->_dataunit != nullptr) {
            value(translated,base,"view topic",topic->_dataunit->ViewUnit,base_topic->_dataunit->ViewUnit);
            ref(translated,base,"basket OID",topic->_dataunit->Oid,base_topic->_dataunit->Oid);
            ref(translated,base,"topic OID",topic->_dataunit->TopicOid,base_topic->_dataunit->TopicOid);
            compare_dependencies(topic,base_topic);
         }
      }
      if (auto cls = dynamic_cast<Class *>(translated)) {
         Class *base_class = static_cast<Class *>(base);
         value(translated,base,"class kind",cls->Kind,base_class->Kind);
         value(translated,base,"mandatory",cls->Mandatory,base_class->Mandatory);
         compare_multiplicity(translated,base,"association cardinality",cls->Multiplicity,base_class->Multiplicity);
         ref(translated,base,"object OID",cls->Oid,base_class->Oid);
         value(translated,base,"OID property",cls->OidProperty,base_class->OidProperty);
         value(translated,base,"NO OID",cls->NoOid,base_class->NoOid);
         ref(translated,base,"derived association",cls->View,base_class->View);
      }
      if (auto attribute = dynamic_cast<AttrOrParam *>(translated)) {
         AttrOrParam *base_attribute = static_cast<AttrOrParam *>(base);
         value(translated,base,"subdivision",attribute->SubdivisionKind,base_attribute->SubdivisionKind);
         value(translated,base,"transient",attribute->Transient,base_attribute->Transient);
         compare_expression_list(translated,base,"derivation",attribute->Derivates,base_attribute->Derivates);
      }
      if (auto role = dynamic_cast<Role *>(translated)) {
         Role *base_role = static_cast<Role *>(base);
         value(translated,base,"role strength",role->Strongness,base_role->Strongness);
         value(translated,base,"role ordering",role->Ordered,base_role->Ordered);
         compare_multiplicity(translated,base,"role cardinality",role->Multiplicity,base_role->Multiplicity);
         compare_expression_list(translated,base,"role derivation",role->Derivates,base_role->Derivates);
         value(translated,base,"role hiding",role->Hiding,base_role->Hiding);
      }
      if (auto unit = dynamic_cast<Unit *>(translated)) {
         Unit *base_unit = static_cast<Unit *>(base);
         value(translated,base,"unit kind",unit->Kind,base_unit->Kind);
         if (!same_expression(unit->Definition,base_unit->Definition)) mismatch(translated,base,"unit definition");
      }
      if (auto basket = dynamic_cast<MetaBasketDef *>(translated)) {
         MetaBasketDef *base_basket = static_cast<MetaBasketDef *>(base);
         value(translated,base,"metadata basket kind",basket->Kind,base_basket->Kind);
         ref(translated,base,"metadata topic",basket->MetaDataTopic,base_basket->MetaDataTopic);
         for (size_t i = 0; i < min(basket->Members.size(),base_basket->Members.size()); ++i) {
            auto actual = next(basket->Members.begin(),i);
            auto expected = next(base_basket->Members.begin(),i);
            value(translated,base,"metadata object name",(*actual)->Name,(*expected)->Name);
            ref(translated,base,"metadata object class",(*actual)->Class,(*expected)->Class);
         }
      }
      if (auto function = dynamic_cast<FunctionDef *>(translated)) {
         FunctionDef *base_function = static_cast<FunctionDef *>(base);
         value(translated,base,"function explanation",function->Explanation,base_function->Explanation);
      }
      if (auto argument = dynamic_cast<Argument *>(translated)) {
         value(translated,base,"argument kind",argument->Kind,static_cast<Argument *>(base)->Kind);
      }
      if (auto aref = dynamic_cast<AttributeRefType *>(translated)) {
         for (Type *restriction : aref->TypeRestriction) {
            if (restriction->_translationOf) compare_element(restriction,restriction->_translationOf);
         }
      }
      if (auto line_form = dynamic_cast<LineForm *>(translated)) {
         ref(translated,base,"line-form structure",line_form->Structure,static_cast<LineForm *>(base)->Structure);
      }
      if (auto view = dynamic_cast<View *>(translated)) {
         View *base_view = static_cast<View *>(base);
         value(translated,base,"view formation",view->FormationKind,base_view->FormationKind);
         value(translated,base,"transient view",view->Transient,base_view->Transient);
         value(translated,base,"OR NULL view bases",view->_orNullCount,base_view->_orNullCount);
         vector<string> actual_paths(view->_formationPaths.begin(),view->_formationPaths.end());
         vector<string> base_paths(base_view->_formationPaths.begin(),base_view->_formationPaths.end());
         transform(actual_paths.begin(),actual_paths.end(),actual_paths.begin(),[this](const string &s){return canonical_text(s);});
         transform(base_paths.begin(),base_paths.end(),base_paths.begin(),[this](const string &s){return canonical_text(s);});
         value(translated,base,"view formation paths",actual_paths,base_paths);
         compare_expression_list(translated,base,"view bases",view->FormationParameter,base_view->FormationParameter);
         if (!same_expression(view->Where,base_view->Where)) mismatch(translated,base,"view selection");
      }
      if (auto constraint = dynamic_cast<SimpleConstraint *>(translated)) {
         SimpleConstraint *base_constraint = static_cast<SimpleConstraint *>(base);
         value(translated,base,"constraint kind",constraint->Kind,base_constraint->Kind);
         value(translated,base,"constraint percentage",constraint->Percentage,base_constraint->Percentage);
         value(translated,base,"constraint percentage operator",constraint->_percentage_operation,base_constraint->_percentage_operation);
         if (!same_expression(constraint->LogicalExpression,base_constraint->LogicalExpression)) mismatch(translated,base,"constraint expression");
      }
      if (auto constraint = dynamic_cast<ExistenceConstraint *>(translated)) {
         ExistenceConstraint *base_constraint = static_cast<ExistenceConstraint *>(base);
         if (!same_path(constraint->Attr,base_constraint->Attr)) mismatch(translated,base,"existence attribute");
         vector<ExistenceDef *> actual = as_vector(constraint->ExistsIn);
         vector<ExistenceDef *> expected = as_vector(base_constraint->ExistsIn);
         if (actual.size() != expected.size()) mismatch(translated,base,"REQUIRED IN count");
         for (size_t i = 0; i < min(actual.size(),expected.size()); ++i) {
            if (!same_ref(actual[i]->Viewable,expected[i]->Viewable) || !same_path(actual[i],expected[i])) {
               mismatch(translated,base,"REQUIRED IN paths");
               break;
            }
         }
      }
      if (auto constraint = dynamic_cast<UniqueConstraint *>(translated)) {
         UniqueConstraint *base_constraint = static_cast<UniqueConstraint *>(base);
         value(translated,base,"uniqueness kind",constraint->Kind,base_constraint->Kind);
         compare_expression_list(translated,base,"uniqueness precondition",constraint->Where,base_constraint->Where);
         vector<PathOrInspFactor *> actual = as_vector(constraint->UniqueDef);
         vector<PathOrInspFactor *> expected = as_vector(base_constraint->UniqueDef);
         if (actual.size() != expected.size()) mismatch(translated,base,"unique paths count");
         for (size_t i = 0; i < min(actual.size(),expected.size()); ++i) {
            if (!same_path(actual[i],expected[i])) { mismatch(translated,base,"unique paths"); break; }
         }
      }
      if (auto constraint = dynamic_cast<SetConstraint *>(translated)) {
         SetConstraint *base_constraint = static_cast<SetConstraint *>(base);
         compare_expression_list(translated,base,"set precondition",constraint->Where,base_constraint->Where);
         if (!same_expression(constraint->Constraint,base_constraint->Constraint)) mismatch(translated,base,"set expression");
      }

      if (auto type = dynamic_cast<Type *>(translated)) {
         compare_type(type,static_cast<Type *>(base));
      }

      if (auto package = dynamic_cast<Package *>(translated)) {
         for (MetaElement *element : package->Element) {
            if (element->_translationOf != nullptr) compare_element(element,element->_translationOf);
         }
      }
      if (auto model = dynamic_cast<Model *>(translated)) {
         for (AttrOrParam *parameter : model->_runtimeparameter) {
            if (parameter->_translationOf != nullptr) compare_element(parameter,parameter->_translationOf);
         }
      }
      if (auto cls = dynamic_cast<Class *>(translated)) {
         for (AttrOrParam *attribute : cls->ClassAttribute) if (attribute->_translationOf) compare_element(attribute,attribute->_translationOf);
         for (AttrOrParam *parameter : cls->ClassParameter) if (parameter->_translationOf) compare_element(parameter,parameter->_translationOf);
         for (Role *role : cls->Role) if (role->_translationOf) compare_element(role,role->_translationOf);
         for (Constraint *constraint : cls->Constraints) if (constraint != nullptr && constraint->_translationOf) compare_element(constraint,constraint->_translationOf);
         for (Constraint *constraint : cls->Constraint) if (constraint != nullptr && constraint->_translationOf) compare_element(constraint,constraint->_translationOf);
      }
      if (auto domain = dynamic_cast<DomainType *>(translated)) {
         for (Constraint *constraint : domain->Constraint) {
            if (constraint != nullptr && constraint->_translationOf != nullptr) {
               compare_element(constraint,constraint->_translationOf);
            }
         }
      }
      if (auto attribute = dynamic_cast<AttrOrParam *>(translated)) {
         if (attribute->Type != nullptr && attribute->Type->_translationOf != nullptr) compare_element(attribute->Type,attribute->Type->_translationOf);
      }
      if (auto function = dynamic_cast<FunctionDef *>(translated)) {
         for (Argument *argument : function->Argument) if (argument->_translationOf) compare_element(argument,argument->_translationOf);
         if (function->ResultType != nullptr && function->ResultType->_translationOf) compare_element(function->ResultType,function->ResultType->_translationOf);
      }
      if (auto argument = dynamic_cast<Argument *>(translated)) {
         if (argument->Type != nullptr && argument->Type->_translationOf) compare_element(argument->Type,argument->Type->_translationOf);
      }
      if (auto coord = dynamic_cast<CoordType *>(translated)) {
         for (NumType *axis : coord->Axis) if (axis->_translationOf) compare_element(axis,axis->_translationOf);
      }
      if (auto enumeration = dynamic_cast<EnumType *>(translated)) {
         if (enumeration->TopNode != nullptr && enumeration->TopNode->_translationOf) compare_element(enumeration->TopNode,enumeration->TopNode->_translationOf);
      }
      if (auto node = dynamic_cast<EnumNode *>(translated)) {
         for (EnumNode *child : node->Node) if (child->_translationOf) compare_element(child,child->_translationOf);
      }
      if (auto basket = dynamic_cast<MetaBasketDef *>(translated)) {
         for (MetaObjectDef *object : basket->Members) if (object->_translationOf) compare_element(object,object->_translationOf);
      }
   }

   void compare_metadata(MetaElement *translated,MetaElement *base)
   {
      vector<pair<string,string>> actual;
      vector<pair<string,string>> expected;
      for (MetaAttribute *attribute : translated->MetaAttribute) actual.push_back({attribute->Name,attribute->Value});
      for (MetaAttribute *attribute : base->MetaAttribute) expected.push_back({attribute->Name,attribute->Value});
      sort(actual.begin(),actual.end());
      sort(expected.begin(),expected.end());
      if (actual != expected) mismatch(translated,base,"meta attributes");
   }

   void compare_line_forms(MetaElement *translated,MetaElement *base,
      const list<LineForm *> &actual,const list<LineForm *> &expected)
   {
      vector<LineForm *> remaining(expected.begin(),expected.end());
      for (LineForm *form : actual) {
         auto match = find_if(remaining.begin(),remaining.end(),[this,form](LineForm *candidate) {
            bool predefined = form != nullptr && (form->Name == "STRAIGHTS" || form->Name == "ARCS");
            return predefined ? candidate != nullptr && candidate->Name == form->Name : same_ref(form,candidate);
         });
         if (match == remaining.end()) { mismatch(translated,base,"line forms"); return; }
         remaining.erase(match);
      }
      if (!remaining.empty()) mismatch(translated,base,"line forms");
   }

   void compare_imports(Model *translated,Model *base)
   {
      list<Model *> actual;
      list<Model *> expected;
      for (Import *import : get_all_imports()) {
         if (import->ImportingP == translated && import->ImportedP != nullptr && import->ImportedP->Name != "INTERLIS") actual.push_back(static_cast<Model *>(import->ImportedP));
         if (import->ImportingP == base && import->ImportedP != nullptr && import->ImportedP->Name != "INTERLIS") expected.push_back(static_cast<Model *>(import->ImportedP));
      }
      compare_ref_list(translated,base,"imports",actual,expected,false);
   }

   void compare_dependencies(SubModel *translated,SubModel *base)
   {
      list<DataUnit *> actual;
      list<DataUnit *> expected;
      for (Dependency *dependency : get_all_dependencies()) {
         if (translated->_dataunit != nullptr && dependency->Using == translated->_dataunit) actual.push_back(dependency->Dependent);
         if (base->_dataunit != nullptr && dependency->Using == base->_dataunit) expected.push_back(dependency->Dependent);
      }
      compare_ref_list(translated,base,"topic dependencies",actual,expected,false);
   }
};

}

void check_model_translations()
{
   Checker().run();
}

}
