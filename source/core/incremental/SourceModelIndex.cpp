#include "SourceModelIndex.h"

#include <algorithm>
#include <sstream>

namespace ilic::detail {
namespace {

template<class T>
std::vector<std::string> sortedUnique(const T &values)
{
   std::vector<std::string> result(values.begin(),values.end());
   std::sort(result.begin(),result.end());
   result.erase(std::unique(result.begin(),result.end()),result.end());
   return result;
}

std::vector<std::string> difference(const std::vector<std::string> &left,
   const std::vector<std::string> &right)
{
   std::vector<std::string> result;
   for (const auto &value : left)
      if (std::find(right.begin(),right.end(),value) == right.end()) result.push_back(value);
   return result;
}

void eraseValue(std::vector<std::string> &values,std::string_view value)
{
   values.erase(std::remove(values.begin(),values.end(),value),values.end());
}

} // namespace

void SourceModelIndex::removeReverse(const SourceModelRecord &record)
{
   for (const auto &model : record.declaredModels) {
      auto found = definers_.find(model);
      if (found == definers_.end()) continue;
      eraseValue(found->second,record.uri);
      if (found->second.empty()) definers_.erase(found);
   }
   for (const auto &model : record.requiredModels) {
      auto found = requirers_.find(model);
      if (found == requirers_.end()) continue;
      eraseValue(found->second,record.uri);
      if (found->second.empty()) requirers_.erase(found);
   }
}

void SourceModelIndex::addReverse(const SourceModelRecord &record)
{
   for (const auto &model : sortedUnique(record.declaredModels)) {
      auto &uris = definers_[model];
      if (std::find(uris.begin(),uris.end(),record.uri) == uris.end()) uris.push_back(record.uri);
      std::sort(uris.begin(),uris.end());
   }
   for (const auto &model : sortedUnique(record.requiredModels)) {
      auto &uris = requirers_[model];
      if (std::find(uris.begin(),uris.end(),record.uri) == uris.end()) uris.push_back(record.uri);
      std::sort(uris.begin(),uris.end());
   }
}

SourceModelIndexUpdate SourceModelIndex::replace(const SourceIdentity &identity,
   const ParsedSourceHeader &header,bool syntacticallyUsable)
{
   SourceModelIndexUpdate result;
   auto found = records_.find(identity.uri);
   if (found != records_.end()) {
      result.previous = found->second;
      removeReverse(found->second);
   }
   SourceModelRecord record;
   record.uri = identity.uri;
   record.iliVersion = header.iliVersion;
   record.declaredModels = sortedUnique(header.models);
   record.importedModels = sortedUnique(header.imports);
   record.requiredModels = sortedUnique(header.requiredModels);
   record.identity = identity;
   record.syntacticallyUsable = syntacticallyUsable;
   records_[record.uri] = record;
   addReverse(record);
   result.current = record;
   result.addedModels = difference(record.declaredModels,result.previous.declaredModels);
   result.removedModels = difference(result.previous.declaredModels,record.declaredModels);
   result.addedRequirements = difference(record.requiredModels,result.previous.requiredModels);
   result.removedRequirements = difference(result.previous.requiredModels,record.requiredModels);
   result.iliVersionChanged = result.previous.iliVersion != record.iliVersion;
   result.usabilityChanged = result.previous.syntacticallyUsable != record.syntacticallyUsable;
   return result;
}

SourceModelIndexUpdate SourceModelIndex::remove(std::string_view uri)
{
   SourceModelIndexUpdate result;
   auto found = records_.find(std::string(uri));
   if (found == records_.end()) return result;
   result.previous = found->second;
   removeReverse(found->second);
   records_.erase(found);
   result.removedModels = result.previous.declaredModels;
   result.removedRequirements = result.previous.requiredModels;
   return result;
}

const SourceModelRecord *SourceModelIndex::byUri(std::string_view uri) const noexcept
{
   const auto found = records_.find(std::string(uri));
   return found == records_.end() ? nullptr : &found->second;
}

std::vector<std::string> SourceModelIndex::definingUris(std::string_view model,
   std::string_view iliVersion) const
{
   std::vector<std::string> result;
   const auto found = definers_.find(std::string(model));
   if (found == definers_.end()) return result;
   for (const auto &uri : found->second) {
      const auto *record = byUri(uri);
      if (record != nullptr && (iliVersion.empty() || record->iliVersion == iliVersion))
         result.push_back(uri);
   }
   return result;
}

std::vector<std::string> SourceModelIndex::requiringUris(std::string_view model) const
{
   const auto found = requirers_.find(std::string(model));
   return found == requirers_.end() ? std::vector<std::string>{} : found->second;
}

std::vector<std::string> SourceModelIndex::allUris() const
{
   std::vector<std::string> result;
   for (const auto &record : records_) result.push_back(record.first);
   return result;
}

bool SourceModelIndex::checkInvariants(std::string *error) const
{
   for (const auto &entry : records_) {
      const auto &record = entry.second;
      for (const auto &model : record.declaredModels) {
         const auto found = definers_.find(model);
         if (found == definers_.end() || std::find(found->second.begin(),found->second.end(),record.uri)
            == found->second.end()) {
            if (error != nullptr) *error = "missing model definer reverse index";
            return false;
         }
      }
      for (const auto &model : record.requiredModels) {
         const auto found = requirers_.find(model);
         if (found == requirers_.end() || std::find(found->second.begin(),found->second.end(),record.uri)
            == found->second.end()) {
            if (error != nullptr) *error = "missing model requirer reverse index";
            return false;
         }
      }
   }
   return true;
}

} // namespace ilic::detail
