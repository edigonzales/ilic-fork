#include "ilic/Formatter.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int main(int argc,char **argv)
{
   if (argc < 2) {
      std::cerr << "usage: ilic-format [--check] model.ili ...\n";
      return 2;
   }
   const bool check = std::string(argv[1]) == "--check";
   const int firstFile = check ? 2 : 1;
   if ((!check && argc != 2) || firstFile == argc) {
      std::cerr << "usage: ilic-format [--check] model.ili ...\n";
      return 2;
   }
   int failures = 0;
   for (int i = firstFile; i < argc; ++i) {
      const std::string uri = argv[i];
      std::ifstream input(uri,std::ios::binary);
      if (!input) {
         std::cerr << "unable to open " << uri << "\n";
         ++failures;
         continue;
      }
      const std::string text((std::istreambuf_iterator<char>(input)),std::istreambuf_iterator<char>());
      ilic::FormatResult result = ilic::Formatter().format(uri,text);
      if (!result.success) {
         for (const auto &diagnostic : result.diagnostics) {
            std::cerr << uri << ": " << diagnostic.code << ": " << diagnostic.message << "\n";
         }
         ++failures;
         continue;
      }
      if (check) {
         ilic::FormatResult second = ilic::Formatter().format(uri,result.text);
         if (!second.success || second.text != result.text) {
            std::cerr << uri << ": formatter is not idempotent\n";
            ++failures;
         }
      }
      else std::cout << result.text;
   }
   return failures == 0 ? 0 : 1;
}
