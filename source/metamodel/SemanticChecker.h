#pragma once

namespace metamodel {

   class Role;
   class Type;
   struct Multiplicity;

   // Shared effective multiplicities used by semantic validation and projections.
   Multiplicity effectiveRoleCardinality(Role *role);
   Multiplicity attributeCardinality(Type *type);

   // Runs after every input model has been loaded and before translation
   // linking and output generation.
   void check_model_semantics();

}
