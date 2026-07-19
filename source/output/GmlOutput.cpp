#pragma once

#include "GmlOutput.h"
#include "../metamodel/MetaModelOutput.h"
#include "../util/StringMap.h"
#include "../util/StringUtil.h"
#include "../util/Logger.h"

#include <algorithm>
#include <climits>

using namespace util;
using namespace metamodel;
using namespace output;

GmlOutput::GmlOutput(string gml_output_dir, string iliversion, string ilic_version)
{
   this->gml_output_dir = gml_output_dir;
   this->iliversion = iliversion;
   this->ilic_version = ilic_version;
}

void GmlOutput::preVisit()
{
   list<Model*> allModels = get_all_models();
   set<string> writtenNames;

   for (Model* m : allModels) {
      if (m->Name == "INTERLIS" || m->Kind == Model::TypeM) {
         continue;
      }

      writtenNames.clear();

      for (auto* me : m->Element) {
         if (me == nullptr) {
            continue;
         }

         if (me->getClass() != "SubModel") {
            continue;
         }

         string topicName = me->Name;

         SubModel* sm = static_cast<SubModel*>(me);

         writtenNames.insert(topicName);
         ObjectToNameDefinition.insert({ me,topicName });

         for (auto* sme : sm->Element) {
            if (sme->getClass() != "Class") {
               continue;
            }
            string className = sme->Name;
            if (writtenNames.find(className) != writtenNames.end()) {
               string topicName = sme->ElementInPackage->Name;
               string scopedName = topicName + "." + className;
               writtenNames.insert(scopedName);
               ObjectToNameDefinition.insert({ sme,scopedName });
            }
            else {
               writtenNames.insert(className);
               ObjectToNameDefinition.insert({ sme,className });
            }
         }
      }
   }
}

