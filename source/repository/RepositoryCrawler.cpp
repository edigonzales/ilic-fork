#include "RepositoryCrawler.h"

#include "RepositoryUri.h"
#include "RepositoryVersion.h"

#include <deque>
#include <optional>
#include <unordered_set>

namespace ilic::repository {
namespace {

Diagnostic warning(std::string code,std::string message)
{
   Diagnostic diagnostic;
   diagnostic.severity = DiagnosticSeverity::Warning;
   diagnostic.code = std::move(code);
   diagnostic.message = std::move(message);
   return diagnostic;
}

std::optional<std::string> normalizeRepository(std::string_view value,
   std::vector<Diagnostic> *diagnostics)
{
   std::string error;
   auto uri = RepositoryUri::parse(value,&error);
   if (!uri) {
      if (diagnostics != nullptr) diagnostics->push_back(warning("ILIC-REPO-URI",
         "invalid repository URI " + std::string(value) + ": " + error));
      return std::nullopt;
   }
   return uri->normalized();
}

std::vector<std::string> normalizeSeeds(const std::vector<std::string> &seeds,
   std::vector<Diagnostic> *diagnostics)
{
   std::vector<std::string> result;
   std::unordered_set<std::string> seen;
   for (const auto &seed : seeds) {
      auto normalized = normalizeRepository(seed,diagnostics);
      if (normalized && seen.insert(*normalized).second) result.push_back(std::move(*normalized));
   }
   return result;
}

void enqueueLinks(const std::string &source,const std::vector<std::string> &links,
   std::deque<std::string> &target,std::vector<Diagnostic> *diagnostics)
{
   auto base = RepositoryUri::parse(source);
   if (!base) return;
   for (const auto &link : links) {
      try {
         target.push_back(base->resolve(link).normalized());
      }
      catch (const std::exception &error) {
         if (diagnostics != nullptr) diagnostics->push_back(warning("ILIC-REPO-URI",
            "invalid site link " + link + " in " + source + ": " + error.what()));
      }
   }
}

} // namespace

ModelLookupResult RepositoryCrawler::findModel(
   const std::vector<std::string> &seedRepositories,std::string_view modelName,
   std::string_view schemaLanguage)
{
   ModelLookupResult result;
   const auto seeds = normalizeSeeds(seedRepositories,&result.diagnostics);
   std::unordered_set<std::string> visited;
   auto visitIndex = [&](const std::string &repository) -> bool {
      if (!visited.insert(repository).second) return false;
      const auto *index = backend_.loadIndex(repository,&result.diagnostics);
      if (index == nullptr) return false;
      auto selected = selectLatestVersion(index->models,modelName,schemaLanguage);
      result.diagnostics.insert(result.diagnostics.end(),selected.diagnostics.begin(),
         selected.diagnostics.end());
      if (selected.model == nullptr) return false;
      result.found = true;
      result.metadata = *selected.model;
      return true;
   };

   // ili2c searches every configured seed before following any site link.
   for (const auto &seed : seeds) if (visitIndex(seed)) return result;
   if (!result.found && !seeds.empty()) {
      std::deque<std::string> parents;
      std::deque<std::string> subsidiaries;
      for (const auto &seed : seeds) {
         const auto *site = backend_.loadSite(seed,&result.diagnostics);
         if (site == nullptr) continue;
         enqueueLinks(seed,site->parentSites,parents,&result.diagnostics);
         enqueueLinks(seed,site->subsidiarySites,subsidiaries,&result.diagnostics);
      }

      auto visitParentQueue = [&]() -> bool {
         while (!parents.empty()) {
            const std::string repository = std::move(parents.front());
            parents.pop_front();
            if (visited.count(repository) != 0) continue;
            if (visitIndex(repository)) return true;
            const auto *site = backend_.loadSite(repository,&result.diagnostics);
            if (site != nullptr) enqueueLinks(repository,site->parentSites,parents,&result.diagnostics);
         }
         return false;
      };
      if (visitParentQueue()) return result;

      while (!subsidiaries.empty()) {
         const std::string repository = std::move(subsidiaries.front());
         subsidiaries.pop_front();
         if (visited.count(repository) != 0) continue;
         if (visitIndex(repository)) return result;
         const auto *site = backend_.loadSite(repository,&result.diagnostics);
         if (site != nullptr) {
            enqueueLinks(repository,site->subsidiarySites,subsidiaries,&result.diagnostics);
            enqueueLinks(repository,site->parentSites,parents,&result.diagnostics);
            if (visitParentQueue()) return result;
         }
      }
   }
   return result;
}

std::vector<ModelMetadata> RepositoryCrawler::listModels(
   const std::vector<std::string> &seedRepositories,std::vector<Diagnostic> *diagnostics)
{
   std::vector<Diagnostic> localDiagnostics;
   auto *outputDiagnostics = diagnostics == nullptr ? &localDiagnostics : diagnostics;
   const auto seeds = normalizeSeeds(seedRepositories,outputDiagnostics);
   std::vector<ModelMetadata> models;
   std::unordered_set<std::string> visited;
   auto visitIndex = [&](const std::string &repository) {
      if (!visited.insert(repository).second) return false;
      const auto *index = backend_.loadIndex(repository,outputDiagnostics);
      if (index != nullptr) models.insert(models.end(),index->models.begin(),index->models.end());
      return true;
   };
   for (const auto &seed : seeds) visitIndex(seed);

   std::deque<std::string> parents;
   std::deque<std::string> subsidiaries;
   for (const auto &seed : seeds) {
      const auto *site = backend_.loadSite(seed,outputDiagnostics);
      if (site == nullptr) continue;
      enqueueLinks(seed,site->parentSites,parents,outputDiagnostics);
      enqueueLinks(seed,site->subsidiarySites,subsidiaries,outputDiagnostics);
   }
   auto drainParents = [&] {
      while (!parents.empty()) {
         const std::string repository = std::move(parents.front());
         parents.pop_front();
         if (!visitIndex(repository)) continue;
         const auto *site = backend_.loadSite(repository,outputDiagnostics);
         if (site != nullptr) enqueueLinks(repository,site->parentSites,parents,outputDiagnostics);
      }
   };
   drainParents();
   while (!subsidiaries.empty()) {
      const std::string repository = std::move(subsidiaries.front());
      subsidiaries.pop_front();
      if (!visitIndex(repository)) continue;
      const auto *site = backend_.loadSite(repository,outputDiagnostics);
      if (site != nullptr) {
         enqueueLinks(repository,site->subsidiarySites,subsidiaries,outputDiagnostics);
         enqueueLinks(repository,site->parentSites,parents,outputDiagnostics);
         drainParents();
      }
   }
   return models;
}

} // namespace ilic::repository
