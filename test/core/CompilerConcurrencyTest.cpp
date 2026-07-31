#include "ilic/Compiler.h"
#include "ilic/Semantic.h"
#include "ilic/test/TestHarness.h"

#include <chrono>
#include <condition_variable>
#include <exception>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

class StartGate final {
public:
   explicit StartGate(std::size_t participants)
      : participants_(participants),remaining_(participants) {}

   void arriveAndWait()
   {
      std::unique_lock<std::mutex> lock(mutex_);
      const std::uint64_t generation = generation_;
      if (--remaining_ == 0) {
         remaining_ = participants_;
         ++generation_;
         condition_.notify_all();
         return;
      }
      condition_.wait(lock,[&] { return generation != generation_; });
   }

private:
   const std::size_t participants_ = 0;
   std::mutex mutex_;
   std::condition_variable condition_;
   std::size_t remaining_;
   std::uint64_t generation_ = 0;
};

std::string sourceFor(std::size_t index)
{
   const std::string model = "ParallelModel" + std::to_string(index);
   return "INTERLIS 2.3;\n"
      "!!@ displayName = \"" + model + " display\"\n"
      "MODEL " + model + " AT \"https://example.invalid/ilic/tests\" VERSION \"1\" =\n"
      "  TOPIC Topic =\n"
      "    CLASS Item = Value : TEXT * " + std::to_string(index + 1) + "; END Item;\n"
      "  END Topic;\n"
      "END " + model + ".\n";
}

struct Slot {
   ilic::CompilationResult result;
   std::exception_ptr error;
};

} // namespace

int main()
{
   constexpr std::size_t sessionCount = 8;
   constexpr std::size_t repetitions = 50;

   for (std::size_t repetition = 0; repetition < repetitions; ++repetition) {
      std::vector<std::unique_ptr<ilic::CompilerSession>> sessions;
      std::vector<std::string> uris;
      std::vector<Slot> slots(sessionCount);
      sessions.reserve(sessionCount);
      uris.reserve(sessionCount);
      for (std::size_t index = 0; index < sessionCount; ++index) {
         const std::string uri = "memory:///parallel/" + std::to_string(repetition) + "/" +
            std::to_string(index) + ".ili";
         sessions.push_back(std::make_unique<ilic::CompilerSession>());
         sessions.back()->putSource(uri,sourceFor(index));
         uris.push_back(uri);
      }

      StartGate gate(sessionCount);
      std::vector<std::thread> workers;
      for (std::size_t index = 0; index < sessionCount; ++index) {
         workers.emplace_back([&,index] {
            try {
               gate.arriveAndWait();
               ilic::CompilationRequest request;
               request.roots.push_back(uris[index]);
               slots[index].result = sessions[index]->compile(request);
            }
            catch (...) {
               slots[index].error = std::current_exception();
            }
         });
      }
      for (auto &worker : workers) worker.join();

      for (std::size_t index = 0; index < sessionCount; ++index) {
         ILIC_REQUIRE(slots[index].error == nullptr);
         ILIC_REQUIRE(slots[index].result.success);
         ILIC_REQUIRE(slots[index].result.errorCount == 0);
         bool ownModel = false;
         for (const auto &model : slots[index].result.models) {
            if (model.name == "INTERLIS") continue;
            ILIC_REQUIRE(model.name == "ParallelModel" + std::to_string(index));
            ILIC_REQUIRE(model.uri == uris[index]);
            ownModel = true;
         }
         ILIC_REQUIRE(ownModel);
         ILIC_REQUIRE(slots[index].result.diagnostics.empty());
      }
   }

   // A valid and an invalid run must keep their diagnostics separate while
   // they execute at the same time.
   ilic::CompilerSession valid;
   ilic::CompilerSession invalid;
   valid.putSource("memory:///parallel/valid.ili",sourceFor(99));
   invalid.putSource("memory:///parallel/invalid.ili",
      "INTERLIS 2.3;\nMODEL Invalid AT \"https://example.invalid\" VERSION \"1\" =\n"
      "  TOPIC Topic = CLASS Item = Value : MissingDomain; END Item; END Topic;\n"
      "END Invalid.\n");
   StartGate mixedGate(2);
   Slot validSlot;
   Slot invalidSlot;
   std::thread validWorker([&] {
      try {
         mixedGate.arriveAndWait();
         ilic::CompilationRequest request;
         request.roots.push_back("memory:///parallel/valid.ili");
         validSlot.result = valid.compile(request);
      }
      catch (...) { validSlot.error = std::current_exception(); }
   });
   std::thread invalidWorker([&] {
      try {
         mixedGate.arriveAndWait();
         ilic::CompilationRequest request;
         request.roots.push_back("memory:///parallel/invalid.ili");
         invalidSlot.result = invalid.compile(request);
      }
      catch (...) { invalidSlot.error = std::current_exception(); }
   });
   validWorker.join();
   invalidWorker.join();
   ILIC_REQUIRE(validSlot.error == nullptr);
   ILIC_REQUIRE(invalidSlot.error == nullptr);
   ILIC_REQUIRE(validSlot.result.success);
   ILIC_REQUIRE(!invalidSlot.result.success);
   ILIC_REQUIRE(invalidSlot.result.errorCount == 1);
   ILIC_REQUIRE(invalidSlot.result.diagnostics.front().code == "ILIC-NAME-TYPE-NOT-FOUND");

   // Same-session operations are serialized by the session mutex, while the
   // returned snapshots remain usable after the run Context has been gone.
   ilic::CompilerSession sameSession;
   sameSession.putSource("memory:///parallel/same.ili",sourceFor(7));
   StartGate sameGate(2);
   Slot compileSlot;
   Slot parseSlot;
   std::thread compileWorker([&] {
      try {
         sameGate.arriveAndWait();
         ilic::CompilationRequest request;
         request.roots.push_back("memory:///parallel/same.ili");
         compileSlot.result = sameSession.compile(request);
      }
      catch (...) { compileSlot.error = std::current_exception(); }
   });
   std::thread parseWorker([&] {
      try {
         sameGate.arriveAndWait();
         const auto snapshot = sameSession.parse("memory:///parallel/same.ili");
         if (!snapshot.success) throw std::runtime_error("same-session parse failed");
      }
      catch (...) { parseSlot.error = std::current_exception(); }
   });
   compileWorker.join();
   parseWorker.join();
   ILIC_REQUIRE(compileSlot.error == nullptr);
   ILIC_REQUIRE(parseSlot.error == nullptr);
   ILIC_REQUIRE(compileSlot.result.success);

   ilic::CompilationRequest analysisRequest;
   analysisRequest.roots.push_back("memory:///parallel/same.ili");
   const auto analysis = sameSession.compileAndAnalyze(analysisRequest);
   ILIC_REQUIRE(analysis.compilation.success);
   ILIC_REQUIRE(analysis.semantic.success);
   ILIC_REQUIRE(!analysis.semantic.symbols.empty());
   bool analyzedOwnModel = false;
   for (const auto &symbol : analysis.semantic.symbols)
      analyzedOwnModel = analyzedOwnModel || symbol.name == "ParallelModel7";
   ILIC_REQUIRE(analyzedOwnModel);
   return 0;
}
