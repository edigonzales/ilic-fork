#pragma once

#include "Diagnostic.h"

#include <string_view>
#include <vector>

namespace ilic {

struct DiagnosticDescriptor {
   std::string_view code;
   DiagnosticSeverity defaultSeverity = DiagnosticSeverity::Error;
   DiagnosticPhase phase = DiagnosticPhase::Unknown;
   std::string_view title;
   std::string_view documentationId;
   bool mayBeTreatedAsError = false;
   bool requiresPrimaryRange = false;
};

class DiagnosticCatalog final {
public:
   static const DiagnosticDescriptor *find(std::string_view code) noexcept;
   static const DiagnosticDescriptor &require(std::string_view code);
   static std::vector<DiagnosticDescriptor> all();
   static bool isRegistered(std::string_view code) noexcept;
};

} // namespace ilic
