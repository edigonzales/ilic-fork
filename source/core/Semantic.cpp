#include "../../include/ilic/Semantic.h"

#include "../metamodel/MetaModel.h"
#include "../metamodel/MetaModelInput.h"
#include "../metamodel/SemanticChecker.h"

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

void documentationInlineEnumValues(metamodel::EnumNode *node,
   const std::string &prefix,std::vector<std::string> &values)
{
   if (node == nullptr) return;
   for (auto *child : node->Node) {
      if (child == nullptr) continue;
      const std::string value = join(prefix,child->Name);
      if (child->Node.empty() && !value.empty()) values.push_back(value);
      documentationInlineEnumValues(child,value,values);
   }
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
   if (value.Min < 0 && value.Max < 0) return "*";
   const std::string lower = value.Min < 0 ? "0" : std::to_string(value.Min);
   const std::string upper = value.Max < 0 ? "*" : std::to_string(value.Max);
   if (value.Min >= 0 && value.Max >= 0 && value.Min == value.Max) return lower;
   return lower + ".." + upper;
}

std::string diagramTypeName(metamodel::Type *type)
{
   if (type == nullptr) return "Unknown";
   if (auto *multi = dynamic_cast<metamodel::MultiValue *>(type))
      return diagramTypeName(multi->BaseType);
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
   if (dynamic_cast<metamodel::BlackboxType *>(type) != nullptr) return "BLACKBOX";
   return type->getClass();
}

std::vector<std::string> stereotypes(metamodel::MetaElement *element,
   const std::string &kind)
{
   std::vector<std::string> result;
   if (isAbstract(element)) result.push_back("Abstract");
   if (kind == "structure") result.push_back("Structure");
   else if (kind == "view") result.push_back("View");
   else if (kind == "enumeration") result.push_back("Enumeration");
   else if (kind == "function") result.push_back("Function");
   return result;
}

std::string containingModelName(metamodel::MetaElement *element)
{
   for (auto *current = element; current != nullptr; current = current->ElementInPackage) {
      if (auto *model = dynamic_cast<metamodel::Model *>(current))
         return model->Name;
   }
   return "";
}

void inlineEnumerationValues(metamodel::Type *type,
   std::vector<std::string> &values)
{
   if (type == nullptr) return;
   if (auto *multi = dynamic_cast<metamodel::MultiValue *>(type)) {
      inlineEnumerationValues(multi->BaseType,values);
      return;
   }
   if (!type->Name.empty() && type->Name != "Type" &&
       type->Name != "TYPE") return;
   if (auto *enumeration = dynamic_cast<metamodel::EnumType *>(type)) {
      documentationInlineEnumValues(enumeration->TopNode,"",values);
      return;
   }
   if (auto *tree = dynamic_cast<metamodel::EnumTreeValueType *>(type)) {
      documentationInlineEnumValues(tree->ET == nullptr ? nullptr : tree->ET->TopNode,
         "",values);
   }
}

std::string documentationTypeName(metamodel::Type *type)
{
   if (type == nullptr) return "<Unknown>";
   if (auto *multi = dynamic_cast<metamodel::MultiValue *>(type))
      return documentationTypeName(multi->BaseType);
   if (!type->Name.empty() && type->Name != "Type" && type->Name != "TYPE")
      return type->Name;
   if (auto *object = dynamic_cast<metamodel::ObjectType *>(type)) {
      (void)object;
      return "ObjectType";
   }
   if (auto *related = dynamic_cast<metamodel::ClassRelatedType *>(type)) {
      if (related->_baseclass != nullptr && !related->_baseclass->Name.empty())
         return related->_baseclass->Name;
      return dynamic_cast<metamodel::ReferenceType *>(type) != nullptr ?
         "Reference" : "Composition";
   }
   if (dynamic_cast<metamodel::EnumType *>(type) != nullptr ||
       dynamic_cast<metamodel::EnumTreeValueType *>(type) != nullptr)
      return "Enumeration";
   if (dynamic_cast<metamodel::BooleanType *>(type) != nullptr)
      return "BOOLEAN";
   if (auto *text = dynamic_cast<metamodel::TextType *>(type)) {
      switch (text->Kind) {
         case metamodel::TextType::MText: return "MTEXT";
         case metamodel::TextType::NameVal: return "NAME";
         case metamodel::TextType::Uri: return "URI";
         case metamodel::TextType::Text: return "Text";
      }
   }
   if (dynamic_cast<metamodel::NumType *>(type) != nullptr)
      return "Numeric";
   if (auto *coord = dynamic_cast<metamodel::CoordType *>(type))
      return std::string(coord->Multi ? "MultiCoord" : "Coord") +
         std::to_string(coord->Axis.size());
   if (auto *line = dynamic_cast<metamodel::LineType *>(type)) {
      switch (line->Kind) {
         case metamodel::LineType::Polyline: return "Polyline";
         case metamodel::LineType::DirectedPolyline: return "DirectedPolyline";
         case metamodel::LineType::Surface: return "Surface";
         case metamodel::LineType::Area: return "Area";
         case metamodel::LineType::MultiPolyline: return "MultiPolyline";
         case metamodel::LineType::DirectedMultiPolyline: return "DirectedMultiPolyline";
         case metamodel::LineType::MultiSurface: return "MultiSurface";
         case metamodel::LineType::MultiArea: return "MultiArea";
      }
   }
   if (dynamic_cast<metamodel::AnyOIDType *>(type) != nullptr)
      return "ANY OID";
   if (dynamic_cast<metamodel::AttributeRefType *>(type) != nullptr)
      return "ATTRIBUTE";
   if (dynamic_cast<metamodel::BlackboxType *>(type) != nullptr)
      return "BLACKBOX";
   return type->getClass();
}

