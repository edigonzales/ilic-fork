#include "SemanticChecker.h"

#include <algorithm>
#include <regex>
#include <set>
#include <unordered_set>
#include <vector>

#include "MetaModel.h"
#include "MetaModelStore.h"
#include "DiagnosticUtil.h"
#include "../util/Logger.h"

using namespace std;
using namespace util;

namespace metamodel {
namespace {

using Kind = ExpressionTypeKind;
using Descriptor = ExpressionTypeDescriptor;

Type *canonical_declared_type(Type *type)
{
   Type *current = type;
   unordered_set<Type *> seen;
   while (current != nullptr && current->ElementInPackage == nullptr && seen.insert(current).second) {
      Type *base = dynamic_cast<Type *>(current->Super);
      if (base == nullptr) {
         break;
      }
      current = base;
   }
   return current;
}

string declaration_kind(MetaElement *element)
{
   if (auto viewable = dynamic_cast<Class *>(element)) {
      switch (viewable->Kind) {
         case Class::Structure: return "STRUCTURE";
         case Class::ClassVal: return "CLASS";
         case Class::ViewVal: return "VIEW";
         case Class::Association: return "ASSOCIATION";
      }
   }
   if (dynamic_cast<SubModel *>(element) != nullptr) return "TOPIC";
   if (dynamic_cast<Model *>(element) != nullptr) return "MODEL";
   if (dynamic_cast<Type *>(element) != nullptr) return "DOMAIN";
   if (dynamic_cast<Graphic *>(element) != nullptr) return "GRAPHIC";
   return element == nullptr ? "DECLARATION" : element->getClass();
}

string diagnostic_type_name(Type *type)
{
   Type *declared = canonical_declared_type(type);
   if (declared != nullptr && declared->ElementInPackage != nullptr) {
      return get_path(declared);
   }
   if (type == nullptr) return "unknown type";
   if (dynamic_cast<FormattedType *>(type) != nullptr) return "FORMAT";
   if (dynamic_cast<ReferenceType *>(type) != nullptr) return "REFERENCE";
   if (auto text = dynamic_cast<TextType *>(type)) {
      return text->Kind == TextType::MText ? "MTEXT" : "TEXT";
   }
   if (dynamic_cast<NumType *>(type) != nullptr) return "NUMERIC";
   if (dynamic_cast<CoordType *>(type) != nullptr) return "COORD";
   if (dynamic_cast<EnumType *>(type) != nullptr) return "ENUMERATION";
   if (dynamic_cast<LineType *>(type) != nullptr) return "LINE";
   return type->getClass();
}

void append_related(
   vector<ilic::RelatedInformation> &target,
   MetaElement *element,
   const string &message
)
{
   auto information = related_information(element,message);
   target.insert(target.end(),information.begin(),information.end());
}

Descriptor descriptor_from_type(Type *type)
{
   Descriptor descriptor;
   if (type == nullptr) {
      return descriptor;
   }

   Type *declared = canonical_declared_type(type);
   Type *effective = declared == nullptr ? type : declared;
   descriptor.DeclaredType = effective;

   if (auto formatted = dynamic_cast<FormattedType *>(effective)) {
      descriptor.Kind = Kind::Formatted;
      return descriptor;
   }
   if (auto text = dynamic_cast<TextType *>(effective)) {
      descriptor.Kind = text->OIDType ? Kind::Oid : Kind::Text;
      return descriptor;
   }
   if (auto numeric = dynamic_cast<NumType *>(effective)) {
      descriptor.Kind = numeric->OIDType ? Kind::Oid : Kind::Numeric;
      return descriptor;
   }
   if (dynamic_cast<AnyOIDType *>(effective) != nullptr) {
      descriptor.Kind = Kind::Oid;
   }
   else if (dynamic_cast<BooleanType *>(effective) != nullptr) {
      descriptor.Kind = Kind::Boolean;
   }
   else if (dynamic_cast<CoordType *>(effective) != nullptr) {
      descriptor.Kind = Kind::Coordinate;
   }
   else if (dynamic_cast<EnumType *>(effective) != nullptr) {
      descriptor.Kind = Kind::Enumeration;
   }
   else if (dynamic_cast<EnumTreeValueType *>(effective) != nullptr) {
      descriptor.Kind = Kind::EnumTreeValue;
   }
   else if (auto reference = dynamic_cast<ReferenceType *>(effective)) {
      descriptor.Kind = Kind::Object;
      descriptor.Viewable = reference->_baseclass;
   }
   else if (auto object = dynamic_cast<ObjectType *>(effective)) {
      descriptor.Kind = Kind::Object;
      descriptor.Viewable = object->_baseclass;
   }
   else if (auto classReference = dynamic_cast<ClassRefType *>(effective)) {
      descriptor.Kind = Kind::ClassReference;
      descriptor.Viewable = classReference->_baseclass;
   }
   else if (dynamic_cast<AttributeRefType *>(effective) != nullptr) {
      descriptor.Kind = Kind::AttributeReference;
   }
   else if (auto multi = dynamic_cast<MultiValue *>(effective)) {
      descriptor.Kind = Kind::Structure;
      descriptor.Viewable = dynamic_cast<Class *>(multi->BaseType);
   }
   else if (auto viewable = dynamic_cast<Class *>(effective)) {
      descriptor.Kind = viewable->Kind == Class::Structure ? Kind::Structure : Kind::Object;
      descriptor.Viewable = viewable;
   }
   return descriptor;
}

void collect_enum_nodes(EnumNode *node,const string &prefix,set<string> &values)
{
   if (node == nullptr) {
      return;
   }
   for (EnumNode *child : node->Node) {
      string path = prefix.empty() ? child->Name : prefix + "." + child->Name;
      values.insert(path);
      collect_enum_nodes(child,path,values);
   }
}

void collect_enum_values(EnumType *type,set<string> &values)
{
   if (type == nullptr) {
      return;
   }
   collect_enum_values(dynamic_cast<EnumType *>(type->Super),values);
   collect_enum_nodes(type->TopNode,"",values);
}

EnumType *enumeration_type(const Descriptor &descriptor)
{
   if (descriptor.Kind == Kind::Enumeration) {
      return dynamic_cast<EnumType *>(descriptor.DeclaredType);
   }
   if (descriptor.Kind == Kind::EnumTreeValue) {
      auto tree = dynamic_cast<EnumTreeValueType *>(descriptor.DeclaredType);
      return tree == nullptr ? nullptr : tree->ET;
   }
   return nullptr;
}

Expression *literal_expression(Expression *expression)
{
   Expression *current = expression;
   while (auto unary = dynamic_cast<UnaryExpr *>(current)) {
      if (unary->Operation != UnaryExpr::None) {
         break;
      }
      current = unary->SubExpression;
   }
   return current;
}

bool is_enum_literal(Expression *expression)
{
   auto constant = dynamic_cast<Constant *>(literal_expression(expression));
   return constant != nullptr && constant->Kind == Constant::Enumeration &&
          constant->ResolvedType.Kind == Kind::Enumeration &&
          constant->ResolvedType.DeclaredType == nullptr;
}

bool validate_enum_literal(Logger &logger,Expression *expression,const Descriptor &target,bool allowNodes)
{
   auto constant = dynamic_cast<Constant *>(literal_expression(expression));
   EnumType *type = enumeration_type(target);
   if (constant == nullptr || type == nullptr) {
      return false;
   }

   set<string> values;
   collect_enum_values(type,values);
   bool found = values.find(constant->Value) != values.end();
   if (found && !allowNodes) {
      string prefix = constant->Value + ".";
      found = none_of(values.begin(),values.end(),[&](const string &value) {
         return value.rfind(prefix,0) == 0;
      });
   }
   if (!found) {
      logger.error(DiagnosticId::EnumerationValueInvalid,
         "enumeration value #" + constant->Value + " is not valid for the compared domain",
         diagnostic_range(expression));
      return false;
   }
   constant->ResolvedType = target;
   return true;
}

string declared_type_name(Type *type)
{
   for (Type *current = type; current != nullptr; current = dynamic_cast<Type *>(current->Super)) {
      if (!current->Name.empty() && current->Name != "TYPE") {
         return current->Name;
      }
   }
   return "";
}

bool validate_standard_formatted_value(FormattedType *type,const string &value)
{
   string name = declared_type_name(type);
   smatch match;
   if (name == "XMLDate") {
      if (!regex_match(value,match,regex("([0-9]{4})-([0-9]{2})-([0-9]{2})"))) {
         return false;
      }
      int year = stoi(match[1]);
      int month = stoi(match[2]);
      int day = stoi(match[3]);
      return year >= 1582 && year <= 2999 && month >= 1 && month <= 12 && day >= 1 && day <= 31;
   }
   if (name == "XMLTime") {
      if (!regex_match(value,match,regex("([0-9]{2}):([0-9]{2}):([0-9]{2}(?:\\.[0-9]{1,3})?)"))) {
         return false;
      }
      return stoi(match[1]) <= 23 && stoi(match[2]) <= 59 && stod(match[3]) < 60.0;
   }
   if (name == "XMLDateTime") {
      size_t separator = value.find('T');
      if (separator == string::npos) {
         return false;
      }
      FormattedType date;
      date.Name = "XMLDate";
      FormattedType time;
      time.Name = "XMLTime";
      return validate_standard_formatted_value(&date,value.substr(0,separator)) &&
             validate_standard_formatted_value(&time,value.substr(separator + 1));
   }

   // Explicit bounds are part of every restricted formatted domain and their
   // canonical representation is lexically sortable by definition.
   if (!type->Min.empty() && value < type->Min) {
      return false;
   }
   if (!type->Max.empty() && value > type->Max) {
      return false;
   }
   return true;
}

bool is_text_literal(Expression *expression,string *value = nullptr)
{
   auto constant = dynamic_cast<Constant *>(literal_expression(expression));
   if (constant == nullptr || constant->Kind != Constant::Text) {
      return false;
   }
   if (value != nullptr) {
      *value = constant->Value;
   }
   return true;
}

bool validate_formatted_literal(Logger &logger,Expression *literal,const Descriptor &formatted)
{
   string value;
   if (!is_text_literal(literal,&value)) {
      return false;
   }
   auto type = dynamic_cast<FormattedType *>(formatted.DeclaredType);
   if (type == nullptr || !validate_standard_formatted_value(type,value)) {
      logger.error(DiagnosticId::TypeFormattedValueOutOfRange,
         "formatted value \"" + value + "\" is outside the domain format or range",
         diagnostic_range(literal));
      return false;
   }
   return true;
}

bool same_declared_type(const Descriptor &left,const Descriptor &right)
{
   return left.DeclaredType != nullptr && left.DeclaredType == right.DeclaredType;
}

bool package_is_same_or_extension(Package *candidate,Package *base)
{
   for (Package *current = candidate; current != nullptr; current = current->_super) {
      if (current == base) {
         return true;
      }
   }
   return false;
}

bool class_is_same_or_extension(Class *candidate,Class *base)
{
   for (Class *current = candidate; current != nullptr;
        current = dynamic_cast<Class *>(current->Super)) {
      if (current == base) {
         return true;
      }
   }
   return false;
}

bool extended_class_chain_contains(Class *extended,Class *candidateBase)
{
   for (Class *current = extended; current != nullptr && current->Extended;) {
      current = dynamic_cast<Class *>(current->Super);
      if (current == candidateBase) {
         return true;
      }
   }
   return false;
}

Model *containing_model(MetaElement *element)
{
   for (MetaElement *current = element; current != nullptr;
        current = current->ElementInPackage) {
      if (auto model = dynamic_cast<Model *>(current)) {
         return model;
      }
   }
   return nullptr;
}

SubModel *containing_topic(MetaElement *element)
{
   if (auto attribute = dynamic_cast<AttrOrParam *>(element)) {
      element = attribute->AttrParent;
   }
   else if (auto role = dynamic_cast<Role *>(element)) {
      element = role->Association;
   }
   else if (auto type = dynamic_cast<Type *>(element)) {
      if (type->ElementInPackage == nullptr && type->_attr != nullptr) {
         element = type->_attr->AttrParent;
      }
   }
   for (MetaElement *current = element; current != nullptr;
        current = current->ElementInPackage) {
      if (auto topic = dynamic_cast<SubModel *>(current)) {
         return topic;
      }
   }
   return nullptr;
}

bool model_imports_directly(const MetaModelStore &store,Model *source,Model *target)
{
   if (source == nullptr || target == nullptr || source == target || target->Name == "INTERLIS") {
      return true;
   }
   for (Import *import : store.imports()) {
      if (import != nullptr && import->ImportingP == source && import->ImportedP == target) {
         return true;
      }
   }
   return false;
}

bool topic_has_dependency(const MetaModelStore &store,SubModel *source,SubModel *target)
{
   if (source == nullptr || target == nullptr || package_is_same_or_extension(source,target)) {
      return true;
   }
   for (Dependency *dependency : store.dependencies()) {
      if (dependency != nullptr && dependency->Using == source->_dataunit &&
          dependency->Dependent == target->_dataunit) {
         return true;
      }
   }
   return false;
}

Multiplicity effective_role_cardinality(Role *role)
{
   if (role == nullptr) {
      return {};
   }
   if (role->MultiplicityDefined) {
      Multiplicity value = role->Multiplicity;
      if (value.Min < 0) {
         value.Min = 0;
      }
      return value;
   }
   if (auto base = dynamic_cast<Role *>(role->Super)) {
      return effective_role_cardinality(base);
   }

   Multiplicity value;
   if (role->Strongness == Role::Comp) {
      Model *model = containing_model(role->Association);
      value.Min = model != nullptr && model->iliVersion == "2.4" ? 1 : 0;
      value.Max = 1;
   }
   else {
      value.Min = 0;
      value.Max = -1;
   }
   return value;
}

bool cardinality_is_subset(const Multiplicity &candidate,const Multiplicity &base)
{
   if (candidate.Min < base.Min) {
      return false;
   }
   if (base.Max < 0) {
      return true;
   }
   return candidate.Max >= 0 && candidate.Max <= base.Max;
}

bool domain_is_mandatory(Type *type)
{
   if (auto domain = dynamic_cast<DomainType *>(type)) {
      return domain->Mandatory;
   }
   if (auto viewable = dynamic_cast<Class *>(type)) {
      return viewable->Mandatory;
   }
   return false;
}

Multiplicity attribute_cardinality(Type *type)
{
   Multiplicity value;
   if (auto multi = dynamic_cast<MultiValue *>(type)) {
      value = multi->Multiplicity;
      if (value.Min < 0) {
         value.Min = 0;
      }
      if (value.Max < 0 && multi->Multiplicity.Min >= 0) {
         value.Max = -1;
      }
   }
   else {
      value.Min = 0;
      value.Max = 1;
   }
   if (domain_is_mandatory(type) && value.Min == 0) {
      value.Min = 1;
   }
   return value;
}

bool declared_type_is_same_or_extension(Type *candidate,Type *base)
{
   Type *expected = canonical_declared_type(base);
   for (Type *current = canonical_declared_type(candidate); current != nullptr;
        current = dynamic_cast<Type *>(current->Super)) {
      if (current == expected) {
         return true;
      }
   }
   return false;
}

bool validate_unique_enum_nodes(Logger &logger,EnumNode *parent,const string &prefix)
{
   if (parent == nullptr) {
      return true;
   }
   bool valid = true;
   set<string> siblings;
   for (EnumNode *node : parent->Node) {
      if (node == nullptr) {
         continue;
      }
      string path = prefix.empty() ? node->Name : prefix + "." + node->Name;
      if (!siblings.insert(node->Name).second) {
         logger.error(DiagnosticId::EnumerationDuplicateElement,
            "duplicate enumeration element " + path,diagnostic_range(node));
         valid = false;
      }
      if (!validate_unique_enum_nodes(logger,node,path)) {
         valid = false;
      }
   }
   return valid;
}

void validate_enum_extension_nodes(Logger &logger,EnumNode *parent,const string &prefix,
                                   const set<string> &baseValues)
{
   if (parent == nullptr) {
      return;
   }
   for (EnumNode *node : parent->Node) {
      if (node == nullptr) {
         continue;
      }
      string path = prefix.empty() ? node->Name : prefix + "." + node->Name;
      if (baseValues.find(path) != baseValues.end() && node->Node.empty() && !node->Final) {
         logger.error(DiagnosticId::EnumerationExtensionDuplicate,
            "enumeration extension duplicates element " + path,diagnostic_range(node));
      }
      validate_enum_extension_nodes(logger,node,path,baseValues);
   }
}

int decimal_accuracy(const string &value)
{
   size_t exponent = value.find_first_of("eES");
   string mantissa = value.substr(0,exponent);
   size_t point = mantissa.find('.');
   return point == string::npos ? 0 : static_cast<int>(mantissa.size() - point - 1);
}

struct ExactDecimal {
   bool Negative = false;
   string Digits;
   int Scale = 0;
   bool Valid = false;
};

// Coordinate range specialization is defined on decimal source values. Keep
// scaled values exact instead of introducing binary floating-point rounding.
ExactDecimal parse_exact_decimal(const string &value)
{
   ExactDecimal result;
   if (value.empty()) {
      return result;
   }

   size_t offset = 0;
   if (value[offset] == '+' || value[offset] == '-') {
      result.Negative = value[offset] == '-';
      ++offset;
   }
   size_t exponentPosition = value.find_first_of("eES",offset);
   string mantissa = value.substr(offset,exponentPosition == string::npos ? string::npos :
                                  exponentPosition - offset);
   int exponent = 0;
   try {
      if (exponentPosition != string::npos) {
         exponent = stoi(value.substr(exponentPosition + 1));
      }
   }
   catch (...) {
      return result;
   }

   size_t point = mantissa.find('.');
   if (point != string::npos && mantissa.find('.',point + 1) != string::npos) {
      return result;
   }
   int fractionDigits = point == string::npos ? 0 :
      static_cast<int>(mantissa.size() - point - 1);
   for (char digit : mantissa) {
      if (digit == '.') {
         continue;
      }
      if (digit < '0' || digit > '9') {
         return result;
      }
      result.Digits.push_back(digit);
   }
   size_t firstNonZero = result.Digits.find_first_not_of('0');
   if (firstNonZero == string::npos) {
      result.Digits = "0";
      result.Negative = false;
      result.Scale = 0;
      result.Valid = true;
      return result;
   }
   result.Digits.erase(0,firstNonZero);
   result.Scale = fractionDigits - exponent;
   while (result.Digits.size() > 1 && result.Digits.back() == '0') {
      result.Digits.pop_back();
      --result.Scale;
   }
   result.Valid = true;
   return result;
}

int compare_exact_decimal(const string &leftValue,const string &rightValue)
{
   ExactDecimal left = parse_exact_decimal(leftValue);
   ExactDecimal right = parse_exact_decimal(rightValue);
   if (!left.Valid || !right.Valid) {
      return leftValue < rightValue ? -1 : leftValue > rightValue ? 1 : 0;
   }
   if (left.Negative != right.Negative) {
      return left.Negative ? -1 : 1;
   }

   int leftMagnitude = static_cast<int>(left.Digits.size()) - left.Scale;
   int rightMagnitude = static_cast<int>(right.Digits.size()) - right.Scale;
   int comparison = 0;
   if (leftMagnitude != rightMagnitude) {
      comparison = leftMagnitude < rightMagnitude ? -1 : 1;
   }
   else {
      size_t width = max(left.Digits.size(),right.Digits.size());
      string leftDigits = left.Digits + string(width - left.Digits.size(),'0');
      string rightDigits = right.Digits + string(width - right.Digits.size(),'0');
      comparison = leftDigits < rightDigits ? -1 : leftDigits > rightDigits ? 1 : 0;
   }
   return left.Negative ? -comparison : comparison;
}

GenericDef *generic_definition_in(Model *model,DomainType *generic)
{
   if (model == nullptr || generic == nullptr) {
      return nullptr;
   }
   for (MetaElement *element : model->Element) {
      auto context = dynamic_cast<Context *>(element);
      if (context == nullptr) {
         continue;
      }
      for (GenericDef *definition : context->GenericDefinitions) {
         if (definition != nullptr && !definition->GenericDomain.empty() &&
             definition->GenericDomain.front() == generic) {
            return definition;
         }
      }
   }
   return nullptr;
}

vector<Model *> directly_imported_models(const MetaModelStore &store,Model *model)
{
   vector<Model *> result;
   for (Import *import : store.imports()) {
      if (import != nullptr && import->ImportingP == model) {
         if (auto imported = dynamic_cast<Model *>(import->ImportedP)) {
            result.push_back(imported);
         }
      }
   }
   reverse(result.begin(),result.end());
   return result;
}

GenericDef *generic_definition_in_imports(const MetaModelStore &store,Model *model,DomainType *generic,
                                          unordered_set<Model *> &visited)
{
   if (model == nullptr || !visited.insert(model).second) {
      return nullptr;
   }
   // RefHB 3.8 makes contexts transitively visible; ili2c resolves the most
   // recently declared import first and stops at the first effective mapping.
   for (Model *imported : directly_imported_models(store,model)) {
      if (GenericDef *definition = generic_definition_in(imported,generic)) {
         return definition;
      }
      if (GenericDef *definition = generic_definition_in_imports(store,imported,generic,visited)) {
         return definition;
      }
   }
   return nullptr;
}

GenericDef *visible_generic_definition(const MetaModelStore &store,Model *model,DomainType *generic)
{
   if (GenericDef *definition = generic_definition_in(model,generic)) {
      return definition;
   }
   unordered_set<Model *> visited;
   return generic_definition_in_imports(store,model,generic,visited);
}

enum class CoordinateFitError {
   None,
   MissingCoordinate,
   DimensionMismatch,
   MissingAxis,
   MinimumBelowRange,
   MaximumAboveRange
};

struct CoordinateFitResult {
   CoordinateFitError Error = CoordinateFitError::None;
   size_t Axis = 0;
   string Actual;
   string Expected;

