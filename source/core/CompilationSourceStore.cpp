#include "CompilationSourceStore.h"

#include <algorithm>
#include <stdexcept>

namespace {

ilic::SourcePosition sourcePosition(const ilic::SourceBuffer &source,std::size_t byteOffset)
{
   const std::size_t offset = std::min(byteOffset,source.text.size());
   ilic::SourcePosition result;
   result.offset = offset;
   for (std::size_t index = 0; index < offset; ++index) {
      const unsigned char value = static_cast<unsigned char>(source.text[index]);
      if (value == '\n') {
         ++result.line;
         result.utf16Column = 0;
      } else if ((value & 0xc0) != 0x80) {
         if (value < 0x80 || (value & 0xe0) == 0xc0 ||
             (value & 0xf0) == 0xe0) ++result.utf16Column;
         else if ((value & 0xf8) == 0xf0) result.utf16Column += 2;
      }
   }
   return result;
}

} // namespace

namespace ilic::detail {

const ilic::SourceBuffer *CompilationSourceStore::get(const std::string &uri) const noexcept
{
   if (const auto *session = sessionSources_.get(uri)) return session;
   const auto found = ownedSources_.find(uri);
   return found == ownedSources_.end() ? nullptr : &found->second.buffer;
}

bool CompilationSourceStore::contains(const std::string &uri) const noexcept
{
   return get(uri) != nullptr;
}

std::vector<std::string> CompilationSourceStore::uris() const
{
   std::vector<std::string> result = sessionSources_.uris();
   for (const auto &entry : ownedSources_) {
      if (!sessionSources_.contains(entry.first)) result.push_back(entry.first);
   }
   return result;
}

const ilic::SourceBuffer &CompilationSourceStore::remember(
   std::string uri,std::string utf8,SourceOrigin origin)
{
   if (const auto *session = sessionSources_.get(uri)) return *session;
   const auto found = ownedSources_.find(uri);
   if (found != ownedSources_.end()) {
      if (found->second.buffer.text != utf8)
         throw std::logic_error("source URI already has different content: " + uri);
      return found->second.buffer;
   }
   CompilationSource source;
   source.buffer.uri = std::move(uri);
   source.buffer.text = std::move(utf8);
   source.origin = origin;
   auto inserted = ownedSources_.emplace(source.buffer.uri,std::move(source));
   return inserted.first->second.buffer;
}

const ilic::SourceBuffer &CompilationSourceStore::rememberFileSystemSource(
   std::string uri,std::string utf8)
{
   return remember(std::move(uri),std::move(utf8),SourceOrigin::FileSystem);
}

const ilic::SourceBuffer &CompilationSourceStore::rememberRepositorySource(
   std::string uri,std::string utf8)
{
   return remember(std::move(uri),std::move(utf8),SourceOrigin::Repository);
}

const ilic::SourceBuffer &CompilationSourceStore::rememberBuiltinSource(
   std::string uri,std::string utf8)
{
   return remember(std::move(uri),std::move(utf8),SourceOrigin::Builtin);
}

ilic::SourcePosition CompilationSourceStore::position(
   const std::string &uri,std::size_t byteOffset) const
{
   if (sessionSources_.contains(uri)) return sessionSources_.position(uri,byteOffset);
   const auto found = ownedSources_.find(uri);
   if (found == ownedSources_.end()) return {};
   return sourcePosition(found->second.buffer,byteOffset);
}

} // namespace ilic::detail
