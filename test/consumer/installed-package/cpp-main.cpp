#include "ilic/Compiler.h"

#ifdef ILIC_TEST_METAMODEL_HEADER
#include "metamodel/MetaModel.h"
#include "metamodel/MetaModelStore.h"
#include "metamodel/SemanticChecker.h"
#endif

#include <string>

int main()
{
#ifdef ILIC_TEST_METAMODEL_HEADER
    static_assert(sizeof(metamodel::Model) > 0,
                  "installed SDK must expose the concrete metamodel API");
    static_assert(sizeof(metamodel::MetaModelStore) > 0,
                  "installed SDK must expose the concrete metamodel store API");
#endif
    return std::string(ilic::version()).empty() ? 1 : 0;
}
