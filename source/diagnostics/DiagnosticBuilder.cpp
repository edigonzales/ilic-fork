#include "ilic/DiagnosticBuilder.h"

#include "ilic/DiagnosticCatalog.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

namespace ilic {

DiagnosticBuilder::DiagnosticBuilder(std::string code)
{
   candidate_.diagnostic.code = std::move(code);
}

DiagnosticBuilder &DiagnosticBuilder::severity(DiagnosticSeverity value)
{
   candidate_.diagnostic.severity = value;
   explicitSeverity_ = true;
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::source(std::string value)
{
   candidate_.diagnostic.source = std::move(value);
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::message(std::string value)
{
   candidate_.diagnostic.message = std::move(value);
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::primaryRange(SourceRange value)
{
   candidate_.diagnostic.range = std::move(value);
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::related(SourceRange range,std::string message)
{
   candidate_.diagnostic.relatedInformation.push_back({std::move(range),std::move(message)});
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::note(std::string value)
{
   candidate_.diagnostic.notes.push_back(std::move(value));
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::treatedAsError(bool value)
{
   candidate_.diagnostic.treatedAsError = value;
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::phase(DiagnosticPhase value)
{
   candidate_.diagnostic.phase = value;
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::tag(DiagnosticTag value)
{
   if (std::find(candidate_.diagnostic.tags.begin(),candidate_.diagnostic.tags.end(),value)
      == candidate_.diagnostic.tags.end()) candidate_.diagnostic.tags.push_back(value);
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::cause(DiagnosticCauseId value)
{
   candidate_.cause = value;
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::dependsOn(DiagnosticCauseId value)
{
   if (std::find(candidate_.dependsOn.begin(),candidate_.dependsOn.end(),value)
      == candidate_.dependsOn.end()) candidate_.dependsOn.push_back(value);
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::publicationClass(DiagnosticPublicationClass value)
{
   candidate_.publicationClass = value;
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::producer(std::string value)
{
   candidate_.producer = std::move(value);
   return *this;
}

DiagnosticBuilder &DiagnosticBuilder::helpId(std::string value)
{
   candidate_.diagnostic.helpId = std::move(value);
   return *this;
}

DiagnosticCandidate DiagnosticBuilder::build()
{
   const auto &descriptor = DiagnosticCatalog::require(candidate_.diagnostic.code);
   if (!explicitSeverity_) candidate_.diagnostic.severity = descriptor.defaultSeverity;
   if (candidate_.diagnostic.message.empty())
      throw std::invalid_argument("diagnostic message must not be empty");
   if (candidate_.diagnostic.source.empty()) candidate_.diagnostic.source = "compiler";
   if (candidate_.diagnostic.phase == DiagnosticPhase::Unknown)
      candidate_.diagnostic.phase = descriptor.phase;
   candidate_.diagnostic.relatedInformation.erase(std::remove_if(
      candidate_.diagnostic.relatedInformation.begin(),
      candidate_.diagnostic.relatedInformation.end(),[](const auto &value) {
         return value.message.empty();
      }),candidate_.diagnostic.relatedInformation.end());
   candidate_.diagnostic.notes.erase(std::remove_if(candidate_.diagnostic.notes.begin(),
      candidate_.diagnostic.notes.end(),[](const auto &value) { return value.empty(); }),
      candidate_.diagnostic.notes.end());
   if (candidate_.publicationClass == DiagnosticPublicationClass::Primary)
      tag(DiagnosticTag::Primary);
   else if (candidate_.publicationClass == DiagnosticPublicationClass::Cascaded)
      tag(DiagnosticTag::Cascaded);
   else if (candidate_.publicationClass == DiagnosticPublicationClass::Recovery)
      tag(DiagnosticTag::Recovery);
   return std::move(candidate_);
}

} // namespace ilic
