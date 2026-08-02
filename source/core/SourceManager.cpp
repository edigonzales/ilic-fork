#include "../../include/ilic/SourceManager.h"

#include "SourceRangeMapper.h"

#include <iomanip>
#include <limits>
#include <sstream>

namespace ilic {

namespace {

std::string contentHash(const std::string &bytes)
{
   // FNV-1a is deliberately used only as a fast cache hint. The byte length
   // and the bytes themselves are compared before an existing entry is used.
   std::uint64_t hash = 1469598103934665603ULL;
   for (const unsigned char byte : bytes) {
      hash ^= byte;
      hash *= 1099511628211ULL;
   }
   std::ostringstream result;
   result << std::hex << std::setw(16) << std::setfill('0') << hash;
   return result.str();
}

bool invalidUri(const std::string &uri)
{
   return uri.empty() || uri.find('\0') != std::string::npos;
}

} // namespace

SourceUpdateResult SourceManager::update(std::string uri,std::string utf8,
   std::uint64_t version)
{
   SourceUpdateResult result;
   if (invalidUri(uri)) return result;

   const std::string hash = contentHash(utf8);
   const auto found = sources_.find(uri);
   if (found != sources_.end()) {
      result.previous = identities_.at(uri);
      if (version < result.previous.documentVersion && version != 0) return result;
      const bool sameBytes = found->second.text.size() == utf8.size()
         && found->second.text == utf8;
      result.current = result.previous;
      // Version zero means that the caller did not provide a new visible
      // document version. It must never downgrade an already known version.
      const std::uint64_t visibleVersion = version == 0
         ? result.previous.documentVersion : version;
      result.current.documentVersion = visibleVersion;
      if (sameBytes && version == result.previous.documentVersion) {
         result.kind = SourceUpdateKind::Unchanged;
         result.current = result.previous;
         result.accepted = true;
         return result;
      }
      if (sameBytes) {
         if (version == 0 || version < result.previous.documentVersion) {
            result.kind = SourceUpdateKind::Unchanged;
            result.current = result.previous;
            result.accepted = true;
            return result;
         }
         found->second.version = visibleVersion;
         result.current.documentVersion = version;
         result.kind = SourceUpdateKind::VersionOnly;
         result.accepted = true;
         identities_[uri] = result.current;
         return result;
      }
      result.kind = SourceUpdateKind::ContentChanged;
      result.current.contentRevision = ++nextContentRevision_;
      result.current.sourceGeneration = ++generation_;
      result.current.documentVersion = visibleVersion;
      result.current.contentHash = hash;
      result.current.byteLength = utf8.size();
      result.accepted = true;
      result.parserInvalidated = true;
      result.analysesInvalidated = true;
   }
   else {
      const auto removed = removedIdentities_.find(uri);
      result.kind = removed == removedIdentities_.end()
         ? SourceUpdateKind::Added : SourceUpdateKind::Reintroduced;
      if (removed != removedIdentities_.end()) result.previous = removed->second;
      result.current.uri = uri;
      result.current.documentVersion = version;
      result.current.contentRevision = ++nextContentRevision_;
      result.current.sourceGeneration = ++generation_;
      result.current.contentHash = hash;
      result.current.byteLength = utf8.size();
      result.accepted = true;
      result.parserInvalidated = false;
      result.analysesInvalidated = true;
   }

   SourceBuffer buffer{uri,std::move(utf8),version};
   sources_[uri] = std::move(buffer);
   identities_[uri] = result.current;
   removedIdentities_.erase(uri);
   return result;
}

void SourceManager::put(std::string uri, std::string utf8, std::uint64_t version)
{
   (void)update(std::move(uri),std::move(utf8),version);
}

bool SourceManager::remove(const std::string &uri)
{
   const auto found = sources_.find(uri);
   if (found == sources_.end()) return false;
   const auto identityFound = identities_.find(uri);
   if (identityFound != identities_.end()) {
      SourceIdentity identity = identityFound->second;
      identity.sourceGeneration = ++generation_;
      removedIdentities_[uri] = std::move(identity);
   }
   sources_.erase(found);
   identities_.erase(uri);
   return true;
}
bool SourceManager::contains(const std::string &uri) const { return sources_.find(uri) != sources_.end(); }

const SourceBuffer *SourceManager::get(const std::string &uri) const
{
   auto found = sources_.find(uri);
   return found == sources_.end() ? nullptr : &found->second;
}

const SourceIdentity *SourceManager::identity(const std::string &uri) const noexcept
{
   const auto found = identities_.find(uri);
   return found == identities_.end() ? nullptr : &found->second;
}

std::vector<std::string> SourceManager::uris() const
{
   std::vector<std::string> result;
   result.reserve(sources_.size());
   for (const auto &entry : sources_) result.push_back(entry.first);
   return result;
}

SourcePosition SourceManager::position(const std::string &uri, std::size_t byteOffset) const
{
   const SourceBuffer *source = get(uri);
   return source == nullptr ? SourcePosition{}
      : detail::SourceRangeMapper(*source).positionFromByteOffset(byteOffset);
}

} // namespace ilic
