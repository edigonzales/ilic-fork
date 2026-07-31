#pragma once

#include "MetaModelStore.h"
#include "../util/Logger.h"
#include "../../include/ilic/SourceManager.h"
#include "antlr4-runtime.h"

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace metamodel {

enum class IliLanguageVersion {
   Ili10,
   Ili23,
   Ili24
};

// Run-local model construction and name-resolution state. The builder is
// intentionally non-copyable and is always owned by a CompilerContext.
class MetaModelBuilder final {
public:
   class SourceScope;
   class ContextScope;

   MetaModelBuilder(MetaModelStore &store,util::Logger &logger);
   MetaModelBuilder(const MetaModelBuilder &) = delete;
   MetaModelBuilder &operator=(const MetaModelBuilder &) = delete;
   void reset();

   void setLanguageVersion(IliLanguageVersion version) noexcept { languageVersion_ = version; }
   IliLanguageVersion languageVersion() const noexcept { return languageVersion_; }
   std::string languageVersionName() const {
      return languageVersion_ == IliLanguageVersion::Ili24 ? "2.4" :
         (languageVersion_ == IliLanguageVersion::Ili10 ? "1.0" : "2.3");
   }
   bool isIli23() const noexcept { return languageVersion_ == IliLanguageVersion::Ili23; }
   bool isIli24() const noexcept { return languageVersion_ == IliLanguageVersion::Ili24; }

   SourceScope enterSource(const ilic::SourceBuffer &source);
   ContextScope enterContext(MetaElement &element);
   MMObject *current() const noexcept;
   Class *currentClass() const noexcept;
   Package *currentPackage() const noexcept;
   SubModel *currentTopic() const noexcept;
   Model *currentModel() const noexcept;
   std::size_t contextDepth() const noexcept { return context_.size(); }

   MetaModelStore &store() noexcept { return store_; }
   const MetaModelStore &store() const noexcept { return store_; }
   util::Logger &logger() noexcept { return logger_; }
   const std::string &currentSourceUri() const noexcept { return currentSourceUri_; }
   void pushContext(MetaElement &element);
   void popContext() noexcept;

   void prepareMetaAttributes(const std::string &source);
   ilic::SourceRange tokenRange(antlr4::Token *token) const;
   void setSelectionSource(MetaElement *element,antlr4::Token *token);
   void setEndSelectionSource(MetaElement *element,antlr4::Token *token);
   void setReferenceSource(MMObject *object,std::string kind,antlr4::Token *token);
   void setReferenceSource(MMObject *object,std::string kind,
      antlr4::ParserRuleContext *context);

   void initObject(MMObject *object,int line);
   void initMetaElement(MetaElement *element,int line);
   void initExtendable(ExtendableME *element,int line);
   void initPackage(Package *package,int line);
   void initType(Type *type,int line);
   void initDomainType(DomainType *type,int line);
   void initClass(Class *klass,int line);
   void initGraphic(Graphic *graphic,int line);
   void initFunction(FunctionDef *function,int line);
   void initLineForm(LineForm *lineForm,int line);
   void initExpression(Expression *expression,int line);
   void initFactor(Factor *factor,int line);
   void initConstraint(Constraint *constraint,int line);

   Model *findModel(const std::string &name,int line);
   void addModel(Model *model);
   void addImport(Import *import);
   void addDataUnit(DataUnit *dataUnit);
   void addDependency(Dependency *dependency);
   void addAxisSpec(AxisSpec *axisSpec);
   DataUnit *findDataUnit(const std::string &name,int line);
   void addPackage(Package *package);
   Package *findPackage(const std::string &name,int line);
   SubModel *findTopic(const std::string &name,int line);
   void addUnit(Unit *unit);
   Unit *findUnit(const std::string &name,int line);
   void addType(Type *type);
   Type *findType(const std::string &name,int line);
   std::string typeString(Type *type) const;
   DomainType *findDomainType(const std::string &name,int line);
   void addClass(Class *klass);
   Class *findClass(const std::string &name,int line);
   Class *findClassType(const std::string &name,int line);
   Class *findClass(Package *package,const std::string &name,int line);
   Class *findClassOrView(const std::string &name,int line);
   Class *findClassOrStructure(const std::string &name,int line);
   Class *findStructure(const std::string &name,int line);
   Class *findStructure(Package *package,const std::string &name,int line);
   View *findView(const std::string &name,int line);
   Class *findAssociation(const std::string &name,int line,
      const ilic::SourceRange &referenceRange = {});
   Class *findAssociation(Package *package,const std::string &name,int line);
   AttrOrParam *findAttribute(Class *klass,const std::string &name);
   Role *findRole(Class *klass,const std::string &name);
   AttrOrParam *findParameter(Class *klass,const std::string &name,int line);
   void addGraphic(Graphic *graphic);
   Graphic *findGraphic(const std::string &name,int line);
   void addFunction(FunctionDef *function);
   FunctionDef *findFunction(const std::string &name,int line);
   void addLineForm(LineForm *lineForm);
   LineForm *findLineForm(const std::string &name,int line);
   std::vector<std::string> unqualifiedImports(const std::string &modelName) const;
   bool dependsOn(Package *package) const;

   Type *anyToType(antlrcpp::Any value);
   DomainType *anyToDomainType(antlrcpp::Any value);
   void debug(antlr4::ParserRuleContext *context,std::string message);
   static int line(antlr4::ParserRuleContext *context);
   static int line(antlr4::Token *token);
   static int line(antlr4::tree::TerminalNode *node);
   static bool isReservedName(const std::string &name);

   MMObject *clone(const MMObject &object) { return object.clone(store_); }

private:
   struct PendingMetaAttribute {
      std::string name;
      std::string value;
      std::string rawText;
      int line = -1;
      int column = 0;
   };

   friend class SourceScope;
   friend class ContextScope;
   void popContext(MetaElement &element) noexcept;
   Type *findType(const std::string &name,int line,bool reportError);
   std::size_t utf8ByteOffset(std::size_t codepointOffset) const;

   MetaModelStore &store_;
   util::Logger &logger_;
   IliLanguageVersion languageVersion_ = IliLanguageVersion::Ili23;
   std::vector<MetaElement *> context_;
   std::string currentSourceUri_;
   std::string currentSourceText_;
   std::map<int,std::vector<PendingMetaAttribute>> pendingMetaAttributes_;
   std::map<int,std::string> pendingDocumentation_;
};

class MetaModelBuilder::SourceScope final {
public:
   SourceScope(MetaModelBuilder &builder,const ilic::SourceBuffer &source);
   ~SourceScope() noexcept;
   SourceScope(const SourceScope &) = delete;
   SourceScope &operator=(const SourceScope &) = delete;
   SourceScope(SourceScope &&other) noexcept;
   SourceScope &operator=(SourceScope &&) = delete;

private:
   MetaModelBuilder *builder_ = nullptr;
   std::string previousUri_;
   std::string previousText_;
   std::map<int,std::vector<PendingMetaAttribute>> previousMetaAttributes_;
   std::map<int,std::string> previousDocumentation_;
   util::Logger::SourceScope loggerSourceScope_;
};

class MetaModelBuilder::ContextScope final {
public:
   ContextScope(MetaModelBuilder &builder,MetaElement &element);
   ~ContextScope() noexcept;
   ContextScope(const ContextScope &) = delete;
   ContextScope &operator=(const ContextScope &) = delete;
   ContextScope(ContextScope &&other) noexcept;
   ContextScope &operator=(ContextScope &&) = delete;

private:
   MetaModelBuilder *builder_ = nullptr;
   MetaElement *expected_ = nullptr;
};

} // namespace metamodel
