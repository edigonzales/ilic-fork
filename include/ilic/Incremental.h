#pragma once

#include "SourceManager.h"

#include <cstdint>
#include <string>
#include <vector>

namespace ilic {

struct IncrementalStats {
   std::uint64_t sourceAdds = 0;
   std::uint64_t sourceRemoves = 0;
   std::uint64_t sourceNoOps = 0;
   std::uint64_t versionOnlyUpdates = 0;
   std::uint64_t contentChanges = 0;
   std::uint64_t sourceReintroductions = 0;
   std::uint64_t rejectedUpdates = 0;

   std::uint64_t parserBuilds = 0;
   std::uint64_t parserHits = 0;
   std::uint64_t parserEvictions = 0;
   std::uint64_t parserBytes = 0;
   std::uint64_t syntaxMaterializations = 0;
   std::uint64_t editorMaterializations = 0;

   std::uint64_t rootAnalysisHits = 0;
   std::uint64_t rootAnalysisMisses = 0;
   std::uint64_t rootAnalysisBuilds = 0;
   std::uint64_t rootAnalysisEvictions = 0;
   std::uint64_t invalidatedRootEntries = 0;
   std::uint64_t reusedClosureSources = 0;
   std::uint64_t reparsedClosureSources = 0;
   std::uint64_t compilationInvocations = 0;
   std::uint64_t cancelledPlans = 0;

   void reset() noexcept { *this = {}; }
};

struct IncrementalTrace {
   std::string operation;
   std::string planKind;
   std::vector<std::string> roots;
   std::vector<std::string> closure;
   std::vector<std::string> parserHits;
   std::vector<std::string> parserMisses;
   std::vector<std::string> invalidatedRoots;
   std::vector<std::string> reasons;
};

struct ParsedSourceCacheOptions {
   std::size_t maxEntries = 512;
   std::size_t maxRetainedBytes = 256 * 1024 * 1024;
};

struct RootAnalysisCacheOptions {
   std::size_t maxEntries = 64;
   std::size_t maxRetainedBytes = 512 * 1024 * 1024;
};

struct IncrementalCacheOptions {
   ParsedSourceCacheOptions parsed;
   RootAnalysisCacheOptions root;
};

} // namespace ilic
