#pragma once

#include "../util/TextWriter.h"
#include "../metamodel/MetaModelTreeVisitor.h"

namespace output {
   
   class Ili1Output : public metamodel::MetaModelTreeVisitor {
   public:

      Ili1Output(metamodel::MetaModelStore &store,util::Logger &logger,string ilifile);

      void preVisitModel(metamodel::Model *m) override;
      void postVisitModel(metamodel::Model *m) override;

      void preVisitSubModel(metamodel::SubModel *s) override;
      void postVisitSubModel(metamodel::SubModel *s) override;

      void preVisitClass(metamodel::Class *c) override;
      void postVisitClass(metamodel::Class *c) override;

      void visitAttrOrParam(metamodel::AttrOrParam *a) override;
      void visitRole(metamodel::Role *r) override;

      void visitDomainType(metamodel::DomainType* t) override;
      void visitEnumNode(metamodel::EnumNode* n) override {};
      void visitEnumTreeValueType(metamodel::EnumTreeValueType* t) override {};

      string getShortName() override { return "ili1"; };

// create visit...Override() methods
#undef MMV_VISITOR
#define MMV_VISITOR Ili1Output
#include "../metamodel/MetaModelTreeVisitor.inc"

   private:
      util::TextWriter ili1;
      string ili_file;
      bool domainheader_written = false;
      bool visitClassFlag = false;

      string get_enum_nodes(list <metamodel::EnumNode*> ln);
      string get_type(metamodel::DomainType* t, bool domainflag);



   };

};
