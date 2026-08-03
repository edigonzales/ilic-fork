#pragma once

#include "SnapshotPipeline.h"

namespace ilic::detail {

class ParsedSourceArtifactFactory final {
public:
   static SnapshotBundle build(
      const SourceManager &sources,
      const std::string &uri,
      ParseMode mode
   );

   static SnapshotBundle build(
      const SourceBuffer &source,
      ParseMode mode
   );
};

} // namespace ilic::detail
