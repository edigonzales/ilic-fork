#include "ilic/DiagnosticCatalog.h"

#include "../util/DiagnosticCode.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

namespace ilic {
namespace {

DiagnosticPhase phaseFor(std::string_view code) noexcept
{
   if (code.find("ILIC-LIVE-") == 0) return DiagnosticPhase::EditorRecovery;
   if (code.find("ILIC-PARSE-") == 0 || code.find("ILIC-SYNTAX") == 0)
      return DiagnosticPhase::Syntax;
   if (code.find("ILIC-REPO-") == 0) return DiagnosticPhase::Repository;
   if (code.find("ILIC-FORMAT-") == 0) return DiagnosticPhase::Formatting;
   if (code.find("ILIC-ABI-") == 0) return DiagnosticPhase::Request;
   if (code.find("ILIC-COMPILER-INTERNAL") == 0)
      return DiagnosticPhase::Internal;
   if (code.find("ILIC-TRANSLATION-") == 0)
      return DiagnosticPhase::Translation;
   if (code.find("ILIC-NAME-") == 0 || code.find("ILIC-REFERENCE-") == 0
      || code.find("ILIC-MODEL-") == 0 || code.find("ILIC-SOURCE-") == 0)
      return DiagnosticPhase::Resolution;
   if (code.find("ILIC-WARNING") == 0) return DiagnosticPhase::Semantic;
   return DiagnosticPhase::Semantic;
}

std::vector<DiagnosticDescriptor> buildDescriptors()
{
   std::vector<DiagnosticDescriptor> result;
   result.reserve(util::diagnosticDefinitions().size() + 10);
   for (const auto &definition : util::diagnosticDefinitions()) {
      result.push_back({definition.code,DiagnosticSeverity::Error,phaseFor(definition.code),
         definition.code,definition.code,true,false});
   }
   // These are emitted by the tolerant editor projection and are part of the
   // public diagnostic vocabulary even though they are not compiler rules.
   const std::pair<std::string_view,DiagnosticPhase> additive[]{
      {"ILIC-LEX-UNRECOGNIZED-CHARACTER",DiagnosticPhase::Lexical},
      {"ILIC-SYN-UNEXPECTED-TOKEN",DiagnosticPhase::Syntax},
      {"ILIC-SYN-EXTRANEOUS-TOKEN",DiagnosticPhase::Syntax},
      {"ILIC-SYN-MISSING-TOKEN",DiagnosticPhase::Syntax},
      {"ILIC-SYN-MISMATCHED-INPUT",DiagnosticPhase::Syntax},
      {"ILIC-SYN-NO-VIABLE-ALTERNATIVE",DiagnosticPhase::Syntax},
      {"ILIC-SYN-UNEXPECTED-EOF",DiagnosticPhase::Syntax},
      {"ILIC-SYN-INCOMPLETE-CONSTRUCT",DiagnosticPhase::Syntax},
      {"ILIC-LIVE-END-NAME",DiagnosticPhase::EditorRecovery},
      {"ILIC-LIVE-DUPLICATE",DiagnosticPhase::EditorRecovery},
      {"ILIC-REPO-CANCELLED",DiagnosticPhase::Repository},
      {"ILIC-REPO-CONFIG",DiagnosticPhase::Repository},
      {"ILIC-REPO-INSTALL",DiagnosticPhase::Repository},
      {"ILIC-REPO-LIMIT",DiagnosticPhase::Repository},
      {"ILIC-REPO-OFFLINE",DiagnosticPhase::Repository},
      {"ILIC-REPO-POLICY",DiagnosticPhase::Repository},
      {"ILIC-REPO-XML",DiagnosticPhase::Repository}
   };
   for (const auto &[code,phase] : additive)
      result.push_back({code,DiagnosticSeverity::Error,phase,code,code,false,true});
   return result;
}

const std::vector<DiagnosticDescriptor> &descriptors() noexcept
{
   static const std::vector<DiagnosticDescriptor> value = buildDescriptors();
   return value;
}

} // namespace

const DiagnosticDescriptor *DiagnosticCatalog::find(std::string_view code) noexcept
{
   const auto &values = descriptors();
   const auto found = std::find_if(values.begin(),values.end(),[code](const auto &value) {
      return value.code == code;
   });
   return found == values.end() ? nullptr : &*found;
}

const DiagnosticDescriptor &DiagnosticCatalog::require(std::string_view code)
{
   const auto *value = find(code);
   if (value == nullptr) throw std::invalid_argument("unregistered diagnostic code: " + std::string(code));
   return *value;
}

std::vector<DiagnosticDescriptor> DiagnosticCatalog::all()
{
   return descriptors();
}

bool DiagnosticCatalog::isRegistered(std::string_view code) noexcept
{
   return find(code) != nullptr;
}

} // namespace ilic