void GmlOutput::preVisitModel(Model* m) {
   currentModel = m->Name;

   if (m->Name == "INTERLIS") {
      ignoreVisit();
   }

   gml.openFile(gml_output_dir + "/" + m->Name + ".xsd");

   gml.writeln("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
   gml.writeln("<xsd:schema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"");
   gml.incNestLevel();
   gml.write(" xmlns=\"http://www.interlis.ch/INTERLIS2.3/GML32/" + m->Name + "\"");
   gml.write(" targetNamespace=\"http://www.interlis.ch/INTERLIS2.3/GML32/" + m->Name + "\"");
   gml.writeln(" elementFormDefault=\"qualified\" attributeFormDefault=\"unqualified\"");
   gml.writeln("xmlns:gml=\"http://www.opengis.net/gml/3.2\"");
   gml.writeln("xmlns:INTERLIS=\"http://www.interlis.ch/INTERLIS2.3/GML32/INTERLIS\"");
   gml.writeln("xmlns:ili2c=\"http://www.interlis.ch/ili2c\"");

   for (Import* i : get_all_imports()) {
      if (m->Name != i->ImportingP->Name) {
         continue;
      }
      else if (i->ImportedP->Name == "INTERLIS") {
         continue;
      }

      gml.writeln("xmlns:" + i->ImportedP->Name + "=\"http://www.interlis.ch/INTERLIS2.3/GML32/" + i->ImportedP->Name + "\"");
   }

   gml.writeln(">");
   gml.decNestLevel();

   gml.writeln("<xsd:annotation>");

   gml.writelnIncNestLevel("<xsd:appinfo source=\"http://www.interlis.ch/ili2c/ili2cversion\">"
      + ilic_version + "</xsd:appinfo>"
   );

   gml.writeln("<xsd:appinfo source=\"http://www.interlis.ch/ili2c\">");

   gml.writelnIncNestLevel("<ili2c:model>" + m->Name + "</ili2c:model>");
   gml.writeln("<ili2c:modelVersion>" + m->Version + "</ili2c:modelVersion>");

   if (!m->VersionExplanation.empty()) {
      gml.writeln("<ili2c:modelVersionExplanation>" + m->VersionExplanation + "</ili2c:modelVersionExplanation>");
   }

   gml.writeln("<ili2c:modelAt>" + m->At + "</ili2c:modelAt>");

   gml.writelnDecNestLevel("</xsd:appinfo>");
   gml.writelnDecNestLevel("</xsd:annotation>");

   // import of GML schema
   gml.writeln("<xsd:import namespace=\"http://www.opengis.net/gml/3.2\"/>");
   // import of base ILIGML schema
   gml.writeln("<xsd:import namespace=\"http://www.interlis.ch/INTERLIS2.3/GML32/INTERLIS\"/>");

   for (Import* i : get_all_imports()) {
      if (m->Name != i->ImportingP->Name) {
         continue;
      }
      else if (i->ImportedP->Name == "INTERLIS") {
         continue;
      }

      gml.writeln("<xsd:import namespace=\"http://www.interlis.ch/INTERLIS2.3/GML32/" + i->ImportedP->Name + "\"/>");
   }
}

void GmlOutput::visitModel(Model* m) {
   if (m->Name == "INTERLIS") {
      ignoreVisit();
   }
}

void GmlOutput::postVisitModel(Model* m) {
   if (m->Name == "INTERLIS") {
      ignoreVisit();
   }

   write_linetables();
   write_codelists(m);

   gml.writeln("</xsd:schema>");
   gml.closeFile();
}

void GmlOutput::preVisitSubModel(metamodel::SubModel* s) {
   currentSubModel = s->Name;
}

void GmlOutput::postVisitSubModel(metamodel::SubModel* s) {
   gml.writeln("<xsd:complexType name=\"" + s->Name + "MemberType\">");
   gml.writelnIncNestLevel("<xsd:complexContent>");
   gml.writelnIncNestLevel("<xsd:extension base=\"gml:AbstractFeatureMemberType\">");
   gml.writelnIncNestLevel("<xsd:sequence>");
   gml.writelnIncNestLevel("<xsd:choice>");

   gml.incNestLevel();

   list<string> allClassNames;
   vector<string> allPathNames;

   list<MetaElement*> allElements;

   Package* sIter = s;

   list<Package*> packages;
   while (sIter != nullptr) {
      if (sIter->Name == "INTERLIS") {
         break;
      }

      packages.push_front(sIter);

      sIter = sIter->_super;
   }

   for (auto* p : packages) {
      for (auto* a : p->Element) {
         if (a->getClass() == "Class") {
            Class* c = static_cast<Class*>(a);

            if (c->isDomainType) {
               continue;
            }
            else if (c->Abstract) {
               continue;
            }
            else if (c->Kind != Class::ClassVal && c->Kind != Class::Association) {
               continue;
            }
            //else if (getModelName(c) != currentModel) {
            //   continue;
            //}

            list<ExtendableME*> attributes = getClassAttributes(c);

            if (attributes.size() > 0 || c->Kind == Class::ClassVal) {
               if (c->Super != nullptr) {
                  if (find(allElements.begin(), allElements.end(), c->Super) == allElements.end()) {
                     allElements.push_back(c->Super);
                  }

               }
            }
         }

         allElements.push_back(a);
      }
   }

   for (auto* a : allElements) {
      if (a->getClass() == "Class") {
         Class* c = static_cast<Class*>(a);

         if (c->Abstract) {
            continue;
         }
         else if (c->Kind != Class::ClassVal && c->Kind != Class::Association) {
            continue;
         }
         else if (!isRealClass(c)) {
            continue;
         }
         //else if (getModelName(c) != currentModel) {
         //   continue;
         //}

         list<ExtendableME*> attributes = getClassAttributes(c);

         if (attributes.size() > 0 || c->Kind == Class::ClassVal) {
            auto iteratorClassName = find(allClassNames.begin(), allClassNames.end(), c->Name);

            if (iteratorClassName != allClassNames.end()) {
               int index = distance(allClassNames.begin(), iteratorClassName);
               auto iteratorPathName = allPathNames.begin();
               advance(iteratorPathName, index);

               allClassNames.erase(iteratorClassName);
               allPathNames.erase(iteratorPathName);
            }

            string path = getScopedName(c);

            if (path.rfind("INTERLIS.", 0) == 0) {

            }
            else {
               allClassNames.push_back(c->Name);
               allPathNames.push_back(path);
            }
         }
      }
   }

   for (auto path : allPathNames) {
      gml.writeln("<xsd:element ref=\"" + path + "\"/>");
   }

   for (auto* a : areaAttributes) {
      if (a->AttrParent->ElementInPackage != s) {
         continue;
      }

      gml.writeln("<xsd:element ref=\"" + getScopedName(a->AttrParent) + "." + a->Name + "\"/>");
   }

   gml.decNestLevel();

   gml.writeln("</xsd:choice>");
   gml.writelnDecNestLevel("</xsd:sequence>");
   gml.writelnDecNestLevel("</xsd:extension>");
   gml.writelnDecNestLevel("</xsd:complexContent>");
   gml.writelnDecNestLevel("</xsd:complexType>");

   string baseElement = "gml:AbstractFeature";

   string searchDataUnit = get_path(s) + ".BASKET";
   for (auto* du : get_all_dataunits()) {
      string dataunit_path = get_path(du);

      if (searchDataUnit == dataunit_path) {
         if (du->Extended) {
            baseElement = getScopedName(du->Super);
         }
         break;
      }
   }

   gml.writeln("<xsd:element name=\"" + s->Name + "\" type=\"" + s->Name + "Type\" substitutionGroup=\"" + baseElement + "\"/>");
   gml.writeln("<xsd:complexType name=\"" + s->Name + "Type\">");
   gml.writelnIncNestLevel("<xsd:complexContent>");
   gml.writelnIncNestLevel("<xsd:extension base=\"gml:AbstractFeatureType\">");
   gml.writelnIncNestLevel("<xsd:sequence>");
   gml.writelnIncNestLevel("<xsd:element name=\"member\" type=\"" + s->Name + "MemberType\" minOccurs=\"0\" maxOccurs=\"unbounded\"/>");
   gml.writelnDecNestLevel("</xsd:sequence>");
   gml.writeln("<xsd:attributeGroup ref=\"gml:AggregationAttributeGroup\"/>");
   gml.writelnDecNestLevel("</xsd:extension>");
   gml.writelnDecNestLevel("</xsd:complexContent>");
   gml.writelnDecNestLevel("</xsd:complexType>");
}

void GmlOutput::preVisitClass(metamodel::Class* c) {
   if (c->_attr != nullptr) {
      return;
   }

   if (c->Kind == Class::Association && c->Final) {
      return;
   }  

   string base = "gml:AbstractFeature";
   string baseXsd = "gml:AbstractFeatureType";

   if (c->Super != nullptr) {
      base = getScopedName(c->Super);
      baseXsd = base + "Type";
   }
   else if (c->ElementInPackage != nullptr && c->Extended) {
      base = getModelName(c->ElementInPackage->_super) + ":" + c->Name;
      baseXsd = base + "Type";
   }

   gml.writeln("<xsd:element name=\"" + getScopedName(c) + "\" type=\"" + getScopedName(c) + "Type\" substitutionGroup=\"" + base + "\"/>");
   gml.writeln("<xsd:complexType  name=\"" + getScopedName(c) + "Type\">");
   gml.writelnIncNestLevel("<xsd:complexContent>");
   gml.writelnIncNestLevel("<xsd:extension base=\"" + baseXsd + "\">");
   gml.writelnIncNestLevel("<xsd:sequence>");

   if (c->isDomainType) {
      gml.writelnIncNestLevel("<xsd:simpleContent>");
      gml.writelnIncNestLevel("<xsd:extension base=\"xsd:string\"/>");
      gml.writelnDecNestLevel("</xsd:simpleContent>");
      gml.writelnDecNestLevel("</xsd:complexType>");

      return;
   }

   gml.incNestLevel();
}

void GmlOutput::visitClass(metamodel::Class* c) {
   if (c->_attr != nullptr) {
      return;
   }

   if (c->isDomainType) {
      return;
   }

   if (c->Kind == Class::Association && c->Final) {
      return;
   }

   list<ExtendableME*> classesToScan;
   list<ExtendableME*> attributes;

   ExtendableME* attributeClass = c;

   while (attributeClass != nullptr) {

      if (attributeClass->getClass() == "Class") {
         classesToScan.push_front(attributeClass);

      }

      //attributeClass = attributeClass->Super;
      attributeClass = nullptr;
   }

   for (auto cts : classesToScan) {
      Class* ac = static_cast<Class*>(cts);

      if (ac->Kind == Class::Association) {
         for (auto ra : ac->_roleaccess) {
            attributes.push_back(ra);
         }
      }

      for (auto ca : ac->ClassAttribute) {

         string findString = ca->Name;
         auto it = std::find_if(attributes.begin(), attributes.end(), [&findString](metamodel::ExtendableME* obj) {return obj->Name == findString; });

         if (it != attributes.end()) {
            it = attributes.erase(it);
            attributes.insert(it, ca);
         }
         else {
            attributes.push_back(ca);
         }
      }

      if (ac->Kind != Class::Association) {
         for (auto ra : ac->_roleaccess) {
            attributes.push_back(ra);
         }
      }
   }

   for (auto v : attributes) {
      if (v->getClass() == "AttrOrParam") {
         if (v->Super != nullptr) {
            continue;
         }

         AttrOrParam* attr = static_cast<AttrOrParam*>(v);

         if (attr->Extended) {
            continue;
         }

         writeAttrOrParam(attr);
      }
      else if (v->getClass() == "Role") {
         string minOccurs = "";

         Role* role = static_cast<Role*>(v);

         if (role->Extended) {
            continue;
         }

         //if (getModelName(role) != currentModel) {
         //   continue;
         //}

         if (role->Multiplicity.Min < 1) {
            minOccurs = " minOccurs=\"0\"";
         }

         bool isLightWeight = true;

         if (role->Association->Role.size() > 2) {
            isLightWeight = false;
         }
         else {
            Role* role1 = role->Association->Role.front();
            Role* role2 = role->Association->Role.back();

            int role1Min = role1->Multiplicity.Min;
            int role1Max = role1->Multiplicity.Max;

            if (role1Min == -1) {
               role1Min = 0;
            }

            if (role1Max == -1) {
               if (role1->Strongness == Role::Comp) {
                  role1Max = 1;
               }
               else {
                  role1Max = INT_MAX;
               }
            }

            int role2Min = role2->Multiplicity.Min;
            int role2Max = role2->Multiplicity.Max;

            if (role2Min == -1) {
               role2Min = 0;
            }

            if (role2Max == -1) {
               if (role2->Strongness == Role::Comp) {
                  role2Max = 1;
               }
               else {
                  role2Max = INT_MAX;
               }
            }

            if (role1Max > 1 && role2Max > 1) {
               isLightWeight = false;
            }
            else {
               Role* check = nullptr;
               if (role1Max == 1) {
                  check = role2;
               }
               else {
                  check = role1;
               }

               Class* baseClass = check->_baseclass;

               ExtendableME* base = baseClass;

               while (true) {
                  if (base->Super == nullptr) {
                     break;
                  }

                  base = base->Super;
               }

               ExtendableME* currentBase = c;

               while (true) {
                  if (currentBase->Super == nullptr) {
                     break;
                  }

                  currentBase = currentBase->Super;
               }

               if (base->ElementInPackage != currentBase->ElementInPackage) {
                  isLightWeight = false;
               }
            }
         }


         if (!isLightWeight) {
            if (role->Ordered) {
               gml.writeln("<xsd:element name=\"" + role->Name + "\">");
            }
            else {
               gml.writeln("<xsd:element name=\"" + role->Name + "\" type=\"gml:ReferenceType\">");
            }

            string target = getScopedName(role->_baseclass);

            gml.writelnIncNestLevel("<xsd:annotation>");
            gml.writelnIncNestLevel("<xsd:appinfo>");
            gml.writelnIncNestLevel("<gml:targetElement>" + target + "</gml:targetElement>");
            gml.writelnDecNestLevel("</xsd:appinfo>");
            gml.writelnDecNestLevel("</xsd:annotation>");

            if (role->Ordered) {
               string ORDER_POS = "ORDER_POS";

               gml.writeln("<xsd:complexType>");
               gml.writelnIncNestLevel("<xsd:sequence/>");
               gml.writeln("<xsd:attributeGroup ref=\"gml:OwnershipAttributeGroup\"/>");
               gml.writeln("<xsd:attributeGroup ref=\"gml:AssociationAttributeGroup\"/>");
               gml.writeln("<xsd:attribute ref=\"INTERLIS:" + ORDER_POS + "\"/>");
               gml.writelnDecNestLevel("</xsd:complexType>");
            }

            gml.writelnDecNestLevel("</xsd:element>");
         }
         else {
            if (role->Ordered) {
               gml.writeln("<xsd:element name=\"" + role->Name + "\"" + minOccurs + ">");
            }
            else {
               gml.writeln("<xsd:element name=\"" + role->Name + "\" type=\"gml:ReferenceType\"" + minOccurs + ">");
            }

            string target = getScopedName(role->_baseclass);

            gml.writelnIncNestLevel("<xsd:annotation>");
            gml.writelnIncNestLevel("<xsd:appinfo>");
            gml.writelnIncNestLevel("<gml:targetElement>" + target + "</gml:targetElement>");
            gml.writelnDecNestLevel("</xsd:appinfo>");
            gml.writelnDecNestLevel("</xsd:annotation>");

            if (role->Ordered) {
               string ORDER_POS = "ORDER_POS";

               gml.writeln("<xsd:complexType>");
               gml.writelnIncNestLevel("<xsd:sequence/>");
               gml.writeln("<xsd:attributeGroup ref=\"gml:OwnershipAttributeGroup\"/>");
               gml.writeln("<xsd:attributeGroup ref=\"gml:AssociationAttributeGroup\"/>");
               gml.writeln("<xsd:attribute ref=\"INTERLIS:" + ORDER_POS + "\"/>");
               gml.writelnDecNestLevel("</xsd:complexType>");
            }

            gml.writelnDecNestLevel("</xsd:element>");

            if (!role->Association->Final || role->Association->ClassAttribute.size() > 0) {
               gml.writeln("<xsd:element name=\"" + role->Name + ".LINK_DATA\" minOccurs=\"0\">");
               gml.writelnIncNestLevel("<xsd:complexType>");
               gml.writelnIncNestLevel("<xsd:sequence>");
               gml.writelnIncNestLevel("<xsd:element ref=\"" + getScopedName(role->Association) + "\"/>");
               gml.writelnDecNestLevel("</xsd:sequence>");
               gml.writelnDecNestLevel("</xsd:complexType>");
               gml.writelnDecNestLevel("</xsd:element>");
            }
         }
      }
   }
}

void GmlOutput::postVisitClass(metamodel::Class* c) {
   if (c->_attr != nullptr) {
      return;
   }

   if (c->isDomainType) {
      return;
   }

   if (c->Kind == Class::Association && c->Final) {
      return;
   }

   gml.writelnDecNestLevel("</xsd:sequence>");
   gml.writelnDecNestLevel("</xsd:extension>");
   gml.writelnDecNestLevel("</xsd:complexContent>");
   gml.writelnDecNestLevel("</xsd:complexType>");
}

void GmlOutput::visitDomainType(metamodel::DomainType* t) {
   if (t->_attr != nullptr) {

   }
   else if (t->Name == "C1") {
   }
   else if (t->Name == "C2") {
   }
   else if (t->Name == "C3") {
   }
   else if (t->Name == "A1") {
   }
   else if (t->Name == "A2") {
   }
   else if (t->Name == "R") {
   }
   else if (t->Name == "TOP") {
   }
   else if (t->Name == "") {
   }
   else if (t->Name == "TYPE") {
   }
   else if (t->Name == "TEXT") {
   }
   else if (t->Name == "BLACKBOX") {
   }
   else {
      writeType(t);
   }
}

void GmlOutput::writeAttrOrParam(metamodel::AttrOrParam* a) {
   string minOccurs = "";

   DomainType* t = dynamic_cast<DomainType*>(a->Type);
   Class* ct = dynamic_cast<Class*>(a->Type);

   if (t != nullptr) {
      if (t->getClass() == "MultiValue") {
         MultiValue* mv = static_cast<MultiValue*>(t);

         if (mv->Multiplicity.Min == -1 && mv->Multiplicity.Max == -1) {
            minOccurs = "";
         }
         else if (mv->Multiplicity.Min < 1) {
            minOccurs = " minOccurs=\"0\"";
         }
      }
      else if (t->Mandatory != true) {
         minOccurs = " minOccurs=\"0\"";
      }
   }

   if (ct != nullptr) {
      if (ct->Name == "ANYCLASS" || ct->Name == "ANYSTRUCTURE") {
         minOccurs = " minOccurs=\"0\"";
      }
      else if (ct->Multiplicity.Min == -1 && ct->Multiplicity.Max == -1) {
         minOccurs = "";
      }
      else if (ct->Multiplicity.Min < 1) {
         minOccurs = " minOccurs=\"0\"";
      }
   }

   if (a->Type->Name == "TEXT") {
      TextType* tt = static_cast<TextType*>(a->Type);

      if (tt->Kind == TextType::NameVal) {
         gml.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + ">");
         gml.writelnIncNestLevel("<xsd:simpleType>");
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:token\">");
         gml.writelnIncNestLevel("<xsd:maxLength value=\"255\"/><xsd:pattern value=\"[a-zA-Z][a-zA-Z0-9_]*\"/>");
         gml.writelnDecNestLevel("</xsd:restriction>");
         gml.writelnDecNestLevel("</xsd:simpleType>");
         gml.writelnDecNestLevel("</xsd:element>");
         return;
      }
      else if (tt->Kind == TextType::Uri) {
         gml.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + ">");
         gml.writelnIncNestLevel("<xsd:simpleType>");
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:anyURI\">");
         gml.writelnIncNestLevel("<xsd:maxLength value=\"1023\"/>");
         gml.writelnDecNestLevel("</xsd:restriction>");
         gml.writelnDecNestLevel("</xsd:simpleType>");
         gml.writelnDecNestLevel("</xsd:element>");
         return;
      }
   }

   if (t != nullptr && t->Name != "TOP" && t->Name != "" && t->Name != "TYPE" && t->Name != "TEXT"
      && t->Name != "BLACKBOX") {
      if (t->Name == "BOOLEAN") {
         gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"xsd:boolean\"" + minOccurs + "/>");
      }
      else if (t->Name == "HALIGNMENT") {
         gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"INTERLIS:HALIGNMENT\"" + minOccurs + "/>");
      }
      else if (t->Name == "VALIGNMENT") {
         gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"INTERLIS:VALIGNMENT\"" + minOccurs + "/>");
      }
      else if (t->Name == "XMLDate") {
         gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"xsd:date\"" + minOccurs + "/>");
      }
      else if (t->Name == "XMLDateTime") {
         gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"xsd:dateTime\"" + minOccurs + "/>");
      }
      else if (t->Name == "XMLTime") {
         gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"xsd:time\"" + minOccurs + "/>");
      }
      else if (t->Name == "GregorianYear") {
         gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"xsd:gYear\"" + minOccurs + "/>");
      }
      else if (t->Name == "URI") {
         gml.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + ">");
         gml.writelnIncNestLevel("<xsd:simpleType>");
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:anyURI\">");
         gml.writelnIncNestLevel("<xsd:maxLength value=\"1023\"/>");
         gml.writelnDecNestLevel("</xsd:restriction>");
         gml.writelnDecNestLevel("</xsd:simpleType>");
         gml.writelnDecNestLevel("</xsd:element>");
      }
      else {
         if (a->Type->Super == nullptr) {
            gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"" + getScopedName(a->Type) + "\"" + minOccurs + "/>");

            if (a->Type->getClass() == "LineType") {
               LineType* lt = static_cast<LineType*>(a->Type);
               if (lt->Kind == LineType::Area) {
                  areaAttributes.push_back(a);
               }
            }
         }
         else {
            gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"" + getScopedName(a->Type->Super) + "\"" + minOccurs + "/>");

            if (a->Type->Super->getClass() == "LineType") {
               LineType* lt = static_cast<LineType*>(a->Type->Super);
               if (lt->Kind == LineType::Area) {
                  areaAttributes.push_back(a);
               }
            }
         }
      }

      return;
   }

   if (a->Type->getClass() == "CoordType") {
      gml.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + " type=\"gml:PointPropertyType\">");
      gml.writeln("</xsd:element>");
   }
   else if (a->Type->getClass() == "LineType") {
      LineType* lt = static_cast<LineType*>(a->Type);

      if (lt->Kind == LineType::Polyline) {
         gml.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + " type=\"gml:CurvePropertyType\">");
         gml.writeln("</xsd:element>");
      }
      else if (lt->Kind == LineType::Surface) {
         gml.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + " type=\"gml:SurfacePropertyType\">");
         gml.writeln("</xsd:element>");
      }
      else if (lt->Kind == LineType::Area) {
         string base = lt->CoordType->Name;
         gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"" + base + "\"" + minOccurs + ">");
         gml.writeln("</xsd:element>");

         areaAttributes.push_back(a);
      }
   }
   else if (a->Type->getClass() == "EnumType" ||
      a->Type->getClass() == "EnumTreeValueType") {
      EnumType* lt = static_cast<EnumType*>(a->Type);

      string baseClass = t->getBaseClass();

      if (!a->Final) {
         if (lt->TopNode->Node.size() > 0) {
            if (a->Extended == false) {
               gml.writeln("<xsd:element name=\"" + a->Name + "\" type=\"gml:CodeWithAuthorityType\"" + minOccurs + "/>");

               list<string> enums;

               for (auto node : lt->TopNode->Node) {
                  buildEnum(&enums, "", node);
               }

               codelistKeys.push_back(getScopedName(a->AttrParent) + "." + a->Name);
               codelistValues.push_back(enums);
            }  
         }
      }
      else {
         string typeAttribute = "";

         if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
            typeAttribute = " name=\"" + t->Name + "\"";
         }

         gml.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + ">");
         gml.writelnIncNestLevel("<xsd:simpleType" + typeAttribute + ">");
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:normalizedString\">");

         gml.incNestLevel();

         list<string> enums;

         for (auto node : lt->TopNode->Node) {
            buildEnum(&enums, "", node);
         }

         for (auto entry : enums) {
            gml.writeln("<xsd:enumeration value=\"" + entry + "\"/>");
         }

         gml.writelnDecNestLevel("</xsd:restriction>");
         gml.writelnDecNestLevel("</xsd:simpleType>");
         gml.writeln("</xsd:element>");
      }
   }
   else {
      string type = "";
      string maxOccurs = "";

      //type = " type=\"" + getScopedName(a) +"\"";

      if (a->Type->getClass() == "ReferenceType") {
         type = " type=\"gml:ReferenceType\"";
      }
      else if (a->Type->getClass() == "MultiValue") {
         MultiValue* lt = static_cast<MultiValue*>(a->Type);

         if (lt->Multiplicity.Min > 1) {
            minOccurs = " minOccurs=\"" + to_string(lt->Multiplicity.Min) + "\"";
         }
         else
         {
            minOccurs = "";
         }

         if (lt->Multiplicity.Max != 1) {
            if (lt->Multiplicity.Max == -1) {
               maxOccurs = " maxOccurs=\"unbounded\"";
            }
            else {
               maxOccurs = " maxOccurs=\"" + to_string(lt->Multiplicity.Max) + "\"";
            }
         }
      }

      gml.writeln("<xsd:element name=\"" + a->Name + "\"" + type + minOccurs + maxOccurs + ">");

      gml.incNestLevel();

      writeType(a->Type);

      gml.decNestLevel();
      gml.writeln("</xsd:element>");
   }
}

