#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ilic {

struct SourcePosition {
   std::size_t offset = 0;
   std::size_t line = 0;
   std::size_t utf16Column = 0;
};

struct SourceBuffer {
   std::string uri;
   std::string text;
   std::uint64_t version = 0;
};

enum class SourceUpdateKind {
   Added,
   Unchanged,
   VersionOnly,
   ContentChanged,
   Reintroduced,
   Removed,
   Rejected
};

struct SourceIdentity {
   std::string uri;
   std::uint64_t documentVersion = 0;
   std::uint64_t contentRevision = 0;
   std::uint64_t sourceGeneration = 0;
   std::string contentHash;
   std::size_t byteLength = 0;
};

struct SourceUpdateResult {
   SourceUpdateKind kind = SourceUpdateKind::Rejected;
   SourceIdentity previous;
   SourceIdentity current;
   bool accepted = false;
   bool parserInvalidated = false;
   bool analysesInvalidated = false;
};

class SourceManager {
public:
   SourceUpdateResult update(std::string uri,std::string utf8,
      std::uint64_t version = 0);
   void put(std::string uri, std::string utf8, std::uint64_t version = 0);
   bool remove(const std::string &uri);
   bool contains(const std::string &uri) const;
   const SourceBuffer *get(const std::string &uri) const;
   const SourceIdentity *identity(const std::string &uri) const noexcept;
   std::vector<std::string> uris() const;
   SourcePosition position(const std::string &uri, std::size_t byteOffset) const;
   std::uint64_t generation() const noexcept { return generation_; }

private:
   std::map<std::string, SourceBuffer> sources_;
   std::map<std::string, SourceIdentity> identities_;
   std::map<std::string, SourceIdentity> removedIdentities_;
   std::uint64_t nextContentRevision_ = 0;
   std::uint64_t generation_ = 0;
};

} // namespace ilic
