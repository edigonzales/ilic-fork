#pragma once

#include "StrictSourceParser.h"

namespace ilic::detail {

class TolerantEditorParser final {
public:
   static SnapshotBundle parse(const SourceBuffer &source,DetectedLanguage language,
      SnapshotParseBuilder ili1,SnapshotParseBuilder ili2);
};

} // namespace ilic::detail
