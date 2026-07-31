#include "MetaModelBuilder.h"
#include "DiagnosticUtil.h"
#include "../../include/ilic/SourceManager.h"
#include "../util/StringUtil.h"
#include "../util/Logger.h"

#include <algorithm>
#include <sstream>
#include <cctype>

using namespace util;

namespace metamodel {

   MetaModelBuilder::MetaModelBuilder(MetaModelStore &store,util::Logger &logger)
      : store_(store), logger_(logger)
   {
   }

   void MetaModelBuilder::reset()
   {
      context_.clear();
      currentSourceUri_.clear();
      currentSourceText_.clear();
      pendingMetaAttributes_.clear();
      pendingDocumentation_.clear();
      languageVersion_ = IliLanguageVersion::Ili23;
   }

   MMObject *MetaModelBuilder::current() const noexcept
   {
      return context_.empty() ? nullptr : context_.back();
   }

   Class *MetaModelBuilder::currentClass() const noexcept
   {
      for (auto it = context_.rbegin(); it != context_.rend(); ++it) {
         if (auto *value = dynamic_cast<Class *>(*it)) return value;
      }
      return nullptr;
   }

   Package *MetaModelBuilder::currentPackage() const noexcept
   {
      for (auto it = context_.rbegin(); it != context_.rend(); ++it) {
         if (auto *value = dynamic_cast<Package *>(*it)) return value;
      }
      return nullptr;
   }

   SubModel *MetaModelBuilder::currentTopic() const noexcept
   {
      for (auto it = context_.rbegin(); it != context_.rend(); ++it) {
         if (auto *value = dynamic_cast<SubModel *>(*it)) return value;
      }
      return nullptr;
   }

   Model *MetaModelBuilder::currentModel() const noexcept
   {
      for (auto it = context_.rbegin(); it != context_.rend(); ++it) {
         if (auto *value = dynamic_cast<Model *>(*it)) return value;
      }
      return nullptr;
   }

   void MetaModelBuilder::pushContext(MetaElement &element)
   {
      context_.push_back(&element);
   }

   void MetaModelBuilder::popContext(MetaElement &element) noexcept
   {
      if (!context_.empty() && context_.back() == &element) context_.pop_back();
   }

   void MetaModelBuilder::popContext() noexcept
   {
      if (!context_.empty()) context_.pop_back();
   }

   MetaModelBuilder::ContextScope MetaModelBuilder::enterContext(MetaElement &element)
   {
      return ContextScope(*this,element);
   }

   MetaModelBuilder::ContextScope::ContextScope(MetaModelBuilder &builder,
      MetaElement &element) : builder_(&builder), expected_(&element)
   {
      builder_->pushContext(element);
   }

   MetaModelBuilder::ContextScope::~ContextScope() noexcept
   {
      if (builder_ != nullptr && expected_ != nullptr) builder_->popContext(*expected_);
   }

   MetaModelBuilder::ContextScope::ContextScope(ContextScope &&other) noexcept
      : builder_(other.builder_), expected_(other.expected_)
   {
      other.builder_ = nullptr;
      other.expected_ = nullptr;
   }

   MetaModelBuilder::SourceScope MetaModelBuilder::enterSource(
      const ilic::SourceBuffer &source)
   {
      return SourceScope(*this,source);
   }

   MetaModelBuilder::SourceScope::SourceScope(MetaModelBuilder &builder,
      const ilic::SourceBuffer &source) : builder_(&builder),
      previousUri_(builder.currentSourceUri_), previousText_(builder.currentSourceText_),
      previousMetaAttributes_(builder.pendingMetaAttributes_),
      previousDocumentation_(builder.pendingDocumentation_),
      loggerSourceScope_(builder.logger_,source.uri)
   {
      builder_->currentSourceUri_ = source.uri;
      builder_->currentSourceText_ = source.text;
      builder_->prepareMetaAttributes(source.text);
   }

   MetaModelBuilder::SourceScope::~SourceScope() noexcept
   {
      if (builder_ == nullptr) return;
      builder_->currentSourceUri_ = previousUri_;
      builder_->currentSourceText_ = previousText_;
      builder_->pendingMetaAttributes_ = std::move(previousMetaAttributes_);
      builder_->pendingDocumentation_ = std::move(previousDocumentation_);
   }

   MetaModelBuilder::SourceScope::SourceScope(SourceScope &&other) noexcept
      : builder_(other.builder_), previousUri_(std::move(other.previousUri_)),
      previousText_(std::move(other.previousText_)),
      previousMetaAttributes_(std::move(other.previousMetaAttributes_)),
      previousDocumentation_(std::move(other.previousDocumentation_)),
      loggerSourceScope_(std::move(other.loggerSourceScope_))
   {
      other.builder_ = nullptr;
   }

   std::vector<std::string> MetaModelBuilder::unqualifiedImports(
      const std::string &modelName) const
   {
      std::vector<std::string> result;
      for (Import *import : store_.imports()) {
         if (import != nullptr && import->ImportingP != nullptr &&
             import->ImportedP != nullptr && import->_unqualified &&
             import->ImportingP->Name == modelName) {
            result.push_back(import->ImportedP->Name);
         }
      }
      return result;
   }

   bool MetaModelBuilder::dependsOn(Package *package) const
   {
      if (package == nullptr || package->getClass() != "SubModel") return true;
      auto *topic = currentTopic();
      if (topic == nullptr) return true;
      for (auto *candidate = topic; candidate != nullptr;
           candidate = dynamic_cast<SubModel *>(candidate->_super)) {
         if (candidate == package) return true;
      }
      auto *target = static_cast<SubModel *>(package);
      for (Dependency *dependency : store_.dependencies()) {
         if (dependency != nullptr && dependency->Dependent == target->_dataunit &&
             dependency->Using == topic->_dataunit) return true;
      }
      return false;
   }

