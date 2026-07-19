#pragma once

#include "XsdOutput.h"

#include "../metamodel/MetaModelOutput.h"
#include "../util/StringMap.h"
#include "../util/StringUtil.h"
#include "../util/Logger.h"

#include <climits>
#include <regex>
#include <unordered_map>

using namespace util;
using namespace metamodel;
using namespace output;

XsdOutput::XsdOutput(string xsd_file,string iliversion, string ilic_version) {
   this->xsd_file = xsd_file;
   this->iliversion = iliversion;
   this->ilic_version = ilic_version;
}

void XsdOutput::createEmptyFile() {
   xsd.openFile(xsd_file);
   xsd.closeFile();
}
   
void XsdOutput::preVisit(void) {
   xsd.openFile(xsd_file);

   xsd.writeln("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>");
   
   xsd.writeln("<xsd:schema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\""
      " xmlns=\"http://www.interlis.ch/INTERLIS2.3\" xmlns:ili2c=\"http://www.interlis.ch/ili2c\""
      " targetNamespace=\"http://www.interlis.ch/INTERLIS2.3\""
      " elementFormDefault=\"qualified\" attributeFormDefault=\"unqualified\""
      ">"
   );

   Log.debug("write annotations");
   writeAnnotations();
   
   Log.debug("write fix types 1");
   writeFixTypes1();

   Log.debug("write alias tables");
   writeAliasTables();

   Log.debug("write fix types 2");
   writeFixTypes2();

   Log.debug("write datasection");
   writeDatasection();
}

void XsdOutput::postVisit() {
   Log.debug("write interlis reference types");
   writeInterlisReferenceTypes();

   Log.debug("write header section");
   writeHeadersection();

   xsd.writelnDecNestLevel("</xsd:schema>");
   xsd.closeFile();
}

void XsdOutput::preVisitModel(metamodel::Model* m) {
   if (m->Name == "INTERLIS") {
      ignoreVisit();
   }
}

void XsdOutput::visitModel(metamodel::Model* m) {
   if (m->Name == "INTERLIS") {
      ignoreVisit();
   }
}

void XsdOutput::postVisitModel(metamodel::Model* m) {
   if (m->Name == "INTERLIS") {
      ignoreVisit();
   }
}

void XsdOutput::postVisitSubModel(metamodel::SubModel* s) {
   xsd.writeln("<xsd:complexType name=\"" + get_path(s) + "\">");
   xsd.writelnIncNestLevel("<xsd:sequence>");
   xsd.writelnIncNestLevel("<xsd:choice minOccurs=\"0\" maxOccurs=\"unbounded\">");

   xsd.incNestLevel();

   //std::map<string,string> allClassNames;

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

            string path = get_path(c);

            if (path.rfind("INTERLIS.",0) == 0) {

            }
            else {
               allClassNames.push_back(c->Name);
               allPathNames.push_back(path);
            }
         }
      }
   }

   for (int i = 0; i < allPathNames.size();i++) {
      string path = allPathNames.at(i);

      bool suppress = true;

      if (!suppress) {

      }
      else {
         xsd.writeln("<xsd:element name=\"" + path + "\" type=\"" + path + "\"/>");
      }

      
   }

   xsd.decNestLevel();

   xsd.writeln("</xsd:choice>");
   xsd.writelnDecNestLevel("</xsd:sequence>");

   xsd.writeln("<xsd:attribute name=\"BID\" type=\"IliID\" use=\"required\"/>");

   xsd.writelnDecNestLevel("</xsd:complexType>");
}

void XsdOutput::preVisitClass(metamodel::Class* c) {
   all_classes.push_back(c);

   if (c->_attr != nullptr) {
      return;
   }

   classAttributes.clear();
   classAttributes = getClassAttributes(c);

   if (c->Kind == Class::Association && classAttributes.size() == 0) {
      return;
   }

   /*if (!isRealClass(c)) {
      return;
   }*/

   xsd.writeln("<xsd:complexType name=\"" + get_path(c) + "\">");

   if (c->isDomainType) {
      xsd.writelnIncNestLevel("<xsd:simpleContent>");
      xsd.writelnIncNestLevel("<xsd:extension base=\"xsd:string\"/>");
      xsd.writelnDecNestLevel("</xsd:simpleContent>");
      xsd.writelnDecNestLevel("</xsd:complexType>");

      return;
   }

   xsd.writelnIncNestLevel("<xsd:sequence>");

   xsd.incNestLevel();
}

void XsdOutput::visitClass(metamodel::Class* c) {
   if (c->_attr != nullptr) {
      return;
   }

   if(c->isDomainType) {
      return;
   }

   if (c->Kind == Class::Association && classAttributes.size() == 0) {
      return;
   }

   /*if (!isRealClass(c)) {
      return;
   }*/

   for (auto v : classAttributes) {
      if (v->getClass() == "AttrOrParam") {
         writeAttrOrParam(static_cast<AttrOrParam*>(v));
      }
      else if (v->getClass() == "Role") {
         string minOccurs = "";

         Role* role = static_cast<Role*>(v);

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
            xsd.writeln("<xsd:element name=\"" + role->Name + "\" type=\"RoleType\"/>");
         }
         else {
            xsd.writeln("<xsd:element name=\"" + role->Name + "\"" + minOccurs + ">");
            xsd.writelnIncNestLevel("<xsd:complexType>");

            xsd.incNestLevel();

            if (role->Association->ClassAttribute.size() > 0) {
               xsd.writeln("<xsd:sequence>");
               xsd.writelnIncNestLevel("<xsd:element name=\"" + get_path(role->Association) + "\" type=\"" + get_path(role->Association) + "\"/>");
               xsd.writelnDecNestLevel("</xsd:sequence>");
            }

            xsd.writeln("<xsd:attribute name=\"REF\" type=\"IliID\" use=\"required\"/>");
            xsd.writeln("<xsd:attribute name=\"BID\" type=\"IliID\"/>");
            xsd.writeln("<xsd:attribute name=\"ORDER_POS\" type=\"xsd:positiveInteger\"/>");

            xsd.decNestLevel();
            xsd.writeln("</xsd:complexType>");
            xsd.writelnDecNestLevel("</xsd:element>");
         }


      }
   }
}

void XsdOutput::postVisitClass(metamodel::Class* c) {
   if (c->_attr != nullptr) {
      return;
   }

   if (c->isDomainType) {
      return;
   }

   if (c->Kind == Class::Association && classAttributes.size() == 0) {
      return;
   }

   /*if (!isRealClass(c)) {
      return;
   }*/

   xsd.writelnDecNestLevel("</xsd:sequence>");

   if (c->Kind == c->ClassVal) {
      /*if (c->Oid != nullptr) {*/
         xsd.writeln("<xsd:attribute name=\"TID\" type=\"IliID\" use=\"required\"/>");
      /*}*/
   }
   else if (c->Kind == c->Association) {
   }

   xsd.writelnDecNestLevel("</xsd:complexType>");
}

