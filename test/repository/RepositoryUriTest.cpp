#include "RepositoryUri.h"

#include "ilic/test/TestHarness.h"
#include <filesystem>
#include <string>

using ilic::repository::RepositoryUri;
using ilic::repository::RepositoryUriKind;

int main()
{
   std::string error;
   auto http = RepositoryUri::parse("https://example.org/models/",&error);
   ILIC_REQUIRE(http.has_value());
   ILIC_REQUIRE(http->kind() == RepositoryUriKind::Https);
   ILIC_REQUIRE(http->normalized() == "https://example.org/models");
   auto sameHttp = RepositoryUri::parse("https://example.org/models");
   ILIC_REQUIRE(sameHttp.has_value());
   ILIC_REQUIRE(http->cacheKey() == sameHttp->cacheKey());
   ILIC_REQUIRE(http->resolve("sub/Model.ili").normalized()
      == "https://example.org/models/sub/Model.ili");
   ILIC_REQUIRE(http->resolve("../shared").normalized() == "https://example.org/shared");
   ILIC_REQUIRE(http->resolve("/root").normalized() == "https://example.org/root");
   ILIC_REQUIRE(http->resolve("//cdn.example.org/models").normalized()
      == "https://cdn.example.org/models");
   auto upperCase = RepositoryUri::parse("https://example.org/A");
   auto lowerCase = RepositoryUri::parse("https://example.org/a");
   ILIC_REQUIRE(upperCase.has_value());
   ILIC_REQUIRE(lowerCase.has_value());
   ILIC_REQUIRE(upperCase->cacheKey() != lowerCase->cacheKey());
   auto query = RepositoryUri::parse("https://example.org/models/?token=x#part");
   ILIC_REQUIRE(query.has_value());
   ILIC_REQUIRE(query->normalized() == "https://example.org/models?token=x#part");

   auto local = RepositoryUri::parse("./models/");
   ILIC_REQUIRE(local.has_value());
   ILIC_REQUIRE(local->kind() == RepositoryUriKind::LocalPath);
   ILIC_REQUIRE(local->resolve("subdir\\Model.ili").normalized() == "models/subdir/Model.ili");
   auto drive = RepositoryUri::parse("C:\\models");
   auto unc = RepositoryUri::parse("\\\\server\\share\\models");
   ILIC_REQUIRE(drive.has_value());
   ILIC_REQUIRE(unc.has_value());
   ILIC_REQUIRE(drive->normalized() == "C:/models");
   ILIC_REQUIRE(unc->normalized() == "//server/share/models");

   auto file = RepositoryUri::parse("file:///tmp/my%20models");
   ILIC_REQUIRE(file.has_value());
   ILIC_REQUIRE(file->kind() == RepositoryUriKind::FileUri);
   ILIC_REQUIRE(file->toLocalPath().generic_string() == "/tmp/my models");
#ifdef _WIN32
   ILIC_REQUIRE(file->toLocalPath().string() == "\\tmp\\my models");
#endif
   ILIC_REQUIRE(file->resolve("Model.ili").normalized() == "file:///tmp/my models/Model.ili");
   auto driveFile = RepositoryUri::parse("file:///C:/models");
   auto serverFile = RepositoryUri::parse("file://server/share/models");
   ILIC_REQUIRE(driveFile.has_value());
   ILIC_REQUIRE(serverFile.has_value());
   ILIC_REQUIRE(driveFile->normalized() == "file:///C:/models");
   ILIC_REQUIRE(serverFile->normalized() == "file://server/share/models");
   ILIC_REQUIRE(!RepositoryUri::parse("ftp://example.org/models",&error));

   for (const std::string value : {"Model.ili","subdir/Model.ili","subdir\\nested\\Model.ili"})
      ILIC_REQUIRE(ilic::repository::validateRepositoryRelativePath(value).valid);
   for (const std::string value : {"../Model.ili","a/../../Model.ili","/absolute/Model.ili",
      "C:\\absolute\\Model.ili","C:/absolute/Model.ili","\\\\server\\share\\Model.ili",
      "file:///tmp/Model.ili","http://example/Model.ili","https://example/Model.ili"})
      ILIC_REQUIRE(!ilic::repository::validateRepositoryRelativePath(value).valid);

   ILIC_REQUIRE(ilic::repository::isPathWithin("/tmp/repo","/tmp/repo/sub/Model.ili"));
   ILIC_REQUIRE(!ilic::repository::isPathWithin("/tmp/repo","/tmp/repository-escape/Model.ili"));
   return 0;
}
