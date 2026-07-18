#pragma once

namespace metamodel {

   // Runs after every input model has been loaded and before translation
   // linking and output generation.
   void check_model_semantics();

}
