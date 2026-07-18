#include "../../include/ilic/Repository.h"

#include "Md5.h"
#include "../util/Logger.h"

#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <map>
#include <queue>
#include <set>
#include <sstream>

namespace ilic {
namespace {

struct FetchResult {
   bool success = false;
   bool fromCache = false;
   std::string text;
   std::filesystem::path path;
   std::string error;
};

bool isHttp(const std::string &uri)
{
   return uri.rfind("http://",0) == 0 || uri.rfind("https://",0) == 0;
}

std::string stripFileScheme(const std::string &uri)
{
   return uri.rfind("file://",0) == 0 ? uri.substr(7) : uri;
}

std::string joinUri(const std::string &base,const std::string &relative)
{
   if (relative.rfind("http://",0) == 0 || relative.rfind("https://",0) == 0
       || relative.rfind("file://",0) == 0) return relative;
   if (isHttp(base)) return base + (base.back() == '/' ? "" : "/") + relative;
   return (std::filesystem::path(stripFileScheme(base)) / relative).lexically_normal().string();
}

bool safeRelativePath(const std::string &value)
{
   std::filesystem::path path(value);
   if (path.is_absolute()) return false;
   for (const auto &component : path)
      if (component == "..") return false;
   return true;
}

std::string readFile(const std::filesystem::path &path)
{
   std::ifstream input(path,std::ios::binary);
   if (!input) return {};
   return std::string((std::istreambuf_iterator<char>(input)),std::istreambuf_iterator<char>());
}

bool isFresh(const std::filesystem::path &path,std::chrono::seconds ttl)
{
   std::error_code error;
   auto written = std::filesystem::last_write_time(path,error);
   if (error) return false;
   auto age = std::filesystem::file_time_type::clock::now() - written;
   return age <= ttl;
}

std::filesystem::path defaultCacheDirectory()
{
   if (const char *configured = std::getenv("ILIC_CACHE"))
      if (*configured != '\0') return std::filesystem::path(configured);
   if (const char *legacy = std::getenv("ILI_CACHE"))
      if (*legacy != '\0') return std::filesystem::path(legacy) / "ilic-v1";
   if (const char *home = std::getenv("HOME"))
      return std::filesystem::path(home) / ".ilicache" / "ilic-v1";
   return std::filesystem::temp_directory_path() / "ilic-cache-v1";
}

std::size_t curlWrite(char *data,std::size_t size,std::size_t count,void *target)
{
   static_cast<std::string *>(target)->append(data,size * count);
   return size * count;
}

FetchResult fetch(const RepositoryOptions &options,const std::string &uri,
   std::chrono::seconds ttl,bool optional = false)
{
   FetchResult result;
   if (!isHttp(uri)) {
      result.path = stripFileScheme(uri);
      result.text = readFile(result.path);
      result.success = !result.text.empty() || std::filesystem::exists(result.path);
      if (!result.success && !optional) result.error = "unable to read " + result.path.string();
      return result;
   }

   const std::filesystem::path cacheRoot = options.cacheDirectory.empty()
      ? defaultCacheDirectory() : options.cacheDirectory;
   const std::string extension = std::filesystem::path(uri).extension().string();
   result.path = cacheRoot / (repository::md5(uri) + extension);
   if (std::filesystem::exists(result.path) && (options.offline || isFresh(result.path,ttl))) {
      result.text = readFile(result.path);
      result.success = true;
      result.fromCache = true;
      Log.log(LogLevel::Debug,"cache","cache hit",{{"uri",uri},{"path",result.path.string()}});
      return result;
   }
   if (options.offline) {
      if (!optional) result.error = "offline and no cached copy of " + uri;
      return result;
   }

   CURL *curl = curl_easy_init();
   if (curl != nullptr) {
      curl_easy_setopt(curl,CURLOPT_URL,uri.c_str());
      curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
      curl_easy_setopt(curl,CURLOPT_FAILONERROR,1L);
      curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,15L);
      curl_easy_setopt(curl,CURLOPT_TIMEOUT,60L);
      curl_easy_setopt(curl,CURLOPT_USERAGENT,"ilic/0.9.9");
      curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,curlWrite);
      curl_easy_setopt(curl,CURLOPT_WRITEDATA,&result.text);
      CURLcode status = curl_easy_perform(curl);
      if (status == CURLE_OK) result.success = true;
      else result.error = curl_easy_strerror(status);
      curl_easy_cleanup(curl);
   }
   else result.error = "unable to initialize libcurl";

   if (result.success) {
      std::error_code error;
      std::filesystem::create_directories(cacheRoot,error);
      std::filesystem::path temporary = result.path;
      temporary += ".tmp";
      {
         std::ofstream output(temporary,std::ios::binary | std::ios::trunc);
         output.write(result.text.data(),static_cast<std::streamsize>(result.text.size()));
      }
      std::filesystem::rename(temporary,result.path,error);
      if (error) {
         std::filesystem::remove(result.path,error);
         error.clear();
         std::filesystem::rename(temporary,result.path,error);
      }
      Log.log(LogLevel::Information,"repository","downloaded repository resource",{{"uri",uri}});
      return result;
   }

