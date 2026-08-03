#include "SourceUpdateCoordinator.h"

#include "CompilerSessionState.h"
#include "incremental/SourceImpactAnalyzer.h"

#include <algorithm>
#include <optional>

namespace ilic::detail {

SourceUpdateResult SourceUpdateCoordinator::update(CompilerSessionState &state,
   std::string uri,std::string utf8,std::uint64_t version)
{
   SourceUpdateResult result = state.sources.update(std::move(uri),std::move(utf8),version);
   state.trace = {};
   state.trace.operation = "updateSource";
   state.trace.reasons.push_back(result.current.uri.empty() ? "rejected" : result.current.uri);
   switch (result.kind) {
      case SourceUpdateKind::Added: ++state.stats.sourceAdds; break;
      case SourceUpdateKind::Reintroduced: ++state.stats.sourceReintroductions; break;
      case SourceUpdateKind::Unchanged: ++state.stats.sourceNoOps; break;
      case SourceUpdateKind::VersionOnly: ++state.stats.versionOnlyUpdates; break;
      case SourceUpdateKind::ContentChanged: ++state.stats.contentChanges; break;
      case SourceUpdateKind::Rejected: ++state.stats.rejectedUpdates; break;
      case SourceUpdateKind::Removed: break;
   }
   if (result.accepted) {
      std::size_t parserReleased = 0;
      if (result.kind == SourceUpdateKind::ContentChanged ||
         result.kind == SourceUpdateKind::Reintroduced)
         parserReleased = state.parsedCache.invalidateUri(result.current.uri);

      const auto *source = state.sources.get(result.current.uri);
      const auto *beforeEntry = state.modelIndex.byUri(result.current.uri);
      const std::optional<SourceModelRecord> before = beforeEntry == nullptr
         ? std::optional<SourceModelRecord>{}
         : std::optional<SourceModelRecord>(*beforeEntry);
      SourceImpactAnalyzer impactAnalyzer;
      if (source != nullptr && result.kind != SourceUpdateKind::Unchanged &&
         result.kind != SourceUpdateKind::VersionOnly) {
         const auto entry = state.ensureParsed(*source,ParseMode::StrictCompiler);
         if (entry != nullptr && entry->artifact != nullptr) {
            const auto modelUpdate = state.modelIndex.replace(result.current,
               entry->artifact->header(),entry->artifact->parserDiagnostics().empty());
            state.dependencyIndex.replaceSourceDependencies(result.current.uri,
               modelUpdate.current.requiredModels);
         }
      }
      const auto *after = state.modelIndex.byUri(result.current.uri);
      const auto impact = impactAnalyzer.analyze(result,
         before == std::nullopt ? nullptr : &*before,after,state.rootCache);
      RootInvalidationResult invalidated;
      if (result.kind == SourceUpdateKind::ContentChanged ||
         result.kind == SourceUpdateKind::Reintroduced)
         invalidated = state.rootCache.invalidateBySource(result.current.uri);
      else if (result.kind == SourceUpdateKind::Added) {
         const auto *record = after;
         if (impact.invalidateAllRoots || record == nullptr || !record->syntacticallyUsable ||
            record->declaredModels.empty()) {
            invalidated = state.rootCache.invalidateIf(
               [](const RootAnalysisCacheEntry &) { return true; });
         }
         else {
            invalidated = state.rootCache.invalidateIf([&](const RootAnalysisCacheEntry &entry) {
               for (const auto &model : record->declaredModels) {
                  if (std::find(entry.missingModels.begin(),entry.missingModels.end(),model) !=
                     entry.missingModels.end()) return true;
                  if (std::find(entry.resolvedModels.begin(),entry.resolvedModels.end(),model) !=
                     entry.resolvedModels.end()) return true;
               }
               return false;
            });
         }
      }
      state.stats.invalidatedRootEntries += invalidated.invalidatedKeyIds.size();
      state.trace.bytesReleased += parserReleased + invalidated.releasedBytes;
      state.trace.invalidatedRoots = invalidated.invalidatedRoots;
   }
   state.syncCacheStats();
   return result;
}

bool SourceUpdateCoordinator::remove(CompilerSessionState &state,const std::string &uri)
{
   state.trace = {};
   state.trace.operation = "removeSource";
   state.trace.reasons.push_back(uri);
   if (!state.sources.remove(uri)) return false;
   ++state.stats.sourceRemoves;
   const std::size_t parserReleased = state.parsedCache.invalidateUri(uri);
   state.modelIndex.remove(uri);
   state.dependencyIndex.removeSource(uri);
   const auto invalidated = state.rootCache.invalidateIf(
      [](const RootAnalysisCacheEntry &) { return true; });
   state.stats.invalidatedRootEntries += invalidated.invalidatedKeyIds.size();
   state.trace.bytesReleased = parserReleased + invalidated.releasedBytes;
   state.trace.invalidatedRoots = invalidated.invalidatedRoots;
   state.syncCacheStats();
   return true;
}

} // namespace ilic::detail
