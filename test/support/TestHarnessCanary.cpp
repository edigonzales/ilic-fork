#include "ilic/test/TestHarness.h"

#include <cstring>

int main(int argc,char **argv)
{
   ILIC_REQUIRE_MSG(argc == 2,"expected exactly one canary mode");
   if (std::strcmp(argv[1],"--pass") == 0) {
      ILIC_REQUIRE(true);
      return 0;
   }
   if (std::strcmp(argv[1],"--fail") == 0) {
      ILIC_REQUIRE_MSG(false,"intentional canary failure");
   }
   ILIC_FAIL("unknown canary mode");
}