   void MetaModelBuilder::addModel(Model *model)
   {
      if (model == nullptr) return;
      for (Model *existing : store_.models()) {
         if (existing != nullptr && existing->Name == model->Name) return;
      }
      store_.addModel(*model);
      if (model->Name == "INTERLIS") store_.setInterlisModel(*model);
   }

   void MetaModelBuilder::addImport(Import *import)
   {
      if (import != nullptr) store_.addImport(*import);
   }

   void MetaModelBuilder::addDataUnit(DataUnit *dataUnit)
   {
      if (dataUnit != nullptr) store_.addDataUnit(*dataUnit);
   }

   void MetaModelBuilder::addDependency(Dependency *dependency)
   {
      if (dependency != nullptr) store_.addDependency(*dependency);
   }

   void MetaModelBuilder::addAxisSpec(AxisSpec *axisSpec)
   {
      if (axisSpec != nullptr) store_.addAxisSpec(*axisSpec);
   }

   static string trim_copy(const string &value)
   {
      size_t first = 0;
      while (first < value.size() && isspace(static_cast<unsigned char>(value[first]))) ++first;
      size_t last = value.size();
      while (last > first && isspace(static_cast<unsigned char>(value[last - 1]))) --last;
      return value.substr(first,last - first);
   }

   static string diagnostic_type_kind(Type *type)
   {
      if (auto viewable = dynamic_cast<Class *>(type)) {
         switch (viewable->Kind) {
            case Class::Structure: return "STRUCTURE";
            case Class::ClassVal: return "CLASS";
            case Class::ViewVal: return "VIEW";
            case Class::Association: return "ASSOCIATION";
         }
      }
      return type == nullptr ? "UNKNOWN" : "DOMAIN";
   }

   static string decode_meta_value(string value)
   {
      value = trim_copy(value);
      if (value.size() < 2 || value.front() != '"' || value.back() != '"') return value;
      string decoded;
      for (size_t i = 1; i + 1 < value.size(); ++i) {
         if (value[i] == '\\' && i + 2 < value.size()
             && (value[i + 1] == '\\' || value[i + 1] == '"')) {
            decoded += value[++i];
         }
         else decoded += value[i];
      }
      return decoded;
   }

    void MetaModelBuilder::prepareMetaAttributes(const string &source)
   {
      currentSourceText_ = source;
      pendingMetaAttributes_.clear();
      pendingDocumentation_.clear();

      auto normalize_documentation = [](string value) {
         istringstream lines(value);
         vector<string> normalized;
         string line;
         while (getline(lines,line)) {
            line = trim_copy(line);
            if (!line.empty() && line.front() == '*') {
               line = trim_copy(line.substr(1));
            }
            normalized.push_back(line);
         }
         while (!normalized.empty() && normalized.front().empty())
            normalized.erase(normalized.begin());
         while (!normalized.empty() && normalized.back().empty())
            normalized.pop_back();
         string result;
         for (const auto &line : normalized) {
            if (!result.empty()) result += '\n';
            result += line;
         }
         return result;
      };

      size_t search = 0;
      while ((search = source.find("/**",search)) != string::npos) {
         const size_t contentStart = search + 3;
         const size_t end = source.find("*/",contentStart);
         if (end == string::npos) break;
         const string text = normalize_documentation(
            source.substr(contentStart,end - contentStart));
         size_t next = end + 2;
         int lineNumber = 1;
         for (size_t index = 0; index < next && index < source.size(); ++index)
            if (source[index] == '\n') ++lineNumber;
         while (next < source.size()) {
            const size_t lineEnd = source.find('\n',next);
            const size_t length = lineEnd == string::npos ? source.size() - next :
               lineEnd - next;
            const string line = source.substr(next,length);
            const size_t first = line.find_first_not_of(" \t\r");
            const string trimmed = first == string::npos ? "" : line.substr(first);
            if (trimmed.empty() || trimmed.rfind("!!",0) == 0) {
               if (lineEnd == string::npos) break;
               next = lineEnd + 1;
               ++lineNumber;
               continue;
            }
            break;
         }
         if (!text.empty() && next < source.size())
            pendingDocumentation_[lineNumber] = text;
         search = end + 2;
      }
      vector<PendingMetaAttribute> pending;
      istringstream lines(source);
      string line;
      int lineNumber = 0;
      bool inBlockComment = false;
      while (getline(lines,line)) {
         ++lineNumber;
         size_t first = line.find_first_not_of(" \t\r");
         string trimmed = first == string::npos ? "" : line.substr(first);
         if (inBlockComment) {
            size_t end = trimmed.find("*/");
            if (end == string::npos) continue;
            inBlockComment = false;
            trimmed = trim_copy(trimmed.substr(end + 2));
            if (trimmed.empty()) continue;
         }
         while (trimmed.rfind("/*",0) == 0) {
            size_t end = trimmed.find("*/",2);
            if (end == string::npos) {
               inBlockComment = true;
               trimmed.clear();
               break;
            }
            trimmed = trim_copy(trimmed.substr(end + 2));
         }
         if (trimmed.empty()) continue;
         if (trimmed.rfind("!!@",0) == 0) {
            string option = trim_copy(trimmed.substr(3));
            size_t equals = option.find('=');
            string name = equals == string::npos ? "" : trim_copy(option.substr(0,equals));
            if (equals == string::npos || name.empty()) {
               logger_.error(DiagnosticId::MetaSyntax,
                  "invalid meta attribute; expected !!@ name=value",lineNumber,
                  static_cast<int>(first == string::npos ? 0 : first));
               continue;
            }
            PendingMetaAttribute attribute;
            attribute.name = std::move(name);
            attribute.value = decode_meta_value(option.substr(equals + 1));
            attribute.rawText = trimmed;
            attribute.line = lineNumber;
            attribute.column = static_cast<int>(first == string::npos ? 0 : first);
            pending.push_back(std::move(attribute));
         }
         else if (trimmed.rfind("!!",0) != 0 && !pending.empty()) {
            pendingMetaAttributes_[lineNumber] = pending;
            pending.clear();
         }
      }
      if (!pending.empty()) {
         const PendingMetaAttribute &attribute = pending.front();
         logger_.error(DiagnosticId::MetaDangling,
            "meta attribute is not followed by a model element",attribute.line,
            attribute.column);
      }
   }

