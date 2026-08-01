#include "RepositoryPolicy.h"

#include <limits>

namespace ilic::repository::core {
namespace {

std::chrono::milliseconds checkedSeconds(std::chrono::seconds seconds,
   std::chrono::milliseconds fallback,std::vector<Diagnostic> &diagnostics,
   const char *name)
{
   if (seconds.count() < 0) {
      diagnostics.push_back({DiagnosticSeverity::Error,"ILIC-REPO-POLICY",
         std::string(name) + " must not be negative"});
      return fallback;
   }
   const auto count = seconds.count();
   const auto limit = std::numeric_limits<long long>::max() / 1000;
   if (count > limit) {
      diagnostics.push_back({DiagnosticSeverity::Error,"ILIC-REPO-POLICY",
         std::string(name) + " is too large"});
      return fallback;
   }
   return std::chrono::milliseconds(count * 1000);
}

} // namespace

RepositoryPolicy normalizeRepositoryPolicy(const RepositoryOptions &options,
   std::vector<Diagnostic> &diagnostics)
{
   RepositoryPolicy policy;
   policy.metadataTtl = checkedSeconds(options.metadataTtl,policy.metadataTtl,diagnostics,
      "metadataTtl");
   policy.modelTtl = checkedSeconds(options.modelTtl,policy.modelTtl,diagnostics,"modelTtl");
   policy.offline = options.offline;
   policy.allowStaleOnError = options.allowStaleOnError;
   policy.followSiteLinks = options.followSiteLinks;
   policy.validateChecksums = options.validateChecksums;
   return policy;
}

} // namespace ilic::repository::core
