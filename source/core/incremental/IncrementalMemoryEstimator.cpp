#include "IncrementalMemoryEstimator.h"

namespace ilic::detail {
namespace {

std::size_t stringBytes(const std::string &value) noexcept { return value.capacity(); }

template<class T>
std::size_t vectorBytes(const std::vector<T> &value) noexcept
{
   return value.capacity() * sizeof(T);
}

std::size_t rangeBytes(const SourceRange &value) noexcept
{
   return sizeof(value) + stringBytes(value.uri);
}

std::size_t diagnosticBytes(const Diagnostic &value) noexcept
{
   std::size_t bytes = sizeof(value) + stringBytes(value.code) + stringBytes(value.message)
      + rangeBytes(value.range) + stringBytes(value.source)
      + vectorBytes(value.relatedInformation) + vectorBytes(value.notes);
   for (const auto &related : value.relatedInformation)
      bytes += rangeBytes(related.range) + stringBytes(related.message);
   for (const auto &note : value.notes) bytes += stringBytes(note);
   return bytes;
}

std::size_t diagramBytes(const DiagramProjection &value) noexcept
{
   std::size_t bytes = sizeof(value) + value.nodes.capacity() * sizeof(DiagramNode)
      + value.edges.capacity() * sizeof(DiagramEdge);
   for (const auto &node : value.nodes) {
      bytes += stringBytes(node.id) + stringBytes(node.containerId) + stringBytes(node.label)
         + stringBytes(node.kind) + rangeBytes(node.range)
         + node.stereotypes.capacity() * sizeof(std::string)
         + node.members.capacity() * sizeof(DiagramMember)
         + node.enumValues.capacity() * sizeof(std::string)
         + node.operations.capacity() * sizeof(std::string);
      for (const auto &stereotype : node.stereotypes) bytes += stringBytes(stereotype);
      for (const auto &member : node.members) {
         bytes += stringBytes(member.name) + stringBytes(member.type)
            + stringBytes(member.cardinality) + stringBytes(member.declaringType)
            + member.inlineEnumValues.capacity() * sizeof(std::string);
         for (const auto &value : member.inlineEnumValues) bytes += stringBytes(value);
      }
      for (const auto &value : node.enumValues) bytes += stringBytes(value);
      for (const auto &operation : node.operations) bytes += stringBytes(operation);
   }
   for (const auto &edge : value.edges)
      bytes += stringBytes(edge.id) + stringBytes(edge.sourceId) + stringBytes(edge.targetId)
         + stringBytes(edge.kind) + stringBytes(edge.label) + stringBytes(edge.cardinality)
         + stringBytes(edge.sourceCardinality) + stringBytes(edge.targetCardinality);
   return bytes;
}

std::size_t documentationBytes(const DocumentationProjection &value) noexcept
{
   std::size_t bytes = sizeof(value) + stringBytes(value.title)
      + value.sections.capacity() * sizeof(DocumentationSection)
      + value.models.capacity() * sizeof(DocumentationModel);
   for (const auto &section : value.sections)
      bytes += stringBytes(section.id) + stringBytes(section.title)
         + stringBytes(section.kind) + stringBytes(section.text);
   for (const auto &model : value.models) {
      bytes += stringBytes(model.name) + stringBytes(model.uri) + stringBytes(model.title)
         + stringBytes(model.shortDescription)
         + model.topics.capacity() * sizeof(DocumentationTopic)
         + model.viewables.capacity() * sizeof(DocumentationViewable)
         + model.enumerations.capacity() * sizeof(DocumentationEnumeration);
      for (const auto &topic : model.topics) {
         bytes += stringBytes(topic.name) + stringBytes(topic.documentation)
            + topic.viewables.capacity() * sizeof(DocumentationViewable)
            + topic.enumerations.capacity() * sizeof(DocumentationEnumeration);
      }
      for (const auto &viewable : model.viewables) {
         bytes += stringBytes(viewable.name) + stringBytes(viewable.kind)
            + stringBytes(viewable.documentation)
            + viewable.rows.capacity() * sizeof(DocumentationRow);
      }
      for (const auto &enumeration : model.enumerations) {
         bytes += stringBytes(enumeration.name) + stringBytes(enumeration.documentation)
            + enumeration.entries.capacity() * sizeof(DocumentationEnumerationEntry);
      }
   }
   return bytes;
}

} // namespace

std::size_t IncrementalMemoryEstimator::sourceBytes(const std::string &value) noexcept
{
   return sizeof(value) + stringBytes(value);
}

std::size_t IncrementalMemoryEstimator::diagnostics(const std::vector<Diagnostic> &values) noexcept
{
   std::size_t bytes = sizeof(values) + vectorBytes(values);
   for (const auto &value : values) bytes += diagnosticBytes(value);
   return bytes;
}

std::size_t IncrementalMemoryEstimator::syntax(const SyntaxSnapshot &value) noexcept
{
   std::size_t bytes = sizeof(value) + stringBytes(value.uri) + stringBytes(value.iliVersion)
      + vectorBytes(value.tokens) + vectorBytes(value.nodes) + vectorBytes(value.contexts)
      + vectorBytes(value.imports) + vectorBytes(value.importReferences)
      + diagnostics(value.diagnostics);
   for (const auto &token : value.tokens)
      bytes += stringBytes(token.kind) + stringBytes(token.text) + rangeBytes(token.range);
   for (const auto &node : value.nodes) bytes += stringBytes(node.kind) + rangeBytes(node.range);
   for (const auto &context : value.contexts)
      bytes += stringBytes(context.kind) + rangeBytes(context.range);
   for (const auto &name : value.imports) bytes += stringBytes(name);
   for (const auto &reference : value.importReferences)
      bytes += stringBytes(reference.model) + rangeBytes(reference.range);
   return bytes;
}

std::size_t IncrementalMemoryEstimator::editor(const EditorSnapshot &value) noexcept
{
   std::size_t bytes = sizeof(value) + stringBytes(value.uri) + stringBytes(value.iliVersion)
      + vectorBytes(value.declarations) + vectorBytes(value.references)
      + vectorBytes(value.imports) + vectorBytes(value.contexts) + diagnostics(value.diagnostics);
   for (const auto &declaration : value.declarations) {
      bytes += stringBytes(declaration.id) + stringBytes(declaration.name)
         + stringBytes(declaration.qualifiedName) + stringBytes(declaration.containerId)
         + rangeBytes(declaration.range) + rangeBytes(declaration.selectionRange)
         + rangeBytes(declaration.endRange);
   }
   for (const auto &reference : value.references)
      bytes += stringBytes(reference.text) + stringBytes(reference.sourceId) + rangeBytes(reference.range);
   for (const auto &reference : value.imports)
      bytes += stringBytes(reference.model) + rangeBytes(reference.range);
   for (const auto &context : value.contexts)
      bytes += stringBytes(context.kind) + rangeBytes(context.range);
   return bytes;
}

std::size_t IncrementalMemoryEstimator::artifact(const ParsedSourceArtifact &value) noexcept
{
   return value.estimatedRetainedBytes() + diagnostics(value.parserDiagnostics());
}

std::size_t IncrementalMemoryEstimator::compilation(const CompilationAnalysisResult &value) noexcept
{
   std::size_t bytes = sizeof(value) + value.compilation.missingModels.capacity() * sizeof(std::string)
      + value.compilation.models.capacity() * sizeof(CompiledModel)
      + value.compilation.diagnostics.capacity() * sizeof(Diagnostic)
      + value.compilation.logs.capacity() * sizeof(LogEvent)
      + value.compilation.transcript.capacity() * sizeof(std::string)
      + value.syntax.capacity() * sizeof(SyntaxSnapshot);
   for (const auto &model : value.compilation.models) {
      bytes += stringBytes(model.name) + stringBytes(model.iliVersion) + stringBytes(model.uri)
         + model.metaAttributes.capacity() * sizeof(CompiledModel::MetaAttributeValue);
      for (const auto &attribute : model.metaAttributes)
         bytes += stringBytes(attribute.name) + stringBytes(attribute.value);
   }
   for (const auto &missing : value.compilation.missingModels) bytes += stringBytes(missing);
   for (const auto &diagnostic : value.compilation.diagnostics) bytes += diagnosticBytes(diagnostic);
   for (const auto &event : value.compilation.logs) {
      bytes += stringBytes(event.category) + stringBytes(event.message);
      for (const auto &context : event.context)
         bytes += stringBytes(context.first) + stringBytes(context.second);
   }
   for (const auto &line : value.compilation.transcript) bytes += stringBytes(line);
   bytes += sizeof(value.semantic) + value.semantic.symbols.capacity() * sizeof(SemanticSymbol)
      + value.semantic.references.capacity() * sizeof(SemanticReference)
      + value.semantic.dependencies.capacity() * sizeof(SemanticDependency)
      + value.semantic.diagnostics.capacity() * sizeof(Diagnostic)
      + value.semantic.documentVersions.size() * (sizeof(std::string) + sizeof(std::uint64_t))
      + value.semantic.missingModels.capacity() * sizeof(std::string)
      + diagramBytes(value.semantic.diagram)
      + documentationBytes(value.semantic.documentation);
   for (const auto &version : value.semantic.documentVersions)
      bytes += stringBytes(version.first);
   for (const auto &missing : value.semantic.missingModels) bytes += stringBytes(missing);
   for (const auto &symbol : value.semantic.symbols)
      bytes += stringBytes(symbol.id) + stringBytes(symbol.name) + stringBytes(symbol.qualifiedName)
         + stringBytes(symbol.kind) + stringBytes(symbol.containerId) + rangeBytes(symbol.range)
         + rangeBytes(symbol.selectionRange) + rangeBytes(symbol.endRange);
   for (const auto &reference : value.semantic.references)
      bytes += stringBytes(reference.sourceId) + stringBytes(reference.targetId)
         + stringBytes(reference.kind) + rangeBytes(reference.range);
   for (const auto &dependency : value.semantic.dependencies)
      bytes += stringBytes(dependency.sourceUri) + stringBytes(dependency.targetUri)
         + stringBytes(dependency.model) + rangeBytes(dependency.range);
   for (const auto &diagnostic : value.semantic.diagnostics) bytes += diagnosticBytes(diagnostic);
   for (const auto &syntax : value.syntax) bytes += IncrementalMemoryEstimator::syntax(syntax);
   return bytes;
}

} // namespace ilic::detail
