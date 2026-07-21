#pragma once

#include "ilic/Repository.h"

#include "../util/StringMap.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ilic::cli {

struct CliRepositoryConfig {
   std::vector<std::string> explicitRepositories;
   std::vector<std::string> effectiveRepositories;
   std::vector<std::string> requestedModels;
   std::string schemaLanguage;
   bool useDefaultRepositories = true;
};

std::vector<std::string> splitList(const std::string &value,std::string_view delimiters);
std::optional<std::string> schemaLanguageFromIliVersion(const std::string &value);
CliRepositoryConfig buildRepositoryConfig(util::StringMap &arguments);
std::optional<std::string> validateRepositoryCliConfig(util::StringMap &arguments,
   const CliRepositoryConfig &config,bool hasExplicitIliFiles);
std::unique_ptr<RepositoryManager> createRepositoryManager(const CliRepositoryConfig &config);
void logRepositoryDiagnostics(const std::vector<Diagnostic> &diagnostics);
bool loadResolvedRepositoryModels(const RepositoryResult &resolved);

} // namespace ilic::cli
