#include "Ili2Output.h"
#include "../metamodel/MetaModel.h"
#include "../metamodel/MetaModelOutput.h"
#include "../util/TextWriter.h"
#include "../util/Logger.h"
#include <stdexcept>

using namespace util;
using namespace metamodel;
using namespace output;

static string get_properties(metamodel::ExtendableME* t)
{
   string properties = "";
   if (t->Abstract) {
      properties = "ABSTRACT";
   }
   if (t->Generic) {
      if (properties == "") {
         properties = "GENERIC";
      }
      else {
         properties += ",GENERIC";
      }
   }
   if (t->Final) {
      if (properties == "") {
         properties = "FINAL";
      }
      else {
         properties += ",FINAL";
      }
   }
   if (properties == "") {
      return "";
   }
   else {
      return " (" + properties + ")";
   }
}

Ili2Output::Ili2Output(metamodel::MetaModelStore &store,util::Logger &logger,
   string ili_f,string model_v)
   : MetaModelTreeVisitor(store,logger)
{
   ili_file = ili_f;
   model_version = model_v;
}

void Ili2Output::preVisit()
{

   logger().warning("INTERLIS " + model_version + " generation is not fully implemented yet");

   ili2.openFile(ili_file);
   ili2.writeln("INTERLIS " + model_version + ";");

}

void Ili2Output::postVisit()
{
   ili2.closeFile();
}

static void write_type(TextWriter &writer,Type *t);

string multiplicity_to_string(Multiplicity m)
{
   if (m.Min == -1) {
      return "";
   }
   string multiplicity = "{";
   if (m.Min == m.Max) {
      multiplicity += to_string(m.Min);
   }
   else if (m.Max == -1) {
      multiplicity += to_string(m.Min) + "..*";
   }
   else {
      multiplicity += to_string(m.Min) + ".." + to_string(m.Max);
   }
   return multiplicity + "}";
}

static void write_texttype(TextWriter &writer,TextType *t)
{

   if (t->Kind == TextType::MText) {
      writer.write(0, "MTEXT");
   }
   else if (t->Kind == TextType::Uri) {
      writer.write(0, "URI");
   }
   else {
      writer.write(0, "TEXT");
   }

   if (t->MaxLength > 0) {
      writer.write(0, "*" + to_string(t->MaxLength));
   }

}

static void write_numtype(TextWriter &writer,NumType *t)
{

   /* class NumType : public DomainType {
      // MetaElement.Name :=
      // DomainName if defined explicitly as a domain,
      // "Type" if defined within an attribute definition,
      // "C1", "C2", "C3" if defined within a coordinate type
   public:
      string Min;
      string Max;
      bool Circular = false;
      bool Clockwise = false;
      // frole from ASSOCIATION NumUnit
      Unit *Unit;
   */

   if (t->Min == "" && t->Max == "") {
      writer.write(0,"NUMERIC");
   }
   else {
      writer.write(0,t->Min + ".." + t->Max);
   }
   if (t->Circular) {
      writer.write(0," CIRCULAR");
   }
   if (t->Unit != nullptr) {
      writer.write(0," [");
      writer.write(0,get_path(t->Unit));
      writer.write(0,"]");
   }

}

static void write_coordtype(TextWriter &writer,CoordType *t)
{

   /* class CoordType : public DomainType {
   public:
      int NullAxis = 1;
      int PiHalfAxis = 2;
      bool Multi = false; // 2.4
      // role from ASSOCIATION LineCoord
      list <NumType *> Axis;
   */

   if (t->Multi) {
      writer.write(0, "MULTICOORD ");
   }
   else {
      writer.write(0, "COORD ");
   }
   bool start = true;
   for (auto a : t->Axis) {
      if (!start) {
         writer.write(0,", ");
      }
      write_numtype(writer,a);
      start = false;
   }
   if (t->NullAxis > 0) {
      writer.write(0,", ROTATION " + to_string(t->NullAxis) + "->" + to_string(t->PiHalfAxis));
   }

}

