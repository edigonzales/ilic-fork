#pragma once

#include "Diagnostic.h"
#include "SourceManager.h"

#include <cstddef>
#include <string_view>

namespace ilic {

class DiagnosticRangeResolver final {
public:
   explicit DiagnosticRangeResolver(const SourceManager &sources) noexcept
      : sources_(sources) {}

   SourceRange byteRange(std::string_view uri,std::size_t startByte,
      std::size_t endByte) const;
   SourceRange insertionPoint(std::string_view uri,std::size_t byteOffset) const;
   SourceRange normalize(SourceRange range) const;

private:
   const SourceManager &sources_;
};

} // namespace ilic