   bool fits() const
   {
      return Error == CoordinateFitError::None;
   }
};

CoordinateFitResult coordinate_type_fit(CoordType *concrete,CoordType *generic)
{
   if (concrete == nullptr || generic == nullptr) {
      return {CoordinateFitError::MissingCoordinate};
   }
   if (concrete->Axis.size() != generic->Axis.size()) {
      return {
         CoordinateFitError::DimensionMismatch,
         0,
         to_string(concrete->Axis.size()),
         to_string(generic->Axis.size())
      };
   }
   auto concreteAxis = concrete->Axis.begin();
   auto genericAxis = generic->Axis.begin();
   size_t axis = 1;
   for (; concreteAxis != concrete->Axis.end(); ++concreteAxis,++genericAxis) {
      NumType *candidate = *concreteAxis;
      NumType *base = *genericAxis;
      if (candidate == nullptr || base == nullptr) {
         return {CoordinateFitError::MissingAxis,axis};
      }
      if (!base->Min.empty() &&
          (candidate->Min.empty() || compare_exact_decimal(candidate->Min,base->Min) < 0)) {
         return {
            CoordinateFitError::MinimumBelowRange,
            axis,
            candidate->Min.empty() ? "unbounded" : candidate->Min,
            base->Min
         };
      }
      if (!base->Max.empty() &&
          (candidate->Max.empty() || compare_exact_decimal(candidate->Max,base->Max) > 0)) {
         return {
            CoordinateFitError::MaximumAboveRange,
            axis,
            candidate->Max.empty() ? "unbounded" : candidate->Max,
            base->Max
         };
      }
      ++axis;
   }
   return {};
}

class Checker {
public:
   Checker(const MetaModelStore &store,Logger &logger) : store_(store),logger_(logger) {}

