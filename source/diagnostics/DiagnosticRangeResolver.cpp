#include "ilic/DiagnosticRangeResolver.h"

#include <algorithm>

namespace ilic {

SourceRange DiagnosticRangeResolver::byteRange(std::string_view uri,
   std::size_t startByte,std::size_t endByte) const
{
   SourceRange result;
   const std::string key(uri);
   const auto *source = sources_.get(key);
   if (source == nullptr) return result;
   startByte = std::min(startByte,source->text.size());
   endByte = std::min(std::max(endByte,startByte),source->text.size());
   const auto start = sources_.position(key,startByte);
   const auto end = sources_.position(key,endByte);
   result.valid = true;
   result.uri = key;
   result.start = {start.line,start.utf16Column,start.offset};
   result.end = {end.line,end.utf16Column,end.offset};
   return result;
}

SourceRange DiagnosticRangeResolver::insertionPoint(std::string_view uri,
   std::size_t byteOffset) const
{
   return byteRange(uri,byteOffset,byteOffset);
}

SourceRange DiagnosticRangeResolver::normalize(SourceRange range) const
{
   if (!range.valid) return range;
   if (range.uri.empty()) {
      range.valid = false;
      return range;
   }
   if (range.start.byteOffset > range.end.byteOffset) {
      range.end = range.start;
   }
   if (const auto *source = sources_.get(range.uri)) {
      const std::size_t start = std::min(range.start.byteOffset,source->text.size());
      const std::size_t end = std::min(std::max(range.end.byteOffset,start),source->text.size());
      const auto startPosition = sources_.position(range.uri,start);
      const auto endPosition = sources_.position(range.uri,end);
      range.start = {startPosition.line,startPosition.utf16Column,startPosition.offset};
      range.end = {endPosition.line,endPosition.utf16Column,endPosition.offset};
   }
   return range;
}

} // namespace ilic
