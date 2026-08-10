#include "ilic/Compiler.h"

#ifdef ILIC_TEST_METAMODEL_HEADER
#include "metamodel/MetaModel.h"
#endif

#include <string>

int main()
{
#ifdef ILIC_TEST_METAMODEL_HEADER
    static_assert(sizeof(metamodel::Model) > 0,
                  "installed SDK must expose the concrete metamodel API");
#endif
    return std::string(ilic::version()).empty() ? 1 : 0;
}