   Descriptor evaluate(Expression *expression,DomainType *domain = nullptr)
   {
      if (expression == nullptr) {
         return {};
      }
      auto cached = evaluated.find(expression);
      if (cached != evaluated.end()) {
         return expression->ResolvedType;
      }
      if (!evaluating.insert(expression).second) {
         return {};
      }

      Descriptor result;
      if (auto unary = dynamic_cast<UnaryExpr *>(expression)) {
         Descriptor operand = evaluate(unary->SubExpression,domain);
         if (unary->Operation == UnaryExpr::Not) {
            require(operand,Kind::Boolean,DiagnosticId::ExpressionLogicalRequired,
               "logical expression required",expression);
            result.Kind = Kind::Boolean;
         }
         else if (unary->Operation == UnaryExpr::Defined) {
            result.Kind = Kind::Boolean;
         }
         else {
            result = operand;
         }
      }
      else if (auto compound = dynamic_cast<CompoundExpr *>(expression)) {
         vector<Expression *> operands(compound->SubExpressions.begin(),compound->SubExpressions.end());
         vector<Descriptor> types;
         for (Expression *operand : operands) {
            types.push_back(evaluate(operand,domain));
         }
         result = validate_compound(compound,operands,types);
      }
      else if (auto path = dynamic_cast<PathOrInspFactor *>(expression)) {
         result = path_type(path,domain);
      }
      else if (auto function = dynamic_cast<FunctionCall *>(expression)) {
         result = descriptor_from_type(function->Function == nullptr ? nullptr : function->Function->ResultType);
         validate_arguments(function,domain);
      }
      else if (auto parameter = dynamic_cast<RuntimeParamRef *>(expression)) {
         result = descriptor_from_type(parameter->RuntimeParam == nullptr ? nullptr : parameter->RuntimeParam->Type);
      }
      else if (auto constant = dynamic_cast<Constant *>(expression)) {
         if (constant->Kind == Constant::Undefined) result.Kind = Kind::Undefined;
         else if (constant->Kind == Constant::Numeric) result.Kind = Kind::Numeric;
         else if (constant->Kind == Constant::Text) result.Kind = Kind::Text;
         else if (constant->Value == "true" || constant->Value == "false" ||
                  constant->Value == "INTERLIS.true" || constant->Value == "INTERLIS.false") {
            result.Kind = Kind::Boolean;
         }
         else result.Kind = Kind::Enumeration;
      }
      else if (auto classConstant = dynamic_cast<ClassConst *>(expression)) {
         result.Kind = Kind::ClassReference;
         result.Viewable = classConstant->Class;
      }
      else if (dynamic_cast<AttributeConst *>(expression) != nullptr) {
         result.Kind = Kind::AttributeReference;
      }

      expression->ResolvedType = result;
      evaluating.erase(expression);
      evaluated.insert(expression);
      return result;
   }

