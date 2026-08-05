#include "../../include/ilic/Compiler.h"

#include "CompilationOrchestrator.h"
#include "CompilerSessionState.h"
#include "SnapshotService.h"
#include "SourceUpdateCoordinator.h"
#include "ilic/IlicVersionConfig.h"

#include <mutex>

namespace ilic {

CompilerSession::CompilerSession() : CompilerSession(IncrementalCacheOptions{}) {}

CompilerSession::CompilerSession(IncrementalCacheOptions cacheOptions)
   : impl_(std::make_unique<detail::CompilerSessionState>(cacheOptions)) {}

CompilerSession::~CompilerSession() = default;

void CompilerSession::putSource(std::string uri,std::string utf8,std::uint64_t version)
{
   (void)updateSource(std::move(uri),std::move(utf8),version);
}

SourceUpdateResult CompilerSession::updateSource(std::string uri,std::string utf8,
   std::uint64_t version)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return detail::SourceUpdateCoordinator::update(*impl_,std::move(uri),std::move(utf8),version);
}

bool CompilerSession::removeSource(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return detail::SourceUpdateCoordinator::remove(*impl_,uri);
}

SourceManager &CompilerSession::sources() { return impl_->sources; }
const SourceManager &CompilerSession::sources() const { return impl_->sources; }

SyntaxSnapshot CompilerSession::parse(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return detail::SnapshotService::parse(*impl_,uri);
}

EditorSnapshot CompilerSession::editorSnapshot(const std::string &uri)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return detail::SnapshotService::editor(*impl_,uri);
}

SemanticSnapshot CompilerSession::analyze(const CompilationRequest &request)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return compileAndAnalyzeUnlocked(request).semantic;
}

CompilationAnalysisResult CompilerSession::compileAndAnalyze(
   const CompilationRequest &request)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return compileAndAnalyzeUnlocked(request);
}

CompilationAnalysisResult CompilerSession::compileAndAnalyzeUnlocked(
   const CompilationRequest &request)
{
   return detail::CompilationOrchestrator::compileAndAnalyze(*impl_,request,
      compileInvocationCount_);
}

CompilationResult CompilerSession::compile(const CompilationRequest &request)
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   ++compileInvocationCount_;
   return detail::CompilationOrchestrator::compile(*impl_,request);
}

IncrementalStats CompilerSession::incrementalStats() const
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   impl_->syncCacheStats();
   return impl_->stats;
}

IncrementalTrace CompilerSession::lastIncrementalTrace() const
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   return impl_->trace;
}

IncrementalCacheSnapshot CompilerSession::incrementalCacheSnapshot() const
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   const auto parsed = impl_->parsedCache.metrics();
   const auto root = impl_->rootCache.metrics();
   IncrementalCacheSnapshot snapshot;
   snapshot.parserEntries = parsed.entries;
   snapshot.parserRetainedBytes = parsed.retainedBytes;
   snapshot.parserEvictions = parsed.evictions;
   snapshot.rootEntries = root.entries;
   snapshot.rootRetainedBytes = root.retainedBytes;
   snapshot.rootEvictions = root.evictions;
   snapshot.parserInvariants = impl_->parsedCache.checkInvariants();
   snapshot.rootInvariants = impl_->rootCache.checkInvariants();
   return snapshot;
}

void CompilerSession::resetIncrementalStats()
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   impl_->parsedCache.resetMetrics();
   impl_->rootCache.resetMetrics();
   impl_->stats.reset();
   impl_->syncCacheStats();
}

void CompilerSession::clearIncrementalCaches()
{
   std::lock_guard<std::mutex> lock(impl_->mutex);
   const std::size_t parsedReleased = impl_->parsedCache.clear();
   const auto rootReleased = impl_->rootCache.clear();
   impl_->trace = {};
   impl_->trace.operation = "clearIncrementalCaches";
   impl_->trace.bytesReleased = parsedReleased + rootReleased.releasedBytes;
   impl_->syncCacheStats();
}

const char *version() { return ILIC_RUNTIME_VERSION; }

} // namespace ilic
