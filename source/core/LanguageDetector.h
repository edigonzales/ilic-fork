#pragma once

#include "SnapshotPipeline.h"

namespace ilic::detail {

enum class DetectedLanguage { Unknown, Ili1, Ili2 };

class LanguageDetector final {
public:
   static DetectedLanguage detect(const SourceBuffer &source) noexcept;
};

} // namespace ilic::detail
