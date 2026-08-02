#include "SourceDependencyIndex.h"

#include <algorithm>
#include <set>

namespace ilic::detail {
namespace {

void removeValue(std::vector<std::string> &values,std::string_view value)
{
   values.erase(std::remove(values.begin(),values.end(),value),values.end());
}

} // namespace

void SourceDependencyIndex::replaceSourceDependencies(std::string uri,
   std::vector<std::string> requiredModels)
{
   removeSource(uri);
   std::sort(requiredModels.begin(),requiredModels.end());
   requiredModels.erase(std::unique(requiredModels.begin(),requiredModels.end()),requiredModels.end());
   requiredByUri_[uri] = requiredModels;
   for (const auto &model : requiredModels) urisByRequiredModel_[model].push_back(uri);
   for (auto &entry : urisByRequiredModel_) {
      std::sort(entry.second.begin(),entry.second.end());
      entry.second.erase(std::unique(entry.second.begin(),entry.second.end()),entry.second.end());
   }
}

void SourceDependencyIndex::removeSource(std::string_view uri)
{
   const auto found = requiredByUri_.find(std::string(uri));
   if (found == requiredByUri_.end()) return;
   for (const auto &model : found->second) {
      auto reverse = urisByRequiredModel_.find(model);
      if (reverse == urisByRequiredModel_.end()) continue;
      removeValue(reverse->second,uri);
      if (reverse->second.empty()) urisByRequiredModel_.erase(reverse);
   }
   requiredByUri_.erase(found);
}

std::vector<std::string> SourceDependencyIndex::directRequiredModels(std::string_view uri) const
{
   const auto found = requiredByUri_.find(std::string(uri));
   return found == requiredByUri_.end() ? std::vector<std::string>{} : found->second;
}

std::vector<std::string> SourceDependencyIndex::dependentSourceUrisForModel(
   std::string_view model) const
{
   const auto found = urisByRequiredModel_.find(std::string(model));
   return found == urisByRequiredModel_.end() ? std::vector<std::string>{} : found->second;
}

std::vector<std::string> SourceDependencyIndex::transitiveClosureForRoots(
   const std::vector<std::string> &roots,const SourceModelIndex &models) const
{
   std::set<std::string> visited;
   std::vector<std::string> pending(roots.begin(),roots.end());
   while (!pending.empty()) {
      const std::string uri = pending.back();
      pending.pop_back();
      if (!visited.insert(uri).second) continue;
      for (const auto &model : directRequiredModels(uri)) {
         const auto defining = models.definingUris(model);
         if (!defining.empty()) pending.push_back(defining.front());
      }
   }
   return {visited.begin(),visited.end()};
}

bool SourceDependencyIndex::checkInvariants(std::string *error) const
{
   for (const auto &entry : requiredByUri_)
      for (const auto &model : entry.second) {
         const auto reverse = urisByRequiredModel_.find(model);
         if (reverse == urisByRequiredModel_.end()
            || std::find(reverse->second.begin(),reverse->second.end(),entry.first)
               == reverse->second.end()) {
            if (error != nullptr) *error = "missing dependency reverse index";
            return false;
         }
      }
   return true;
}

} // namespace ilic::detail
