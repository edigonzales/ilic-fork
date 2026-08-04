#include "ilic/Compiler.h"

#include <iostream>
#include <string>

#ifndef ILIC_EXPECTED_VERSION
#error ILIC_EXPECTED_VERSION must be defined by CMake
#endif

int main()
{
   const std::string actual = ilic::version();
   const std::string expected = ILIC_EXPECTED_VERSION;
   if (actual != expected) {
      std::cerr << "expected ilic version " << expected << ", got " << actual
                << '\n';
      return 1;
   }
   return 0;
}
