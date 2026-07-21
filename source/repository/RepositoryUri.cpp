#include "RepositoryUri.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <vector>

namespace ilic::repository {
namespace {

std::string lower(std::string value)
{
   std::transform(value.begin(),value.end(),value.begin(),[](unsigned char character) {
      return static_cast<char>(std::tolower(character));
   });
   return value;
}

bool hasScheme(std::string_view value)
{
   if (value.empty() || !std::isalpha(static_cast<unsigned char>(value.front()))) return false;
   for (std::size_t index = 1; index < value.size(); ++index) {
      const unsigned char character = static_cast<unsigned char>(value[index]);
      if (character == ':') return true;
      if (!std::isalnum(character) && character != '+' && character != '-' && character != '.') return false;
   }
   return false;
}

bool isWindowsDrivePath(std::string_view value)
{
   return value.size() >= 2 && std::isalpha(static_cast<unsigned char>(value[0]))
      && value[1] == ':';
}

std::optional<std::string> percentDecode(std::string_view value,std::string *error)
{
   std::string decoded;
   decoded.reserve(value.size());
   auto hex = [](char value) -> int {
      if (value >= '0' && value <= '9') return value - '0';
      if (value >= 'a' && value <= 'f') return value - 'a' + 10;
      if (value >= 'A' && value <= 'F') return value - 'A' + 10;
      return -1;
   };
   for (std::size_t index = 0; index < value.size(); ++index) {
      if (value[index] != '%') {
         decoded.push_back(value[index]);
         continue;
      }
      if (index + 2 >= value.size() || hex(value[index + 1]) < 0 || hex(value[index + 2]) < 0) {
         if (error != nullptr) *error = "invalid percent escape in file URI";
         return std::nullopt;
      }
      const char character = static_cast<char>((hex(value[index + 1]) << 4) | hex(value[index + 2]));
      if (character == '\0') {
         if (error != nullptr) *error = "NUL is not permitted in a file URI";
         return std::nullopt;
      }
      decoded.push_back(character);
      index += 2;
   }
   return decoded;
}

std::string normalizeSlashes(std::string value)
{
   std::replace(value.begin(),value.end(),'\\','/');
   const bool unc = value.rfind("//",0) == 0;
   const bool absolute = !value.empty() && value.front() == '/';
   const bool drive = isWindowsDrivePath(value);
   std::string prefix;
   std::size_t start = 0;
   if (unc) {
      prefix = "//";
      start = 2;
   }
   else if (absolute) {
      prefix = "/";
      start = 1;
   }
   else if (drive) {
      prefix = value.substr(0,2);
      start = 2;
      if (start < value.size() && value[start] == '/') {
         prefix += '/';
         ++start;
      }
   }

   std::vector<std::string> parts;
   while (start <= value.size()) {
      const std::size_t slash = value.find('/',start);
      std::string part = value.substr(start,slash == std::string::npos
         ? std::string::npos : slash - start);
      if (!part.empty() && part != ".") {
         if (part == ".." && !parts.empty() && parts.back() != "..") parts.pop_back();
         else if (part != ".." || prefix.empty()) parts.push_back(std::move(part));
      }
      if (slash == std::string::npos) break;
      start = slash + 1;
   }

   std::string normalized = prefix;
   for (const auto &part : parts) {
      if (!normalized.empty() && normalized.back() != '/') normalized.push_back('/');
      normalized += part;
   }
   if (normalized.empty()) normalized = ".";
   while (normalized.size() > 1 && normalized.back() == '/'
      && !(normalized.size() == 3 && drive)) normalized.pop_back();
   return normalized;
}

std::string normalizeRemote(std::string value)
{
   const std::size_t suffix = value.find_first_of("?#");
   std::string base = value.substr(0,suffix);
   const std::string tail = suffix == std::string::npos ? std::string{} : value.substr(suffix);
   const std::size_t schemeEnd = base.find("://");
   const std::size_t pathStart = base.find('/',schemeEnd + 3);
   if (pathStart == std::string::npos) return base + tail;

   const std::string authority = base.substr(0,pathStart);
   const std::string path = base.substr(pathStart + 1);
   std::vector<std::string> parts;
   std::size_t start = 0;
   while (start <= path.size()) {
      const std::size_t slash = path.find('/',start);
      std::string part = path.substr(start,slash == std::string::npos
         ? std::string::npos : slash - start);
      if (!part.empty() && part != ".") {
         if (part == "..") {
            if (!parts.empty()) parts.pop_back();
         }
         else parts.push_back(std::move(part));
      }
      if (slash == std::string::npos) break;
      start = slash + 1;
   }
   std::string normalized = authority;
   for (const auto &part : parts) normalized += '/' + part;
   return normalized + tail;
}

std::pair<std::string,std::string> splitUriSuffix(const std::string &value)
{
   const std::size_t suffix = value.find_first_of("?#");
   if (suffix == std::string::npos) return {value,{}};
   return {value.substr(0,suffix),value.substr(suffix)};
}

bool componentEqual(const std::filesystem::path &left,const std::filesystem::path &right)
{
#ifdef _WIN32
   return lower(left.string()) == lower(right.string());
#else
   return left == right;
#endif
}

} // namespace

RepositoryUri::RepositoryUri(RepositoryUriKind kind,std::string original,
   std::string normalized,std::string localPath)
   : kind_(kind),original_(std::move(original)),normalized_(std::move(normalized)),
     localPath_(std::move(localPath))
{
}

std::optional<RepositoryUri> RepositoryUri::parse(std::string_view input,std::string *error)
{
   if (input.empty()) {
      if (error != nullptr) *error = "repository URI is empty";
      return std::nullopt;
   }
   const std::string original(input);
   const std::size_t colon = original.find(':');
   const std::string scheme = colon == std::string::npos ? std::string{} : lower(original.substr(0,colon));
   if (scheme == "http" || scheme == "https") {
      if (original.size() < colon + 3 || original.substr(colon,3) != "://") {
         if (error != nullptr) *error = "HTTP repository URI requires //";
         return std::nullopt;
      }
      return RepositoryUri(scheme == "http" ? RepositoryUriKind::Http : RepositoryUriKind::Https,
         original,normalizeRemote(original));
   }
   if (scheme == "file") {
      if (original.size() < colon + 3 || original.substr(colon,3) != "://") {
         if (error != nullptr) *error = "file repository URI requires //";
         return std::nullopt;
      }
      const std::string encoded = original.substr(colon + 3);
      auto decoded = percentDecode(encoded,error);
      if (!decoded) return std::nullopt;
      std::string local;
      if (!decoded->empty() && decoded->front() == '/') local = *decoded;
      else local = "//" + *decoded;
      const std::string normalizedPath = normalizeSlashes(local);
      std::string normalized;
      if (normalizedPath.rfind("//",0) == 0) normalized = "file:" + normalizedPath;
      else normalized = "file://" + normalizedPath;
      return RepositoryUri(RepositoryUriKind::FileUri,original,normalized,normalizedPath);
   }
   if (hasScheme(original) && !isWindowsDrivePath(original)) {
      if (error != nullptr) *error = "unsupported repository URI scheme";
      return std::nullopt;
   }
   const std::string normalized = normalizeSlashes(original);
   return RepositoryUri(RepositoryUriKind::LocalPath,original,normalized,normalized);
}

bool RepositoryUri::isRemote() const
{
   return kind_ == RepositoryUriKind::Http || kind_ == RepositoryUriKind::Https;
}

RepositoryUri RepositoryUri::resolve(std::string_view relative) const
{
   std::string parseError;
   if (hasScheme(relative) || isWindowsDrivePath(relative) || relative.rfind("\\\\",0) == 0) {
      auto absolute = parse(relative,&parseError);
      if (!absolute) throw std::invalid_argument(parseError);
      return *absolute;
   }
   std::string clean(relative);
   std::replace(clean.begin(),clean.end(),'\\','/');
   if (isRemote()) {
      auto [base,suffix] = splitUriSuffix(normalized_);
      if (clean.rfind("//",0) == 0) {
         auto joined = parse(base.substr(0,base.find(':')) + ':' + clean,&parseError);
         if (!joined) throw std::invalid_argument(parseError);
         return *joined;
      }
      const std::size_t pathStart = base.find('/',base.find("://") + 3);
      const std::string authority = pathStart == std::string::npos
         ? base : base.substr(0,pathStart);
      if (!clean.empty() && (clean.front() == '?' || clean.front() == '#')) {
         auto joined = parse(base + clean,&parseError);
         if (!joined) throw std::invalid_argument(parseError);
         return *joined;
      }
      if (!clean.empty() && clean.front() == '/') {
         auto joined = parse(authority + clean,&parseError);
         if (!joined) throw std::invalid_argument(parseError);
         return *joined;
      }
      while (!base.empty() && base.back() == '/') base.pop_back();
      auto joined = parse(base + '/' + clean + suffix,&parseError);
      if (!joined) throw std::invalid_argument(parseError);
      return *joined;
   }
   if (relative.rfind("/",0) == 0) {
      auto absolute = parse(relative,&parseError);
      if (!absolute) throw std::invalid_argument(parseError);
      return *absolute;
   }
   auto joined = parse((toLocalPath() / std::filesystem::path(clean)).lexically_normal().string(),&parseError);
   if (!joined) throw std::invalid_argument(parseError);
   if (kind_ == RepositoryUriKind::FileUri) {
      std::string fileValue = joined->normalized();
      if (fileValue.rfind("//",0) == 0) fileValue = "file:" + fileValue;
      else fileValue = "file://" + fileValue;
      return RepositoryUri(RepositoryUriKind::FileUri,fileValue,fileValue,joined->normalized());
   }
   return *joined;
}

std::filesystem::path RepositoryUri::toLocalPath() const
{
   if (isRemote()) throw std::logic_error("remote repository URI has no local path");
   std::string value = localPath_;
#ifdef _WIN32
   if (kind_ == RepositoryUriKind::FileUri && value.size() >= 3 && value[0] == '/'
      && std::isalpha(static_cast<unsigned char>(value[1])) && value[2] == ':') value.erase(0,1);
   std::replace(value.begin(),value.end(),'/',std::filesystem::path::preferred_separator);
#endif
   return std::filesystem::path(value);
}

RelativePathValidation validateRepositoryRelativePath(std::string_view input)
{
   RelativePathValidation result;
   if (input.empty()) {
      result.error = "repository path is empty";
      return result;
   }
   std::string value(input);
   std::replace(value.begin(),value.end(),'\\','/');
   if (value.front() == '/' || value.rfind("//",0) == 0 || isWindowsDrivePath(value)
      || hasScheme(value)) {
      result.error = "repository path must be relative";
      return result;
   }
   std::size_t start = 0;
   std::vector<std::string> parts;
   while (start <= value.size()) {
      const std::size_t slash = value.find('/',start);
      std::string part = value.substr(start,slash == std::string::npos
         ? std::string::npos : slash - start);
      if (part == "..") {
         result.error = "repository path must not contain parent traversal";
         return result;
      }
      if (!part.empty() && part != ".") parts.push_back(std::move(part));
      if (slash == std::string::npos) break;
      start = slash + 1;
   }
   if (parts.empty()) {
      result.error = "repository path is empty";
      return result;
   }
   for (const auto &part : parts) {
      if (!result.normalized.empty()) result.normalized.push_back('/');
      result.normalized += part;
   }
   result.valid = true;
   return result;
}

bool isPathWithin(const std::filesystem::path &root,const std::filesystem::path &target)
{
   const auto normalizedRoot = std::filesystem::absolute(root).lexically_normal();
   const auto normalizedTarget = std::filesystem::absolute(target).lexically_normal();
   auto rootPart = normalizedRoot.begin();
   auto targetPart = normalizedTarget.begin();
   for (; rootPart != normalizedRoot.end(); ++rootPart,++targetPart) {
      if (targetPart == normalizedTarget.end() || !componentEqual(*rootPart,*targetPart)) return false;
   }
   return true;
}

} // namespace ilic::repository