   void check_constraint(Constraint *constraint)
   {
      if (constraint == nullptr || !checkedConstraints.insert(constraint).second) {
         return;
      }
      DomainType *domain = constraint->toDomain;
      if (domain != nullptr &&
          (dynamic_cast<ClassRefType *>(canonical_declared_type(domain)) != nullptr ||
           dynamic_cast<AttributeRefType *>(canonical_declared_type(domain)) != nullptr)) {
         logger_.error(DiagnosticId::ConstraintDomainTargetUnsupported,
            "domain constraints are not supported for CLASS, STRUCTURE, or ATTRIBUTE domains",
            diagnostic_range(constraint));
      }

      if (auto simple = dynamic_cast<SimpleConstraint *>(constraint)) {
         Descriptor value = evaluate(simple->LogicalExpression,domain);
         require(value,Kind::Boolean,DiagnosticId::ExpressionLogicalRequired,
            "logical expression required",constraint);
      }
      else if (auto existence = dynamic_cast<ExistenceConstraint *>(constraint)) {
         Descriptor required = evaluate(existence->Attr,domain);
         for (ExistenceDef *candidate : existence->ExistsIn) {
            Descriptor available = evaluate(candidate,domain);
            if (!equality_compatible(existence->Attr,required,candidate,available,false)) {
               logger_.error(DiagnosticId::ConstraintExistenceTypeMismatch,
                  "existence constraint paths have incompatible datatypes",
                  diagnostic_range(constraint));
            }
         }
      }
      else if (auto unique = dynamic_cast<UniqueConstraint *>(constraint)) {
         for (Expression *where : unique->Where) {
            require(evaluate(where,domain),Kind::Boolean,
               DiagnosticId::ExpressionLogicalRequired,"logical expression required",where);
         }
         if (unique->PerBasket && unique->Kind == UniqueConstraint::LocalU) {
            // RefHB 2.4 section 3.12 and ili2c uniquenessConstraint: LOCAL is
            // already scoped by its owning object and cannot also be BASKET.
            logger_.error(DiagnosticId::ConstraintUniqueBasketLocal,
               "UNIQUE cannot combine BASKET and LOCAL",diagnostic_range(constraint));
         }
         for (PathOrInspFactor *path : unique->UniqueDef) {
            evaluate(path,domain);
            if (unique->Kind != UniqueConstraint::GlobalU || path == nullptr) {
               continue;
            }
            // Every hop in a global uniqueness key denotes one value. A
            // collection-valued attribute or role would produce several keys
            // for one object and is prohibited by RefHB 3.12/ili2c uniqueEl.
            for (PathEl *element : path->PathEls) {
               if (element == nullptr) {
                  continue;
               }
               if (auto attribute = dynamic_cast<AttrOrParam *>(element->Ref)) {
                  Multiplicity cardinality = attribute_cardinality(attribute->Type);
                  if (cardinality.Max < 0 || cardinality.Max > 1) {
                     logger_.error(DiagnosticId::ConstraintGlobalUniqueMultivalueAttribute,
                        "global UNIQUE path contains multivalued attribute " +
                           attribute->Name,diagnostic_range(constraint));
                  }
               }
               else if (auto role = dynamic_cast<Role *>(element->Ref)) {
                  if (path->OccurrenceScope == role->Association) {
                     // A role named inside its own association selects that
                     // association instance's endpoint. Its external access
                     // cardinality applies only when navigating from the
                     // opposite target class (ili2c PathElAbstractClassRole).
                     continue;
                  }
                  Multiplicity cardinality = effective_role_cardinality(role);
                  if (cardinality.Max < 0 || cardinality.Max > 1) {
                     logger_.error(DiagnosticId::ConstraintGlobalUniqueMultivalueRole,
                        "global UNIQUE path contains multivalued role " +
                           role->Name,diagnostic_range(constraint));
                  }
               }
            }
         }
      }
      else if (auto set = dynamic_cast<SetConstraint *>(constraint)) {
         if (set->ToClass != nullptr && set->ToClass->Kind == Class::Structure) {
            logger_.error(DiagnosticId::ConstraintSetOnStructure,
               "SET CONSTRAINT is not allowed in a STRUCTURE",diagnostic_range(constraint));
         }
         for (Expression *where : set->Where) {
            require(evaluate(where,domain),Kind::Boolean,
               DiagnosticId::ExpressionLogicalRequired,"logical expression required",where);
         }
         require(evaluate(set->Constraint,domain),Kind::Boolean,
            DiagnosticId::ExpressionLogicalRequired,"logical expression required",constraint);
      }
   }

   void check_package(Package *package)
   {
      if (package == nullptr || !checkedPackages.insert(package).second) {
         return;
      }
      if (auto topic = dynamic_cast<SubModel *>(package)) {
         check_topic_abstract_classes(topic);
         check_topic_generics(topic);
      }
      check_package_namespace(package);
      for (MetaElement *element : package->Element) {
         if (auto child = dynamic_cast<Package *>(element)) {
            check_package(child);
         }
         if (auto viewable = dynamic_cast<Class *>(element)) {
            check_class(viewable);
         }
         else if (auto domain = dynamic_cast<DomainType *>(element)) {
            check_type(domain,containing_model(domain));
            for (Constraint *constraint : domain->Constraint) {
               check_constraint(constraint);
            }
         }
         else if (auto context = dynamic_cast<Context *>(element)) {
            check_context(context);
         }
         if (auto graphic = dynamic_cast<Graphic *>(element)) {
            require_topic_dependency(containing_topic(graphic),
                                     containing_topic(graphic->Base),graphic,
                                     "graphic base");
            require(evaluate(graphic->Where),Kind::Boolean,
               DiagnosticId::ExpressionLogicalRequired,"logical expression required",graphic);
         }
         if (auto basket = dynamic_cast<MetaBasketDef *>(element)) {
            require_model_import(containing_model(basket),
                                 basket->MetaDataTopic == nullptr ? nullptr :
                                    containing_model(basket->MetaDataTopic->ElementInPackage),
                                 basket,"metadata basket topic");
         }
         if (auto rule = dynamic_cast<DrawingRule *>(element)) {
            for (CondSignParamAssignment *assignment : rule->Rule) {
               require(evaluate(assignment->Where),Kind::Boolean,
                  DiagnosticId::ExpressionLogicalRequired,"logical expression required",rule);
               for (SignParamAssignment *parameter : assignment->Assignments) {
                  evaluate(parameter->Assignment);
               }
            }
         }
      }
   }

private:
   const MetaModelStore &store_;
   Logger &logger_;
   unordered_set<Expression *> evaluating;
   unordered_set<Expression *> evaluated;
   unordered_set<Constraint *> checkedConstraints;
   unordered_set<Package *> checkedPackages;
   unordered_set<Class *> checkedClasses;
   unordered_set<Type *> checkedTypes;

   void check_context(Context *context)
   {
      if (context == nullptr) {
         return;
      }
      Model *model = containing_model(context);
      for (GenericDef *definition : context->GenericDefinitions) {
         if (definition == nullptr || definition->GenericDomain.empty()) {
            continue;
         }
         DomainType *generic = definition->GenericDomain.front();
         auto genericCoordinate = dynamic_cast<CoordType *>(generic);
         if (genericCoordinate == nullptr || !generic->Generic) {
            logger_.error(DiagnosticId::GenericContextDefaultDomainRequired,
               "context " + context->Name + " requires a GENERIC coordinate domain",
               diagnostic_range(definition));
            continue;
         }

         unordered_set<Model *> visited;
         GenericDef *imported = generic_definition_in_imports(store_,model,generic,visited);
         for (DomainType *concrete : definition->ConcreteDomain) {
            auto concreteCoordinate = dynamic_cast<CoordType *>(concrete);
            CoordinateFitResult fit =
               coordinate_type_fit(concreteCoordinate,genericCoordinate);
            if (fit.Error == CoordinateFitError::MinimumBelowRange ||
                fit.Error == CoordinateFitError::MaximumAboveRange) {
               const string bound = fit.Error ==
                  CoordinateFitError::MinimumBelowRange ? "minimum" : "maximum";
               const string relation = fit.Error ==
                  CoordinateFitError::MinimumBelowRange ?
                     "is below the allowed minimum" :
                     "exceeds the allowed maximum";
               vector<ilic::RelatedInformation> related;
               append_related(related,concrete,
                  "Concrete coordinate domain: " + get_path(concrete));
               append_related(related,generic,
                  "Generic coordinate domain: " + get_path(generic));
               logger_.error(DiagnosticId::GenericCoordRangeMismatch,
                  "context mapping " + get_path(concrete) + " to " +
                     get_path(generic) + " is outside the generic coordinate " +
                     "range on axis " + to_string(fit.Axis) + ": " + bound +
                     " " + fit.Actual + " " + relation + " " + fit.Expected,
                  diagnostic_range(definition),
                  std::move(related)
               );
               continue;
            }
            if (!fit.fits()) {
               logger_.error(DiagnosticId::GenericContextConcreteDomainIncompatible,
                  "concrete domain " + (concrete == nullptr ? string("???") : concrete->Name) +
                     " is not a compatible specialization of generic domain " + generic->Name,
                  diagnostic_range(definition));
               continue;
            }
            if (imported == nullptr) {
               continue;
            }
            bool allowed = false;
            for (DomainType *base : imported->ConcreteDomain) {
               if (declared_type_is_same_or_extension(concrete,base)) {
                  allowed = true;
                  break;
               }
            }
            if (!allowed) {
               logger_.error(DiagnosticId::GenericContextImportedDomainExtensionRequired,
                  "concrete domain " + concrete->Name +
                     " does not extend a concrete domain from the imported context",
                  diagnostic_range(definition));
            }
         }
      }
   }

