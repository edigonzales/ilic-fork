#include "../../include/ilic/SourceManager.h"

#include <algorithm>

namespace ilic {
namespace {
thread_local SourceManager *currentSourceManager = nullptr;

std::size_t utf16Width(const unsigned char *bytes, std::size_t remaining, std::size_t &consumed)
{
   consumed = 1;
   if (remaining == 0 || bytes[0] < 0x80) return 1;
   std::uint32_t codepoint = 0xfffd;
   if ((bytes[0] & 0xe0) == 0xc0 && remaining >= 2) {
      codepoint = ((bytes[0] & 0x1f) << 6) | (bytes[1] & 0x3f);
      consumed = 2;
   }
   else if ((bytes[0] & 0xf0) == 0xe0 && remaining >= 3) {
      codepoint = ((bytes[0] & 0x0f) << 12) | ((bytes[1] & 0x3f) << 6) | (bytes[2] & 0x3f);
      consumed = 3;
   }
   else if ((bytes[0] & 0xf8) == 0xf0 && remaining >= 4) {
      codepoint = ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3f) << 12)
         | ((bytes[2] & 0x3f) << 6) | (bytes[3] & 0x3f);
      consumed = 4;
   }
   return codepoint > 0xffff ? 2 : 1;
}
}

void SourceManager::put(std::string uri, std::string utf8, std::uint64_t version)
{
   SourceBuffer buffer{std::move(uri), std::move(utf8), version};
   sources_[buffer.uri] = std::move(buffer);
}

bool SourceManager::remove(const std::string &uri) { return sources_.erase(uri) != 0; }
bool SourceManager::contains(const std::string &uri) const { return sources_.find(uri) != sources_.end(); }

const SourceBuffer *SourceManager::get(const std::string &uri) const
{
   auto found = sources_.find(uri);
   return found == sources_.end() ? nullptr : &found->second;
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
   SourcePosition result;
   const SourceBuffer *source = get(uri);
   if (source == nullptr) return result;
   byteOffset = std::min(byteOffset, source->text.size());
   result.offset = byteOffset;
   std::size_t lineStart = 0;
   for (std::size_t i = 0; i < byteOffset; ++i) {
      if (source->text[i] == '\n') {
         ++result.line;
         lineStart = i + 1;
      }
   }
   for (std::size_t i = lineStart; i < byteOffset;) {
      std::size_t consumed = 1;
      result.utf16Column += utf16Width(
         reinterpret_cast<const unsigned char *>(source->text.data() + i), byteOffset - i, consumed);
      i += consumed;
   }
   return result;
}

SourceManager *activeSourceManager() { return currentSourceManager; }
void setActiveSourceManager(SourceManager *manager) { currentSourceManager = manager; }

ActiveSourceManagerScope::ActiveSourceManagerScope(SourceManager *manager)
   : previous_(activeSourceManager()) { setActiveSourceManager(manager); }
ActiveSourceManagerScope::~ActiveSourceManagerScope() { setActiveSourceManager(previous_); }

} // namespace ilic
