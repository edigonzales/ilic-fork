#pragma once

#include "RepositoryXml.h"

#include <string>
#include <string_view>
#include <vector>

namespace ilic::repository {

class RepositoryBackend {
public:
   virtual ~RepositoryBackend() = default;
   virtual const RepositoryIndex *loadIndex(const std::string &repository,
      std::vector<Diagnostic> *diagnostics) = 0;
   virtual const RepositorySite *loadSite(const std::string &repository,
      std::vector<Diagnostic> *diagnostics) = 0;
};

struct ModelLookupResult {
   bool found = false;
   ModelMetadata metadata;
   std::vector<Diagnostic> diagnostics;
};

class RepositoryCrawler {
public:
   explicit RepositoryCrawler(RepositoryBackend &backend) : backend_(backend) {}

   ModelLookupResult findModel(const std::vector<std::string> &seedRepositories,
      std::string_view modelName,std::string_view schemaLanguage);

   std::vector<ModelMetadata> listModels(const std::vector<std::string> &seedRepositories,
      std::vector<Diagnostic> *diagnostics = nullptr);

private:
   RepositoryBackend &backend_;
};

} // namespace ilic::repository
