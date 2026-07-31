#pragma once

#include <map>
#include <string>
#include <vector>

#include "../../include/ilic/SourceManager.h"

namespace ilic::detail {

enum class SourceOrigin {
   Session,
   FileSystem,
   Builtin
};

struct CompilationSource {
   SourceBuffer buffer;
   SourceOrigin origin = SourceOrigin::Session;
};

class CompilationSourceStore final {
public:
   explicit CompilationSourceStore(SourceManager &sessionSources) noexcept
      : sessionSources_(sessionSources) {}

   CompilationSourceStore(const CompilationSourceStore &) = delete;
   CompilationSourceStore &operator=(const CompilationSourceStore &) = delete;

   const SourceBuffer *get(const std::string &uri) const noexcept;
   bool contains(const std::string &uri) const noexcept;
   std::vector<std::string> uris() const;

   const SourceBuffer &rememberFileSystemSource(std::string uri,std::string utf8);
   const SourceBuffer &rememberBuiltinSource(std::string uri,std::string utf8);

   SourcePosition position(const std::string &uri,std::size_t byteOffset) const;

   // Compatibility access for the public session-source API. Compiler
   // internals should use the combined source view above.
   const SourceManager &manager() const noexcept { return sessionSources_; }

private:
   const SourceBuffer &remember(std::string uri,std::string utf8,SourceOrigin origin);

   SourceManager &sessionSources_;
   std::map<std::string,CompilationSource> ownedSources_;
};

} // namespace ilic::detail
