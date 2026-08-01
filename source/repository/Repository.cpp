#include "ilic/Repository.h"

#include "Md5.h"
#include "RepositoryCache.h"
#include "RepositoryCrawler.h"
#include "RepositoryResourceLoader.h"
#include "RepositoryTransport.h"
#include "RepositoryUri.h"
#include "RepositoryXml.h"

#include <algorithm>
#include <cctype>
#include <functional>
#include <optional>
#include <map>
#include <set>
#include <memory>

namespace ilic {
namespace {

Diagnostic repositoryDiagnostic(DiagnosticSeverity severity,std::string code,std::string message)
{
   Diagnostic diagnostic;
   diagnostic.severity = severity;
   diagnostic.code = std::move(code);
   diagnostic.message = std::move(message);
   return diagnostic;
}

bool hasErrors(const std::vector<Diagnostic> &diagnostics)
{
   return std::any_of(diagnostics.begin(),diagnostics.end(),[](const Diagnostic &diagnostic) {
      return diagnostic.severity == DiagnosticSeverity::Error;
   });
}

template<typename Value>
struct LoadState {
   enum class Status { NotLoaded, Available, Unavailable };
   Status status = Status::NotLoaded;
   Value value;
};

struct ResolutionState {
   std::set<std::string> resolvedModels;
   std::vector<std::string> stack;
   std::map<std::string,std::string> emittedFileChecksums;
};

std::string lower(std::string value)
{
   std::transform(value.begin(),value.end(),value.begin(),[](unsigned char character) {
      return static_cast<char>(std::tolower(character));
   });
   return value;
}

class StackGuard {
public:
   explicit StackGuard(std::vector<std::string> &stack) : stack_(stack) {}
   ~StackGuard() { stack_.pop_back(); }
private:
   std::vector<std::string> &stack_;
};

class PortTransportAdapter final : public repository::RepositoryTransport {
public:
   explicit PortTransportAdapter(repository::ports::RepositoryTransportPort &port) : port_(port) {}

   repository::TransportResponse get(const std::string &uri) override
   {
      repository::RepositoryTransportRequest request;
      request.uri = uri;
      const auto response = get(request);
      return response;
   }

   repository::TransportResponse get(const repository::RepositoryTransportRequest &request) override
   {
      repository::ports::RepositoryTransportRequest portRequest;
      portRequest.uri = request.uri;
      portRequest.maxBytes = request.maxBytes;
      portRequest.maxRedirects = request.maxRedirects;
      portRequest.kind = request.kind == repository::RepositoryResourceKind::ModelIndex
         ? repository::ports::RepositoryResourceKind::ModelIndex
         : request.kind == repository::RepositoryResourceKind::SiteIndex
            ? repository::ports::RepositoryResourceKind::SiteIndex
            : repository::ports::RepositoryResourceKind::Model;
      const auto response = port_.get(portRequest);
      return {response.success,response.statusCode,response.body,response.error,
         response.finalUri,response.notFound,response.retryable};
   }

private:
   repository::ports::RepositoryTransportPort &port_;
};

} // namespace

class RepositoryManager::Impl final : public repository::RepositoryBackend {
public:
   explicit Impl(RepositoryOptions configured,RepositoryPorts configuredPorts = {})
      : options(std::move(configured)),injectedPorts(std::move(configuredPorts)),
        injectedTransport(injectedPorts.transport
           ? std::make_unique<PortTransportAdapter>(*injectedPorts.transport) : nullptr),
        ownedTransport(injectedTransport ? nullptr
           : std::make_unique<repository::CurlRepositoryTransport>()),
        transport(injectedTransport ? static_cast<repository::RepositoryTransport *>(injectedTransport.get())
           : static_cast<repository::RepositoryTransport *>(ownedTransport.get())),
        cache(options.cacheDirectory.empty()
           ? repository::defaultCacheDirectory() : options.cacheDirectory,
           injectedPorts.cache.get(),injectedPorts.clock.get()),
        loader(options,*transport,cache,injectedPorts.checksum.get()),crawler(*this)
   {
      if (options.cacheDirectory.empty()) options.cacheDirectory = cache.root();
   }

