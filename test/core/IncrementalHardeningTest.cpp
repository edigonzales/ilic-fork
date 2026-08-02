#include "ilic/Compiler.h"
#include "ilic/Semantic.h"
#include "ilic/test/TestHarness.h"

#include "../../source/core/SnapshotPipeline.h"
#include "../../source/core/incremental/ParsedSourceCache.h"
#include "../../source/core/incremental/RootAnalysisCache.h"
#include "../../source/core/incremental/SourceModelIndex.h"

#include <algorithm>
#include <array>
#include <vector>

namespace {

const std::string validModel(const char *name,const char *body = "")
{
   return "INTERLIS 2.3;\nMODEL " + std::string(name)
      + " AT \"https://example.invalid/ilic/hardening\" VERSION \"1\" =\n"
      + body + "\nEND " + name + ".\n";
}

void requireEquivalent(const ilic::CompilationAnalysisResult &left,
   const ilic::CompilationAnalysisResult &right)
{
   ILIC_REQUIRE(left.compilation.success == right.compilation.success);
   ILIC_REQUIRE(left.compilation.errorCount == right.compilation.errorCount);
   ILIC_REQUIRE(left.compilation.warningCount == right.compilation.warningCount);
   ILIC_REQUIRE(left.compilation.missingModels == right.compilation.missingModels);
   ILIC_REQUIRE(left.compilation.models.size() == right.compilation.models.size());
   for (std::size_t index = 0; index < left.compilation.models.size(); ++index) {
      ILIC_REQUIRE(left.compilation.models[index].name == right.compilation.models[index].name);
      ILIC_REQUIRE(left.compilation.models[index].iliVersion == right.compilation.models[index].iliVersion);
   }
   ILIC_REQUIRE(left.semantic.success == right.semantic.success);
   ILIC_REQUIRE(left.semantic.missingModels == right.semantic.missingModels);
   ILIC_REQUIRE(left.semantic.symbols.size() == right.semantic.symbols.size());
   ILIC_REQUIRE(left.semantic.references.size() == right.semantic.references.size());
   ILIC_REQUIRE(left.semantic.dependencies.size() == right.semantic.dependencies.size());
}

} // namespace