void XsdOutput::visitDomainType(metamodel::DomainType* t) {
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

void XsdOutput::writeAnnotations() {
   xsd.writelnIncNestLevel("<xsd:annotation>");

   xsd.writelnIncNestLevel("<xsd:appinfo source=\"http://www.interlis.ch/ili2c/ili2cversion\">"
      + ilic_version + "</xsd:appinfo>"
   );

   list<Model*> allModels = get_all_models();
   allModels.sort([](const Model* a, const Model* b) { return a->Name < b->Name; });

   for (Model *m : allModels) {
      if (m->Name == "INTERLIS") {
         continue;
      }

      xsd.writeln("<xsd:appinfo source=\"http://www.interlis.ch/ili2c\">");
      xsd.writelnIncNestLevel("<ili2c:model>" + m->Name + "</ili2c:model>");
      xsd.writeln("<ili2c:modelVersion>" + m->Version + "</ili2c:modelVersion>");

      if (!m->VersionExplanation.empty()) {
         xsd.writeln("<ili2c:modelVersionExplanation>" + m->VersionExplanation + "</ili2c:modelVersionExplanation>");
      }

      xsd.writeln("<ili2c:modelAt>" + m->At + "</ili2c:modelAt>");
      xsd.writeln("<ili2c:textMinimalCharset>" "ili23AnnexB" "</ili2c:textMinimalCharset>");
      xsd.writelnDecNestLevel("</xsd:appinfo>");
   }

   xsd.writelnDecNestLevel("</xsd:annotation>");
}

void XsdOutput::writeFixTypes1() {
   xsd.writeln("<xsd:element name=\"TRANSFER\" type=\"Transfer\"/>");

   xsd.writeln("<xsd:simpleType name=\"IliID\">");
   xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:string\">");
   xsd.writelnIncNestLevel("<xsd:pattern value=\"([a-zA-Z_][0-9a-zA-Z_\\-\\.]*:)?[0-9a-zA-Z_][0-9a-zA-Z_\\-\\.]*\"/>");
   xsd.writelnDecNestLevel("</xsd:restriction>");
   xsd.writelnDecNestLevel("</xsd:simpleType>");

   xsd.writeln("<xsd:complexType name=\"Transfer\">");
   xsd.writelnIncNestLevel("<xsd:sequence>");
   xsd.writelnIncNestLevel("<xsd:element name=\"HEADERSECTION\" type=\"HeaderSection\"/>");
   xsd.writeln("<xsd:element name=\"DATASECTION\" type=\"DataSection\"/>");
   xsd.writelnDecNestLevel("</xsd:sequence>");
   xsd.writelnDecNestLevel("</xsd:complexType>");
}

void XsdOutput::writeAliasTables() {
   //Log.warning("writeAliasTables not implemented yet");
}