static void write_linetype(TextWriter &writer,LineType *t)
{

   /*class LineType : public DomainType {
   public:
      enum {Polyline, DirectedPolyline, Surface, Area} Kind;
      string MaxOverlap;
      bool Multi = false; // 2.4
      // role from ASSOCIATION LineCoord
      CoordType *CoordType;
      // role from ASSOCIATION LineAttr
      Class *LAStructure;
   */

   switch (t->Kind) {
      case LineType::Polyline:
         writer.write(0,"POLYLINE");
         break;
      case LineType::DirectedPolyline:
         writer.write(0,"DIRECTED POLYLINE");
         break;
      case LineType::Surface:
         writer.write(0,"SURFACE");
         break;
      case LineType::Area:
         writer.write(0,"AREA");
         break;
   }

   writer.write(0," WITH (");
   bool comma = false;
   for (auto f : t->LineForm) {
      if (comma) {
         writer.write(0,",");
      }
      writer.write(0,f->Name);
      comma = true;
   }
   writer.write(0,")");

   if (t->CoordType != nullptr) {
      writer.write(0, " VERTEX " + t->CoordType->Name);
   }
   else {
      writer.write(0, " VERTEX ???");
   }
   if (t->MaxOverlap != "") {
      writer.write(0," WITHOUT OVERLAPS>" + t->MaxOverlap);
   }
   if (t->LAStructure != nullptr) {
      writer.write(0," LINE ATTRIBUTES " + get_path(t->LAStructure));
   }

}

static void write_referencetype(TextWriter &writer,ReferenceType *t)
{

   /* class ClassRelatedType : public DomainType { // ABSTRACT
   public:
      Class *BaseClass = nullptr;
   */

   /* class ReferenceType : public ClassRelatedType {
   public:
      bool External = false;
   */

   writer.write(0, "REFERENCE TO ");
   if (t->External) {
      writer.write(0,"(EXTERNAL) ");
   }
   writer.write(0, get_path(t->_baseclass));

   if (t->_classrestriction.size() > 0) {
      writer.write(0," RESTRICTION (");
      bool semi = false;
      for (auto r: t->_classrestriction) {
         if (semi) {
            writer.write(0,";");
         }
         writer.write(0,get_path(r));
         semi = true;
      }
      writer.write(0,")");
   }
}

static void write_multivalue(TextWriter &writer,MultiValue *t)
{

   /* class TypeRelatedType : public DomainType { // ABSTRACT
   public:
      // Role from ASSOCIATION BaseType
      Type *BaseType = nullptr;
   */

   /* class MultiValue : public TypeRelatedType {
   public:
      bool Ordered = false;
      Multiplicity Multiplicity;
      list<Type *> TypeRestriction;
   */

   if (t->Multiplicity.Min == 0 && t->Multiplicity.Max == 1) {
      writer.write(0,get_path(t->BaseType));
   }
   else {
      if (t->Ordered) {
         writer.write(0,"LIST ");
      }
      else {
         writer.write(0,"BAG ");
      }
      writer.write(0,multiplicity_to_string(t->Multiplicity));
      writer.write(0," OF " + get_path(t->BaseType));
   }

   if (t->TypeRestriction.size() > 0) {
      writer.write(0," RESTRICTION (");
      bool semi = false;
      for (auto r: t->TypeRestriction) {
         if (semi) {
            writer.write(0,";");
         }
         writer.write(0,get_path(r));
         semi = true;
      }
      writer.write(0,")");
   }

}

static void write_blackboxtype(TextWriter &writer,BlackboxType *t)
{

   /* class BlackboxType : public DomainType {
   public:
      enum { Binary, Xml } Kind;
   */

   if (t->Kind == BlackboxType::Binary) {
      writer.write(0,"BLACKBOX BINARY");
   }
   else {
      writer.write(0,"BLACKBOX XML");
   }

}

static void write_formattedtype(TextWriter &writer,FormattedType *t)
{

   /* class FormattedType : public NumType {
   public:
      string Format;
      // role from ASSOCIATION
      Class *Struct = nullptr;
      FormattedType* BaseFormattedType = nullptr;
   */

   if (t->Struct != nullptr) {
      writer.write(0,"FORMAT BASED ON " + get_path(t->Struct) + " (" + t->Format + ")");
   }
   else if (t->BaseFormattedType != nullptr) {
      writer.write(0,"FORMAT " + get_path(t->BaseFormattedType) + " \"" + t->Min + "\" .. \"" + t->Max + "\"");
   }
   else {
      writer.write(0,"\"" + t->Min + "\" .. \"" + t->Max + "\"");
   }

}

