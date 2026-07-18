#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace ilic {

struct SourcePosition {
   std::size_t offset = 0;
   std::size_t line = 0;
   std::size_t utf16Column = 0;
};

struct SourceBuffer {
   std::string uri;
   std::string text;
   std::uint64_t version = 0;
};

class SourceManager {
public:
   void put(std::string uri, std::string utf8, std::uint64_t version = 0);
   bool remove(const std::string &uri);
   bool contains(const std::string &uri) const;
   const SourceBuffer *get(const std::string &uri) const;
   std::vector<std::string> uris() const;
   SourcePosition position(const std::string &uri, std::size_t byteOffset) const;

private:
   std::map<std::string, SourceBuffer> sources_;
};

SourceManager *activeSourceManager();
void setActiveSourceManager(SourceManager *manager);

class ActiveSourceManagerScope {
public:
   explicit ActiveSourceManagerScope(SourceManager *manager);
   ~ActiveSourceManagerScope();

   ActiveSourceManagerScope(const ActiveSourceManagerScope &) = delete;
   ActiveSourceManagerScope &operator=(const ActiveSourceManagerScope &) = delete;

private:
   SourceManager *previous_;
};

} // namespace ilic