void XsdOutput::writeFixTypes2() {
   xsd.writeln("<xsd:complexType name=\"Models\">");
   xsd.writelnIncNestLevel("<xsd:sequence>");
   xsd.writelnIncNestLevel("<xsd:element name=\"MODEL\" type=\"Model\" minOccurs=\"0\" maxOccurs=\"unbounded\"/>");
   xsd.writelnDecNestLevel("</xsd:sequence>");
   xsd.writelnDecNestLevel("</xsd:complexType>");

   xsd.writeln("<xsd:complexType name=\"Model\">");
   xsd.writelnIncNestLevel("<xsd:attribute name=\"NAME\" type=\"INTERLIS.NAME\" use=\"required\"/>");
   xsd.writeln("<xsd:attribute name=\"VERSION\" type=\"xsd:string\" use=\"required\"/>");
   xsd.writeln("<xsd:attribute name=\"URI\" type=\"xsd:anyURI\" use=\"required\"/>");
   xsd.writelnDecNestLevel("</xsd:complexType>");

   bool addAliasTable = false;

   if(get_all_models().size() > 0) {
      Model *m = get_all_models().back();

      //Log.warning("ili2c.ili23xsd.addAliasTableDefault not implemented yet");
      for (MetaAttribute* ma : m->MetaAttribute) {
         if (ma->Name == "ili2c.ili23xsd.addAliasTableDefault") {
            if (ma->Value == "true") {
               addAliasTable = true;
            }
         }
      }
   }

   if(addAliasTable) {
      xsd.writeln("<xsd:complexType name=\"Alias\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"ENTRIES\" type=\"Entries\" minOccurs=\"0\" maxOccurs=\"unbounded\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");

      xsd.writeln("<xsd:complexType name=\"Entries\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:choice maxOccurs=\"unbounded\">");
      xsd.writelnIncNestLevel("<xsd:element name=\"TAGENTRY\" type=\"Tagentry\"/>");
      xsd.writeln("<xsd:element name=\"VALENTRY\" type=\"Valentry\"/>");
      xsd.writeln("<xsd:element name=\"DELENTRY\" type=\"Delentry\"/>");
      xsd.writelnDecNestLevel("</xsd:choice>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writeln("<xsd:attribute name=\"FOR\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");

      xsd.writeln("<xsd:complexType name=\"Tagentry\">");
      xsd.writelnIncNestLevel("<xsd:attribute name=\"FROM\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writeln("<xsd:attribute name=\"TO\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");

      xsd.writeln("<xsd:complexType name=\"Valentry\">");
      xsd.writelnIncNestLevel("<xsd:attribute name=\"TAG\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writeln("<xsd:attribute name=\"ATTR\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writeln("<xsd:attribute name=\"FROM\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writeln("<xsd:attribute name=\"TO\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");

      xsd.writeln("<xsd:complexType name=\"Delentry\">");
      xsd.writelnIncNestLevel("<xsd:attribute name=\"TAG\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writeln("<xsd:attribute name=\"ATTR\" type=\"xsd:string\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   }

   xsd.writeln("<xsd:complexType name=\"RoleType\">");
   xsd.writelnIncNestLevel("<xsd:simpleContent>");
   xsd.writelnIncNestLevel("<xsd:extension base=\"xsd:string\">");
   xsd.writelnIncNestLevel("<xsd:attribute name=\"REF\" type=\"IliID\" use=\"required\"/>");
   xsd.writeln("<xsd:attribute name=\"BID\" type=\"IliID\"/>");
   xsd.writeln("<xsd:attribute name=\"ORDER_POS\" type=\"xsd:positiveInteger\"/>");
   xsd.writelnDecNestLevel("</xsd:extension>");
   xsd.writelnDecNestLevel("</xsd:simpleContent>");
   xsd.writelnDecNestLevel("</xsd:complexType>");
}

void XsdOutput::writeDatasection() {
   xsd.writeln("<xsd:complexType name=\"DataSection\">");
   xsd.writelnIncNestLevel("<xsd:sequence>");
   xsd.writelnIncNestLevel("<xsd:choice minOccurs=\"0\" maxOccurs=\"unbounded\">");

   xsd.incNestLevel();

   list<Model*> allModels = get_all_models();
   allModels.sort([](const Model* a, const Model* b) { return a->Name < b->Name; });

   for (Model *m : allModels) {
      if (m->Name == "INTERLIS" || m->Kind == Model::TypeM) {
         continue;
      }

      for (auto *me : m->Element) {
         if (me == nullptr) {
            continue;
         }

         if (me->getClass() == "SubModel") {
            SubModel* subModel = static_cast<SubModel*>(me);

            string searchDataUnit = get_path(subModel) + ".BASKET";
            for (auto *du : get_all_dataunits()) {
               string dataunit_path = get_path(du);

               if (searchDataUnit == dataunit_path) {
                  if (du->Abstract) {
                     break;
                  }

                  xsd.writeln("<xsd:element name=\"" + get_path(me) + "\" type=\"" + get_path(me) + "\"/>");

                  break;
               }
            }

            
         }
      }
   }

   xsd.decNestLevel();

   xsd.writeln("</xsd:choice>");
   xsd.writelnDecNestLevel("</xsd:sequence>");
   xsd.writelnDecNestLevel("</xsd:complexType>");
}

void XsdOutput::writeInterlisReferenceTypes() {
   //if (addAllInterlisTypes || refCoordValue) {
      xsd.writeln("<xsd:complexType name=\"CoordValue\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"C1\" type=\"xsd:double\"/>");
      xsd.writeln("<xsd:element name=\"C2\" type=\"xsd:double\" minOccurs=\"0\"/>");
      xsd.writeln("<xsd:element name=\"C3\" type=\"xsd:double\" minOccurs=\"0\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || refArcPoint) {
      xsd.writeln("<xsd:complexType name=\"ArcPoint\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"C1\" type=\"xsd:double\"/>");
      xsd.writeln("<xsd:element name=\"C2\" type=\"xsd:double\"/>");
      xsd.writeln("<xsd:element name=\"C3\" type=\"xsd:double\" minOccurs=\"0\"/>");
      xsd.writeln("<xsd:element name=\"A1\" type=\"xsd:double\"/>");
      xsd.writeln("<xsd:element name=\"A2\" type=\"xsd:double\"/>");
      xsd.writeln("<xsd:element name=\"R\" type=\"xsd:double\" minOccurs=\"0\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}

   xsd.writeln("<xsd:simpleType name=\"INTERLIS.NAME\">");
   xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:normalizedString\">");
   xsd.writelnIncNestLevel("<xsd:maxLength value=\"255\"/>");
   xsd.writelnDecNestLevel("</xsd:restriction>");
   xsd.writelnDecNestLevel("</xsd:simpleType>");

   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.METAOBJECT)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.METAOBJECT\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"Name\" type=\"INTERLIS.NAME\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writeln("<xsd:attribute name=\"TID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.METAOBJECT_TRANSLATION)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.METAOBJECT_TRANSLATION\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"Name\" type=\"INTERLIS.NAME\"/>");
      xsd.writeln("<xsd:element name=\"NameInBaseLanguage\" type=\"INTERLIS.NAME\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writeln("<xsd:attribute name=\"TID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.AXIS)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.AXIS\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writeln("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.REFSYSTEM)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.REFSYSTEM\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"Name\" type=\"INTERLIS.NAME\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writeln("<xsd:attribute name=\"TID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.COORDSYSTEM)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.COORDSYSTEM\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"Name\" type=\"INTERLIS.NAME\"/>");
      xsd.writeln("<xsd:element name=\"Axis\">");
      xsd.writelnIncNestLevel("<xsd:complexType>");
      xsd.writelnIncNestLevel("<xsd:sequence>");

      map<string, string> coordSystems = { 
         {"INTERLIS.AXIS", "INTERLIS.AXIS"}
      };
      int coordSystemsMin = -1;
      int coordSystemsMax = -1;
      
      for (auto* c : all_classes) {
         string superPath = get_path(c->Super);

         if (superPath == "INTERLIS.COORDSYSTEM") {
            for (auto* a : c->ClassAttribute) {
               if (a->Name == "Axis") {
                  if (a->Type->getClass() == "MultiValue") {
                     MultiValue* multiValue = static_cast<MultiValue*>(a->Type);

                     coordSystems.insert({get_path(multiValue->BaseType),get_path(multiValue->BaseType) });

                     if (multiValue->Multiplicity.Min > coordSystemsMin && multiValue->Multiplicity.Min < multiValue->Multiplicity.Max) {
                        coordSystemsMin = multiValue->Multiplicity.Min;
                     }

                     if (multiValue->Multiplicity.Max > coordSystemsMax) {
                        coordSystemsMax = multiValue->Multiplicity.Max;
                     }
                  }
               }
            }
         }
      }

      if (coordSystems.size() > 1) {
         string minOccurs = "";
         string maxOccurs = "";

         if (coordSystemsMin > -1) {
            minOccurs = " minOccurs=\"" + to_string(coordSystemsMin) + "\"";
         }

         if (coordSystemsMax > -1) {
            maxOccurs = " maxOccurs=\"" + to_string(coordSystemsMax) + "\"";
         }

         xsd.writelnIncNestLevel("<xsd:choice" + minOccurs + maxOccurs + ">");

         xsd.incNestLevel();

         for (auto const& [key,value] : coordSystems) {
            xsd.writeln("<xsd:element name=\""+key+"\" type=\""+value+"\"/>");
         }

         xsd.decNestLevel();

         xsd.writeln("</xsd:choice>");
      }
      else {
         xsd.writelnIncNestLevel("<xsd:element name=\"INTERLIS.AXIS\" type=\"INTERLIS.AXIS\"  maxOccurs=\"3\"/>");
      }
      
      
      
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
      xsd.writelnDecNestLevel("</xsd:element>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writeln("<xsd:attribute name=\"TID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.SCALSYSTEM)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.SCALSYSTEM\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"Name\" type=\"INTERLIS.NAME\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writeln("<xsd:attribute name=\"TID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.SIGN)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.SIGN\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"Name\" type=\"INTERLIS.NAME\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writeln("<xsd:attribute name=\"TID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.TIMESYSTEMS.CALENDAR)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.TIMESYSTEMS.CALENDAR\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"Name\" type=\"INTERLIS.NAME\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writeln("<xsd:attribute name=\"TID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.TIMESYSTEMS.TIMEOFDAYSYS)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.TIMESYSTEMS.TIMEOFDAYSYS\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"Name\" type=\"INTERLIS.NAME\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writeln("<xsd:attribute name=\"TID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.TimeOfDay)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.TimeOfDay\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");

      xsd.writelnIncNestLevel("<xsd:element name=\"Hours\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"0\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"23\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writeln("<xsd:element name=\"Minutes\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"0\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"59\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writeln("<xsd:element name=\"Seconds\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:double\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"0.000\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"59.999\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.UTC)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.UTC\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");

      xsd.writelnIncNestLevel("<xsd:element name=\"Hours\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"0\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"23\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writeln("<xsd:element name=\"Minutes\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"0\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"59\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writeln("<xsd:element name=\"Seconds\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:double\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"0.000\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"59.999\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.GregorianDate)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.GregorianDate\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");

      xsd.writelnIncNestLevel("<xsd:element name=\"Year\" type=\"INTERLIS.GregorianYear\" minOccurs=\"0\"/>");

      xsd.writeln("<xsd:element name=\"Month\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"1\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"12\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writeln("<xsd:element name=\"Day\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"1\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"31\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.GregorianDateTime)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.GregorianDateTime\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");

      xsd.writelnIncNestLevel("<xsd:element name=\"Year\" type=\"INTERLIS.GregorianYear\" minOccurs=\"0\"/>");

      xsd.writeln("<xsd:element name=\"Month\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"1\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"12\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writeln("<xsd:element name=\"Day\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"1\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"31\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writelnIncNestLevel("<xsd:element name=\"Hours\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"0\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"23\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writeln("<xsd:element name=\"Minutes\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"0\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"59\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writeln("<xsd:element name=\"Seconds\" minOccurs=\"0\">");
      xsd.writelnIncNestLevel("<xsd:simpleType>");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:double\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"0.000\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"59.999\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writelnDecNestLevel("</xsd:element>");

      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.HALIGNMENT)) {
      xsd.writeln("<xsd:simpleType name=\"INTERLIS.HALIGNMENT\">");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:string\">");
      xsd.writelnIncNestLevel("<xsd:enumeration value=\"Left\"/>");
      xsd.writeln("<xsd:enumeration value=\"Center\"/>");
      xsd.writeln("<xsd:enumeration value=\"Right\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.VALIGNMENT)) {
      xsd.writeln("<xsd:simpleType name=\"INTERLIS.VALIGNMENT\">");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:string\">");
      xsd.writelnIncNestLevel("<xsd:enumeration value=\"Top\"/>");
      xsd.writeln("<xsd:enumeration value=\"Cap\"/>");
      xsd.writeln("<xsd:enumeration value=\"Half\"/>");
      xsd.writeln("<xsd:enumeration value=\"Base\"/>");
      xsd.writeln("<xsd:enumeration value=\"Bottom\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.URI)) {
      xsd.writeln("<xsd:simpleType name=\"INTERLIS.URI\">");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:normalizedString\">");
      xsd.writelnIncNestLevel("<xsd:maxLength value=\"1023\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.INTERLIS_1_DATE)) {
      xsd.writeln("<xsd:simpleType name=\"INTERLIS.INTERLIS_1_DATE\">");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:normalizedString\">");
      xsd.writelnIncNestLevel("<xsd:maxLength value=\"8\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.STANDARDOID)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.STANDARDOID\">");
      xsd.writelnIncNestLevel("<xsd:attribute name=\"OID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.I32OID)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.I32OID\">");
      xsd.writelnIncNestLevel("<xsd:attribute name=\"OID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.UUIDOID)) {
      xsd.writeln("<xsd:complexType name=\"INTERLIS.UUIDOID\">");
      xsd.writelnIncNestLevel("<xsd:attribute name=\"OID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   //}
   //if (addAllInterlisTypes || referencedTypes.contains(td.INTERLIS.GregorianYear)) {
      xsd.writeln("<xsd:simpleType name=\"INTERLIS.GregorianYear\">");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"1582\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"2999\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
   //}
}

void XsdOutput::writeHeadersection() {
   xsd.writeln("<xsd:complexType name=\"HeaderSection\">");
   xsd.writelnIncNestLevel("<xsd:sequence>");
   xsd.writelnIncNestLevel("<xsd:element name=\"MODELS\" type=\"Models\"/>");

   bool addAliasTable = false;
   if (addAliasTable) {
      xsd.writeln("<xsd:element name=\"ALIAS\" type=\"Alias\" minOccurs=\"0\"/>");
   }

   //if (refOidSpaces) {
      xsd.writeln("<xsd:element name=\"OIDSPACES\" type=\"OidSpaces\" minOccurs=\"0\"/>");
   //}

   xsd.writeln("<xsd:element name=\"COMMENT\" type=\"xsd:string\" minOccurs=\"0\"/>");
   xsd.writelnDecNestLevel("</xsd:sequence>");
   xsd.writeln("<xsd:attribute name=\"VERSION\" type=\"xsd:decimal\" use=\"required\" fixed=\"2.3\"/>");
   xsd.writeln("<xsd:attribute name=\"SENDER\" type=\"xsd:string\" use=\"required\"/>");
   xsd.writelnDecNestLevel("</xsd:complexType>");

   //if (refOidSpaces) {
      xsd.writeln("<xsd:complexType name=\"OidSpaces\">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"OIDSPACE\" type=\"OidSpace\" minOccurs=\"0\" maxOccurs=\"unbounded\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");

      xsd.writeln("<xsd:complexType name=\"OidSpace\">");
      xsd.writelnIncNestLevel("<xsd:attribute name=\"NAME\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writeln("<xsd:attribute name=\"OIDDOMAIN\" type=\"xsd:string\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");

   //}
   //if (addAllInterlisTypes || refIncrementalTypes) {
      xsd.writeln("<xsd:simpleType name=\"OperationType\">");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:string\">");
      xsd.writelnIncNestLevel("<xsd:enumeration value=\"INSERT\"/>");
      xsd.writeln("<xsd:enumeration value=\"UPDATE\"/>");
      xsd.writeln("<xsd:enumeration value=\"DELETE\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
      xsd.writeln("<xsd:simpleType name=\"TransferKindType\">");
      xsd.writeln("<xsd:restriction base=\"xsd:string\">");
      xsd.writelnIncNestLevel("<xsd:enumeration value=\"FULL\"/>");
      xsd.writeln("<xsd:enumeration value=\"INITIAL\"/>");
      xsd.writeln("<xsd:enumeration value=\"UPDATE\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
   //}
   //if (addAllInterlisTypes || refConsistencyType) {
      xsd.writeln("<xsd:simpleType name=\"ConsistencyType\">");
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:string\">");
      xsd.writelnIncNestLevel("<xsd:enumeration value=\"COMPLETE\"/>");
      xsd.writeln("<xsd:enumeration value=\"INCOMPLETE\"/>");
      xsd.writeln("<xsd:enumeration value=\"INCONSISTENT\"/>");
      xsd.writeln("<xsd:enumeration value=\"ADAPTED\"/>");
      xsd.writelnDecNestLevel("</xsd:restriction>");
      xsd.writelnDecNestLevel("</xsd:simpleType>");
   //}
}

void XsdOutput::writeAttrOrParam(metamodel::AttrOrParam* a) {
   string minOccurs = "";

   DomainType* t = dynamic_cast<DomainType*>(a->Type);
   Class* ct = dynamic_cast<Class*>(a->Type);

   if (t != nullptr) {
      if (t->getClass() == "MultiValue") {
         MultiValue* mv = static_cast<MultiValue*>(t);
         if (mv->Multiplicity.Min < 1) {
            minOccurs = " minOccurs=\"0\"";
         }
      }
      else if (t->Mandatory != true) {
         minOccurs = " minOccurs=\"0\"";
      }
   }

   if (ct != nullptr) {
      if (ct->Mandatory != true) {
         minOccurs = " minOccurs=\"0\"";
      }
      else if (ct->Multiplicity.Min < 1 && ct->Multiplicity.Min > -1) {
         minOccurs = " minOccurs=\"0\"";
      }

      Class* ctSuper = dynamic_cast<Class*>(ct->Super);

      if (ctSuper->Kind == Class::Structure) {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + ">");

         xsd.incNestLevel();

         writeClassType(ct);

         xsd.decNestLevel();

         xsd.writeln("</xsd:element>");
         return;
      }
      else if (ctSuper->ElementInPackage->ElementInPackage == nullptr) {
         if (a->AttrParent->ElementInPackage->ElementInPackage != ctSuper->ElementInPackage) {
            xsd.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + ">");

            xsd.writeln("<xsd:complexType>");
            xsd.writelnIncNestLevel("<xsd:sequence>");
            xsd.writelnIncNestLevel("<xsd:element name=\"" + get_path(ctSuper) + "\" type=\"" + get_path(ctSuper) + "\"/>");
            xsd.writelnDecNestLevel("</xsd:sequence>");
            xsd.writelnDecNestLevel("</xsd:complexType>");

            xsd.writeln("</xsd:element>");
            return;
         }
      }
      else if (a->AttrParent->ElementInPackage->ElementInPackage != ctSuper->ElementInPackage->ElementInPackage) {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + ">");

         xsd.writeln("<xsd:complexType>");
         xsd.writelnIncNestLevel("<xsd:sequence>");
         xsd.writelnIncNestLevel("<xsd:element name=\"" + get_path(ctSuper) + "\" type=\"" + get_path(ctSuper) + "\"/>");

         
         xsd.writelnDecNestLevel("</xsd:sequence>");
         xsd.writelnDecNestLevel("</xsd:complexType>");

         xsd.writeln("</xsd:element>");
         return;
      }
      else if (static_cast<Class*>(ct->Super)->_attr == nullptr) {
         if (ctSuper->ElementInPackage->ElementInPackage == nullptr) {
            if (a->AttrParent->ElementInPackage->ElementInPackage != ctSuper->ElementInPackage) {
               xsd.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + ">");

               xsd.writeln("<xsd:complexType>");
               xsd.writelnIncNestLevel("<xsd:sequence>");
               xsd.writelnIncNestLevel("<xsd:element name=\"" + get_path(ctSuper) + "\" type=\"" + get_path(ctSuper) + "\"/>");
               xsd.writelnDecNestLevel("</xsd:sequence>");
               xsd.writelnDecNestLevel("</xsd:complexType>");

               xsd.writeln("</xsd:element>");
               return;
            }
         }

         xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"" + get_path(a->Type->Super) + "\"" + minOccurs + "/>");
         return;
      }
   }

   if (a->Type->Name == "TEXT") {
      TextType* tt = static_cast<TextType*>(a->Type);

      if (tt->Kind == TextType::NameVal) {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"INTERLIS.NAME\"" + minOccurs + "/>");
         return;
      }
      else if (tt->Kind == TextType::Uri) {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"INTERLIS.URI\"" + minOccurs + "/>");
         return;
      }
   }

   if (t != nullptr && t->Name != "TOP" && t->Name != "" && t->Name != "TYPE" && t->Name != "TEXT"
      && t->Name != "BLACKBOX") {
      if (t->Name == "BOOLEAN") {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"xsd:boolean\"" + minOccurs + "/>");
      }
      else if (t->Name == "HALIGNMENT") {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"INTERLIS.HALIGNMENT\"" + minOccurs + "/>");
      }
      else if (t->Name == "VALIGNMENT") {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"INTERLIS.VALIGNMENT\"" + minOccurs + "/>");
      }
      else if (t->Name == "XMLDate") {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"xsd:date\"" + minOccurs + "/>");
      }
      else if (t->Name == "XMLDateTime") {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"xsd:dateTime\"" + minOccurs + "/>");
      }
      else if (t->Name == "XMLTime") {
         xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"xsd:time\"" + minOccurs + " / > ");
      }
      else {
         if (t->Super == nullptr) {
            xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"" + get_path(a->Type) + "\"" + minOccurs + "/>");
         }
         else {
            xsd.writeln("<xsd:element name=\"" + a->Name + "\" type=\"" + get_path(a->Type->Super) + "\"" + minOccurs + "/>");
         }
      }

      return;
   }

   xsd.writeln("<xsd:element name=\"" + a->Name + "\"" + minOccurs + ">");

   xsd.incNestLevel();
   writeType(a->Type);

   xsd.decNestLevel();
   xsd.writeln("</xsd:element>");
}

