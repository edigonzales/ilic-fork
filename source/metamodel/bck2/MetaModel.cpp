#include "MetaModel.h"
#include "../util/Logger.h"

using namespace util;

namespace metamodel {

   // global variables

   static list <Model*> AllModels;
   static list <Import*> AllImports;
   static list <Dependency*> AllDependencies;
   static Class* class_context;

   // helper functions

   void init(string version)
   {
   }

   // model helpers

   void add_model(Model* model)
   {
      if (model == nullptr) {
         return;
      }
      for (Package* m : AllModels) {
         if (m->Name == model->Name) {
            // add model only once
            return;
         }
      }
      AllModels.push_back(model);
   }

   list<Model*> get_all_models()
   {
      return AllModels;
   }

   void add_import(Import* import)
   {
      if (import == nullptr) {
         return;
      }
      AllImports.push_back(import);
   }

   list<Import*> get_all_imports()
   {
      return AllImports;
   }

   void add_dependency(Dependency* d)
   {
      if (d == nullptr) {
         return;
      }
      AllDependencies.push_back(d);
   }

   list<Dependency*> get_all_dependencies()
   {
      return AllDependencies;
   }

   // path utilities

   string get_enumnode_path(EnumNode* n)
   {
      if (n->ParentNode == nullptr) {
         return n->Name; // TOP ???, to do !!!
      }
      else {
         return get_enumnode_path(n->ParentNode) + "." + n->Name;
      }
   }
      