bool isObjectType(metamodel::Type *type)
{
   if (auto *multi = dynamic_cast<metamodel::MultiValue *>(type))
      return isObjectType(multi->BaseType);
   return dynamic_cast<metamodel::ObjectType *>(type) != nullptr;
}

void appendDocumentationEnumEntries(
   std::vector<DocumentationEnumerationEntry> &entries,
   metamodel::EnumNode *node,const std::string &prefix,bool includeIntermediate)
{
   if (node == nullptr) return;
   for (auto *child : node->Node) {
      if (child == nullptr) continue;
      const std::string value = join(prefix,child->Name);
      const bool hasChildren = !child->Node.empty();
      if (!hasChildren || includeIntermediate)
         entries.push_back({value,documentation(child)});
      appendDocumentationEnumEntries(entries,child,value,includeIntermediate);
   }
}

metamodel::EnumType *documentationEnumerationType(metamodel::MetaElement *element)
{
   if (auto *enumeration = dynamic_cast<metamodel::EnumType *>(element))
      return enumeration;
   if (auto *tree = dynamic_cast<metamodel::EnumTreeValueType *>(element))
      return tree->ET;
   return nullptr;
}

DocumentationEnumeration documentationEnumeration(metamodel::MetaElement *element)
{
   DocumentationEnumeration result;
   if (element == nullptr) return result;
   result.name = element->Name;
   result.documentation = documentation(element);
   auto *enumeration = documentationEnumerationType(element);
   if (enumeration != nullptr) {
      const bool includeIntermediate =
         dynamic_cast<metamodel::EnumTreeValueType *>(element) != nullptr;
      appendDocumentationEnumEntries(result.entries,enumeration->TopNode,"",
         includeIntermediate);
   }
   return result;
}

DocumentationViewable documentationViewable(metamodel::Class *viewable,
   metamodel::Package *scope)
{
   DocumentationViewable result;
   if (viewable == nullptr) return result;
   result.name = viewable->Name;
   result.kind = classKind(*viewable);
   result.isAbstract = isAbstract(viewable);
   result.documentation = documentation(viewable);

   for (auto *attribute : viewable->ClassAttribute) {
      if (attribute == nullptr || isObjectType(attribute->Type)) continue;
      DocumentationRow row;
      row.name = attribute->Name;
      row.cardinality = cardinality(metamodel::attributeCardinality(attribute->Type));
      row.type = documentationTypeName(attribute->Type);
      row.description = documentation(attribute);
      std::vector<std::string> inlineValues;
      inlineEnumerationValues(attribute->Type,inlineValues);
      if (!inlineValues.empty()) {
         row.description.clear();
         for (std::size_t index = 0; index < inlineValues.size(); ++index) {
            if (index != 0) row.description += ", ";
            row.description += inlineValues[index];
         }
      }
      result.rows.push_back(std::move(row));
   }

   std::sort(result.rows.begin(), result.rows.end(), [](const auto &left, const auto &right) {
      return left.name < right.name;
   });

   if (viewable->Kind == metamodel::Class::ClassVal && scope != nullptr) {
      for (auto *element : scope->Element) {
         auto *association = dynamic_cast<metamodel::Class *>(element);
         if (association == nullptr ||
             association->Kind != metamodel::Class::Association ||
             association->Role.size() != 2) continue;
         auto role = association->Role.begin();
         auto *left = *role++;
         auto *right = *role;
         if (left == nullptr || right == nullptr) continue;
         auto addRole = [&](metamodel::Role *me,metamodel::Role *other) {
            if (me == nullptr || other == nullptr || me->_baseclass != viewable ||
                other->_baseclass == nullptr) return;
            result.rows.push_back({me->Name.empty() ? "role" : me->Name,
               cardinality(metamodel::effectiveRoleCardinality(me)),
               other->_baseclass->Name,""});
         };
         addRole(left,right);
         addRole(right,left);
      }
   }
   return result;
}

