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
      for (Constraint *constraint : viewable->Constraints) check_constraint(constraint);
      for (Constraint *constraint : viewable->Constraint) check_constraint(constraint);
      for (AttrOrParam *attribute : viewable->ClassAttribute) {
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
