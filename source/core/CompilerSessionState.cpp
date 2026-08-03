#include "CompilerSessionState.h"

#include "CompilationKeys.h"
#include "SnapshotPipeline.h"

namespace ilic::detail {

CompilerSessionState::CompilerSessionState(const IncrementalCacheOptions &options)
   : parsedCache(options.parsed),rootCache(options.root) {}

std::shared_ptr<const ParsedSourceCacheEntry> CompilerSessionState::ensureParsed(
   const SourceBuffer &source,ParseMode mode)
{
   const auto identity = sourceIdentity(sources,source);
   const auto before = parsedCache.metrics();
   auto result = parsedCache.getOrBuild(source,identity,mode,"generated-grammar-v1",
      [this](const SourceBuffer &value,ParseMode requested) {
         return SnapshotPipeline(sources).build(value,requested);
      });
   const auto after = parsedCache.metrics();
   const bool built = mode == ParseMode::StrictCompiler
      ? after.strictBuilds > before.strictBuilds
      : after.tolerantBuilds > before.tolerantBuilds;
   const bool hit = mode == ParseMode::StrictCompiler
      ? after.strictHits > before.strictHits
      : after.tolerantHits > before.tolerantHits;
   if (built) {
      if (mode == ParseMode::StrictCompiler) trace.strictParserBuilds.push_back(source.uri);
      else trace.tolerantParserBuilds.push_back(source.uri);
      trace.parserMisses.push_back(source.uri);
   }
   else if (hit) {
      if (mode == ParseMode::StrictCompiler) trace.strictParserHits.push_back(source.uri);
      else trace.tolerantParserHits.push_back(source.uri);
      trace.parserHits.push_back(source.uri);
   }
   return result;
}

ParsedSourceArtifactPtr CompilerSessionState::parsedArtifact(const SourceBuffer &source)
{
   const auto entry = ensureParsed(source,ParseMode::StrictCompiler);
   return entry == nullptr ? ParsedSourceArtifactPtr{} : entry->artifact;
}

void CompilerSessionState::syncCacheStats()
{
   const auto parsed = parsedCache.metrics();
   const auto root = rootCache.metrics();
   stats.strictParserBuilds = parsed.strictBuilds;
   stats.tolerantParserBuilds = parsed.tolerantBuilds;
   stats.strictParserHits = parsed.strictHits;
   stats.tolerantParserHits = parsed.tolerantHits;
   stats.parserBuilds = parsed.strictBuilds + parsed.tolerantBuilds;
   stats.parserHits = parsed.strictHits + parsed.tolerantHits;
   stats.parserEvictions = parsed.evictions;
   stats.parserEntries = parsed.entries;
   stats.parserRetainedBytes = parsed.retainedBytes;
   stats.parserBytes = parsed.retainedBytes;
   stats.rootEntries = root.entries;
   stats.rootRetainedBytes = root.retainedBytes;
   stats.rootAnalysisEvictions = root.evictions;
   trace.bytesRetained = parsed.retainedBytes + root.retainedBytes;
}

} // namespace ilic::detail
