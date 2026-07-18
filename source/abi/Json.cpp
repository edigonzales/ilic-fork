#include "Json.h"

#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace ilic::json {
namespace {

class Parser {
public:
   explicit Parser(const std::string &text) : text_(text) {}

   Value parseValue()
   {
      whitespace();
      if (position_ >= text_.size()) fail("unexpected end of JSON");
      const char current = text_[position_];
      if (current == '{') return object();
      if (current == '[') return array();
      if (current == '"') return Value(string());
      if (current == 't') return literal("true",Value(true));
      if (current == 'f') return literal("false",Value(false));
      if (current == 'n') return literal("null",Value(nullptr));
      if (current == '-' || std::isdigit(static_cast<unsigned char>(current))) return number();
      fail("unexpected character");
      return {};
   }

   void finish()
   {
      whitespace();
      if (position_ != text_.size()) fail("trailing JSON content");
   }

private:
   const std::string &text_;
   std::size_t position_ = 0;

   [[noreturn]] void fail(const std::string &message) const
   {
      throw std::runtime_error(message + " at byte " + std::to_string(position_));
   }

   void whitespace()
   {
      while (position_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[position_]))) ++position_;
   }

   bool consume(char value)
   {
      whitespace();
      if (position_ < text_.size() && text_[position_] == value) { ++position_; return true; }
      return false;
   }

   Value object()
   {
      ++position_;
      Value::Object result;
      if (consume('}')) return result;
      while (true) {
         whitespace();
         if (position_ >= text_.size() || text_[position_] != '"') fail("expected object key");
         std::string key = string();
         if (!consume(':')) fail("expected ':'");
         result[std::move(key)] = parseValue();
         if (consume('}')) break;
         if (!consume(',')) fail("expected ','");
      }
      return result;
   }

   Value array()
   {
      ++position_;
      Value::Array result;
      if (consume(']')) return result;
      while (true) {
         result.push_back(parseValue());
         if (consume(']')) break;
         if (!consume(',')) fail("expected ','");
      }
      return result;
   }

   std::string string()
   {
      if (text_[position_++] != '"') fail("expected string");
      std::string result;
      while (position_ < text_.size()) {
         char current = text_[position_++];
         if (current == '"') return result;
         if (current != '\\') { result += current; continue; }
         if (position_ >= text_.size()) fail("incomplete escape");
         char escaped = text_[position_++];
         switch (escaped) {
            case '"': result += '"'; break;
            case '\\': result += '\\'; break;
            case '/': result += '/'; break;
            case 'b': result += '\b'; break;
            case 'f': result += '\f'; break;
            case 'n': result += '\n'; break;
            case 'r': result += '\r'; break;
            case 't': result += '\t'; break;
            case 'u': {
               if (position_ + 4 > text_.size()) fail("incomplete unicode escape");
               unsigned codepoint = 0;
               for (int i = 0; i < 4; ++i) {
                  char digit = text_[position_++];
                  codepoint <<= 4;
                  if (digit >= '0' && digit <= '9') codepoint += digit - '0';
                  else if (digit >= 'a' && digit <= 'f') codepoint += digit - 'a' + 10;
                  else if (digit >= 'A' && digit <= 'F') codepoint += digit - 'A' + 10;
                  else fail("invalid unicode escape");
               }
               if (codepoint <= 0x7f) result += static_cast<char>(codepoint);
               else if (codepoint <= 0x7ff) {
                  result += static_cast<char>(0xc0 | (codepoint >> 6));
                  result += static_cast<char>(0x80 | (codepoint & 0x3f));
               }
               else {
                  result += static_cast<char>(0xe0 | (codepoint >> 12));
                  result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f));
                  result += static_cast<char>(0x80 | (codepoint & 0x3f));
               }
               break;
            }
            default: fail("invalid escape");
         }
      }
      fail("unterminated string");
      return {};
   }

   Value number()
   {
      std::size_t start = position_;
      if (text_[position_] == '-') ++position_;
      while (position_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[position_]))) ++position_;
      if (position_ < text_.size() && text_[position_] == '.') {
         ++position_;
         while (position_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[position_]))) ++position_;
      }
      if (position_ < text_.size() && (text_[position_] == 'e' || text_[position_] == 'E')) {
         ++position_;
         if (position_ < text_.size() && (text_[position_] == '+' || text_[position_] == '-')) ++position_;
         while (position_ < text_.size() && std::isdigit(static_cast<unsigned char>(text_[position_]))) ++position_;
      }
      return std::stod(text_.substr(start,position_ - start));
   }

   Value literal(const char *literal,Value value)
   {
      std::string expected(literal);
      if (text_.compare(position_,expected.size(),expected) != 0) fail("invalid literal");
      position_ += expected.size();
      return value;
   }
};

std::string escaped(const std::string &value)
{
   std::ostringstream result;
   result << '"';
   for (unsigned char character : value) {
      switch (character) {
         case '"': result << "\\\""; break;
         case '\\': result << "\\\\"; break;
         case '\b': result << "\\b"; break;
         case '\f': result << "\\f"; break;
         case '\n': result << "\\n"; break;
         case '\r': result << "\\r"; break;
         case '\t': result << "\\t"; break;
         default:
            if (character < 0x20) result << "\\u" << std::hex << std::setw(4)
               << std::setfill('0') << static_cast<int>(character);
            else result << static_cast<char>(character);
      }
   }
   result << '"';
   return result.str();
}

}

bool Value::isNull() const { return std::holds_alternative<std::nullptr_t>(value_); }
bool Value::isBool() const { return std::holds_alternative<bool>(value_); }
bool Value::isNumber() const { return std::holds_alternative<double>(value_); }
bool Value::isString() const { return std::holds_alternative<std::string>(value_); }
bool Value::isArray() const { return std::holds_alternative<Array>(value_); }
bool Value::isObject() const { return std::holds_alternative<Object>(value_); }
bool Value::boolean(bool fallback) const { return isBool() ? std::get<bool>(value_) : fallback; }
double Value::number(double fallback) const { return isNumber() ? std::get<double>(value_) : fallback; }
const std::string &Value::string() const { static const std::string empty; return isString() ? std::get<std::string>(value_) : empty; }
const Value::Array &Value::array() const { static const Array empty; return isArray() ? std::get<Array>(value_) : empty; }
const Value::Object &Value::object() const { static const Object empty; return isObject() ? std::get<Object>(value_) : empty; }
const Value &Value::get(const std::string &name) const
{
   static const Value missing;
   if (!isObject()) return missing;
   auto found = std::get<Object>(value_).find(name);
   return found == std::get<Object>(value_).end() ? missing : found->second;
}

Value parse(const std::string &text)
{
   Parser parser(text);
   Value result = parser.parseValue();
   parser.finish();
   return result;
}

std::string stringify(const Value &value)
{
   if (value.isNull()) return "null";
   if (value.isBool()) return value.boolean() ? "true" : "false";
   if (value.isNumber()) {
      std::ostringstream result;
      result << std::setprecision(15) << value.number();
      return result.str();
   }
   if (value.isString()) return escaped(value.string());
   if (value.isArray()) {
      std::string result = "[";
      for (std::size_t i = 0; i < value.array().size(); ++i) {
         if (i != 0) result += ',';
         result += stringify(value.array()[i]);
      }
      return result + ']';
   }
   std::string result = "{";
   std::size_t index = 0;
   for (const auto &entry : value.object()) {
      if (index++ != 0) result += ',';
      result += escaped(entry.first) + ':' + stringify(entry.second);
   }
   return result + '}';
}

} // namespace ilic::json
