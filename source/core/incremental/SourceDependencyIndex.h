#pragma once

#include "SourceModelIndex.h"

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace ilic::detail {

class SourceDependencyIndex final {
public:
   void replaceSourceDependencies(std::string uri,std::vector<std::string> requiredModels);
   void removeSource(std::string_view uri);
   std::vector<std::string> directRequiredModels(std::string_view uri) const;
   std::vector<std::string> dependentSourceUrisForModel(std::string_view model) const;
   std::vector<std::string> transitiveClosureForRoots(const std::vector<std::string> &roots,
      const SourceModelIndex &models) const;
   bool checkInvariants(std::string *error = nullptr) const;

private:
   std::map<std::string,std::vector<std::string>> requiredByUri_;
   std::map<std::string,std::vector<std::string>> urisByRequiredModel_;
};

} // namespace ilic::detail
