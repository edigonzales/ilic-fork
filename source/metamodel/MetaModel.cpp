#include "MetaModel.h"
#include "MetaModelStore.h"
#include "DiagnosticUtil.h"
#include "../util/Logger.h"

#include <unordered_map>
#include <stdexcept>
#include <vector>

using namespace util;

namespace metamodel {

   MetaElement *MetaElement::getTranslationOfRoot()
   {
      MetaElement *element = this;
      while (element->_translationOf != nullptr) {
         element = element->_translationOf;
      }
      return element;
   }

   // path utilities

   string get_type_path(Type* t)
   {

      string path;

      if ((t->Name == "C1" || t->Name == "C2" || t->Name == "C3") && t->ElementInPackage == nullptr) {
         path = get_path(t->_other_type) + "." + t->Name;
      }
      else if (t->ElementInPackage != nullptr) {
         // class / structure
         path = get_path(t->ElementInPackage) + "." + t->Name;
      }
      else if (t->LTParent != nullptr) {
         // class attribute
         path = get_path(t->LTParent) + "." + t->Name;
      }
      else if (t->LFTParent != nullptr) {
         // function argument, result
         path = get_path(t->LFTParent) + "." + t->Name;
         if (t->Name != "TYPE") {
            path += ".TYPE";
         }
      }
      else {
         // local attribute type
         if (t->Name == "TYPE") {
            path = get_path(t->_attr) + ".TYPE";
         }
         else {
            path = get_path(t->Super);
         }
      }

      return path;

   }
      
   string get_path(MMObject* o)
   {

      if (o == nullptr) {
         return "nullptr";
      }
      else if (!dynamic_cast<MetaElement*>(o)) {
         return "unknown";
      }
      
      MetaElement* e = dynamic_cast<MetaElement*>(o);
      
      if (e->Name == "ANYCLASS") {
         return "ANYCLASS";
      }
      if (e->Name == "ANYSTRUCTURE") {
         return "ANYSTRUCTURE";
      }
      
      if (e->getClass() == "EnumNode") {
         EnumNode *n = dynamic_cast<EnumNode *>(e);
         if (n->EnumType != nullptr) {
            return get_path(n->EnumType) + "." + n->Name;
         }
         else {
            return get_path(n->ParentNode) + "." + n->Name;
         }
      }
      else if (e->getClass() == "AttrOrParam") {
         AttrOrParam *a = dynamic_cast<AttrOrParam *>(e);
         return get_path(a->AttrParent) + "." + a->Name;
      }
      else if (e->getClass() == "Argument") {
         Argument *a = dynamic_cast<Argument *>(e);
         return get_path(a->Function) + "." + a->Name;
      }
      else if (e->isSubClassOf("Type")) {
         Type *t = dynamic_cast<Type *>(e);
         return get_type_path(t);
      }

      string path = "";
      while (e != nullptr) {
         if (path == "") {
            path = e->Name;
         }
         else {
            path = e->Name + "." + path;
         }
         e = e->ElementInPackage;
      }

      return path;

   }

   string get_parent_path(MetaElement* e)
   {
      if (e == nullptr) {
         return "nullptr";
      }
      return get_path(e->ElementInPackage);
   }

   bool MMObject::isSubClassOf(string classname)
   {
      if (getClass() == classname || getBaseClass() == classname ||
          classname == "MMObject") return true;
      if (classname == "MetaElement") return dynamic_cast<MetaElement *>(this) != nullptr;
      if (classname == "ExtendableME") return dynamic_cast<ExtendableME *>(this) != nullptr;
      if (classname == "Package") return dynamic_cast<Package *>(this) != nullptr;
      if (classname == "Model") return dynamic_cast<Model *>(this) != nullptr;
      if (classname == "SubModel") return dynamic_cast<SubModel *>(this) != nullptr;
      if (classname == "Type") return dynamic_cast<Type *>(this) != nullptr;
      if (classname == "DomainType") return dynamic_cast<DomainType *>(this) != nullptr;
      if (classname == "Class") return dynamic_cast<Class *>(this) != nullptr;
      if (classname == "AttrOrParam") return dynamic_cast<AttrOrParam *>(this) != nullptr;
      if (classname == "Expression") return dynamic_cast<Expression *>(this) != nullptr;
      if (classname == "Factor") return dynamic_cast<Factor *>(this) != nullptr;
      if (classname == "Constraint") return dynamic_cast<Constraint *>(this) != nullptr;
      return false;
   }
   
   // clone() method implementation

   static void clone_init_mmobject(MetaModelStore &destination,MMObject *clone,MMObject *org)
   {
      clone->_line = org->_line;
      clone->_source = org->_source;
      clone->_selectionSource = org->_selectionSource;
      clone->_endSelectionSource = org->_endSelectionSource;
      clone->_referenceSources = org->_referenceSources;
   }

   static void clone_init_doctext(MetaModelStore &destination,DocText *clone,DocText *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Name = org->Name;
      clone->Text = org->Text;

   }

   static void clone_init_metaattribute(MetaModelStore &destination,MetaAttribute *clone,MetaAttribute *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Name = org->Name;
      clone->Value = org->Value;
      clone->MetaElement = org->MetaElement;

   }

   static void clone_init_metaelement(MetaModelStore &destination,MetaElement *clone,MetaElement *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Name = org->Name;
      clone->Documentation = org->Documentation;
      clone->ElementInPackage = org->ElementInPackage;

   }

   static void clone_init_extendableme(MetaModelStore &destination,ExtendableME *clone,ExtendableME *org)
   {

      clone_init_metaelement(destination,clone,org);

      clone->Abstract = org->Abstract;
      clone->Generic = org->Generic;
      clone->Final = org->Final;
      clone->Super = org->Super;

   }

   static void clone_init_package(MetaModelStore &destination,Package *clone,Package *org)
   {

      clone_init_metaelement(destination,clone,org);
      clone->_super = org->_super;

   }

   static void clone_init_import(MetaModelStore &destination,Import *clone,Import *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->ImportingP = org->ImportingP;
      clone->ImportedP = org->ImportedP;
      clone->_unqualified = org->_unqualified;

   }

   static void clone_init_ili1format(MetaModelStore &destination,Ili1Format *clone,Ili1Format *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->LineSize = org->LineSize;
      clone->tidSize = org->tidSize;
      clone->blankCode = org->blankCode;
      clone->undefinedCode = org->undefinedCode;
      clone->continueCode = org->continueCode;
      clone->Font = org->Font;
      clone->tidKind = org->tidKind;
      clone->tidExplanation = org->tidExplanation;

   }

