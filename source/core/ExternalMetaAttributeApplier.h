#pragma once

#include "../../include/ilic/Compiler.h"

namespace metamodel {
class MetaModelStore;
}

namespace util {
class Logger;
}

namespace ilic::detail {

class ExternalMetaAttributeApplier final {
public:
   static void apply(const CompilationRequest &request,const metamodel::MetaModelStore &store,
      util::Logger &logger);
};

} // namespace ilic::detail