int main()
{
   const std::string uri = "memory:///hardening/Partial.ili";
   const std::string sourceText = "INTERLIS 2.3;\nMODEL Partial =\n";
   ilic::SourceManager sources;
   const auto update = sources.update(uri,sourceText,7);
   ILIC_REQUIRE(update.accepted);
   const auto *source = sources.get(uri);
   ILIC_REQUIRE(source != nullptr);

   ilic::detail::SnapshotPipeline pipeline(sources);
   const auto strict = pipeline.build(*source,ilic::detail::ParseMode::StrictCompiler);
   const auto tolerant = pipeline.build(*source,ilic::detail::ParseMode::TolerantEditor);
   ILIC_REQUIRE(strict.mode == ilic::detail::ParseMode::StrictCompiler);
   ILIC_REQUIRE(tolerant.mode == ilic::detail::ParseMode::TolerantEditor);
   ILIC_REQUIRE(strict.artifact != nullptr);
   ILIC_REQUIRE(tolerant.artifact != nullptr);
   ILIC_REQUIRE(strict.artifact->mode() == ilic::detail::ParseMode::StrictCompiler);
   ILIC_REQUIRE(tolerant.artifact->mode() == ilic::detail::ParseMode::TolerantEditor);
   ILIC_REQUIRE(strict.artifact->supportsMetaModelBuild());
   ILIC_REQUIRE(!tolerant.artifact->supportsMetaModelBuild());
   ILIC_REQUIRE(!strict.syntax.success);
   ILIC_REQUIRE(tolerant.editor.recovered);
   ILIC_REQUIRE(!tolerant.editor.complete);
   ILIC_REQUIRE(strict.syntax.tokens.size() <= tolerant.syntax.tokens.size());

   ilic::detail::ParsedSourceCache cache({8,1024 * 1024});
   const auto builder = [&](const ilic::SourceBuffer &value,ilic::detail::ParseMode mode) {
      return pipeline.build(value,mode);
   };
   const auto strictEntry = cache.getOrBuild(*source,update.current,
      ilic::detail::ParseMode::StrictCompiler,"test-grammar",builder);
   const auto tolerantEntry = cache.getOrBuild(*source,update.current,
      ilic::detail::ParseMode::TolerantEditor,"test-grammar",builder);
   ILIC_REQUIRE(strictEntry != nullptr && tolerantEntry != nullptr);
   ILIC_REQUIRE(strictEntry->artifact->mode() != tolerantEntry->artifact->mode());
   ILIC_REQUIRE(cache.getOrBuild(*source,update.current,
      ilic::detail::ParseMode::StrictCompiler,"test-grammar",builder) == strictEntry);
   const auto metrics = cache.metrics();
   ILIC_REQUIRE(metrics.strictBuilds == 1);
   ILIC_REQUIRE(metrics.tolerantBuilds == 1);
   ILIC_REQUIRE(metrics.strictHits == 1);
   ILIC_REQUIRE(cache.checkInvariants());

   ilic::detail::SourceModelIndex models;
   const auto header = strict.artifact->header();
   const auto modelUpdate = models.replace(update.current,header,false);
   ILIC_REQUIRE(modelUpdate.current.uri == uri);
   ILIC_REQUIRE(models.checkInvariants());

   ilic::detail::RootAnalysisCache roots({4,1024 * 1024});
   ilic::detail::RootAnalysisCacheEntry rootEntry;
   rootEntry.baseKey = "base";
   rootEntry.key = "root";
   rootEntry.closureUris = {uri};
   rootEntry.identities = {update.current};
   roots.insert(std::move(rootEntry));
   ILIC_REQUIRE(roots.metrics().entries == 1);
   ILIC_REQUIRE(roots.metrics().retainedBytes > 0);
   ILIC_REQUIRE(roots.checkInvariants());
   roots.invalidateBySource(uri);
   ILIC_REQUIRE(roots.metrics().entries == 0);
   ILIC_REQUIRE(roots.checkInvariants());
   const auto removed = models.remove(uri);
   ILIC_REQUIRE(removed.previous.uri == uri);
   ILIC_REQUIRE(models.checkInvariants());

   const std::string stableUri = "memory:///hardening/Stable.ili";
   const std::string stableSource = validModel("Stable");
   ilic::CompilationRequest stableRequest;
   stableRequest.roots.push_back(stableUri);
   const auto runOrder = [&](int order) {
      ilic::CompilerSession current;
      ILIC_REQUIRE(current.updateSource(stableUri,stableSource,1).accepted);
      if (order == 0) {
         current.editorSnapshot(stableUri);
         current.compileAndAnalyze(stableRequest);
      } else if (order == 1) {
         current.compileAndAnalyze(stableRequest);
         current.editorSnapshot(stableUri);
      } else if (order == 2) {
         current.parse(stableUri);
         current.compileAndAnalyze(stableRequest);
      } else if (order == 3) {
         current.compileAndAnalyze(stableRequest);
         current.parse(stableUri);
      } else if (order == 4) {
         current.editorSnapshot(stableUri);
         current.parse(stableUri);
         current.compileAndAnalyze(stableRequest);
      } else {
         current.compileAndAnalyze(stableRequest);
         current.parse(stableUri);
         current.editorSnapshot(stableUri);
      }
      return current.compileAndAnalyze(stableRequest);
   };
   const auto cold = [&]() {
      ilic::CompilerSession current;
      ILIC_REQUIRE(current.updateSource(stableUri,stableSource,1).accepted);
      return current.compileAndAnalyze(stableRequest);
   }();
   for (int order = 0; order < 6; ++order) requireEquivalent(runOrder(order),cold);

   const std::string missingRoot = "memory:///hardening/MissingRoot.ili";
   const std::string missingSource =
      "INTERLIS 2.3;\nMODEL MissingRoot AT \"https://example.invalid/ilic/hardening\" VERSION \"1\" =\n"
      "  IMPORTS MissingModel;\nEND MissingRoot.\n";
   ilic::CompilerSession missingSession;
   ILIC_REQUIRE(missingSession.updateSource(missingRoot,missingSource,1).accepted);
   ilic::CompilationRequest missingRequest;
   missingRequest.roots.push_back(missingRoot);
   const auto missing = missingSession.compileAndAnalyze(missingRequest);
   ILIC_REQUIRE(!missing.compilation.success);
   ILIC_REQUIRE(missing.compilation.missingModels.size() == 1);
   const auto missingStats = missingSession.incrementalStats();
   ILIC_REQUIRE(missingStats.rootAnalysisBuilds == 1);
   ILIC_REQUIRE(missingSession.updateSource("memory:///hardening/MissingModel.ili",
      validModel("MissingModel"),1).accepted);
   const auto resolved = missingSession.compileAndAnalyze(missingRequest);
   ILIC_REQUIRE(resolved.compilation.success);
   ILIC_REQUIRE(missingSession.incrementalStats().rootAnalysisBuilds == 2);

   return 0;
}