   // mmobject helpers

   void MetaModelBuilder::initObject(MMObject* o, int line)
   {
      o->_line = line;
      if (line > 0 && !currentSourceUri_.empty()) {
         o->_source.valid = true;
         o->_source.uri = currentSourceUri_;
         o->_source.start.line = static_cast<size_t>(line - 1);
         o->_source.end = o->_source.start;
         o->_source.end.character = 1;
      }
   }

   size_t MetaModelBuilder::utf8ByteOffset(size_t codepointOffset) const
   {
      size_t byteOffset = 0;
      for (size_t index = 0;
           index < codepointOffset && byteOffset < currentSourceText_.size();++index) {
         const unsigned char lead = static_cast<unsigned char>(currentSourceText_[byteOffset]);
         size_t width = 1;
         if ((lead & 0xe0) == 0xc0) width = 2;
         else if ((lead & 0xf0) == 0xe0) width = 3;
         else if ((lead & 0xf8) == 0xf0) width = 4;
         byteOffset = min(currentSourceText_.size(),byteOffset + width);
      }
      return byteOffset;
   }

   ilic::SourceRange MetaModelBuilder::tokenRange(antlr4::Token *token) const
   {
      ilic::SourceRange result;
      if (token == nullptr || token->getStartIndex() > currentSourceText_.size() || currentSourceUri_.empty()) return result;
      const size_t start = utf8ByteOffset(static_cast<size_t>(token->getStartIndex()));
      const size_t end = utf8ByteOffset(static_cast<size_t>(token->getStopIndex() + 1));
      ilic::SourceManager sources;
      sources.put(currentSourceUri_,currentSourceText_,0);
      const ilic::SourcePosition startPosition = sources.position(currentSourceUri_,start);
      const ilic::SourcePosition endPosition = sources.position(currentSourceUri_,end);
      result.valid = true;
      result.uri = currentSourceUri_;
      result.start = {startPosition.line,startPosition.utf16Column,startPosition.offset};
      result.end = {endPosition.line,endPosition.utf16Column,endPosition.offset};
      return result;
   }

   void MetaModelBuilder::setSelectionSource(MetaElement *element,antlr4::Token *token)
   {
      if (element != nullptr) element->_selectionSource = tokenRange(token);
   }

   void MetaModelBuilder::setEndSelectionSource(MetaElement *element,antlr4::Token *token)
   {
      if (element != nullptr) element->_endSelectionSource = tokenRange(token);
   }

   void MetaModelBuilder::setReferenceSource(MMObject *object,string kind,antlr4::Token *token)
   {
      if (object == nullptr) return;
      const ilic::SourceRange source = tokenRange(token);
      if (source.valid) object->_referenceSources[kind] = source;
   }

   void MetaModelBuilder::setReferenceSource(MMObject *object,string kind,
      antlr4::ParserRuleContext *context)
   {
      setReferenceSource(object,kind,context == nullptr ? nullptr : context->getStop());
   }

   // metaelement helpers

   void MetaModelBuilder::initMetaElement(MetaElement* e, int line)
   {

      // list <DocText> Documentation;
      // ROLE from ASSOCIATION MetaAttributes
      // list <MetaAttribute *> MetaAttribute;

      initObject(e, line);
      auto metadata = pendingMetaAttributes_.find(line);
      if (metadata != pendingMetaAttributes_.end()) {
         for (const auto &entry : metadata->second) {
            MetaAttribute *attribute = store_.make<MetaAttribute>();
            initObject(attribute,entry.line);
            attribute->_source.start.character = static_cast<size_t>(entry.column);
            attribute->_source.end = attribute->_source.start;
            attribute->_source.end.character += entry.rawText.size();
            attribute->Name = entry.name;
            attribute->Value = entry.value;
            attribute->_rawText = entry.rawText;
            attribute->MetaElement = e;
            e->MetaAttribute.push_back(attribute);
         }
      }
      auto documentation = pendingDocumentation_.find(line);
      if (documentation != pendingDocumentation_.end() &&
          !documentation->second.empty()) {
         auto *doc = store_.make<DocText>();
         initObject(doc,line);
         doc->Text = documentation->second;
         e->Documentation.push_back(doc);
      }
      MMObject *ctx = current();
      if (ctx == nullptr) {
         return;
      }

      string context = ctx->getClass();
      if (context == "Model" || context == "SubModel") {
         e->ElementInPackage = static_cast<Package *>(ctx);
         e->ElementInPackage->Element.push_back(e);
      }
      else if (context == "Class") {
         if (e->getClass() == "AttrOrParam") {
            e->ElementInPackage = nullptr;
            AttrOrParam* t = static_cast<AttrOrParam*>(e);
            t->AttrParent = static_cast<Class *>(ctx); // reverse Role, to do !!!
         }
         else if (e->isSubClassOf("Constraint")) {
            e->ElementInPackage = nullptr;
            Constraint* c = static_cast<Constraint*>(e);
            c->ToClass = static_cast<Class *>(ctx); // reverse Role, to do !!!
         }
         else if (e->getClass() == "Role") {
            e->ElementInPackage = nullptr;
            Role* r = static_cast<Role*>(e);
            r->Association = static_cast<Class *>(ctx); // reverse Role, to do !!!
         }
      }
      else if (context == "FunctionDef") {
         e->ElementInPackage = nullptr;
         FunctionDef* f = static_cast<FunctionDef*>(ctx);
         if (e->getClass() == "Argument") {
            Argument *a = static_cast<Argument*>(e);
            a->Function = f;
         }
         else { // ResultType
            Type* t = static_cast<Type*>(e);
            t->LFTParent = f;
         }
      }
      else if (context == "AttrOrParam") {
         e->ElementInPackage = nullptr;
         if (e->isSubClassOf("Type")) {
            Type* t = static_cast<Type*>(e);
            AttrOrParam *a = static_cast<AttrOrParam *>(ctx);
            t->_attr = a;
            a->Type = t; // localtype ???, to do !!!
         }
      }

   }

