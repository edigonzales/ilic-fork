#pragma once

#include "../../include/ilic/SourceManager.h"
#include "../../include/ilic/Diagnostic.h"

#include "Token.h"
#include "ParserRuleContext.h"

#include <cstddef>
#include <string>
#include <vector>

namespace ilic::detail {

class SourceRangeMapper final {
public:
   explicit SourceRangeMapper(const SourceBuffer &source);

   SourcePosition positionFromByteOffset(std::size_t offset) const;
   std::size_t byteOffsetFromCodepointOffset(std::size_t offset) const;
   SourceRange range(const std::string &uri,std::size_t begin,std::size_t end) const;
   SourceRange token(const antlr4::Token *token) const;
   SourceRange context(const antlr4::ParserRuleContext *context) const;
   SourceRange eof() const;
   std::string normalizedUtf8() const;

   const SourceBuffer &source() const noexcept { return source_; }

private:
   SourceRange codepointRange(std::size_t begin,std::size_t end) const;

   const SourceBuffer &source_;
   std::vector<SourcePosition> positions_;
   std::vector<std::size_t> codepointToByte_;
};

} // namespace ilic::detail