void GmlOutput::writeType(metamodel::Type *t) {
   if (t->getClass() == "TextType") {
      TextType* textType = static_cast<TextType*>(t);

      
      if (textType->OIDType) {
         string typeAttribute;
         if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE" && t->Name != "TEXT") {
            typeAttribute = " name=\"" + t->Name + "\"";
         }

         gml.writeln("<xsd:simpleType" + typeAttribute + ">");
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:token\"/>");
         gml.writelnDecNestLevel("</xsd:simpleType>");
      }
      else if (textType->Kind == TextType::Uri) {
         string typeAttribute;
         if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE" && t->Name != "TEXT") {
            typeAttribute = " name=\"" + t->Name + "\"";
         }

         gml.writeln("<xsd:simpleType" + typeAttribute + ">");
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:anyURI\">");
         gml.writelnIncNestLevel("<xsd:maxLength value=\"1023\"/>");
         gml.writelnDecNestLevel("</xsd:restriction>");
         gml.writelnDecNestLevel("</xsd:simpleType>");
      }
      else {
         writeTextType(textType);
      }
   }
   else if (t->getClass() == "ReferenceType") {
      writeReferenceType(static_cast<ReferenceType*>(t));
   }
   else if (t->getClass() == "CoordType") {
      writeCoordType(static_cast<CoordType *>(t));
   }
   else if (t->getClass() == "AnyOIDType") {
      
   }
   else if (t->getClass() == "EnumType") {
      writeEnumType(static_cast<EnumType *>(t));
   }
   else if (t->getClass() == "NumType") {
      NumType* numType = static_cast<NumType*>(t);

      if (numType->OIDType) {
         string typeAttribute;
         if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE" && t->Name != "TEXT") {
            typeAttribute = " name=\"" + t->Name + "\"";
         }

         gml.writeln("<xsd:simpleType" + typeAttribute + ">");
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:int\"/>");
         gml.writelnDecNestLevel("</xsd:simpleType>");
      }
      else {
         writeNumType(numType);
      }
   }
   else if (t->getClass() == "LineType") {
      writeLineType(static_cast<LineType *>(t));
   }
   else if (t->getClass() == "FormattedType") {
      writeFormattedType(static_cast<FormattedType *>(t));
   }
   else if (t->getClass() == "BlackboxType") {
      writeBlackboxType(static_cast<BlackboxType *>(t));
   }
   else if (t->getClass() == "MultiValue") {
      writeMultiValue(static_cast<MultiValue*>(t));
   }
   else if (t->getClass() == "Class" && t->Name != "ANYCLASS" && t->Name != "ANYSTRUCTURE") {
      writeClassType(static_cast<Class*>(t));
   }
   else {
      gml.writeln("<xsd:complexType" + t->Name + ">");
      gml.writelnIncNestLevel("<xsd:simpleContent>");
      gml.writelnIncNestLevel("<xsd:extension base=\"xsd:string\"/>");
      gml.writelnDecNestLevel("</xsd:simpleContent>");
      gml.writelnDecNestLevel("</xsd:complexType>");
   }
}

