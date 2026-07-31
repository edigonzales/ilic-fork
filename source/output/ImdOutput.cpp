#include "ImdOutput.h"

#include "../metamodel/MetaModelOutput.h"
#include "../util/XTFWriter.h"
#include "../util/Logger.h"
#include "../util/Converter.h"

using namespace util;
using namespace metamodel;
using namespace output;

// private helper functions

// class implementation

static string getMetaModel(string version)
{
   if (version == "2.4") {
      return "IlisMeta16";
   }
   else {
      return "IlisMeta07";
   }
}   

ImdOutput::ImdOutput(metamodel::MetaModelStore &store,util::Logger &logger,
   string xtf_file,string version)
   : MetaModelTreeVisitor(store,logger)
{
   this->xtf_file = xtf_file;
   this->version = version;
}

void ImdOutput::preVisit(void)
{
   logger().warning("imd generation not fully implemented yet");
   xtf.openFile(xtf_file,list<Model *>(store().models().begin(),store().models().end()));
}

void ImdOutput::postVisit(void)
{
   xtf.closeFile();
}

void ImdOutput::preVisitModel(Model *m)
{
   xtf.openBasket(getMetaModel(version), "ModelData", "MODEL." + m->Name);
}

void ImdOutput::postVisitModel(Model* m)
{
   xtf.closeBasket();
}

// --- from MetaModel.h generated source ---

void ImdOutput::visitDocText(DocText *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "DocText", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // DocText
   xtf.writeTag("Name", t->Name);
   xtf.writeTag("Text", t->Text);

   xtf.closeObject();

}

void ImdOutput::visitMetaAttribute(MetaAttribute *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "MetaAttribute", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaAttribute
   xtf.writeTag("Name", t->Name);
   xtf.writeTag("Value", t->Value);
   xtf.writeReference("MetaElement", get_path(t->MetaElement));

   xtf.closeObject();

}

void ImdOutput::visitImport(Import *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Import", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Import
   xtf.writeReference("ImportingP", get_path(t->ImportingP));
   xtf.writeReference("ImportedP", get_path(t->ImportedP));

   xtf.closeObject();

}

void ImdOutput::visitIli1Format(Ili1Format *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Ili1Format", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Ili1Format
   xtf.writeTag("LineSize", to_string(t->LineSize));
   xtf.writeTag("tidSize", to_string(t->tidSize));
   xtf.writeTag("blankCode", to_string_bool(t->blankCode));
   xtf.writeTag("undefinedCode", to_string_bool(t->undefinedCode));
   xtf.writeTag("continueCode", to_string_bool(t->continueCode));
   xtf.writeTag("Font", t->Font);
   switch (t->tidKind) {
      case Ili1Format::TID_I16:
         xtf.writeTag("tidKind","TID_I16");
         break;
      case Ili1Format::TID_I32:
         xtf.writeTag("tidKind","TID_I32");
         break;
      case Ili1Format::TID_ANY:
         xtf.writeTag("tidKind","TID_ANY");
         break;
      case Ili1Format::TID_EXPLANATION:
         xtf.writeTag("tidKind","TID_EXPLANATION");
         break;
      default:
         logger().error("unsupported Ili1Format.tidKind value " + to_string(t->tidKind));
         break;
   }
   xtf.writeTag("tidExplanation", t->tidExplanation);

   xtf.closeObject();

}