   DomainType *generic_domain(Type *type)
   {
      auto domain = dynamic_cast<DomainType *>(canonical_declared_type(type));
      return domain != nullptr && domain->Generic ? domain : nullptr;
   }

   void collect_generic_domains(Type *type,set<DomainType *> &domains,
                                unordered_set<Type *> &visitedTypes,
                                unordered_set<Class *> &visitedViewables)
   {
      if (type == nullptr || !visitedTypes.insert(type).second) {
         return;
      }
      if (DomainType *generic = generic_domain(type)) {
         domains.insert(generic);
      }

      Type *declared = canonical_declared_type(type);
      Type *effective = declared == nullptr ? type : declared;
      if (auto line = dynamic_cast<LineType *>(effective)) {
         if (DomainType *generic = generic_domain(line->CoordType)) {
            domains.insert(generic);
         }
      }
      if (auto multi = dynamic_cast<MultiValue *>(effective)) {
         if (auto component = dynamic_cast<Class *>(multi->BaseType)) {
            collect_generic_domains(component,domains,visitedTypes,visitedViewables);
         }
         else {
            collect_generic_domains(multi->BaseType,domains,visitedTypes,visitedViewables);
         }
         for (Type *restriction : multi->TypeRestriction) {
            collect_generic_domains(restriction,domains,visitedTypes,visitedViewables);
         }
      }
      if (auto viewable = dynamic_cast<Class *>(effective)) {
         collect_generic_domains(viewable,domains,visitedTypes,visitedViewables);
      }
   }

   void collect_generic_domains(Class *viewable,set<DomainType *> &domains,
                                unordered_set<Type *> &visitedTypes,
                                unordered_set<Class *> &visitedViewables)
   {
      if (viewable == nullptr || !visitedViewables.insert(viewable).second) {
         return;
      }
      for (AttrOrParam *attribute : viewable->ClassAttribute) {
         if (attribute != nullptr) {
            collect_generic_domains(attribute->Type,domains,visitedTypes,visitedViewables);
         }
      }
      collect_generic_domains(dynamic_cast<Class *>(viewable->Super),domains,
                              visitedTypes,visitedViewables);
   }

   void collect_topic_generics(SubModel *topic,set<DomainType *> &domains,
                               unordered_set<SubModel *> &visitedTopics,
                               unordered_set<Model *> &scopeModels,
                               unordered_set<Type *> &visitedTypes,
                               unordered_set<Class *> &visitedViewables)
   {
      if (topic == nullptr || !visitedTopics.insert(topic).second) {
         return;
      }
      if (Model *model = containing_model(topic)) {
         scopeModels.insert(model);
      }
      for (MetaElement *element : topic->Element) {
         if (auto viewable = dynamic_cast<Class *>(element)) {
            collect_generic_domains(viewable,domains,visitedTypes,visitedViewables);
         }
      }
      collect_topic_generics(dynamic_cast<SubModel *>(topic->_super),domains,
                             visitedTopics,scopeModels,visitedTypes,visitedViewables);
   }

   void check_topic_generics(SubModel *topic)
   {
      if (topic == nullptr || topic->_dataunit == nullptr || topic->_dataunit->Abstract) {
         return;
      }

      set<DomainType *> used;
      unordered_set<SubModel *> visitedTopics;
      unordered_set<Model *> scopeModels;
      unordered_set<Type *> visitedTypes;
      unordered_set<Class *> visitedViewables;
      collect_topic_generics(topic,used,visitedTopics,scopeModels,visitedTypes,visitedViewables);

      // Model-level structures can specialize an imported structure used by
      // the topic. RefHB 3.5/3.8 and ili2c therefore include all viewables in
      // the lexical model scope when determining indirect generic usage.
      for (Model *model : scopeModels) {
         for (MetaElement *element : model->Element) {
            if (auto viewable = dynamic_cast<Class *>(element)) {
               collect_generic_domains(viewable,used,visitedTypes,visitedViewables);
            }
         }
      }

      Model *model = containing_model(topic);
      set<DomainType *> deferred;
      for (const SubModel::DeferredGenericRef &reference : topic->DeferredGenerics) {
         DomainType *domain = reference.Domain;
         if (domain == nullptr) {
            continue;
         }
         deferred.insert(domain);
         if (!domain->Generic || dynamic_cast<CoordType *>(domain) == nullptr) {
            logger_.error(DiagnosticId::GenericDeferredCoordinateDomainRequired,
               "DEFERRED GENERICS requires a GENERIC coordinate domain",
               diagnostic_range(topic));
         }
         if (visible_generic_definition(store_,model,domain) == nullptr) {
            logger_.error(DiagnosticId::GenericDeferredContextRequired,
               "deferred generic domain " + domain->Name + " has no visible context",
               diagnostic_range(topic));
         }
         if (used.find(domain) == used.end()) {
            logger_.error(DiagnosticId::GenericDeferredDomainUnused,
               "deferred generic domain " + domain->Name + " is not used by topic " +
                  topic->Name,diagnostic_range(topic));
         }
      }

      for (DomainType *domain : used) {
            GenericDef *definition = visible_generic_definition(store_,model,domain);
         if (definition == nullptr) {
            logger_.error(DiagnosticId::GenericDeferredContextRequired,
               "generic domain " + domain->Name + " has no visible context",
               diagnostic_range(topic));
         }
         else if (definition->ConcreteDomain.size() > 1 && deferred.find(domain) == deferred.end()) {
            logger_.error(DiagnosticId::GenericDeferredListingRequired,
               "generic domain " + domain->Name + " must be listed in DEFERRED GENERICS",
               diagnostic_range(topic));
         }
      }
   }

   void require(const Descriptor &descriptor,Kind expected,DiagnosticId id,
                const string &message,const MMObject *owner)
   {
      if (descriptor.Kind != Kind::Unknown && descriptor.Kind != expected) {
         logger_.error(id,message,diagnostic_range(owner));
      }
   }

   Descriptor path_type(PathOrInspFactor *path,DomainType *domain)
   {
      if (path == nullptr || path->PathEls.empty()) {
         return {};
      }
      PathEl *terminal = path->PathEls.back();
      MetaElement *target = terminal->Ref;
      if (auto attribute = dynamic_cast<AttrOrParam *>(target)) {
         target = attribute->AttrParent;
      }
      else if (auto role = dynamic_cast<Role *>(target)) {
         target = role->_baseclass;
      }
      require_topic_dependency(containing_topic(path->OccurrencePackage),
                               containing_topic(target),path,"object path");
      if (terminal->Kind == PathEl::This) {
         DomainType *valueDomain = domain != nullptr ? domain : dynamic_cast<DomainType *>(path->OccurrenceScope);
         if (valueDomain != nullptr) {
            return descriptor_from_type(valueDomain);
         }
      }
      if (auto attribute = dynamic_cast<AttrOrParam *>(terminal->Ref)) {
         return descriptor_from_type(attribute->Type);
      }
      if (auto role = dynamic_cast<Role *>(terminal->Ref)) {
         Descriptor descriptor;
         descriptor.Kind = Kind::Object;
         descriptor.DeclaredType = role;
         descriptor.Viewable = role->_baseclass;
         return descriptor;
      }
      if (auto viewable = dynamic_cast<Class *>(terminal->Ref)) {
         Descriptor descriptor;
         descriptor.Kind = Kind::Object;
         descriptor.DeclaredType = viewable;
         descriptor.Viewable = viewable;
         return descriptor;
      }
      return {};
   }

   Descriptor validate_compound(CompoundExpr *compound,const vector<Expression *> &operands,
                                const vector<Descriptor> &types)
   {
      Descriptor result;
      switch (compound->Operation) {
         case CompoundExpr_OperationType::And:
         case CompoundExpr_OperationType::Or:
         case CompoundExpr_OperationType::Implication:
            for (const Descriptor &type : types) {
               require(type,Kind::Boolean,DiagnosticId::ExpressionLogicalRequired,
                  "logical expression required",compound);
            }
            result.Kind = Kind::Boolean;
            break;
         case CompoundExpr_OperationType::Plus:
         case CompoundExpr_OperationType::Minus:
         case CompoundExpr_OperationType::Mult:
         case CompoundExpr_OperationType::Div:
            for (const Descriptor &type : types) {
               require(type,Kind::Numeric,DiagnosticId::ExpressionNumericRequired,
                  "numeric expression required",compound);
            }
            result.Kind = Kind::Numeric;
            break;
         case CompoundExpr_OperationType::Relation_Equal:
         case CompoundExpr_OperationType::Relation_NotEqual:
            if (operands.size() >= 2 && !equality_compatible(operands[0],types[0],operands[1],types[1],true)) {
               logger_.error(DiagnosticId::ExpressionComparisonTypeMismatch,
                  "incompatible datatypes",diagnostic_range(compound));
            }
            result.Kind = Kind::Boolean;
            break;
         case CompoundExpr_OperationType::Relation_Less:
         case CompoundExpr_OperationType::Relation_LessOrEqual:
         case CompoundExpr_OperationType::Relation_Greater:
         case CompoundExpr_OperationType::Relation_GreaterOrEqual:
            if (operands.size() >= 2 && !ordering_compatible(operands[0],types[0],operands[1],types[1])) {
               logger_.error(DiagnosticId::ExpressionOrderingTypeMismatch,
                  "incompatible datatypes for ordering comparison",
                  diagnostic_range(compound));
            }
            result.Kind = Kind::Boolean;
            break;
      }
      return result;
   }