   if (options.allowStaleOnError && std::filesystem::exists(result.path)) {
      result.text = readFile(result.path);
      result.success = true;
      result.fromCache = true;
      Log.log(LogLevel::Warning,"cache","using stale cache entry",{{"uri",uri},{"error",result.error}});
      return result;
   }
   if (optional) result.error.clear();
   return result;
}

std::string nodeText(xmlNode *node)
{
   xmlChar *content = xmlNodeGetContent(node);
   if (content == nullptr) return {};
   std::string value(reinterpret_cast<const char *>(content));
   xmlFree(content);
   return value;
}

std::string childText(xmlNode *node,const char *name)
{
   for (xmlNode *child = node->children; child != nullptr; child = child->next) {
      if (child->type == XML_ELEMENT_NODE && xmlStrEqual(child->name,BAD_CAST name)) return nodeText(child);
   }
   return {};
}

void descendantValues(xmlNode *node,std::vector<std::string> &values)
{
   for (xmlNode *child = node->children; child != nullptr; child = child->next) {
      if (child->type != XML_ELEMENT_NODE) continue;
      if (xmlStrEqual(child->name,BAD_CAST "value")) values.push_back(nodeText(child));
      else descendantValues(child,values);
   }
}

void collectMetadata(xmlNode *node,const std::string &repository,
   std::vector<ModelMetadata> &models)
{
   for (xmlNode *current = node; current != nullptr; current = current->next) {
      if (current->type == XML_ELEMENT_NODE) {
         const std::string name(reinterpret_cast<const char *>(current->name));
         if (name.find("ModelMetadata") != std::string::npos) {
            ModelMetadata model;
            model.name = childText(current,"Name");
            model.schemaLanguage = childText(current,"SchemaLanguage");
            model.file = childText(current,"File");
            model.version = childText(current,"Version");
            model.publishingDate = childText(current,"publishingDate");
            model.precursorVersion = childText(current,"precursorVersion");
            model.md5 = childText(current,"md5");
            model.repository = repository;
            model.browseOnly = childText(current,"browseOnly") == "true";
            for (xmlNode *child = current->children; child != nullptr; child = child->next) {
               if (child->type == XML_ELEMENT_NODE && xmlStrEqual(child->name,BAD_CAST "dependsOnModel"))
                  descendantValues(child,model.dependencies);
            }
            if (!model.name.empty() && !model.file.empty()) models.push_back(std::move(model));
         }
         else collectMetadata(current->children,repository,models);
      }
   }
}

std::vector<std::string> parseSiteLinks(const std::string &xml)
{
   std::vector<std::string> links;
   xmlDocPtr document = xmlReadMemory(xml.data(),static_cast<int>(xml.size()),"ilisite.xml",nullptr,
      XML_PARSE_NONET | XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
   if (document == nullptr) return links;
   std::function<void(xmlNode *,bool)> visit = [&](xmlNode *node,bool inLink) {
      for (xmlNode *current = node; current != nullptr; current = current->next) {
         bool link = inLink;
         if (current->type == XML_ELEMENT_NODE) {
            link = link || xmlStrEqual(current->name,BAD_CAST "parentSite")
               || xmlStrEqual(current->name,BAD_CAST "subsidiarySite");
            if (link && xmlStrEqual(current->name,BAD_CAST "value")) links.push_back(nodeText(current));
            visit(current->children,link);
         }
      }
   };
   visit(xmlDocGetRootElement(document),false);
   xmlFreeDoc(document);
   return links;
}

Diagnostic repositoryError(const std::string &code,const std::string &message)
{
   Diagnostic diagnostic;
   diagnostic.code = code;
   diagnostic.message = message;
   return diagnostic;
}

struct Catalog {
   std::vector<ModelMetadata> models;
   std::vector<Diagnostic> diagnostics;
};

Catalog loadCatalog(const RepositoryOptions &options)
{
   Catalog catalog;
   std::queue<std::string> pending;
   for (const auto &repository : options.repositories) pending.push(repository);
   std::set<std::string> visited;
   while (!pending.empty()) {
      std::string repository = pending.front();
      pending.pop();
      while (repository.size() > 1 && repository.back() == '/') repository.pop_back();
      if (!visited.insert(repository).second) continue;

      FetchResult index = fetch(options,joinUri(repository,"ilimodels.xml"),options.metadataTtl);
      if (!index.success) {
         catalog.diagnostics.push_back(repositoryError("ILIC-REPO-INDEX",
            "unable to read ilimodels.xml from " + repository + ": " + index.error));
         continue;
      }
      auto parsed = RepositoryManager::parseIliModelsXml(index.text,repository,&catalog.diagnostics);
      catalog.models.insert(catalog.models.end(),parsed.begin(),parsed.end());

      if (options.followSiteLinks) {
         FetchResult site = fetch(options,joinUri(repository,"ilisite.xml"),options.metadataTtl,true);
         if (site.success) for (const auto &link : parseSiteLinks(site.text)) pending.push(link);
      }
   }
   return catalog;
}

const ModelMetadata *selectModel(const std::vector<ModelMetadata> &models,
   const std::string &name,const std::string &schemaLanguage)
{
   const ModelMetadata *selected = nullptr;
   for (const auto &model : models) {
      if (model.name != name || model.browseOnly) continue;
      if (!schemaLanguage.empty() && model.schemaLanguage != schemaLanguage) continue;
      if (selected == nullptr) selected = &model;
      else if (selected->repository == model.repository && selected->version < model.version) selected = &model;
   }
   return selected;
}

} // namespace

