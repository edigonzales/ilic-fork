#pragma once

#include "../util/TextWriter.h"
#include "../metamodel/MetaModelTreeVisitor.h"

#include <vector>
#include <map>
#include <set>

namespace output {
   
   class GmlOutput : public metamodel::MetaModelTreeVisitor {
   public:

      GmlOutput(metamodel::MetaModelStore &store,util::Logger &logger,
         string ilifile, string iliversion, string ilic_version);

      void preVisit() override;
   
      void preVisitModel(metamodel::Model *m) override;
      void visitModel(metamodel::Model* m) override;
      void postVisitModel(metamodel::Model *m) override;

      void preVisitSubModel(metamodel::SubModel* s) override;
      void postVisitSubModel(metamodel::SubModel *s) override;
      
      void preVisitClass(metamodel::Class *c) override;
      void visitClass(metamodel::Class *c) override;
      void postVisitClass(metamodel::Class *c) override;

      void visitDomainType(metamodel::DomainType* d) override;

      string getShortName() override { return "gml"; };

// create visit...Override() methods
#undef MMV_VISITOR
#define MMV_VISITOR GmlOutput
#include "../metamodel/MetaModelTreeVisitor.inc"

   private:
      string iliversion;
      string ilic_version;
      string gml_output_dir;
      util::TextWriter gml;

      string currentModel;
      string currentSubModel;
      vector<string> codelistKeys;
      vector<list<string>> codelistValues;
      list<metamodel::AttrOrParam*> areaAttributes;

      list<metamodel::Class*> writtenClasses;
      map<metamodel::MetaElement*,string> ObjectToNameDefinition;

      void writeAttrOrParam(metamodel::AttrOrParam* a);

      void writeType(metamodel::Type* t);
      void writeTextType(metamodel::TextType* t);
      void writeCoordType(metamodel::CoordType* t);
      void writeEnumType(metamodel::EnumType* t);
      void writeNumType(metamodel::NumType* t);
      void writeLineType(metamodel::LineType* t);
      void writeFormattedType(metamodel::FormattedType* t);
      void writeBlackboxType(metamodel::BlackboxType* t);
      void writeReferenceType(metamodel::ReferenceType* t);
      void writeMultiValue(metamodel::MultiValue* t);
      void writeClassType(metamodel::Class* t);

      void write_codelists(metamodel::Model* m);
      void write_linetables();

      void buildEnum(list<string>* list, string prefix, metamodel::EnumNode* enumNode);

      string getModelName(metamodel::MMObject *o);
      string getScopedName(metamodel::MetaElement *o);

      list<metamodel::ExtendableME*> getClassAttributes(metamodel::Class* c);
      bool isRealClass(metamodel::Class* c);
   };

};
