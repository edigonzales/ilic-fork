#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace util {

enum class DiagnosticId {
   AbiRequest,
   AssociationDuplicateRole,
   AssociationEndNameMismatch,
   AssociationInvalidBaseKind,
   AssociationMissingEndName,
   AssociationRoleCount,
   AssociationUnexpectedEndName,
   AttributeIncompatibleExtension,
   CardinalityRule,
   ClassExtendedRequired,
   CompilerInternal,
   ConstraintRule,
   DependencyRule,
   EnumerationRule,
   FormatEncoding,
   FormatVersion,
   FunctionSignature,
   GenericContext,
   GenericCoordRangeMismatch,
   InheritanceRule,
   InputLoad,
   InputUnsupportedVersion,
   MetaDangling,
   MetaSyntax,
   MetaTarget,
   ModelDependency,
   ModelInvalidDeclaration,
   NameAttributeNotFound,
   NameDuplicate,
   NameElementNotFound,
   NameEndMismatch,
   NameModelNotFound,
   NameNotFound,
   NameRoleNotFound,
   NameTopicNotFound,
   NameTypeNotFound,
   NamespaceDuplicateDeclaration,
   ParseEncoding,
   ParseSyntax,
   PropertyRule,
   ReferenceRule,
   RepositoryCache,
   RepositoryChecksum,
   RepositoryCycle,
   RepositoryDownload,
   RepositoryIndex,
   RepositoryNotFound,
   RepositoryPath,
   RepositorySite,
   RepositoryUri,
   RepositoryVersion,
   SourceNotFound,
   Syntax,
   TranslationCoordDimensionMismatch,
   TranslationDerivedAssociationMismatch,
   TranslationDomainReferenceMismatch,
   TranslationEnumFinalMismatch,
   TranslationMismatch,
   TranslationRule,
   TypeMismatch,
   ValueRange,
   Warning
};

struct DiagnosticDefinition {
   DiagnosticId id;
   std::string_view code;
};

std::string_view diagnosticCode(DiagnosticId id);
const std::vector<DiagnosticDefinition> &diagnosticDefinitions();

// Returns a stable public code for legacy semantic diagnostics that have not
// yet been migrated to an explicit source-site code.
std::string diagnosticCodeForMessage(std::string_view message);

}
