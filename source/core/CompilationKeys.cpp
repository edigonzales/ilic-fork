#include "CompilationKeys.h"

#include "incremental/IncrementalFingerprint.h"

#include <iomanip>
#include <sstream>

namespace ilic::detail {
namespace {

std::string contentHash(const std::string &bytes)
{
   std::uint64_t hash = 1469598103934665603ULL;
   for (const unsigned char byte : bytes) {
      hash ^= byte;
      hash *= 1099511628211ULL;
   }
   std::ostringstream result;
   result << std::hex << std::setw(16) << std::setfill('0') << hash;
   return result.str();
}

} // namespace

SourceIdentity sourceIdentity(const SourceManager &sources,const std::string &uri)
{
   if (const auto *identity = sources.identity(uri)) return *identity;
   const auto *source = sources.get(uri);
   if (source == nullptr) return {uri,0,0,0,"",0};
   return {uri,source->version,0,0,contentHash(source->text),source->text.size()};
}

SourceIdentity sourceIdentity(const SourceManager &sources,const SourceBuffer &source)
{
   if (const auto *identity = sources.identity(source.uri)) return *identity;
   return {source.uri,source.version,0,0,contentHash(source.text),source.text.size()};
}

std::string requestKey(const CompilationRequest &request)
{
   std::ostringstream key;
   key << (request.options.autoSearch ? '1' : '0') << ':'
      << (request.options.warningsAsErrors ? '1' : '0') << ':';
   for (const auto &root : request.roots) key << root.size() << ':' << root << ';';
   key << '|';
   for (const auto &directory : request.options.modelDirectories)
      key << directory.size() << ':' << directory << ';';
   key << '|';
   for (const auto &attribute : request.externalMetaAttributes)
      key << attribute.element.size() << ':' << attribute.element << ':'
         << attribute.name.size() << ':' << attribute.name << ':'
         << attribute.value.size() << ':' << attribute.value << ';';
   return key.str();
}

std::string rootKey(const std::string &base,const std::vector<std::string> &uris,
   const SourceManager &sources)
{
   std::ostringstream key;
   key << base << "|fingerprint=" << currentCompilerFingerprint().canonicalString() << ';';
   for (const auto &uri : uris) {
      const auto identity = sourceIdentity(sources,uri);
      key << uri.size() << ':' << uri << ':' << identity.contentRevision << ':'
         << identity.contentHash << ':' << identity.byteLength << ';';
   }
   return key.str();
}

} // namespace ilic::detail
