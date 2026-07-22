#include "MetaModel.h"
#include "../util/Logger.h"

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

   // global variables

   static list <DataUnit*> AllDataUnits;
   static list <Model*> AllModels;
   static list <Import*> AllImports;
   static list <Dependency*> AllDependencies;
   static list <AxisSpec*> AllAxisSpecs;
   static Class* class_context;
   static list<MetaElement*> context;

   // helper functions

   void init(string version)
   {
   }

   void reset()
   {
      AllDataUnits.clear();
      AllModels.clear();
      AllImports.clear();
      AllDependencies.clear();
      AllAxisSpecs.clear();
      context.clear();
      class_context = nullptr;
   }

   // model helpers

   void add_dataunit(DataUnit* u)
   {
      if (u == nullptr) {
         return;
      }
      for (DataUnit* uu : AllDataUnits) {
         if (uu->Name == u->ElementInPackage->Name) {
            Log.error("multiple declaration of topic " + u->ElementInPackage->Name, u->_line);
            return;
         }
      }
      AllDataUnits.push_back(u);
   }

   list<DataUnit*> get_all_dataunits()
   {
      return AllDataUnits;
   }

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

   list<string> get_all_unqualified_imports(string modelname)
   {
      list<string> unqualified_imports;
      for (auto i : AllImports) {
         if (i->ImportingP->Name == modelname && i->_unqualified) {
            unqualified_imports.push_back(i->ImportedP->Name);
         }
      }
      return unqualified_imports;
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
   
   bool depends_on(Package *p)
   {
      // Predefined universal types such as ANYCLASS and model-level elements do
      // not belong to a topic and therefore require no topic dependency.
      if (p == nullptr) {
         return true;
      }
      if (p->getClass() != "SubModel") {
         return true;
      }
      SubModel *s = get_topic_context();
      if (s == nullptr) {
         return true;
      }
      SubModel *ss = s;
      while (ss != nullptr) {
         if (ss == p) {
            return true;
         }
         ss = static_cast<SubModel *>(ss->_super);
      }
      ss = static_cast<SubModel*>(p);
      for (auto d : get_all_dependencies()) {
         if ((d->Dependent == ss->_dataunit) && (d->Using == s->_dataunit)) {
            return true;
         }
      }
      return false;
   }
   
   // AxisSpec

   void add_axisspec(AxisSpec* s)
   {
      if (s == nullptr) {
         return;
      }
      AllAxisSpecs.push_back(s);
   }

   list<AxisSpec*> get_all_axisspecs()
   {
      return AllAxisSpecs;
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

   // context helpers

   static string get_context_string()
   {
      string cstring = "";
      for (auto c : context) {
         cstring += ">>>" + c->getClass() + ":" + c->Name;
      }
      return cstring;
   }

   void push_context(MetaElement* m)
   {
      context.push_back(m);
      Log.debug(">>> push_context " + get_context_string());
   }

   void pop_context()
   {
      MetaElement* m = context.back();
      Log.debug("<<< pop_context " + get_context_string());
      context.pop_back();
   }

   MetaElement* get_context()
   {
      if (context.size() > 0) {
         return context.back();
      }
      else {
         return nullptr;
      }
   }

   MetaElement* get_context(string classname)
   {
      list<MetaElement*> c;
      c = context;
      while (true) {
         if (c.empty()) {
            break;
         }
         MetaElement* m = c.back();
         if (m->isSubClassOf(classname)) {
            return m;
         }
         c.pop_back();
      }
      return nullptr;
   }

   Class* get_class_context()
   {
      return dynamic_cast<Class*>(get_context("Class"));
   }

   Package* get_package_context()
   {
      return dynamic_cast<Package*>(get_context("Package"));
   }

   SubModel* get_topic_context()
   {
      return dynamic_cast<SubModel*>(get_context("SubModel"));
   }

   Model* get_model_context()
   {
      return dynamic_cast<Model*>(get_context("Model"));
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
      else if (classname == "MultiValue") {
         return new MultiValue();
      }
      else if (classname == "ClassRelatedType") {
         return new ClassRelatedType();
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
   
   // clone() method implementation

   static void clone_init_mmobject(MMObject *clone,MMObject *org)
   {
      clone->_line = org->_line;
      clone->_source = org->_source;
      clone->_selectionSource = org->_selectionSource;
      clone->_endSelectionSource = org->_endSelectionSource;
      clone->_referenceSources = org->_referenceSources;
   }

   static void clone_init_doctext(DocText *clone,DocText *org)
   {

      clone_init_mmobject(clone,org);

      clone->Name = org->Name;
      clone->Text = org->Text;

   }

   static void clone_init_metaattribute(MetaAttribute *clone,MetaAttribute *org)
   {

      clone_init_mmobject(clone,org);

      clone->Name = org->Name;
      clone->Value = org->Value;
      clone->MetaElement = org->MetaElement;

   }

   static void clone_init_metaelement(MetaElement *clone,MetaElement *org)
   {

      clone_init_mmobject(clone,org);

      clone->Name = org->Name;
      clone->Documentation = org->Documentation;
      clone->ElementInPackage = org->ElementInPackage;

   }

   static void clone_init_extendableme(ExtendableME *clone,ExtendableME *org)
   {

      clone_init_metaelement(clone,org);

      clone->Abstract = org->Abstract;
      clone->Generic = org->Generic;
      clone->Final = org->Final;
      clone->Super = org->Super;

   }

   static void clone_init_package(Package *clone,Package *org)
   {

      clone_init_metaelement(clone,org);
      clone->_super = org->_super;

   }

   static void clone_init_import(Import *clone,Import *org)
   {

      clone_init_mmobject(clone,org);

      clone->ImportingP = org->ImportingP;
      clone->ImportedP = org->ImportedP;
      clone->_unqualified = org->_unqualified;

   }

   static void clone_init_ili1format(Ili1Format *clone,Ili1Format *org)
   {

      clone_init_mmobject(clone,org);

      clone->LineSize = org->LineSize;
      clone->tidSize = org->tidSize;
      clone->blankCode = org->blankCode;
      clone->undefinedCode = org->undefinedCode;
      clone->continueCode = org->continueCode;
      clone->Font = org->Font;
      clone->tidKind = org->tidKind;
      clone->tidExplanation = org->tidExplanation;

   }

   static void clone_init_model(Model *clone,Model *org)
   {

      clone_init_package(clone,org);

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

   static void clone_init_submodel(SubModel *clone,SubModel *org)
   {
Log.message(">>> clone topic");
      clone_init_package(clone,org);
      clone->_dataunit = org->_dataunit;
      clone->DeferredGenerics = org->DeferredGenerics;
   }

   static void clone_init_type(Type *clone,Type *org)
   {

      clone_init_extendableme(clone,org);

      clone->LTParent = org->LTParent;
      clone->LFTParent = org->LFTParent;
      clone->_other_type = org->_other_type;
      clone->_attr = org->_attr;

   }

   static void clone_init_multiplicity(Multiplicity *clone,Multiplicity *org)
   {

      clone_init_mmobject(clone,org);

      clone->Min = org->Min;
      clone->Max = org->Max;

   }

   static void clone_init_constraint(Constraint *clone,Constraint *org)
   {

      clone_init_metaelement(clone,org);
      clone->toDomain = org->toDomain;
      clone->ToClass = org->ToClass;

   }

   static void clone_init_domaintype(DomainType *clone,DomainType *org)
   {

      clone_init_type(clone,org);

      clone->Mandatory = org->Mandatory;
      clone->GenericDef = org->GenericDef;

   }

   static void clone_init_class(Class *clone,Class *org)
   {

      clone_init_type(clone,org);

      clone->Kind = org->Kind;
      clone->Multiplicity = org->Multiplicity;
      clone->EmbeddedRoleTransfer = org->EmbeddedRoleTransfer;
      clone->ili1OptionalTable = org->ili1OptionalTable;
      clone->Oid = org->Oid;
      clone->View = org->View;
      clone->isDomainType = org->isDomainType;

   }

   static void clone_init_attrorparam(AttrOrParam *clone,AttrOrParam *org)
   {

      clone_init_extendableme(clone,org);

      clone->SubdivisionKind = org->SubdivisionKind;
      clone->Transient = org->Transient;
      clone->Derivates = org->Derivates;
      clone->AttrParent = org->AttrParent;
      clone->Type = org->Type;
      clone->TypeExplicitlyDefined = org->TypeExplicitlyDefined;

   }

   static void clone_init_typerelatedtype(TypeRelatedType *clone,TypeRelatedType *org)
   {

      clone_init_domaintype(clone,org);

      clone->BaseType = org->BaseType;

   }

   static void clone_init_multivalue(MultiValue *clone,MultiValue *org)
   {

      clone_init_typerelatedtype(clone,org);

      clone->Ordered = org->Ordered;
      clone->Multiplicity = org->Multiplicity;

   }

   static void clone_init_classrelatedtype(ClassRelatedType *clone,ClassRelatedType *org)
   {

      clone_init_domaintype(clone,org);

      clone->_baseclass = org->_baseclass;
      clone->_classrestriction = org->_classrestriction;

   }

   static void clone_init_referencetype(ReferenceType *clone,ReferenceType *org)
   {

      clone_init_classrelatedtype(clone,org);

      clone->External = org->External;

   }

   static void clone_init_role(Role *clone,Role *org)
   {

      clone_init_referencetype(clone,org);

      clone->Strongness = org->Strongness;
      clone->Ordered = org->Ordered;
      clone->Multiplicity = org->Multiplicity;
      clone->MultiplicityDefined = org->MultiplicityDefined;
      clone->Derivates = org->Derivates;
      clone->EmbeddedTransfer = org->EmbeddedTransfer;
      clone->Association = org->Association;

   }

   static void clone_init_explicitassocaccess(ExplicitAssocAccess *clone,ExplicitAssocAccess *org)
   {

      clone_init_extendableme(clone,org);

      clone->AssocAccOf = org->AssocAccOf;
      clone->OriginRole = org->OriginRole;
      clone->TargetRole = org->TargetRole;

   }

   static void clone_init_assocacc(AssocAcc *clone,AssocAcc *org)
   {

      clone_init_mmobject(clone,org);

      clone->Class = org->Class;
      clone->AssocAcc_ = org->AssocAcc_;

   }

   static void clone_init_transferelement(TransferElement *clone,TransferElement *org)
   {

      clone_init_mmobject(clone,org);

      clone->TransferClass = org->TransferClass;
      clone->TransferElement_ = org->TransferElement_;

   }

   static void clone_init_ili1transferelement(Ili1TransferElement *clone,Ili1TransferElement *org)
   {

      clone_init_mmobject(clone,org);

      clone->Ili1TransferClass = org->Ili1TransferClass;
      clone->Ili1RefAttr = org->Ili1RefAttr;

   }

   static void clone_init_dataunit(DataUnit *clone,DataUnit *org)
   {

      clone_init_extendableme(clone,org);

      clone->ViewUnit = org->ViewUnit;
      clone->DataUnitName = org->DataUnitName;
      clone->Oid = org->Oid;

   }

   static void clone_init_dependency(Dependency *clone,Dependency *org)
   {

      clone_init_mmobject(clone,org);

      clone->Using = org->Using;
      clone->Dependent = org->Dependent;

   }

   static void clone_init_allowedinbasket(AllowedInBasket *clone,AllowedInBasket *org)
   {

      clone_init_mmobject(clone,org);

      clone->OfDataUnit = org->OfDataUnit;
      clone->ClassInBasket = org->ClassInBasket;

   }

   static void clone_init_context(Context *clone,Context *org)
   {

      clone_init_metaelement(clone,org);
      clone->GenericDefinitions = org->GenericDefinitions;

   }

   static void clone_init_genericdef(GenericDef *clone,GenericDef *org)
   {

      clone_init_mmobject(clone,org);

      clone->OID = org->OID;
      clone->Context = org->Context;
      clone->GenericDomain = org->GenericDomain;
      clone->ConcreteDomain = org->ConcreteDomain;

   }

   static void clone_init_unit(Unit *clone,Unit *org)
   {

      clone_init_extendableme(clone,org);

      clone->Kind = org->Kind;
      clone->Definition = org->Definition;
      clone->_unitname = org->_unitname;

   }

   static void clone_init_metabasketdef(MetaBasketDef *clone,MetaBasketDef *org)
   {

      clone_init_extendableme(clone,org);

      clone->Kind = org->Kind;
      clone->MetaDataTopic = org->MetaDataTopic;
      clone->Member = org->Member;

   }

   static void clone_init_metaobjectdef(MetaObjectDef *clone,MetaObjectDef *org)
   {

      clone_init_metaelement(clone,org);

      clone->IsRefSystem = org->IsRefSystem;
      clone->Class = org->Class;

   }

   static void clone_init_booleantype(BooleanType *clone,BooleanType *org)
   {

      clone_init_domaintype(clone,org);


   }

   static void clone_init_texttype(TextType *clone,TextType *org)
   {

      clone_init_domaintype(clone,org);

      clone->Kind = org->Kind;
      clone->MaxLength = org->MaxLength;
      clone->OIDType = org->OIDType;
   }

   static void clone_init_blackboxtype(BlackboxType *clone,BlackboxType *org)
   {

      clone_init_domaintype(clone,org);

      clone->Kind = org->Kind;

   }

   static void clone_init_numtype(NumType *clone,NumType *org)
   {

      clone_init_domaintype(clone,org);

      clone->Min = org->Min;
      clone->Max = org->Max;
      clone->Circular = org->Circular;
      clone->Clockwise = org->Clockwise;
      clone->Unit = org->Unit;
      clone->OIDType = org->OIDType;

   }

   static void clone_init_coordtype(CoordType *clone,CoordType *org)
   {

      clone_init_domaintype(clone,org);

      clone->NullAxis = org->NullAxis;
      clone->PiHalfAxis = org->PiHalfAxis;
      clone->Multi = org->Multi;

   }

   static void clone_init_axisspec(AxisSpec *clone,AxisSpec *org)
   {

      clone_init_mmobject(clone,org);

      clone->CoordType = org->CoordType;
      clone->Axis = org->Axis;

   }

   static void clone_init_numsrefsys(NumsRefSys *clone,NumsRefSys *org)
   {

      clone_init_mmobject(clone,org);

      clone->NumType = org->NumType;
      clone->RefSys = org->RefSys;
      clone->AxisInd = org->AxisInd;

   }

   static void clone_init_formattedtype(FormattedType *clone,FormattedType *org)
   {

      clone_init_numtype(clone,org);

      clone->Format = org->Format;
      clone->Struct = org->Struct;
      clone->BaseFormattedType = org->BaseFormattedType;
   }

   static void clone_init_anyoidtype(AnyOIDType *clone,AnyOIDType *org)
   {
      clone_init_domaintype(clone,org);
   }

   static void clone_init_functiondef(FunctionDef *clone,FunctionDef *org)
   {

      clone_init_metaelement(clone,org);

      clone->Explanation = org->Explanation;
      clone->LocalType = org->LocalType;
      clone->ResultType = org->ResultType;

   }

   static void clone_init_argument(Argument *clone,Argument *org)
   {

      clone_init_metaelement(clone,org);

      clone->Kind = org->Kind;
      clone->Function = org->Function;
      clone->Type = org->Type;

   }

   static void clone_init_classreftype(ClassRefType *clone,ClassRefType *org)
   {
      clone_init_classrelatedtype(clone,org);
   }

   static void clone_init_objecttype(ObjectType *clone,ObjectType *org)
   {
      clone_init_classrelatedtype(clone,org);
      clone->Multiple = org->Multiple;
   }

   static void clone_init_attributereftype(AttributeRefType *clone,AttributeRefType *org)
   {

      clone_init_domaintype(clone,org);

      clone->Of = org->Of;
      clone->AttrRestriction = org->AttrRestriction;

   }

   static void clone_init_arefrestriction(ARefRestriction *clone,ARefRestriction *org)
   {

      clone_init_mmobject(clone,org);

      clone->ARef = org->ARef;
      clone->Type = org->Type;

   }

   static void clone_init_enumnode(EnumNode* clone, EnumNode* org)
   {

      clone_init_extendableme(clone, org);

      clone->EnumType = org->EnumType;
      clone->ParentNode = org->ParentNode;

      for (auto* orgNode : org->Node) {
         MMObject* cloneNode = nullptr;
         cloneNode = new EnumNode();
         EnumNode* c = static_cast<EnumNode*>(cloneNode);
         clone_init_enumnode(c, orgNode);
         clone->Node.push_back(c);
      }
   }

   static void clone_init_enumtreevaluetype(EnumTreeValueType* clone, EnumTreeValueType* org)
   {

      clone_init_domaintype(clone, org);

      clone->ET = org->ET;

   }

   static void clone_init_enumtype(EnumType *clone,EnumType *org)
   {

      clone_init_domaintype(clone,org);

      clone->Order = org->Order;

      clone->TopNode = new EnumNode;
      clone_init_enumnode(clone->TopNode, org->TopNode);

      for (auto* orgNode : org->ETVT) {
         MMObject* cloneNode = nullptr;
         cloneNode = new EnumTreeValueType();
         EnumTreeValueType* c = static_cast<EnumTreeValueType*>(cloneNode);

         clone_init_enumtreevaluetype(c, orgNode);
         clone->ETVT.push_back(c);
      }

   }

   static void clone_init_lineform(LineForm *clone,LineForm *org)
   {

      clone_init_metaelement(clone,org);

      clone->Structure = org->Structure;

   }

   static void clone_init_linesform(LinesForm* clone, LinesForm* org)
   {

      clone_init_mmobject(clone, org);

      clone->LineType = org->LineType;
      clone->LineForm = org->LineForm;

   }

   static void clone_init_linetype(LineType *clone,LineType *org)
   {

      clone_init_domaintype(clone,org);

      clone->Kind = org->Kind;
      clone->MaxOverlap = org->MaxOverlap;
      clone->Multi = org->Multi;
      clone->CoordType = org->CoordType;
      clone->LAStructure = org->LAStructure;

      for (auto* orgNode : org->LineForm) {
         MMObject* cloneNode = nullptr;
         cloneNode = new LineForm();
         LineForm* c = static_cast<LineForm*>(cloneNode);
         clone_init_lineform(c, orgNode);
         clone->LineForm.push_back(c);
      }

   }

   static void clone_init_view(View *clone,View *org)
   {

      clone_init_class(clone,org);

      clone->FormationKind = org->FormationKind;
      clone->FormationParameter = org->FormationParameter;
      clone->Where = org->Where;
      clone->Transient = org->Transient;

   }

   static void clone_init_renamedbaseview(RenamedBaseView *clone,RenamedBaseView *org)
   {

      clone_init_extendableme(clone,org);

      clone->OrNull = org->OrNull;
      clone->View = org->View;
      clone->BaseView = org->BaseView;

   }

   static void clone_init_expression(Expression *clone,Expression *org)
   {

      clone_init_mmobject(clone,org);

      clone->_type = org->_type;
      clone->ResolvedType = org->ResolvedType;
      clone->OccurrenceScope = org->OccurrenceScope;
      clone->OccurrencePackage = org->OccurrencePackage;

   }

   static void clone_init_unaryexpr(UnaryExpr *clone,UnaryExpr *org)
   {

      clone_init_expression(clone,org);

      clone->Operation = org->Operation;
      clone->SubExpression = org->SubExpression;

   }

   static void clone_init_compoundexpr(CompoundExpr *clone,CompoundExpr *org)
   {

      clone_init_expression(clone,org);

      clone->Operation = org->Operation;
      clone->SubExpressions = org->SubExpressions;

   }

   static void clone_init_factor(Factor *clone,Factor *org)
   {

      clone_init_expression(clone,org);


   }

   static void clone_init_pathel(PathEl *clone,PathEl *org)
   {

      clone_init_mmobject(clone,org);

      clone->Kind = org->Kind;
      clone->Ref = org->Ref;
      clone->NumIndex = org->NumIndex;
      clone->SpecIndex = org->SpecIndex;

   }

   static void clone_init_pathorinspfactor(PathOrInspFactor *clone,PathOrInspFactor *org)
   {

      clone_init_factor(clone,org);

      clone->PathEls = org->PathEls;
      clone->Inspection = org->Inspection;
      clone->_path = org->_path;

   }

   static void clone_init_enumassignment(EnumAssignment *clone,EnumAssignment *org)
   {

      clone_init_mmobject(clone,org);

      clone->ValueToAssign = org->ValueToAssign;
      clone->MinEnumValue = org->MinEnumValue;
      clone->MaxEnumValue = org->MaxEnumValue;

   }

   static void clone_init_enummapping(EnumMapping *clone,EnumMapping *org)
   {

      clone_init_factor(clone,org);

      clone->EnumValue = org->EnumValue;
      clone->Cases = org->Cases;

   }

   static void clone_init_classref(ClassRef *clone,ClassRef *org)
   {

      clone_init_mmobject(clone,org);

      clone->Ref = org->Ref;

   }

   static void clone_init_actualargument(ActualArgument *clone,ActualArgument *org)
   {

      clone_init_mmobject(clone,org);

      clone->FormalArgument = org->FormalArgument;
      clone->Kind = org->Kind;
      clone->Expression = org->Expression;

   }

   static void clone_init_functioncall(FunctionCall *clone,FunctionCall *org)
   {

      clone_init_factor(clone,org);

      clone->Function = org->Function;
      clone->Arguments = org->Arguments;

   }

   static void clone_init_runtimeparamref(RuntimeParamRef *clone,RuntimeParamRef *org)
   {

      clone_init_factor(clone,org);

      clone->RuntimeParam = org->RuntimeParam;

   }

   static void clone_init_constant(Constant *clone,Constant *org)
   {

      clone_init_factor(clone,org);

      clone->Value = org->Value;
      clone->Kind = org->Kind;

   }

   static void clone_init_classconst(ClassConst *clone,ClassConst *org)
   {

      clone_init_factor(clone,org);

      clone->Class = org->Class;

   }

   static void clone_init_attributeconst(AttributeConst *clone,AttributeConst *org)
   {

      clone_init_factor(clone,org);

      clone->Attribute = org->Attribute;

   }

   static void clone_init_unitref(UnitRef *clone,UnitRef *org)
   {

      clone_init_factor(clone,org);

      clone->Unit = org->Unit;

   }

   static void clone_init_unitfunction(UnitFunction *clone,UnitFunction *org)
   {

      clone_init_factor(clone,org);

      clone->Explanation = org->Explanation;

   }

   static void clone_init_simpleconstraint(SimpleConstraint *clone,SimpleConstraint *org)
   {

      clone_init_constraint(clone,org);

      clone->Kind = org->Kind;
      clone->Percentage = org->Percentage;
      clone->LogicalExpression = org->LogicalExpression;
      clone->_percentage_operation = org->_percentage_operation;

   }

   static void clone_init_existencedef(ExistenceDef *clone,ExistenceDef *org)
   {

      clone_init_pathorinspfactor(clone,org);

      clone->Viewable = org->Viewable;

   }

   static void clone_init_existenceconstraint(ExistenceConstraint *clone,ExistenceConstraint *org)
   {

      clone_init_constraint(clone,org);

      clone->Attr = org->Attr;

   }

   static void clone_init_uniqueconstraint(UniqueConstraint *clone,UniqueConstraint *org)
   {

      clone_init_constraint(clone,org);

      clone->Kind = org->Kind;
      clone->PerBasket = org->PerBasket;
      clone->Where = org->Where;
      clone->UniqueDef = org->UniqueDef;

   }

   static void clone_init_setconstraint(SetConstraint *clone,SetConstraint *org)
   {

      clone_init_constraint(clone,org);

      clone->Where = org->Where;
      clone->PerBasket = org->PerBasket;
      clone->Constraint = org->Constraint;

   }

   static void clone_init_graphic(Graphic *clone,Graphic *org)
   {

      clone_init_extendableme(clone,org);

      clone->Where = org->Where;
      clone->Base = org->Base;

   }

   static void clone_init_signparamassignment(SignParamAssignment *clone,SignParamAssignment *org)
   {

      clone_init_mmobject(clone,org);

      clone->Param = org->Param;
      clone->Assignment = org->Assignment;

   }

   static void clone_init_condsignparamassignment(CondSignParamAssignment *clone,CondSignParamAssignment *org)
   {

      clone_init_mmobject(clone,org);

      clone->Where = org->Where;
      clone->Assignments = org->Assignments;

   }

   static void clone_init_drawingrule(DrawingRule *clone,DrawingRule *org)
   {

      clone_init_extendableme(clone,org);

      clone->Rule = org->Rule;
      clone->Graphic = org->Graphic;
      clone->Class = org->Class;

   }

   static void clone_init_doctexttranslation(DocTextTranslation *clone,DocTextTranslation *org)
   {

      clone_init_mmobject(clone,org);

      clone->Text = org->Text;

   }

   static void clone_init_metranslation(METranslation *clone,METranslation *org)
   {

      clone_init_mmobject(clone,org);

      clone->Of = org->Of;
      clone->TranslatedName = org->TranslatedName;
      clone->TranslatedDoc = org->TranslatedDoc;

   }

   static void clone_init_translation(Translation *clone,Translation *org)
   {

      clone_init_mmobject(clone,org);

      clone->Language = org->Language;

   }

   MMObject* MMObject::clone()
   {

      MMObject *clone = nullptr;
      string classname = this->getClass();

      if (classname == "MMObject") {
         clone = new MMObject();
         MMObject* c = static_cast<MMObject*>(clone);
         MMObject* o = static_cast<MMObject*>(this);
         clone_init_mmobject(c,o);
      }
      else if (classname == "DocText") {
         clone = new DocText();
         DocText* c = static_cast<DocText*>(clone);
         DocText* o = static_cast<DocText*>(this);
         clone_init_doctext(c,o);
      }
      else if (classname == "MetaAttribute") {
         clone = new MetaAttribute();
         MetaAttribute* c = static_cast<MetaAttribute*>(clone);
         MetaAttribute* o = static_cast<MetaAttribute*>(this);
         clone_init_metaattribute(c,o);
      }
      else if (classname == "MetaElement") {
         clone = new MetaElement();
         MetaElement* c = static_cast<MetaElement*>(clone);
         MetaElement* o = static_cast<MetaElement*>(this);
         clone_init_metaelement(c,o);
      }
      else if (classname == "ExtendableME") {
         clone = new ExtendableME();
         ExtendableME* c = static_cast<ExtendableME*>(clone);
         ExtendableME* o = static_cast<ExtendableME*>(this);
         clone_init_extendableme(c,o);
      }
      else if (classname == "Package") {
         clone = new Package();
         Package* c = static_cast<Package*>(clone);
         Package* o = static_cast<Package*>(this);
         clone_init_package(c,o);
      }
      else if (classname == "Import") {
         clone = new Import();
         Import* c = static_cast<Import*>(clone);
         Import* o = static_cast<Import*>(this);
         clone_init_import(c,o);
      }
      else if (classname == "Ili1Format") {
         clone = new Ili1Format();
         Ili1Format* c = static_cast<Ili1Format*>(clone);
         Ili1Format* o = static_cast<Ili1Format*>(this);
         clone_init_ili1format(c,o);
      }
      else if (classname == "Model") {
         clone = new Model();
         Model* c = static_cast<Model*>(clone);
         Model* o = static_cast<Model*>(this);
         clone_init_model(c,o);
      }
      else if (classname == "SubModel") {
         clone = new SubModel();
         SubModel* c = static_cast<SubModel*>(clone);
         SubModel* o = static_cast<SubModel*>(this);
         clone_init_submodel(c,o);
      }
      else if (classname == "Type") {
         clone = new Type();
         Type* c = static_cast<Type*>(clone);
         Type* o = static_cast<Type*>(this);
         clone_init_type(c,o);
      }
      else if (classname == "Multiplicity") {
         clone = new Multiplicity();
         Multiplicity* c = static_cast<Multiplicity*>(clone);
         Multiplicity* o = static_cast<Multiplicity*>(this);
         clone_init_multiplicity(c,o);
      }
      else if (classname == "Constraint") {
         clone = new Constraint();
         Constraint* c = static_cast<Constraint*>(clone);
         Constraint* o = static_cast<Constraint*>(this);
         clone_init_constraint(c,o);
      }
      else if (classname == "DomainType") {
         clone = new DomainType();
         DomainType* c = static_cast<DomainType*>(clone);
         DomainType* o = static_cast<DomainType*>(this);
         clone_init_domaintype(c,o);
      }
      else if (classname == "Class") {
         clone = new Class();
         Class* c = static_cast<Class*>(clone);
         Class* o = static_cast<Class*>(this);
         clone_init_class(c,o);
      }
      else if (classname == "AttrOrParam") {
         clone = new AttrOrParam();
         AttrOrParam* c = static_cast<AttrOrParam*>(clone);
         AttrOrParam* o = static_cast<AttrOrParam*>(this);
         clone_init_attrorparam(c,o);
      }
      else if (classname == "TypeRelatedType") {
         clone = new TypeRelatedType();
         TypeRelatedType* c = static_cast<TypeRelatedType*>(clone);
         TypeRelatedType* o = static_cast<TypeRelatedType*>(this);
         clone_init_typerelatedtype(c,o);
      }
      else if (classname == "MultiValue") {
         clone = new MultiValue();
         MultiValue* c = static_cast<MultiValue*>(clone);
         MultiValue* o = static_cast<MultiValue*>(this);
         clone_init_multivalue(c,o);
      }
      else if (classname == "ClassRelatedType") {
         clone = new ClassRelatedType();
         ClassRelatedType* c = static_cast<ClassRelatedType*>(clone);
         ClassRelatedType* o = static_cast<ClassRelatedType*>(this);
         clone_init_classrelatedtype(c,o);
      }
      else if (classname == "ReferenceType") {
         clone = new ReferenceType();
         ReferenceType* c = static_cast<ReferenceType*>(clone);
         ReferenceType* o = static_cast<ReferenceType*>(this);
         clone_init_referencetype(c,o);
      }
      else if (classname == "Role") {
         clone = new Role();
         Role* c = static_cast<Role*>(clone);
         Role* o = static_cast<Role*>(this);
         clone_init_role(c,o);
      }
      else if (classname == "ExplicitAssocAccess") {
         clone = new ExplicitAssocAccess();
         ExplicitAssocAccess* c = static_cast<ExplicitAssocAccess*>(clone);
         ExplicitAssocAccess* o = static_cast<ExplicitAssocAccess*>(this);
         clone_init_explicitassocaccess(c,o);
      }
      else if (classname == "AssocAcc") {
         clone = new AssocAcc();
         AssocAcc* c = static_cast<AssocAcc*>(clone);
         AssocAcc* o = static_cast<AssocAcc*>(this);
         clone_init_assocacc(c,o);
      }
      else if (classname == "TransferElement") {
         clone = new TransferElement();
         TransferElement* c = static_cast<TransferElement*>(clone);
         TransferElement* o = static_cast<TransferElement*>(this);
         clone_init_transferelement(c,o);
      }
      else if (classname == "Ili1TransferElement") {
         clone = new Ili1TransferElement();
         Ili1TransferElement* c = static_cast<Ili1TransferElement*>(clone);
         Ili1TransferElement* o = static_cast<Ili1TransferElement*>(this);
         clone_init_ili1transferelement(c,o);
      }
      else if (classname == "DataUnit") {
         clone = new DataUnit();
         DataUnit* c = static_cast<DataUnit*>(clone);
         DataUnit* o = static_cast<DataUnit*>(this);
         clone_init_dataunit(c,o);
      }
      else if (classname == "Dependency") {
         clone = new Dependency();
         Dependency* c = static_cast<Dependency*>(clone);
         Dependency* o = static_cast<Dependency*>(this);
         clone_init_dependency(c,o);
      }
      else if (classname == "AllowedInBasket") {
         clone = new AllowedInBasket();
         AllowedInBasket* c = static_cast<AllowedInBasket*>(clone);
         AllowedInBasket* o = static_cast<AllowedInBasket*>(this);
         clone_init_allowedinbasket(c,o);
      }
      else if (classname == "Context") {
         clone = new Context();
         Context* c = static_cast<Context*>(clone);
         Context* o = static_cast<Context*>(this);
         clone_init_context(c,o);
      }
      else if (classname == "GenericDef") {
         clone = new GenericDef();
         GenericDef* c = static_cast<GenericDef*>(clone);
         GenericDef* o = static_cast<GenericDef*>(this);
         clone_init_genericdef(c,o);
      }
      else if (classname == "Unit") {
         clone = new Unit();
         Unit* c = static_cast<Unit*>(clone);
         Unit* o = static_cast<Unit*>(this);
         clone_init_unit(c,o);
      }
      else if (classname == "MetaBasketDef") {
         clone = new MetaBasketDef();
         MetaBasketDef* c = static_cast<MetaBasketDef*>(clone);
         MetaBasketDef* o = static_cast<MetaBasketDef*>(this);
         clone_init_metabasketdef(c,o);
      }
      else if (classname == "MetaObjectDef") {
         clone = new MetaObjectDef();
         MetaObjectDef* c = static_cast<MetaObjectDef*>(clone);
         MetaObjectDef* o = static_cast<MetaObjectDef*>(this);
         clone_init_metaobjectdef(c,o);
      }
      else if (classname == "BooleanType") {
         clone = new BooleanType();
         BooleanType* c = static_cast<BooleanType*>(clone);
         BooleanType* o = static_cast<BooleanType*>(this);
         clone_init_booleantype(c,o);
      }
      else if (classname == "TextType") {
         clone = new TextType();
         TextType* c = static_cast<TextType*>(clone);
         TextType* o = static_cast<TextType*>(this);
         clone_init_texttype(c,o);
      }
      else if (classname == "BlackboxType") {
         clone = new BlackboxType();
         BlackboxType* c = static_cast<BlackboxType*>(clone);
         BlackboxType* o = static_cast<BlackboxType*>(this);
         clone_init_blackboxtype(c,o);
      }
      else if (classname == "NumType") {
         clone = new NumType();
         NumType* c = static_cast<NumType*>(clone);
         NumType* o = static_cast<NumType*>(this);
         clone_init_numtype(c,o);
      }
      else if (classname == "CoordType") {
         clone = new CoordType();
         CoordType* c = static_cast<CoordType*>(clone);
         CoordType* o = static_cast<CoordType*>(this);
         clone_init_coordtype(c,o);
      }
      else if (classname == "AxisSpec") {
         clone = new AxisSpec();
         AxisSpec* c = static_cast<AxisSpec*>(clone);
         AxisSpec* o = static_cast<AxisSpec*>(this);
         clone_init_axisspec(c,o);
      }
      else if (classname == "NumsRefSys") {
         clone = new NumsRefSys();
         NumsRefSys* c = static_cast<NumsRefSys*>(clone);
         NumsRefSys* o = static_cast<NumsRefSys*>(this);
         clone_init_numsrefsys(c,o);
      }
      else if (classname == "FormattedType") {
         clone = new FormattedType();
         FormattedType* c = static_cast<FormattedType*>(clone);
         FormattedType* o = static_cast<FormattedType*>(this);
         clone_init_formattedtype(c,o);
      }
      else if (classname == "AnyOIDType") {
         clone = new AnyOIDType();
         AnyOIDType* c = static_cast<AnyOIDType*>(clone);
         AnyOIDType* o = static_cast<AnyOIDType*>(this);
         clone_init_anyoidtype(c,o);
      }
      else if (classname == "FunctionDef") {
         clone = new FunctionDef();
         FunctionDef* c = static_cast<FunctionDef*>(clone);
         FunctionDef* o = static_cast<FunctionDef*>(this);
         clone_init_functiondef(c,o);
      }
      else if (classname == "Argument") {
         clone = new Argument();
         Argument* c = static_cast<Argument*>(clone);
         Argument* o = static_cast<Argument*>(this);
         clone_init_argument(c,o);
      }
      else if (classname == "ClassRefType") {
         clone = new ClassRefType();
         ClassRefType* c = static_cast<ClassRefType*>(clone);
         ClassRefType* o = static_cast<ClassRefType*>(this);
         clone_init_classreftype(c,o);
      }
      else if (classname == "ObjectType") {
         clone = new ObjectType();
         ObjectType* c = static_cast<ObjectType*>(clone);
         ObjectType* o = static_cast<ObjectType*>(this);
         clone_init_objecttype(c,o);
      }
      else if (classname == "AttributeRefType") {
         clone = new AttributeRefType();
         AttributeRefType* c = static_cast<AttributeRefType*>(clone);
         AttributeRefType* o = static_cast<AttributeRefType*>(this);
         clone_init_attributereftype(c,o);
      }
      else if (classname == "ARefRestriction") {
         clone = new ARefRestriction();
         ARefRestriction* c = static_cast<ARefRestriction*>(clone);
         ARefRestriction* o = static_cast<ARefRestriction*>(this);
         clone_init_arefrestriction(c,o);
      }
      else if (classname == "EnumType") {
         clone = new EnumType();
         EnumType* c = static_cast<EnumType*>(clone);
         EnumType* o = static_cast<EnumType*>(this);
         clone_init_enumtype(c,o);
      }
      else if (classname == "EnumNode") {
         clone = new EnumNode();
         EnumNode* c = static_cast<EnumNode*>(clone);
         EnumNode* o = static_cast<EnumNode*>(this);
         clone_init_enumnode(c,o);
      }
      else if (classname == "EnumTreeValueType") {
         clone = new EnumTreeValueType();
         EnumTreeValueType* c = static_cast<EnumTreeValueType*>(clone);
         EnumTreeValueType* o = static_cast<EnumTreeValueType*>(this);
         clone_init_enumtreevaluetype(c,o);
      }
      else if (classname == "LineForm") {
         clone = new LineForm();
         LineForm* c = static_cast<LineForm*>(clone);
         LineForm* o = static_cast<LineForm*>(this);
         clone_init_lineform(c,o);
      }
      else if (classname == "LineType") {
         clone = new LineType();
         LineType* c = static_cast<LineType*>(clone);
         LineType* o = static_cast<LineType*>(this);
         clone_init_linetype(c,o);
      }
      else if (classname == "LinesForm") {
         clone = new LinesForm();
         LinesForm* c = static_cast<LinesForm*>(clone);
         LinesForm* o = static_cast<LinesForm*>(this);
         clone_init_linesform(c,o);
      }
      else if (classname == "View") {
         clone = new View();
         View* c = static_cast<View*>(clone);
         View* o = static_cast<View*>(this);
         clone_init_view(c,o);
      }
      else if (classname == "RenamedBaseView") {
         clone = new RenamedBaseView();
         RenamedBaseView* c = static_cast<RenamedBaseView*>(clone);
         RenamedBaseView* o = static_cast<RenamedBaseView*>(this);
         clone_init_renamedbaseview(c,o);
      }
      else if (classname == "Expression") {
         clone = new Expression();
         Expression* c = static_cast<Expression*>(clone);
         Expression* o = static_cast<Expression*>(this);
         clone_init_expression(c,o);
      }
      else if (classname == "UnaryExpr") {
         clone = new UnaryExpr();
         UnaryExpr* c = static_cast<UnaryExpr*>(clone);
         UnaryExpr* o = static_cast<UnaryExpr*>(this);
         clone_init_unaryexpr(c,o);
      }
      else if (classname == "CompoundExpr") {
         clone = new CompoundExpr();
         CompoundExpr* c = static_cast<CompoundExpr*>(clone);
         CompoundExpr* o = static_cast<CompoundExpr*>(this);
         clone_init_compoundexpr(c,o);
      }
      else if (classname == "Factor") {
         clone = new Factor();
         Factor* c = static_cast<Factor*>(clone);
         Factor* o = static_cast<Factor*>(this);
         clone_init_factor(c,o);
      }
      else if (classname == "PathEl") {
         clone = new PathEl();
         PathEl* c = static_cast<PathEl*>(clone);
         PathEl* o = static_cast<PathEl*>(this);
         clone_init_pathel(c,o);
      }
      else if (classname == "PathOrInspFactor") {
         clone = new PathOrInspFactor();
         PathOrInspFactor* c = static_cast<PathOrInspFactor*>(clone);
         PathOrInspFactor* o = static_cast<PathOrInspFactor*>(this);
         clone_init_pathorinspfactor(c,o);
      }
      else if (classname == "EnumAssignment") {
         clone = new EnumAssignment();
         EnumAssignment* c = static_cast<EnumAssignment*>(clone);
         EnumAssignment* o = static_cast<EnumAssignment*>(this);
         clone_init_enumassignment(c,o);
      }
      else if (classname == "EnumMapping") {
         clone = new EnumMapping();
         EnumMapping* c = static_cast<EnumMapping*>(clone);
         EnumMapping* o = static_cast<EnumMapping*>(this);
         clone_init_enummapping(c,o);
      }
      else if (classname == "ClassRef") {
         clone = new ClassRef();
         ClassRef* c = static_cast<ClassRef*>(clone);
         ClassRef* o = static_cast<ClassRef*>(this);
         clone_init_classref(c,o);
      }
      else if (classname == "ActualArgument") {
         clone = new ActualArgument();
         ActualArgument* c = static_cast<ActualArgument*>(clone);
         ActualArgument* o = static_cast<ActualArgument*>(this);
         clone_init_actualargument(c,o);
      }
      else if (classname == "FunctionCall") {
         clone = new FunctionCall();
         FunctionCall* c = static_cast<FunctionCall*>(clone);
         FunctionCall* o = static_cast<FunctionCall*>(this);
         clone_init_functioncall(c,o);
      }
      else if (classname == "RuntimeParamRef") {
         clone = new RuntimeParamRef();
         RuntimeParamRef* c = static_cast<RuntimeParamRef*>(clone);
         RuntimeParamRef* o = static_cast<RuntimeParamRef*>(this);
         clone_init_runtimeparamref(c,o);
      }
      else if (classname == "Constant") {
         clone = new Constant();
         Constant* c = static_cast<Constant*>(clone);
         Constant* o = static_cast<Constant*>(this);
         clone_init_constant(c,o);
      }
      else if (classname == "ClassConst") {
         clone = new ClassConst();
         ClassConst* c = static_cast<ClassConst*>(clone);
         ClassConst* o = static_cast<ClassConst*>(this);
         clone_init_classconst(c,o);
      }
      else if (classname == "AttributeConst") {
         clone = new AttributeConst();
         AttributeConst* c = static_cast<AttributeConst*>(clone);
         AttributeConst* o = static_cast<AttributeConst*>(this);
         clone_init_attributeconst(c,o);
      }
      else if (classname == "UnitRef") {
         clone = new UnitRef();
         UnitRef* c = static_cast<UnitRef*>(clone);
         UnitRef* o = static_cast<UnitRef*>(this);
         clone_init_unitref(c,o);
      }
      else if (classname == "UnitFunction") {
         clone = new UnitFunction();
         UnitFunction* c = static_cast<UnitFunction*>(clone);
         UnitFunction* o = static_cast<UnitFunction*>(this);
         clone_init_unitfunction(c,o);
      }
      else if (classname == "SimpleConstraint") {
         clone = new SimpleConstraint();
         SimpleConstraint* c = static_cast<SimpleConstraint*>(clone);
         SimpleConstraint* o = static_cast<SimpleConstraint*>(this);
         clone_init_simpleconstraint(c,o);
      }
      else if (classname == "ExistenceDef") {
         clone = new ExistenceDef();
         ExistenceDef* c = static_cast<ExistenceDef*>(clone);
         ExistenceDef* o = static_cast<ExistenceDef*>(this);
         clone_init_existencedef(c,o);
      }
      else if (classname == "ExistenceConstraint") {
         clone = new ExistenceConstraint();
         ExistenceConstraint* c = static_cast<ExistenceConstraint*>(clone);
         ExistenceConstraint* o = static_cast<ExistenceConstraint*>(this);
         clone_init_existenceconstraint(c,o);
      }
      else if (classname == "UniqueConstraint") {
         clone = new UniqueConstraint();
         UniqueConstraint* c = static_cast<UniqueConstraint*>(clone);
         UniqueConstraint* o = static_cast<UniqueConstraint*>(this);
         clone_init_uniqueconstraint(c,o);
      }
      else if (classname == "SetConstraint") {
         clone = new SetConstraint();
         SetConstraint* c = static_cast<SetConstraint*>(clone);
         SetConstraint* o = static_cast<SetConstraint*>(this);
         clone_init_setconstraint(c,o);
      }
      else if (classname == "Graphic") {
         clone = new Graphic();
         Graphic* c = static_cast<Graphic*>(clone);
         Graphic* o = static_cast<Graphic*>(this);
         clone_init_graphic(c,o);
      }
      else if (classname == "SignParamAssignment") {
         clone = new SignParamAssignment();
         SignParamAssignment* c = static_cast<SignParamAssignment*>(clone);
         SignParamAssignment* o = static_cast<SignParamAssignment*>(this);
         clone_init_signparamassignment(c,o);
      }
      else if (classname == "CondSignParamAssignment") {
         clone = new CondSignParamAssignment();
         CondSignParamAssignment* c = static_cast<CondSignParamAssignment*>(clone);
         CondSignParamAssignment* o = static_cast<CondSignParamAssignment*>(this);
         clone_init_condsignparamassignment(c,o);
      }
      else if (classname == "DrawingRule") {
         clone = new DrawingRule();
         DrawingRule* c = static_cast<DrawingRule*>(clone);
         DrawingRule* o = static_cast<DrawingRule*>(this);
         clone_init_drawingrule(c,o);
      }
      else if (classname == "DocTextTranslation") {
         clone = new DocTextTranslation();
         DocTextTranslation* c = static_cast<DocTextTranslation*>(clone);
         DocTextTranslation* o = static_cast<DocTextTranslation*>(this);
         clone_init_doctexttranslation(c,o);
      }
      else if (classname == "METranslation") {
         clone = new METranslation();
         METranslation* c = static_cast<METranslation*>(clone);
         METranslation* o = static_cast<METranslation*>(this);
         clone_init_metranslation(c,o);
      }
      else if (classname == "Translation") {
         clone = new Translation();
         Translation* c = static_cast<Translation*>(clone);
         Translation* o = static_cast<Translation*>(this);
         clone_init_translation(c,o);
      }
      else {
         Log.internal_error("clone(): unsupported class " + classname, 1);
      }

      return clone;

   }
      
} // namespace metamodel
