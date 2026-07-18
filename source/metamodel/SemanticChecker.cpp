#include "SemanticChecker.h"

#include <algorithm>
#include <regex>
#include <set>
#include <unordered_set>
#include <vector>

#include "MetaModel.h"
#include "MetaModelInput.h"
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

bool validate_enum_literal(Expression *expression,const Descriptor &target,bool allowNodes)
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
      Log.error("enumeration value #" + constant->Value + " is not valid for the compared domain",expression->_line);
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

bool validate_formatted_literal(Expression *literal,const Descriptor &formatted)
{
   string value;
   if (!is_text_literal(literal,&value)) {
      return false;
   }
   auto type = dynamic_cast<FormattedType *>(formatted.DeclaredType);
   if (type == nullptr || !validate_standard_formatted_value(type,value)) {
      Log.error("formatted value \"" + value + "\" is outside the domain format or range",literal->_line);
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

bool validate_unique_enum_nodes(EnumNode *parent,const string &prefix)
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
         Log.error("duplicate enumeration element " + path,node->_line);
         valid = false;
      }
      if (!validate_unique_enum_nodes(node,path)) {
         valid = false;
      }
   }
   return valid;
}

void validate_enum_extension_nodes(EnumNode *parent,const string &prefix,
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
         Log.error("enumeration extension duplicates element " + path,node->_line);
      }
      validate_enum_extension_nodes(node,path,baseValues);
   }
}

int decimal_accuracy(const string &value)
{
   size_t exponent = value.find_first_of("eES");
   string mantissa = value.substr(0,exponent);
   size_t point = mantissa.find('.');
   return point == string::npos ? 0 : static_cast<int>(mantissa.size() - point - 1);
}

class Checker {
public:
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
            require(operand,Kind::Boolean,"logical expression required",expression->_line);
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
         Log.error("domain constraints are not supported for CLASS, STRUCTURE, or ATTRIBUTE domains",constraint->_line);
      }

      if (auto simple = dynamic_cast<SimpleConstraint *>(constraint)) {
         Descriptor value = evaluate(simple->LogicalExpression,domain);
         require(value,Kind::Boolean,"logical expression required",constraint->_line);
      }
      else if (auto existence = dynamic_cast<ExistenceConstraint *>(constraint)) {
         Descriptor required = evaluate(existence->Attr,domain);
         for (ExistenceDef *candidate : existence->ExistsIn) {
            Descriptor available = evaluate(candidate,domain);
            if (!equality_compatible(existence->Attr,required,candidate,available,false)) {
               Log.error("existence constraint paths have incompatible datatypes",constraint->_line);
            }
         }
      }
      else if (auto unique = dynamic_cast<UniqueConstraint *>(constraint)) {
         for (Expression *where : unique->Where) {
            require(evaluate(where,domain),Kind::Boolean,"logical expression required",where->_line);
         }
         for (Expression *path : unique->UniqueDef) {
            evaluate(path,domain);
         }
      }
      else if (auto set = dynamic_cast<SetConstraint *>(constraint)) {
         for (Expression *where : set->Where) {
            require(evaluate(where,domain),Kind::Boolean,"logical expression required",where->_line);
         }
         require(evaluate(set->Constraint,domain),Kind::Boolean,"logical expression required",constraint->_line);
      }
   }

   void check_package(Package *package)
   {
      if (package == nullptr || !checkedPackages.insert(package).second) {
         return;
      }
      for (MetaElement *element : package->Element) {
         if (auto child = dynamic_cast<Package *>(element)) {
            check_package(child);
         }
         if (auto viewable = dynamic_cast<Class *>(element)) {
            check_class(viewable);
         }
         else if (auto domain = dynamic_cast<DomainType *>(element)) {
            check_type(domain);
            for (Constraint *constraint : domain->Constraint) {
               check_constraint(constraint);
            }
         }
         if (auto graphic = dynamic_cast<Graphic *>(element)) {
            require(evaluate(graphic->Where),Kind::Boolean,"logical expression required",graphic->_line);
         }
         if (auto rule = dynamic_cast<DrawingRule *>(element)) {
            for (CondSignParamAssignment *assignment : rule->Rule) {
               require(evaluate(assignment->Where),Kind::Boolean,"logical expression required",rule->_line);
               for (SignParamAssignment *parameter : assignment->Assignments) {
                  evaluate(parameter->Assignment);
               }
            }
         }
      }
   }

