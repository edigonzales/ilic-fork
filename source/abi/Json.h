#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

namespace ilic::json {

class Value {
public:
   using Array = std::vector<Value>;
   using Object = std::map<std::string,Value>;
   using Storage = std::variant<std::nullptr_t,bool,double,std::string,Array,Object>;

   Value() : value_(nullptr) {}
   Value(std::nullptr_t) : value_(nullptr) {}
   Value(bool value) : value_(value) {}
   Value(int value) : value_(static_cast<double>(value)) {}
   Value(std::size_t value) : value_(static_cast<double>(value)) {}
   Value(double value) : value_(value) {}
   Value(const char *value) : value_(std::string(value)) {}
   Value(std::string value) : value_(std::move(value)) {}
   Value(Array value) : value_(std::move(value)) {}
   Value(Object value) : value_(std::move(value)) {}

   bool isNull() const;
   bool isBool() const;
   bool isNumber() const;
   bool isString() const;
   bool isArray() const;
   bool isObject() const;
   bool boolean(bool fallback = false) const;
   double number(double fallback = 0) const;
   const std::string &string() const;
   const Array &array() const;
   const Object &object() const;
   const Value &get(const std::string &name) const;

private:
   Storage value_;
   friend std::string stringify(const Value &value);
};

Value parse(const std::string &text);
std::string stringify(const Value &value);

} // namespace ilic::json
