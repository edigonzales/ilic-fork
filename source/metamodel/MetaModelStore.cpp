#include "MetaModelStore.h"

namespace metamodel {

MetaModelStore::~MetaModelStore() noexcept
{
   clear();
}

bool MetaModelStore::owns(const MMObject *object) const noexcept
{
   return object != nullptr && owned_.find(object) != owned_.end();
}

bool MetaModelStore::discardUnlinked(MMObject *object) noexcept
{
   if (!owns(object) || published_.find(object) != published_.end()) return false;
   const auto found = std::find_if(objects_.begin(),objects_.end(),
      [object](const auto &candidate) { return candidate.get() == object; });
   if (found == objects_.end()) return false;
   owned_.erase(object);
   objects_.erase(found);
   return true;
}

void MetaModelStore::clear() noexcept
{
   models_.clear();
   packages_.clear();
   types_.clear();
   units_.clear();
   imports_.clear();
   dependencies_.clear();
   axisSpecs_.clear();
   dataUnits_.clear();
   functions_.clear();
   lineForms_.clear();
   graphics_.clear();
   interlis_ = nullptr;
   anyClass_ = nullptr;
   anyStructure_ = nullptr;
   published_.clear();
   owned_.clear();
   objects_.clear();
}

void MetaModelStore::addModel(Model &model)
{
   appendUnique(models_,model);
   published_.insert(&model);
}

void MetaModelStore::addPackage(Package &package)
{
   appendUnique(packages_,package);
   published_.insert(&package);
}

void MetaModelStore::addType(Type &type)
{
   appendUnique(types_,type);
   published_.insert(&type);
}

void MetaModelStore::addUnit(Unit &unit)
{
   appendUnique(units_,unit);
   published_.insert(&unit);
}

void MetaModelStore::addImport(Import &import)
{
   appendUnique(imports_,import);
   published_.insert(&import);
}

void MetaModelStore::addDependency(Dependency &dependency)
{
   appendUnique(dependencies_,dependency);
   published_.insert(&dependency);
}

void MetaModelStore::addAxisSpec(AxisSpec &axisSpec)
{
   appendUnique(axisSpecs_,axisSpec);
   published_.insert(&axisSpec);
}

void MetaModelStore::addDataUnit(DataUnit &dataUnit)
{
   appendUnique(dataUnits_,dataUnit);
   published_.insert(&dataUnit);
}

void MetaModelStore::addFunction(FunctionDef &function)
{
   appendUnique(functions_,function);
   published_.insert(&function);
}

void MetaModelStore::addLineForm(LineForm &lineForm)
{
   appendUnique(lineForms_,lineForm);
   published_.insert(&lineForm);
}

void MetaModelStore::addGraphic(Graphic &graphic)
{
   appendUnique(graphics_,graphic);
   published_.insert(&graphic);
}

Class &MetaModelStore::anyClass()
{
   if (anyClass_ == nullptr) {
      anyClass_ = make<Class>();
      anyClass_->Name = "ANYCLASS";
      anyClass_->Kind = Class::ClassVal;
      published_.insert(anyClass_);
   }
   return *anyClass_;
}

Class &MetaModelStore::anyStructure()
{
   if (anyStructure_ == nullptr) {
      anyStructure_ = make<Class>();
      anyStructure_->Name = "ANYSTRUCTURE";
      anyStructure_->Kind = Class::Structure;
      published_.insert(anyStructure_);
   }
   return *anyStructure_;
}

} // namespace metamodel
