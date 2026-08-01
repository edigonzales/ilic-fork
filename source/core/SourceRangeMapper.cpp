#include "SourceRangeMapper.h"

#include <algorithm>
#include <cstdint>

namespace ilic::detail {
namespace {

struct DecodedCodepoint {
   std::size_t width = 1;
   std::size_t utf16Width = 1;
   std::uint32_t value = 0xfffd;
};

bool continuation(unsigned char value)
{
   return (value & 0xc0u) == 0x80u;
}

DecodedCodepoint decode(const std::string &text,std::size_t offset)
{
   const auto *bytes = reinterpret_cast<const unsigned char *>(text.data());
   const std::size_t remaining = text.size() - offset;
   if (remaining == 0) return {};

   const unsigned char lead = bytes[offset];
   if (lead < 0x80u) return {1,1,lead};

   if ((lead & 0xe0u) == 0xc0u && remaining >= 2 && continuation(bytes[offset + 1])) {
      const std::uint32_t value = ((lead & 0x1fu) << 6) | (bytes[offset + 1] & 0x3fu);
      if (value >= 0x80u) return {2,1,value};
   }
   if ((lead & 0xf0u) == 0xe0u && remaining >= 3
       && continuation(bytes[offset + 1]) && continuation(bytes[offset + 2])) {
      const std::uint32_t value = ((lead & 0x0fu) << 12)
         | ((bytes[offset + 1] & 0x3fu) << 6) | (bytes[offset + 2] & 0x3fu);
      if (value >= 0x800u && !(value >= 0xd800u && value <= 0xdfffu))
         return {3,1,value};
   }
   if ((lead & 0xf8u) == 0xf0u && remaining >= 4
       && continuation(bytes[offset + 1]) && continuation(bytes[offset + 2])
       && continuation(bytes[offset + 3])) {
      const std::uint32_t value = ((lead & 0x07u) << 18)
         | ((bytes[offset + 1] & 0x3fu) << 12)
         | ((bytes[offset + 2] & 0x3fu) << 6) | (bytes[offset + 3] & 0x3fu);
      if (value >= 0x10000u && value <= 0x10ffffu) return {4,2,value};
   }
   return {};
}

} // namespace

SourceRangeMapper::SourceRangeMapper(const SourceBuffer &source) : source_(source)
{
   positions_.resize(source_.text.size() + 1);
   codepointToByte_.push_back(0);
   SourcePosition current;
   std::size_t offset = 0;
   while (offset < source_.text.size()) {
      current.offset = offset;
      positions_[offset] = current;
      const DecodedCodepoint value = decode(source_.text,offset);
      const std::size_t width = std::min(value.width,source_.text.size() - offset);
      for (std::size_t index = 1; index < width; ++index) {
         SourcePosition interior = current;
         interior.offset = offset + index;
         positions_[offset + index] = interior;
      }
      offset += width;
      if (value.value == '\n') {
         ++current.line;
         current.utf16Column = 0;
      }
      else current.utf16Column += value.utf16Width;
      current.offset = offset;
      positions_[offset] = current;
      codepointToByte_.push_back(offset);
   }
   if (positions_.empty()) positions_.push_back(current);
   positions_.back().offset = source_.text.size();
}

SourcePosition SourceRangeMapper::positionFromByteOffset(std::size_t offset) const
{
   if (positions_.empty()) return {};
   offset = std::min(offset,source_.text.size());
   return positions_[offset];
}

std::size_t SourceRangeMapper::byteOffsetFromCodepointOffset(std::size_t offset) const
{
   if (codepointToByte_.empty()) return 0;
   offset = std::min(offset,codepointToByte_.size() - 1);
   return codepointToByte_[offset];
}

SourceRange SourceRangeMapper::range(const std::string &uri,std::size_t begin,std::size_t end) const
{
   begin = std::min(begin,source_.text.size());
   end = std::min(std::max(end,begin),source_.text.size());
   SourceRange value;
   value.uri = uri;
   const SourcePosition start = positionFromByteOffset(begin);
   const SourcePosition finish = positionFromByteOffset(end);
   value.start = {start.line,start.utf16Column,start.offset};
   value.end = {finish.line,finish.utf16Column,finish.offset};
   value.valid = true;
   return value;
}

SourceRange SourceRangeMapper::codepointRange(std::size_t begin,std::size_t end) const
{
   return range(source_.uri,byteOffsetFromCodepointOffset(begin),
      byteOffsetFromCodepointOffset(end));
}

SourceRange SourceRangeMapper::token(const antlr4::Token *token) const
{
   if (token == nullptr) return {};
   if (token->getType() == antlr4::Token::EOF) return eof();
   const std::size_t start = token->getStartIndex();
   const std::size_t stop = token->getStopIndex();
   if (start == INVALID_INDEX) return {};
   const std::size_t end = stop == INVALID_INDEX || stop < start ? start : stop + 1;
   return codepointRange(start,end);
}

SourceRange SourceRangeMapper::context(const antlr4::ParserRuleContext *context) const
{
   if (context == nullptr) return {};
   const antlr4::Token *startToken = const_cast<antlr4::ParserRuleContext *>(context)->getStart();
   if (startToken == nullptr || startToken->getType() == antlr4::Token::EOF)
      return eof();
   const std::size_t start = startToken->getStartIndex();
   if (start == INVALID_INDEX) return eof();
   const antlr4::Token *stopToken = const_cast<antlr4::ParserRuleContext *>(context)->getStop();
   if (stopToken == nullptr || stopToken->getType() == antlr4::Token::EOF)
      return codepointRange(start,codepointToByte_.size() - 1);
   const std::size_t stop = stopToken->getStopIndex();
   return codepointRange(start,stop == INVALID_INDEX || stop < start ? start : stop + 1);
}

SourceRange SourceRangeMapper::eof() const
{
   return range(source_.uri,source_.text.size(),source_.text.size());
}

std::string SourceRangeMapper::normalizedUtf8() const
{
   std::string normalized;
   normalized.reserve(source_.text.size() + 1);
   std::size_t offset = 0;
   while (offset < source_.text.size()) {
      const DecodedCodepoint value = decode(source_.text,offset);
      const unsigned char lead = static_cast<unsigned char>(source_.text[offset]);
      const bool invalid = value.width == 1 && lead >= 0x80u;
      if (invalid) {
         normalized.append("\xEF\xBF\xBD");
         ++offset;
      }
      else {
         normalized.append(source_.text,offset,value.width);
         offset += value.width;
      }
   }
   // INTERLIS line comments are traditionally terminated by a line break.
   // Treat an unterminated final comment as a comment as well, without
   // changing any source ranges (the synthetic byte is never mapped back).
   if (!normalized.empty() && normalized.back() != '\n') normalized.push_back('\n');
   return normalized;
}

} // namespace ilic::detail