   void MetaModelBuilder::initExtendable(ExtendableME* e, int line)
   {
      initMetaElement(e, line);
      // bool Abstract;
      // bool Final;
      // ROLE from ASSOCIATION Inheritance
      // ExtendableME *Super;
      // ROLE from ASSOCIATION Inheritance
      // list <ExtendableME *> Sub;
   }

   // package helpers

   void MetaModelBuilder::initPackage(Package* p, int line)
   {
      initMetaElement(p, line);
      // ROLE from ASSOCIATION PackageElements
      // list <MetaElement *> Element;
   }

   // Model helpers

   Model* MetaModelBuilder::findModel(const string &name, int line)
   {

      logger_.debug("findModel " + name);

      for (Model* m : store_.models()) {
         if (m->Name == name) {
            return m;
         }
      }

      logger_.error(DiagnosticId::NameModelNotFound,"model " + name + " not found.",line);
      return nullptr;

   }

   // Topic / DataUnit helpers

   DataUnit* MetaModelBuilder::findDataUnit(const string &name, int line)
   {
      logger_.debug("findDataUnit " + name);
      for (DataUnit* u : store_.dataUnits()) {
         if (get_path(u) == (get_path(currentModel()) + "." + name + ".BASKET")) {
            return u;
         }
         else if (get_path(u) == (name + ".BASKET")) {
            return u;
         }
      }
      logger_.error(DiagnosticId::NameTopicNotFound,"unknown topic " + name,line);
      return nullptr;
   }

   void MetaModelBuilder::addPackage(Package* p)
   {
      if (p == nullptr) {
         return;
      }
      for (auto pp : store_.packages()) {
         if (get_path(pp) == get_path(p)) {
            if (pp->ElementInPackage == nullptr ||
                pp->ElementInPackage != p->ElementInPackage) {
               logger_.error(DiagnosticId::NameMultipleDeclarations,
                  "multiple declarations of " + get_path(p),diagnostic_range(p));
            }
            return;
         }
      }
      store_.addPackage(*p);
   }

   Package* MetaModelBuilder::findPackage(const string &name, int line)
   {
      string package_name = name;
      if (ends_with(package_name,".BASKET")) {
         // DataUnit
         package_name = package_name.substr(0,package_name.length()-7);
      }
      logger_.debug("findPackage " + package_name);
      for (Package* p : store_.packages()) {
         for (auto unqualified : unqualifiedImports(currentModel()->Name)) {
            if (get_path(p) == unqualified + "." + package_name) {
               return p;
            }
         }
      }
      for (Package* p : store_.packages()) {
         if (get_path(p) == package_name) {
            return p;
         }
         else if (p->Name == package_name) {
            return p;
         }
      }
      logger_.error(DiagnosticId::NamePackageNotFound,"unknown package " + package_name,line);
      return nullptr;
   }

   SubModel* MetaModelBuilder::findTopic(const string &name, int line)
   {
      logger_.debug("findTopic " + name);
      Package *p = findPackage(name,line);
      if (p->getClass() == "SubModel") {
         return static_cast<SubModel *>(p);
      }
      else {
         logger_.error(DiagnosticId::ReferenceTopicRequired,name + " is not a topic",0);
         return nullptr;
      }         
   }

   // Unit helpers

   void MetaModelBuilder::addUnit(Unit* u)
   {
      if (u == nullptr) {
         return;
      }
      for (Unit* uu : store_.units()) {
         if (get_path(uu) == get_path(u)) {
            logger_.error(DiagnosticId::NameMultipleDeclarations,
               "multiple declaration of unit " + u->Name,diagnostic_range(u));
            return;
         }
      }
      store_.addUnit(*u);
   }

   Unit* MetaModelBuilder::findUnit(const string &name, int line)
   {
      logger_.debug("findUnit " + name);

      // Qualified unit references identify their declaration directly. The
      // semantic checker separately enforces that the declaring model was
      // imported at this lexical occurrence.
      for (Unit* u : store_.units()) {
         if (get_path(u) == name) {
            return u;
         }
      }

      Model *model = currentModel();
      auto matches = [&name](Unit *unit) {
         return unit != nullptr && (unit->Name == name || unit->_unitname == name);
      };

      // Short unit names are resolved in the current model first and then in
      // directly imported models. This avoids the old load-order-dependent
      // global lookup while retaining the RefHB unit-short-name notation.
      for (Unit *u : store_.units()) {
         if (matches(u) && u->ElementInPackage == model) {
            return u;
         }
      }
      for (Import *import : store_.imports()) {
         if (import == nullptr || import->ImportingP != model) {
            continue;
         }
         for (Unit *u : store_.units()) {
            if (matches(u) && u->ElementInPackage == import->ImportedP) {
               return u;
            }
         }
      }

      // Preserve a useful semantic "missing import" diagnostic for an
      // otherwise uniquely named unit instead of reducing it to an unknown
      // symbol solely because the model omitted IMPORTS.
      for (Unit *u : store_.units()) {
         if (matches(u)) {
            return u;
         }
      }
      logger_.error(DiagnosticId::NameUnitNotFound,"unknown unit " + name,line);
      return nullptr;
   }

   // Type helpers

   void MetaModelBuilder::initType(Type* t, int line)
   {
      initExtendable(t, line);
      // FunctionDef *LFTParent;
   }

   void MetaModelBuilder::addType(Type* t)
   {
      if (t == nullptr) {
         return;
      }
      if (util::starts_with(t->Name, "ILIC_")) {
         t->Name = t->Name.substr(5);
      }
      logger_.debug(">>> addType " + get_path(t));
      for (Type* tt : store_.types()) {
         if (get_path(tt) == get_path(t) && t->Name != "???") {
            if (tt->ElementInPackage == nullptr ||
                tt->ElementInPackage != t->ElementInPackage) {
               logger_.error(DiagnosticId::NameMultipleDeclarations,
                  "multiple declarations of " + get_path(t),diagnostic_range(t));
            }
            return;
         }
      }
      logger_.debug("<<< addType " + get_path(t));
      store_.addType(*t);
   }

