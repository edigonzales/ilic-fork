#pragma once

#include "../util/TextWriter.h"
#include "../metamodel/MetaModelTreeVisitor.h"

namespace output {
   
   class Ili2Output : public metamodel::MetaModelTreeVisitor {
   public:

      Ili2Output(string ilifile,string model_version);

      void preVisit() override;
      void postVisit() override;
   
      void preVisitModel(metamodel::Model* m) override;
      void visitModel(metamodel::Model* m) override;
      void postVisitModel(metamodel::Model *m) override;

      void visitImport(metamodel::Import *i) override;

      void preVisitSubModel(metamodel::SubModel *s) override;
      void visitSubModel(metamodel::SubModel* s) override;
      void postVisitSubModel(metamodel::SubModel *s) override;

      void preVisitClass(metamodel::Class *c) override;
      void postVisitClass(metamodel::Class *c) override;

      void visitSimpleConstraint(metamodel::SimpleConstraint* c) override;
      void visitUniqueConstraint(metamodel::UniqueConstraint* c) override;

      void visitDomainType(metamodel::DomainType *t) override;
      void visitContext(metamodel::Context *context) override;

      void visitAttrOrParam(metamodel::AttrOrParam *a) override;
      void visitRole(metamodel::Role* r) override;

      void visitUnit(metamodel::Unit* u) override;

      void visitFunctionDef(metamodel::FunctionDef* f) override;

      string getShortName() override { return "ili2"; };

// create visit...Override() methods
#undef MMV_VISITOR
#define MMV_VISITOR Ili2Output
#include "../metamodel/MetaModelTreeVisitor.inc"

   };

};