static void write_type(TextWriter &writer,Type *t)
{

   try {
      if (t->getClass() == "TextType") {
         write_texttype(writer,static_cast<TextType *>(t));
      }
      else if (t->getClass() == "BooleanType") {
         writer.write(0,"BOOLEAN");
      }
      else if (t->getClass() == "CoordType") {
         write_coordtype(writer,static_cast<CoordType *>(t));
      }
      else if (t->getClass() == "EnumType") {
         write_enumtype(&writer,static_cast<EnumType *>(t));
      }
      else if (t->getClass() == "NumType") {
         write_numtype(writer,static_cast<NumType *>(t));
      }
      else if (t->getClass() == "LineType") {
         write_linetype(writer,static_cast<LineType *>(t));
      }
      else if (t->getClass() == "ReferenceType") {
         write_referencetype(writer,static_cast<ReferenceType *>(t));
      }
      else if (t->getClass() == "MultiValue") {
         write_multivalue(writer,static_cast<MultiValue *>(t));
      }
      else if (t->getClass() == "FormattedType") {
         write_formattedtype(writer,static_cast<FormattedType *>(t));
      }
      else if (t->getClass() == "BlackboxType") {
         write_blackboxtype(writer,static_cast<BlackboxType *>(t));
      }
      else if (t->getClass() == "AnyOIDType") {
         // to do !!!
      }
      else if (t->getClass() == "ReferenceType") {
         // to do !!!
      }
      else if (t->getClass() == "AttributeRefType") {
         AttributeRefType *attributeRef = static_cast<AttributeRefType *>(t);
         writer.write(0,"ATTRIBUTE");
         if (attributeRef->AttrRestriction != nullptr) {
            writer.write(0," OF ");
            write_expression(&writer,attributeRef->AttrRestriction);
         }
         if (!attributeRef->TypeRestriction.empty()) {
            writer.write(0," RESTRICTION (");
            bool separator = false;
            for (Type *restriction : attributeRef->TypeRestriction) {
               if (separator) {
                  writer.write(0," : ");
               }
               if (restriction->ElementInPackage != nullptr) {
                  writer.write(0,get_path(restriction));
               }
               else {
                  write_type(writer,static_cast<DomainType *>(restriction));
               }
               separator = true;
            }
            writer.write(0,")");
         }
      }
      else if (t->getClass() == "ClassRefType") {
         writer.write(0,"CLASS");
      }
      else if (t->getClass() == "EnumTreeValueType") {
         EnumTreeValueType* tt = static_cast<EnumTreeValueType*>(t);
         writer.write(0,"ALL OF " + get_path(tt->ET));
      }
      else if (t->getClass() == "ObjectType") {
         ObjectType *o = static_cast<ObjectType *>(t);
         if (o->Multiple) {
            writer.write(0,"OBJECTS OF " + get_path(o->_baseclass));
         }
         else {
            writer.write(0,"OBJECT OF " + get_path(o->_baseclass));
         }
      }
      else {
         throw std::runtime_error("write_type(): <" + t->getClass() + ">");
      }
   }
   catch (exception e) {
      throw;
   }

}

void Ili2Output::preVisitModel(Model *m)
{

   /* class Model : public Package {
   public:
      string iliVersion;
      bool Contracted = false;
      enum { NormalM, TypeM, RefSystemM, SymbologyM } Kind;
      string Language;
      string At;
      string Version;
      string VersionExplanation;
      bool NoIncrementalTransfer = true; // 2.4
      string CharSetIANAName; // 2.4
      string xmlns; // 2.4
      string ili1Transfername;
      Ili1Format *ili1Format;
   */

   /* MODEL RoadsExdm2ben (de)
      AT "http://www.interlis.ch/models"
      VERSION "2005-06-16"  =
   */

   if (m->Name == "INTERLIS") {
      ignoreVisit();
   }

   ili2.writeln("");
   ili2.write("");
   if (m->Contracted) {
      ili2.write(0,"CONTRACTED ");
   }
   if (m->Kind == Model::TypeM) {
      ili2.write(0,"TYPE ");
   }
   else if (m->Kind == Model::RefSystemM) {
      ili2.write(0,"REFSYSTEM ");
   }
   else if (m->Kind == Model::SymbologyM) {
      ili2.write(0,"SYMBOLOGY ");
   }
   ili2.writeln(0,"MODEL " + m->Name + " (" + m->Language + ")");
   ili2.incNestLevel();
   ili2.writeln("AT \"" + m->At + "\"");
   ili2.write("VERSION \"" + m->Version + "\"");
   if (!m->_translationOfName.empty()) {
      ili2.write(" TRANSLATION OF " + m->_translationOfName
         + " [\"" + m->_translationOfVersion + "\"]");
   }
   ili2.writeln(" =");

}

