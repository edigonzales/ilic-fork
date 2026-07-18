#include "ilic/Compiler.h"
#include "ilic/Repository.h"

#include <cassert>
#include <filesystem>

int main(int argc,char **argv)
{
   assert(argc == 2);
   ilic::RepositoryOptions options;
   options.repositories.push_back(argv[1]);
   options.followSiteLinks = false;
   ilic::RepositoryManager manager(options);

   const auto catalog = manager.listModels();
   assert(catalog.size() == 2);
   ilic::RepositoryResult resolved = manager.resolve("RepositoryRoot","ili2_3");
   assert(resolved.success);
   assert(resolved.models.size() == 2);
   assert(resolved.models.front().metadata.name == "RepositoryBase");
   assert(resolved.models.back().metadata.name == "RepositoryRoot");

   ilic::CompilerSession session;
   std::string rootUri;
   for (const auto &model : resolved.models) {
      session.putSource(model.uri,model.source);
      if (model.metadata.name == "RepositoryRoot") rootUri = model.uri;
   }
   ilic::CompilationRequest request;
   request.roots.push_back(rootUri);
   assert(session.compile(request).success);
   return 0;
}
