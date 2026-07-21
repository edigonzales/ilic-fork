#pragma once

#include "ilic/Diagnostic.h"
#include "ilic/Repository.h"

#include <string>
#include <string_view>
#include <vector>

namespace ilic::repository {

struct VersionSelectionResult {
   const ModelMetadata *model = nullptr;
   std::vector<Diagnostic> diagnostics;
};

const std::vector<std::string> &supportedSchemaLanguagePreference();

VersionSelectionResult selectLatestVersion(const std::vector<ModelMetadata> &models,
   std::string_view modelName,std::string_view schemaLanguage);

} // namespace ilic::repository
