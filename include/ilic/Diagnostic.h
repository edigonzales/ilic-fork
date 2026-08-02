#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace ilic {

enum class DiagnosticSeverity { Error, Warning, Information, Hint };

// Diagnostic metadata is deliberately additive.  The compiler's decision is
// still made by the existing producers; these values describe how a result is
// published and consumed.
enum class DiagnosticPhase {
   Unknown,
   Lexical,
   Syntax,
   EditorRecovery,
   ModelDiscovery,
   Resolution,
   Semantic,
   Translation,
   Repository,
   Formatting,
   Request,
   Internal
};

enum class DiagnosticTag {
   Primary,
   Cascaded,
   Recovery,
   Deprecated,
   Unnecessary
};

struct DiagnosticCauseId {
   std::uint64_t value = 0;

   friend bool operator==(DiagnosticCauseId left,DiagnosticCauseId right) noexcept
   {
      return left.value == right.value;
   }
};

enum class DiagnosticPublicationClass {
   Primary,
   Independent,
   Cascaded,
   Recovery
};

struct Position {
   std::size_t line = 0;
   std::size_t character = 0;
   std::size_t byteOffset = 0;
};

struct SourceRange {
   std::string uri;
   Position start;
   Position end;
   bool valid = false;
};

struct RelatedInformation {
   SourceRange range;
   std::string message;
};

struct Diagnostic {
   DiagnosticSeverity severity = DiagnosticSeverity::Error;
   std::string code;
   std::string message;
   SourceRange range;
   std::vector<RelatedInformation> relatedInformation;
   std::vector<std::string> notes;
   bool treatedAsError = false;
   std::string source;
   DiagnosticPhase phase = DiagnosticPhase::Unknown;
   std::vector<DiagnosticTag> tags;
   std::string helpId;
   std::string fingerprint;
};

struct DiagnosticCandidate {
   Diagnostic diagnostic;
   DiagnosticCauseId cause;
   std::vector<DiagnosticCauseId> dependsOn;
   DiagnosticPublicationClass publicationClass = DiagnosticPublicationClass::Primary;
   std::string producer;
};

struct DiagnosticPipelineStats {
   std::uint64_t candidates = 0;
   std::uint64_t published = 0;
   std::uint64_t exactDuplicatesRemoved = 0;
   std::uint64_t cascadesSuppressed = 0;
   std::uint64_t invalidRangesNormalized = 0;
   std::uint64_t unknownCodes = 0;
};

struct PublishedDiagnostics {
   std::vector<Diagnostic> values;
   DiagnosticPipelineStats stats;
};

enum class LogLevel { Trace, Debug, Information, Warning, Error };

struct LogEvent {
   LogLevel level = LogLevel::Information;
   std::string category;
   std::string message;
   std::map<std::string, std::string> context;
};

using LogSink = std::function<void(const LogEvent &)>;
using DiagnosticSink = std::function<void(const Diagnostic &)>;

} // namespace ilic