void Ili2Output::visitModel(Model *m)
{

   if (m->Name == "INTERLIS") {
      ignoreVisit();
   }

   // imports
   for (auto i : store().imports()) {
      if (i->ImportingP->Name == m->Name) {
         visit(i);
      }
   }

   if (m->_runtimeparameter.size() > 0) {
      ili2.writeln("");
      ili2.writeln("PARAMETER");
      ili2.incNestLevel();
      for (auto p: m->_runtimeparameter) {
         ili2.write(p->Name);
         ili2.write(0,": TEXT;");
//         visitAttrOrParam(p); to do !!
      }
      ili2.decNestLevel();
      ili2.writeln("");
   }

   bool indomain = false;
   bool inunit = false;

   for (auto e: m->Element) {
      if (e->ElementInPackage == nullptr) {
         continue;
      }
      if (e->isSubClassOf("Unit")) {
         if (!inunit) {
            ili2.writeln("");
            ili2.writeln("UNIT");
         }
         indomain = false;
         inunit = true;
         visit(e);
      }
      else if (e->isSubClassOf("DomainType")) {
         if (!indomain) {
            ili2.writeln("");
            ili2.writeln("DOMAIN");
         }
         indomain = true;
         inunit = false;
         visit(e);
      }
      else {
         visit(e);
         indomain = false;
         inunit = false;
      }
   }

   ignoreVisit();

}

void Ili2Output::postVisitModel(Model *m)
{

   if (m->Name == "INTERLIS") {
      ignoreVisit();
   }

   ili2.decNestLevel();
   ili2.writeln("");
   ili2.writeln("END " + m->Name + ".");

}

void Ili2Output::visitImport(metamodel::Import *i)
{

   /* class Import : public MMObject { // ASSOCIATION
   public:
      Package *ImportingP;
      Package *ImportedP;
      bool _unqualified = false;
   */

   /* importDef
   : IMPORTS importing (COMMA importing)* SEMI

   importing
   : UNQUALIFIED? (importname=path)
   */

   ili2.write("IMPORTS ");
   if (i->_unqualified) {
      ili2.writeNoIdent("UNQUALIFIED ");
   }
   ili2.writelnNoIdent(i->ImportedP->Name + ";");

}

void Ili2Output::preVisitSubModel(SubModel *s)
{

   ili2.writeln("");
   ili2.write("TOPIC " + s->Name);
   if (s->_dataunit->Abstract) {
      ili2.write(0," (ABSTRACT)");
   }
   if (s->_super != nullptr) {
      ili2.write(0," EXTENDS " + get_path(s->_super));
   }
   ili2.writeln(0," =");

   for (auto d : store().dependencies()) {
      ili2.incNestLevel();
      if (d->Using == s->_dataunit) {
         ili2.writeln("DEPENDS ON " + get_parent_path(d->Dependent) + ";");
      }
      ili2.decNestLevel();
   }

   ili2.incNestLevel();

   if (!s->DeferredGenerics.empty()) {
      ili2.write("DEFERRED GENERICS ");
      bool comma = false;
      for (const SubModel::DeferredGenericRef &reference : s->DeferredGenerics) {
         if (comma) {
            ili2.write(0,", ");
         }
         ili2.write(0,reference.Domain == nullptr ? reference.Name : get_path(reference.Domain));
         comma = true;
      }
      ili2.writeln(0,";");
   }

}

void Ili2Output::visitSubModel(SubModel *s)
{

   bool indomain = false;
   bool inunit = false;

   for (auto e : s->Element) {
      if (e->ElementInPackage == nullptr) {
         continue;
      }
      if (e->isSubClassOf("Unit")) {
         if (!inunit) {
            ili2.writeln("");
            ili2.writeln("UNIT");
         }
         indomain = false;
         inunit = true;
         visit(e);
      }
      else if (e->isSubClassOf("DomainType")) {
         if (!indomain) {
            ili2.writeln("");
            ili2.writeln("DOMAIN");
         }
         indomain = true;
         inunit = false;
         visit(e);
      }
      else {
         visit(e);
         indomain = false;
         inunit = false;
      }
   }

   ignoreVisit();

}