   static void clone_init_model(MetaModelStore &destination,Model *clone,Model *org)
   {

      clone_init_package(destination,clone,org);

      clone->iliVersion = org->iliVersion;
      clone->Contracted = org->Contracted;
      clone->Kind = org->Kind;
      clone->Language = org->Language;
      clone->At = org->At;
      clone->Version = org->Version;
      clone->VersionExplanation = org->VersionExplanation;
      clone->NoIncrementalTransfer = org->NoIncrementalTransfer;
      clone->CharSetIANAName = org->CharSetIANAName;
      clone->xmlns = org->xmlns;
      clone->ili1Transfername = org->ili1Transfername;
      clone->ili1Format = org->ili1Format;

   }

   static void clone_init_submodel(MetaModelStore &destination,SubModel *clone,SubModel *org)
   {
      clone_init_package(destination,clone,org);
      clone->_dataunit = org->_dataunit;
      clone->DeferredGenerics = org->DeferredGenerics;
   }

   static void clone_init_type(MetaModelStore &destination,Type *clone,Type *org)
   {

      clone_init_extendableme(destination,clone,org);

      clone->LTParent = org->LTParent;
      clone->LFTParent = org->LFTParent;
      clone->_other_type = org->_other_type;
      clone->_attr = org->_attr;

   }

   static void clone_init_multiplicity(MetaModelStore &destination,Multiplicity *clone,Multiplicity *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Min = org->Min;
      clone->Max = org->Max;

   }

   static void clone_init_constraint(MetaModelStore &destination,Constraint *clone,Constraint *org)
   {

      clone_init_metaelement(destination,clone,org);
      clone->toDomain = org->toDomain;
      clone->ToClass = org->ToClass;

   }

   static void clone_init_domaintype(MetaModelStore &destination,DomainType *clone,DomainType *org)
   {

      clone_init_type(destination,clone,org);

      clone->Mandatory = org->Mandatory;
      clone->GenericDef = org->GenericDef;

   }

   static void clone_init_class(MetaModelStore &destination,Class *clone,Class *org)
   {

      clone_init_type(destination,clone,org);

      clone->Kind = org->Kind;
      clone->Multiplicity = org->Multiplicity;
      clone->EmbeddedRoleTransfer = org->EmbeddedRoleTransfer;
      clone->ili1OptionalTable = org->ili1OptionalTable;
      clone->Oid = org->Oid;
      clone->View = org->View;
      clone->isDomainType = org->isDomainType;

   }

   static void clone_init_attrorparam(MetaModelStore &destination,AttrOrParam *clone,AttrOrParam *org)
   {

      clone_init_extendableme(destination,clone,org);

      clone->SubdivisionKind = org->SubdivisionKind;
      clone->Transient = org->Transient;
      clone->Derivates = org->Derivates;
      clone->AttrParent = org->AttrParent;
      clone->Type = org->Type;
      clone->TypeExplicitlyDefined = org->TypeExplicitlyDefined;

   }

   static void clone_init_typerelatedtype(MetaModelStore &destination,TypeRelatedType *clone,TypeRelatedType *org)
   {

      clone_init_domaintype(destination,clone,org);

      clone->BaseType = org->BaseType;

   }

   static void clone_init_multivalue(MetaModelStore &destination,MultiValue *clone,MultiValue *org)
   {

      clone_init_typerelatedtype(destination,clone,org);

      clone->Ordered = org->Ordered;
      clone->Multiplicity = org->Multiplicity;

   }

   static void clone_init_classrelatedtype(MetaModelStore &destination,ClassRelatedType *clone,ClassRelatedType *org)
   {

      clone_init_domaintype(destination,clone,org);

      clone->_baseclass = org->_baseclass;
      clone->_classrestriction = org->_classrestriction;

   }

   static void clone_init_referencetype(MetaModelStore &destination,ReferenceType *clone,ReferenceType *org)
   {

      clone_init_classrelatedtype(destination,clone,org);

      clone->External = org->External;

   }

   static void clone_init_role(MetaModelStore &destination,Role *clone,Role *org)
   {

      clone_init_referencetype(destination,clone,org);

      clone->Strongness = org->Strongness;
      clone->Ordered = org->Ordered;
      clone->Multiplicity = org->Multiplicity;
      clone->MultiplicityDefined = org->MultiplicityDefined;
      clone->Derivates = org->Derivates;
      clone->EmbeddedTransfer = org->EmbeddedTransfer;
      clone->Association = org->Association;

   }

   static void clone_init_explicitassocaccess(MetaModelStore &destination,ExplicitAssocAccess *clone,ExplicitAssocAccess *org)
   {

      clone_init_extendableme(destination,clone,org);

      clone->AssocAccOf = org->AssocAccOf;
      clone->OriginRole = org->OriginRole;
      clone->TargetRole = org->TargetRole;

   }

   static void clone_init_assocacc(MetaModelStore &destination,AssocAcc *clone,AssocAcc *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Class = org->Class;
      clone->AssocAcc_ = org->AssocAcc_;

   }

   static void clone_init_transferelement(MetaModelStore &destination,TransferElement *clone,TransferElement *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->TransferClass = org->TransferClass;
      clone->TransferElement_ = org->TransferElement_;

   }

   static void clone_init_ili1transferelement(MetaModelStore &destination,Ili1TransferElement *clone,Ili1TransferElement *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Ili1TransferClass = org->Ili1TransferClass;
      clone->Ili1RefAttr = org->Ili1RefAttr;

   }

   static void clone_init_dataunit(MetaModelStore &destination,DataUnit *clone,DataUnit *org)
   {

      clone_init_extendableme(destination,clone,org);

      clone->ViewUnit = org->ViewUnit;
      clone->DataUnitName = org->DataUnitName;
      clone->Oid = org->Oid;

   }

   static void clone_init_dependency(MetaModelStore &destination,Dependency *clone,Dependency *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Using = org->Using;
      clone->Dependent = org->Dependent;

   }

   static void clone_init_allowedinbasket(MetaModelStore &destination,AllowedInBasket *clone,AllowedInBasket *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->OfDataUnit = org->OfDataUnit;
      clone->ClassInBasket = org->ClassInBasket;

   }

   static void clone_init_context(MetaModelStore &destination,Context *clone,Context *org)
   {

      clone_init_metaelement(destination,clone,org);
      clone->GenericDefinitions = org->GenericDefinitions;

   }