std::vector<DocumentationViewable> documentationViewables(metamodel::Package *scope)
{
   std::vector<DocumentationViewable> result;
   if (scope == nullptr) return result;
   for (auto *element : scope->Element) {
      auto *viewable = dynamic_cast<metamodel::Class *>(element);
      if (viewable == nullptr || viewable->Kind == metamodel::Class::Association)
         continue;
      result.push_back(documentationViewable(viewable,scope));
   }
   std::sort(result.begin(),result.end(),[](const auto &left,const auto &right) {
      return left.name < right.name;
   });
   return result;
}

std::vector<DocumentationEnumeration> documentationEnumerations(metamodel::Package *scope)
{
   std::vector<DocumentationEnumeration> result;
   if (scope == nullptr) return result;
   for (auto *element : scope->Element) {
      if (documentationEnumerationType(element) != nullptr)
         result.push_back(documentationEnumeration(element));
   }
   std::sort(result.begin(),result.end(),[](const auto &left,const auto &right) {
      return left.name < right.name;
   });
   return result;
}

DocumentationModel documentationModel(metamodel::Model *model)
{
   DocumentationModel result;
   if (model == nullptr) return result;
   result.name = model->Name;
   result.uri = model->_ilifile;
   for (auto *attribute : model->MetaAttribute) {
      if (attribute == nullptr) continue;
      if (attribute->Name == "title") result.title = attribute->Value;
      else if (attribute->Name == "shortDescription")
         result.shortDescription = attribute->Value;
   }
   result.viewables = documentationViewables(model);
   result.enumerations = documentationEnumerations(model);
   for (auto *element : model->Element) {
      auto *topic = dynamic_cast<metamodel::SubModel *>(element);
      if (topic == nullptr) continue;
      DocumentationTopic topicResult;
      topicResult.name = topic->Name;
      topicResult.documentation = documentation(topic);
      topicResult.viewables = documentationViewables(topic);
      topicResult.enumerations = documentationEnumerations(topic);
      result.topics.push_back(std::move(topicResult));
   }
   std::sort(result.topics.begin(),result.topics.end(),[](const auto &left,const auto &right) {
      return left.name < right.name;
   });
   return result;
}

class SnapshotBuilder {
public:
   SnapshotBuilder(SemanticSnapshot &snapshot,
      const std::set<std::string> &diagramRootFiles)
      : snapshot_(snapshot),diagramRootFiles_(diagramRootFiles) {}

   void addModel(metamodel::Model *model)
   {
      if (model == nullptr) return;
      const bool documentationRoot = diagramRootFiles_.count(model->_ilifile) != 0;
      if (snapshot_.documentation.title.empty() && documentationRoot)
         snapshot_.documentation.title = model->Name;
      if (documentationRoot)
         snapshot_.documentation.models.push_back(documentationModel(model));
      addElement(model,"","",1,diagramRootFiles_.count(model->_ilifile) != 0,"");
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
      std::sort(snapshot_.documentation.models.begin(),
         snapshot_.documentation.models.end(),[](const auto &left,const auto &right) {
            return left.name < right.name;
         });
   }

private:
   SemanticSnapshot &snapshot_;
   const std::set<std::string> &diagramRootFiles_;
   std::map<metamodel::MetaElement *,std::string> ids_;
   std::vector<metamodel::MetaElement *> ordered_;
   std::set<std::string> diagramNodeIds_;
   std::set<metamodel::MetaElement *> diagramVisibleElements_;
   bool modelScopeAdded_ = false;
   const std::string modelScopeId_ = "diagram:model-scope";

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

   void ensureModelScope()
   {
      if (modelScopeAdded_) return;
      modelScopeAdded_ = true;
      addDiagramNode({modelScopeId_,"","Model Scope","modelScope",false,
         {},{}, {},{},{}});
   }