   Type* MetaModelBuilder::findType(const string &name, int line, bool error)
   {
      
      string search;
      if (util::starts_with(name, "ILIC_")) {
         search = name.substr(5);
      }
      else {
         search = name;
      }
      
      if (search == "HALIGNMENT") {
         search = "INTERLIS.HALIGNMENT";
      }
      else if (search == "VALIGNMENT") {
         search = "INTERLIS.VALIGNMENT";
      }

      MetaElement *ctx = currentPackage();
         
      string package_path = get_path(currentPackage());
      logger_.debug(">>> findType <" + search + "> in context " + package_path);

      // A class inherited through an extended topic may be addressed through
      // the extending topic's qualified path. The element itself remains
      // owned by the base topic, so include the corresponding base path in
      // the lookup candidates.
      vector<string> searches({search});
      for (size_t index = 0; index < searches.size(); ++index) {
         string candidate = searches[index];
         for (Package *package : store_.packages()) {
            if (package->getClass() != "SubModel") {
               continue;
            }
            SubModel *topic = static_cast<SubModel *>(package);
            if (topic->_super == nullptr) {
               continue;
            }
            string prefix = get_path(topic) + ".";
            if (util::starts_with(candidate,prefix)) {
               string inherited = get_path(topic->_super) + "." + candidate.substr(prefix.length());
               if (find(searches.begin(),searches.end(),inherited) == searches.end()) {
                  searches.push_back(inherited);
               }
            }
         }
      }

      Type *found = nullptr;
      for (Type* t : store_.types()) {
         string path = get_path(t);
         for (auto candidate : searches) {
            if (candidate == path) {
               found = t;
               break;
            }
         }
         if (path == package_path + "." + search) {
            found = t;
         }
         if (ctx->getClass() == "Model") {
            string first_unqualified_match = "";
            for (auto unqualified : unqualifiedImports(ctx->Name)) {
               if (path == unqualified + "." + search) {
                  if (first_unqualified_match == "") {
                     first_unqualified_match = unqualified;
                  }
                  else {
                     logger_.error(DiagnosticId::NameAmbiguous,
                        "ambiguous path " + search + " found in " +
                           first_unqualified_match + " and " + unqualified,line);
                  }
                  found = t;
                  break;
               }
            }
         }
         else if (ctx->getClass() == "SubModel") {
            if (path == get_parent_path(ctx) + "." + search) {
               found = t;
            }
            for (auto unqualified : unqualifiedImports(get_parent_path(ctx))) {
               string first_unqualified_match = "";
               if (path == unqualified + "." + search) {
                  if (first_unqualified_match == "") {
                     first_unqualified_match = unqualified;
                  }
                  else {
                     logger_.error(DiagnosticId::NameAmbiguous,
                        "ambiguous path " + search + " found in " +
                           first_unqualified_match + " and " + unqualified,line);
                  }
                  found = t;
                  break;
               }
            }
            SubModel *s = static_cast<SubModel *>(ctx);
            while (s->_super != nullptr) {
               if (path == get_path(s->_super) + "." + search) {
                  found = t;
                  break;
               }
               s = static_cast<SubModel *>(s->_super);
            }
         }
      }
      
      if (found != nullptr) {
         logger_.debug("<<< findType " + get_path(found));
         return found;
      }

      if (error) {
         logger_.error(DiagnosticId::NameTypeNotFound,"type " + search + " not found.",line);
      }

      return nullptr;

   }

   Type* MetaModelBuilder::findType(const string &name, int line)
   {
      return findType(name, line, true);
   }

   string MetaModelBuilder::typeString(Type *t) const
   {
      if (t == nullptr) {
         return "???";
      }
      if (t->Name == "BOOLEAN") {
         return "BooleanType";
      }
      else {
         return t->getClass();
      }
   }

   void MetaModelBuilder::initDomainType(DomainType* t, int line)
   {
      initType(t, line);
   }

   DomainType* MetaModelBuilder::findDomainType(const string &name, int line)
   {
      Type* t = findType(name, line);
      if (t == nullptr) {
         return nullptr;
      }
      if (t->isSubClassOf("DomainType")) {
         return static_cast<DomainType*>(t);
      }
      else {
         return nullptr;
      }
   }

   // Class helpers

   void MetaModelBuilder::initClass(Class* c, int line)
   {
      initType(c, line);
   }

   void MetaModelBuilder::addClass(Class* c)
   {
      addType(c);
   }

   Class* MetaModelBuilder::findClassOrView(const string &name,int line)
   {

      Type* t = findType(name, line, false);
      if (t == nullptr) {
         logger_.error(DiagnosticId::NameViewableNotFound,"viewable " + name + " not found",line);
         return nullptr;
      }

      if (t->getClass() != "Class" && t->getClass() != "View") {
         logger_.error(DiagnosticId::ReferenceClassOrViewRequired,
            name + " is no class or view",line);
         return nullptr;
      }

      return static_cast<Class*>(t);

   }

   Class* MetaModelBuilder::findClassOrStructure(const string &name,int line)
   {

      Type* t = findType(name, line, false);
      if (t == nullptr) {
         logger_.error(DiagnosticId::NameViewableNotFound,"viewable " + name + " not found",line);
         return nullptr;
      }

      if (t->getClass() != "Class") {
         logger_.error(DiagnosticId::ReferenceClassOrStructureRequired,
            name + " is no class or structure",line);
         return nullptr;
      }
      
      Class *c = static_cast<Class *>(t);
      if (c->Kind != Class::ClassVal && c->Kind != Class::Structure) {
         logger_.error(DiagnosticId::ReferenceClassOrStructureRequired,
            name + " is no class or structure",line);
         return nullptr;
      }

      return c;

   }

