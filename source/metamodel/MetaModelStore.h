#pragma once

#include "MetaModel.h"

#include <algorithm>
#include <memory>
#include <type_traits>
#include <unordered_set>
#include <utility>

namespace metamodel {

// Owns every dynamically created MMObject belonging to one compiler run. Raw
// pointers in the generated model graph remain non-owning and are valid only
// while this store is alive.
class MetaModelStore final {
public:
   MetaModelStore() = default;
   ~MetaModelStore() noexcept;

   MetaModelStore(const MetaModelStore &) = delete;
   MetaModelStore &operator=(const MetaModelStore &) = delete;
   MetaModelStore(MetaModelStore &&) = delete;
   MetaModelStore &operator=(MetaModelStore &&) = delete;

   template <typename T, typename... Args>
   T *make(Args &&...args)
   {
      static_assert(std::is_base_of_v<MMObject,T>,
         "MetaModelStore::make requires an MMObject-derived type");
      auto object = std::make_unique<T>(std::forward<Args>(args)...);
      T *result = object.get();
      objects_.push_back(std::move(object));
      owned_.insert(result);
      return result;
   }

   bool owns(const MMObject *object) const noexcept;
   std::size_t objectCount() const noexcept { return objects_.size(); }
   bool discardUnlinked(MMObject *object) noexcept;
   void clear() noexcept;

   void addModel(Model &model);
   void addPackage(Package &package);
   void addType(Type &type);
   void addUnit(Unit &unit);
   void addImport(Import &import);
   void addDependency(Dependency &dependency);
   void addAxisSpec(AxisSpec &axisSpec);
   void addDataUnit(DataUnit &dataUnit);
   void addFunction(FunctionDef &function);
   void addLineForm(LineForm &lineForm);
   void addGraphic(Graphic &graphic);

   const std::vector<Model *> &models() const noexcept { return models_; }
   const std::vector<Package *> &packages() const noexcept { return packages_; }
   const std::vector<Type *> &types() const noexcept { return types_; }
   const std::vector<Unit *> &units() const noexcept { return units_; }
   const std::vector<Import *> &imports() const noexcept { return imports_; }
   const std::vector<Dependency *> &dependencies() const noexcept { return dependencies_; }
   const std::vector<AxisSpec *> &axisSpecs() const noexcept { return axisSpecs_; }
   const std::vector<DataUnit *> &dataUnits() const noexcept { return dataUnits_; }
   const std::vector<FunctionDef *> &functions() const noexcept { return functions_; }
   const std::vector<LineForm *> &lineForms() const noexcept { return lineForms_; }
   const std::vector<Graphic *> &graphics() const noexcept { return graphics_; }

   Model *interlisModel() const noexcept { return interlis_; }
   void setInterlisModel(Model &model) noexcept { interlis_ = &model; }

   Class &anyClass();
   Class &anyStructure();

private:
   template <typename T,typename U>
   static void appendUnique(std::vector<T *> &items,U &item)
   {
      T *pointer = static_cast<T *>(&item);
      if (std::find(items.begin(),items.end(),pointer) == items.end())
         items.push_back(pointer);
   }

   std::vector<std::unique_ptr<MMObject>> objects_;
   std::unordered_set<const MMObject *> owned_;
   std::unordered_set<const MMObject *> published_;

   std::vector<Model *> models_;
   std::vector<Package *> packages_;
   std::vector<Type *> types_;
   std::vector<Unit *> units_;
   std::vector<Import *> imports_;
   std::vector<Dependency *> dependencies_;
   std::vector<AxisSpec *> axisSpecs_;
   std::vector<DataUnit *> dataUnits_;
   std::vector<FunctionDef *> functions_;
   std::vector<LineForm *> lineForms_;
   std::vector<Graphic *> graphics_;

   Model *interlis_ = nullptr;
   Class *anyClass_ = nullptr;
   Class *anyStructure_ = nullptr;
};

} // namespace metamodel
