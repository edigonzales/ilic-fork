#include "RepositoryXml.h"

#include <pugixml.hpp>

#include <algorithm>
#include <cctype>
#include <functional>

namespace ilic::repository {
namespace {

std::string_view localName(std::string_view name)
{
   const std::size_t colon = name.rfind(':');
   return colon == std::string_view::npos ? name : name.substr(colon + 1);
}

bool named(const pugi::xml_node &node,std::string_view expected)
{
   return node.type() == pugi::node_element && localName(node.name()) == expected;
}

std::string childText(const pugi::xml_node &node,std::string_view childName)
{
   for (const auto &child : node.children()) {
      if (named(child,childName)) return child.text().as_string();
   }
   return {};
}

void collectValues(const pugi::xml_node &node,std::vector<std::string> &values)
{
   for (const auto &child : node.children()) {
      if (named(child,"value")) {
         const std::string value = child.text().as_string();
         if (!value.empty()) values.push_back(value);
      }
      else collectValues(child,values);
   }
}

Diagnostic diagnostic(DiagnosticSeverity severity,std::string code,std::string message)
{
   Diagnostic value;
   value.severity = severity;
   value.code = std::move(code);
   value.message = std::move(message);
   return value;
}

bool isMetadataNode(const pugi::xml_node &node)
{
   if (node.type() != pugi::node_element) return false;
   const std::string_view name = localName(node.name());
   constexpr std::string_view suffix = "ModelMetadata";
   return name.size() >= suffix.size()
      && name.substr(name.size() - suffix.size()) == suffix;
}

bool trueValue(std::string value)
{
   std::transform(value.begin(),value.end(),value.begin(),[](unsigned char character) {
      return static_cast<char>(std::tolower(character));
   });
   return value == "true" || value == "1";
}

} // namespace

RepositoryIndex RepositoryXml::parseModelIndex(std::string_view xml,
   std::string_view repository,std::vector<Diagnostic> *diagnostics)
{
   RepositoryIndex index;
   index.repository = std::string(repository);
   pugi::xml_document document;
   const pugi::xml_parse_result parsed = document.load_buffer(xml.data(),xml.size(),
      pugi::parse_default | pugi::parse_ws_pcdata_single);
   if (!parsed) {
      if (diagnostics != nullptr) diagnostics->push_back(diagnostic(DiagnosticSeverity::Error,
         "ILIC-REPO-INDEX","invalid ilimodels.xml in " + index.repository + ": "
            + parsed.description()));
      return index;
   }

   std::function<void(const pugi::xml_node &)> visit = [&](const pugi::xml_node &node) {
      for (const auto &current : node.children()) {
         if (!isMetadataNode(current)) {
            visit(current);
            continue;
         }
         ModelMetadata model;
         model.name = childText(current,"Name");
         model.schemaLanguage = childText(current,"SchemaLanguage");
         model.file = childText(current,"File");
         model.version = childText(current,"Version");
         model.publishingDate = childText(current,"publishingDate");
         model.precursorVersion = childText(current,"precursorVersion");
         model.md5 = childText(current,"md5");
         model.repository = index.repository;
         model.browseOnly = trueValue(childText(current,"browseOnly"));
         for (const auto &child : current.children())
            if (named(child,"dependsOnModel")) collectValues(child,model.dependencies);
         if (model.name.empty() || model.schemaLanguage.empty() || model.file.empty()) {
            if (diagnostics != nullptr) diagnostics->push_back(diagnostic(DiagnosticSeverity::Warning,
               "ILIC-REPO-INDEX","ignoring invalid ModelMetadata entry in " + index.repository
                  + " (Name, SchemaLanguage and File are required)"));
            continue;
         }
         index.models.push_back(std::move(model));
      }
   };
   visit(document);
   if (index.models.empty() && diagnostics != nullptr) diagnostics->push_back(diagnostic(
      DiagnosticSeverity::Error,"ILIC-REPO-INDEX",
      "no valid ModelMetadata entries in ilimodels.xml from " + index.repository));
   return index;
}

RepositorySite RepositoryXml::parseSite(std::string_view xml,
   std::string_view repository,std::vector<Diagnostic> *diagnostics)
{
   RepositorySite site;
   pugi::xml_document document;
   const pugi::xml_parse_result parsed = document.load_buffer(xml.data(),xml.size(),
      pugi::parse_default | pugi::parse_ws_pcdata_single);
   if (!parsed) {
      if (diagnostics != nullptr) diagnostics->push_back(diagnostic(DiagnosticSeverity::Warning,
         "ILIC-REPO-SITE","invalid ilisite.xml in " + std::string(repository) + ": "
            + parsed.description()));
      return site;
   }
   std::function<void(const pugi::xml_node &)> visit = [&](const pugi::xml_node &node) {
      for (const auto &current : node.children()) {
         if (named(current,"parentSite")) collectValues(current,site.parentSites);
         else if (named(current,"subsidiarySite")) collectValues(current,site.subsidiarySites);
         else visit(current);
      }
   };
   visit(document);
   return site;
}

} // namespace ilic::repository