void GmlOutput::writeTextType(metamodel::TextType *t) {
   DomainType* domainType = static_cast<DomainType*>(t);
   string typeAttribute = "";

   if (domainType->Name != "" && domainType->Name != "TOP" && domainType->Name != "TYPE" && domainType->Name != "TEXT") {
      typeAttribute = " name=\"" + domainType->Name + "\"";
   }

   gml.writeln("<xsd:simpleType" + typeAttribute + ">");

   if (t->Kind == TextType::Text) {
      gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:normalizedString\">");
   }
   else {
      gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:string\">");
   }



   if (t->MaxLength > 0) {
      gml.writelnIncNestLevel("<xsd:maxLength value=\"" + to_string(t->MaxLength) + "\"/>");
      gml.decNestLevel();
   }


   gml.writeln("</xsd:restriction>");
   gml.writelnDecNestLevel("</xsd:simpleType>");
}

void GmlOutput::writeCoordType(metamodel::CoordType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + t->Name + "\"";
   }

   gml.writeln("<xsd:complexType" + typeAttribute + ">");
   gml.writelnIncNestLevel("<xsd:complexContent>");

   string base = "gml:PointPropertyType";

   if (t->Super != nullptr) {
      get_path(t->Super);
   }

   gml.writelnIncNestLevel("<xsd:restriction base=\"" + base + "\">");
   gml.writelnIncNestLevel("<xsd:sequence>");
   gml.writelnIncNestLevel("<xsd:element ref=\"gml:Point\"/>");
   gml.writelnDecNestLevel("</xsd:sequence>");
   gml.writelnDecNestLevel("</xsd:restriction>");
   gml.writelnDecNestLevel("</xsd:complexContent>");
   gml.writelnDecNestLevel("</xsd:complexType>");
}

