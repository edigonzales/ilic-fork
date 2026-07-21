#include "ilic/Repository.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <string>

namespace {

const ilic::Diagnostic *findDiagnostic(const ilic::RepositoryResult &result,const std::string &code)
{
   const auto found = std::find_if(result.diagnostics.begin(),result.diagnostics.end(),
      [&](const ilic::Diagnostic &diagnostic) { return diagnostic.code == code; });
   return found == result.diagnostics.end() ? nullptr : &*found;
}

}

int main(int argc,char **argv)
{
   assert(argc == 2);
   const std::filesystem::path fixture = std::filesystem::absolute(argv[1]);
   ilic::RepositoryOptions options;
   options.repositories = {fixture.string()};
   options.followSiteLinks = false;
   ilic::RepositoryManager manager(options);

   auto root = manager.resolve("Root","");
   assert(root.success && root.models.size() == 2);
   assert(root.models[0].metadata.name == "Base");
   assert(root.models[1].metadata.name == "Root");
   assert(root.models[1].metadata.version == "2.10");

   auto shared = manager.resolve(std::vector<std::string>{"SharedA","SharedB"},"ili2_4");
   assert(shared.success && shared.models.size() == 1);
   auto sharedBad = manager.resolve(
      std::vector<std::string>{"SharedA","SharedBadChecksum"},"ili2_4");
   assert(!sharedBad.success && sharedBad.models.size() == 1);
   assert(findDiagnostic(sharedBad,"ILIC-REPO-CHECKSUM"));

   auto cycle = manager.resolve("CycleA","ili2_4");
   assert(!cycle.success);
   const auto *cycleDiagnostic = findDiagnostic(cycle,"ILIC-REPO-CYCLE");
   assert(cycleDiagnostic && cycleDiagnostic->message.find("CycleA -> CycleB -> CycleA")
      != std::string::npos);

   auto unsafe = manager.resolve("Unsafe","ili2_4");
   assert(!unsafe.success && findDiagnostic(unsafe,"ILIC-REPO-PATH"));

   auto browsable = manager.resolve("Browsable","ili2_4");
   assert(browsable.success && browsable.models[0].metadata.version == "1");
   auto language = manager.resolve("LanguageChoice","");
   assert(language.success && language.models[0].metadata.schemaLanguage == "ili2_4");

   ilic::RepositoryOptions partial;
   partial.repositories = {(fixture / "missing").string(),fixture.string()};
   partial.followSiteLinks = false;
   ilic::RepositoryManager partialManager(partial);
   auto recovered = partialManager.resolve("Base","ili2_4");
   assert(recovered.success && findDiagnostic(recovered,"ILIC-REPO-INDEX"));
   assert(findDiagnostic(recovered,"ILIC-REPO-INDEX")->severity
      == ilic::DiagnosticSeverity::Warning);

   ilic::RepositoryOptions fileOptions;
   fileOptions.repositories = {"file://" + fixture.generic_string()};
   fileOptions.followSiteLinks = false;
   ilic::RepositoryManager fileManager(fileOptions);
   assert(fileManager.resolve("Base","ili2_4").success);
   return 0;
}