   static void clone_init_genericdef(MetaModelStore &destination,GenericDef *clone,GenericDef *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->OID = org->OID;
      clone->Context = org->Context;
      clone->GenericDomain = org->GenericDomain;
      clone->ConcreteDomain = org->ConcreteDomain;

   }

   static void clone_init_unit(MetaModelStore &destination,Unit *clone,Unit *org)
   {

      clone_init_extendableme(destination,clone,org);

      clone->Kind = org->Kind;
      clone->Definition = org->Definition;
      clone->_unitname = org->_unitname;

   }

   static void clone_init_metabasketdef(MetaModelStore &destination,MetaBasketDef *clone,MetaBasketDef *org)
   {

      clone_init_extendableme(destination,clone,org);

      clone->Kind = org->Kind;
      clone->MetaDataTopic = org->MetaDataTopic;
      clone->Member = org->Member;

   }

   static void clone_init_metaobjectdef(MetaModelStore &destination,MetaObjectDef *clone,MetaObjectDef *org)
   {

      clone_init_metaelement(destination,clone,org);

      clone->IsRefSystem = org->IsRefSystem;
      clone->Class = org->Class;

   }

   static void clone_init_booleantype(MetaModelStore &destination,BooleanType *clone,BooleanType *org)
   {

      clone_init_domaintype(destination,clone,org);


   }

   static void clone_init_texttype(MetaModelStore &destination,TextType *clone,TextType *org)
   {

      clone_init_domaintype(destination,clone,org);

      clone->Kind = org->Kind;
      clone->MaxLength = org->MaxLength;
      clone->OIDType = org->OIDType;
   }

   static void clone_init_blackboxtype(MetaModelStore &destination,BlackboxType *clone,BlackboxType *org)
   {

      clone_init_domaintype(destination,clone,org);

      clone->Kind = org->Kind;

   }

   static void clone_init_numtype(MetaModelStore &destination,NumType *clone,NumType *org)
   {

      clone_init_domaintype(destination,clone,org);

      clone->Min = org->Min;
      clone->Max = org->Max;
      clone->Circular = org->Circular;
      clone->Clockwise = org->Clockwise;
      clone->Unit = org->Unit;
      clone->OIDType = org->OIDType;

   }

   static void clone_init_coordtype(MetaModelStore &destination,CoordType *clone,CoordType *org)
   {

      clone_init_domaintype(destination,clone,org);

      clone->NullAxis = org->NullAxis;
      clone->PiHalfAxis = org->PiHalfAxis;
      clone->Multi = org->Multi;

   }

   static void clone_init_axisspec(MetaModelStore &destination,AxisSpec *clone,AxisSpec *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->CoordType = org->CoordType;
      clone->Axis = org->Axis;

   }

   static void clone_init_numsrefsys(MetaModelStore &destination,NumsRefSys *clone,NumsRefSys *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->NumType = org->NumType;
      clone->RefSys = org->RefSys;
      clone->AxisInd = org->AxisInd;

   }

   static void clone_init_formattedtype(MetaModelStore &destination,FormattedType *clone,FormattedType *org)
   {

      clone_init_numtype(destination,clone,org);

      clone->Format = org->Format;
      clone->Struct = org->Struct;
      clone->BaseFormattedType = org->BaseFormattedType;
   }

   static void clone_init_anyoidtype(MetaModelStore &destination,AnyOIDType *clone,AnyOIDType *org)
   {
      clone_init_domaintype(destination,clone,org);
   }

   static void clone_init_functiondef(MetaModelStore &destination,FunctionDef *clone,FunctionDef *org)
   {

      clone_init_metaelement(destination,clone,org);

      clone->Explanation = org->Explanation;
      clone->LocalType = org->LocalType;
      clone->ResultType = org->ResultType;

   }

   static void clone_init_argument(MetaModelStore &destination,Argument *clone,Argument *org)
   {

      clone_init_metaelement(destination,clone,org);

      clone->Kind = org->Kind;
      clone->Function = org->Function;
      clone->Type = org->Type;

   }

   static void clone_init_classreftype(MetaModelStore &destination,ClassRefType *clone,ClassRefType *org)
   {
      clone_init_classrelatedtype(destination,clone,org);
   }

   static void clone_init_objecttype(MetaModelStore &destination,ObjectType *clone,ObjectType *org)
   {
      clone_init_classrelatedtype(destination,clone,org);
      clone->Multiple = org->Multiple;
   }

   static void clone_init_attributereftype(MetaModelStore &destination,AttributeRefType *clone,AttributeRefType *org)
   {

      clone_init_domaintype(destination,clone,org);

      clone->Of = org->Of;
      clone->AttrRestriction = org->AttrRestriction;

   }

   static void clone_init_arefrestriction(MetaModelStore &destination,ARefRestriction *clone,ARefRestriction *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->ARef = org->ARef;
      clone->Type = org->Type;

   }

   static void clone_init_enumnode(MetaModelStore &destination,EnumNode* clone, EnumNode* org)
   {

      clone_init_extendableme(destination,clone, org);

      clone->EnumType = org->EnumType;
      clone->ParentNode = org->ParentNode;

      for (auto* orgNode : org->Node) {
         MMObject* cloneNode = nullptr;
         cloneNode = destination.make<EnumNode>();
         EnumNode* c = static_cast<EnumNode*>(cloneNode);
         clone_init_enumnode(destination,c, orgNode);
         clone->Node.push_back(c);
      }
   }

   static void clone_init_enumtreevaluetype(MetaModelStore &destination,EnumTreeValueType* clone, EnumTreeValueType* org)
   {

      clone_init_domaintype(destination,clone, org);

      clone->ET = org->ET;

   }

   static void clone_init_enumtype(MetaModelStore &destination,EnumType *clone,EnumType *org)
   {

      clone_init_domaintype(destination,clone,org);

      clone->Order = org->Order;

      clone->TopNode = destination.make<EnumNode>();
      clone_init_enumnode(destination,clone->TopNode, org->TopNode);

      for (auto* orgNode : org->ETVT) {
         MMObject* cloneNode = nullptr;
         cloneNode = destination.make<EnumTreeValueType>();
         EnumTreeValueType* c = static_cast<EnumTreeValueType*>(cloneNode);

         clone_init_enumtreevaluetype(destination,c, orgNode);
         clone->ETVT.push_back(c);
      }

   }

