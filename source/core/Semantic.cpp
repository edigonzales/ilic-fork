#include "../../include/ilic/Semantic.h"

#include "../metamodel/MetaModel.h"
#include "../metamodel/MetaModelInput.h"

#include <algorithm>
#include <map>
#include <set>
#include <sstream>

namespace ilic {
namespace {

std::string join(const std::string &prefix,const std::string &name)
{
   return prefix.empty() ? name : prefix + "." + name;
}

std::string classKind(const metamodel::Class &value)
{
   switch (value.Kind) {
      case metamodel::Class::Structure: return "structure";
      case metamodel::Class::ClassVal: return "class";
      case metamodel::Class::ViewVal: return "view";
      case metamodel::Class::Association: return "association";
   }
   return "class";
}

std::string semanticKind(metamodel::MetaElement *element)
{
   if (dynamic_cast<metamodel::Model *>(element) != nullptr) return "model";
   if (dynamic_cast<metamodel::SubModel *>(element) != nullptr) return "topic";
   if (auto *value = dynamic_cast<metamodel::Class *>(element)) return classKind(*value);
   if (dynamic_cast<metamodel::AttrOrParam *>(element) != nullptr) return "attribute";
   if (dynamic_cast<metamodel::Unit *>(element) != nullptr) return "unit";
   if (dynamic_cast<metamodel::FunctionDef *>(element) != nullptr) return "function";
   if (dynamic_cast<metamodel::Constraint *>(element) != nullptr) return "constraint";
   if (dynamic_cast<metamodel::Graphic *>(element) != nullptr) return "graphic";
   if (dynamic_cast<metamodel::DomainType *>(element) != nullptr) return "domain";
   return element->getClass();
}

bool isAbstract(metamodel::MetaElement *element)
{
   auto *extendable = dynamic_cast<metamodel::ExtendableME *>(element);
   return extendable != nullptr && extendable->Abstract;
}

std::string documentation(metamodel::MetaElement *element)
{
   std::ostringstream text;
   bool first = true;
   for (auto *entry : element->Documentation) {
      if (entry == nullptr || entry->Text.empty()) continue;
      if (!first) text << '\n';
      text << entry->Text;
      first = false;
   }
   return text.str();
}

std::string cardinality(const metamodel::Multiplicity &value)
{
   if (value.Min < 0 && value.Max < 0) return "";
   const std::string lower = value.Min < 0 ? "*" : std::to_string(value.Min);
   const std::string upper = value.Max < 0 ? "*" : std::to_string(value.Max);
   return "{" + lower + ".." + upper + "}";
}

class SnapshotBuilder {
public:
   explicit SnapshotBuilder(SemanticSnapshot &snapshot) : snapshot_(snapshot) {}

   void addModel(metamodel::Model *model)
   {
      if (model == nullptr) return;
      if (snapshot_.documentation.title.empty() && model->_ilifile != "internal")
         snapshot_.documentation.title = model->Name;
      addElement(model,"","",1);
   }

   void finishReferences()
   {
      for (auto *element : ordered_) {
         const std::string sourceId = id(element);
         if (auto *extendable = dynamic_cast<metamodel::ExtendableME *>(element)) {
            addReference(sourceId,extendable->Super,"inheritance",element->_source);
         }
         if (auto *attribute = dynamic_cast<metamodel::AttrOrParam *>(element)) {
            addReference(sourceId,attribute->Type,"type",element->_source);
         }
         if (auto *role = dynamic_cast<metamodel::Role *>(element)) {
            addReference(sourceId,role->_baseclass,"role",element->_source);
         }
      }
      addDiagramEdges();
   }

private:
   SemanticSnapshot &snapshot_;
   std::map<metamodel::MetaElement *,std::string> ids_;
   std::vector<metamodel::MetaElement *> ordered_;
   std::set<std::string> diagramNodeIds_;

   std::string id(metamodel::MetaElement *element) const
   {
      auto found = ids_.find(element);
      return found == ids_.end() ? "" : found->second;
   }

