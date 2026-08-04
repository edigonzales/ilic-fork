#include "ilic/capi.h"

#include <stdio.h>
#include <string.h>

#ifndef ILIC_EXPECTED_VERSION
#error ILIC_EXPECTED_VERSION must be defined by CMake
#endif

int main(void)
{
   if (ilic_abi_version() != 1U) {
      fprintf(stderr,"expected ilic ABI 1, got %u\n",
         (unsigned)ilic_abi_version());
      return 1;
   }
   if (strcmp(ilic_version(),ILIC_EXPECTED_VERSION) != 0) {
      fprintf(stderr,"expected ilic version %s, got %s\n",
         ILIC_EXPECTED_VERSION,ilic_version());
      return 1;
   }
   return 0;
}