void Ili2Output::postVisitSubModel(SubModel *s)
{

   ili2.decNestLevel();
   ili2.writeln("");
   ili2.writeln("END " + s->Name + ";");

}

void Ili2Output::preVisitClass(Class *c)
{

   /* class Class : public Type {
      // MetaElement.Name := StructureName, ClassName,
      //                     AssociationName, ViewName
      //                     as defined in the INTERLIS-Model
   public:
      enum {Structure,ClassVal,ViewVal,Association} Kind;
      Multiplicity Multiplicity; // for associations only
      list<Constraint *> Constraints;
      bool EmbeddedRoleTransfer = false;
      bool ili1OptionalTable = false;
      // role from ASSOCIATION ClassAttr
      list<metamodel::AttrOrParam *> ClassAttribute;
      // role from ASSOCIATION AssocRole
      list<Role *> Role;
      // role from ExplicitAssocAcc
      list<ExplicitAssocAccess *> ExplicitAssocAccess;
      // role from ASSOCIATION MetaObjectClass
      list<MetaObjectDef *> MetaObjectDef;
      // role from ASSOCIATION StructOfFormat
      list<FormattedType *> FormattedType;
      // role form ASSOCIATION ObjectOID
      DomainType *Oid = nullptr; // RESTRICTION(TextType; NumType; AnyOIDType)
      // role from ASSOCIATION ARefOf
      list<AttributeRefType *> ForARef;
      // role from ASSOCIATION LineFormStructure
      list<LineForm *> LineForm;
      // role from ASSOCIATION LineAttr
      list<LineType *> LineType;
      // role from ASSOCIATION BaseViewRef
      list<RenamedBaseView *> RenamedBaseView;
      // role from ASSOCIATION DerivedAssoc
      View *View = nullptr;
      // role from ASSOCIATION GraphicBase
      //list<Graphic *> Graphic;
      // role from ASSOCIATION SignClass
      list <DrawingRule *> DrawingRule;
      // from from ASSOCIATION ClassConstraint
      list<Constraint *> Constraint;
   */

   currentClass_ = c;

   ili2.writeln("");

   if (c->Kind == Class::ClassVal) {
      ili2.write("CLASS " + c->Name);
   }
   else if (c->Kind == Class::Structure) {
      ili2.write("STRUCTURE " + c->Name);
   }
   else if (c->Kind == Class::ViewVal) {
      View *v = static_cast<View *>(c);
      ili2.write("VIEW " + v->Name);
      if (v->FormationKind == View::Projection) {
         ili2.write(0," PROJECTION OF ");
         bool comma = false;
         for (auto a: v->ClassAttribute) {
            if (a->Type == nullptr) {
               continue;
            }
            if (a->Type->getClass() != "ObjectType") {
               continue;
            }
            if (comma) {
               ili2.write(0,",");
            }
            ObjectType *o = static_cast<ObjectType *>(a->Type);
            ili2.write(0,a->Name);
            if (a->Name != o->_baseclass->Name) {
               ili2.write(0,"~" + get_path(o->_baseclass));
            }
            comma = true;
         }
      }
      ili2.write(0,";");
   }
   else if (c->Kind == Class::Association) {
      if (c->Name == "???") {
         ili2.write("ASSOCIATION");
      }
      else {
         ili2.write("ASSOCIATION " + c->Name);
      }
   }

   ili2.write(0,get_properties(c));

   if (c->Super != nullptr) {
      bool extended = false;
      if (c->Super->Name == c->Name) {
         Package *p = c->ElementInPackage == nullptr ? nullptr : c->ElementInPackage->_super;
         while (p != nullptr) {
            if (p == c->Super->ElementInPackage) {
               extended = true;
               break;
            }
            p = p->_super;
         }
      }
      if (extended) {
         ili2.write(0," (EXTENDED)");
      }
      else {
         ili2.write(0," EXTENDS " + get_path(c->Super));
      }
   }
   ili2.writeln(0," =");

   firstClassParam = true;

   ili2.incNestLevel();

   if (c->Oid != nullptr) {
      ili2.writeln("OID AS " + get_path(c->Oid) + ";");
   }

}