   const repository::RepositoryIndex *loadIndex(const std::string &repositoryUri,
      std::vector<Diagnostic> *diagnostics) override
   {
      auto &state = repositoryIndexes[repositoryUri];
      if (state.status == LoadState<repository::RepositoryIndex>::Status::Available) return &state.value;
      if (state.status == LoadState<repository::RepositoryIndex>::Status::Unavailable) return nullptr;
      state.status = LoadState<repository::RepositoryIndex>::Status::Unavailable;

      std::string uriError;
      auto repositoryRoot = repository::RepositoryUri::parse(repositoryUri,&uriError);
      if (!repositoryRoot) {
         diagnostics->push_back(repositoryDiagnostic(DiagnosticSeverity::Warning,"ILIC-REPO-INDEX",
            "invalid repository URI " + repositoryUri + ": " + uriError));
         return nullptr;
      }
      const auto indexUri = repositoryRoot->resolve("ilimodels.xml");
      auto resource = loader.load(indexUri,{options.metadataTtl,false},
         repository::RepositoryResourceKind::ModelIndex);
      appendResourceWarnings(resource,"ILIC-REPO-CACHE",diagnostics);
      if (!resource.success) {
         diagnostics->push_back(repositoryDiagnostic(DiagnosticSeverity::Warning,"ILIC-REPO-INDEX",
            "unable to read ilimodels.xml from " + repositoryUri + ": " + resource.error));
         return nullptr;
      }
      std::vector<Diagnostic> parseDiagnostics;
      if (injectedPorts.metadataDecoder != nullptr) {
         state.value.repository = repositoryUri;
         injectedPorts.metadataDecoder->decodeModelIndex(resource.content,repositoryUri,
            state.value.models,parseDiagnostics);
      }
      else state.value = repository::RepositoryXml::parseModelIndex(resource.content,repositoryUri,
         &parseDiagnostics);
      for (auto &diagnostic : parseDiagnostics) {
         // A broken repository is recoverable while later repositories remain.
         diagnostic.severity = DiagnosticSeverity::Warning;
         diagnostics->push_back(std::move(diagnostic));
      }
      if (state.value.models.empty()) return nullptr;
      state.status = LoadState<repository::RepositoryIndex>::Status::Available;
      return &state.value;
   }

   const repository::RepositorySite *loadSite(const std::string &repositoryUri,
      std::vector<Diagnostic> *diagnostics) override
   {
      if (!options.followSiteLinks) return nullptr;
      auto &state = repositorySites[repositoryUri];
      if (state.status == LoadState<repository::RepositorySite>::Status::Available) return &state.value;
      if (state.status == LoadState<repository::RepositorySite>::Status::Unavailable) return nullptr;
      state.status = LoadState<repository::RepositorySite>::Status::Unavailable;
      auto repositoryRoot = repository::RepositoryUri::parse(repositoryUri);
      if (!repositoryRoot) return nullptr;
      auto resource = loader.load(repositoryRoot->resolve("ilisite.xml"),
         {options.metadataTtl,true},repository::RepositoryResourceKind::SiteIndex);
      appendResourceWarnings(resource,"ILIC-REPO-CACHE",diagnostics);
      if (!resource.success) return nullptr; // ilisite.xml is optional.
      if (injectedPorts.metadataDecoder != nullptr) {
         injectedPorts.metadataDecoder->decodeSite(resource.content,repositoryUri,
            state.value.parentSites,state.value.subsidiarySites,*diagnostics);
      }
      else state.value = repository::RepositoryXml::parseSite(resource.content,repositoryUri,diagnostics);
      state.status = LoadState<repository::RepositorySite>::Status::Available;
      return &state.value;
   }