void ImdOutput::visitModel(Model *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Model", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // Package

   // Model
   xtf.writeTag("iliVersion", t->iliVersion);
   xtf.writeTag("Contracted", to_string_bool(t->Contracted));
   switch (t->Kind) {
      case Model::NormalM:
         xtf.writeTag("Kind","NormalM");
         break;
      case Model::TypeM:
         xtf.writeTag("Kind","TypeM");
         break;
      case Model::RefSystemM:
         xtf.writeTag("Kind","RefSystemM");
         break;
      case Model::SymbologyM:
         xtf.writeTag("Kind","SymbologyM");
         break;
      default:
         logger().error("unsupported Model.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.writeTag("Language", t->Language);
   xtf.writeTag("At", t->At);
   xtf.writeTag("Version", t->Version);
   xtf.writeTag("VersionExplanation", t->VersionExplanation);
   if (version == "2.4") {
      xtf.writeTag("NoIncrementalTransfer", to_string_bool(t->NoIncrementalTransfer));
   }
   if (version == "2.4") {
      xtf.writeTag("CharSetIANAName", t->CharSetIANAName);
   }
   if (version == "2.4") {
      xtf.writeTag("xmlns", t->xmlns);
   }
   xtf.writeTag("ili1Transfername", t->ili1Transfername);
   xtf.writeReference("ili1Format", get_path(t->ili1Format));

   xtf.closeObject();

}

void ImdOutput::visitSubModel(SubModel *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "SubModel", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // Package

   // SubModel

   xtf.closeObject();

}

void ImdOutput::visitMultiplicity(Multiplicity *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Multiplicity", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Multiplicity
   xtf.writeTag("Min", to_string(t->Min));
   xtf.writeTag("Max", to_string(t->Max));

   xtf.closeObject();

}

void ImdOutput::visitClass(Class *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Class", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // Class
   switch (t->Kind) {
      case Class::Structure:
         xtf.writeTag("Kind","Structure");
         break;
      case Class::ClassVal:
         xtf.writeTag("Kind","ClassVal");
         break;
      case Class::ViewVal:
         xtf.writeTag("Kind","ViewVal");
         break;
      case Class::Association:
         xtf.writeTag("Kind","Association");
         break;
      default:
         logger().error("unsupported Class.Kind value " + to_string(t->Kind));
         break;
   }
   if (t->Kind == Class::Association) {
      xtf.openTag("Multiplicity");
      visitMultiplicity(&t->Multiplicity);
      xtf.closeTag();
   }
   xtf.writeTag("EmbeddedRoleTransfer", to_string_bool(t->EmbeddedRoleTransfer));
   xtf.writeTag("ili1OptionalTable", to_string_bool(t->ili1OptionalTable));
   xtf.writeReference("LTParent", get_path(t->LTParent));
   xtf.writeReference("Oid", get_path(t->Oid));
   xtf.writeReference("View", get_path(t->View));

   xtf.closeObject();

}

void ImdOutput::visitAttrOrParam(AttrOrParam *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "AttrOrParam", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // AttrOrParam
   switch (t->SubdivisionKind) {
      case AttrOrParam::NoSubDiv:
         xtf.writeTag("SubdivisionKind","NoSubDiv");
         break;
      case AttrOrParam::SubDiv:
         xtf.writeTag("SubdivisionKind","SubDiv");
         break;
      case AttrOrParam::ContSubDiv:
         xtf.writeTag("SubdivisionKind","ContSubDiv");
         break;
      default:
         logger().error("unsupported AttrOrParam.SubdivisionKind value " + to_string(t->SubdivisionKind));
         break;
   }
   xtf.writeTag("Transient", to_string_bool(t->Transient));
   if (t->Derivates.size() > 0) {
      xtf.openTag("Derivates");
      for (auto tt : t->Derivates) {
         visitExpression(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("Type", get_path(t->Type));
   xtf.writeReference("AttrParent", get_path(t->AttrParent));

   xtf.closeObject();

}

void ImdOutput::visitMultiValue(MultiValue *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "MultiValue", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }

   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // TypeRelatedType
   xtf.writeReference("BaseType", get_path(t->BaseType));

   // MultiValue
   xtf.writeTag("Ordered", to_string_bool(t->Ordered));
   xtf.openTag("Multiplicity");
   visitMultiplicity(&t->Multiplicity);
   xtf.closeTag();

   xtf.closeObject();

}

void ImdOutput::visitReferenceType(ReferenceType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ReferenceType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }

   if (t->LFTParent != nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // ClassRelatedType
   xtf.writeReference("BaseClass", get_path(t->_baseclass));

   // ReferenceType
   xtf.writeTag("External", to_string_bool(t->External));

   xtf.closeObject();

}

void ImdOutput::visitRole(Role *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Role", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // ClassRelatedType
   xtf.writeReference("BaseClass", get_path(t->_baseclass));

   // ReferenceType
   xtf.writeTag("External", to_string_bool(t->External));

   // Role
   switch (t->Strongness) {
      case Role::Assoc:
         xtf.writeTag("Strongness","Assoc");
         break;
      case Role::Aggr:
         xtf.writeTag("Strongness","Aggr");
         break;
      case Role::Comp:
         xtf.writeTag("Strongness","Comp");
         break;
      default:
         logger().error("unsupported Role.Strongness value " + to_string(t->Strongness));
         break;
   }
   xtf.writeTag("Ordered", to_string_bool(t->Ordered));
   xtf.openTag("Multiplicity");
   visitMultiplicity(&t->Multiplicity);
   xtf.closeTag();
   if (t->Derivates.size() > 0) {
      xtf.openTag("Derivates");
      for (auto tt : t->Derivates) {
         visitExpression(tt);
      }
      xtf.closeTag();
   }
   xtf.writeTag("EmbeddedTransfer", to_string_bool(t->EmbeddedTransfer));
   xtf.writeReference("Association", get_path(t->Association));

   xtf.closeObject();

}

void ImdOutput::visitExplicitAssocAccess(ExplicitAssocAccess *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ExplicitAssocAccess", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // ExplicitAssocAccess
   xtf.writeReference("AssocAccOf", get_path(t->AssocAccOf));
   xtf.writeReference("OriginRole", get_path(t->OriginRole));
   xtf.writeReference("TargetRole", get_path(t->TargetRole));

   xtf.closeObject();

}

void ImdOutput::visitAssocAcc(AssocAcc *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "AssocAcc", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // AssocAcc
   xtf.writeReference("Class", get_path(t->Class));
   xtf.writeReference("AssocAcc", get_path(t->AssocAcc_));

   xtf.closeObject();

}

void ImdOutput::visitTransferElement(TransferElement *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "TransferElement", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // TransferElement
   xtf.writeReference("TransferClass", get_path(t->TransferClass));
   xtf.writeReference("TransferElement", get_path(t->TransferElement_));

   xtf.closeObject();

}

void ImdOutput::visitIli1TransferElement(Ili1TransferElement *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Ili1TransferElement", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Ili1TransferElement
   xtf.writeReference("Ili1TransferClass", get_path(t->Ili1TransferClass));
   xtf.writeReference("Ili1RefAttr", get_path(t->Ili1RefAttr));

   xtf.closeObject();

}

void ImdOutput::visitDataUnit(DataUnit *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "DataUnit", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // DataUnit
   xtf.writeTag("ViewUnit", to_string_bool(t->ViewUnit));
   xtf.writeTag("DataUnitName", t->DataUnitName);
   xtf.writeReference("Oid", get_path(t->Oid));

   xtf.closeObject();

}

void ImdOutput::visitDependency(Dependency *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Dependency", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Dependency
   xtf.writeReference("Using", get_path(t->Using));
   xtf.writeReference("Dependent", get_path(t->Dependent));

   xtf.closeObject();

}

void ImdOutput::visitAllowedInBasket(AllowedInBasket *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "AllowedInBasket", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // AllowedInBasket
   xtf.writeReference("OfDataUnit", get_path(t->OfDataUnit));
   xtf.writeReference("ClassInBasket", get_path(t->ClassInBasket));

   xtf.closeObject();

}

void ImdOutput::visitContext(Context *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Context", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // Context

   xtf.closeObject();

}

void ImdOutput::visitGenericDef(GenericDef *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "GenericDef", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // GenericDef
   xtf.writeReference("OID", get_path(t->OID));
   xtf.writeReference("Context", get_path(t->Context));

   xtf.closeObject();

}

void ImdOutput::visitUnit(Unit *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Unit", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Unit
   switch (t->Kind) {
      case Unit::BaseU:
         xtf.writeTag("Kind","BaseU");
         break;
      case Unit::DerivedU:
         xtf.writeTag("Kind","DerivedU");
         break;
      case Unit::ComposedU:
         xtf.writeTag("Kind","ComposedU");
         break;
      default:
         logger().error("unsupported Unit.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.writeReference("Definition", get_path(t->Definition));

   xtf.closeObject();

}

void ImdOutput::visitMetaBasketDef(MetaBasketDef *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "MetaBasketDef", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // MetaBasketDef
   switch (t->Kind) {
      case MetaBasketDef::SignB:
         xtf.writeTag("Kind","SignB");
         break;
      case MetaBasketDef::RefSystemB:
         xtf.writeTag("Kind","RefSystemB");
         break;
      default:
         logger().error("unsupported MetaBasketDef.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.writeReference("MetaDataTopic", get_path(t->MetaDataTopic));
   xtf.writeReference("Member", get_path(t->Member));

   xtf.closeObject();

}

void ImdOutput::visitMetaObjectDef(MetaObjectDef *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "MetaObjectDef", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // MetaObjectDef
   xtf.writeTag("IsRefSystem", to_string_bool(t->IsRefSystem));
   xtf.writeReference("Class", get_path(t->Class));

   xtf.closeObject();

}

void ImdOutput::visitBooleanType(BooleanType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "BooleanType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // BooleanType

   xtf.closeObject();

}

void ImdOutput::visitTextType(TextType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "TextType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // TextType
   switch (t->Kind) {
      case TextType::MText:
         xtf.writeTag("Kind","MText");
         break;
      case TextType::Text:
         xtf.writeTag("Kind","Text");
         break;
      case TextType::NameVal:
         xtf.writeTag("Kind","NameVal");
         break;
      case TextType::Uri:
         xtf.writeTag("Kind","Uri");
         break;
      default:
         logger().error("unsupported TextType.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.writeTag("MaxLength", to_string(t->MaxLength));

   xtf.closeObject();

}

void ImdOutput::visitBlackboxType(BlackboxType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "BlackboxType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
   xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // BlackboxType
   switch (t->Kind) {
      case BlackboxType::Binary:
         xtf.writeTag("Kind","Binary");
         break;
      case BlackboxType::Xml:
         xtf.writeTag("Kind","Xml");
         break;
      default:
         logger().error("unsupported BlackboxType.Kind value " + to_string(t->Kind));
         break;
   }

   xtf.closeObject();

}

void ImdOutput::visitNumType(NumType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "NumType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
   xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // NumType
   xtf.writeTag("Min", t->Min);
   xtf.writeTag("Max", t->Max);
   xtf.writeTag("Circular", to_string_bool(t->Circular));
   xtf.writeTag("Clockwise", to_string_bool(t->Clockwise));
   xtf.writeReference("Unit", get_path(t->Unit));

   xtf.closeObject();

}

void ImdOutput::visitCoordType(CoordType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "CoordType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
   xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // CoordType
   xtf.writeTag("NullAxis", to_string(t->NullAxis));
   xtf.writeTag("PiHalfAxis", to_string(t->PiHalfAxis));
   if (version == "2.4") {
      xtf.writeTag("Multi", to_string_bool(t->Multi));
   }

   xtf.closeObject();

}

void ImdOutput::visitAxisSpec(AxisSpec *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "AxisSpec", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // AxisSpec
   xtf.writeReference("CoordType", get_path(t->CoordType));
   xtf.writeReference("Axis", get_path(t->Axis));

   xtf.closeObject();

}

void ImdOutput::visitNumsRefSys(NumsRefSys *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "NumsRefSys", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // NumsRefSys
   xtf.writeReference("NumType", get_path(t->NumType));
   xtf.writeReference("RefSys", get_path(t->RefSys));
   xtf.writeTag("AxisInd", to_string(t->AxisInd));

   xtf.closeObject();

}

void ImdOutput::visitFormattedType(FormattedType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "FormattedType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
   xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // NumType
   xtf.writeTag("Min", t->Min);
   xtf.writeTag("Max", t->Max);
   xtf.writeTag("Circular", to_string_bool(t->Circular));
   xtf.writeTag("Clockwise", to_string_bool(t->Clockwise));
   xtf.writeReference("Unit", get_path(t->Unit));

   // FormattedType
   xtf.writeTag("Format", t->Format);
   xtf.writeReference("Struct", get_path(t->Struct));

   xtf.closeObject();

}

void ImdOutput::visitAnyOIDType(AnyOIDType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "AnyOIDType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
   xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // AnyOIDType

   xtf.closeObject();

}

void ImdOutput::visitFunctionDef(FunctionDef *t)
{
      
   visit(t->ResultType);
   //visit(t->LocalType);

   xtf.openObject("IlisMeta07", "ModelData", "FunctionDef", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // FunctionDef
   xtf.writeTag("Explanation", t->Explanation);
   xtf.writeReference("LocalType", get_path(t->LocalType));
   xtf.writeReference("ResultType", get_path(t->ResultType));

   xtf.closeObject();

}

void ImdOutput::visitArgument(Argument *t)
{
   
   visit(t->Type);

   xtf.openObject("IlisMeta07", "ModelData", "Argument", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // Argument
   switch (t->Kind) {
      case Argument::TypeVal:
         xtf.writeTag("Kind","TypeVal");
         break;
      case Argument::EnumVal:
         xtf.writeTag("Kind","EnumVal");
         break;
      case Argument::EnumTreeVal:
         xtf.writeTag("Kind","EnumTreeVal");
         break;
      default:
         logger().error("unsupported Argument.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.writeReference("Function", get_path(t->Function));
   xtf.writeReference("Type", get_path(t->Type));

   xtf.closeObject();

}

void ImdOutput::visitClassRefType(ClassRefType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ClassRefType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
   xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // ClassRelatedType
   xtf.writeReference("BaseClass", get_path(t->_baseclass));

   // ClassRefType

   xtf.closeObject();

}

void ImdOutput::visitObjectType(ObjectType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ObjectType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
   xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // ClassRelatedType
   xtf.writeReference("BaseClass", get_path(t->_baseclass));

   // ObjectType
   xtf.writeTag("Multiple", to_string_bool(t->Multiple));

   xtf.closeObject();

}

void ImdOutput::visitAttributeRefType(AttributeRefType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "AttributeRefType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
   xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // AttributeRefType
   xtf.writeReference("Of", get_path(t->Of));

   xtf.closeObject();

}

void ImdOutput::visitARefRestriction(ARefRestriction *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ARefRestriction", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // ARefRestriction
   xtf.writeReference("ARef", get_path(t->ARef));
   xtf.writeReference("Type", get_path(t->Type));

   xtf.closeObject();

}

void ImdOutput::visitEnumType(EnumType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "EnumType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // EnumType
   switch (t->Order) {
      case EnumType::Unordered:
         xtf.writeTag("Order","Unordered");
         break;
      case EnumType::Ordered:
         xtf.writeTag("Order","Ordered");
         break;
      case EnumType::Circular:
         xtf.writeTag("Order","Circular");
         break;
      default:
         logger().error("unsupported EnumType.Order value " + to_string(t->Order));
         break;
   }

   xtf.closeObject();

}

void ImdOutput::visitEnumNode(EnumNode *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "EnumNode", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // EnumNode
   xtf.writeReference("EnumType", get_path(t->EnumType));
   xtf.writeReference("ParentNode", get_path(t->ParentNode));
   
   xtf.closeObject();

}

void ImdOutput::visitEnumTreeValueType(EnumTreeValueType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "EnumTreeValueType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // EnumTreeValueType
   xtf.writeReference("ET", get_path(t->ET));

   xtf.closeObject();

}

void ImdOutput::visitLineForm(LineForm *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "LineForm", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // LineForm
   xtf.writeReference("Structure", get_path(t->Structure));

   xtf.closeObject();

}

void ImdOutput::visitLineType(LineType *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "LineType", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // DomainType
   xtf.writeTag("Mandatory", to_string_bool(t->Mandatory));
   xtf.writeReference("GenericDef", get_path(t->GenericDef));

   // LineType
   switch (t->Kind) {
      case LineType::Polyline:
         xtf.writeTag("Kind","Polyline");
         break;
      case LineType::DirectedPolyline:
         xtf.writeTag("Kind","DirectedPolyline");
         break;
      case LineType::Surface:
         xtf.writeTag("Kind","Surface");
         break;
      case LineType::Area:
         xtf.writeTag("Kind","Area");
         break;
      default:
         logger().error("unsupported LineType.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.writeTag("MaxOverlap", t->MaxOverlap);
   if (version == "2.4") {
      xtf.writeTag("Multi", to_string_bool(t->Multi));
   }
   xtf.writeReference("CoordType", get_path(t->CoordType));
   xtf.writeReference("LAStructure", get_path(t->LAStructure));

   xtf.closeObject();

}

void ImdOutput::visitLinesForm(LinesForm *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "LinesForm", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // LinesForm
   xtf.writeReference("LineType", get_path(t->LineType));
   xtf.writeReference("LineForm", get_path(t->LineForm));

   xtf.closeObject();

}

void ImdOutput::visitView(View *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "View", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   if (t->LFTParent == nullptr) {
      xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));
   }

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Type
   xtf.writeReference("LFTParent", get_path(t->LFTParent));

   // Class
   switch (t->Kind) {
      case Class::Structure:
         xtf.writeTag("Kind","Structure");
         break;
      case Class::ClassVal:
         xtf.writeTag("Kind","ClassVal");
         break;
      case Class::ViewVal:
         xtf.writeTag("Kind","ViewVal");
         break;
      case Class::Association:
         xtf.writeTag("Kind","Association");
         break;
      default:
         logger().error("unsupported Class.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.openTag("Multiplicity");
   visitMultiplicity(&t->Multiplicity);
   xtf.closeTag();
   xtf.writeTag("EmbeddedRoleTransfer", to_string_bool(t->EmbeddedRoleTransfer));
   xtf.writeTag("ili1OptionalTable", to_string_bool(t->ili1OptionalTable));
   xtf.writeReference("LTParent", get_path(t->LTParent));
   xtf.writeReference("Oid", get_path(t->Oid));
   //xtf.writeReference("View", get_path(t->View)); to do !!!

   // View
   switch (t->FormationKind) {
      case View::Projection:
         xtf.writeTag("FormationKind","Projection");
         break;
      case View::Join:
         xtf.writeTag("FormationKind","Join");
         break;
      case View::Union:
         xtf.writeTag("FormationKind","Union");
         break;
      case View::Aggregation_All:
         xtf.writeTag("FormationKind","Aggregation_All");
         break;
      case View::Aggregation_Equal:
         xtf.writeTag("FormationKind","Aggregation_Equal");
         break;
      case View::Inspection_Normal:
         xtf.writeTag("FormationKind","Inspection_Normal");
         break;
      case View::Inspection_Area:
         xtf.writeTag("FormationKind","Inspection_Area");
         break;
      default:
         logger().error("unsupported View.FormationKind value " + to_string(t->FormationKind));
         break;
   }
   if (t->FormationParameter.size() > 0) {
      xtf.openTag("FormationParameter");
      for (auto tt : t->FormationParameter) {
         visitExpression(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("Where", get_path(t->Where));
   xtf.writeTag("Transient", to_string_bool(t->Transient));

   xtf.closeObject();

}

void ImdOutput::visitRenamedBaseView(RenamedBaseView *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "RenamedBaseView", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // RenamedBaseView
   xtf.writeTag("OrNull", to_string_bool(t->OrNull));
   xtf.writeReference("View", get_path(t->View));
   xtf.writeReference("BaseView", get_path(t->BaseView));

   xtf.closeObject();

}

void ImdOutput::visitUnaryExpr(UnaryExpr *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "UnaryExpr", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // UnaryExpr
   switch (t->Operation) {
      case UnaryExpr::Not:
         xtf.writeTag("Operation","Not");
         break;
      case UnaryExpr::Defined:
         xtf.writeTag("Operation","Defined");
         break;
      default:
         logger().error("unsupported UnaryExpr.Operation value " + to_string(t->Operation));
         break;
   }
   xtf.writeReference("SubExpression", get_path(t->SubExpression));

   xtf.closeObject();

}

void ImdOutput::visitCompoundExpr(CompoundExpr *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "CompoundExpr", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // CompoundExpr
   xtf.openTag("Operation");
   //visitCompoundExpr_OperationType(t->Operation); to do !!!
   xtf.closeTag();
   if (t->SubExpressions.size() > 0) {
      xtf.openTag("SubExpressions");
      for (auto tt : t->SubExpressions) {
         visitExpression(tt);
      }
      xtf.closeTag();
   }

   xtf.closeObject();

}

void ImdOutput::visitPathEl(PathEl *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "PathEl", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // PathEl
   switch (t->Kind) {
      case PathEl::This:
         xtf.writeTag("Kind","This");
         break;
      case PathEl::ThisArea:
         xtf.writeTag("Kind","ThisArea");
         break;
      case PathEl::ThatArea:
         xtf.writeTag("Kind","ThatArea");
         break;
      case PathEl::Parent:
         xtf.writeTag("Kind","Parent");
         break;
      case PathEl::ReferenceAttr:
         xtf.writeTag("Kind","ReferenceAttr");
         break;
      case PathEl::AssocPath:
         xtf.writeTag("Kind","AssocPath");
         break;
      case PathEl::Role:
         xtf.writeTag("Kind","Role");
         break;
      case PathEl::ViewBase:
         xtf.writeTag("Kind","ViewBase");
         break;
      case PathEl::Attribute:
         xtf.writeTag("Kind","Attribute");
         break;
      case PathEl::MetaObject:
         xtf.writeTag("Kind","MetaObject");
         break;
      default:
         logger().error("unsupported PathEl.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.writeReference("Ref", get_path(t->Ref));
   xtf.writeTag("NumIndex", to_string(t->NumIndex));
   switch (t->SpecIndex) {
      case PathEl::First:
         xtf.writeTag("SpecIndex","First");
         break;
      case PathEl::Last:
         xtf.writeTag("SpecIndex","Last");
         break;
      default:
         logger().error("unsupported PathEl.SpecIndex value " + to_string(t->SpecIndex));
         break;
   }

   xtf.closeObject();

}

void ImdOutput::visitPathOrInspFactor(PathOrInspFactor *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "PathOrInspFactor", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // PathOrInspFactor
   if (t->PathEls.size() > 0) {
      xtf.openTag("PathEls");
      for (auto tt : t->PathEls) {
         visitPathEl(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("Inspection", get_path(t->Inspection));

   xtf.closeObject();

}

void ImdOutput::visitEnumAssignment(EnumAssignment *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "EnumAssignment", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // EnumAssignment
   xtf.writeReference("ValueToAssign", get_path(t->ValueToAssign));
   xtf.writeReference("MinEnumValue", get_path(t->MinEnumValue));
   xtf.writeReference("MaxEnumValue", get_path(t->MaxEnumValue));

   xtf.closeObject();

}

void ImdOutput::visitEnumMapping(EnumMapping *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "EnumMapping", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // EnumMapping
   xtf.writeReference("EnumValue", get_path(t->EnumValue));
   if (t->Cases.size() > 0) {
      xtf.openTag("Cases");
      for (auto tt : t->Cases) {
         visitEnumAssignment(tt);
      }
      xtf.closeTag();
   }

   xtf.closeObject();

}

void ImdOutput::visitClassRef(ClassRef *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ClassRef", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // ClassRef
   xtf.writeReference("Ref", get_path(t->Ref));

   xtf.closeObject();

}

void ImdOutput::visitActualArgument(ActualArgument *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ActualArgument", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // ActualArgument
   xtf.writeReference("FormalArgument", get_path(t->FormalArgument));
   switch (t->Kind) {
      case ActualArgument::ExpressionVal:
         xtf.writeTag("Kind","ExpressionVal");
         break;
      case ActualArgument::AllOf:
         xtf.writeTag("Kind","AllOf");
         break;
      default:
         logger().error("unsupported ActualArgument.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.writeReference("Expression", get_path(t->Expression));

   xtf.closeObject();

}

void ImdOutput::visitFunctionCall(FunctionCall *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "FunctionCall", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // FunctionCall
   xtf.writeReference("Function", get_path(t->Function));
   if (t->Arguments.size() > 0) {
      xtf.openTag("Arguments");
      for (auto tt : t->Arguments) {
         visitActualArgument(tt);
      }
      xtf.closeTag();
   }

   xtf.closeObject();

}

void ImdOutput::visitRuntimeParamRef(RuntimeParamRef *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "RuntimeParamRef", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // RuntimeParamRef
   xtf.writeReference("RuntimeParam", get_path(t->RuntimeParam));

   xtf.closeObject();

}

void ImdOutput::visitConstant(Constant *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Constant", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // Constant
   xtf.writeTag("Value", t->Value);
   switch (t->Kind) {
      case Constant::Undefined:
         xtf.writeTag("Kind","Undefined");
         break;
      case Constant::Numeric:
         xtf.writeTag("Kind","Numeric");
         break;
      case Constant::Text:
         xtf.writeTag("Kind","Text");
         break;
      case Constant::Enumeration:
         xtf.writeTag("Kind","Enumeration");
         break;
      default:
         logger().error("unsupported Constant.Kind value " + to_string(t->Kind));
         break;
   }

   xtf.closeObject();

}

void ImdOutput::visitClassConst(ClassConst *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ClassConst", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // ClassConst
   xtf.writeReference("Class", get_path(t->Class));

   xtf.closeObject();

}

void ImdOutput::visitAttributeConst(AttributeConst *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "AttributeConst", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // AttributeConst
   xtf.writeReference("Attribute", get_path(t->Attribute));

   xtf.closeObject();

}

void ImdOutput::visitUnitRef(UnitRef *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "UnitRef", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // UnitRef
   xtf.writeReference("Unit", get_path(t->Unit));

   xtf.closeObject();

}

void ImdOutput::visitUnitFunction(UnitFunction *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "UnitFunction", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // UnitFunction
   xtf.writeTag("Explanation", t->Explanation);

   xtf.closeObject();

}

void ImdOutput::visitSimpleConstraint(SimpleConstraint *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "SimpleConstraint", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // Constraint
   if (version == "2.4") {
      xtf.writeReference("toDomain", get_path(t->toDomain));
   }
   if (version == "2.4") {
      xtf.writeReference("ToClass", get_path(t->ToClass));
   }

   // SimpleConstraint
   switch (t->Kind) {
      case SimpleConstraint::MandC:
         xtf.writeTag("Kind","MandC");
         break;
      case SimpleConstraint::LowPercC:
         xtf.writeTag("Kind","LowPercC");
         break;
      case SimpleConstraint::HighPercC:
         xtf.writeTag("Kind","HighPercC");
         break;
      default:
         logger().error("unsupported SimpleConstraint.Kind value " + to_string(t->Kind));
         break;
   }
   xtf.writeTag("Percentage", to_string(t->Percentage));
   xtf.writeReference("LogicalExpression", get_path(t->LogicalExpression));

   xtf.closeObject();

}

void ImdOutput::visitExistenceDef(ExistenceDef *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ExistenceDef", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Expression

   // Factor

   // PathOrInspFactor
   if (t->PathEls.size() > 0) {
      xtf.openTag("PathEls");
      for (auto tt : t->PathEls) {
         visitPathEl(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("Inspection", get_path(t->Inspection));

   // ExistenceDef
   xtf.writeReference("Viewable", get_path(t->Viewable));

   xtf.closeObject();

}

void ImdOutput::visitExistenceConstraint(ExistenceConstraint *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "ExistenceConstraint", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // Constraint
   if (version == "2.4") {
      xtf.writeReference("toDomain", get_path(t->toDomain));
   }
   if (version == "2.4") {
      xtf.writeReference("ToClass", get_path(t->ToClass));
   }

   // ExistenceConstraint
   xtf.writeReference("Attr", get_path(t->Attr));

   xtf.closeObject();

}

void ImdOutput::visitUniqueConstraint(UniqueConstraint *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "UniqueConstraint", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // Constraint
   if (version == "2.4") {
      xtf.writeReference("toDomain", get_path(t->toDomain));
   }
   if (version == "2.4") {
      xtf.writeReference("ToClass", get_path(t->ToClass));
   }

   // UniqueConstraint
   switch (t->Kind) {
      case UniqueConstraint::GlobalU:
         xtf.writeTag("Kind","GlobalU");
         break;
      case UniqueConstraint::LocalU:
         xtf.writeTag("Kind","LocalU");
         break;
      default:
         logger().error("unsupported UniqueConstraint.Kind value " + to_string(t->Kind));
         break;
   }

   xtf.closeObject();

}

void ImdOutput::visitSetConstraint(SetConstraint *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "SetConstraint", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // Constraint
   if (version == "2.4") {
      xtf.writeReference("toDomain", get_path(t->toDomain));
   }
   if (version == "2.4") {
      xtf.writeReference("ToClass", get_path(t->ToClass));
   }

   // SetConstraint
   xtf.writeReference("Constraint", get_path(t->Constraint));

   xtf.closeObject();

}

void ImdOutput::visitGraphic(Graphic *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Graphic", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // Graphic
   xtf.writeReference("Where", get_path(t->Where));
   xtf.writeReference("Base", get_path(t->Base));

   xtf.closeObject();

}

void ImdOutput::visitSignParamAssignment(SignParamAssignment *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "SignParamAssignment", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // SignParamAssignment
   xtf.writeReference("Param", get_path(t->Param));
   xtf.writeReference("Assignment", get_path(t->Assignment));

   xtf.closeObject();

}

void ImdOutput::visitCondSignParamAssignment(CondSignParamAssignment *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "CondSignParamAssignment", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // CondSignParamAssignment
   xtf.writeReference("Where", get_path(t->Where));
   if (t->Assignments.size() > 0) {
      xtf.openTag("Assignments");
      for (auto tt : t->Assignments) {
         visitSignParamAssignment(tt);
      }
      xtf.closeTag();
   }

   xtf.closeObject();

}

void ImdOutput::visitDrawingRule(DrawingRule *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "DrawingRule", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // MetaElement
   xtf.writeTag("Name", t->Name);
   if (t->Documentation.size() > 0) {
      xtf.openTag("Documentation");
      for (auto tt : t->Documentation) {
         visitDocText(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("ElementInPackage", get_path(t->ElementInPackage));

   // ExtendableME
   xtf.writeTag("Abstract", to_string_bool(t->Abstract));
   if (version == "2.4") {
      xtf.writeTag("Generic", to_string_bool(t->Generic));
   }
   xtf.writeTag("Final", to_string_bool(t->Final));
   xtf.writeReference("Super", get_path(t->Super));

   // DrawingRule
   if (t->Rule.size() > 0) {
      xtf.openTag("Rule");
      for (auto tt : t->Rule) {
         visitCondSignParamAssignment(tt);
      }
      xtf.closeTag();
   }
   xtf.writeReference("Graphic", get_path(t->Graphic));
   xtf.writeReference("Class", get_path(t->Class));

   xtf.closeObject();

}

void ImdOutput::visitDocTextTranslation(DocTextTranslation *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "DocTextTranslation", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // DocTextTranslation
   xtf.writeTag("Text", t->Text);

   xtf.closeObject();

}

void ImdOutput::visitMETranslation(METranslation *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "METranslation", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // METranslation
   xtf.writeReference("Of", get_path(t->Of));
   xtf.writeTag("TranslatedName", t->TranslatedName);
   if (t->TranslatedDoc.size() > 0) {
      xtf.openTag("TranslatedDoc");
      for (auto tt : t->TranslatedDoc) {
         visitDocTextTranslation(tt);
      }
      xtf.closeTag();
   }

   xtf.closeObject();

}

void ImdOutput::visitTranslation(Translation *t)
{

   xtf.openObject("IlisMeta07", "ModelData", "Translation", get_path(t));
   xtf.writeComment("line=" + to_string(t->_line));

   // Translation
   xtf.writeTag("Language", t->Language);

   xtf.closeObject();

}
