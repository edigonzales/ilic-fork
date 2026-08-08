#include "ilic/Compiler.h"

#include <string>

int main()
{
    return std::string(ilic::version()).empty() ? 1 : 0;
}