   void addElement(metamodel::MetaElement *element,const std::string &prefix,
      const std::string &containerId,int level)
   {
      if (element == nullptr || ids_.find(element) != ids_.end()) return;
      const std::string qualifiedName = join(prefix,element->Name.empty() ? element->getClass() : element->Name);
      const std::string kind = semanticKind(element);
      const std::string symbolId = kind + ":" + qualifiedName;
      ids_[element] = symbolId;
      ordered_.push_back(element);
      snapshot_.symbols.push_back({symbolId,element->Name,qualifiedName,kind,containerId,
         element->_source,isAbstract(element)});
      snapshot_.documentation.sections.push_back({symbolId,
         element->Name.empty() ? element->getClass() : element->Name,
         kind,documentation(element),level});

      if (dynamic_cast<metamodel::Model *>(element) != nullptr ||
          dynamic_cast<metamodel::SubModel *>(element) != nullptr) {
         snapshot_.diagram.nodes.push_back({symbolId,containerId,element->Name,"container",
            false,element->_source,{}});
         diagramNodeIds_.insert(symbolId);
      }
      else if (auto *viewable = dynamic_cast<metamodel::Class *>(element)) {
         if (viewable->Kind != metamodel::Class::Association) {
            DiagramNode node{symbolId,containerId,element->Name,classKind(*viewable),
               isAbstract(element),element->_source,{}};
            for (auto *attribute : viewable->ClassAttribute) {
               if (attribute == nullptr) continue;
               const std::string type = attribute->Type != nullptr ? attribute->Type->getClass() : "Unknown";
               node.members.push_back({attribute->Name,type,false});
            }
            snapshot_.diagram.nodes.push_back(std::move(node));
            diagramNodeIds_.insert(symbolId);
         }
      }

      const std::string childContainer =
         dynamic_cast<metamodel::Package *>(element) != nullptr ? symbolId : containerId;
      if (auto *package = dynamic_cast<metamodel::Package *>(element)) {
         for (auto *child : package->Element)
            addElement(child,qualifiedName,childContainer,level + 1);
      }
      if (auto *viewable = dynamic_cast<metamodel::Class *>(element)) {
         for (auto *attribute : viewable->ClassAttribute)
            addElement(attribute,qualifiedName,symbolId,level + 1);
         for (auto *parameter : viewable->ClassParameter)
            addElement(parameter,qualifiedName,symbolId,level + 1);
         for (auto *role : viewable->Role)
            addElement(role,qualifiedName,symbolId,level + 1);
         for (auto *constraint : viewable->Constraint)
            addElement(constraint,qualifiedName,symbolId,level + 1);
      }
   }

   void addReference(const std::string &sourceId,metamodel::MetaElement *target,
      const std::string &kind,const SourceRange &range)
   {
      const std::string targetId = id(target);
      if (sourceId.empty() || targetId.empty()) return;
      snapshot_.references.push_back({sourceId,targetId,kind,range});
   }

   void addDiagramEdges()
   {
      std::size_t edgeIndex = 0;
      for (auto *element : ordered_) {
         auto *viewable = dynamic_cast<metamodel::Class *>(element);
         if (viewable == nullptr) continue;
         const std::string sourceId = id(element);
         if (viewable->Super != nullptr && diagramNodeIds_.count(sourceId) != 0 &&
             diagramNodeIds_.count(id(viewable->Super)) != 0) {
            snapshot_.diagram.edges.push_back({"inheritance:" + std::to_string(edgeIndex++),
               sourceId,id(viewable->Super),"inheritance","",""});
         }
         if (viewable->Kind != metamodel::Class::Association || viewable->Role.size() < 2) continue;
         auto iterator = viewable->Role.begin();
         metamodel::Role *left = *iterator++;
         metamodel::Role *right = *iterator;
         if (left == nullptr || right == nullptr) continue;
         const std::string leftId = id(left->_baseclass);
         const std::string rightId = id(right->_baseclass);
         if (diagramNodeIds_.count(leftId) == 0 || diagramNodeIds_.count(rightId) == 0) continue;
         snapshot_.diagram.edges.push_back({"association:" + std::to_string(edgeIndex++),
            leftId,rightId,"association",element->Name,
            left->Name + " " + cardinality(left->Multiplicity) + " / " +
               right->Name + " " + cardinality(right->Multiplicity)});
      }
   }
};

} // namespace

SemanticSnapshot buildSemanticSnapshot(const SourceManager &sources,
   const CompilationRequest &request,const CompilationResult &compilation)
{
   SemanticSnapshot snapshot;
   snapshot.success = compilation.success;
   snapshot.cancelled = compilation.cancelled;
   snapshot.roots = request.roots;
   snapshot.diagnostics = compilation.diagnostics;
   snapshot.logs = compilation.logs;
   for (const auto &uri : sources.uris()) {
      if (const SourceBuffer *source = sources.get(uri))
         snapshot.documentVersions[uri] = source->version;
   }

   std::map<std::string,std::string> modelUris;
   for (const auto &model : compilation.models) modelUris[model.name] = model.uri;
   for (const auto &uri : sources.uris()) {
      const SyntaxSnapshot syntax = parseSyntax(sources,uri);
      for (const auto &model : syntax.imports) {
         auto found = modelUris.find(model);
         if (found != modelUris.end()) snapshot.dependencies.push_back({uri,found->second,model});
      }
   }

   if (compilation.success) {
      SnapshotBuilder builder(snapshot);
      for (auto *model : metamodel::get_all_models()) builder.addModel(model);
      builder.finishReferences();
   }
   return snapshot;
}

} // namespace ilic