   void addDiagramMember(DiagramNode &node,metamodel::AttrOrParam *attribute,
      bool inherited,const std::string &declaringType)
   {
      if (attribute == nullptr) return;
      DiagramMember member;
      member.name = attribute->Name;
      member.type = diagramTypeName(attribute->Type);
      member.cardinality =
         cardinality(metamodel::attributeCardinality(attribute->Type));
      member.declaringType = declaringType;
      member.inherited = inherited;
      inlineEnumerationValues(attribute->Type,member.inlineEnumValues);
      node.members.push_back(std::move(member));
   }

   void addViewableMembers(DiagramNode &node,metamodel::Class *viewable)
   {
      for (auto *attribute : viewable->ClassAttribute)
         addDiagramMember(node,attribute,false,"");

      std::set<metamodel::Class *> visited;
      for (auto *base = dynamic_cast<metamodel::Class *>(viewable->Super);
           base != nullptr && visited.insert(base).second;
           base = dynamic_cast<metamodel::Class *>(base->Super)) {
         for (auto *attribute : base->ClassAttribute)
            addDiagramMember(node,attribute,true,base->Name);
      }

      std::set<metamodel::Constraint *> constraints;
      int unnamed = 1;
      auto addConstraint = [&](metamodel::Constraint *constraint) {
         if (constraint == nullptr || !constraints.insert(constraint).second) return;
         std::string name = constraint->Name;
         if (name.empty()) name = "constraint" + std::to_string(unnamed++);
         node.operations.push_back(name + "()");
      };
      for (auto *constraint : viewable->Constraints) addConstraint(constraint);
      for (auto *constraint : viewable->Constraint) addConstraint(constraint);
   }

   metamodel::EnumType *enumerationType(metamodel::MetaElement *element) const
   {
      if (auto *enumeration = dynamic_cast<metamodel::EnumType *>(element))
         return enumeration;
      auto *tree = dynamic_cast<metamodel::EnumTreeValueType *>(element);
      return tree != nullptr ? tree->ET : nullptr;
   }

