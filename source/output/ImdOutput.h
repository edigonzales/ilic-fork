#pragma once

#include <map>

#include "../metamodel/MetaModelTreeVisitor.h"
#include "../util/XTFWriter.h"
#include "../util/Logger.h"
#include "../util/Converter.h"

namespace output {
   
   class ImdOutput : public metamodel::MetaModelTreeVisitor {
   public:

      ImdOutput(metamodel::MetaModelStore &store,util::Logger &logger,
         string xtf_file,string version);

      void preVisit(void) override;
      void postVisit(void) override;

      void preVisitModel(metamodel::Model *m) override;
      void visitModel(metamodel::Model *m) override;
      void postVisitModel(metamodel::Model *m) override;

      void visitImport(metamodel::Import *i) override;

      void visitUnit(metamodel::Unit *u) override;

      void visitNumType(metamodel::NumType *t) override;
      void visitTextType(metamodel::TextType *t) override;
      void visitFormattedType(metamodel::FormattedType *t) override;
      void visitEnumType(metamodel::EnumType *t) override;
      void visitCoordType(metamodel::CoordType *t) override;
      void visitLineForm(metamodel::LineForm *l) override;
      void visitFunctionDef(metamodel::FunctionDef *f) override;
      void visitSubModel(metamodel::SubModel *s) override;
      void visitClass(metamodel::Class *c) override;

      void visitARefRestriction(metamodel::ARefRestriction *t) override;

      void visitDocText(metamodel::DocText *t);
      void visitMetaAttribute(metamodel::MetaAttribute *a) override;
      void visitIli1Format(metamodel::Ili1Format *f) override;
      void visitMultiplicity(metamodel::Multiplicity *m) override;
      void visitAttrOrParam(metamodel::AttrOrParam *a) override;
      void visitMultiValue(metamodel::MultiValue* m) override;
      void visitReferenceType(metamodel::ReferenceType* t) override;
      void visitRole(metamodel::Role* r) override;
      void visitExplicitAssocAccess(metamodel::ExplicitAssocAccess *a) override;
      void visitAssocAcc(metamodel::AssocAcc *a) override;
      void visitTransferElement(metamodel::TransferElement *t) override;
      void visitIli1TransferElement(metamodel::Ili1TransferElement *t) override;
      void visitDataUnit(metamodel::DataUnit* d) override;
      void visitAllowedInBasket(metamodel::AllowedInBasket *b) override;
      void visitMetaBasketDef(metamodel::MetaBasketDef *m) override;
      void visitMetaObjectDef(metamodel::MetaObjectDef *o) override;
      void visitBooleanType(metamodel::BooleanType *t) override;
      void visitBlackboxType(metamodel::BlackboxType *t) override;
      void visitAxisSpec(metamodel::AxisSpec *a) override;
      void visitNumsRefSys(metamodel::NumsRefSys *s) override;
      void visitAnyOIDType(metamodel::AnyOIDType *t) override;
      void visitArgument(metamodel::Argument *a) override;
      void visitObjectType(metamodel::ObjectType *t) override;
      void visitAttributeRefType(metamodel::AttributeRefType *r) override;
      void visitEnumNode(metamodel::EnumNode *n) override;
      void visitEnumTreeValueType(metamodel::EnumTreeValueType *t) override;
      void visitLineType(metamodel::LineType *t) override;
      void visitLinesForm(metamodel::LinesForm *f) override;
      void visitView(metamodel::View *v) override;
      void visitRenamedBaseView(metamodel::RenamedBaseView *v) override;
      void visitUnaryExpr(metamodel::UnaryExpr *e) override;
      void visitCompoundExpr(metamodel::CompoundExpr *e) override;
      void visitPathEl(metamodel::PathEl *e) override;
      void visitPathOrInspFactor(metamodel::PathOrInspFactor *f) override;
      void visitEnumAssignment(metamodel::EnumAssignment *a) override;
      void visitEnumMapping(metamodel::EnumMapping *m) override;
      void visitClassRef(metamodel::ClassRef *r) override;
      void visitActualArgument(metamodel::ActualArgument *a) override;
      void visitFunctionCall(metamodel::FunctionCall *f) override;
      void visitRuntimeParamRef(metamodel::RuntimeParamRef *r) override;
      void visitConstant(metamodel::Constant *c) override;
      void visitClassConst(metamodel::ClassConst *c) override;
      void visitAttributeConst(metamodel::AttributeConst *c) override;
      void visitUnitRef(metamodel::UnitRef *u) override;
      void visitUnitFunction(metamodel::UnitFunction *f) override;
      void visitSimpleConstraint(metamodel::SimpleConstraint *c) override;
      void visitExistenceDef(metamodel::ExistenceDef *e) override;
      void visitExistenceConstraint(metamodel::ExistenceConstraint *c) override;
      void visitUniqueConstraint(metamodel::UniqueConstraint *u) override;
      void visitSetConstraint(metamodel::SetConstraint *c) override;
      void visitGraphic(metamodel::Graphic *g) override;
      void visitSignParamAssignment(metamodel::SignParamAssignment *a) override;
      void visitCondSignParamAssignment(metamodel::CondSignParamAssignment *a) override;
      void visitDrawingRule(metamodel::DrawingRule *d) override;
      void visitDocTextTranslation(metamodel::DocTextTranslation *t) override;
      void visitMETranslation(metamodel::METranslation *t) override;
      void visitTranslation(metamodel::Translation* t) override;

      void visitContext(metamodel::Context* c) override;
      void visitGenericDef(metamodel::GenericDef* t) override;
      void visitClassRefType(metamodel::ClassRefType* t) override;
      void visitDependency(metamodel::Dependency* t) override;

      string getShortName() override {return "imd";};

// create visit...Override() methods
#undef MMV_VISITOR
#define MMV_VISITOR ImdOutput
#include "../metamodel/MetaModelTreeVisitor.inc"

   private:

      string xtf_file;
      string version;
      util::XTFWriter xtf;

   }; // class

}; // namespace
