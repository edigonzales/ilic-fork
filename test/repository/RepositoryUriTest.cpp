#include "RepositoryUri.h"

#include <cassert>
#include <filesystem>
#include <string>

using ilic::repository::RepositoryUri;
using ilic::repository::RepositoryUriKind;

int main()
{
   std::string error;
   auto http = RepositoryUri::parse("https://example.org/models/",&error);
   assert(http && http->kind() == RepositoryUriKind::Https);
   assert(http->normalized() == "https://example.org/models");
   assert(http->cacheKey() == RepositoryUri::parse("https://example.org/models")->cacheKey());
   assert(http->resolve("sub/Model.ili").normalized()
      == "https://example.org/models/sub/Model.ili");
   assert(http->resolve("../shared").normalized() == "https://example.org/shared");
   assert(http->resolve("/root").normalized() == "https://example.org/root");
   assert(http->resolve("//cdn.example.org/models").normalized()
      == "https://cdn.example.org/models");
   assert(RepositoryUri::parse("https://example.org/A")->cacheKey()
      != RepositoryUri::parse("https://example.org/a")->cacheKey());
   assert(RepositoryUri::parse("https://example.org/models/?token=x#part")->normalized()
      == "https://example.org/models?token=x#part");

   auto local = RepositoryUri::parse("./models/");
   assert(local && local->kind() == RepositoryUriKind::LocalPath);
   assert(local->resolve("subdir\\Model.ili").normalized() == "models/subdir/Model.ili");
   assert(RepositoryUri::parse("C:\\models")->normalized() == "C:/models");
   assert(RepositoryUri::parse("\\\\server\\share\\models")->normalized()
      == "//server/share/models");

   auto file = RepositoryUri::parse("file:///tmp/my%20models");
   assert(file && file->kind() == RepositoryUriKind::FileUri);
   assert(file->toLocalPath().generic_string() == "/tmp/my models");
#ifdef _WIN32
   assert(file->toLocalPath().string() == "\\tmp\\my models");
#endif
   assert(file->resolve("Model.ili").normalized() == "file:///tmp/my models/Model.ili");
   assert(RepositoryUri::parse("file:///C:/models")->normalized() == "file:///C:/models");
   assert(RepositoryUri::parse("file://server/share/models")->normalized()
      == "file://server/share/models");
   assert(!RepositoryUri::parse("ftp://example.org/models",&error));

   for (const std::string value : {"Model.ili","subdir/Model.ili","subdir\\nested\\Model.ili"})
      assert(ilic::repository::validateRepositoryRelativePath(value).valid);
   for (const std::string value : {"../Model.ili","a/../../Model.ili","/absolute/Model.ili",
      "C:\\absolute\\Model.ili","C:/absolute/Model.ili","\\\\server\\share\\Model.ili",
      "file:///tmp/Model.ili","http://example/Model.ili","https://example/Model.ili"})
      assert(!ilic::repository::validateRepositoryRelativePath(value).valid);

   assert(ilic::repository::isPathWithin("/tmp/repo","/tmp/repo/sub/Model.ili"));
   assert(!ilic::repository::isPathWithin("/tmp/repo","/tmp/repository-escape/Model.ili"));
   return 0;
}