   Class* MetaModelBuilder::findClassType(const string &name,int line)
   {

      Type* t = findType(name, line, false);
      if (t == nullptr) {
         logger_.error(DiagnosticId::NameViewableNotFound,"viewable " + name + " not found",line);
         return nullptr;
      }

      if (t->getClass() != "Class") {
         logger_.error(DiagnosticId::ReferenceClassStructureOrAssociationRequired,
            name + " is no class, structure or association ",line);
         return nullptr;
      }
      
      return static_cast<Class *>(t);

   }

   Class* MetaModelBuilder::findClass(const string &name, int line)
   {
      logger_.debug("find_class " + name);
      Class* c;
      if (name == "ANYCLASS" || name == "INTERLIS.ANYCLASS" || name == "CLASS" || name == "INTERLIS.CLASS") { (void)store_.anyClass(); (void)store_.anyStructure();
         c = &store_.anyClass();
      }
      else if (name == "ANYSTRUCTURE" || name == "INTERLIS.ANYSTRUCTURE" || name == "STRUCTURE" || name == "INTERLIS.STRUCTURE") { (void)store_.anyClass(); (void)store_.anyStructure();
         c = &store_.anyStructure();
      }
      else {
         Type* t = findType(name, line, false);
         if (t == nullptr) {
            logger_.error(DiagnosticId::NameClassNotFound,"class " + name + " not found",line);
            return nullptr;
         }
         if (t->getClass() != "Class") {
            logger_.error(DiagnosticId::ReferenceClassRequired,name + " is no class",line);
            return nullptr;
         }
         c = static_cast<Class *>(t);
         if (c->Kind != Class::ClassVal) {
            logger_.error(DiagnosticId::ReferenceClassRequired,name + " is no class",line);
         }
      }
      return c;
   }

   Class* MetaModelBuilder::findClass(Package* p, const string &name, int line)
   {
      logger_.debug("find_class " + name);
      if (p == nullptr) {
         return nullptr;
      }
      for (auto e : p->Element) {
         if (e->Name == name && e->getClass() == "Class") {
            Class* c = static_cast<Class*>(e);
            if (c->Kind == Class::ClassVal) {
               return c;
            }
         }
      }
      logger_.error(DiagnosticId::NameClassNotFound,"class " + name + " not found",line);
      return nullptr;
   }

   Class* MetaModelBuilder::findStructure(const string &name, int line)
   {
      logger_.debug("findStructure " + name);
      Type* t = findType(name, line, false);
      if (t == nullptr) {
         logger_.error(DiagnosticId::NameStructureNotFound,
            "structure " + name + " not found",line);
         return nullptr;
      }
      if (t->getClass() != "Class") {
         logger_.error(DiagnosticId::ReferenceStructureRequired,name + " is no structure",line);
         return nullptr;
      }
      Class *c = static_cast<Class *>(t);
      if (c->Kind != Class::Structure) {
         logger_.error(DiagnosticId::ReferenceStructureRequired,name + " is no structure",line);
      }
      return c;
   }

   Class* MetaModelBuilder::findStructure(Package* p, const string &name, int line)
   {
      logger_.debug("findStructure " + name);
      if (p == nullptr) {
         return nullptr;
      }
      for (auto e : p->Element) {
         if (e->Name == name && e->getClass() == "Class") {
            Class* c = static_cast<Class*>(e);
            if (c->Kind == Class::Structure) {
               return c;
            }
         }
      }
      logger_.error(DiagnosticId::NameStructureNotFound,
         "structure " + name + " not found",line);
      return nullptr;
   }

   Class* MetaModelBuilder::findAssociation(const string &name,int line,const ilic::SourceRange &referenceRange)
   {
      logger_.debug("findAssociation " + name);
      Type* t = findType(name, line, false);
      if (t == nullptr) {
         logger_.error(DiagnosticId::NameAssociationNotFound,
            "association " + name + " not found",line);
         return nullptr;
      }
      ilic::SourceRange primaryRange = referenceRange;
      if (!primaryRange.valid && line > 0 && !currentSourceUri_.empty()) {
         primaryRange.valid = true;
         primaryRange.uri = currentSourceUri_;
         primaryRange.start.line = static_cast<size_t>(line - 1);
         primaryRange.end = primaryRange.start;
         primaryRange.end.character++;
      }
      if (t->getClass() != "Class") {
         const string declaration = get_path(t);
         logger_.error(DiagnosticId::AssociationInvalidBaseKind,
            declaration + " is a " + diagnostic_type_kind(t) +
               ", but an ASSOCIATION is required as an association base",
            primaryRange,
            related_information(t,"Base declaration: " + declaration)
         );
         return nullptr;
      }
      Class *c = static_cast<Class *>(t);
      if (c->Kind != Class::Association) {
         const string declaration = get_path(c);
         logger_.error(DiagnosticId::AssociationInvalidBaseKind,
            declaration + " is a " + diagnostic_type_kind(c) +
               ", but an ASSOCIATION is required as an association base",
            primaryRange,
            related_information(c,"Base declaration: " + declaration)
         );
         return nullptr;
      }
      return c;
   }

   Class* MetaModelBuilder::findAssociation(Package* p, const string &name, int line)
   {
      if (p == nullptr) {
         return nullptr;
      }
      logger_.debug("findAssociation " + name);
      for (auto e : p->Element) {
         if (e->Name == name && e->getClass() == "Class") {
            Class* c = static_cast<Class*>(e);
            if (c->Kind == Class::Association) {
               return c;
            }
         }
      }
      logger_.error(DiagnosticId::NameAssociationNotFound,
         "association " + name + " not found",line);
      return nullptr;
   }

   View* MetaModelBuilder::findView(const string &name, int line)
   {
      logger_.debug("findView " + name);
      Type* t = findType(name, line, false);
      if (t == nullptr) {
         logger_.error(DiagnosticId::NameViewNotFound,"view " + name + " not found",line);
         return nullptr;
      }
      if (t->getClass() != "View") {
         logger_.error(DiagnosticId::ReferenceViewRequired,name + " is no view",line);
         return nullptr;
      }
      return static_cast<View *>(t);
   }