   bool equality_compatible(Expression *leftExpression,const Descriptor &left,
                            Expression *rightExpression,const Descriptor &right,bool reportLiteral)
   {
      // RefHB 3.13 and ili2c validateEqualsArgumentTypes: comparison is based
      // on semantic value categories, with enum and formatted literals checked
      // against the declared domain on the opposite side.
      if (left.Kind == Kind::Unknown || right.Kind == Kind::Unknown ||
          left.Kind == Kind::Undefined || right.Kind == Kind::Undefined) {
         return true;
      }
      if (left.Kind == Kind::Formatted && right.Kind == Kind::Text) {
         return is_text_literal(rightExpression) &&
               (!reportLiteral || validate_formatted_literal(logger_,rightExpression,left));
      }
      if (left.Kind == Kind::Text && right.Kind == Kind::Formatted) {
         return is_text_literal(leftExpression) &&
                (!reportLiteral || validate_formatted_literal(logger_,leftExpression,right));
      }
      if ((left.Kind == Kind::Enumeration || left.Kind == Kind::EnumTreeValue) && is_enum_literal(rightExpression)) {
         return validate_enum_literal(logger_,rightExpression,left,left.Kind == Kind::EnumTreeValue);
      }
      if (is_enum_literal(leftExpression) &&
          (right.Kind == Kind::Enumeration || right.Kind == Kind::EnumTreeValue)) {
         return validate_enum_literal(logger_,leftExpression,right,right.Kind == Kind::EnumTreeValue);
      }
      if (left.Kind != right.Kind) {
         return false;
      }
      switch (left.Kind) {
         case Kind::Enumeration:
         case Kind::EnumTreeValue:
            if (is_enum_literal(leftExpression) && is_enum_literal(rightExpression)) {
               return true;
            }
            return same_declared_type(left,right);
         case Kind::Boolean:
         case Kind::Numeric:
         case Kind::Text:
         case Kind::Formatted:
         case Kind::Coordinate:
         case Kind::Oid:
         case Kind::ClassReference:
         case Kind::AttributeReference:
         case Kind::Object:
         case Kind::Structure:
            return true;
         default:
            return false;
      }
   }

   bool ordering_compatible(Expression *leftExpression,const Descriptor &left,
                            Expression *rightExpression,const Descriptor &right)
   {
      // RefHB 3.13 permits ordering only for numeric, formatted, and explicitly
      // ORDERED enumeration values; ili2c applies the same category boundary.
      if (left.Kind == Kind::Unknown || right.Kind == Kind::Unknown) {
         return true;
      }
      if (left.Kind == Kind::Numeric && right.Kind == Kind::Numeric) {
         return true;
      }
      if (left.Kind == Kind::Formatted && right.Kind == Kind::Formatted) {
         return true;
      }
      if (left.Kind == Kind::Formatted && right.Kind == Kind::Text) {
         return is_text_literal(rightExpression) && validate_formatted_literal(logger_,rightExpression,left);
      }
      if (left.Kind == Kind::Text && right.Kind == Kind::Formatted) {
         return is_text_literal(leftExpression) && validate_formatted_literal(logger_,leftExpression,right);
      }

      Descriptor enumType;
      if ((left.Kind == Kind::Enumeration || left.Kind == Kind::EnumTreeValue) && is_enum_literal(rightExpression)) {
         enumType = left;
         if (!validate_enum_literal(logger_,rightExpression,left,left.Kind == Kind::EnumTreeValue)) return false;
      }
      else if (is_enum_literal(leftExpression) &&
               (right.Kind == Kind::Enumeration || right.Kind == Kind::EnumTreeValue)) {
         enumType = right;
         if (!validate_enum_literal(logger_,leftExpression,right,right.Kind == Kind::EnumTreeValue)) return false;
      }
      else if ((left.Kind == Kind::Enumeration || left.Kind == Kind::EnumTreeValue) &&
               left.Kind == right.Kind && same_declared_type(left,right)) {
         enumType = left;
      }
      else {
         return false;
      }

      EnumType *enumeration = enumeration_type(enumType);
      return enumeration != nullptr && enumeration->Order == EnumType::Ordered;
   }

   void validate_arguments(FunctionCall *call,DomainType *domain)
   {
      if (call == nullptr || call->Function == nullptr) {
         return;
      }
      auto formal = call->Function->Argument.begin();
      auto actual = call->Arguments.begin();
      while (formal != call->Function->Argument.end() && actual != call->Arguments.end()) {
         ActualArgument *argument = *actual;
         if (argument != nullptr && argument->Kind == ActualArgument::ExpressionVal && argument->Expression != nullptr) {
            Descriptor expected = descriptor_from_type((*formal)->Type);
            Descriptor supplied = evaluate(argument->Expression,domain);
            bool anyStructure = expected.Kind == Kind::Structure && expected.Viewable != nullptr &&
                                expected.Viewable->Name == "ANYSTRUCTURE" &&
                                (supplied.Kind == Kind::Object || supplied.Kind == Kind::Structure);
            if (!anyStructure && !equality_compatible(argument->Expression,supplied,nullptr,expected,false)) {
               logger_.error(DiagnosticId::FunctionArgumentTypeMismatch,
                  "incompatible function argument type for " + call->Function->Name,
                  diagnostic_range(argument));
            }
         }
         ++formal;
         ++actual;
      }
   }

   void check_class(Class *viewable)
   {
      if (viewable == nullptr || !checkedClasses.insert(viewable).second) {
         return;
      }
      if (viewable->Kind == Class::Association) {
         check_association(viewable);
      }
      else if (viewable->Kind == Class::ClassVal || viewable->Kind == Class::Structure) {
         check_class_specialization(viewable);
      }
      for (Constraint *constraint : viewable->Constraints) check_constraint(constraint);
      for (Constraint *constraint : viewable->Constraint) check_constraint(constraint);
      check_attribute_namespace(viewable);
      for (AttrOrParam *attribute : viewable->ClassAttribute) {
         check_attribute(attribute);
         for (Expression *derivation : attribute->Derivates) evaluate(derivation);
      }
      for (Role *role : viewable->Role) {
         for (Expression *derivation : role->Derivates) evaluate(derivation);
      }
      if (auto view = dynamic_cast<View *>(viewable)) {
         for (AttrOrParam *attribute : view->ClassAttribute) {
            auto object = attribute == nullptr ? nullptr :
               dynamic_cast<ObjectType *>(attribute->Type);
            if (object != nullptr) {
               require_topic_dependency(containing_topic(view),
                                        containing_topic(object->_baseclass),attribute,
                                        "view base");
            }
         }
         if (view->Where != nullptr) {
            require(evaluate(view->Where),Kind::Boolean,
               DiagnosticId::ExpressionLogicalRequired,"logical expression required",
               view->Where);
         }
         for (Expression *parameter : view->FormationParameter) evaluate(parameter);
      }
   }

   void check_class_specialization(Class *viewable)
   {
      auto base = dynamic_cast<Class *>(viewable->Super);
      auto topic = dynamic_cast<SubModel *>(viewable->ElementInPackage);
      auto baseTopic = base == nullptr ? nullptr :
         dynamic_cast<SubModel *>(base->ElementInPackage);
      if (base == nullptr || topic == nullptr || baseTopic == nullptr) {
         return;
      }

      // RefHB 3.5: same-name specialization across inherited topics is the
      // EXTENDED form; EXTENDS must introduce a distinct class name.
      if (!viewable->Extended && viewable->Name == base->Name &&
          topic != baseTopic && package_is_same_or_extension(topic,baseTopic)) {
         const string viewablePath = get_path(viewable);
         const string basePath = get_path(base);
         logger_.error(DiagnosticId::ClassExtendedRequired,
            declaration_kind(viewable) + " " + viewablePath + " in TOPIC " +
               get_path(topic) + " extends same-named " + declaration_kind(base) +
               " " + basePath + " from extended TOPIC " + get_path(baseTopic) +
               "; declare " + viewable->Name + " using the EXTENDED form",
            diagnostic_range(viewable),
            related_information(base,"Base declaration: " + basePath)
         );
         return;
      }
      if (!viewable->Extended) {
         return;
      }

      // An inherited class can either be extended in place or specialized by
      // a differently named class in the topic chain, but not both.
      for (SubModel *scope = topic; scope != nullptr; scope =
           dynamic_cast<SubModel *>(scope->_super)) {
         for (MetaElement *element : scope->Element) {
            auto other = dynamic_cast<Class *>(element);
            auto otherBase = other == nullptr ? nullptr :
               dynamic_cast<Class *>(other->Super);
            if (other == nullptr || other == viewable || other == base || otherBase == nullptr) {
               continue;
            }
            if (extended_class_chain_contains(viewable,otherBase)) {
               logger_.error(DiagnosticId::ClassSpecializationConflict,
                  "class or structure " + viewable->Name +
                     " cannot be EXTENDED because " + other->Name +
                     " specializes the same base",diagnostic_range(viewable));
               return;
            }
         }
      }
   }