   void addElement(metamodel::MetaElement *element,const std::string &prefix,
      const std::string &containerId,int level,bool diagramVisible,
      const std::string &diagramContainerId)
   {
      if (element == nullptr || ids_.find(element) != ids_.end()) return;
      const std::string qualifiedName = join(prefix,element->Name.empty() ? element->getClass() : element->Name);
      const std::string kind = semanticKind(element);
      const std::string symbolId = kind + ":" + qualifiedName;
      ids_[element] = symbolId;
      ordered_.push_back(element);
      snapshot_.symbols.push_back({symbolId,element->Name,qualifiedName,kind,containerId,
         element->_source,element->_selectionSource,element->_endSelectionSource,
         isAbstract(element)});
      snapshot_.documentation.sections.push_back({symbolId,
         element->Name.empty() ? element->getClass() : element->Name,
         kind,documentation(element),level});

      if (diagramVisible) diagramVisibleElements_.insert(element);
      if (diagramVisible && dynamic_cast<metamodel::Model *>(element) != nullptr) {
         ensureModelScope();
      }
      else if (diagramVisible && dynamic_cast<metamodel::SubModel *>(element) != nullptr) {
         const std::string modelName = containingModelName(element);
         const std::string label = modelName.empty() ? element->Name :
            element->Name + " (" + modelName + ")";
         addDiagramNode({symbolId,"",label,"topic",isAbstract(element),
            element->_source,stereotypes(element,"topic"),{},{},{}});
      }
      else if (auto *viewable = dynamic_cast<metamodel::Class *>(element);
               diagramVisible && viewable != nullptr) {
         if (viewable->Kind != metamodel::Class::Association) {
            const std::string nodeKind = classKind(*viewable);
            DiagramNode node{symbolId,diagramContainerId,element->Name,nodeKind,
               isAbstract(element),element->_source,stereotypes(element,nodeKind),
               {},{},{}};
            addViewableMembers(node,viewable);
            addDiagramNode(std::move(node));
         }
      }
      else if (diagramVisible &&
               dynamic_cast<metamodel::FunctionDef *>(element) != nullptr) {
         addDiagramNode({symbolId,diagramContainerId,element->Name,"function",false,
            element->_source,stereotypes(element,"function"),{},{},{}});
      }
      else if (diagramVisible) {
         metamodel::EnumType *enumeration = enumerationType(element);
         if (enumeration != nullptr) {
            DiagramNode node{symbolId,diagramContainerId,element->Name,
               "enumeration",isAbstract(element),element->_source,
               stereotypes(element,"enumeration"),{},{},{}};
            enumValues(enumeration,node.enumValues);
            addDiagramNode(std::move(node));
         }
      }

      const std::string childContainer =
         dynamic_cast<metamodel::Package *>(element) != nullptr ? symbolId : containerId;
      std::string childDiagramContainer = diagramContainerId;
      if (dynamic_cast<metamodel::Model *>(element) != nullptr && diagramVisible)
         childDiagramContainer = modelScopeId_;
      else if (dynamic_cast<metamodel::SubModel *>(element) != nullptr && diagramVisible)
         childDiagramContainer = symbolId;
      if (auto *package = dynamic_cast<metamodel::Package *>(element)) {
         for (auto *child : package->Element)
            addElement(child,qualifiedName,childContainer,level + 1,
               diagramVisible,childDiagramContainer);
      }
      if (auto *viewable = dynamic_cast<metamodel::Class *>(element)) {
         for (auto *attribute : viewable->ClassAttribute)
            addElement(attribute,qualifiedName,symbolId,level + 1,
               diagramVisible,diagramContainerId);
         for (auto *parameter : viewable->ClassParameter)
            addElement(parameter,qualifiedName,symbolId,level + 1,
               diagramVisible,diagramContainerId);
         for (auto *role : viewable->Role)
            addElement(role,qualifiedName,symbolId,level + 1,
               diagramVisible,diagramContainerId);
         std::set<metamodel::Constraint *> constraints;
         for (auto *constraint : viewable->Constraints)
            if (constraint != nullptr && constraints.insert(constraint).second)
               addElement(constraint,qualifiedName,symbolId,level + 1,
                  diagramVisible,diagramContainerId);
         for (auto *constraint : viewable->Constraint)
            if (constraint != nullptr && constraints.insert(constraint).second)
               addElement(constraint,qualifiedName,symbolId,level + 1,
                  diagramVisible,diagramContainerId);
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
      auto ensureExternal = [&](metamodel::Class *viewable) -> std::string {
         if (viewable == nullptr) return "";
         const std::string symbolId = id(viewable);
         if (symbolId.empty()) return "";
         if (diagramNodeIds_.count(symbolId) != 0) return symbolId;
         ensureModelScope();
         auto externalStereotypes = stereotypes(viewable,classKind(*viewable));
         externalStereotypes.push_back("External");
         addDiagramNode({symbolId,modelScopeId_,viewable->Name,"external",
            isAbstract(viewable),viewable->_source,
            std::move(externalStereotypes),{},{},{}});
         return symbolId;
      };
      for (auto *element : ordered_) {
         auto *viewable = dynamic_cast<metamodel::Class *>(element);
         if (viewable == nullptr ||
             diagramVisibleElements_.count(element) == 0) continue;
         const std::string sourceId = id(element);
         if (viewable->Super != nullptr && diagramNodeIds_.count(sourceId) != 0) {
            auto *parent = dynamic_cast<metamodel::Class *>(viewable->Super);
            const std::string targetId = ensureExternal(parent);
            if (!targetId.empty()) {
               snapshot_.diagram.edges.push_back({"inheritance:" + sourceId + "->" +
                  targetId,sourceId,targetId,"inheritance","","","",""});
            }
         }
         if (viewable->Kind != metamodel::Class::Association ||
             viewable->Role.size() != 2) continue;
         auto iterator = viewable->Role.begin();
         metamodel::Role *left = *iterator++;
         metamodel::Role *right = *iterator;
         if (left == nullptr || right == nullptr) continue;
         const std::string leftId = ensureExternal(left->_baseclass);
         const std::string rightId = ensureExternal(right->_baseclass);
         if (leftId.empty() || rightId.empty()) continue;
         const std::string leftCardinality =
            cardinality(metamodel::effectiveRoleCardinality(left));
         const std::string rightCardinality =
            cardinality(metamodel::effectiveRoleCardinality(right));
         const std::string leftName = left->Name.empty() ? "role" : left->Name;
         const std::string rightName = right->Name.empty() ? "role" : right->Name;
         const std::string label = leftName + "\xE2\x80\x93" + rightName;
         snapshot_.diagram.edges.push_back({"association:" + sourceId + ":" +
            leftId + "->" + rightId,leftId,rightId,"association",label,
            leftName + " " + leftCardinality + " / " + rightName + " " +
               rightCardinality,leftCardinality,rightCardinality});
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