   static void clone_init_lineform(MetaModelStore &destination,LineForm *clone,LineForm *org)
   {

      clone_init_metaelement(destination,clone,org);

      clone->Structure = org->Structure;

   }

   static void clone_init_linesform(MetaModelStore &destination,LinesForm* clone, LinesForm* org)
   {

      clone_init_mmobject(destination,clone, org);

      clone->LineType = org->LineType;
      clone->LineForm = org->LineForm;

   }

   static void clone_init_linetype(MetaModelStore &destination,LineType *clone,LineType *org)
   {

      clone_init_domaintype(destination,clone,org);

      clone->Kind = org->Kind;
      clone->MaxOverlap = org->MaxOverlap;
      clone->Multi = org->Multi;
      clone->CoordType = org->CoordType;
      clone->LAStructure = org->LAStructure;

      for (auto* orgNode : org->LineForm) {
         MMObject* cloneNode = nullptr;
         cloneNode = destination.make<LineForm>();
         LineForm* c = static_cast<LineForm*>(cloneNode);
         clone_init_lineform(destination,c, orgNode);
         clone->LineForm.push_back(c);
      }

   }

   static void clone_init_view(MetaModelStore &destination,View *clone,View *org)
   {

      clone_init_class(destination,clone,org);

      clone->FormationKind = org->FormationKind;
      clone->FormationParameter = org->FormationParameter;
      clone->Where = org->Where;
      clone->Transient = org->Transient;

   }

   static void clone_init_renamedbaseview(MetaModelStore &destination,RenamedBaseView *clone,RenamedBaseView *org)
   {

      clone_init_extendableme(destination,clone,org);

      clone->OrNull = org->OrNull;
      clone->View = org->View;
      clone->BaseView = org->BaseView;

   }

   static void clone_init_expression(MetaModelStore &destination,Expression *clone,Expression *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->_type = org->_type;
      clone->ResolvedType = org->ResolvedType;
      clone->OccurrenceScope = org->OccurrenceScope;
      clone->OccurrencePackage = org->OccurrencePackage;

   }

   static void clone_init_unaryexpr(MetaModelStore &destination,UnaryExpr *clone,UnaryExpr *org)
   {

      clone_init_expression(destination,clone,org);

      clone->Operation = org->Operation;
      clone->SubExpression = org->SubExpression;

   }

   static void clone_init_compoundexpr(MetaModelStore &destination,CompoundExpr *clone,CompoundExpr *org)
   {

      clone_init_expression(destination,clone,org);

      clone->Operation = org->Operation;
      clone->SubExpressions = org->SubExpressions;

   }

   static void clone_init_factor(MetaModelStore &destination,Factor *clone,Factor *org)
   {

      clone_init_expression(destination,clone,org);


   }

   static void clone_init_pathel(MetaModelStore &destination,PathEl *clone,PathEl *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Kind = org->Kind;
      clone->Ref = org->Ref;
      clone->NumIndex = org->NumIndex;
      clone->SpecIndex = org->SpecIndex;

   }

   static void clone_init_pathorinspfactor(MetaModelStore &destination,PathOrInspFactor *clone,PathOrInspFactor *org)
   {

      clone_init_factor(destination,clone,org);

      clone->PathEls = org->PathEls;
      clone->Inspection = org->Inspection;
      clone->_path = org->_path;

   }

   static void clone_init_enumassignment(MetaModelStore &destination,EnumAssignment *clone,EnumAssignment *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->ValueToAssign = org->ValueToAssign;
      clone->MinEnumValue = org->MinEnumValue;
      clone->MaxEnumValue = org->MaxEnumValue;

   }

   static void clone_init_enummapping(MetaModelStore &destination,EnumMapping *clone,EnumMapping *org)
   {

      clone_init_factor(destination,clone,org);

      clone->EnumValue = org->EnumValue;
      clone->Cases = org->Cases;

   }

   static void clone_init_classref(MetaModelStore &destination,ClassRef *clone,ClassRef *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Ref = org->Ref;

   }

   static void clone_init_actualargument(MetaModelStore &destination,ActualArgument *clone,ActualArgument *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->FormalArgument = org->FormalArgument;
      clone->Kind = org->Kind;
      clone->Expression = org->Expression;

   }

   static void clone_init_functioncall(MetaModelStore &destination,FunctionCall *clone,FunctionCall *org)
   {

      clone_init_factor(destination,clone,org);

      clone->Function = org->Function;
      clone->Arguments = org->Arguments;

   }

   static void clone_init_runtimeparamref(MetaModelStore &destination,RuntimeParamRef *clone,RuntimeParamRef *org)
   {

      clone_init_factor(destination,clone,org);

      clone->RuntimeParam = org->RuntimeParam;

   }

   static void clone_init_constant(MetaModelStore &destination,Constant *clone,Constant *org)
   {

      clone_init_factor(destination,clone,org);

      clone->Value = org->Value;
      clone->Kind = org->Kind;

   }

   static void clone_init_classconst(MetaModelStore &destination,ClassConst *clone,ClassConst *org)
   {

      clone_init_factor(destination,clone,org);

      clone->Class = org->Class;

   }

   static void clone_init_attributeconst(MetaModelStore &destination,AttributeConst *clone,AttributeConst *org)
   {

      clone_init_factor(destination,clone,org);

      clone->Attribute = org->Attribute;

   }

   static void clone_init_unitref(MetaModelStore &destination,UnitRef *clone,UnitRef *org)
   {

      clone_init_factor(destination,clone,org);

      clone->Unit = org->Unit;

   }

   static void clone_init_unitfunction(MetaModelStore &destination,UnitFunction *clone,UnitFunction *org)
   {

      clone_init_factor(destination,clone,org);

      clone->Explanation = org->Explanation;

   }

   static void clone_init_simpleconstraint(MetaModelStore &destination,SimpleConstraint *clone,SimpleConstraint *org)
   {

      clone_init_constraint(destination,clone,org);

      clone->Kind = org->Kind;
      clone->Percentage = org->Percentage;
      clone->LogicalExpression = org->LogicalExpression;
      clone->_percentage_operation = org->_percentage_operation;

   }

   static void clone_init_existencedef(MetaModelStore &destination,ExistenceDef *clone,ExistenceDef *org)
   {

      clone_init_pathorinspfactor(destination,clone,org);

      clone->Viewable = org->Viewable;

   }