   void check_topic_abstract_classes(SubModel *topic)
   {
      if (topic == nullptr || topic->_dataunit == nullptr || topic->_dataunit->Abstract) {
         return;
      }

      vector<Class *> effectiveClasses;
      unordered_set<Class *> hiddenClasses;
      for (SubModel *scope = topic; scope != nullptr; scope =
           dynamic_cast<SubModel *>(scope->_super)) {
         for (MetaElement *element : scope->Element) {
            auto viewable = dynamic_cast<Class *>(element);
            if (viewable == nullptr || viewable->Kind != Class::ClassVal) {
               continue;
            }
            if (viewable->Extended) {
               if (auto base = dynamic_cast<Class *>(viewable->Super)) {
                  hiddenClasses.insert(base);
               }
            }
            if (hiddenClasses.find(viewable) == hiddenClasses.end()) {
               effectiveClasses.push_back(viewable);
            }
         }
      }

      // RefHB 3.5 requires concrete topics to concretize their effectively
      // inherited abstract identifiable classes. Structures and views do not
      // create independently transferable class instances and are excluded.
      for (Class *abstractClass : effectiveClasses) {
         if (!abstractClass->Abstract) {
            continue;
         }
         bool concretized = false;
         for (Class *candidate : effectiveClasses) {
            if (!candidate->Abstract && candidate != abstractClass &&
                class_is_same_or_extension(candidate,abstractClass)) {
               concretized = true;
               break;
            }
         }
         if (!concretized) {
            logger_.error(DiagnosticId::ClassAbstractInConcreteTopic,
               "concrete topic " + topic->Name +
                  " contains abstract class " + abstractClass->Name +
                  " without a concrete extension",diagnostic_range(topic));
         }
      }
   }

   void check_package_namespace(Package *package)
   {
      unordered_map<string,MetaElement *> localNames;
      for (MetaElement *element : package->Element) {
         if (element == nullptr || element->ElementInPackage != package ||
             dynamic_cast<DataUnit *>(element) != nullptr ||
             element->Name.empty() || element->Name == "???") {
            continue;
         }
         auto existing = localNames.find(element->Name);
         if (existing != localNames.end()) {
            MetaElement *first = existing->second;
            const string container = get_path(package);
            logger_.error(DiagnosticId::NamespaceDuplicateDeclaration,
               declaration_kind(element) + " " + element->Name +
                  " duplicates " + declaration_kind(first) + " " + first->Name +
                  " in " + declaration_kind(package) + " " + container,
               diagnostic_range(element),
               related_information(first,
                  "First declaration: " + declaration_kind(first) + " " +
                     get_path(first))
            );
         }
         else {
            localNames[element->Name] = element;
         }
      }

      if (auto model = dynamic_cast<Model *>(package)) {
         unordered_set<string> runtimeParameterNames;
         for (AttrOrParam *parameter : model->_runtimeparameter) {
            if (parameter == nullptr || parameter->Name.empty()) {
               continue;
            }
            if (!runtimeParameterNames.insert(parameter->Name).second) {
               logger_.error(DiagnosticId::ModelRuntimeParameterDuplicate,
                  "duplicate runtime parameter " + parameter->Name + " in model " +
                     model->Name,diagnostic_range(parameter));
            }
         }
      }

      auto topic = dynamic_cast<SubModel *>(package);
      if (topic == nullptr || topic->_super == nullptr) {
         return;
      }
      unordered_map<string,MetaElement *> inheritedNames;
      for (SubModel *scope = dynamic_cast<SubModel *>(topic->_super); scope != nullptr;
           scope = dynamic_cast<SubModel *>(scope->_super)) {
         for (MetaElement *element : scope->Element) {
            if (element != nullptr && element->ElementInPackage == scope &&
                dynamic_cast<DataUnit *>(element) == nullptr &&
                !element->Name.empty() && element->Name != "???" &&
                inheritedNames.find(element->Name) == inheritedNames.end()) {
               inheritedNames[element->Name] = element;
            }
         }
      }
      for (MetaElement *element : package->Element) {
         auto inherited = element == nullptr ? inheritedNames.end() :
            inheritedNames.find(element->Name);
         if (inherited == inheritedNames.end()) {
            continue;
         }
         auto extendable = dynamic_cast<ExtendableME *>(element);
         bool explicitOverride = extendable != nullptr && extendable->Extended &&
                                 extendable->Super == inherited->second;
         auto viewable = dynamic_cast<Class *>(element);
         bool sameNameSpecialization = viewable != nullptr &&
            viewable->Super == inherited->second && !viewable->Extended;
         if (!explicitOverride && !sameNameSpecialization) {
            logger_.error(DiagnosticId::NamespaceInheritedDeclarationConflict,
               "declaration " + element->Name + " in topic " + topic->Name +
                  " conflicts with an inherited declaration",diagnostic_range(element));
         }
      }
   }

   void check_attribute_namespace(Class *viewable)
   {
      unordered_map<string,AttrOrParam *> names;
      for (AttrOrParam *attribute : viewable->ClassAttribute) {
         if (attribute == nullptr) {
            continue;
         }
         auto first = names.find(attribute->Name);
         if (first != names.end()) {
            logger_.error(DiagnosticId::NamespaceMemberDuplicate,
               "duplicate attribute or view-base name " + attribute->Name +
                  " in " + viewable->Name,diagnostic_range(attribute),
               related_information(first->second,
                  "First attribute or view-base declaration: " +
                     first->second->Name));
         }
         else {
            names[attribute->Name] = attribute;
         }
      }
   }

   void check_type(Type *type,Model *occurrenceModel)
   {
      if (type == nullptr || !checkedTypes.insert(type).second) {
         return;
      }
      if (auto enumeration = dynamic_cast<EnumType *>(type)) {
         validate_unique_enum_nodes(logger_,enumeration->TopNode,"");
         if (enumeration->ElementInPackage != nullptr) {
            if (auto base = dynamic_cast<EnumType *>(enumeration->Super)) {
               set<string> baseValues;
               collect_enum_values(base,baseValues);
               validate_enum_extension_nodes(logger_,enumeration->TopNode,"",baseValues);
            }
         }
      }
      if (auto line = dynamic_cast<LineType *>(type)) {
         if (!line->MaxOverlap.empty() && line->CoordType != nullptr &&
             !line->CoordType->Axis.empty()) {
            NumType *axis = line->CoordType->Axis.front();
            string coordinateLimit = axis == nullptr ? "" :
               (!axis->Min.empty() ? axis->Min : axis->Max);
            if (!coordinateLimit.empty() &&
                decimal_accuracy(line->MaxOverlap) != decimal_accuracy(coordinateLimit)) {
               logger_.error(DiagnosticId::TypeLineOverlapPrecision,
                  "line overlap must use the coordinate domain precision",
                  diagnostic_range(line));
            }
         }
      }
      if (auto coordinate = dynamic_cast<CoordType *>(type)) {
         for (NumType *axis : coordinate->Axis) {
            check_type(axis,occurrenceModel);
         }
      }
      if (auto numeric = dynamic_cast<NumType *>(type)) {
         if (dynamic_cast<FormattedType *>(numeric) == nullptr &&
             !numeric->Min.empty() && !numeric->Max.empty() &&
             compare_exact_decimal(numeric->Min,numeric->Max) > 0) {
            logger_.error(DiagnosticId::ValueMinimumExceedsMaximum,
               "minimum value must not exceed maximum value",
               diagnostic_range(numeric));
         }
         require_model_import(occurrenceModel,
                              numeric->Unit == nullptr ? nullptr :
                                 containing_model(numeric->Unit),
                              numeric,"unit reference");
      }
      if (auto multi = dynamic_cast<MultiValue *>(type)) {
         check_type(multi->BaseType,occurrenceModel);
      }
   }

