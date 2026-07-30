#include "ilic/Compiler.h"
#include "ilic/Repository.h"

#include "ilic/test/TestHarness.h"
#include <filesystem>

int main(int argc,char **argv)
{
   ILIC_REQUIRE_MSG(argc == 2,"expected exactly one fixture-directory argument");
   ilic::RepositoryOptions options;
   options.repositories.push_back(argv[1]);
   options.followSiteLinks = false;
   ilic::RepositoryManager manager(options);

   const auto catalog = manager.listModels();
   ILIC_REQUIRE(catalog.size() == 2);
   ilic::RepositoryResult resolved = manager.resolve("RepositoryRoot","ili2_3");
   ILIC_REQUIRE(resolved.success);
   ILIC_REQUIRE(resolved.models.size() == 2);
   ILIC_REQUIRE(resolved.models.front().metadata.name == "RepositoryBase");
   ILIC_REQUIRE(resolved.models.back().metadata.name == "RepositoryRoot");

   ilic::CompilerSession session;
   std::string rootUri;
   for (const auto &model : resolved.models) {
      session.putSource(model.uri,model.source);
      if (model.metadata.name == "RepositoryRoot") rootUri = model.uri;
   }
   ILIC_REQUIRE(!rootUri.empty());
   ilic::CompilationRequest request;
   request.roots.push_back(rootUri);
   const auto compilation = session.compile(request);
   ILIC_REQUIRE(compilation.success);
   return 0;
}