   static void clone_init_existenceconstraint(MetaModelStore &destination,ExistenceConstraint *clone,ExistenceConstraint *org)
   {

      clone_init_constraint(destination,clone,org);

      clone->Attr = org->Attr;

   }

   static void clone_init_uniqueconstraint(MetaModelStore &destination,UniqueConstraint *clone,UniqueConstraint *org)
   {

      clone_init_constraint(destination,clone,org);

      clone->Kind = org->Kind;
      clone->PerBasket = org->PerBasket;
      clone->Where = org->Where;
      clone->UniqueDef = org->UniqueDef;

   }

   static void clone_init_setconstraint(MetaModelStore &destination,SetConstraint *clone,SetConstraint *org)
   {

      clone_init_constraint(destination,clone,org);

      clone->Where = org->Where;
      clone->PerBasket = org->PerBasket;
      clone->Constraint = org->Constraint;

   }

   static void clone_init_graphic(MetaModelStore &destination,Graphic *clone,Graphic *org)
   {

      clone_init_extendableme(destination,clone,org);

      clone->Where = org->Where;
      clone->Base = org->Base;

   }

   static void clone_init_signparamassignment(MetaModelStore &destination,SignParamAssignment *clone,SignParamAssignment *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Param = org->Param;
      clone->Assignment = org->Assignment;

   }

   static void clone_init_condsignparamassignment(MetaModelStore &destination,CondSignParamAssignment *clone,CondSignParamAssignment *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Where = org->Where;
      clone->Assignments = org->Assignments;

   }

   static void clone_init_drawingrule(MetaModelStore &destination,DrawingRule *clone,DrawingRule *org)
   {

      clone_init_extendableme(destination,clone,org);

      clone->Rule = org->Rule;
      clone->Graphic = org->Graphic;
      clone->Class = org->Class;

   }

   static void clone_init_doctexttranslation(MetaModelStore &destination,DocTextTranslation *clone,DocTextTranslation *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Text = org->Text;

   }

   static void clone_init_metranslation(MetaModelStore &destination,METranslation *clone,METranslation *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Of = org->Of;
      clone->TranslatedName = org->TranslatedName;
      clone->TranslatedDoc = org->TranslatedDoc;

   }

   static void clone_init_translation(MetaModelStore &destination,Translation *clone,Translation *org)
   {

      clone_init_mmobject(destination,clone,org);

      clone->Language = org->Language;

   }

