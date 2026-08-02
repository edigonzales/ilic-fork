#include "ilic/DiagnosticPipeline.h"

#include "ilic/DiagnosticCatalog.h"

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>

namespace ilic {
namespace {

int severityRank(DiagnosticSeverity value) noexcept
{
   switch (value) {
      case DiagnosticSeverity::Error: return 0;
      case DiagnosticSeverity::Warning: return 1;
      case DiagnosticSeverity::Information: return 2;
      case DiagnosticSeverity::Hint: return 3;
   }
   return 4;
}

int phaseRank(DiagnosticPhase value) noexcept
{
   return static_cast<int>(value);
}

std::string rangeKey(const SourceRange &range)
{
   if (!range.valid) return "invalid";
   return range.uri + ":" + std::to_string(range.start.byteOffset) + ":"
      + std::to_string(range.end.byteOffset);
}

std::string normalizedMessage(std::string value)
{
   for (char &character : value)
      if (character == '\r' || character == '\n' || std::isspace(static_cast<unsigned char>(character)))
         character = ' ';
   std::string result;
   bool previousSpace = false;
   for (const char character : value) {
      if (character == ' ') {
         if (previousSpace) continue;
         previousSpace = true;
      } else previousSpace = false;
      result.push_back(character);
   }
   while (!result.empty() && result.back() == ' ') result.pop_back();
   return result;
}

std::string identity(const Diagnostic &diagnostic)
{
   std::ostringstream result;
   result << diagnostic.code << '\x1f' << static_cast<int>(diagnostic.severity) << '\x1f'
      << diagnostic.source << '\x1f' << rangeKey(diagnostic.range) << '\x1f'
      << normalizedMessage(diagnostic.message) << '\x1f' << diagnostic.treatedAsError;
   for (const auto &related : diagnostic.relatedInformation)
      result << '\x1e' << rangeKey(related.range) << '\x1f' << related.message;
   for (const auto &note : diagnostic.notes) result << '\x1d' << note;
   return result.str();
}

bool sameCause(const DiagnosticCauseId &left,const DiagnosticCauseId &right) noexcept
{
   return left.value != 0 && left == right;
}

} // namespace

void DiagnosticPipeline::normalize(Diagnostic &diagnostic,DiagnosticPipelineStats &stats)
{
   if (diagnostic.code.empty()) {
      diagnostic.code = "ILIC-COMPILER-INTERNAL";
      ++stats.unknownCodes;
   }
   else if (!DiagnosticCatalog::isRegistered(diagnostic.code)) ++stats.unknownCodes;
   if (diagnostic.source.empty()) diagnostic.source = "compiler";
   diagnostic.message = normalizedMessage(std::move(diagnostic.message));
   if (diagnostic.message.empty()) diagnostic.message = "The compiler reported an unspecified diagnostic.";
   if (diagnostic.range.valid && diagnostic.range.uri.empty()) {
      diagnostic.range.valid = false;
      ++stats.invalidRangesNormalized;
   }
   if (diagnostic.range.valid && diagnostic.range.start.byteOffset > diagnostic.range.end.byteOffset) {
      diagnostic.range.end = diagnostic.range.start;
      ++stats.invalidRangesNormalized;
   }
   if (diagnostic.phase == DiagnosticPhase::Unknown) {
      if (const auto *descriptor = DiagnosticCatalog::find(diagnostic.code))
         diagnostic.phase = descriptor->phase;
   }
   diagnostic.relatedInformation.erase(std::unique(diagnostic.relatedInformation.begin(),
      diagnostic.relatedInformation.end(),[](const auto &left,const auto &right) {
         return left.message == right.message && rangeKey(left.range) == rangeKey(right.range);
      }),diagnostic.relatedInformation.end());
   diagnostic.notes.erase(std::remove_if(diagnostic.notes.begin(),diagnostic.notes.end(),
      [](const std::string &value) { return value.empty(); }),diagnostic.notes.end());
   if (diagnostic.fingerprint.empty()) diagnostic.fingerprint = identity(diagnostic);
}

void DiagnosticPipeline::deduplicate(std::vector<DiagnosticCandidate> &candidates,
   DiagnosticPipelineStats &stats)
{
   std::set<std::string> seen;
   candidates.erase(std::remove_if(candidates.begin(),candidates.end(),[&](const auto &candidate) {
      const auto result = seen.insert(identity(candidate.diagnostic));
      if (result.second) return false;
      ++stats.exactDuplicatesRemoved;
      return true;
   }),candidates.end());
}

void DiagnosticPipeline::suppressCascades(std::vector<DiagnosticCandidate> &candidates,
   DiagnosticPipelineStats &stats)
{
   std::set<std::uint64_t> causes;
   for (const auto &candidate : candidates)
      if (candidate.cause.value != 0
         && candidate.publicationClass != DiagnosticPublicationClass::Cascaded)
         causes.insert(candidate.cause.value);
   candidates.erase(std::remove_if(candidates.begin(),candidates.end(),[&](const auto &candidate) {
      if (candidate.publicationClass != DiagnosticPublicationClass::Cascaded) return false;
      const bool hasCause = std::any_of(candidate.dependsOn.begin(),candidate.dependsOn.end(),
         [&](const auto &cause) { return causes.find(cause.value) != causes.end(); });
      if (hasCause) ++stats.cascadesSuppressed;
      return hasCause;
   }),candidates.end());
}

void DiagnosticPipeline::sort(std::vector<DiagnosticCandidate> &candidates)
{
   std::stable_sort(candidates.begin(),candidates.end(),[](const auto &left,const auto &right) {
      const auto &a = left.diagnostic;
      const auto &b = right.diagnostic;
      const std::string aUri = a.range.valid ? a.range.uri : std::string();
      const std::string bUri = b.range.valid ? b.range.uri : std::string();
      if (aUri != bUri) return aUri < bUri;
      if (a.range.valid != b.range.valid) return a.range.valid > b.range.valid;
      if (a.range.valid && a.range.start.byteOffset != b.range.start.byteOffset)
         return a.range.start.byteOffset < b.range.start.byteOffset;
      if (a.range.valid && a.range.end.byteOffset != b.range.end.byteOffset)
         return a.range.end.byteOffset < b.range.end.byteOffset;
      if (a.severity != b.severity) return severityRank(a.severity) < severityRank(b.severity);
      if (a.phase != b.phase) return phaseRank(a.phase) < phaseRank(b.phase);
      if (a.code != b.code) return a.code < b.code;
      if (a.message != b.message) return a.message < b.message;
      return left.producer < right.producer;
   });
}

PublishedDiagnostics DiagnosticPipeline::publish(std::vector<DiagnosticCandidate> candidates) const
{
   PublishedDiagnostics result;
   result.stats.candidates = candidates.size();
   for (auto &candidate : candidates) normalize(candidate.diagnostic,result.stats);
   deduplicate(candidates,result.stats);
   suppressCascades(candidates,result.stats);
   sort(candidates);
   result.values.reserve(candidates.size());
   for (auto &candidate : candidates) result.values.push_back(std::move(candidate.diagnostic));
   result.stats.published = result.values.size();
   return result;
}

} // namespace ilic