void XsdOutput::writeType(metamodel::Type *t) {
   bool isOIDType = false;

   string clas = t->getClass();

   if (t->getClass() == "LineType") {
      writeLineType(static_cast<LineType*>(t));
   } 
   else if (t->getClass() == "CoordType") {
      writeCoordType(static_cast<CoordType *>(t));
   }
   else if (t->getClass() == "ReferenceType") {
      writeReferenceType(static_cast<ReferenceType*>(t));
   }
   else if (t->getClass() == "AnyOIDType") {
      isOIDType = true;
   }
   else if (t->getClass() == "EnumType") {
      writeEnumType(static_cast<EnumType *>(t));
   }
   else if (t->getClass() == "NumType") {
      NumType *numType = static_cast<NumType*>(t);

      if (numType->OIDType) {
         isOIDType = true;
      }
      else {
         writeNumType(numType);
      }
   }
   else if (t->getClass() == "TextType") {
      TextType* textType = static_cast<TextType*>(t);

      if (textType->OIDType) {
         isOIDType = true;
      }
      else {
         writeTextType(textType);
      }
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
      string typeAttribute = "";

      if (t->Name != "ANYCLASS" && t->Name != "ANYSTRUCTURE") {
         typeAttribute = " name=\"" + get_path(t) + "\"";
      }

      xsd.writeln("<xsd:complexType" + typeAttribute + ">");
      xsd.writelnIncNestLevel("<xsd:simpleContent>");
      xsd.writelnIncNestLevel("<xsd:extension base=\"xsd:string\"/>");
      xsd.writelnDecNestLevel("</xsd:simpleContent>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   }

   if (isOIDType) {
      string name = "";
      if (t->_attr == nullptr) {
         name = " name=\"" + get_path(t) + "\"";
      }

      xsd.writeln("<xsd:complexType" + name + ">");
      xsd.writelnIncNestLevel("<xsd:attribute name=\"OID\" type=\"IliID\" use=\"required\"/>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   }
}

void XsdOutput::writeTextType(metamodel::TextType *t) {
   DomainType* domainType = static_cast<DomainType*>(t);
   string typeAttribute = "";
   
   if (domainType->Name != "" && domainType->Name != "TOP" && domainType->Name != "TYPE" && domainType->Name != "TEXT") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   xsd.writeln("<xsd:simpleType" + typeAttribute + ">");

   if (t->Kind == t->Text) {
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:normalizedString\">");
   }
   else {
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:string\">");
   }

   if (t->MaxLength > 0) {
      xsd.writelnIncNestLevel("<xsd:maxLength value=\"" + to_string(t->MaxLength) + "\"/>");
      xsd.decNestLevel();
   }

   xsd.writeln("</xsd:restriction>");
   xsd.writelnDecNestLevel("</xsd:simpleType>");
}

void XsdOutput::writeCoordType(metamodel::CoordType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   xsd.writeln("<xsd:complexType" + typeAttribute + ">");
   xsd.writelnIncNestLevel("<xsd:sequence>");
   xsd.writelnIncNestLevel("<xsd:element name=\"COORD\" type=\"CoordValue\"/>");
   xsd.writelnDecNestLevel("</xsd:sequence>");
   xsd.writelnDecNestLevel("</xsd:complexType>");
}

void XsdOutput::writeEnumType(metamodel::EnumType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   xsd.writeln("<xsd:simpleType" + typeAttribute + ">");
   xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:string\">");

   xsd.incNestLevel();

   EnumType* consolidated = getConsolidatedEnumeration(t);

   list<string> enums;
   
   for (auto node : consolidated->TopNode->Node) {
      buildEnum(&enums, "", node);
   }

   for (auto entry : enums) {
      xsd.writeln("<xsd:enumeration value=\"" + entry + "\"/>");
   }

   //for (auto v : t->ETVT) {
   //   xsd.writeln("<xsd:enumeration value=\"" + v->Name + "\"/>");
   //}

   xsd.writelnDecNestLevel("</xsd:restriction>");
   xsd.writelnDecNestLevel("</xsd:simpleType>");
}

void XsdOutput::writeNumType(metamodel::NumType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   xsd.writeln("<xsd:simpleType" + typeAttribute + ">");

   if (t->Min.find(".") < t->Min.length()) {
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:double\">");

      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"" + t->Min + "\"/>");

      string tMax = t->Max;
      if (util::starts_with(tMax, "+")) {
         tMax.erase(0, 1);
      }

      xsd.writeln("<xsd:maxInclusive value=\"" + tMax + "\"/>");

      xsd.writelnDecNestLevel("</xsd:restriction>");
   }
   else {
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:integer\">");

      long long min = stoll(t->Min);
      if (min >= INT_MAX) {
         min = INT_MAX;
      }

      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"" + to_string(min) + "\"/>");

      string tMax = t->Max;
      if (util::starts_with(tMax, "+")) {
         tMax.erase(0, 1);
      }

      long long max = stoll(t->Max);
      if (max >= INT_MAX) {
         max = INT_MAX;
      }

      try {
         xsd.writeln("<xsd:maxInclusive value=\"" + to_string(max) + "\"/>");
      }
      catch (...) {

      }
      xsd.writelnDecNestLevel("</xsd:restriction>");
   }

   
   xsd.writelnDecNestLevel("</xsd:simpleType>");
}

