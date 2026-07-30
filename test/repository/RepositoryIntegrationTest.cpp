#include "ilic/Repository.h"

#include <algorithm>
#include <cctype>
#include "ilic/test/TestHarness.h"
#include <filesystem>
#include <string>

namespace {

const ilic::Diagnostic *findDiagnostic(const ilic::RepositoryResult &result,const std::string &code)
{
   const auto found = std::find_if(result.diagnostics.begin(),result.diagnostics.end(),
      [&](const ilic::Diagnostic &diagnostic) { return diagnostic.code == code; });
   return found == result.diagnostics.end() ? nullptr : &*found;
}

std::string fileUri(const std::filesystem::path &path)
{
   const std::string generic = path.generic_string();
#ifdef _WIN32
   if (generic.size() >= 2 && std::isalpha(static_cast<unsigned char>(generic[0]))
       && generic[1] == ':') return "file:///" + generic;
#endif
   return "file://" + generic;
}

}

int main(int argc,char **argv)
{
   ILIC_REQUIRE_MSG(argc == 2,"expected exactly one integration-fixture directory argument");
   const std::filesystem::path fixture = std::filesystem::absolute(argv[1]);
   ilic::RepositoryOptions options;
   options.repositories = {fixture.string()};
   options.followSiteLinks = false;
   ilic::RepositoryManager manager(options);

   auto root = manager.resolve("Root","");
   ILIC_REQUIRE(root.success);
   ILIC_REQUIRE(root.models.size() == 2);
   ILIC_REQUIRE(root.models[0].metadata.name == "Base");
   ILIC_REQUIRE(root.models[1].metadata.name == "Root");
   ILIC_REQUIRE(root.models[1].metadata.version == "2.10");

   auto shared = manager.resolve(std::vector<std::string>{"SharedA","SharedB"},"ili2_4");
   ILIC_REQUIRE(shared.success);
   ILIC_REQUIRE(shared.models.size() == 1);
   auto sharedBad = manager.resolve(
      std::vector<std::string>{"SharedA","SharedBadChecksum"},"ili2_4");
   ILIC_REQUIRE(!sharedBad.success);
   ILIC_REQUIRE(sharedBad.models.size() == 1);
   const auto *checksumDiagnostic = findDiagnostic(sharedBad,"ILIC-REPO-CHECKSUM");
   ILIC_REQUIRE(checksumDiagnostic != nullptr);

   auto cycle = manager.resolve("CycleA","ili2_4");
   ILIC_REQUIRE(!cycle.success);
   const auto *cycleDiagnostic = findDiagnostic(cycle,"ILIC-REPO-CYCLE");
   ILIC_REQUIRE(cycleDiagnostic != nullptr);
   ILIC_REQUIRE(cycleDiagnostic->message.find("CycleA -> CycleB -> CycleA")
      != std::string::npos);

   auto unsafe = manager.resolve("Unsafe","ili2_4");
   ILIC_REQUIRE(!unsafe.success);
   const auto *pathDiagnostic = findDiagnostic(unsafe,"ILIC-REPO-PATH");
   ILIC_REQUIRE(pathDiagnostic != nullptr);

   auto browsable = manager.resolve("Browsable","ili2_4");
   ILIC_REQUIRE(browsable.success);
   ILIC_REQUIRE(browsable.models.size() >= 1);
   ILIC_REQUIRE(browsable.models[0].metadata.version == "1");
   auto language = manager.resolve("LanguageChoice","");
   ILIC_REQUIRE(language.success);
   ILIC_REQUIRE(language.models.size() >= 1);
   ILIC_REQUIRE(language.models[0].metadata.schemaLanguage == "ili2_4");

   ilic::RepositoryOptions partial;
   partial.repositories = {(fixture / "missing").string(),fixture.string()};
   partial.followSiteLinks = false;
   ilic::RepositoryManager partialManager(partial);
   auto recovered = partialManager.resolve("Base","ili2_4");
   ILIC_REQUIRE(recovered.success);
   const auto *indexDiagnostic = findDiagnostic(recovered,"ILIC-REPO-INDEX");
   ILIC_REQUIRE(indexDiagnostic != nullptr);
   ILIC_REQUIRE(indexDiagnostic->severity == ilic::DiagnosticSeverity::Warning);

   ilic::RepositoryOptions fileOptions;
   fileOptions.repositories = {fileUri(fixture)};
   fileOptions.followSiteLinks = false;
   ilic::RepositoryManager fileManager(fileOptions);
   const auto fileResult = fileManager.resolve("Base","ili2_4");
   const std::string fileError = fileResult.diagnostics.empty()
      ? "repository resolution failed" : fileResult.diagnostics.front().message;
   ILIC_REQUIRE_MSG(fileResult.success,fileError);
   return 0;
}
