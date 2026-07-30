#pragma once

#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string_view>

namespace ilic::test {
namespace detail {

inline std::mutex &failureOutputMutex()
{
   static std::mutex mutex;
   return mutex;
}

} // namespace detail

[[noreturn]] inline void fail(
   const char *expression,
   const char *file,
   int line,
   std::string_view message = {}) noexcept
{
   {
      std::lock_guard<std::mutex> lock(detail::failureOutputMutex());
      std::fprintf(stderr,"ILIC_TEST_FAILURE\n");
      std::fprintf(stderr,"  file: %s\n",file != nullptr ? file : "<unknown>");
      std::fprintf(stderr,"  line: %d\n",line);
      std::fprintf(stderr,"  expression: %s\n",
         expression != nullptr ? expression : "<explicit failure>");
      if (!message.empty()) {
         std::fputs("  message: ",stderr);
         std::fwrite(message.data(),1,message.size(),stderr);
         std::fputc('\n',stderr);
      }
      std::fflush(stderr);
   }
   std::exit(EXIT_FAILURE);
}

} // namespace ilic::test

#define ILIC_REQUIRE(expression) \
   do { \
      if (!(expression)) { \
         ::ilic::test::fail(#expression,__FILE__,__LINE__); \
      } \
   } while (false)

#define ILIC_REQUIRE_MSG(expression,message) \
   do { \
      if (!(expression)) { \
         ::ilic::test::fail(#expression,__FILE__,__LINE__,(message)); \
      } \
   } while (false)

#define ILIC_FAIL(message) \
   do { \
      ::ilic::test::fail(nullptr,__FILE__,__LINE__,(message)); \
   } while (false)