void GmlOutput::writeEnumType(metamodel::EnumType *t) {
   string typeAttribute = "";
   bool hasDomain = true;
   bool isFinal = t->Final;

   string baseClass = t->getBaseClass();

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      hasDomain = true;
      typeAttribute = " name=\"" + t->Name + "\"";
   }

   if (t->TopNode->Node.size() == 2) {
      ;
      t->TopNode->Node.back();

      if (t->TopNode->Node.front()->Name == "false" && t->TopNode->Node.back()->Name == "true") {
         gml.writeln("<xsd:simpleType" + typeAttribute + ">");
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:boolean\">");
         gml.writelnDecNestLevel("</xsd:restriction>");
         gml.writeln("</xsd:simpleType>");

         return;
      }
   }

   if (!isFinal) {
      string base = "gml:CodeWithAuthorityType";

      if (t->Super != nullptr) {
         base = t->Super->Name;
      }

      gml.writeln("<xsd:complexType" + typeAttribute + ">");
      gml.writelnIncNestLevel("<xsd:simpleContent>");
      gml.writelnIncNestLevel("<xsd:restriction base=\"" + base + "\">");
      gml.writelnIncNestLevel("<xsd:attribute name=\"codeSpace\" type=\"xsd:anyURI\" use=\"required\"/>");
      gml.writelnDecNestLevel("</xsd:restriction>");
      gml.writelnDecNestLevel("</xsd:simpleContent>");
      gml.writelnDecNestLevel("</xsd:complexType>");

      list<string> enums;

      for (auto node : t->TopNode->Node) {
         buildEnum(&enums, "", node);
      }

      codelistKeys.push_back(t->Name);
      codelistValues.push_back(enums);
      //codelist.insert({ t->Name, enums });
   }
   else {
      gml.writeln("<xsd:simpleType" + typeAttribute + ">");
      gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:normalizedString\">");

      gml.incNestLevel();

      list<string> enums;

      for (auto node : t->TopNode->Node) {
         buildEnum(&enums, "", node);
      }

      for (auto entry : enums) {
         gml.writeln("<xsd:enumeration value=\"" + entry + "\"/>");
      }

      //for (auto v : t->ETVT) {
      //   xsd.writeln("<xsd:enumeration value=\"" + v->Name + "\"/>");
      //}

      gml.writelnDecNestLevel("</xsd:restriction>");
      gml.writelnDecNestLevel("</xsd:simpleType>");
   }
}

