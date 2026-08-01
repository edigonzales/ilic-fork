#pragma once

#include "Repository.h"

#include <string>
#include <vector>

namespace ilic::detail {
class CompilationSourceStore;
}

namespace util {
class IliFileCatalog;
class Logger;
}

namespace ilic {

struct RepositoryInstallResult {
   bool success = false;
   std::vector<std::string> installedUris;
   std::vector<Diagnostic> diagnostics;
};

class RepositoryWorkspaceInstaller final {
public:
   RepositoryWorkspaceInstaller(detail::CompilationSourceStore &sources,
      util::IliFileCatalog &files,util::Logger &logger);

   RepositoryInstallResult install(const RepositoryResult &resolved);

private:
   detail::CompilationSourceStore &sources_;
   util::IliFileCatalog &files_;
   util::Logger &logger_;
};

} // namespace ilic
