#include "RepositoryVersion.h"

#include <algorithm>
#include <unordered_set>

namespace ilic::repository {
namespace {

Diagnostic warning(std::string message)
{
   Diagnostic diagnostic;
   diagnostic.severity = DiagnosticSeverity::Warning;
   diagnostic.code = "ILIC-REPO-VERSION";
   diagnostic.message = std::move(message);
   return diagnostic;
}

VersionSelectionResult selectForLanguage(const std::vector<ModelMetadata> &models,
   std::string_view modelName,std::string_view schemaLanguage)
{
   VersionSelectionResult result;
   std::vector<const ModelMetadata *> candidates;
   for (const auto &model : models) {
      if (model.name == modelName && model.schemaLanguage == schemaLanguage && !model.browseOnly)
         candidates.push_back(&model);
   }
   if (candidates.empty()) return result;

   std::vector<const ModelMetadata *> roots;
   for (const auto *candidate : candidates)
      if (candidate->precursorVersion.empty()) roots.push_back(candidate);
   if (roots.empty()) {
      result.diagnostics.push_back(warning("model " + std::string(modelName) + " ("
         + std::string(schemaLanguage) + ") has no root version without precursorVersion"));
      return result;
   }

   const ModelMetadata *current = roots.front();
   std::unordered_set<const ModelMetadata *> accounted;
   accounted.insert(current);
   for (std::size_t index = 1; index < roots.size(); ++index) {
      accounted.insert(roots[index]);
      result.diagnostics.push_back(warning("model " + std::string(modelName)
         + " has duplicate root version " + roots[index]->version
         + "; using the first root in repository order"));
   }

   while (true) {
      std::vector<const ModelMetadata *> successors;
      for (const auto *candidate : candidates) {
         if (accounted.count(candidate) == 0
            && candidate->precursorVersion == current->version) successors.push_back(candidate);
      }
      if (successors.empty()) break;
      current = successors.front();
      accounted.insert(current);
      for (std::size_t index = 1; index < successors.size(); ++index) {
         accounted.insert(successors[index]);
         result.diagnostics.push_back(warning("model " + std::string(modelName)
            + " has multiple successors of version " + successors[index]->precursorVersion
            + "; using " + current->version + " in repository order"));
      }
   }

   const bool broken = std::any_of(candidates.begin(),candidates.end(),
      [&](const ModelMetadata *candidate) { return accounted.count(candidate) == 0; });
   if (broken) result.diagnostics.push_back(warning("model " + std::string(modelName)
      + " has versions outside the precursorVersion chain; using version " + current->version));
   result.model = current;
   return result;
}

} // namespace

const std::vector<std::string> &supportedSchemaLanguagePreference()
{
   static const std::vector<std::string> order = {"ili2_4","ili2_3","ili1"};
   return order;
}

VersionSelectionResult selectLatestVersion(const std::vector<ModelMetadata> &models,
   std::string_view modelName,std::string_view schemaLanguage)
{
   if (!schemaLanguage.empty()) return selectForLanguage(models,modelName,schemaLanguage);
   VersionSelectionResult result;
   for (const auto &language : supportedSchemaLanguagePreference()) {
      auto selected = selectForLanguage(models,modelName,language);
      result.diagnostics.insert(result.diagnostics.end(),
         selected.diagnostics.begin(),selected.diagnostics.end());
      if (selected.model != nullptr) {
         result.model = selected.model;
         return result;
      }
   }
   return result;
}

} // namespace ilic::repository