void GmlOutput::writeNumType(metamodel::NumType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + t->Name + "\"";
   }

   gml.writeln("<xsd:simpleType" + typeAttribute + ">");

   string min = t->Min;
   string max = t->Max;

   if (t->Min.find(".") < t->Min.length()) {

      if (t->Min.find("S") < t->Min.length() ||
         t->Min.find("e") < t->Min.length() ||
         t->Min.find("E") < t->Min.length()) {
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:double\">");
      }
      else {
         gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:decimal\">");
      }
      
   }
   else {
      gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");

      long long minll = stoll(t->Min);
      if (minll >= INT_MAX) {
         minll = INT_MAX;
      }

      long long maxll = stoll(t->Max);
      if (maxll >= INT_MAX) {
         maxll = INT_MAX;
      }

      min = to_string(minll);
      max = to_string(maxll);
   }

   if (util::starts_with(max, "+")) {
      max.erase(0, 1);
   }

   gml.writelnIncNestLevel("<xsd:minInclusive value=\"" + min + "\"/>");
   gml.writeln("<xsd:maxInclusive value=\"" + max + "\"/>");
   gml.writelnDecNestLevel("</xsd:restriction>");
   gml.writelnDecNestLevel("</xsd:simpleType>");
}

void GmlOutput::writeLineType(metamodel::LineType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + t->Name + "\"";
   }

   string base;

   if (t->Super != nullptr) {
      base = getScopedName(t->Super);
   }
   else if(t->Kind == t->Area || t->Kind == t->Surface){
      base = "gml:SurfacePropertyType";
   }
   else {
      base = "gml:CurvePropertyType";
   }

   gml.writeln("<xsd:complexType" + typeAttribute + ">");
   gml.writelnIncNestLevel("<xsd:complexContent>");
   gml.writelnIncNestLevel("<xsd:restriction base=\"" + base + "\">");
   gml.writelnIncNestLevel("<xsd:sequence>");



   if (t->Kind == t->Area || t->Kind == t->Surface) {
      gml.writelnIncNestLevel("<xsd:element ref=\"gml:Polygon\"/>");
   }
   else {
      gml.writelnIncNestLevel("<xsd:element ref=\"gml:AbstractCurve\"/>");
   }

   gml.writelnDecNestLevel("</xsd:sequence>");
   gml.writelnDecNestLevel("</xsd:restriction>");
   gml.writelnDecNestLevel("</xsd:complexContent>");
   gml.writelnDecNestLevel("</xsd:complexType>");
}

void GmlOutput::writeFormattedType(metamodel::FormattedType *t) {
   string typeAttribute = "";

   if (t->_attr != nullptr) {
   }
   else if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + t->Name + "\"";
   }

   string name = t->Name;
   if (t->BaseFormattedType != nullptr) {
      name = t->BaseFormattedType->Name;
   }
   else if (t->Struct != nullptr) {
      name = t->Struct->Name;
   }

   string base = "";
   bool hasBaseDomain = true;

   if (name == "XMLDate") {
      base = "xsd:date";
   }
   else if (name == "XMLDateTime") {
      base = "xsd:dateTime";
   }
   else if (name == "XMLTime") {
      base = "xsd:time";
   }
   else {
      hasBaseDomain = false;
      base = "xsd:normalizedString";
   }

   gml.writeln("<xsd:simpleType" + typeAttribute + ">");
   gml.writelnIncNestLevel("<xsd:restriction base=\"" + base + "\">");

   if (!hasBaseDomain) {
      string regex = "([\\+\\-0-9\\.]+):([\\+\\-0-9\\.]+):([\\+\\-0-9\\.]+)";
      gml.writelnIncNestLevel("<xsd:pattern value=\"" + regex + "\"/>");
      gml.decNestLevel();
   }

   gml.writeln("</xsd:restriction>");
   gml.writelnDecNestLevel("</xsd:simpleType>");
}