void XsdOutput::writeLineType(metamodel::LineType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   unordered_map<string, string> lineforms = {};
   LineType* search = t;

   while (search != nullptr) {
      for (auto* lf : search->LineForm) {
         if (lf->Name == "STRAIGHTS") {

         }
         else if (lf->Name == "ARCS") {
            lineforms.insert({ "ARC","ArcPoint" });
         }
         else {
            lineforms.insert({ get_path(lf), get_path(lf->Structure) });
         }
      }

      search = static_cast<LineType*>(search->Super);
   }

   if (t->Kind == t->Polyline || t->Kind == t->DirectedPolyline) {
      xsd.writeln("<xsd:complexType" + typeAttribute + ">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"POLYLINE\">");

      xsd.writelnIncNestLevel("<xsd:complexType>");
      xsd.writelnIncNestLevel("<xsd:choice minOccurs=\"2\" maxOccurs=\"unbounded\">");
      xsd.writelnIncNestLevel("<xsd:element name=\"COORD\" type=\"CoordValue\"/>");
      
      for (auto const& [key,value] : lineforms) {
         string type = "";

         if (value != "") {
            type = " type=\"" + value + "\"";
         }
         
         xsd.writeln("<xsd:element name=\"" + key + "\"" + type + "/>");
      }

      xsd.writelnDecNestLevel("</xsd:choice>");
      xsd.writelnDecNestLevel("</xsd:complexType>");

      xsd.writelnDecNestLevel("</xsd:element>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   }
   else if (t->Kind == t->Area || t->Kind == t->Surface) {
      xsd.writeln("<xsd:complexType" + typeAttribute + ">");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"SURFACE\">");
      xsd.writelnIncNestLevel("<xsd:complexType>");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"BOUNDARY\" maxOccurs=\"unbounded\">");
      xsd.writelnIncNestLevel("<xsd:complexType>");
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:element name=\"POLYLINE\" maxOccurs=\"unbounded\">");

      xsd.writelnIncNestLevel("<xsd:complexType>");

      bool hasLineAttr = false;
      if (t->LAStructure != nullptr) {
         hasLineAttr = true;
         xsd.writelnIncNestLevel("<xsd:sequence>");
         xsd.writelnIncNestLevel("<xsd:element name=\"LINEATTR\">");
         xsd.writelnIncNestLevel("<xsd:complexType>");
         xsd.writelnIncNestLevel("<xsd:sequence>");
         xsd.writelnIncNestLevel("<xsd:element name=\"" + get_path(t->LAStructure) + "\" type=\"" + get_path(t->LAStructure) + "\"/>");
         xsd.writelnDecNestLevel("</xsd:sequence>");
         xsd.writelnDecNestLevel("</xsd:complexType>");
         xsd.writelnDecNestLevel("</xsd:element>");
      }

      if (!hasLineAttr) {
         xsd.incNestLevel();
      }

      xsd.writeln("<xsd:choice minOccurs=\"2\" maxOccurs=\"unbounded\">");
      xsd.writelnIncNestLevel("<xsd:element name=\"COORD\" type=\"CoordValue\"/>");

      for (auto const& [key, value] : lineforms) {
         string type = "";

         if (value != "") {
            type = " type=\"" + value + "\"";
         }

         xsd.writeln("<xsd:element name=\"" + key + "\"" + type + "/>");
      }

      xsd.writelnDecNestLevel("</xsd:choice>");
      if (hasLineAttr) {
         xsd.writelnDecNestLevel("</xsd:sequence>");
      }

      xsd.writelnDecNestLevel("</xsd:complexType>");

      xsd.writelnDecNestLevel("</xsd:element>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
      xsd.writelnDecNestLevel("</xsd:element>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
      xsd.writelnDecNestLevel("</xsd:element>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
      xsd.writelnDecNestLevel("</xsd:complexType>");
   }
}

void XsdOutput::writeFormattedType(metamodel::FormattedType *t) {
   string typeAttribute = "";

   if (t->_attr != nullptr) {
   }
   else if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   xsd.writeln("<xsd:simpleType" + typeAttribute + ">");

   string name = t->Name;
   if (t->BaseFormattedType != nullptr) {
       name = t->BaseFormattedType->Name;
   }
   else if (t->Struct != nullptr) {
       name = t->Struct->Name;
   }

   if (name == "XMLDate") {
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:date\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"" + getFormattedDateTime(t->Min) + "\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"" + getFormattedDateTime(t->Max) + "\"/>");
   }
   else if (name == "XMLDateTime") {
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:dateTime\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"" + getFormattedDateTime(t->Min) + "\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"" + getFormattedDateTime(t->Max) + "\"/>");
   }
   else if (name == "XMLTime") {
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:time\">");
      xsd.writelnIncNestLevel("<xsd:minInclusive value=\"" + getFormattedDateTime(t->Min) + "\"/>");
      xsd.writeln("<xsd:maxInclusive value=\"" + getFormattedDateTime(t->Max) + "\"/>");
   }
   else {
      xsd.writelnIncNestLevel("<xsd:restriction base=\"xsd:normalizedString\">");
      xsd.incNestLevel();
   }

   xsd.writelnDecNestLevel("</xsd:restriction>");
   xsd.writelnDecNestLevel("</xsd:simpleType>");
}

