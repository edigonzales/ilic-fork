#pragma once

#include "Diagnostic.h"

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace ilic {

struct ModelMetadata {
   std::string name;
   std::string schemaLanguage;
   std::string file;
   std::string version;
   std::string publishingDate;
   std::string precursorVersion;
   std::string md5;
   std::string repository;
   std::vector<std::string> dependencies;
   bool browseOnly = false;
};

struct RepositoryOptions {
   std::vector<std::string> repositories;
   std::filesystem::path cacheDirectory;
   std::chrono::seconds metadataTtl = std::chrono::hours(24);
   std::chrono::seconds modelTtl = std::chrono::hours(24 * 7);
   bool offline = false;
   bool allowStaleOnError = true;
   bool followSiteLinks = true;
   bool validateChecksums = true;
};

struct ResolvedModel {
   ModelMetadata metadata;
   std::string uri;
   std::filesystem::path localPath;
   std::string source;
   bool fromCache = false;
   bool stale = false;
};

struct RepositoryResult {
   bool success = false;
   std::vector<ResolvedModel> models;
   std::vector<Diagnostic> diagnostics;
};

class RepositoryManager {
public:
   explicit RepositoryManager(RepositoryOptions options = {});
   ~RepositoryManager();

   RepositoryManager(RepositoryManager &&) noexcept;
   RepositoryManager &operator=(RepositoryManager &&) noexcept;

   RepositoryManager(const RepositoryManager &) = delete;
   RepositoryManager &operator=(const RepositoryManager &) = delete;

   const RepositoryOptions &options() const;
   std::vector<ModelMetadata> listModels();
   RepositoryResult resolve(const std::vector<std::string> &models,
      const std::string &schemaLanguage);
   RepositoryResult resolve(const std::string &model,const std::string &schemaLanguage);

   static std::vector<std::string> defaultRepositories();

   static std::vector<ModelMetadata> parseIliModelsXml(
      const std::string &xml,const std::string &repository,std::vector<Diagnostic> *diagnostics = nullptr);

private:
   class Impl;
   std::unique_ptr<Impl> impl_;
};

} // namespace ilic
