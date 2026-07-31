#include "../../include/ilic/SourceManager.h"

#include "SourceRangeMapper.h"

namespace ilic {

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
   const SourceBuffer *source = get(uri);
   return source == nullptr ? SourcePosition{}
      : detail::SourceRangeMapper(*source).positionFromByteOffset(byteOffset);
}

} // namespace ilic