void XsdOutput::writeBlackboxType(metamodel::BlackboxType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "BLACKBOX") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   xsd.writeln("<xsd:complexType" + typeAttribute + ">");
   xsd.writelnIncNestLevel("<xsd:sequence>");

   if (t->Kind == t->Binary) {
      xsd.writelnIncNestLevel("<xsd:element name=\"BINBLBOX\">");
   }
   else {
      xsd.writelnIncNestLevel("<xsd:element name=\"XMLBLBOX\">");
   }

   xsd.writelnIncNestLevel("<xsd:complexType>");

   if (t->Kind == t->Binary) {
      xsd.writelnIncNestLevel("<xsd:simpleContent>");
      xsd.writelnIncNestLevel("<xsd:extension base=\"xsd:base64Binary\"/>");
      xsd.writelnDecNestLevel("</xsd:simpleContent>");
   }
   else {
      xsd.writelnIncNestLevel("<xsd:sequence>");
      xsd.writelnIncNestLevel("<xsd:any minOccurs=\"0\" maxOccurs=\"unbounded\" processContents=\"skip\"/>");
      xsd.writelnDecNestLevel("</xsd:sequence>");
   }

   xsd.writelnDecNestLevel("</xsd:complexType>");

   xsd.writelnDecNestLevel("</xsd:element>");
   xsd.writelnDecNestLevel("</xsd:sequence>");
   xsd.writelnDecNestLevel("</xsd:complexType>");
}

