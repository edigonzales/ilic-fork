#include "ExternalMetaAttributeApplier.h"

#include "../metamodel/MetaModel.h"
#include "../metamodel/MetaModelStore.h"
#include "../util/Logger.h"

namespace ilic::detail {
namespace {

metamodel::MetaElement *findTarget(metamodel::Package *package,const std::string &path,
   const std::string &prefix)
{
   if (package == nullptr) return nullptr;
   const std::string packagePath = prefix.empty() ? package->Name : prefix + "." + package->Name;
   if (packagePath == path) return package;
   for (auto *element : package->Element) {
      if (element == nullptr) continue;
      const std::string elementPath = packagePath + "." + element->Name;
      if (elementPath == path) return element;
      if (auto *child = dynamic_cast<metamodel::Package *>(element))
         if (auto *found = findTarget(child,path,packagePath)) return found;
      if (auto *viewable = dynamic_cast<metamodel::Class *>(element)) {
         for (auto *attribute : viewable->ClassAttribute)
            if (attribute != nullptr && elementPath + "." + attribute->Name == path) return attribute;
         for (auto *parameter : viewable->ClassParameter)
            if (parameter != nullptr && elementPath + "." + parameter->Name == path) return parameter;
         for (auto *role : viewable->Role)
            if (role != nullptr && elementPath + "." + role->Name == path) return role;
         for (auto *constraint : viewable->Constraint)
            if (constraint != nullptr && elementPath + "." + constraint->Name == path) return constraint;
      }
      if (auto *function = dynamic_cast<metamodel::FunctionDef *>(element))
         for (auto *argument : function->Argument)
            if (argument != nullptr && elementPath + "." + argument->Name == path) return argument;
   }
   return nullptr;
}

} // namespace

void ExternalMetaAttributeApplier::apply(const CompilationRequest &request,
   const metamodel::MetaModelStore &store,util::Logger &logger)
{
   for (const auto &external : request.externalMetaAttributes) {
      metamodel::MetaElement *target = nullptr;
      for (auto *model : store.models()) {
         target = findTarget(model,external.element,"");
         if (target != nullptr) break;
      }
      if (target == nullptr) {
         logger.error(util::DiagnosticId::MetaTarget,
            "external meta attribute target " + external.element + " not found",-1);
         continue;
      }
      if (external.name == "ili2c.translationOf") {
         if (auto *model = dynamic_cast<metamodel::Model *>(target))
            model->_translationOfName = external.value;
         else
            logger.error(util::DiagnosticId::MetaTarget,
               "ili2c.translationOf may only target a model",-1);
      }
   }
}

} // namespace ilic::detail
