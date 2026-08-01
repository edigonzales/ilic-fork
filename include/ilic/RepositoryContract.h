#pragma once

#include "Diagnostic.h"

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace ilic {
struct ModelMetadata;
}

namespace ilic::repository::ports {

enum class RepositoryResourceKind {
   ModelIndex,
   SiteIndex,
   Model
};

struct RepositoryTransportRequest {
   std::string uri;
   RepositoryResourceKind kind = RepositoryResourceKind::Model;
   std::size_t maxBytes = 0;
   std::size_t maxRedirects = 0;
};

struct RepositoryTransportResponse {
   bool success = false;
   long statusCode = 0;
   std::string body;
   std::string finalUri;
   std::string error;
   bool notFound = false;
   bool retryable = false;
};

class RepositoryTransportPort {
public:
   virtual ~RepositoryTransportPort() = default;
   virtual RepositoryTransportResponse get(const RepositoryTransportRequest &request) = 0;
};

class RepositoryClock {
public:
   virtual ~RepositoryClock() = default;
   virtual std::chrono::system_clock::time_point now() const = 0;
};

class SystemRepositoryClock final : public RepositoryClock {
public:
   std::chrono::system_clock::time_point now() const override;
};

class ManualRepositoryClock final : public RepositoryClock {
public:
   explicit ManualRepositoryClock(std::chrono::system_clock::time_point initial);
   std::chrono::system_clock::time_point now() const override;
   void advance(std::chrono::milliseconds duration);
   void set(std::chrono::system_clock::time_point value);

private:
   std::chrono::system_clock::time_point value_;
};

struct RepositoryCacheEntry {
   std::string value;
   std::chrono::system_clock::time_point storedAt;
};

struct RepositoryCacheLookup {
   bool hit = false;
   RepositoryCacheEntry entry;
   std::string error;
};

struct RepositoryCacheStoreResult {
   bool success = false;
   std::string error;
   std::filesystem::path localPath;
};

class RepositoryCachePort {
public:
   virtual ~RepositoryCachePort() = default;
   virtual RepositoryCacheLookup get(std::string_view key) = 0;
   virtual RepositoryCacheStoreResult put(std::string_view key,std::string_view value,
      std::chrono::system_clock::time_point storedAt) = 0;
   virtual void remove(std::string_view key) = 0;
   virtual void clear() = 0;
};

class RepositoryMetadataDecoder {
public:
   virtual ~RepositoryMetadataDecoder() = default;
   virtual bool decodeModelIndex(std::string_view bytes,std::string_view repository,
      std::vector<ModelMetadata> &models,std::vector<Diagnostic> &diagnostics) = 0;
   virtual bool decodeSite(std::string_view bytes,std::string_view repository,
      std::vector<std::string> &parents,std::vector<std::string> &subsidiaries,
      std::vector<Diagnostic> &diagnostics) = 0;
};

class RepositoryChecksum {
public:
   virtual ~RepositoryChecksum() = default;
   virtual std::string md5(std::string_view bytes) const = 0;
};

} // namespace ilic::repository::ports
