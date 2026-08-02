#pragma once

#include "../ParsedSourceArtifact.h"

#include "../../../include/ilic/SourceManager.h"

#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace ilic::detail {

struct SourceModelRecord final {
   std::string uri;
   std::string iliVersion;
   std::vector<std::string> declaredModels;
   std::vector<std::string> importedModels;
   std::vector<std::string> requiredModels;
   SourceIdentity identity;
   bool syntacticallyUsable = false;
};

struct SourceModelIndexUpdate final {
   std::vector<std::string> addedModels;
   std::vector<std::string> removedModels;
   std::vector<std::string> addedRequirements;
   std::vector<std::string> removedRequirements;
   bool iliVersionChanged = false;
   bool usabilityChanged = false;
   SourceModelRecord previous;
   SourceModelRecord current;
};

class SourceModelIndex final {
public:
   SourceModelIndexUpdate replace(const SourceIdentity &identity,
      const ParsedSourceHeader &header,bool syntacticallyUsable = true);
   SourceModelIndexUpdate remove(std::string_view uri);
   const SourceModelRecord *byUri(std::string_view uri) const noexcept;
   std::vector<std::string> definingUris(std::string_view model,
      std::string_view iliVersion = {}) const;
   std::vector<std::string> requiringUris(std::string_view model) const;
   std::vector<std::string> allUris() const;
   bool checkInvariants(std::string *error = nullptr) const;

private:
   void removeReverse(const SourceModelRecord &record);
   void addReverse(const SourceModelRecord &record);

   std::map<std::string,SourceModelRecord> records_;
   std::map<std::string,std::vector<std::string>> definers_;
   std::map<std::string,std::vector<std::string>> requirers_;
};

} // namespace ilic::detail
