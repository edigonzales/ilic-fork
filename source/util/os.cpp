#include <string>

#include "os.h"

string util::get_platform()
{
#ifdef _WIN32
   const string os = "win";
#elif __APPLE__
   const string os = "macos";
#elif __EMSCRIPTEN__
   const string os = "wasm";
#elif __linux__
   const string os = "linux";
#elif __unix__
   const string os = "unix";
#endif
   if (sizeof(void *) == 8) {
      return os + "64";
   }
   else {
      return os + "32";
   }
}