private:
   unordered_set<Expression *> evaluating;
   unordered_set<Expression *> evaluated;
   unordered_set<Constraint *> checkedConstraints;
   unordered_set<Package *> checkedPackages;
   unordered_set<Class *> checkedClasses;
   unordered_set<Type *> checkedTypes;

   void require(const Descriptor &descriptor,Kind expected,const string &message,int line)
   {
      if (descriptor.Kind != Kind::Unknown && descriptor.Kind != expected) {
         Log.error(message,line);
      }
   }

   Descriptor path_type(PathOrInspFactor *path,DomainType *domain)
   {
      if (path == nullptr || path->PathEls.empty()) {
         return {};
      }
      PathEl *terminal = path->PathEls.back();
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
               require(type,Kind::Boolean,"logical expression required",compound->_line);
            }
            result.Kind = Kind::Boolean;
            break;
         case CompoundExpr_OperationType::Plus:
         case CompoundExpr_OperationType::Minus:
         case CompoundExpr_OperationType::Mult:
         case CompoundExpr_OperationType::Div:
            for (const Descriptor &type : types) {
               require(type,Kind::Numeric,"numeric expression required",compound->_line);
            }
            result.Kind = Kind::Numeric;
            break;
         case CompoundExpr_OperationType::Relation_Equal:
         case CompoundExpr_OperationType::Relation_NotEqual:
            if (operands.size() >= 2 && !equality_compatible(operands[0],types[0],operands[1],types[1],true)) {
               Log.error("incompatible datatypes",compound->_line);
            }
            result.Kind = Kind::Boolean;
            break;
         case CompoundExpr_OperationType::Relation_Less:
         case CompoundExpr_OperationType::Relation_LessOrEqual:
         case CompoundExpr_OperationType::Relation_Greater:
         case CompoundExpr_OperationType::Relation_GreaterOrEqual:
            if (operands.size() >= 2 && !ordering_compatible(operands[0],types[0],operands[1],types[1])) {
               Log.error("incompatible datatypes for ordering comparison",compound->_line);
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
                (!reportLiteral || validate_formatted_literal(rightExpression,left));
      }
      if (left.Kind == Kind::Text && right.Kind == Kind::Formatted) {
         return is_text_literal(leftExpression) &&
                (!reportLiteral || validate_formatted_literal(leftExpression,right));
      }
      if ((left.Kind == Kind::Enumeration || left.Kind == Kind::EnumTreeValue) && is_enum_literal(rightExpression)) {
         return validate_enum_literal(rightExpression,left,left.Kind == Kind::EnumTreeValue);
      }
      if (is_enum_literal(leftExpression) &&
          (right.Kind == Kind::Enumeration || right.Kind == Kind::EnumTreeValue)) {
         return validate_enum_literal(leftExpression,right,right.Kind == Kind::EnumTreeValue);
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
         return is_text_literal(rightExpression) && validate_formatted_literal(rightExpression,left);
      }
      if (left.Kind == Kind::Text && right.Kind == Kind::Formatted) {
         return is_text_literal(leftExpression) && validate_formatted_literal(leftExpression,right);
      }

      Descriptor enumType;
      if ((left.Kind == Kind::Enumeration || left.Kind == Kind::EnumTreeValue) && is_enum_literal(rightExpression)) {
         enumType = left;
         if (!validate_enum_literal(rightExpression,left,left.Kind == Kind::EnumTreeValue)) return false;
      }
      else if (is_enum_literal(leftExpression) &&
               (right.Kind == Kind::Enumeration || right.Kind == Kind::EnumTreeValue)) {
         enumType = right;
         if (!validate_enum_literal(leftExpression,right,right.Kind == Kind::EnumTreeValue)) return false;
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
               Log.error("incompatible function argument type for " + call->Function->Name,argument->_line);
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
      for (AttrOrParam *attribute : viewable->ClassAttribute) {
         check_attribute(attribute);
         for (Expression *derivation : attribute->Derivates) evaluate(derivation);
      }
      for (Role *role : viewable->Role) {
         for (Expression *derivation : role->Derivates) evaluate(derivation);
      }
      if (auto view = dynamic_cast<View *>(viewable)) {
         if (view->Where != nullptr) {
            require(evaluate(view->Where),Kind::Boolean,"logical expression required",view->Where->_line);
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
         Log.error("class or structure " + viewable->Name +
                   " must use EXTENDED when retaining the base name",viewable->_line);
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
               Log.error("class or structure " + viewable->Name +
                         " cannot be EXTENDED because " + other->Name +
                         " specializes the same base",viewable->_line);
               return;
            }
         }
      }
   }

   void check_type(Type *type)
   {
      if (type == nullptr || !checkedTypes.insert(type).second) {
         return;
      }
      if (auto enumeration = dynamic_cast<EnumType *>(type)) {
         validate_unique_enum_nodes(enumeration->TopNode,"");
         if (enumeration->ElementInPackage != nullptr) {
            if (auto base = dynamic_cast<EnumType *>(enumeration->Super)) {
               set<string> baseValues;
               collect_enum_values(base,baseValues);
               validate_enum_extension_nodes(enumeration->TopNode,"",baseValues);
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
               Log.error("line overlap must use the coordinate domain precision",line->_line);
            }
         }
      }
      if (auto multi = dynamic_cast<MultiValue *>(type)) {
         check_type(multi->BaseType);
      }
   }

   void check_attribute(AttrOrParam *attribute)
   {
      if (attribute == nullptr) {
         return;
      }
      check_type(attribute->Type);
      AttrOrParam *base = attribute->Extending;
      if (base == nullptr || base->Type == nullptr || attribute->Type == nullptr) {
         return;
      }

      Multiplicity baseCardinality = attribute_cardinality(base->Type);
      Multiplicity cardinality = attribute_cardinality(attribute->Type);
      if (!cardinality_is_subset(cardinality,baseCardinality)) {
         Log.error("cardinality of extended attribute " + attribute->Name +
                   " is not a subset of its base",attribute->_line);
      }
      if (attribute->Transient != base->Transient) {
         Log.error("TRANSIENT mode of extended attribute " + attribute->Name +
                   " differs from its base",attribute->_line);
      }

      Type *baseDeclared = canonical_declared_type(base->Type);
      Type *declared = canonical_declared_type(attribute->Type);
      if (baseDeclared != nullptr && declared != nullptr &&
          baseDeclared->ElementInPackage != nullptr && declared->ElementInPackage != nullptr &&
          !declared_type_is_same_or_extension(attribute->Type,base->Type)) {
         Log.error("domain of extended attribute " + attribute->Name +
                   " does not extend its base domain",attribute->_line);
      }

      if (base->Type->getClass() != attribute->Type->getClass()) {
         Log.error("type of extended attribute " + attribute->Name +
                   " is incompatible with its base",attribute->_line);
         return;
      }
      if (auto text = dynamic_cast<TextType *>(attribute->Type)) {
         auto baseText = static_cast<TextType *>(base->Type);
         if (text->Kind != baseText->Kind) {
            Log.error("TEXT and MTEXT kinds may not change in an attribute extension",attribute->_line);
         }
         if (baseText->MaxLength >= 0 &&
             (text->MaxLength < 0 || text->MaxLength > baseText->MaxLength)) {
            Log.error("text length of extended attribute exceeds its base",attribute->_line);
         }
      }
      if (attribute->TypeExplicitlyDefined) {
         if (auto enumeration = dynamic_cast<EnumType *>(attribute->Type)) {
            auto baseEnumeration = static_cast<EnumType *>(base->Type);
            if (declared->ElementInPackage == nullptr && baseDeclared->ElementInPackage == nullptr) {
               set<string> baseValues;
               collect_enum_values(baseEnumeration,baseValues);
               validate_enum_extension_nodes(enumeration->TopNode,"",baseValues);
            }
         }
      }
      if (auto multi = dynamic_cast<MultiValue *>(attribute->Type)) {
         auto baseMulti = static_cast<MultiValue *>(base->Type);
         if (baseMulti->Ordered && !multi->Ordered) {
            Log.error("an extended LIST attribute may not become an unordered BAG",attribute->_line);
         }
         auto target = dynamic_cast<Class *>(multi->BaseType);
         auto baseTarget = dynamic_cast<Class *>(baseMulti->BaseType);
         if (target != nullptr && baseTarget != nullptr &&
             !class_is_same_or_extension(target,baseTarget)) {
            Log.error("structure target of extended attribute does not extend its base",attribute->_line);
         }
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
            Log.error("duplicate role " + role->Name + " in association " + association->Name,role->_line);
         }
         if (role->Strongness != Role::Assoc) {
            ++aggregateRoles;
         }
         if (role->_baseclass != nullptr && role->_baseclass->Kind == Class::Structure) {
            Log.error("role " + role->Name + " may only reference a class or association",role->_line);
         }

         auto associationTopic = dynamic_cast<SubModel *>(association->ElementInPackage);
         auto targetTopic = role->_baseclass == nullptr ? nullptr :
            dynamic_cast<SubModel *>(role->_baseclass->ElementInPackage);
         if (associationTopic != nullptr && targetTopic != nullptr &&
             !package_is_same_or_extension(associationTopic,targetTopic) && !role->External) {
            Log.error("cross-topic role " + role->Name + " requires EXTERNAL",role->_line);
         }

         Multiplicity cardinality = effective_role_cardinality(role);
         if (role->Strongness == Role::Comp && (cardinality.Max < 0 || cardinality.Max > 1)) {
            Log.error("composition role " + role->Name + " has maximum cardinality greater than 1",role->_line);
         }

         if (auto base = dynamic_cast<Role *>(role->Super)) {
            Multiplicity baseCardinality = effective_role_cardinality(base);
            if (!cardinality_is_subset(cardinality,baseCardinality)) {
               Log.error("cardinality of extended role " + role->Name + " is not a subset of its base",role->_line);
            }
            if (role->Strongness < base->Strongness) {
               Log.error("extended role " + role->Name + " weakens its aggregation strength",role->_line);
            }
            if (role->_baseclass != nullptr && base->_baseclass != nullptr &&
                !class_is_same_or_extension(role->_baseclass,base->_baseclass)) {
               Log.error("target of extended role " + role->Name + " does not extend its base target",role->_line);
            }
            if (base->Ordered && !role->Ordered && cardinality.Max != 1) {
               Log.error("extended role " + role->Name + " removes ordering",role->_line);
            }
         }
      }

      if (aggregateRoles > 1) {
         Log.error("an association may have only one aggregation or composition role",association->_line);
      }
      if (aggregateRoles == 1 && association->Role.size() > 2) {
         Log.error("aggregation and composition associations must be binary",association->_line);
      }
   }
};

}

void check_model_semantics()
{
   Checker checker;
   for (Model *model : get_all_models()) {
      checker.check_package(model);
   }
}

}