   AttrOrParam* MetaModelBuilder::findAttribute(Class* c,const string &name)
   {
      if (c == nullptr) {
         return nullptr;
      }
      logger_.debug("findAttribute " + name + " in context " + get_path(c));
      for (auto a : c->ClassAttribute) {
         if (a->Name == name) {
            return a;
         }
      }
      if (c->Super != nullptr) {
         Class* s = static_cast<Class*>(c->Super);
         return findAttribute(s,name);
      }
      else {
         return nullptr;
      }
   }

   Role* MetaModelBuilder::findRole(Class* c,const string &name)
   {
      if (c == nullptr) {
         return nullptr;
      }
      logger_.debug("findRole " + name + " in context " + get_path(c) + " " + to_string(c->_roleaccess.size()));
      for (auto r : c->Role) {
         if (r->Name == name) {
            return r;
         }
      }
      for (auto r : c->_roleaccess) {
         if (r->Name == name) {
            return r;
         }
      }
      if (c->Super != nullptr) {
         Class* s = static_cast<Class*>(c->Super);
         return findRole(s,name);
      }
      else {
         return nullptr;
      }
   }

   AttrOrParam* MetaModelBuilder::findParameter(Class* c,const string &name,int line)
   {
      logger_.debug("find_paramer " + name);
      if (c == nullptr) {
         return nullptr;
      }
      for (auto a : c->ClassParameter) {
         if (a->Name == name) {
            return a;
         }
      }
      if (c->Super != nullptr) {
         Class* s = static_cast<Class*>(c->Super);
         return findParameter(s,name,line);
      }
      else {
         logger_.error(DiagnosticId::NameParameterNotFound,
            "parameter " + name + " not found",line);
         return nullptr;
      }
   }

   // Graphic helpers

   void MetaModelBuilder::initGraphic(Graphic* g, int line)
   {
      initExtendable(g, line);
   }

   void MetaModelBuilder::addGraphic(Graphic* g)
   {
      if (g == nullptr) {
         return;
      }
      logger_.debug(">>> addGraphic " + get_path(g));
      for (Graphic* gg : store_.graphics()) {
         if (get_path(gg) == get_path(g)) {
            if (gg->ElementInPackage == nullptr ||
                gg->ElementInPackage != g->ElementInPackage) {
               logger_.error(DiagnosticId::NameMultipleDeclarations,
                  "multiple declarations of " + get_path(g),diagnostic_range(g));
            }
            return;
         }
      }
      logger_.debug("<<< addGraphic " + get_path(g));
      store_.addGraphic(*g);
   }

   Graphic* MetaModelBuilder::findGraphic(const string &name,int line)
   {

      string search;
      search = name;

      MetaElement *ctx = currentPackage();
         
      string package_path = get_path(currentPackage());
      logger_.debug(">>> findGraphic <" + search + "> in context " + package_path);

      Graphic *found = nullptr;
      for (Graphic* g : store_.graphics()) {
         string path = get_path(g);
         if (path == search) {
            found = g;
            break;
         }
         else if (path == package_path + "." + search) {
            found = g;
            break;
         }
         else if (ctx->getClass() == "Model") {
            for (auto unqualified : unqualifiedImports(ctx->Name)) {
               if (path == unqualified + "." + search) {
                  found = g;
                  break;
               }
            }
         }
         else if (ctx->getClass() == "SubModel") {
            SubModel *s = static_cast<SubModel *>(ctx);
            while (s->_super != nullptr) {
               if (path == get_path(s->_super) + "." + search) {
                  found = g;
                  break;
               }
               s = static_cast<SubModel *>(s->_super);
            }
            if (path == get_parent_path(ctx) + "." + search) {
               found = g;
               break;
            }
            for (auto unqualified : unqualifiedImports(get_parent_path(ctx))) {
               if (path == unqualified + "." + search) {
                  found = g;
                  break;
               }
            }
         }
      }
      
      logger_.debug("<<< findGraphic " + get_path(found));
      
      return found;

   }

   // expression helpers

   void MetaModelBuilder::initExpression(Expression* e, int line)
   {
      initObject(e, line);
      e->OccurrenceScope = dynamic_cast<MetaElement *>(current());
      e->OccurrencePackage = currentPackage();
   }

   void MetaModelBuilder::initFactor(Factor* f, int line)
   {
      initExpression(f, line);
   }

   // function helpers

   void MetaModelBuilder::initFunction(FunctionDef* f, int line)
   {
      initMetaElement(f, line);
   }

   void MetaModelBuilder::addFunction(FunctionDef* function)
   {
      if (function == nullptr) {
         return;
      }
      logger_.debug("addFunction " + function->Name + "(" + get_path(function) + ")");
      for (FunctionDef* f : store_.functions()) {
         if (f->Name == function->Name) {
            // add domain type only once
            return;
         }
      }
      store_.addFunction(*function);
   }

   FunctionDef* MetaModelBuilder::findFunction(const string &name, int line)
   {

      logger_.debug("findFunction " + name);

      for (FunctionDef* f : store_.functions()) {
         if (get_path(f) == name) {
            return f;
         }
         else if (get_path(f) == get_parent_path(f) + "." + name) {
            return f;
         }
      }

      logger_.error(DiagnosticId::NameFunctionNotFound,
         "function " + name + " not found",line);
      return nullptr;

   }

   // lineform helpers

   void MetaModelBuilder::initLineForm(LineForm* f, int line)
   {
      initMetaElement(f, line);
   }

   void MetaModelBuilder::addLineForm(LineForm* lineform)
   {
      if (lineform == nullptr) {
         return;
      }
      logger_.debug("addLineForm " + lineform->Name + "(" + get_path(lineform) + ")");
      for (LineForm* f : store_.lineForms()) {
         if (f->Name == lineform->Name) {
            // add domain type only once
            return;
         }
      }
      store_.addLineForm(*lineform);
   }

