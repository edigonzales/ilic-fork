#include "StringUtil.h"
#include <algorithm>
#include <fstream>


bool util::starts_with(std::string const &value, std::string const &start)
{
   return (value.rfind(start, 0) == 0);
}

bool util::ends_with(std::string const &value, std::string const &ending)
{
   if (ending.size() > value.size()) {
      return false;
   }
   return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool util::find_string(vector<string> list,string search_string)
{
   for (auto l : list) {
      if (l == search_string) {
         return true;
      }
   }
   return false;
}


inline bool compare_case_insensitive_char(char a, char b) 
{
   return (tolower(a) == tolower(b));
}

bool util::compare_case_insensitive(string s1, string s2) 
{
   return(
      (s1.size() == s2.size()) &&
      equal(s1.begin(), s1.end(), s2.begin(), compare_case_insensitive_char)
   );
}

string util::load_string(string fname)
{
   ifstream in(fname, ios::binary);
   if (!in.is_open()) {
      return "";
   }
   return string((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
}

string util::load_filtered_string_from_file(string fname)
{
   return load_string(std::move(fname));
}
