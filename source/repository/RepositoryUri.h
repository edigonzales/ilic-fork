#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

namespace ilic::repository {

enum class RepositoryUriKind { LocalPath, FileUri, Http, Https };

struct RelativePathValidation {
   bool valid = false;
   std::string normalized;
   std::string error;
};

class RepositoryUri {
public:
   static std::optional<RepositoryUri> parse(
      std::string_view value,std::string *error = nullptr);

   RepositoryUriKind kind() const { return kind_; }
   bool isRemote() const;
   bool isLocal() const { return !isRemote(); }
   const std::string &original() const { return original_; }
   const std::string &normalized() const { return normalized_; }
   std::string cacheKey() const { return normalized_; }

   RepositoryUri resolve(std::string_view relative) const;
   std::filesystem::path toLocalPath() const;

private:
   RepositoryUri(RepositoryUriKind kind,std::string original,
      std::string normalized,std::string localPath = {});

   RepositoryUriKind kind_;
   std::string original_;
   std::string normalized_;
   std::string localPath_;
};

RelativePathValidation validateRepositoryRelativePath(std::string_view value);

bool isPathWithin(const std::filesystem::path &root,
   const std::filesystem::path &target);

} // namespace ilic::repository