   string get_type_path(Type* t)
   {
      if (t->_attr == nullptr) {
         return get_parent_path(t) + "." + t->Name;
      }
      else {
         // local attribute type
         return get_path(t->_attr) + ".TYPE";
      }
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
      if (e->getClass() == "EnumNode") {
         EnumNode *n = dynamic_cast<EnumNode *>(e);
         return get_path(n->EnumType) + "." + get_enumnode_path(n);
      }
      else if (e->getClass() == "AttrOrParam") {
         AttrOrParam *a = dynamic_cast<AttrOrParam *>(e);
         return get_path(a->AttrParent) + "." + a->Name;
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

   // instance methods

   static MMObject* get_instance(string classname)
   {
      if (classname == "Model") {
         return new Model();
      }
      else if (classname == "MMObject") {
         return new MMObject();
      }
      else if (classname == "DocText") {
         return new DocText();
      }
      else if (classname == "MetaAttribute") {
         return new MetaAttribute();
      }
      else if (classname == "MetaElement") {
         return new MetaElement();
      }
      else if (classname == "ExtendableME") {
         return new ExtendableME();
      }
      else if (classname == "Package") {
         return new Package();
      }
      else if (classname == "Import") {
         return new Import();
      }
      else if (classname == "Ili1Format") {
         return new Ili1Format();
      }
      else if (classname == "Model") {
         return new Model();
      }
      else if (classname == "SubModel") {
         return new SubModel();
      }
      else if (classname == "Type") {
         return new Type();
      }
      else if (classname == "Multiplicity") {
         return new Multiplicity();
      }
      else if (classname == "Constraint") {
         return new Constraint();
      }
      else if (classname == "DomainType") {
         return new DomainType();
      }
      else if (classname == "Class") {
         return new Class();
      }
      else if (classname == "AttrOrParam") {
         return new AttrOrParam();
      }
      else if (classname == "TypeRelatedType") {
         return new TypeRelatedType();
      }
      else if (classname == "TypeRestriction") {
         return new TypeRestriction();
      }
      else if (classname == "MultiValue") {
         return new MultiValue();
      }
      else if (classname == "ClassRelatedType") {
         return new ClassRelatedType();
      }
      else if (classname == "BaseClass") {
         return new BaseClass();
      }
      else if (classname == "ClassRestriction") {
         return new ClassRestriction();
      }
      else if (classname == "ReferenceType") {
         return new ReferenceType();
      }
      else if (classname == "Role") {
         return new Role();
      }
      else if (classname == "ExplicitAssocAccess") {
         return new ExplicitAssocAccess();
      }
      else if (classname == "AssocAcc") {
         return new AssocAcc();
      }
      else if (classname == "TransferElement") {
         return new TransferElement();
      }
      else if (classname == "Ili1TransferElement") {
         return new Ili1TransferElement();
      }
      else if (classname == "DataUnit") {
         return new DataUnit();
      }
      else if (classname == "Dependency") {
         return new Dependency();
      }
      else if (classname == "AllowedInBasket") {
         return new AllowedInBasket();
      }
      else if (classname == "Context") {
         return new Context();
      }
      else if (classname == "GenericDef") {
         return new GenericDef();
      }
      else if (classname == "Unit") {
         return new Unit();
      }
      else if (classname == "MetaBasketDef") {
         return new MetaBasketDef();
      }
      else if (classname == "MetaObjectDef") {
         return new MetaObjectDef();
      }
      else if (classname == "BooleanType") {
         return new BooleanType();
      }
      else if (classname == "TextType") {
         return new TextType();
      }
      else if (classname == "BlackboxType") {
         return new BlackboxType();
      }
      else if (classname == "NumType") {
         return new NumType();
      }
      else if (classname == "CoordType") {
         return new CoordType();
      }
      else if (classname == "AxisSpec") {
         return new AxisSpec();
      }
      else if (classname == "NumsRefSys") {
         return new NumsRefSys();
      }
      else if (classname == "FormattedType") {
         return new FormattedType();
      }
      else if (classname == "AnyOIDType") {
         return new AnyOIDType();
      }
      else if (classname == "FunctionDef") {
         return new FunctionDef();
      }
      else if (classname == "Argument") {
         return new Argument();
      }
      else if (classname == "ClassRefType") {
         return new ClassRefType();
      }
      else if (classname == "ObjectType") {
         return new ObjectType();
      }
      else if (classname == "AttributeRefType") {
         return new AttributeRefType();
      }
      else if (classname == "ARefRestriction") {
         return new ARefRestriction();
      }
      else if (classname == "EnumType") {
         return new EnumType();
      }
      else if (classname == "EnumNode") {
         return new EnumNode();
      }
      else if (classname == "EnumTreeValueType") {
         return new EnumTreeValueType();
      }
      else if (classname == "LineForm") {
         return new LineForm();
      }
      else if (classname == "LineType") {
         return new LineType();
      }
      else if (classname == "LinesForm") {
         return new LinesForm();
      }
      else if (classname == "View") {
         return new View();
      }
      else if (classname == "Expression") {
         return new Expression();
      }
      else if (classname == "UnaryExpr") {
         return new UnaryExpr();
      }
      else if (classname == "CompoundExpr") {
         return new CompoundExpr();
      }
      else if (classname == "Factor") {
         return new Factor();
      }
      else if (classname == "PathEl") {
         return new PathEl();
      }
      else if (classname == "PathOrInspFactor") {
         return new PathOrInspFactor();
      }
      else if (classname == "EnumAssignment") {
         return new EnumAssignment();
      }
      else if (classname == "EnumMapping") {
         return new EnumMapping();
      }
      else if (classname == "ClassRef") {
         return new ClassRef();
      }
      else if (classname == "ActualArgument") {
         return new ActualArgument();
      }
      else if (classname == "FunctionCall") {
         return new FunctionCall();
      }
      else if (classname == "RuntimeParamRef") {
         return new RuntimeParamRef();
      }
      else if (classname == "Constant") {
         return new Constant();
      }
      else if (classname == "ClassConst") {
         return new ClassConst();
      }
      else if (classname == "AttributeConst") {
         return new AttributeConst();
      }
      else if (classname == "UnitRef") {
         return new UnitRef();
      }
      else if (classname == "UnitFunction") {
         return new UnitFunction();
      }
      else if (classname == "Constraint") {
         return new Constraint();
      }
      else if (classname == "SimpleConstraint") {
         return new SimpleConstraint();
      }
      else if (classname == "ExistenceDef") {
         return new ExistenceDef();
      }
      else if (classname == "ExistenceConstraint") {
         return new ExistenceConstraint();
      }
      else if (classname == "UniqueConstraint") {
         return new UniqueConstraint();
      }
      else if (classname == "SetConstraint") {
         return new SetConstraint();
      }
      else if (classname == "Graphic") {
         return new Graphic();
      }
      else if (classname == "SignParamAssignment") {
         return new SignParamAssignment();
      }
      else if (classname == "CondSignParamAssignment") {
         return new CondSignParamAssignment();
      }
      else if (classname == "DrawingRule") {
         return new DrawingRule();
      }
      else if (classname == "DocTextTranslation") {
         return new DocTextTranslation();
      }
      else if (classname == "METranslation") {
         return new METranslation();
      }
      else if (classname == "Translation") {
         return new Translation();
      }
      else {
         return nullptr;
      }

   }

   bool MMObject::isSubClassOf(string classname)
   {

      MMObject* o = this;

      while (true) {
         if (o->getClass() == classname) {
            break;
         }
         else if (o->getBaseClass() == classname) {
            break;
         }
         else {
            string basename = o->getBaseClass();
            if (o != this) {
               delete o;
            }
            o = get_instance(basename);
            if (o == nullptr) {
               break;
            }
         }
      }

      if (o == nullptr) {
         return false;
      }
      else {
         if (o != this) {
            delete o;
         }
         return true;
      }

   }

   static void clone_init_mmobject(MMObject *clone, MMObject *org)
   {

      /* class MMObject { // ABSTRACT
      public:
         int _line;
      */
      
      clone->_line = org->_line;
      clone->_source = org->_source;
      clone->_selectionSource = org->_selectionSource;
      clone->_referenceSources = org->_referenceSources;

   }
      
   static void clone_init_metaelement(MetaElement *clone, MetaElement *org)
   {

      /* class MetaElement : public MMObject { // ABSTRACT
         // OID: <Parent-OID>.Name
      public:
         string Name = "";
         list <DocText *> Documentation; // LIST
         // ROLE from ASSOCIATION MetaAttributes
         list <MetaAttribute *> MetaAttribute;
         // ROLE from ASSOCIATION PackageElements
         Package *ElementInPackage = nullptr;
      */
      
      clone_init_mmobject(clone,org);
      
      clone->Name = org->Name;
      clone->Documentation = org->Documentation;
      clone->MetaAttribute = org->MetaAttribute;
      clone->ElementInPackage = org->ElementInPackage;

   }

   static void clone_init_extendableme(ExtendableME *clone, ExtendableME *org)
   {

      /* class ExtendableME : public MetaElement { // ABSTRACT
      public:
         bool Abstract = false;
         bool Generic = false; // 2.4
         bool Final = false;
         // ROLE from ASSOCIATION Inheritance
         ExtendableME *Super = nullptr;
         // ROLE from ASSOCIATION Inheritance
         list <ExtendableME *> Sub;
      */

      clone_init_metaelement(clone,org);
      
      clone->Abstract = org->Abstract;
      clone->Generic = org->Generic;
      clone->Final = org->Final;
      clone->Super = org->Super;
      clone->Sub = org->Sub;

   }
   
   static void clone_init_type(Type *clone,Type *base)
   {

      /* class Type : public ExtendableME { // ABSTRACT
      public:
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
         Type *_base_type = nullptr;
         metamodel::AttrOrParam *_attr = nullptr;
         virtual string getClass() { return "Type"; }
         virtual string getBaseClass() { return "ExtendableME"; };
         virtual bool isAbstract() { return true; }
      */
     
      clone_init_extendableme(clone,base);

      clone->AttrOrParam = base->AttrOrParam;
      clone->TRT = base->TRT;
      clone->LFTParent = base->LFTParent;
      // etc., to do !!!
         
   }

   static void clone_init_domaintype(DomainType *clone,DomainType *base)
   {

      /* class DomainType : public Type { // ABSTRACT
      public:
         bool Mandatory = false;
         // role from ASSOCIATION BasketOID
         list <DataUnit *> ForDataUnit;
         // role form ASSOCIATION DomainConstraint
         list <Constraint *> Constraint;
         // role form ASSOCIATION ConcreteForGeneric
         GenericDef *GenericDef = nullptr;
      */
      
      clone_init_type(clone,base);

      clone->Mandatory = base->Mandatory;
      clone->ForDataUnit = base->ForDataUnit;
      clone->Constraint = base->Constraint;
      clone->GenericDef = base->GenericDef;
         
   }
   
   static void clone_init_booleantype(BooleanType *clone,BooleanType *org)
   {

      /*class BooleanType : public DomainType {
        public:
      */   
      
      clone_init_domaintype(clone,org);
      
   }

   static void clone_init_numtype(NumType *clone,NumType *org)
   {

      /*
      class NumType : public DomainType {
      public:
         string Min = "";
         string Max = "";
         bool Circular = false;
         bool Clockwise = false;
         // frole from ASSOCIATION NumUnit
         Unit *Unit = nullptr;
      */
     
      clone_init_domaintype(clone,org);
     
      clone->Min = org->Min;
      clone->Max = org->Max;
      clone->Circular = org->Circular;
      clone->Unit = org->Unit;
      
   }
   
   static void clone_init_texttype(TextType *clone,TextType *org)
   {

      /* class TextType : public DomainType {
      public:
         enum { MText, Text, NameVal, Uri } Kind;
         int MaxLength = 0;
      */
     
      clone_init_domaintype(clone,org);
     
      clone->Kind = org->Kind;
      clone->MaxLength = org->MaxLength;
      
   }
   
   static void clone_init_enumtype(EnumType *clone,EnumType *org)
   {

      /* class EnumType : public DomainType {
      public:
         enum {Unordered, Ordered, Circular} Order;
         // role from ASSOCIATION TopNode
         list <EnumNode *> TopNode;
         // role from ASSOCIATION TreeValueTypeOf
         list <EnumTreeValueType *> ETVT;
      */
     
      clone_init_domaintype(clone,org);
     
      clone->Order = org->Order;
      clone->TopNode = org->TopNode;
      clone->ETVT = org->ETVT;
      
   }
   
   static void clone_init_coordtype(CoordType *clone,CoordType *org)
   {

      /* class CoordType : public DomainType {
      public:
         int NullAxis = 1;
         int PiHalfAxis = 2;
         bool Multi = false; // 2.4
         // role from ASSOCIATION LineCoord
         list <NumType *> Axis;
      */
     
      clone_init_domaintype(clone,org);
     
      clone->NullAxis = org->NullAxis;
      clone->PiHalfAxis = org->PiHalfAxis;
      clone->Multi = org->Multi;
      clone->Axis = org->Axis;
      
   }
   
   MMObject* MMObject::clone()
   {

      MMObject *clone = nullptr;
      string classname = this->getClass();
      
      if (classname == "BooleanType") {
         clone = new BooleanType();
         BooleanType* c = static_cast<BooleanType*>(clone);
         BooleanType* o = static_cast<BooleanType*>(this);
         clone_init_booleantype(c,o);
      }
      else if (classname == "NumType") {
         clone = new NumType();
         NumType* c = static_cast<NumType*>(clone);
         NumType* o = static_cast<NumType*>(this);
         clone_init_numtype(c,o);
      }
      else if (classname == "TextType") {
         clone = new TextType();
         TextType* c = static_cast<TextType*>(clone);
         TextType* o = static_cast<TextType*>(this);
         clone_init_texttype(c,o);
      }
      else if (classname == "EnumType") {
         clone = new EnumType();
         EnumType* c = static_cast<EnumType*>(clone);
         EnumType* o = static_cast<EnumType*>(this);
         clone_init_enumtype(c,o);
      }
      else if (classname == "CoordType") {
         clone = new CoordType();
         CoordType* c = static_cast<CoordType*>(clone);
         CoordType* o = static_cast<CoordType*>(this);
         clone_init_coordtype(c,o);
      }
      else {
         Log.internal_error("clone(): unsupported type " + classname,1);
      }

      return clone;

   }
      
} // namespace metamodel
