#include "ilic/capi.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

std::string snapshotUri(const std::string &path)
{
   const std::size_t slash = path.find_last_of("/\\");
   const std::string name = slash == std::string::npos ? path : path.substr(slash + 1);
   return "memory:///snapshot/" + name;
}

} // namespace

int main(int argc,char **argv)
{
   if (argc != 2) {
      std::cerr << "usage: ilic_snapshot_json_driver SOURCE\n";
      return 2;
   }
   std::ifstream input(argv[1],std::ios::binary);
   if (!input) {
      std::cerr << "cannot read snapshot source: " << argv[1] << "\n";
      return 2;
   }
   const std::vector<char> bytes((std::istreambuf_iterator<char>(input)),{});
   const std::string uri = snapshotUri(argv[1]);
   const std::uint32_t session = ilic_session_create();
   if (session == 0) return 3;
   const int putStatus = ilic_session_put_source(session,uri.data(),uri.size(),
      reinterpret_cast<const std::uint8_t *>(bytes.data()),bytes.size(),1);
   if (putStatus != 0) {
      ilic_session_destroy(session);
      return 3;
   }
   const std::string request = "{\"schemaVersion\":1,\"uri\":\"" + uri + "\"}";
   const std::uint32_t result = ilic_editor_snapshot(session,request.data(),request.size());
   std::size_t length = 0;
   const char *json = ilic_result_json(result,&length);
   if (json == nullptr) {
      ilic_result_destroy(result);
      ilic_session_destroy(session);
      return 3;
   }
   std::cout.write(json,static_cast<std::streamsize>(length));
   std::cout << '\n';
   ilic_result_destroy(result);
   ilic_session_destroy(session);
   return 0;
}