   bool resolveOne(const std::string &name,const std::string &schemaLanguage,
      ResolutionState &state,RepositoryResult &result)
   {
      if (name == "INTERLIS" || state.resolvedModels.count(name) != 0) return true;
      const auto cycle = std::find(state.stack.begin(),state.stack.end(),name);
      if (cycle != state.stack.end()) {
         std::string path;
         for (auto current = cycle; current != state.stack.end(); ++current) {
            if (!path.empty()) path += " -> ";
            path += *current;
         }
         path += " -> " + name;
         result.diagnostics.push_back(repositoryDiagnostic(DiagnosticSeverity::Error,
            "ILIC-REPO-CYCLE","dependency cycle: " + path));
         return false;
      }
      state.stack.push_back(name);
      StackGuard guard(state.stack);

      auto lookup = crawler.findModel(options.repositories,name,schemaLanguage);
      result.diagnostics.insert(result.diagnostics.end(),lookup.diagnostics.begin(),
         lookup.diagnostics.end());
      if (!lookup.found) {
         result.diagnostics.push_back(repositoryDiagnostic(DiagnosticSeverity::Error,
            "ILIC-REPO-NOT-FOUND","model " + name + " not found in configured repositories"));
         return false;
      }

      bool dependenciesOk = true;
      for (const auto &dependency : lookup.metadata.dependencies)
         dependenciesOk = resolveOne(dependency,schemaLanguage,state,result) && dependenciesOk;
      if (!dependenciesOk) return false;

      const auto path = repository::validateRepositoryRelativePath(lookup.metadata.file);
      if (!path.valid) {
         result.diagnostics.push_back(repositoryDiagnostic(DiagnosticSeverity::Error,
            "ILIC-REPO-PATH","unsafe repository path " + lookup.metadata.file + ": " + path.error));
         return false;
      }
      auto root = repository::RepositoryUri::parse(lookup.metadata.repository);
      if (!root) {
         result.diagnostics.push_back(repositoryDiagnostic(DiagnosticSeverity::Error,
            "ILIC-REPO-PATH","invalid repository root " + lookup.metadata.repository));
         return false;
      }
      repository::RepositoryUri uri = root->resolve(path.normalized);
      if (root->isLocal() && (!uri.isLocal()
         || !repository::isPathWithin(root->toLocalPath(),uri.toLocalPath()))) {
         result.diagnostics.push_back(repositoryDiagnostic(DiagnosticSeverity::Error,
            "ILIC-REPO-PATH","repository path escapes its root: " + lookup.metadata.file));
         return false;
      }

      const auto emitted = state.emittedFileChecksums.find(uri.normalized());
      if (emitted == state.emittedFileChecksums.end()) {
         auto resource = loader.loadModel(uri,lookup.metadata.md5,false);
         appendResourceWarnings(resource,"ILIC-REPO-CACHE",&result.diagnostics);
         if (!resource.success) {
            const std::string code = resource.error.rfind("MD5 mismatch",0) == 0
               ? "ILIC-REPO-CHECKSUM" : "ILIC-REPO-DOWNLOAD";
            result.diagnostics.push_back(repositoryDiagnostic(DiagnosticSeverity::Error,code,
               "unable to fetch " + uri.normalized() + ": " + resource.error));
            return false;
         }
         state.emittedFileChecksums.emplace(uri.normalized(),checksum(resource.content));
         result.models.push_back({lookup.metadata,uri.normalized(),resource.localPath,
            std::move(resource.content),resource.fromCache,resource.stale});
      }
      else if (options.validateChecksums && !lookup.metadata.md5.empty()
         && lower(emitted->second) != lower(lookup.metadata.md5)) {
         result.diagnostics.push_back(repositoryDiagnostic(DiagnosticSeverity::Error,
            "ILIC-REPO-CHECKSUM","unable to fetch " + uri.normalized() + ": MD5 mismatch for "
               + uri.normalized() + ": expected " + lookup.metadata.md5 + ", actual "
               + emitted->second));
         return false;
      }
      state.resolvedModels.insert(name);
      return true;
   }

   void appendResourceWarnings(const repository::RepositoryResource &resource,
      const std::string &code,std::vector<Diagnostic> *diagnostics)
   {
      for (const auto &message : resource.warnings) diagnostics->push_back(
         repositoryDiagnostic(DiagnosticSeverity::Warning,code,message));
   }

   std::string checksum(std::string_view content) const
   {
      return injectedPorts.checksum == nullptr ? repository::md5(std::string(content))
         : injectedPorts.checksum->md5(content);
   }

   RepositoryOptions options;
   RepositoryPorts injectedPorts;
   std::unique_ptr<PortTransportAdapter> injectedTransport;
   std::unique_ptr<repository::RepositoryTransport> ownedTransport;
   repository::RepositoryTransport *transport = nullptr;
   repository::RepositoryCache cache;
   repository::RepositoryResourceLoader loader;
   repository::RepositoryCrawler crawler;
   std::map<std::string,LoadState<repository::RepositoryIndex>> repositoryIndexes;
   std::map<std::string,LoadState<repository::RepositorySite>> repositorySites;
};

RepositoryManager::RepositoryManager(RepositoryOptions options)
   : impl_(std::make_unique<Impl>(std::move(options)))
{
}

RepositoryManager::RepositoryManager(RepositoryOptions options,RepositoryPorts ports)
   : impl_(std::make_unique<Impl>(std::move(options),std::move(ports)))
{
}

RepositoryManager::~RepositoryManager() = default;
RepositoryManager::RepositoryManager(RepositoryManager &&) noexcept = default;
RepositoryManager &RepositoryManager::operator=(RepositoryManager &&) noexcept = default;

const RepositoryOptions &RepositoryManager::options() const { return impl_->options; }

std::vector<std::string> RepositoryManager::defaultRepositories()
{
   return {"https://models.interlis.ch"};
}

std::vector<ModelMetadata> RepositoryManager::parseIliModelsXml(const std::string &xml,
   const std::string &repository,std::vector<Diagnostic> *diagnostics)
{
   return repository::RepositoryXml::parseModelIndex(xml,repository,diagnostics).models;
}

std::vector<ModelMetadata> RepositoryManager::listModels()
{
   return impl_->crawler.listModels(impl_->options.repositories);
}

RepositoryResult RepositoryManager::resolve(const std::string &model,
   const std::string &schemaLanguage)
{
   return resolve(std::vector<std::string>{model},schemaLanguage);
}

RepositoryResult RepositoryManager::resolve(const std::vector<std::string> &requested,
   const std::string &schemaLanguage)
{
   RepositoryResult result;
   result.success = true;
   ResolutionState state;
   for (const auto &model : requested)
      result.success = impl_->resolveOne(model,schemaLanguage,state,result) && result.success;
   result.success = result.success && !hasErrors(result.diagnostics);
   return result;
}

} // namespace ilic