void Ili2Output::postVisitClass(Class *c)
{

   ili2.decNestLevel();
   if (c->Name == "???") {
      ili2.write("END;");
   }
   else {
      ili2.writeln("END " + c->Name + ";");
   }
   currentClass_ = nullptr;

   if (model_version == "2.3" && c->Kind != Class::Structure) {
      for (auto a : c->ClassAttribute) {
         if (a->Type->getClass() == "ReferenceType") {
            ReferenceType *rt = static_cast<ReferenceType *>(a->Type);
            string assocname = c->Name + "_" + a->Name;
            ili2.writeln("");
            ili2.writeln("ASSOCIATION " + assocname + " =");
            ili2.incNestLevel();
            ili2.writeln(a->Name + " -- {1} " + get_path(rt->_baseclass) + ";");
            ili2.writeln(c->Name + " -- {0..*} " + get_path(c) + ";");
            ili2.decNestLevel();
            ili2.writeln("END " + assocname + ";");
         }
      }
   }

}

void Ili2Output::visitSimpleConstraint(metamodel::SimpleConstraint *c)
{

   /* class SimpleConstraint : public Constraint {
   public:
      enum {MandC, LowPercC, HighPercC} Kind;
      double Percentage = 100.0;
      Expression *LogicalExpression = nullptr;
      enum {Equal, LessEqual, GreaterEqual} _percentage_operation = Equal;
   */

   // Domain constraints are emitted inline by visitDomainType.
   if (c->toDomain != nullptr) {
      return;
   }

   switch (c->Kind) {
      case SimpleConstraint::MandC:
         ili2.writeln("MANDATORY CONSTRAINT");
         ili2.incNestLevel();
         ili2.write("");write_expression(&ili2,c->LogicalExpression);
         ili2.writeln(0,";");
         ili2.decNestLevel();
         break;
      case SimpleConstraint::LowPercC:
         ili2.writeln("PLAUSIBILITY CONSTRAINT");
         ili2.incNestLevel();
         ili2.write("");write_expression(&ili2,c->LogicalExpression);
         ili2.writeln(0,";");
         ili2.decNestLevel();
         break;
      case SimpleConstraint::HighPercC: // High vs. Low ???, to do !!!
         ili2.writeln("PLAUSIBILITY CONSTRAINT");
         ili2.incNestLevel();
         ili2.write("");write_expression(&ili2,c->LogicalExpression);
         ili2.writeln(0,";");
         ili2.decNestLevel();
         break;
      default:
         logger().internal_error("visitSimpleConstraint(): unknown Kind id " + to_string(c->Kind));
   }

}

void Ili2Output::visitUniqueConstraint(metamodel::UniqueConstraint *c)
{

   /* class UniqueConstraint : public Constraint {
   public:
      list<Expression *> Where;
      enum {GlobalU, LocalU} Kind;
      list<PathOrInspFactor *> UniqueDef;
   */

   /* struct PathOrInspFactor : public Factor {
   public:
      list <PathEl *> PathEls; // LIST
      View *Inspection = nullptr;
      string _path = "";
   */

   if (currentClass_ != nullptr && currentClass_->Kind != Class::Structure && model_version == "2.3") {
      for (auto f: c->UniqueDef) {
         for (auto p: f->PathEls) {
            if (p->Ref == nullptr) {
               continue;
            }
            AttrOrParam *a = static_cast<AttrOrParam *>(p->Ref);
            if (a->Type == nullptr) {
               continue;
            }
            if (a->Type->getClass() == "ReferenceType") {
               // this attribute was moved to association
               return;
            }
         }
      }
   }

   ili2.decNestLevel();
   ili2.write("UNIQUE ");

   if (c->PerBasket) {
      ili2.write(0,"(BASKET) ");
   }

   if (!c->Name.empty()) {
      ili2.write(0,c->Name + ": ");
   }

   if (c->Kind == UniqueConstraint::LocalU) {
      ili2.write(0,"(LOCAL) ");
   }

   bool comma = false;
   for (auto f: c->UniqueDef) {
      if (comma) {
         ili2.write(0,",");
      }
      ili2.write(0,f->_path);
      comma = true;
   }

   ili2.writeln(0,";");
   ili2.incNestLevel();

}

