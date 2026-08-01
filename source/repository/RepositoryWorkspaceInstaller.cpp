#include "ilic/RepositoryCompilerBridge.h"

#include "../core/CompilationSourceStore.h"
#include "../util/IliFile.h"
#include "../util/Logger.h"

#include <algorithm>

namespace ilic {

RepositoryWorkspaceInstaller::RepositoryWorkspaceInstaller(
   detail::CompilationSourceStore &sources,util::IliFileCatalog &files,util::Logger &logger)
   : sources_(sources),files_(files),logger_(logger)
{
}

RepositoryInstallResult RepositoryWorkspaceInstaller::install(const RepositoryResult &resolved)
{
   RepositoryInstallResult result;
   result.success = resolved.success;
   result.diagnostics = resolved.diagnostics;
   for (const auto &model : resolved.models) {
      if (model.uri.empty() || model.source.empty()) {
         result.success = false;
         result.diagnostics.push_back({DiagnosticSeverity::Error,"ILIC-REPO-INSTALL",
            "resolved repository model has no URI or source"});
         continue;
      }
      if (auto *existing = files_.findByUri(model.uri)) {
         if (existing->source().text != model.source) {
            result.success = false;
            result.diagnostics.push_back({DiagnosticSeverity::Error,"ILIC-REPO-INSTALL",
               "repository URI is already installed with different content: " + model.uri});
            continue;
         }
         files_.select(*existing);
         result.installedUris.push_back(model.uri);
         continue;
      }
      sources_.rememberRepositorySource(model.uri,model.source);
      auto *file = files_.loadByFile(model.uri);
      if (file == nullptr) {
         result.success = false;
         result.diagnostics.push_back({DiagnosticSeverity::Error,"ILIC-REPO-INSTALL",
            "unable to parse resolved repository source: " + model.uri});
         continue;
      }
      if (std::find(file->models().begin(),file->models().end(),model.metadata.name)
         == file->models().end()) {
         result.success = false;
         result.diagnostics.push_back({DiagnosticSeverity::Error,"ILIC-REPO-INSTALL",
            "resolved source does not declare model " + model.metadata.name + ": " + model.uri});
         continue;
      }
      files_.select(*file);
      result.installedUris.push_back(model.uri);
   }
   (void)logger_;
   return result;
}

} // namespace ilic
