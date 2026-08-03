#include "StrictSourceParser.h"

namespace ilic::detail {

SnapshotBundle StrictSourceParser::parse(const SourceBuffer &source,
   DetectedLanguage language,SnapshotParseBuilder ili1,SnapshotParseBuilder ili2)
{
   return (language == DetectedLanguage::Ili1 ? ili1 : ili2)(source,ParseMode::StrictCompiler);
}

} // namespace ilic::detail
