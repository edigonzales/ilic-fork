#pragma once

#include "../util/TextWriter.h"
#include "../metamodel/MetaModelTreeVisitor.h"

namespace output {
   
   class XsdOutput : public metamodel::MetaModelTreeVisitor {
   public:

      XsdOutput(metamodel::MetaModelStore &store,util::Logger &logger,
         string ilifile,string iliversion,string ilic_version);

      void preVisit(void) override;
      void postVisit(void) override;
   
      void preVisitModel(metamodel::Model* m) override;
      void visitModel(metamodel::Model *m) override;
      void postVisitModel(metamodel::Model *m) override;

      void postVisitSubModel(metamodel::SubModel *s) override;
      
      void preVisitClass(metamodel::Class *c) override;
      void visitClass(metamodel::Class *c) override;
      void postVisitClass(metamodel::Class *c) override;

      void visitDomainType(metamodel::DomainType *d) override;    

      string getShortName() override { return "xsd"; };

      void createEmptyFile();

// create visit...Override() methods
#undef MMV_VISITOR
#define MMV_VISITOR XsdOutput
#include "../metamodel/MetaModelTreeVisitor.inc"

   private:
      string iliversion;
      string ilic_version;
      string xsd_file;
      util::TextWriter xsd;

      list<metamodel::Class*> all_classes;
      list<metamodel::ExtendableME*> classAttributes;

      void writeAnnotations();
      void writeFixTypes1();
      void writeAliasTables();
      void writeFixTypes2();
      void writeDatasection();
      void writeInterlisReferenceTypes();
      void writeHeadersection();

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

      list<metamodel::ExtendableME*> getClassAttributes(metamodel::Class* c);
      bool isRealClass(metamodel::Class* c);
      metamodel::EnumType* getConsolidatedEnumeration(metamodel::EnumType* t);
      void mergeTree(list<metamodel::EnumNode*> tree, metamodel::EnumNode* enumNode);
      void buildEnum(list<string>* list, string prefix, metamodel::EnumNode* enumNode);
      string getFormattedDateTime(string input);
   };

};
