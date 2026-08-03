#include "LanguageDetector.h"

#include <cctype>
#include <string_view>

namespace ilic::detail {

DetectedLanguage LanguageDetector::detect(const SourceBuffer &source) noexcept
{
   const std::string &text = source.text;
   std::size_t offset = 0;
   while (offset < text.size()) {
      while (offset < text.size() && std::isspace(static_cast<unsigned char>(text[offset]))) ++offset;
      if (offset + 1 < text.size() && text[offset] == '!' && text[offset + 1] == '!') {
         offset += 2;
         while (offset < text.size() && text[offset] != '\n') ++offset;
         continue;
      }
      if (offset + 1 < text.size() && text[offset] == '/' && text[offset + 1] == '*') {
         offset += 2;
         std::size_t depth = 1;
         while (offset < text.size() && depth != 0) {
            if (offset + 1 < text.size() && text[offset] == '/' && text[offset + 1] == '*') {
               ++depth;
               offset += 2;
            }
            else if (offset + 1 < text.size() && text[offset] == '*' && text[offset + 1] == '/') {
               --depth;
               offset += 2;
            }
            else ++offset;
         }
         continue;
      }
      const std::size_t begin = offset;
      while (offset < text.size() && (std::isalpha(static_cast<unsigned char>(text[offset])) || text[offset] == '_')) ++offset;
      if (begin == offset) return DetectedLanguage::Unknown;
      const std::string_view word(text.data() + begin, offset - begin);
      if (word == "TRANSFER") return DetectedLanguage::Ili1;
      if (word == "INTERLIS") return DetectedLanguage::Ili2;
      return DetectedLanguage::Unknown;
   }
   return DetectedLanguage::Unknown;
}

} // namespace ilic::detail