void GmlOutput::writeBlackboxType(metamodel::BlackboxType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "BLACKBOX") {
      typeAttribute = " name=\"" + t->Name + "\"";
   }

   if (t->Kind == t->Xml) {
      gml.writeln("<xsd:complexType" + typeAttribute + ">");
      gml.writelnIncNestLevel("<xsd:sequence>");
      gml.writelnIncNestLevel("<xsd:any namespace=\"##any\" minOccurs=\"0\" maxOccurs=\"unbounded\" processContents=\"lax\"/>");
      gml.writelnDecNestLevel("</xsd:sequence>");
      gml.writelnDecNestLevel("</xsd:complexType>");
   }
   else {
      gml.writeln("<xsd:simpleType" + typeAttribute + ">");
      gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:base64Binary\">");
      gml.writeln("</xsd:restriction>");
      gml.writelnDecNestLevel("</xsd:simpleType>");
   }
}

void GmlOutput::writeReferenceType(metamodel::ReferenceType* t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   gml.writeln("<xsd:annotation>");
   gml.writelnIncNestLevel("<xsd:appinfo>");
   gml.writelnIncNestLevel("<gml:targetElement>" + getScopedName(t->_baseclass) + "</gml:targetElement>");
   gml.writelnDecNestLevel("</xsd:appinfo>");
   gml.writelnDecNestLevel("</xsd:annotation>");
}

void GmlOutput::writeMultiValue(metamodel::MultiValue* t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   gml.writeln("<xsd:complexType" + typeAttribute + ">");
   gml.writelnIncNestLevel("<xsd:sequence>");

   gml.incNestLevel();

   if(t->BaseType != nullptr) {
      gml.writeln("<xsd:element ref=\"" + getScopedName(t->BaseType) + "\"/>");
   }

   gml.decNestLevel();

   gml.writelnDecNestLevel("</xsd:sequence>");
   gml.writeln("</xsd:complexType>");
}

