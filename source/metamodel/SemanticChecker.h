#pragma once

#include "../util/Logger.h"

namespace metamodel {

   class MetaModelStore;

   class Role;
   class Type;
   struct Multiplicity;

   // Shared effective multiplicities used by semantic validation and projections.
   Multiplicity effectiveRoleCardinality(Role *role);
   Multiplicity attributeCardinality(Type *type);

   // Runs after every input model has been loaded and before translation
   // linking and output generation.
   void check_model_semantics(const MetaModelStore &store,util::Logger &logger);

}