void Ili2Output::visitAttrOrParam(AttrOrParam *a)
{

   /* class AttrOrParam : public ExtendableME {
      // MetaElement.Name := AttributeName, ParameterName
      //                    as defined in the INTERLIS-Model
   public:
      enum { NoSubDiv, SubDiv, ContSubDiv } SubdivisionKind;
      bool Transient = false;
      list<Expression *> Derivates;
      // ROLE from ASSOCIATION LocalType
      list<Type *> LocalType;
      // ROLE from ASSOCIATION AttrOrParamType
      Type *Type;
      // ROLE from ASSOCIATION ClassAttr
      Class *AttrParent;
      virtual string getClass() { return "AttrOrParam"; }
      virtual string getBaseClass() { return "ExtendableME"; };
   */

   if (!a->_visible) {
      return;
   }

   if (a->ParamParent != nullptr && firstClassParam) {
      ili2.writeln("PARAMETER ");
      firstClassParam = false;
   }

   if (a->Type != nullptr) {
      if (a->Type->getClass() == "ObjectType" && currentClass_ != nullptr && currentClass_->getClass() == "View") {
         ObjectType *o = static_cast<ObjectType *>(a->Type);
         ili2.writeln("ALL OF " + a->Name + ";");
         return;
      }
      if (model_version == "2.3" && currentClass_ != nullptr && currentClass_->Kind == Class::ClassVal && a->Type->getClass() == "ReferenceType") {
         return;
      }
      try {
         DomainType *t = static_cast<DomainType *>(a->Type);
         if (a->Extending != nullptr) {
            ili2.write(a->Name + " (EXTENDED): ");
         }
         else {
            ili2.write(a->Name + " " + get_properties(a) + ": ");
         }
         if (t->Mandatory) {
            ili2.write(0,"MANDATORY ");
         }
         if (t->Super != nullptr) {
            ili2.write(0,get_path(t->Super));
         }
         else if (t->ElementInPackage != nullptr) {
            ili2.write(0,get_path(t));
         }
         else {
            write_type(ili2,t);
         }
         if (!a->Derivates.empty()) {
            ili2.write(0," := ");
            bool comma = false;
            for (Expression *derivate : a->Derivates) {
               if (comma) {
                  ili2.write(0,", ");
               }
               write_expression(&ili2,derivate);
               comma = true;
            }
         }
         ili2.writeln(0,";");
      }
      catch (exception e) {
         logger().error("unable to cast " + a->Type->getClass() + " to DomainType");
      }
   }
   else {
      ili2.writeln(a->Name + ": ???;");
   }

}

void Ili2Output::visitRole(Role *r)
{

   /* class Role : public ReferenceType {
      // MetaElement.Name := RoleName as defined in the INTERLIS-Model
   public:
      enum { Assoc, Aggr, Comp } Strongness;
      bool Ordered = false;
      Multiplicity Multiplicity;
      list<Expression *> Derivates;
      bool EmbeddedTransfer = false;
      // role from ASSOCIATION AssocRole
      Class *Association;
      // role from ASSOCIATION AssocAccOrign
      list<ExplicitAssocAccess *> UseAsOrigin;
      // role from ASSOCIATION AssocAccTarget
      list<ExplicitAssocAccess *> UseAsTarget;
   */

   /* struct Multiplicity : public MMObject {
   public:
      int Min;
      int Max;
   */

   /* ASSOCIATION StreetAxisAssoc =
      Street -- {1} Street;
      StreetAxis -- {0..*} StreetAxis;
   END StreetAxisAssoc;
   */

   string strongness;
   if (r->Strongness == Role::Assoc) {
      strongness = "--";
   }
   else if (r->Strongness == Role::Aggr) {
      strongness = "-<>";
   }
   else { // Role::Comp
      strongness = "-<#>";
   }

   string target = get_path(r->_baseclass);

   ili2.writeln(r->Name + " " + strongness + " " + multiplicity_to_string(r->Multiplicity) + " " + target + ";");

}

