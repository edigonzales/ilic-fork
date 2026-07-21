#include "RepositoryCrawler.h"

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace {

ilic::ModelMetadata model(std::string name,std::string repository)
{
   ilic::ModelMetadata value;
   value.name = std::move(name);
   value.repository = std::move(repository);
   value.schemaLanguage = "ili2_4";
   value.file = value.name + ".ili";
   value.version = "1";
   return value;
}

class FakeBackend final : public ilic::repository::RepositoryBackend {
public:
   std::map<std::string,ilic::repository::RepositoryIndex> indexes;
   std::map<std::string,ilic::repository::RepositorySite> sites;
   std::vector<std::string> indexRequests;
   std::vector<std::string> siteRequests;

   const ilic::repository::RepositoryIndex *loadIndex(const std::string &repository,
      std::vector<ilic::Diagnostic> *) override
   {
      indexRequests.push_back(repository);
      auto found = indexes.find(repository);
      return found == indexes.end() ? nullptr : &found->second;
   }
   const ilic::repository::RepositorySite *loadSite(const std::string &repository,
      std::vector<ilic::Diagnostic> *) override
   {
      siteRequests.push_back(repository);
      auto found = sites.find(repository);
      return found == sites.end() ? nullptr : &found->second;
   }
};

void addEmpty(FakeBackend &backend,const std::string &repository)
{
   backend.indexes[repository].repository = repository;
   backend.sites[repository] = {};
}

}

int main()
{
   {
      FakeBackend backend;
      addEmpty(backend,"/A"); addEmpty(backend,"/B");
      backend.indexes["/A"].models.push_back(model("ModelX","/A"));
      backend.indexes["/B"].models.push_back(model("ModelX","/B"));
      ilic::repository::RepositoryCrawler crawler(backend);
      auto found = crawler.findModel({"/A","/B"},"ModelX","ili2_4");
      assert(found.found && found.metadata.repository == "/A");
      assert((backend.indexRequests == std::vector<std::string>{"/A"}));
      assert(backend.siteRequests.empty());
   }
   {
      FakeBackend backend;
      for (const auto &repository : {"/A","/B","/P1","/P2","/P3"}) addEmpty(backend,repository);
      backend.sites["/A"].parentSites = {"/P1","/P2"};
      backend.sites["/B"].parentSites = {"/P3"};
      backend.indexes["/P3"].models.push_back(model("Wanted","/P3"));
      ilic::repository::RepositoryCrawler crawler(backend);
      auto found = crawler.findModel({"/A","/B"},"Wanted","ili2_4");
      assert(found.found && found.metadata.repository == "/P3");
      assert((backend.indexRequests == std::vector<std::string>{"/A","/B","/P1","/P2","/P3"}));
   }
   {
      FakeBackend backend;
      for (const auto &repository : {"/A","/C1","/C2","/P"}) addEmpty(backend,repository);
      backend.sites["/A"].subsidiarySites = {"/C1","/C2"};
      backend.sites["/C1"].parentSites = {"/P"};
      backend.indexes["/P"].models.push_back(model("Wanted","/P"));
      ilic::repository::RepositoryCrawler crawler(backend);
      auto found = crawler.findModel({"/A"},"Wanted","ili2_4");
      assert(found.found && found.metadata.repository == "/P");
      assert((backend.indexRequests == std::vector<std::string>{"/A","/C1","/P"}));
   }
   {
      FakeBackend backend;
      addEmpty(backend,"/A"); addEmpty(backend,"/B");
      backend.sites["/A"].parentSites = {"/B"};
      backend.sites["/B"].parentSites = {"/A"};
      ilic::repository::RepositoryCrawler crawler(backend);
      assert(!crawler.findModel({"/A"},"Missing","ili2_4").found);
      assert((backend.indexRequests == std::vector<std::string>{"/A","/B"}));
   }
   return 0;
}
