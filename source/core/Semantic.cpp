#include "../../include/ilic/Semantic.h"

#include "../metamodel/MetaModel.h"
#include "../metamodel/MetaModelInput.h"

#include <algorithm>
#include <functional>
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
   if (auto *topic = dynamic_cast<metamodel::SubModel *>(element))
      return topic->_dataunit != nullptr && topic->_dataunit->Abstract;
   auto *extendable = dynamic_cast<metamodel::ExtendableME *>(element);
   return extendable != nullptr && extendable->Abstract;
}

void enumValues(metamodel::EnumType *type,std::vector<std::string> &values)
{
   if (type == nullptr || type->TopNode == nullptr) return;
   std::function<void(metamodel::EnumNode *,const std::string &)> visit =
      [&](metamodel::EnumNode *node,const std::string &prefix) {
         if (node == nullptr) return;
         if (node == type->TopNode) {
            for (auto *child : node->Node) visit(child,prefix);
            return;
         }
         const std::string value = join(prefix,node->Name);
         if (!value.empty()) values.push_back(value);
         for (auto *child : node->Node) visit(child,value);
      };
   visit(type->TopNode,"");
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

std::string diagramTypeName(metamodel::Type *type)
{
   if (type == nullptr) return "Unknown";
   if (!type->Name.empty() && type->Name != "Type" && type->Name != "TYPE") return type->Name;
   if (auto *related = dynamic_cast<metamodel::ClassRelatedType *>(type)) {
      if (related->_baseclass != nullptr && !related->_baseclass->Name.empty())
         return related->_baseclass->Name;
   }
   if (dynamic_cast<metamodel::BooleanType *>(type) != nullptr) return "BOOLEAN";
   if (auto *text = dynamic_cast<metamodel::TextType *>(type)) {
      switch (text->Kind) {
         case metamodel::TextType::MText: return "MTEXT";
         case metamodel::TextType::NameVal: return "NAME";
         case metamodel::TextType::Uri: return "URI";
         case metamodel::TextType::Text: return "TEXT";
      }
   }
   if (dynamic_cast<metamodel::NumType *>(type) != nullptr) return "NUMERIC";
   if (auto *coord = dynamic_cast<metamodel::CoordType *>(type))
      return coord->Multi ? "MULTICOORD" : "COORD";
   if (auto *line = dynamic_cast<metamodel::LineType *>(type)) {
      switch (line->Kind) {
         case metamodel::LineType::Polyline: return "POLYLINE";
         case metamodel::LineType::DirectedPolyline: return "DIRECTED POLYLINE";
         case metamodel::LineType::Surface: return "SURFACE";
         case metamodel::LineType::Area: return "AREA";
         case metamodel::LineType::MultiPolyline: return "MULTIPOLYLINE";
         case metamodel::LineType::DirectedMultiPolyline: return "DIRECTED MULTIPOLYLINE";
         case metamodel::LineType::MultiSurface: return "MULTISURFACE";
         case metamodel::LineType::MultiArea: return "MULTIAREA";
      }
   }
   if (dynamic_cast<metamodel::EnumType *>(type) != nullptr ||
       dynamic_cast<metamodel::EnumTreeValueType *>(type) != nullptr)
      return "ENUMERATION";
   if (dynamic_cast<metamodel::AnyOIDType *>(type) != nullptr) return "ANY OID";
   if (dynamic_cast<metamodel::AttributeRefType *>(type) != nullptr) return "ATTRIBUTE";
   if (auto *multi = dynamic_cast<metamodel::MultiValue *>(type))
      return "LIST OF " + diagramTypeName(multi->BaseType);
   if (dynamic_cast<metamodel::BlackboxType *>(type) != nullptr) return "BLACKBOX";
   return type->getClass();
}

class SnapshotBuilder {
public:
   SnapshotBuilder(SemanticSnapshot &snapshot,
      const std::set<std::string> &diagramRootFiles)
      : snapshot_(snapshot),diagramRootFiles_(diagramRootFiles) {}

   void addModel(metamodel::Model *model)
   {
      if (model == nullptr) return;
      if (snapshot_.documentation.title.empty() && model->_ilifile != "internal")
         snapshot_.documentation.title = model->Name;
      addElement(model,"","",1,diagramRootFiles_.count(model->_ilifile) != 0);
   }

   void finishReferences()
   {
      for (auto *element : ordered_) {
         const std::string sourceId = id(element);
         if (auto *topic = dynamic_cast<metamodel::SubModel *>(element)) {
            addReference(sourceId,topic->_super,"inheritance",element);
         }
         if (auto *extendable = dynamic_cast<metamodel::ExtendableME *>(element)) {
            addReference(sourceId,extendable->Super,"inheritance",element);
         }
         if (auto *attribute = dynamic_cast<metamodel::AttrOrParam *>(element)) {
            addReference(sourceId,attribute->Extending,"inheritance",element);
            addReference(sourceId,typeTarget(attribute->Type),"type",element);
         }
         if (auto *role = dynamic_cast<metamodel::Role *>(element)) {
            addReference(sourceId,role->_baseclass,"role",element);
         }
      }
      addDiagramEdges();
   }

private:
   SemanticSnapshot &snapshot_;
   const std::set<std::string> &diagramRootFiles_;
   std::map<metamodel::MetaElement *,std::string> ids_;
   std::vector<metamodel::MetaElement *> ordered_;
   std::set<std::string> diagramNodeIds_;

   metamodel::MetaElement *typeTarget(metamodel::Type *type) const
   {
      if (type == nullptr) return nullptr;
      if (!id(type).empty()) return type;
      if (type->Super != nullptr && !id(type->Super).empty()) return type->Super;
      if (auto *related = dynamic_cast<metamodel::TypeRelatedType *>(type)) {
         if (related->BaseType != nullptr && !id(related->BaseType).empty())
            return related->BaseType;
      }
      if (auto *related = dynamic_cast<metamodel::ClassRelatedType *>(type))
         return related->_baseclass;
      return nullptr;
   }

   std::string id(metamodel::MetaElement *element) const
   {
      auto found = ids_.find(element);
      return found == ids_.end() ? "" : found->second;
   }

   void addDiagramNode(DiagramNode node)
   {
      diagramNodeIds_.insert(node.id);
      snapshot_.diagram.nodes.push_back(std::move(node));
   }

   void addElement(metamodel::MetaElement *element,const std::string &prefix,
      const std::string &containerId,int level,bool diagramVisible)
   {
      if (element == nullptr || ids_.find(element) != ids_.end()) return;
      const std::string qualifiedName = join(prefix,element->Name.empty() ? element->getClass() : element->Name);
      const std::string kind = semanticKind(element);
      const std::string symbolId = kind + ":" + qualifiedName;
      ids_[element] = symbolId;
      ordered_.push_back(element);
      snapshot_.symbols.push_back({symbolId,element->Name,qualifiedName,kind,containerId,
         element->_source,element->_selectionSource,isAbstract(element)});
      snapshot_.documentation.sections.push_back({symbolId,
         element->Name.empty() ? element->getClass() : element->Name,
         kind,documentation(element),level});

      if (diagramVisible && dynamic_cast<metamodel::Model *>(element) != nullptr) {
         addDiagramNode({symbolId,containerId,element->Name,"model",isAbstract(element),
            element->_source,{},{}});
      }
      else if (diagramVisible && dynamic_cast<metamodel::SubModel *>(element) != nullptr) {
         addDiagramNode({symbolId,containerId,element->Name,"topic",isAbstract(element),
            element->_source,{},{}});
      }
      else if (auto *viewable = dynamic_cast<metamodel::Class *>(element);
               diagramVisible && viewable != nullptr) {
         if (viewable->Kind != metamodel::Class::Association) {
            DiagramNode node{symbolId,containerId,element->Name,classKind(*viewable),
               isAbstract(element),element->_source,{},{}};
            for (auto *attribute : viewable->ClassAttribute) {
               if (attribute == nullptr) continue;
               const std::string type = diagramTypeName(attribute->Type);
               node.members.push_back({attribute->Name,type,false});
            }
            addDiagramNode(std::move(node));
         }
      }
      else if (diagramVisible) {
         metamodel::EnumType *enumeration = dynamic_cast<metamodel::EnumType *>(element);
         if (enumeration == nullptr) {
            auto *tree = dynamic_cast<metamodel::EnumTreeValueType *>(element);
            enumeration = tree != nullptr ? tree->ET : nullptr;
         }
         if (enumeration != nullptr) {
            DiagramNode node{symbolId,containerId,element->Name,"enumeration",false,
               element->_source,{},{}};
            enumValues(enumeration,node.enumValues);
            addDiagramNode(std::move(node));
         }
      }

      const std::string childContainer =
         dynamic_cast<metamodel::Package *>(element) != nullptr ? symbolId : containerId;
      if (auto *package = dynamic_cast<metamodel::Package *>(element)) {
         for (auto *child : package->Element)
            addElement(child,qualifiedName,childContainer,level + 1,diagramVisible);
      }
      if (auto *viewable = dynamic_cast<metamodel::Class *>(element)) {
         for (auto *attribute : viewable->ClassAttribute)
            addElement(attribute,qualifiedName,symbolId,level + 1,diagramVisible);
         for (auto *parameter : viewable->ClassParameter)
            addElement(parameter,qualifiedName,symbolId,level + 1,diagramVisible);
         for (auto *role : viewable->Role)
            addElement(role,qualifiedName,symbolId,level + 1,diagramVisible);
         for (auto *constraint : viewable->Constraint)
            addElement(constraint,qualifiedName,symbolId,level + 1,diagramVisible);
      }
   }

   void addReference(const std::string &sourceId,metamodel::MetaElement *target,
      const std::string &kind,const metamodel::MMObject *source)
   {
      const std::string targetId = id(target);
      if (sourceId.empty() || targetId.empty()) return;
      SourceRange range;
      if (source != nullptr) {
         auto found = source->_referenceSources.find(kind);
         if (found != source->_referenceSources.end()) range = found->second;
      }
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
   const CompilationRequest &request,const CompilationResult &compilation,
   const std::vector<std::string> &compilationSourceUris,
   std::vector<SyntaxSnapshot> *syntaxSnapshots)
{
   SemanticSnapshot snapshot;
   snapshot.success = compilation.success;
   snapshot.cancelled = compilation.cancelled;
   snapshot.roots = request.roots;
   snapshot.diagnostics = compilation.diagnostics;
   snapshot.logs = compilation.logs;
   snapshot.missingModels = compilation.missingModels;
   std::set<std::string> reachableUris(request.roots.begin(),request.roots.end());
   reachableUris.insert(compilationSourceUris.begin(),compilationSourceUris.end());
   for (const auto &model : compilation.models)
      if (sources.get(model.uri) != nullptr) reachableUris.insert(model.uri);
   for (const auto &uri : reachableUris) {
      if (const SourceBuffer *source = sources.get(uri))
         snapshot.documentVersions[uri] = source->version;
   }

   std::map<std::string,std::string> modelUris;
   std::map<std::string,std::string> uriModels;
   for (const auto &model : compilation.models) modelUris[model.name] = model.uri;
   for (const auto &model : compilation.models) uriModels[model.uri] = model.name;
   std::map<std::string,SyntaxSnapshot> syntaxByUri;
   for (const auto &uri : reachableUris) {
      if (sources.get(uri) == nullptr) continue;
      SyntaxSnapshot syntax = parseSyntax(sources,uri);
      for (const auto &reference : syntax.importReferences) {
         auto found = modelUris.find(reference.model);
         if (found != modelUris.end())
            snapshot.dependencies.push_back({uri,found->second,reference.model,reference.range});
      }
      syntaxByUri.emplace(uri,std::move(syntax));
   }

   if (compilation.success) {
      const std::set<std::string> diagramRootFiles(request.roots.begin(),request.roots.end());
      SnapshotBuilder builder(snapshot,diagramRootFiles);
      for (auto *model : metamodel::get_all_models()) builder.addModel(model);
      builder.finishReferences();

      std::map<std::string,const SemanticSymbol *> modelSymbols;
      for (const auto &symbol : snapshot.symbols)
         if (symbol.kind == "model") modelSymbols[symbol.name] = &symbol;
      for (const auto &dependency : snapshot.dependencies) {
         auto sourceName = uriModels.find(dependency.sourceUri);
         auto target = modelSymbols.find(dependency.model);
         if (sourceName == uriModels.end() || target == modelSymbols.end()) continue;
         auto source = modelSymbols.find(sourceName->second);
         if (source == modelSymbols.end()) continue;
         snapshot.references.push_back({source->second->id,target->second->id,
            "import",dependency.range});
      }
      for (const auto &entry : syntaxByUri) {
         auto sourceName = uriModels.find(entry.first);
         if (sourceName == uriModels.end()) continue;
         auto source = modelSymbols.find(sourceName->second);
         if (source == modelSymbols.end()) continue;
         std::vector<SyntaxToken> tokens;
         for (const auto &token : entry.second.tokens)
            if (token.channel == 0) tokens.push_back(token);
         for (std::size_t index = 0; index + 2 < tokens.size(); ++index) {
            if (tokens[index + 1].text != "." || tokens[index + 2].kind != "NAME") continue;
            auto target = modelSymbols.find(tokens[index].text);
            if (target == modelSymbols.end()) continue;
            snapshot.references.push_back({source->second->id,target->second->id,
               "qualifier",tokens[index].range});
         }
      }
   }
   if (syntaxSnapshots != nullptr) {
      syntaxSnapshots->reserve(syntaxByUri.size());
      for (auto &entry : syntaxByUri)
         syntaxSnapshots->push_back(std::move(entry.second));
   }
   return snapshot;
}

} // namespace ilic
