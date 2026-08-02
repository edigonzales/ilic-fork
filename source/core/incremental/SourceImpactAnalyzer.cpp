#include "SourceImpactAnalyzer.h"

#include <algorithm>

namespace ilic::detail {
namespace {

bool contains(const std::vector<std::string> &values,const std::string &value)
{
   return std::find(values.begin(),values.end(),value) != values.end();
}

void appendUnique(std::vector<std::string> &values,const std::vector<std::string> &more)
{
   for (const auto &value : more)
      if (!contains(values,value)) values.push_back(value);
   std::sort(values.begin(),values.end());
}

} // namespace

SourceImpact SourceImpactAnalyzer::analyze(const SourceUpdateResult &update,
   const SourceModelRecord *before,const SourceModelRecord *after,
   const RootAnalysisCache &roots) const
{
   SourceImpact result;
   result.observedRootEntries = roots.metrics().entries;
   result.invalidateParser = update.kind == SourceUpdateKind::ContentChanged
      || update.kind == SourceUpdateKind::Reintroduced;
   result.replaceSourceIndex = update.accepted && update.kind != SourceUpdateKind::Unchanged
      && update.kind != SourceUpdateKind::VersionOnly;

   if (before != nullptr) {
      appendUnique(result.affectedModels,before->declaredModels);
      appendUnique(result.affectedModels,before->requiredModels);
   }
   if (after != nullptr) {
      appendUnique(result.affectedModels,after->declaredModels);
      appendUnique(result.affectedModels,after->requiredModels);
   }

   if (before != nullptr && after != nullptr) {
      for (const auto &model : before->declaredModels)
         if (!contains(after->declaredModels,model)) result.missingModelsResolved.push_back(model);
   }
   std::sort(result.missingModelsResolved.begin(),result.missingModelsResolved.end());

   if (update.kind == SourceUpdateKind::Added) {
      if (after == nullptr || !after->syntacticallyUsable || after->declaredModels.empty()) {
         result.invalidateAllRoots = true;
      } else {
         for (const auto &model : after->declaredModels)
            if (before != nullptr && contains(before->declaredModels,model))
               result.mayCreateAmbiguity = true;
      }
   }
   if (update.kind == SourceUpdateKind::ContentChanged
      || update.kind == SourceUpdateKind::Reintroduced
      || update.kind == SourceUpdateKind::Removed)
      result.invalidateAllRoots = false;

   return result;
}

} // namespace ilic::detail
