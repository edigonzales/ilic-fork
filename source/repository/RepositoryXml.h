#pragma once

#include "ilic/Diagnostic.h"
#include "ilic/Repository.h"

#include <string>
#include <string_view>
#include <vector>

namespace ilic::repository {

struct RepositorySite {
   std::vector<std::string> parentSites;
   std::vector<std::string> subsidiarySites;
};

struct RepositoryIndex {
   std::string repository;
   std::vector<ModelMetadata> models;
};

class RepositoryXml {
public:
   static RepositoryIndex parseModelIndex(std::string_view xml,
      std::string_view repository,std::vector<Diagnostic> *diagnostics = nullptr);

   static RepositorySite parseSite(std::string_view xml,
      std::string_view repository,std::vector<Diagnostic> *diagnostics = nullptr);
};

} // namespace ilic::repository
