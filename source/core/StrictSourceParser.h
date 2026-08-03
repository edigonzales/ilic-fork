#pragma once

#include "LanguageDetector.h"

namespace ilic::detail {

using SnapshotParseBuilder = SnapshotBundle (*)(const SourceBuffer &, ParseMode);

class StrictSourceParser final {
public:
   static SnapshotBundle parse(const SourceBuffer &source,DetectedLanguage language,
      SnapshotParseBuilder ili1,SnapshotParseBuilder ili2);
};

} // namespace ilic::detail
