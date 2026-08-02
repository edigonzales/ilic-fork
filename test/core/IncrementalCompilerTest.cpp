#include "ilic/Compiler.h"
#include "ilic/Semantic.h"

#include "ilic/test/TestHarness.h"

#include <string>

namespace {

const char *modelSource(const char *model,const char *body = "")
{
   static std::string source;
   source = "INTERLIS 2.3;\nMODEL " + std::string(model)
      + " AT \"https://example.invalid/ilic/tests\" VERSION \"1\" =\n"
      + body + "\nEND " + model + ".\n";
   return source.c_str();
}

} // namespace

int main()
{
   ilic::CompilerSession session;
   const std::string root = "memory:///incremental/Root.ili";
   const std::string other = "memory:///incremental/Other.ili";
   const std::string source = modelSource("Root");

   const auto added = session.updateSource(root,source,1);
   ILIC_REQUIRE(added.kind == ilic::SourceUpdateKind::Added);
   ILIC_REQUIRE(added.accepted);
   ILIC_REQUIRE(added.current.contentRevision != 0);

   const auto unchanged = session.updateSource(root,source,1);
   ILIC_REQUIRE(unchanged.kind == ilic::SourceUpdateKind::Unchanged);
   const auto versionOnly = session.updateSource(root,source,2);
   ILIC_REQUIRE(versionOnly.kind == ilic::SourceUpdateKind::VersionOnly);
   ILIC_REQUIRE(!versionOnly.parserInvalidated);
   const auto rejected = session.updateSource(root,source,1);
   ILIC_REQUIRE(rejected.kind == ilic::SourceUpdateKind::Rejected);
   ILIC_REQUIRE(!rejected.accepted);

   const auto editor = session.editorSnapshot(root);
   const auto syntax = session.parse(root);
   ILIC_REQUIRE(editor.uri == root);
   ILIC_REQUIRE(syntax.uri == root);
   ILIC_REQUIRE(syntax.documentVersion == 2);
   auto stats = session.incrementalStats();
   ILIC_REQUIRE(stats.parserBuilds == 2);
   ILIC_REQUIRE(stats.strictParserBuilds == 1);
   ILIC_REQUIRE(stats.tolerantParserBuilds == 1);
   ILIC_REQUIRE(stats.parserHits >= 1);
   ILIC_REQUIRE(stats.editorMaterializations == 1);
   ILIC_REQUIRE(stats.syntaxMaterializations == 1);

   const std::string trailingComment = source + "!! dirty";
   const auto commentUpdate = session.updateSource(root,trailingComment,3);
   ILIC_REQUIRE(commentUpdate.kind == ilic::SourceUpdateKind::ContentChanged);
   ILIC_REQUIRE(session.parse(root).success);

   ilic::CompilationRequest request;
   request.roots.push_back(root);
   const auto first = session.compileAndAnalyze(request);
   ILIC_REQUIRE(first.compilation.success);
   stats = session.incrementalStats();
   ILIC_REQUIRE(stats.rootAnalysisBuilds == 1);
   const auto second = session.compileAndAnalyze(request);
   ILIC_REQUIRE(second.compilation.success);
   ILIC_REQUIRE(second.semantic.documentVersions.at(root) == 3);
   stats = session.incrementalStats();
   ILIC_REQUIRE(stats.rootAnalysisHits == 1);

   const auto changed = session.updateSource(root,modelSource("Root",
      "  TOPIC Topic = END Topic;\n"),4);
   ILIC_REQUIRE(changed.kind == ilic::SourceUpdateKind::ContentChanged);
   const auto rebuilt = session.compileAndAnalyze(request);
   ILIC_REQUIRE(rebuilt.compilation.success);
   stats = session.incrementalStats();
   ILIC_REQUIRE(stats.rootAnalysisBuilds == 2);
   ILIC_REQUIRE(stats.invalidatedRootEntries >= 1);

   ILIC_REQUIRE(session.updateSource(other,modelSource("Other"),1).accepted);
   const auto beforeIndependent = session.incrementalStats();
   const auto independent = session.compileAndAnalyze(request);
   ILIC_REQUIRE(independent.compilation.success);
   const auto afterIndependent = session.incrementalStats();
   ILIC_REQUIRE(afterIndependent.rootAnalysisHits == beforeIndependent.rootAnalysisHits + 1);
   ILIC_REQUIRE(afterIndependent.rootAnalysisBuilds == beforeIndependent.rootAnalysisBuilds);

   ilic::CompilerSession bounded({{1,1},{1,1}});
   ILIC_REQUIRE(bounded.updateSource("memory:///A.ili",modelSource("A"),1).accepted);
   ILIC_REQUIRE(bounded.updateSource("memory:///B.ili",modelSource("B"),1).accepted);
   bounded.parse("memory:///A.ili");
   bounded.parse("memory:///B.ili");
   const auto boundedStats = bounded.incrementalStats();
   ILIC_REQUIRE(boundedStats.parserEvictions >= 1);
   ILIC_REQUIRE(boundedStats.parserBytes == 0);

   ILIC_REQUIRE(session.removeSource(other));
   const auto reintroduced = session.updateSource(other,modelSource("Other"),2);
   ILIC_REQUIRE(reintroduced.kind == ilic::SourceUpdateKind::Reintroduced);
   ILIC_REQUIRE(reintroduced.accepted);
   return 0;
}