   void check_attribute(AttrOrParam *attribute)
   {
      if (attribute == nullptr) {
         return;
      }
      check_type(attribute->Type,containing_model(attribute->AttrParent));
      AttrOrParam *base = attribute->Extending;
      if (base == nullptr || base->Type == nullptr || attribute->Type == nullptr) {
         return;
      }

      Multiplicity baseCardinality = attribute_cardinality(base->Type);
      Multiplicity cardinality = attribute_cardinality(attribute->Type);
      if (!cardinality_is_subset(cardinality,baseCardinality)) {
         logger_.error(DiagnosticId::AttributeCardinalityExtension,
            "cardinality of extended attribute " + attribute->Name +
               " is not a subset of its base",diagnostic_range(attribute));
      }
      if (attribute->Transient != base->Transient) {
         logger_.error(DiagnosticId::AttributeExtensionTransient,
            "TRANSIENT mode of extended attribute " + attribute->Name +
               " differs from its base",diagnostic_range(attribute));
      }

      Type *baseDeclared = canonical_declared_type(base->Type);
      Type *declared = canonical_declared_type(attribute->Type);
      if (baseDeclared != nullptr && declared != nullptr &&
          baseDeclared->ElementInPackage != nullptr && declared->ElementInPackage != nullptr &&
          !declared_type_is_same_or_extension(attribute->Type,base->Type)) {
         const string actualType = diagnostic_type_name(attribute->Type);
         const string expectedType = diagnostic_type_name(base->Type);
         vector<ilic::RelatedInformation> related;
         append_related(related,base,
            "Base attribute: " + get_path(base) + " uses " + expectedType);
         append_related(related,baseDeclared,
            "Expected base domain: " + expectedType);
         logger_.error(DiagnosticId::AttributeIncompatibleExtension,
            "extended attribute " + get_path(attribute) + " uses " + actualType +
               ", but its type must extend " + expectedType,
            diagnostic_range(attribute),
            std::move(related)
         );
         return;
      }

      if (base->Type->getClass() != attribute->Type->getClass()) {
         const string actualType = diagnostic_type_name(attribute->Type);
         const string expectedType = diagnostic_type_name(base->Type);
         logger_.error(DiagnosticId::AttributeIncompatibleExtension,
            "extended attribute " + get_path(attribute) + " uses " + actualType +
               ", but its type must be compatible with " + expectedType,
            diagnostic_range(attribute),
            related_information(base,
               "Base attribute: " + get_path(base) + " uses " + expectedType)
         );
         return;
      }
      if (auto text = dynamic_cast<TextType *>(attribute->Type)) {
         auto baseText = static_cast<TextType *>(base->Type);
         if (text->Kind != baseText->Kind) {
            logger_.error(DiagnosticId::AttributeExtensionTextKind,
               "TEXT and MTEXT kinds may not change in an attribute extension",
               diagnostic_range(attribute));
         }
         if (baseText->MaxLength >= 0 &&
             (text->MaxLength < 0 || text->MaxLength > baseText->MaxLength)) {
            logger_.error(DiagnosticId::AttributeExtensionTextLength,
               "text length of extended attribute exceeds its base",
               diagnostic_range(attribute));
         }
      }
      if (attribute->TypeExplicitlyDefined) {
         if (auto enumeration = dynamic_cast<EnumType *>(attribute->Type)) {
            auto baseEnumeration = static_cast<EnumType *>(base->Type);
            if (declared->ElementInPackage == nullptr && baseDeclared->ElementInPackage == nullptr) {
               set<string> baseValues;
               collect_enum_values(baseEnumeration,baseValues);
               validate_enum_extension_nodes(logger_,enumeration->TopNode,"",baseValues);
            }
         }
      }
      if (auto multi = dynamic_cast<MultiValue *>(attribute->Type)) {
         auto baseMulti = static_cast<MultiValue *>(base->Type);
         if (baseMulti->Ordered && !multi->Ordered) {
            logger_.error(DiagnosticId::AttributeExtensionListOrdering,
               "an extended LIST attribute may not become an unordered BAG",
               diagnostic_range(attribute));
         }
         auto target = dynamic_cast<Class *>(multi->BaseType);
         auto baseTarget = dynamic_cast<Class *>(baseMulti->BaseType);
         if (target != nullptr && baseTarget != nullptr &&
             !class_is_same_or_extension(target,baseTarget)) {
            logger_.error(DiagnosticId::AttributeExtensionStructureTarget,
               "structure target of extended attribute does not extend its base",
               diagnostic_range(attribute));
         }
      }
   }

   void require_model_import(Model *source,Model *target,const MMObject *owner,
                             const string &kind)
   {
      if (!model_imports_directly(store_,source,target)) {
         logger_.error(DiagnosticId::ModelImportRequired,
            kind + " requires model " + source->Name + " to import " + target->Name,
            diagnostic_range(owner));
      }
   }

   void require_topic_dependency(SubModel *source,SubModel *target,const MMObject *owner,
                                 const string &kind)
   {
      if (!topic_has_dependency(store_,source,target)) {
         logger_.error(DiagnosticId::TopicDependencyRequired,
            kind + " requires topic " + source->Name + " to depend on " + target->Name,
            diagnostic_reference_range(owner,"dependency"),
            related_information(target,"Required topic is declared here"));
      }
   }

   void check_association(Class *association)
   {
      // RefHB 3.7 and ili2c RoleDef.setExtending define role compatibility
      // independently of the syntax used to declare the association.
      set<string> roleNames;
      int aggregateRoles = 0;
      for (Role *role : association->Role) {
         if (role == nullptr) {
            continue;
         }
         if (!roleNames.insert(role->Name).second) {
            const string associationName = association->Name.empty() || association->Name == "???"
               ? "anonymous association" : "association \"" + association->Name + "\"";
            logger_.error(DiagnosticId::AssociationDuplicateRole,
               "duplicate role \"" + role->Name + "\" in " + associationName,
               diagnostic_range(role));
         }
         if (role->Strongness != Role::Assoc) {
            ++aggregateRoles;
         }
         if (role->_baseclass != nullptr && role->_baseclass->Kind == Class::Structure) {
            logger_.error(DiagnosticId::ReferenceClassOrAssociationRequired,
               "role " + role->Name + " may only reference a class or association",
               diagnostic_range(role));
         }

         auto associationTopic = dynamic_cast<SubModel *>(association->ElementInPackage);
         auto targetTopic = role->_baseclass == nullptr ? nullptr :
            dynamic_cast<SubModel *>(role->_baseclass->ElementInPackage);
         if (associationTopic != nullptr && targetTopic != nullptr &&
             !package_is_same_or_extension(associationTopic,targetTopic) && !role->External) {
            logger_.error(DiagnosticId::ReferenceExternalRequired,
               "cross-topic role " + role->Name + " requires EXTERNAL",
               diagnostic_range(role));
         }

         Multiplicity cardinality = effective_role_cardinality(role);
         if (role->Strongness == Role::Comp && (cardinality.Max < 0 || cardinality.Max > 1)) {
            logger_.error(DiagnosticId::RoleCompositionCardinality,
               "composition role " + role->Name +
                  " has maximum cardinality greater than 1",diagnostic_range(role));
         }

         if (auto base = dynamic_cast<Role *>(role->Super)) {
            Multiplicity baseCardinality = effective_role_cardinality(base);
            if (!cardinality_is_subset(cardinality,baseCardinality)) {
               logger_.error(DiagnosticId::RoleCardinalityExtension,
                  "cardinality of extended role " + role->Name +
                     " is not a subset of its base",diagnostic_range(role));
            }
            if (role->Strongness < base->Strongness) {
               logger_.error(DiagnosticId::RoleExtensionStrength,
                  "extended role " + role->Name + " weakens its aggregation strength",
                  diagnostic_range(role));
            }
            if (role->_baseclass != nullptr && base->_baseclass != nullptr &&
                !class_is_same_or_extension(role->_baseclass,base->_baseclass)) {
               logger_.error(DiagnosticId::RoleExtensionTarget,
                  "target of extended role " + role->Name +
                     " does not extend its base target",diagnostic_range(role));
            }
            if (base->Ordered && !role->Ordered && cardinality.Max != 1) {
               logger_.error(DiagnosticId::RoleExtensionOrdering,
                  "extended role " + role->Name + " removes ordering",
                  diagnostic_range(role));
            }
         }
      }

      if (aggregateRoles > 1) {
         logger_.error(DiagnosticId::AssociationMultipleAggregationRoles,
            "an association may have only one aggregation or composition role",
            diagnostic_range(association));
      }
      if (aggregateRoles == 1 && association->Role.size() > 2) {
         logger_.error(DiagnosticId::AssociationAggregationBinary,
            "aggregation and composition associations must be binary",
            diagnostic_range(association));
      }
   }
};

}

Multiplicity effectiveRoleCardinality(Role *role)
{
   return effective_role_cardinality(role);
}

Multiplicity attributeCardinality(Type *type)
{
   return attribute_cardinality(type);
}

void check_model_semantics(const MetaModelStore &store,Logger &logger)
{
   logger.setCategory("semantic");
   Checker checker(store,logger);
   for (Model *model : store.models()) {
      logger.setCurrentSource(model->_ilifile);
      checker.check_package(model);
   }
}

}
