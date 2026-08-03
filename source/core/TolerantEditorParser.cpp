#include "TolerantEditorParser.h"

namespace ilic::detail {

SnapshotBundle TolerantEditorParser::parse(const SourceBuffer &source,
   DetectedLanguage language,SnapshotParseBuilder ili1,SnapshotParseBuilder ili2)
{
   return (language == DetectedLanguage::Ili1 ? ili1 : ili2)(source,ParseMode::TolerantEditor);
}

} // namespace ilic::detail
