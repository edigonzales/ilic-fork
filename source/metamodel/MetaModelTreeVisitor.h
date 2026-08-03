#pragma once

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "../util/Logger.h"

#include "MetaModel.h"
#include "MetaModelStore.h"

#define MMV "mmv"

namespace metamodel {

   class MetaModelTreeVisitor {
   public:

      MetaModelTreeVisitor(MetaModelStore &store,util::Logger &logger)
         : store_(store),logger_(logger) {}
      virtual ~MetaModelTreeVisitor() = default;
      MetaModelTreeVisitor(const MetaModelTreeVisitor &) = delete;
      MetaModelTreeVisitor &operator=(const MetaModelTreeVisitor &) = delete;

      MetaModelStore &store() noexcept { return store_; }
      const MetaModelStore &store() const noexcept { return store_; }
      util::Logger &logger() noexcept { return logger_; }
      const util::Logger &logger() const noexcept { return logger_; }
   
      virtual void preVisit() {};
      virtual void postVisit() {};
   
      virtual void visitMMObject(MMObject *o) {};

      virtual void preVisitModel(Model *m) {};
      virtual void visitModel(Model *m) {};
      virtual void postVisitModel(Model *m) {};

      virtual void visitImport(Import *i) {};

      virtual void visitUnit(Unit *u) {};

      virtual void visitNumType(NumType *t) {};
      virtual void visitTextType(TextType *t) {};
      virtual void visitFormattedType(FormattedType *t) {};
      virtual void visitEnumType(EnumType *t) {};
      virtual void visitCoordType(CoordType *t) {};
      virtual void visitLineForm(LineForm *l) {};
 
      virtual void visitFunctionDef(FunctionDef *f) {};

      virtual void preVisitSubModel(SubModel *s) {};
      virtual void visitSubModel(SubModel *s) {};
      virtual void postVisitSubModel(SubModel *s) {};
      
      virtual void preVisitClass(Class *c) {};
      virtual void visitClass(Class *c) {};
      virtual void postVisitClass(Class *c) {};

      virtual void visitDocText(DocText *t) {};
      virtual void visitMetaAttribute(MetaAttribute *a) {};
      virtual void visitMetaElement(MetaElement *e) {};
      virtual void visitExtendableME(ExtendableME *e) {};
      virtual void visitPackage(Package *p) {};
      virtual void visitIli1Format(Ili1Format *f) {};
      virtual void visitExpression(Expression *e) {};
      virtual void visitMultiplicity(Multiplicity *m) {};
      virtual void visitConstraint(Constraint *c) {};
      virtual void visitDomainType(DomainType *d) {};
      virtual void visitType(Type *t) {};
      virtual void visitAttrOrParam(AttrOrParam *a) {};
      virtual void visitTypeRelatedType(TypeRelatedType *r) {};
      virtual void visitMultiValue(MultiValue* m) {};
      virtual void visitClassRelatedType(ClassRelatedType *t) {};
      virtual void visitReferenceType(ReferenceType *r) {};
      virtual void visitRole(Role *r) {};
      virtual void visitExplicitAssocAccess(ExplicitAssocAccess *a) {};
      virtual void visitAssocAcc(AssocAcc *a) {};
      virtual void visitTransferElement(TransferElement *t) {};
      virtual void visitIli1TransferElement(Ili1TransferElement *t) {};
      virtual void visitDataUnit(DataUnit *d) {}
      virtual void visitAllowedInBasket(AllowedInBasket *b) {};
      virtual void visitMetaBasketDef(MetaBasketDef *m) {};
      virtual void visitMetaObjectDef(MetaObjectDef *o) {};
      virtual void visitBooleanType(BooleanType *t) {};
      virtual void visitBlackboxType(BlackboxType *t) {};
      virtual void visitAxisSpec(AxisSpec *a) {};
      virtual void visitNumsRefSys(NumsRefSys *s) {};
      virtual void visitAnyOIDType(AnyOIDType *t) {};
      virtual void visitArgument(Argument *a) {};
      virtual void visitClassRefType(ClassRefType *r) {};
      virtual void visitObjectType(ObjectType *t) {};
      virtual void visitAttributeRefType(AttributeRefType *r) {};
      virtual void visitARefRestriction(ARefRestriction *r) {};
      virtual void visitEnumNode(EnumNode *n) {};
      virtual void visitEnumTreeValueType(EnumTreeValueType *t) {};
      virtual void visitLineType(LineType *t) {};
      virtual void visitLinesForm(LinesForm *f) {};
      virtual void visitView(View *v) {};
      virtual void visitRenamedBaseView(RenamedBaseView *v) {};
      virtual void visitUnaryExpr(UnaryExpr *e) {};
      virtual void visitCompoundExpr(CompoundExpr *e) {};
      virtual void visitFactor(Factor *f) {};
      virtual void visitPathEl(PathEl *e) {};
      virtual void visitPathOrInspFactor(PathOrInspFactor *f) {};
      virtual void visitEnumAssignment(EnumAssignment *a) {};
      virtual void visitEnumMapping(EnumMapping *m) {};
      virtual void visitClassRef(ClassRef *r) {};
      virtual void visitActualArgument(ActualArgument *a) {};
      virtual void visitFunctionCall(FunctionCall *f) {};
      virtual void visitRuntimeParamRef(RuntimeParamRef *r) {};
      virtual void visitConstant(Constant *c) {};
      virtual void visitClassConst(ClassConst *c) {};
      virtual void visitAttributeConst(AttributeConst *c) {};
      virtual void visitUnitRef(UnitRef *u) {};
      virtual void visitUnitFunction(UnitFunction *f) {};
      virtual void visitSimpleConstraint(SimpleConstraint *c) {};
      virtual void visitExistenceDef(ExistenceDef *e) {};
      virtual void visitExistenceConstraint(ExistenceConstraint *c) {};
      virtual void visitUniqueConstraint(UniqueConstraint *u) {};
      virtual void visitSetConstraint(SetConstraint *c) {};
      virtual void visitGraphic(Graphic *g) {};
      virtual void visitSignParamAssignment(SignParamAssignment *a) {};
      virtual void visitCondSignParamAssignment(CondSignParamAssignment *a) {};
      virtual void visitDrawingRule(DrawingRule *d) {};
      virtual void visitDocTextTranslation(DocTextTranslation *t) {};
      virtual void visitMETranslation(METranslation *t) {};
      virtual void visitTranslation(Translation *t) {};

      virtual void visitDependency(Dependency *d) {};
      
      // 2.4
      virtual void visitContext(Context *c) {};
      virtual void visitGenericDef(GenericDef *d) {};

      // helper methods
      void visit(MMObject& o);
      void visit(MMObject* o);
      void visit(MMObject *o,string classname);
      void visitAllModels();
      virtual string getShortName() {return MMV;};
      virtual void ignoreVisit();

#define MMV_VISITOR MetaModelTreeVisitor
#include "MetaModelTreeVisitor.inc"

   private:
      MetaModelStore &store_;
      util::Logger &logger_;

   };

};

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
