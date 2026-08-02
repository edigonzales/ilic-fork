#pragma once

#include "Diagnostic.h"

#include <string>
#include <string_view>

namespace ilic {

class DiagnosticBuilder final {
public:
   explicit DiagnosticBuilder(std::string code);

   DiagnosticBuilder &severity(DiagnosticSeverity value);
   DiagnosticBuilder &source(std::string value);
   DiagnosticBuilder &message(std::string value);
   DiagnosticBuilder &primaryRange(SourceRange value);
   DiagnosticBuilder &related(SourceRange range,std::string message);
   DiagnosticBuilder &note(std::string value);
   DiagnosticBuilder &treatedAsError(bool value);
   DiagnosticBuilder &phase(DiagnosticPhase value);
   DiagnosticBuilder &tag(DiagnosticTag value);
   DiagnosticBuilder &cause(DiagnosticCauseId value);
   DiagnosticBuilder &dependsOn(DiagnosticCauseId value);
   DiagnosticBuilder &publicationClass(DiagnosticPublicationClass value);
   DiagnosticBuilder &producer(std::string value);
   DiagnosticBuilder &helpId(std::string value);

   DiagnosticCandidate build();

private:
   DiagnosticCandidate candidate_;
   bool explicitSeverity_ = false;
};

} // namespace ilic
