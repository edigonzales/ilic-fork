#pragma once

#include "../util/Logger.h"

namespace metamodel {

   class MetaModelStore;

   // Links translated model elements to their base-language counterparts and
   // checks the structural equivalence required by INTERLIS 2.3/2.4.
   void check_model_translations(const MetaModelStore &store,util::Logger &logger);

}
