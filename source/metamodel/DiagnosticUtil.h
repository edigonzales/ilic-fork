#pragma once

#include <string>
#include <utility>
#include <vector>

#include "MetaModel.h"

namespace metamodel {

inline ilic::SourceRange diagnostic_range(const MMObject *object)
{
   if (object == nullptr) {
      return {};
   }
   if (object->_selectionSource.valid) {
      return object->_selectionSource;
   }
   return object->_source;
}

inline ilic::SourceRange diagnostic_reference_range(
   const MMObject *object,
   const std::string &kind
)
{
   if (object == nullptr) {
      return {};
   }
   const auto found = object->_referenceSources.find(kind);
   if (found != object->_referenceSources.end() && found->second.valid) {
      return found->second;
   }
   return diagnostic_range(object);
}

inline MetaElement *diagnostic_owner(MetaElement *element)
{
   if (auto node = dynamic_cast<EnumNode *>(element)) {
      EnumNode *root = node;
      while (root->ParentNode != nullptr) {
         root = root->ParentNode;
      }
      if (root->EnumType != nullptr) {
         element = root->EnumType;
      }
   }
   if (auto type = dynamic_cast<Type *>(element); type != nullptr && type->_attr != nullptr) {
      return type->_attr;
   }
   return element;
}

inline std::vector<ilic::RelatedInformation> related_information(
   MetaElement *element,
   std::string message
)
{
   MetaElement *owner = diagnostic_owner(element);
   ilic::SourceRange range = diagnostic_range(owner);
   if (!range.valid) {
      return {};
   }
   return {{std::move(range),std::move(message)}};
}

} // namespace metamodel
