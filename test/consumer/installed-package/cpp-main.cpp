#include "ilic/Compiler.h"
#include "metamodel/MetaModel.h"

#include <string>

int main()
{
    static_assert(sizeof(metamodel::Model) > 0,
                  "installed SDK must expose the concrete metamodel API");
    return std::string(ilic::version()).empty() ? 1 : 0;
}