void Ili2Output::visitUnit(metamodel::Unit* u)
{

   /*
   class Unit : public ExtendableME {
      // MetaElement.Name := ShortName as defined in the INTERLIS-Model
   public:
      enum { BaseU, DerivedU, ComposedU } Kind;
      Expression* Definition;
      // role from ASSOCIATION NumUnit
      list <NumType*> Num;
      string _unitshort;
   */

   /* unitDecl
      : UNIT(unitDef)*

   unitDef
      : unitname=NAME
      (LPAREN ABSTRACT RPAREN | LBRACE unitshort = path RBRACE) ?
      (EXTENDS super=path) ?
      (EQUAL (derivedUnit | composedUnit)) ?
      SEMI
   */

   if (u->_unitname != u->Name) {
      ili2.write(u->_unitname + " [" + u->Name + "]");
   }
   else {
      ili2.write(u->Name);
   }

   if (u->Abstract) {
      ili2.write(0," (ABSTRACT)");
   }

   if (u->Kind == Unit::DerivedU) {
      ili2.write(0," = ");
      write_expression(&ili2,u->Definition);
      ili2.write(0," [" + get_path(u->Super) + "]");
   }
   else if (u->Kind == Unit::ComposedU) {
      ili2.write(0," = (");
      write_expression(&ili2,u->Definition);
      ili2.write(0,")");
   }
   else if (u->Super != nullptr) {
      ili2.write(0," EXTENDS " + get_path(u->Super));
   }

   ili2.writeln(0,";");

}

void Ili2Output::visitDomainType(metamodel::DomainType* t)
{

   if (t->ElementInPackage == nullptr) {
      return;
   }

   if (currentClass_ != nullptr) {
      return;
   }

   string declaration = t->Name + get_properties(t);
   if (t->Super != nullptr) {
      ili2.write(declaration + " EXTENDS " + get_path(t->Super) + " = ");
   }
   else {
      ili2.write(declaration + " = ");
   }

   write_type(ili2,t);
   if (!t->Constraint.empty()) {
      ili2.write(0," CONSTRAINTS ");
      bool comma = false;
      for (Constraint *constraint : t->Constraint) {
         SimpleConstraint *simple = dynamic_cast<SimpleConstraint *>(constraint);
         if (simple == nullptr) {
            continue;
         }
         if (comma) {
            ili2.write(0,", ");
         }
         ili2.write(0,simple->Name + ": ");
         write_expression(&ili2,simple->LogicalExpression);
         comma = true;
      }
   }
   ili2.writeln(0,";");

}

void Ili2Output::visitContext(metamodel::Context *context)
{
   ili2.writeln("");
   ili2.writeln("CONTEXT " + context->Name + " =");
   ili2.incNestLevel();
   for (GenericDef *definition : context->GenericDefinitions) {
      if (definition == nullptr || definition->GenericDomain.empty()) {
         continue;
      }
      ili2.write(get_path(definition->GenericDomain.front()) + " = ");
      bool separator = false;
      for (DomainType *concrete : definition->ConcreteDomain) {
         if (separator) {
            ili2.write(0," OR ");
         }
         ili2.write(0,get_path(concrete));
         separator = true;
      }
      ili2.writeln(0,";");
   }
   ili2.decNestLevel();
   ignoreVisit();
}

void Ili2Output::visitFunctionDef(metamodel::FunctionDef* f)
{

   /* class FunctionDef : public MetaElement {
      // MetaElement.Name := FunctionName as defined in the INTERLIS-Model
   public:
      string Explanation;
      // role from ASSOCIATION LocalFType
      Type *LocalType = nullptr;
      Type *ResultType = nullptr;
      // role from ASSOCIATION FormalArgument
      list <Argument *> Argument;
   */

   /* class Argument : public MetaElement {
      // MetaElement.Name := ArgumentName as defined in the INTERLIS-Model
   public:
      enum { TypeVal, EnumVal, EnumTreeVal } Kind;
      // role from ASSOCIATION FormalArgument =
      FunctionDef *Function = nullptr;
      // role from ASSOCIATION ArgumentType
      Type *Type = nullptr;
   */

   ili2.write("FUNCTION " + f->Name + "(");
   bool first = true;
   for (auto a : f->Argument) {
      if (!first) {
         ili2.write(0,";");
      }
      ili2.write(0, a->Name + ": ");
      write_type(ili2,a->Type);
      first = false;
   }
   ili2.write(0, "): ");
   write_type(ili2,f->ResultType);
   ili2.writeln(0,";");

}