void GmlOutput::writeClassType(metamodel::Class* t) {
   string typeAttribute = "";

   if (t->_attr == nullptr && t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   if (t->Name == "ANYCLASS" || t->Name == "ANYSTRUCTURE") {
      gml.writeln("<xsd:simpleType>");
      gml.writelnIncNestLevel("<xsd:restriction base=\"xsd:normalizedString\"/>");
      gml.writelnDecNestLevel("</xsd:simpleType>");
   }
   else {
      gml.writeln("<xsd:complexType" + typeAttribute + ">");
      gml.writelnIncNestLevel("<xsd:sequence>");

      /*if (t->Super->Sub.size() > 0) {
         gml.writelnIncNestLevel("<xsd:choice>");

         gml.incNestLevel();

         for (auto* entry : t->Super->Sub) {
            gml.writeln("<xsd:element name=\"" + getScopedName(entry) + "\" type=\"" + getScopedName(entry) + "\"/>");
         }

         gml.writeln("<xsd:element name=\"" + getScopedName(t->Super) + "\" type=\"" + getScopedName(t->Super) + "\"/>");

         gml.decNestLevel();

         gml.writeln("</xsd:choice>");
      }
      else {*/
         gml.writelnIncNestLevel("<xsd:element ref=\"" + getScopedName(t->Super) + "\"/>");
      /* } */

      gml.writelnDecNestLevel("</xsd:sequence>");
      gml.writelnDecNestLevel("</xsd:complexType>");
   }

   
}

void GmlOutput::write_codelists(Model *m) {
   if (codelistKeys.empty()) {
      return;
   }

   gml.writeln("<xsd:annotation>");
   gml.writelnIncNestLevel("<xsd:appinfo source=\"http://www.interlis.ch/ili2c\">");

   gml.incNestLevel();

   int oid = 1;
   for (int i=0;i<codelistKeys.size();i++)
   {
      string key = codelistKeys[i];
      list<string> values = codelistValues[i];
      gml.writeln("<gml:Dictionary gml:id=\"o" + to_string(oid++) + "\">");
      gml.writelnIncNestLevel("<gml:identifier codeSpace=\"http://www.interlis.ch/INTERLIS2.3/GML32/" + m->Name + "\">" + key + "</gml:identifier>");

      string codeSpace = "http://www.interlis.ch/INTERLIS2.3/GML32/" + m->Name + "/" + key;

      for (auto const& value : values) {
         gml.writeln("<gml:dictionaryEntry>");
         gml.writelnIncNestLevel("<gml:Definition gml:id=\"o" + to_string(oid++) + "\">");
         gml.writelnIncNestLevel("<gml:identifier codeSpace=\"" + codeSpace + "\">" + value + "</gml:identifier>");
         gml.writelnDecNestLevel("</gml:Definition>");
         gml.writelnDecNestLevel("</gml:dictionaryEntry>");
      }

      gml.writelnDecNestLevel("</gml:Dictionary>");
   }

   gml.writelnDecNestLevel("</xsd:appinfo>");
   gml.writelnDecNestLevel("</xsd:annotation>");

   codelistKeys.clear();
   codelistValues.clear();
}

void GmlOutput::write_linetables() {
   for (auto *a : areaAttributes) {
      string linetableName = a->AttrParent->Name + "." + a->Name;
      string baseElement = "gml:AbstractFeature";
      gml.writeln("<xsd:element name=\"" + linetableName + "\" type=\"" + linetableName + "Type\" substitutionGroup=\"" + baseElement + "\"/>");
      gml.writeln("<xsd:complexType  name=\"" + linetableName + "Type\">");
      gml.writelnIncNestLevel("<xsd:complexContent>");
      string baseXsdType = "gml:AbstractFeatureType";
      gml.writelnIncNestLevel("<xsd:extension base=\"" + baseXsdType + "\">");
      gml.writelnIncNestLevel("<xsd:sequence>");
      gml.writelnIncNestLevel("<xsd:element name=\"geometry\" type=\"gml:CurvePropertyType\"/>");

      LineType *lt = static_cast<LineType *>(a->Type);
      if (lt->LAStructure != nullptr) {
         gml.writeln("<xsd:element name=\"lineattr\">");
         gml.writelnIncNestLevel("<xsd:complexType>");
         gml.writelnIncNestLevel("<xsd:sequence>");
         gml.writelnIncNestLevel("<xsd:element ref=\"" + getScopedName(lt->LAStructure) + "\"/>");
         gml.writelnDecNestLevel("</xsd:sequence>");
         gml.writeln("<xsd:attributeGroup ref=\"gml:OwnershipAttributeGroup\"/>");
         gml.writelnDecNestLevel("</xsd:complexType>");
         gml.writelnDecNestLevel("</xsd:element>");
      }

      gml.writelnDecNestLevel("</xsd:sequence>");
      gml.writelnDecNestLevel("</xsd:extension>");
      gml.writelnDecNestLevel("</xsd:complexContent>");
      gml.writelnDecNestLevel("</xsd:complexType>");
   }

   areaAttributes.clear();
}

void GmlOutput::buildEnum(list<string>* list, string prefix, metamodel::EnumNode* enumNode) {
   if (prefix.length() > 0) {
      prefix += ".";
   }

   if (enumNode->Node.size() == 0) {
      list->push_back(prefix + enumNode->Name);
   }
   else {
      for (auto* node : enumNode->Node) {
         buildEnum(list, prefix + enumNode->Name, node);
      }
   }
}

string GmlOutput::getModelName(metamodel::MMObject* o) {
   if (o == nullptr) {
      return "";
   }
   else if (!dynamic_cast<MetaElement*>(o)) {
      return "";
   }

   MetaElement* e = dynamic_cast<MetaElement*>(o);
   if (e->getClass() == "EnumNode") {
      EnumNode* n = dynamic_cast<EnumNode*>(e);
      return getModelName(n->EnumType);
   }
   else if (e->getClass() == "AttrOrParam") {
      AttrOrParam* a = dynamic_cast<AttrOrParam*>(e);
      return getModelName(a->AttrParent);
   }
   else if (e->getClass() == "Argument") {
      Argument* a = dynamic_cast<Argument*>(e);
      return getModelName(a->Function);
   }
   else if (e->isSubClassOf("Type")) {
      Type* t = dynamic_cast<Type*>(e);

      if (t->Name == "C1" || t->Name == "C2" || t->Name == "C3") {
         return getModelName(t->_other_type);
      }
      else if (t->ElementInPackage != nullptr) {
         // class / structure
         return getModelName(t->ElementInPackage);
      }
      else if (t->LTParent != nullptr) {
         // class attribute
         return getModelName(t->LTParent);
      }
      else if (t->LFTParent != nullptr) {
         // function argument
         return getModelName(t->LFTParent);
      }
      else {
         // local attribute type
         return getModelName(t->_attr);
      }
   }

   string model = "";
   while (e != nullptr) {
      if (e->getClass() == "Model") {
         return e->Name;
      }

      e = e->ElementInPackage;
   }

   return "";
}

string GmlOutput::getScopedName(metamodel::MetaElement* o) {
   string modelName = getModelName(o);
   string elementName = o->Name;

   map<metamodel::MetaElement*, string>::iterator foundIter = ObjectToNameDefinition.find(o);
   if (foundIter != ObjectToNameDefinition.end())
   {
      elementName = foundIter->second;
   }

   if (modelName != currentModel) {
      elementName = modelName + ":" + elementName;
   }
   
   return elementName;
}

list<metamodel::ExtendableME*> GmlOutput::getClassAttributes(metamodel::Class* c) {
   list<ExtendableME*> attributes;
   list<ExtendableME*> classesToScan;

   ExtendableME* attributeClass = c;

   while (attributeClass != nullptr) {

      if (attributeClass->getClass() == "Class") {
         classesToScan.push_front(attributeClass);

      }

      attributeClass = attributeClass->Super;
   }

   for (auto cts : classesToScan) {
      Class* ac = static_cast<Class*>(cts);

      if (ac->Kind == Class::Association) {
         for (auto ra : ac->_roleaccess) {
            string findString = ra->Name;
            auto it = std::find_if(attributes.begin(), attributes.end(), [&findString](metamodel::ExtendableME* obj) {return obj->Name == findString; });

            if (it != attributes.end()) {
               it = attributes.erase(it);
               attributes.insert(it, ra);
            }
            else {
               attributes.push_back(ra);
            }
         }
      }

      for (auto ca : ac->ClassAttribute) {

         string findString = ca->Name;
         auto it = std::find_if(attributes.begin(), attributes.end(), [&findString](metamodel::ExtendableME* obj) {return obj->Name == findString; });

         if (it != attributes.end()) {
            it = attributes.erase(it);
            attributes.insert(it, ca);
         }
         else {
            attributes.push_back(ca);
         }
      }

      if (ac->Kind != Class::Association) {
         for (auto ra : ac->_roleaccess) {
            string findString = ra->Name;
            auto it = std::find_if(attributes.begin(), attributes.end(), [&findString](metamodel::ExtendableME* obj) {return obj->Name == findString; });

            if (it != attributes.end()) {
               it = attributes.erase(it);
               attributes.insert(it, ra);
            }
            else {
               attributes.push_back(ra);
            }
         }
      }
   }

   return attributes;
}

bool GmlOutput::isRealClass(metamodel::Class* c) {
   if (c->Kind != Class::Association) {
      return true;
   }

   if (c->Role.size() > 2) {
      return true;
   }

   if (c->Role.size() < 2) {
      if (getClassAttributes(c).size() > 0) {
         return true;
      }

      return false;
   }

   Role* role1 = c->Role.front();
   Role* role2 = c->Role.back();

   int role1Min = role1->Multiplicity.Min;
   int role1Max = role1->Multiplicity.Max;

   if (role1Min == -1) {
      role1Min = 0;
   }

   if (role1Max == -1) {
      if (role2->Strongness == Role::Comp) {
         role1Max = 1;
      }
      else {
         role1Max = INT_MAX;
      }
   }

   int role2Min = role2->Multiplicity.Min;
   int role2Max = role2->Multiplicity.Max;

   if (role2Min == -1) {
      role2Min = 0;
   }

   if (role2Max == -1) {
      if (role2->Strongness == Role::Comp) {
         role2Max = 1;
      }
      else {
         role2Max = INT_MAX;
      }
   }

   if (role1Min == 1 && role1Max == 1) {
      return false;
   }

   if (role2Min == 1 && role2Max == 1) {
      return false;
   }

   if (getClassAttributes(c).size() > 0) {
      return true;
   }

   if (role1Max > 1 && role2Max > 1) {
      return true;
   }

   return false;
}
