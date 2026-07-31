#include "ilic/capi.h"

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace {

class StartGate final {
public:
   explicit StartGate(std::size_t participants) : participants_(participants), remaining_(participants) {}

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
      condition_.wait_for(lock,std::chrono::seconds(10),[&] {
         return generation_ != generation;
      });
   }

private:
   std::mutex mutex_;
   std::condition_variable condition_;
   const std::size_t participants_;
   std::size_t remaining_;
   std::uint64_t generation_ = 0;
};

std::string compileRequest(const std::string &uri)
{
   return "{\"schemaVersion\":1,\"roots\":[\"" + uri +
      "\"],\"options\":{\"autoSearch\":false}}";
}

std::string resultJson(std::uint32_t result)
{
   if (result == 0) throw std::runtime_error("C ABI returned an empty result handle");
   std::size_t length = 0;
   const char *json = ilic_result_json(result,&length);
   if (json == nullptr) {
      ilic_result_destroy(result);
      throw std::runtime_error("C ABI returned no result JSON");
   }
   std::string value(json,length);
   ilic_result_destroy(result);
   return value;
}

} // namespace

int main()
{
   constexpr std::size_t sessionCount = 8;
   constexpr std::size_t repetitions = 20;
   std::vector<std::uint32_t> handles;
   std::vector<std::string> uris;
   handles.reserve(sessionCount);
   uris.reserve(sessionCount);

   for (std::size_t index = 0; index < sessionCount; ++index) {
      const std::string model = "CapiParallel" + std::to_string(index);
      const std::string uri = "memory:///capi/" + model + ".ili";
      const std::string source = "INTERLIS 2.3; MODEL " + model +
         " AT \"https://example.invalid/\" VERSION \"1\" =\n"
         "  CLASS Item (ABSTRACT) = Name : TEXT; END Item;\nEND " + model + ".\n";
      const std::uint32_t handle = ilic_session_create();
      if (handle == 0) return 1;
      if (ilic_session_put_source(handle,uri.data(),uri.size(),
            reinterpret_cast<const std::uint8_t *>(source.data()),source.size(),1) != 0)
         return 2;
      handles.push_back(handle);
      uris.push_back(uri);
   }

   for (std::size_t repetition = 0; repetition < repetitions; ++repetition) {
      StartGate gate(sessionCount);
      std::vector<std::thread> workers;
      std::vector<std::exception_ptr> failures(sessionCount);
      workers.reserve(sessionCount);
      for (std::size_t index = 0; index < sessionCount; ++index) {
         workers.emplace_back([&,index] {
            try {
               gate.arriveAndWait();
               const std::string request = compileRequest(uris[index]);
               const std::string json = resultJson(ilic_compile(
                  handles[index],request.data(),request.size()));
               if (json.find("\"success\":true") == std::string::npos ||
                   json.find("CapiParallel" + std::to_string(index)) == std::string::npos ||
                   json.find("CapiParallel" + std::to_string((index + 1) % sessionCount)) != std::string::npos)
                  throw std::runtime_error("C ABI result was not isolated for session " +
                     std::to_string(index) + ": " + json);
            }
            catch (...) {
               failures[index] = std::current_exception();
            }
         });
      }
      for (auto &worker : workers) worker.join();
      for (const auto &failure : failures)
         if (failure != nullptr) std::rethrow_exception(failure);
   }

   for (const auto handle : handles) ilic_session_destroy(handle);
   return 0;
}