void XsdOutput::writeReferenceType(metamodel::ReferenceType *t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   xsd.writeln("<xsd:complexType" + typeAttribute + ">");
   xsd.writelnIncNestLevel("<xsd:attribute name=\"REF\" type=\"IliID\" use=\"required\"/>");
   xsd.writeln("<xsd:attribute name=\"BID\" type=\"IliID\"/>");
   xsd.writelnDecNestLevel("</xsd:complexType>");
}

void XsdOutput::writeMultiValue(metamodel::MultiValue* t) {
   string typeAttribute = "";

   if (t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   string minOccurs = "";
   string maxOccurs = "";

   if (t->Multiplicity.Min > 1) {
      minOccurs = " minOccurs=\"" + to_string(t->Multiplicity.Min) + "\"";
   }

   if (t->Multiplicity.Max != 1) {
      if (t->Multiplicity.Max == -1) {
         maxOccurs = " maxOccurs=\"unbounded\"";
      }
      else {
         maxOccurs = " maxOccurs=\"" + to_string(t->Multiplicity.Max) + "\"";
      }
   }

   xsd.writeln("<xsd:complexType" + typeAttribute + ">");
   xsd.writelnIncNestLevel("<xsd:sequence>");

   xsd.incNestLevel();

   if (t->TypeRestriction.size() > 0) {

      xsd.writelnIncNestLevel("<xsd:choice" + minOccurs + maxOccurs + ">");

      xsd.incNestLevel();

      if (t->BaseType != nullptr) {
         xsd.writeln("<xsd:element name=\"" + get_path(t->BaseType) + "\" type=\"" + get_path(t->BaseType) + "\"/>");
      }

      for (auto *entry : t->TypeRestriction) {
         xsd.writeln("<xsd:element name=\"" + get_path(entry) + "\" type=\"" + get_path(entry) + "\"/>");
      }

      xsd.decNestLevel();

      xsd.writelnDecNestLevel("</xsd:choice>");
   }
   else {
      if (t->BaseType->Sub.size() > 0) {
         xsd.writelnIncNestLevel("<xsd:choice" + minOccurs + maxOccurs + ">");

         xsd.incNestLevel();

         if (t->BaseType != nullptr) {
            xsd.writeln("<xsd:element name=\"" + get_path(t->BaseType) + "\" type=\"" + get_path(t->BaseType) + "\"/>");
         }

         for (auto* entry : t->BaseType->Sub) {
            xsd.writeln("<xsd:element name=\"" + get_path(entry) + "\" type=\"" + get_path(entry) + "\"/>");
         }

         xsd.decNestLevel();

         xsd.writelnDecNestLevel("</xsd:choice>");
      }
      else {
         xsd.writeln("<xsd:element name=\"" + get_path(t->BaseType) + "\" type=\"" + get_path(t->BaseType) + "\"" + minOccurs + maxOccurs + "/>");
      }

      
   }
   

   xsd.writelnDecNestLevel("</xsd:sequence>");
   xsd.writelnDecNestLevel("</xsd:complexType>");
}

void XsdOutput::writeClassType(metamodel::Class* t) {
   string typeAttribute = "";

   if (t->_attr == nullptr && t->Name != "" && t->Name != "TOP" && t->Name != "TYPE") {
      typeAttribute = " name=\"" + get_path(t) + "\"";
   }

   xsd.writeln("<xsd:complexType" + typeAttribute + ">");
   xsd.writelnIncNestLevel("<xsd:sequence>");

   if (t->Super->Sub.size() > 0) {
      xsd.writelnIncNestLevel("<xsd:choice>");

      xsd.incNestLevel();

      for (auto* entry : t->Super->Sub) {
         xsd.writeln("<xsd:element name=\"" + get_path(entry) + "\" type=\"" + get_path(entry) + "\"/>");
      }

      xsd.writeln("<xsd:element name=\"" + get_path(t->Super) + "\" type=\"" + get_path(t->Super) + "\"/>");

      xsd.decNestLevel();

      xsd.writeln("</xsd:choice>");
   }
   else {
      xsd.writelnIncNestLevel("<xsd:element name=\"" + get_path(t->Super) + "\" type=\"" + get_path(t->Super) + "\"/>");
   }

   
   xsd.writelnDecNestLevel("</xsd:sequence>");
   xsd.writelnDecNestLevel("</xsd:complexType>");
}

list<metamodel::ExtendableME*> XsdOutput::getClassAttributes(metamodel::Class* c) {
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

bool XsdOutput::isRealClass(metamodel::Class* c) {
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

metamodel::EnumType* XsdOutput::getConsolidatedEnumeration(metamodel::EnumType *t) {
   if (t->LTParent == nullptr) {
      return t;
   }

   if (t->LTParent->Extending == nullptr) {
      return t;
   }

   EnumType* extending = static_cast<EnumType*>(t->LTParent->Extending->Type);

   if (extending == nullptr) {
      return t;
   }

   EnumType *ret = getConsolidatedEnumeration(extending);

   for (auto *node : t->TopNode->Node) {
      mergeTree(ret->TopNode->Node, node);
   }

   return ret;
}

void XsdOutput::mergeTree(list<metamodel::EnumNode*> tree, metamodel::EnumNode *enumNode) {
   for (auto* node : tree) {
      if (node->Name == enumNode->Name) {

         if (enumNode->Node.size() > 0) {
            if (node->Node.size() > 0) {
               for (auto* subNode : enumNode->Node) {
                  mergeTree(node->Node,subNode);
               }
            }
            else {
               node->Node = enumNode->Node;
            }
         }
         
         return;
      }
   }

   tree.push_back(enumNode);
}

void XsdOutput::buildEnum(list<string> *list,string prefix, metamodel::EnumNode *enumNode) {
   if (prefix.length() > 0) {
      prefix += ".";
   }

   if (enumNode->Node.size() == 0) {
      list->push_back(prefix + enumNode->Name);
   }
   else {
      for (auto *node : enumNode->Node) {
         buildEnum(list,prefix+ enumNode->Name,node);
      }
   }
}

string XsdOutput::getFormattedDateTime(string input) {
   string output = "";
   std::regex rgx("([0-9]+)");
   std::smatch matches;

   bool hasDate = false;
   bool hasTime = false;

   if (input.find('-') != std::string::npos) {
      hasDate = true;
   }

   if (input.find(':') != std::string::npos) {
      hasTime = true;
   }

   vector<string> entries;
   string str = input;
   while (regex_search(str, matches, rgx)) {
      entries.push_back(matches[0]);
      str = matches.suffix();
   }

   for (size_t i = 0; i < entries.size(); ++i) {
      int match = std::stoi(entries[i].c_str());
      string append = "";

      if (i == 0) {
         if (hasDate) {
            append += to_string(match);
            append += "-";
         }
         else if (hasTime) {
            if (match < 10) {
               append += "0";
            }

            append += to_string(match);
            append += ":";
         }
      }
      else if (i == 1) {
         if (hasDate) {
            if (match < 10) {
               append += "0";
            }

            append += to_string(match);
            append += "-";
         }
         else if (hasTime) {
            if (match < 10) {
               append += "0";
            }

            append += to_string(match);
            append += ":";
         }
      }
      else if (i == 2) {
         if (hasDate) {
            if (match < 10) {
               append += "0";
            }

            append += to_string(match);

            if (hasTime) {
               append += "T";
            }
         }
         else if (hasTime) {
            if (match < 10) {
               append += "0";
            }

            append += to_string(match);
         }
      }
      else if (i == 3) {
         if (hasDate) {
            if (match < 10) {
               append += "0";
            }

            append += to_string(match);
            append += ":";
         }
         else if (hasTime) {
            append += ".";
            if (match < 10) {
               append += "00";
            }
            else if (match < 100) {
               append += "0";
            }

            append += to_string(match);
         }
      }
      else if (i == 4) {
         if (hasTime) {
            if (match < 10) {
               append += "0";
            }

            append += to_string(match);
            append += ":";
         }
      }
      else if (i == 5) {
         if (hasTime) {
            if (match < 10) {
               append += "0";
            }

            append += to_string(match);
            append += ".";
         }
      }
      else if (i == 6) {
         if (hasTime) {
            if (match < 10) {
               append += "00";
            }
            else if (match < 100) {
               append += "0";
            }

            append += to_string(match);
         }
      }

      output += append;
   }

   return output;
}