   MMObject* MMObject::clone(MetaModelStore &destination) const
   {

      MMObject *source = const_cast<MMObject *>(this);
      MMObject *clone = nullptr;
      string classname = source->getClass();

      if (classname == "MMObject") {
         clone = destination.make<MMObject>();
         MMObject* c = static_cast<MMObject*>(clone);
         MMObject* o = static_cast<MMObject*>(source);
         clone_init_mmobject(destination,c,o);
      }
      else if (classname == "DocText") {
         clone = destination.make<DocText>();
         DocText* c = static_cast<DocText*>(clone);
         DocText* o = static_cast<DocText*>(source);
         clone_init_doctext(destination,c,o);
      }
      else if (classname == "MetaAttribute") {
         clone = destination.make<MetaAttribute>();
         MetaAttribute* c = static_cast<MetaAttribute*>(clone);
         MetaAttribute* o = static_cast<MetaAttribute*>(source);
         clone_init_metaattribute(destination,c,o);
      }
      else if (classname == "MetaElement") {
         clone = destination.make<MetaElement>();
         MetaElement* c = static_cast<MetaElement*>(clone);
         MetaElement* o = static_cast<MetaElement*>(source);
         clone_init_metaelement(destination,c,o);
      }
      else if (classname == "ExtendableME") {
         clone = destination.make<ExtendableME>();
         ExtendableME* c = static_cast<ExtendableME*>(clone);
         ExtendableME* o = static_cast<ExtendableME*>(source);
         clone_init_extendableme(destination,c,o);
      }
      else if (classname == "Package") {
         clone = destination.make<Package>();
         Package* c = static_cast<Package*>(clone);
         Package* o = static_cast<Package*>(source);
         clone_init_package(destination,c,o);
      }
      else if (classname == "Import") {
         clone = destination.make<Import>();
         Import* c = static_cast<Import*>(clone);
         Import* o = static_cast<Import*>(source);
         clone_init_import(destination,c,o);
      }
      else if (classname == "Ili1Format") {
         clone = destination.make<Ili1Format>();
         Ili1Format* c = static_cast<Ili1Format*>(clone);
         Ili1Format* o = static_cast<Ili1Format*>(source);
         clone_init_ili1format(destination,c,o);
      }
      else if (classname == "Model") {
         clone = destination.make<Model>();
         Model* c = static_cast<Model*>(clone);
         Model* o = static_cast<Model*>(source);
         clone_init_model(destination,c,o);
      }
      else if (classname == "SubModel") {
         clone = destination.make<SubModel>();
         SubModel* c = static_cast<SubModel*>(clone);
         SubModel* o = static_cast<SubModel*>(source);
         clone_init_submodel(destination,c,o);
      }
      else if (classname == "Type") {
         clone = destination.make<Type>();
         Type* c = static_cast<Type*>(clone);
         Type* o = static_cast<Type*>(source);
         clone_init_type(destination,c,o);
      }
      else if (classname == "Multiplicity") {
         clone = destination.make<Multiplicity>();
         Multiplicity* c = static_cast<Multiplicity*>(clone);
         Multiplicity* o = static_cast<Multiplicity*>(source);
         clone_init_multiplicity(destination,c,o);
      }
      else if (classname == "Constraint") {
         clone = destination.make<Constraint>();
         Constraint* c = static_cast<Constraint*>(clone);
         Constraint* o = static_cast<Constraint*>(source);
         clone_init_constraint(destination,c,o);
      }
      else if (classname == "DomainType") {
         clone = destination.make<DomainType>();
         DomainType* c = static_cast<DomainType*>(clone);
         DomainType* o = static_cast<DomainType*>(source);
         clone_init_domaintype(destination,c,o);
      }
      else if (classname == "Class") {
         clone = destination.make<Class>();
         Class* c = static_cast<Class*>(clone);
         Class* o = static_cast<Class*>(source);
         clone_init_class(destination,c,o);
      }
      else if (classname == "AttrOrParam") {
         clone = destination.make<AttrOrParam>();
         AttrOrParam* c = static_cast<AttrOrParam*>(clone);
         AttrOrParam* o = static_cast<AttrOrParam*>(source);
         clone_init_attrorparam(destination,c,o);
      }
      else if (classname == "TypeRelatedType") {
         clone = destination.make<TypeRelatedType>();
         TypeRelatedType* c = static_cast<TypeRelatedType*>(clone);
         TypeRelatedType* o = static_cast<TypeRelatedType*>(source);
         clone_init_typerelatedtype(destination,c,o);
      }
      else if (classname == "MultiValue") {
         clone = destination.make<MultiValue>();
         MultiValue* c = static_cast<MultiValue*>(clone);
         MultiValue* o = static_cast<MultiValue*>(source);
         clone_init_multivalue(destination,c,o);
      }
      else if (classname == "ClassRelatedType") {
         clone = destination.make<ClassRelatedType>();
         ClassRelatedType* c = static_cast<ClassRelatedType*>(clone);
         ClassRelatedType* o = static_cast<ClassRelatedType*>(source);
         clone_init_classrelatedtype(destination,c,o);
      }
      else if (classname == "ReferenceType") {
         clone = destination.make<ReferenceType>();
         ReferenceType* c = static_cast<ReferenceType*>(clone);
         ReferenceType* o = static_cast<ReferenceType*>(source);
         clone_init_referencetype(destination,c,o);
      }
      else if (classname == "Role") {
         clone = destination.make<Role>();
         Role* c = static_cast<Role*>(clone);
         Role* o = static_cast<Role*>(source);
         clone_init_role(destination,c,o);
      }
      else if (classname == "ExplicitAssocAccess") {
         clone = destination.make<ExplicitAssocAccess>();
         ExplicitAssocAccess* c = static_cast<ExplicitAssocAccess*>(clone);
         ExplicitAssocAccess* o = static_cast<ExplicitAssocAccess*>(source);
         clone_init_explicitassocaccess(destination,c,o);
      }
      else if (classname == "AssocAcc") {
         clone = destination.make<AssocAcc>();
         AssocAcc* c = static_cast<AssocAcc*>(clone);
         AssocAcc* o = static_cast<AssocAcc*>(source);
         clone_init_assocacc(destination,c,o);
      }
      else if (classname == "TransferElement") {
         clone = destination.make<TransferElement>();
         TransferElement* c = static_cast<TransferElement*>(clone);
         TransferElement* o = static_cast<TransferElement*>(source);
         clone_init_transferelement(destination,c,o);
      }
      else if (classname == "Ili1TransferElement") {
         clone = destination.make<Ili1TransferElement>();
         Ili1TransferElement* c = static_cast<Ili1TransferElement*>(clone);
         Ili1TransferElement* o = static_cast<Ili1TransferElement*>(source);
         clone_init_ili1transferelement(destination,c,o);
      }
      else if (classname == "DataUnit") {
         clone = destination.make<DataUnit>();
         DataUnit* c = static_cast<DataUnit*>(clone);
         DataUnit* o = static_cast<DataUnit*>(source);
         clone_init_dataunit(destination,c,o);
      }
      else if (classname == "Dependency") {
         clone = destination.make<Dependency>();
         Dependency* c = static_cast<Dependency*>(clone);
         Dependency* o = static_cast<Dependency*>(source);
         clone_init_dependency(destination,c,o);
      }
      else if (classname == "AllowedInBasket") {
         clone = destination.make<AllowedInBasket>();
         AllowedInBasket* c = static_cast<AllowedInBasket*>(clone);
         AllowedInBasket* o = static_cast<AllowedInBasket*>(source);
         clone_init_allowedinbasket(destination,c,o);
      }
      else if (classname == "Context") {
         clone = destination.make<Context>();
         Context* c = static_cast<Context*>(clone);
         Context* o = static_cast<Context*>(source);
         clone_init_context(destination,c,o);
      }
      else if (classname == "GenericDef") {
         clone = destination.make<GenericDef>();
         GenericDef* c = static_cast<GenericDef*>(clone);
         GenericDef* o = static_cast<GenericDef*>(source);
         clone_init_genericdef(destination,c,o);
      }
      else if (classname == "Unit") {
         clone = destination.make<Unit>();
         Unit* c = static_cast<Unit*>(clone);
         Unit* o = static_cast<Unit*>(source);
         clone_init_unit(destination,c,o);
      }
      else if (classname == "MetaBasketDef") {
         clone = destination.make<MetaBasketDef>();
         MetaBasketDef* c = static_cast<MetaBasketDef*>(clone);
         MetaBasketDef* o = static_cast<MetaBasketDef*>(source);
         clone_init_metabasketdef(destination,c,o);
      }
      else if (classname == "MetaObjectDef") {
         clone = destination.make<MetaObjectDef>();
         MetaObjectDef* c = static_cast<MetaObjectDef*>(clone);
         MetaObjectDef* o = static_cast<MetaObjectDef*>(source);
         clone_init_metaobjectdef(destination,c,o);
      }
      else if (classname == "BooleanType") {
         clone = destination.make<BooleanType>();
         BooleanType* c = static_cast<BooleanType*>(clone);
         BooleanType* o = static_cast<BooleanType*>(source);
         clone_init_booleantype(destination,c,o);
      }
      else if (classname == "TextType") {
         clone = destination.make<TextType>();
         TextType* c = static_cast<TextType*>(clone);
         TextType* o = static_cast<TextType*>(source);
         clone_init_texttype(destination,c,o);
      }
      else if (classname == "BlackboxType") {
         clone = destination.make<BlackboxType>();
         BlackboxType* c = static_cast<BlackboxType*>(clone);
         BlackboxType* o = static_cast<BlackboxType*>(source);
         clone_init_blackboxtype(destination,c,o);
      }
      else if (classname == "NumType") {
         clone = destination.make<NumType>();
         NumType* c = static_cast<NumType*>(clone);
         NumType* o = static_cast<NumType*>(source);
         clone_init_numtype(destination,c,o);
      }
      else if (classname == "CoordType") {
         clone = destination.make<CoordType>();
         CoordType* c = static_cast<CoordType*>(clone);
         CoordType* o = static_cast<CoordType*>(source);
         clone_init_coordtype(destination,c,o);
      }
      else if (classname == "AxisSpec") {
         clone = destination.make<AxisSpec>();
         AxisSpec* c = static_cast<AxisSpec*>(clone);
         AxisSpec* o = static_cast<AxisSpec*>(source);
         clone_init_axisspec(destination,c,o);
      }
      else if (classname == "NumsRefSys") {
         clone = destination.make<NumsRefSys>();
         NumsRefSys* c = static_cast<NumsRefSys*>(clone);
         NumsRefSys* o = static_cast<NumsRefSys*>(source);
         clone_init_numsrefsys(destination,c,o);
      }
      else if (classname == "FormattedType") {
         clone = destination.make<FormattedType>();
         FormattedType* c = static_cast<FormattedType*>(clone);
         FormattedType* o = static_cast<FormattedType*>(source);
         clone_init_formattedtype(destination,c,o);
      }
      else if (classname == "AnyOIDType") {
         clone = destination.make<AnyOIDType>();
         AnyOIDType* c = static_cast<AnyOIDType*>(clone);
         AnyOIDType* o = static_cast<AnyOIDType*>(source);
         clone_init_anyoidtype(destination,c,o);
      }
      else if (classname == "FunctionDef") {
         clone = destination.make<FunctionDef>();
         FunctionDef* c = static_cast<FunctionDef*>(clone);
         FunctionDef* o = static_cast<FunctionDef*>(source);
         clone_init_functiondef(destination,c,o);
      }
      else if (classname == "Argument") {
         clone = destination.make<Argument>();
         Argument* c = static_cast<Argument*>(clone);
         Argument* o = static_cast<Argument*>(source);
         clone_init_argument(destination,c,o);
      }
      else if (classname == "ClassRefType") {
         clone = destination.make<ClassRefType>();
         ClassRefType* c = static_cast<ClassRefType*>(clone);
         ClassRefType* o = static_cast<ClassRefType*>(source);
         clone_init_classreftype(destination,c,o);
      }
      else if (classname == "ObjectType") {
         clone = destination.make<ObjectType>();
         ObjectType* c = static_cast<ObjectType*>(clone);
         ObjectType* o = static_cast<ObjectType*>(source);
         clone_init_objecttype(destination,c,o);
      }
      else if (classname == "AttributeRefType") {
         clone = destination.make<AttributeRefType>();
         AttributeRefType* c = static_cast<AttributeRefType*>(clone);
         AttributeRefType* o = static_cast<AttributeRefType*>(source);
         clone_init_attributereftype(destination,c,o);
      }
      else if (classname == "ARefRestriction") {
         clone = destination.make<ARefRestriction>();
         ARefRestriction* c = static_cast<ARefRestriction*>(clone);
         ARefRestriction* o = static_cast<ARefRestriction*>(source);
         clone_init_arefrestriction(destination,c,o);
      }
      else if (classname == "EnumType") {
         clone = destination.make<EnumType>();
         EnumType* c = static_cast<EnumType*>(clone);
         EnumType* o = static_cast<EnumType*>(source);
         clone_init_enumtype(destination,c,o);
      }
      else if (classname == "EnumNode") {
         clone = destination.make<EnumNode>();
         EnumNode* c = static_cast<EnumNode*>(clone);
         EnumNode* o = static_cast<EnumNode*>(source);
         clone_init_enumnode(destination,c,o);
      }
      else if (classname == "EnumTreeValueType") {
         clone = destination.make<EnumTreeValueType>();
         EnumTreeValueType* c = static_cast<EnumTreeValueType*>(clone);
         EnumTreeValueType* o = static_cast<EnumTreeValueType*>(source);
         clone_init_enumtreevaluetype(destination,c,o);
      }
      else if (classname == "LineForm") {
         clone = destination.make<LineForm>();
         LineForm* c = static_cast<LineForm*>(clone);
         LineForm* o = static_cast<LineForm*>(source);
         clone_init_lineform(destination,c,o);
      }
      else if (classname == "LineType") {
         clone = destination.make<LineType>();
         LineType* c = static_cast<LineType*>(clone);
         LineType* o = static_cast<LineType*>(source);
         clone_init_linetype(destination,c,o);
      }
      else if (classname == "LinesForm") {
         clone = destination.make<LinesForm>();
         LinesForm* c = static_cast<LinesForm*>(clone);
         LinesForm* o = static_cast<LinesForm*>(source);
         clone_init_linesform(destination,c,o);
      }
      else if (classname == "View") {
         clone = destination.make<View>();
         View* c = static_cast<View*>(clone);
         View* o = static_cast<View*>(source);
         clone_init_view(destination,c,o);
      }
      else if (classname == "RenamedBaseView") {
         clone = destination.make<RenamedBaseView>();
         RenamedBaseView* c = static_cast<RenamedBaseView*>(clone);
         RenamedBaseView* o = static_cast<RenamedBaseView*>(source);
         clone_init_renamedbaseview(destination,c,o);
      }
      else if (classname == "Expression") {
         clone = destination.make<Expression>();
         Expression* c = static_cast<Expression*>(clone);
         Expression* o = static_cast<Expression*>(source);
         clone_init_expression(destination,c,o);
      }
      else if (classname == "UnaryExpr") {
         clone = destination.make<UnaryExpr>();
         UnaryExpr* c = static_cast<UnaryExpr*>(clone);
         UnaryExpr* o = static_cast<UnaryExpr*>(source);
         clone_init_unaryexpr(destination,c,o);
      }
      else if (classname == "CompoundExpr") {
         clone = destination.make<CompoundExpr>();
         CompoundExpr* c = static_cast<CompoundExpr*>(clone);
         CompoundExpr* o = static_cast<CompoundExpr*>(source);
         clone_init_compoundexpr(destination,c,o);
      }
      else if (classname == "Factor") {
         clone = destination.make<Factor>();
         Factor* c = static_cast<Factor*>(clone);
         Factor* o = static_cast<Factor*>(source);
         clone_init_factor(destination,c,o);
      }
      else if (classname == "PathEl") {
         clone = destination.make<PathEl>();
         PathEl* c = static_cast<PathEl*>(clone);
         PathEl* o = static_cast<PathEl*>(source);
         clone_init_pathel(destination,c,o);
      }
      else if (classname == "PathOrInspFactor") {
         clone = destination.make<PathOrInspFactor>();
         PathOrInspFactor* c = static_cast<PathOrInspFactor*>(clone);
         PathOrInspFactor* o = static_cast<PathOrInspFactor*>(source);
         clone_init_pathorinspfactor(destination,c,o);
      }
      else if (classname == "EnumAssignment") {
         clone = destination.make<EnumAssignment>();
         EnumAssignment* c = static_cast<EnumAssignment*>(clone);
         EnumAssignment* o = static_cast<EnumAssignment*>(source);
         clone_init_enumassignment(destination,c,o);
      }
      else if (classname == "EnumMapping") {
         clone = destination.make<EnumMapping>();
         EnumMapping* c = static_cast<EnumMapping*>(clone);
         EnumMapping* o = static_cast<EnumMapping*>(source);
         clone_init_enummapping(destination,c,o);
      }
      else if (classname == "ClassRef") {
         clone = destination.make<ClassRef>();
         ClassRef* c = static_cast<ClassRef*>(clone);
         ClassRef* o = static_cast<ClassRef*>(source);
         clone_init_classref(destination,c,o);
      }
      else if (classname == "ActualArgument") {
         clone = destination.make<ActualArgument>();
         ActualArgument* c = static_cast<ActualArgument*>(clone);
         ActualArgument* o = static_cast<ActualArgument*>(source);
         clone_init_actualargument(destination,c,o);
      }
      else if (classname == "FunctionCall") {
         clone = destination.make<FunctionCall>();
         FunctionCall* c = static_cast<FunctionCall*>(clone);
         FunctionCall* o = static_cast<FunctionCall*>(source);
         clone_init_functioncall(destination,c,o);
      }
      else if (classname == "RuntimeParamRef") {
         clone = destination.make<RuntimeParamRef>();
         RuntimeParamRef* c = static_cast<RuntimeParamRef*>(clone);
         RuntimeParamRef* o = static_cast<RuntimeParamRef*>(source);
         clone_init_runtimeparamref(destination,c,o);
      }
      else if (classname == "Constant") {
         clone = destination.make<Constant>();
         Constant* c = static_cast<Constant*>(clone);
         Constant* o = static_cast<Constant*>(source);
         clone_init_constant(destination,c,o);
      }
      else if (classname == "ClassConst") {
         clone = destination.make<ClassConst>();
         ClassConst* c = static_cast<ClassConst*>(clone);
         ClassConst* o = static_cast<ClassConst*>(source);
         clone_init_classconst(destination,c,o);
      }
      else if (classname == "AttributeConst") {
         clone = destination.make<AttributeConst>();
         AttributeConst* c = static_cast<AttributeConst*>(clone);
         AttributeConst* o = static_cast<AttributeConst*>(source);
         clone_init_attributeconst(destination,c,o);
      }
      else if (classname == "UnitRef") {
         clone = destination.make<UnitRef>();
         UnitRef* c = static_cast<UnitRef*>(clone);
         UnitRef* o = static_cast<UnitRef*>(source);
         clone_init_unitref(destination,c,o);
      }
      else if (classname == "UnitFunction") {
         clone = destination.make<UnitFunction>();
         UnitFunction* c = static_cast<UnitFunction*>(clone);
         UnitFunction* o = static_cast<UnitFunction*>(source);
         clone_init_unitfunction(destination,c,o);
      }
      else if (classname == "SimpleConstraint") {
         clone = destination.make<SimpleConstraint>();
         SimpleConstraint* c = static_cast<SimpleConstraint*>(clone);
         SimpleConstraint* o = static_cast<SimpleConstraint*>(source);
         clone_init_simpleconstraint(destination,c,o);
      }
      else if (classname == "ExistenceDef") {
         clone = destination.make<ExistenceDef>();
         ExistenceDef* c = static_cast<ExistenceDef*>(clone);
         ExistenceDef* o = static_cast<ExistenceDef*>(source);
         clone_init_existencedef(destination,c,o);
      }
      else if (classname == "ExistenceConstraint") {
         clone = destination.make<ExistenceConstraint>();
         ExistenceConstraint* c = static_cast<ExistenceConstraint*>(clone);
         ExistenceConstraint* o = static_cast<ExistenceConstraint*>(source);
         clone_init_existenceconstraint(destination,c,o);
      }
      else if (classname == "UniqueConstraint") {
         clone = destination.make<UniqueConstraint>();
         UniqueConstraint* c = static_cast<UniqueConstraint*>(clone);
         UniqueConstraint* o = static_cast<UniqueConstraint*>(source);
         clone_init_uniqueconstraint(destination,c,o);
      }
      else if (classname == "SetConstraint") {
         clone = destination.make<SetConstraint>();
         SetConstraint* c = static_cast<SetConstraint*>(clone);
         SetConstraint* o = static_cast<SetConstraint*>(source);
         clone_init_setconstraint(destination,c,o);
      }
      else if (classname == "Graphic") {
         clone = destination.make<Graphic>();
         Graphic* c = static_cast<Graphic*>(clone);
         Graphic* o = static_cast<Graphic*>(source);
         clone_init_graphic(destination,c,o);
      }
      else if (classname == "SignParamAssignment") {
         clone = destination.make<SignParamAssignment>();
         SignParamAssignment* c = static_cast<SignParamAssignment*>(clone);
         SignParamAssignment* o = static_cast<SignParamAssignment*>(source);
         clone_init_signparamassignment(destination,c,o);
      }
      else if (classname == "CondSignParamAssignment") {
         clone = destination.make<CondSignParamAssignment>();
         CondSignParamAssignment* c = static_cast<CondSignParamAssignment*>(clone);
         CondSignParamAssignment* o = static_cast<CondSignParamAssignment*>(source);
         clone_init_condsignparamassignment(destination,c,o);
      }
      else if (classname == "DrawingRule") {
         clone = destination.make<DrawingRule>();
         DrawingRule* c = static_cast<DrawingRule*>(clone);
         DrawingRule* o = static_cast<DrawingRule*>(source);
         clone_init_drawingrule(destination,c,o);
      }
      else if (classname == "DocTextTranslation") {
         clone = destination.make<DocTextTranslation>();
         DocTextTranslation* c = static_cast<DocTextTranslation*>(clone);
         DocTextTranslation* o = static_cast<DocTextTranslation*>(source);
         clone_init_doctexttranslation(destination,c,o);
      }
      else if (classname == "METranslation") {
         clone = destination.make<METranslation>();
         METranslation* c = static_cast<METranslation*>(clone);
         METranslation* o = static_cast<METranslation*>(source);
         clone_init_metranslation(destination,c,o);
      }
      else if (classname == "Translation") {
         clone = destination.make<Translation>();
         Translation* c = static_cast<Translation*>(clone);
         Translation* o = static_cast<Translation*>(source);
         clone_init_translation(destination,c,o);
      }
      else {
         throw std::runtime_error("clone(): unsupported class " + classname);
      }

      return clone;

   }
      
} // namespace metamodel
