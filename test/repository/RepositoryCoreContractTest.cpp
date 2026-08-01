#include "RepositoryResourceCoordinator.h"
#include "RepositoryPolicy.h"

#include "ilic/RepositoryContract.h"
#include "ilic/Repository.h"
#include "ilic/test/TestHarness.h"

#include <chrono>

int main()
{
   using namespace std::chrono;
   const auto initial = system_clock::time_point(milliseconds(1000));
   ilic::repository::ports::ManualRepositoryClock clock(initial);
   ilic::repository::ports::RepositoryCacheEntry entry{"content",initial};
   ILIC_REQUIRE(ilic::repository::core::classifyCacheEntry(&entry,initial + milliseconds(999),
      milliseconds(1000)) == ilic::repository::core::CacheFreshness::Fresh);
   ILIC_REQUIRE(ilic::repository::core::classifyCacheEntry(&entry,initial + milliseconds(1000),
      milliseconds(1000)) == ilic::repository::core::CacheFreshness::Stale);
   clock.advance(milliseconds(1000));
   ILIC_REQUIRE(clock.now() == initial + milliseconds(1000));

   ilic::RepositoryOptions options;
   std::vector<ilic::Diagnostic> diagnostics;
   const auto policy = ilic::repository::core::normalizeRepositoryPolicy(options,diagnostics);
   ILIC_REQUIRE(diagnostics.empty());
   ILIC_REQUIRE(policy.validateChecksums);
   return 0;
}