   LineForm* MetaModelBuilder::findLineForm(const string &name, int line)
   {

      logger_.debug("findLineForm " + name);

      for (LineForm* f : store_.lineForms()) {
         if (get_path(f) == name) {
            return f;
         }
         else if (get_path(f) == get_parent_path(f) + "." + name) {
            return f;
         }
      }

      logger_.error(DiagnosticId::NameLineFormNotFound,
         "lineform " + name + " not found.",line);
      return nullptr;

   }

   // constraint helpers

   void MetaModelBuilder::initConstraint(Constraint* c, int line)
   {
      initMetaElement(c, line);
   }

   // other helpers

   void MetaModelBuilder::debug(antlr4::ParserRuleContext *ctx, string message)
   {
      logger_.debug(message + ", line=" +
         to_string(ctx == nullptr ? -1 : ctx->start->getLine()));
   }

   Type* MetaModelBuilder::anyToType(antlrcpp::Any any)
   {

      logger_.debug(">>> anyToType()");
      Type* t;

      try {
         t = any.as<TypeRelatedType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<ClassRelatedType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<BooleanType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<TextType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<BlackboxType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<NumType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<CoordType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         AnyOIDType* t = any.as<AnyOIDType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         AttributeRefType* t = any.as<AttributeRefType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<EnumType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<EnumTreeValueType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<LineType*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<Class*>();
         logger_.debug(">>> class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      try {
         t = any.as<DomainType*>();
         logger_.debug(">>> DomainType class=" + t->getClass());
         return t;
      }
      catch (exception e) {
      }

      logger_.internal_error("anyToType: unsupported type", 1);
      return nullptr;

   }

   DomainType* MetaModelBuilder::anyToDomainType(antlrcpp::Any any)
   {
      Type* t = anyToType(any);
      try {
         return static_cast<DomainType*>(t);
      }
      catch (exception e) {
         logger_.internal_error("unable to cast " + t->getClass() + " to DomainType (" + e.what(), 1);
         return nullptr;
      }
   }

   int MetaModelBuilder::line(antlr4::ParserRuleContext *ctx)
   {
      return ctx->start->getLine();
   }

   int MetaModelBuilder::line(antlr4::Token *token)
   {
      return token->getLine();
   }
   
   int MetaModelBuilder::line(antlr4::tree::TerminalNode* node)
   {
      return node->getSymbol()->getLine();
   }
   
   static list <string> reserved_names = {
      "INTERLIS",
      "CONTRACTED",
      "REFSYSTEM",
      "SYMBOLOGY",
      "TYPE",
      "MODEL",
      "END",
      "VERSION",
      "NOINCREMENTALTRANSFER",
      "TRANSLATION",
      "OF",
      "AT",
      "CHARSET",
      "XMLNS",
      "IMPORTS",
      "UNQUALIFIED",
      "TOPIC",
      "DEPENDS",
      "ON",
      "AS",
      "VIEW",
      "EXTENDS",
      "BASKET",
      "OID",
      "BOOLEAN",
      "HALIGNMENT",
      "VALIGNMENT",
      "CLASS",
      "ABSTRACT",
      "EXTENDED",
      "FINAL",
      "TRANSIENT",
      "CONTINUOUS",
      "SUBDIVISION",
      "NO",
      "ATTRIBUTE",
      "TEXT",
      "DATE",
      "TIMEOFDAY",
      "DATETIME",
      "STRUCTURE",
      "PARAMETER",
      "MANDATORY",
      "GENERIC",
      "GENERICS",
      "DEFERRED",
      "CONTEXT",
      "MULTICOORD",
      "MULTIPOLYLINE",
      "MULTISURFACE",
      "MULTIAREA",
      "BAG",
      "LIST",
      "REFERENCE",
      "TO",
      "ANYCLASS",
      "RESTRICTION",
      "ASSOCIATION",
      "DERIVED",
      "EXTERNAL",
      "FROM",
      "UNDEFINED",
      "MTEXT",
      "NAME",
      "URI",
      "ALL",
      "CIRCULAR",
      "OTHERS",
      "NUMERIC",
      "CLOCKWISE",
      "COUNTERCLOCKWISE",
      "CARDINALITY",
      "OR",
      "HIDING",
      "ORDERED",
      "DOMAIN",
      "PI",
      "LNBASE",
      "FORMAT",
      "INHERIT",
      "COORD",
      "ROTATION",
      "ANY",
      "BLACKBOX",
      "XML",
      "BINARY",
      "DIRECTED",
      "POLYLINE",
      "SURFACE",
      "AREA",
      "WITH",
      "STRAIGHTS",
      "ARCS",
      "VERTEX",
      "WITHOUT",
      "OVERLAPS",
      "LINE",
      "ATTRIBUTES",
      "FORM",
      "UNIT",
      "FUNCTION",
      "SIGN",
      "OBJECTS",
      "METAOBJECT",
      "CONSTRAINT",
      "CONSTRAINTS",
      "EXISTENCE",
      "REQUIRED",
      "IN",
      "UNIQUE",
      "WHERE",
      "LOCAL",
      "SET",
      "AND",
      "NOT",
      "BASED",
      "BASE",
      "INHERITANCE",
      "DEFINED",
      "INSPECTION",
      "THIS",
      "THISAREA",
      "THATAREA",
      "PARENT",
      "FIRST",
      "LAST",
      "AGGREGATES",
      "OBJECT",
      "ENUMVAL",
      "ENUMTREEVAL",
      "PROJECTION",
      "JOIN",
      "NULL",
      "UNION",
      "AGGREGATION",
      "BY",
      "GRAPHIC",
      "ACCORDING",
      "WHEN",
      "ANYSTRUCTURE",
      "TRANSFER",
      "URL"
   };

   bool MetaModelBuilder::isReservedName(const string &name)
   {
      for (auto n: reserved_names) {
         if (n == name) {
            return true;
         }
      }
      return false;
   }

}
