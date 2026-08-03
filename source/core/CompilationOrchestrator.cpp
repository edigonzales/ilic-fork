#include "CompilationOrchestrator.h"

#include "CompilationKeys.h"
#include "CompilationRun.h"
#include "CompilationTranscript.h"
#include "CompilerContext.h"
#include "CompilerSessionState.h"
#include "../../include/ilic/Semantic.h"

namespace ilic::detail {

CompilationAnalysisResult CompilationOrchestrator::compileAndAnalyze(
   CompilerSessionState &state,const CompilationRequest &request,
   std::uint64_t &invocationCount)
{
   state.trace = {};
   state.trace.operation = "compileAndAnalyze";
   state.trace.roots = request.roots;
   ++state.stats.compileRequests;
   ++invocationCount;
   ++state.stats.compilationInvocations;
   const std::string baseKey = requestKey(request);
   if (auto cached = state.rootCache.findExact(baseKey,state.sources)) {
      ++state.stats.rootAnalysisHits;
      state.stats.reusedClosureSources += cached->closureUris.size();
      CompilationAnalysisResult result = cached->result;
      for (auto &version : result.semantic.documentVersions)
         if (const auto *source = state.sources.get(version.first)) version.second = source->version;
      for (auto &syntax : result.syntax)
         if (const auto *source = state.sources.get(syntax.uri)) syntax.documentVersion = source->version;
      for (auto iterator = result.compilation.transcript.rbegin();
         iterator != result.compilation.transcript.rend(); ++iterator) {
         if (iterator->rfind("inf: ilic completed with ",0) == 0) {
            const std::size_t timestamp = iterator->rfind(' ');
            if (timestamp != std::string::npos)
               *iterator = iterator->substr(0,timestamp + 1) + CompilationTranscript::timestamp();
            break;
         }
      }
      state.trace.planKind = "ExactCacheHit";
      state.trace.closure = cached->closureUris;
      state.trace.reasons.push_back("root cache hit");
      state.syncCacheStats();
      return result;
   }

   ++state.stats.rootAnalysisMisses;
   ++state.stats.compileExecutions;
   const auto parserBefore = state.parsedCache.metrics();
   CompilerContext context(state.sources,request.options,
      [&state](const SourceBuffer &source) { return state.parsedArtifact(source); });
   CompilationAnalysisResult result;
   std::vector<std::string> sourceUris;
   result.compilation = CompilationRun(context).run(request,sourceUris);
   for (const auto &uri : sourceUris) {
      const auto *source = state.sources.get(uri);
      if (source == nullptr) continue;
      const auto entry = state.ensureParsed(*source,ParseMode::StrictCompiler);
      if (entry == nullptr) continue;
      ++state.stats.syntaxMaterializations;
      SyntaxSnapshot syntax = entry->syntax;
      syntax.documentVersion = source->version;
      result.syntax.push_back(std::move(syntax));
   }
   result.semantic = buildSemanticSnapshot(state.sources,request,result.compilation,sourceUris,
      &result.syntax,&context.models());
   const std::string key = rootKey(baseKey,sourceUris,state.sources);
   RootAnalysisCacheEntry cached;
   cached.baseKey = baseKey;
   cached.key = key;
   cached.closureUris = sourceUris;
   for (const auto &uri : sourceUris) cached.identities.push_back(sourceIdentity(state.sources,uri));
   cached.hadMissingModels = !result.compilation.missingModels.empty();
   cached.missingModels = result.compilation.missingModels;
   for (const auto &model : result.compilation.models) cached.resolvedModels.push_back(model.name);
   cached.result = result;
   state.rootCache.insert(std::move(cached));
   ++state.stats.rootAnalysisBuilds;
   const auto parserAfter = state.parsedCache.metrics();
   const std::size_t reparsed = static_cast<std::size_t>(
      parserAfter.strictBuilds - parserBefore.strictBuilds);
   state.stats.reparsedClosureSources += reparsed;
   state.stats.reusedClosureSources += sourceUris.size() > reparsed
      ? sourceUris.size() - reparsed : 0;
   state.trace.planKind = "RebuildWithParseReuse";
   state.trace.closure = sourceUris;
   state.trace.reasons.push_back("root cache miss");
   state.syncCacheStats();
   return result;
}

CompilationResult CompilationOrchestrator::compile(CompilerSessionState &state,
   const CompilationRequest &request)
{
   ++state.stats.compileRequests;
   ++state.stats.compileExecutions;
   ++state.stats.compilationInvocations;
   CompilerContext context(state.sources,request.options,
      [&state](const SourceBuffer &source) { return state.parsedArtifact(source); });
   std::vector<std::string> sourceUris;
   auto result = CompilationRun(context).run(request,sourceUris);
   state.syncCacheStats();
   return result;
}

} // namespace ilic::detail
