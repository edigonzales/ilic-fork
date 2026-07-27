#include "DiagnosticCode.h"

#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <stdexcept>

namespace util {

namespace {

const std::vector<DiagnosticDefinition> definitions{
   DiagnosticDefinition{DiagnosticId::AbiRequest,"ILIC-ABI-REQUEST"},
   DiagnosticDefinition{DiagnosticId::AssociationDuplicateRole,"ILIC-ASSOCIATION-DUPLICATE-ROLE"},
   DiagnosticDefinition{DiagnosticId::AssociationEndNameMismatch,"ILIC-ASSOCIATION-END-NAME-MISMATCH"},
   DiagnosticDefinition{DiagnosticId::AssociationInvalidBaseKind,"ILIC-ASSOCIATION-INVALID-BASE-KIND"},
   DiagnosticDefinition{DiagnosticId::AssociationMissingEndName,"ILIC-ASSOCIATION-MISSING-END-NAME"},
   DiagnosticDefinition{DiagnosticId::AssociationRoleCount,"ILIC-ASSOCIATION-ROLE-COUNT"},
   DiagnosticDefinition{DiagnosticId::AssociationUnexpectedEndName,"ILIC-ASSOCIATION-UNEXPECTED-END-NAME"},
   DiagnosticDefinition{DiagnosticId::AttributeIncompatibleExtension,"ILIC-ATTRIBUTE-INCOMPATIBLE-EXTENSION"},
   DiagnosticDefinition{DiagnosticId::CardinalityRule,"ILIC-CARDINALITY-RULE"},
   DiagnosticDefinition{DiagnosticId::ClassExtendedRequired,"ILIC-CLASS-EXTENDED-REQUIRED"},
   DiagnosticDefinition{DiagnosticId::CompilerInternal,"ILIC-COMPILER-INTERNAL"},
   DiagnosticDefinition{DiagnosticId::ConstraintRule,"ILIC-CONSTRAINT-RULE"},
   DiagnosticDefinition{DiagnosticId::DependencyRule,"ILIC-DEPENDENCY-RULE"},
   DiagnosticDefinition{DiagnosticId::EnumerationRule,"ILIC-ENUMERATION-RULE"},
   DiagnosticDefinition{DiagnosticId::FormatEncoding,"ILIC-FORMAT-ENCODING"},
   DiagnosticDefinition{DiagnosticId::FormatVersion,"ILIC-FORMAT-VERSION"},
   DiagnosticDefinition{DiagnosticId::FunctionSignature,"ILIC-FUNCTION-SIGNATURE"},
   DiagnosticDefinition{DiagnosticId::GenericContext,"ILIC-GENERIC-CONTEXT"},
   DiagnosticDefinition{DiagnosticId::GenericCoordRangeMismatch,"ILIC-GENERIC-COORD-RANGE-MISMATCH"},
   DiagnosticDefinition{DiagnosticId::InheritanceRule,"ILIC-INHERITANCE-RULE"},
   DiagnosticDefinition{DiagnosticId::InputLoad,"ILIC-INPUT-LOAD"},
   DiagnosticDefinition{DiagnosticId::InputUnsupportedVersion,"ILIC-INPUT-UNSUPPORTED-VERSION"},
   DiagnosticDefinition{DiagnosticId::MetaDangling,"ILIC-META-DANGLING"},
   DiagnosticDefinition{DiagnosticId::MetaSyntax,"ILIC-META-SYNTAX"},
   DiagnosticDefinition{DiagnosticId::MetaTarget,"ILIC-META-TARGET"},
   DiagnosticDefinition{DiagnosticId::ModelDependency,"ILIC-MODEL-DEPENDENCY"},
   DiagnosticDefinition{DiagnosticId::ModelInvalidDeclaration,"ILIC-MODEL-INVALID-DECLARATION"},
   DiagnosticDefinition{DiagnosticId::NameAttributeNotFound,"ILIC-NAME-ATTRIBUTE-NOT-FOUND"},
   DiagnosticDefinition{DiagnosticId::NameDuplicate,"ILIC-NAME-DUPLICATE"},
   DiagnosticDefinition{DiagnosticId::NameElementNotFound,"ILIC-NAME-ELEMENT-NOT-FOUND"},
   DiagnosticDefinition{DiagnosticId::NameEndMismatch,"ILIC-NAME-END-MISMATCH"},
   DiagnosticDefinition{DiagnosticId::NameModelNotFound,"ILIC-NAME-MODEL-NOT-FOUND"},
   DiagnosticDefinition{DiagnosticId::NameNotFound,"ILIC-NAME-NOT-FOUND"},
   DiagnosticDefinition{DiagnosticId::NameRoleNotFound,"ILIC-NAME-ROLE-NOT-FOUND"},
   DiagnosticDefinition{DiagnosticId::NameTopicNotFound,"ILIC-NAME-TOPIC-NOT-FOUND"},
   DiagnosticDefinition{DiagnosticId::NameTypeNotFound,"ILIC-NAME-TYPE-NOT-FOUND"},
   DiagnosticDefinition{DiagnosticId::NamespaceDuplicateDeclaration,"ILIC-NAMESPACE-DUPLICATE-DECLARATION"},
   DiagnosticDefinition{DiagnosticId::ParseEncoding,"ILIC-PARSE-ENCODING"},
   DiagnosticDefinition{DiagnosticId::ParseSyntax,"ILIC-PARSE-SYNTAX"},
   DiagnosticDefinition{DiagnosticId::PropertyRule,"ILIC-PROPERTY-RULE"},
   DiagnosticDefinition{DiagnosticId::ReferenceRule,"ILIC-REFERENCE-RULE"},
   DiagnosticDefinition{DiagnosticId::RepositoryCache,"ILIC-REPO-CACHE"},
   DiagnosticDefinition{DiagnosticId::RepositoryChecksum,"ILIC-REPO-CHECKSUM"},
   DiagnosticDefinition{DiagnosticId::RepositoryCycle,"ILIC-REPO-CYCLE"},
   DiagnosticDefinition{DiagnosticId::RepositoryDownload,"ILIC-REPO-DOWNLOAD"},
   DiagnosticDefinition{DiagnosticId::RepositoryIndex,"ILIC-REPO-INDEX"},
   DiagnosticDefinition{DiagnosticId::RepositoryNotFound,"ILIC-REPO-NOT-FOUND"},
   DiagnosticDefinition{DiagnosticId::RepositoryPath,"ILIC-REPO-PATH"},
   DiagnosticDefinition{DiagnosticId::RepositorySite,"ILIC-REPO-SITE"},
   DiagnosticDefinition{DiagnosticId::RepositoryUri,"ILIC-REPO-URI"},
   DiagnosticDefinition{DiagnosticId::RepositoryVersion,"ILIC-REPO-VERSION"},
   DiagnosticDefinition{DiagnosticId::SourceNotFound,"ILIC-SOURCE-NOT-FOUND"},
   DiagnosticDefinition{DiagnosticId::Syntax,"ILIC-SYNTAX"},
   DiagnosticDefinition{DiagnosticId::TranslationCoordDimensionMismatch,"ILIC-TRANSLATION-COORD-DIMENSION-MISMATCH"},
   DiagnosticDefinition{DiagnosticId::TranslationDerivedAssociationMismatch,"ILIC-TRANSLATION-DERIVED-ASSOCIATION-MISMATCH"},
   DiagnosticDefinition{DiagnosticId::TranslationDomainReferenceMismatch,"ILIC-TRANSLATION-DOMAIN-REFERENCE-MISMATCH"},
   DiagnosticDefinition{DiagnosticId::TranslationEnumFinalMismatch,"ILIC-TRANSLATION-ENUM-FINAL-MISMATCH"},
   DiagnosticDefinition{DiagnosticId::TranslationMismatch,"ILIC-TRANSLATION-MISMATCH"},
   DiagnosticDefinition{DiagnosticId::TranslationRule,"ILIC-TRANSLATION-RULE"},
   DiagnosticDefinition{DiagnosticId::TypeMismatch,"ILIC-TYPE-MISMATCH"},
   DiagnosticDefinition{DiagnosticId::ValueRange,"ILIC-VALUE-RANGE"},
   DiagnosticDefinition{DiagnosticId::Warning,"ILIC-WARNING"}
};

std::string lower(std::string_view value)
{
   std::string result(value);
   std::transform(result.begin(),result.end(),result.begin(),[](unsigned char character) {
      return static_cast<char>(std::tolower(character));
   });
   return result;
}

bool contains(const std::string &value,std::initializer_list<std::string_view> needles)
{
   return std::any_of(needles.begin(),needles.end(),[&](std::string_view needle) {
      return value.find(needle) != std::string::npos;
   });
}

}

std::string_view diagnosticCode(DiagnosticId id)
{
   auto found = std::find_if(definitions.begin(),definitions.end(),
      [id](const DiagnosticDefinition &definition) { return definition.id == id; });
   if (found == definitions.end()) {
      throw std::invalid_argument("unknown DiagnosticId");
   }
   return found->code;
}

const std::vector<DiagnosticDefinition> &diagnosticDefinitions()
{
   return definitions;
}

std::string diagnosticCodeForMessage(std::string_view message)
{
   const std::string value = lower(message);

   if (contains(value,{"unsupported iliversion","unsupported interlis version"}))
      return "ILIC-INPUT-UNSUPPORTED-VERSION";
   if (contains(value,{"internal compiler failure","internal compiler error"}))
      return "ILIC-COMPILER-INTERNAL";
   if (contains(value,{"unable to load root source","unable to read","could not open"}))
      return "ILIC-INPUT-LOAD";
   if (contains(value,{"unable to order model dependencies","dependency cycle"}))
      return "ILIC-MODEL-DEPENDENCY";

   if (contains(value,{"translation mismatch"}))
      return "ILIC-TRANSLATION-MISMATCH";
   if (contains(value,{"translation of","translation base","translation language",
      "translation metadata","translation chain"}))
      return "ILIC-TRANSLATION-RULE";

   if (contains(value,{"not found","unknown ","there is no ","has no visible",
      "has no accessible","missing a context"})) {
      if (contains(value,{"attribute"})) return "ILIC-NAME-ATTRIBUTE-NOT-FOUND";
      if (contains(value,{"role"})) return "ILIC-NAME-ROLE-NOT-FOUND";
      if (contains(value,{"topic"})) return "ILIC-NAME-TOPIC-NOT-FOUND";
      if (contains(value,{"model"})) return "ILIC-NAME-MODEL-NOT-FOUND";
      if (contains(value,{"domain","type","unit"})) return "ILIC-NAME-TYPE-NOT-FOUND";
      if (contains(value,{"path","reference","viewable","class","structure","association","view"}))
         return "ILIC-NAME-ELEMENT-NOT-FOUND";
      return "ILIC-NAME-NOT-FOUND";
   }

   if (contains(value,{"does not match","must match","should end with","must end with"," expected"}))
      return "ILIC-NAME-END-MISMATCH";
   if (contains(value,{"duplicate","multiple declaration","multiple declarations",
      "already exists","there is already","same name","name conflict","several attributes"}))
      return "ILIC-NAME-DUPLICATE";

   if (contains(value,{"cardinality","multiplicity"}))
      return "ILIC-CARDINALITY-RULE";
   if (contains(value,{"association requires at least","association must have at least"}))
      return "ILIC-ASSOCIATION-ROLE-COUNT";
   if (contains(value,{"extend","extension","extended","inherited","inheritance",
      "base class","base topic","base association","base attribute","base role",
      "base view","final base","subrange"}))
      return "ILIC-INHERITANCE-RULE";
   if (contains(value,{"topic dependency","depend on","requires model","to import",
      "cross-topic","cross topic","requires external","other topic"}))
      return "ILIC-DEPENDENCY-RULE";
   if (contains(value,{"constraint","unique","basket and local","set constraint",
      "required in"}))
      return "ILIC-CONSTRAINT-RULE";
   if (contains(value,{"parent is only valid","thisarea is only valid",
      "thatarea is only valid","aggregates has no aggregation base"}))
      return "ILIC-REFERENCE-RULE";
   if (contains(value,{"generic","deferred","context definition","context default"}))
      return "ILIC-GENERIC-CONTEXT";
   if (contains(value,{"enumeration","enum ","#"}))
      return "ILIC-ENUMERATION-RULE";
   if (contains(value,{"function","argument"}))
      return "ILIC-FUNCTION-SIGNATURE";
   if (contains(value,{"path","reference","referenced","target role","roleaccess"}))
      return "ILIC-REFERENCE-RULE";
   if (contains(value,{"datatype","type","numeric","boolean","logical","text length",
      "formatted value","coordinate","coord ","structure","class ","association ",
      "association","view ","domain "}))
      return "ILIC-TYPE-MISMATCH";
   if (contains(value,{"minimum","maximum","range","precision","overlap","outside"}))
      return "ILIC-VALUE-RANGE";
   if (contains(value,{"abstract","final","transient","external","ordered","hiding",
      "mandatory","composition","aggregation","property ","properties "}))
      return "ILIC-PROPERTY-RULE";

   return "ILIC-MODEL-INVALID-DECLARATION";
}

}