RepositoryManager::RepositoryManager(RepositoryOptions options) : options_(std::move(options))
{
   if (options_.cacheDirectory.empty()) options_.cacheDirectory = defaultCacheDirectory();
   curl_global_init(CURL_GLOBAL_DEFAULT);
}

const RepositoryOptions &RepositoryManager::options() const { return options_; }

std::vector<ModelMetadata> RepositoryManager::parseIliModelsXml(
   const std::string &xml,const std::string &repository,std::vector<Diagnostic> *diagnostics)
{
   std::vector<ModelMetadata> models;
   xmlDocPtr document = xmlReadMemory(xml.data(),static_cast<int>(xml.size()),"ilimodels.xml",nullptr,
      XML_PARSE_NONET | XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
   if (document == nullptr) {
      if (diagnostics != nullptr) diagnostics->push_back(repositoryError(
         "ILIC-REPO-INDEX","invalid ilimodels.xml in " + repository));
      return models;
   }
   collectMetadata(xmlDocGetRootElement(document),repository,models);
   xmlFreeDoc(document);
   return models;
}

std::vector<ModelMetadata> RepositoryManager::listModels()
{
   return loadCatalog(options_).models;
}

RepositoryResult RepositoryManager::resolve(const std::string &model,const std::string &schemaLanguage)
{
   return resolve(std::vector<std::string>{model},schemaLanguage);
}

RepositoryResult RepositoryManager::resolve(const std::vector<std::string> &requested,
   const std::string &schemaLanguage)
{
   RepositoryResult result;
   Catalog catalog = loadCatalog(options_);
   result.diagnostics = std::move(catalog.diagnostics);
   if (!result.diagnostics.empty() && catalog.models.empty()) return result;

   std::set<std::string> resolved;
   std::set<std::string> visiting;
   std::set<std::string> fetchedFiles;
   std::function<bool(const std::string &)> resolveOne = [&](const std::string &name) {
      if (name == "INTERLIS" || resolved.count(name) != 0) return true;
      if (!visiting.insert(name).second) {
         result.diagnostics.push_back(repositoryError("ILIC-REPO-CYCLE",
            "dependency cycle involving model " + name));
         return false;
      }
      const ModelMetadata *metadata = selectModel(catalog.models,name,schemaLanguage);
      if (metadata == nullptr) {
         result.diagnostics.push_back(repositoryError("ILIC-REPO-NOT-FOUND",
            "model " + name + " not found in configured repositories"));
         visiting.erase(name);
         return false;
      }
      bool dependenciesOk = true;
      for (const auto &dependency : metadata->dependencies) dependenciesOk = resolveOne(dependency) && dependenciesOk;
      if (!safeRelativePath(metadata->file)) {
         result.diagnostics.push_back(repositoryError("ILIC-REPO-PATH",
            "unsafe repository path " + metadata->file));
         visiting.erase(name);
         return false;
      }
      const std::string uri = joinUri(metadata->repository,metadata->file);
      if (fetchedFiles.insert(uri).second) {
         FetchResult file = fetch(options_,uri,options_.modelTtl);
         if (!file.success) {
            result.diagnostics.push_back(repositoryError("ILIC-REPO-DOWNLOAD",
               "unable to fetch " + uri + ": " + file.error));
            visiting.erase(name);
            return false;
         }
         if (!metadata->md5.empty() && repository::md5(file.text) != metadata->md5) {
            result.diagnostics.push_back(repositoryError("ILIC-REPO-CHECKSUM",
               "MD5 mismatch for " + uri));
            visiting.erase(name);
            return false;
         }
         result.models.push_back({*metadata,uri,file.path,std::move(file.text),file.fromCache});
      }
      visiting.erase(name);
      resolved.insert(name);
      return dependenciesOk;
   };

   bool success = true;
   for (const auto &model : requested) success = resolveOne(model) && success;
   result.success = success;
   return result;
}

} // namespace ilic
