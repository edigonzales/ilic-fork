#include "ilic/capi.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int contains(const char *data,size_t length,const char *needle)
{
   size_t needle_length = strlen(needle);
   if (needle_length > length) return 0;
   for (size_t offset = 0; offset + needle_length <= length; ++offset) {
      if (memcmp(data + offset,needle,needle_length) == 0) return 1;
   }
   return 0;
}

int main(void)
{
   static const char uri[] = "memory:///CExample.ili";
   static const char source[] =
      "INTERLIS 2.3;\n"
      "MODEL CExample AT \"https://example.invalid/ilic/docs\" VERSION \"1\" =\n"
      "END CExample.\n";
   static const char request[] =
      "{\"schemaVersion\":1,\"roots\":[\"memory:///CExample.ili\"]}";

   uint32_t session = ilic_session_create();
   if (session == 0) return 1;
   if (ilic_session_put_source(session,uri,strlen(uri),
      (const uint8_t *)source,strlen(source),1) != 0) {
      ilic_session_destroy(session);
      return 2;
   }

   uint32_t result = ilic_compile(session,request,strlen(request));
   size_t result_length = 0;
   const char *json = ilic_result_json(result,&result_length);
   if (json == NULL) {
      ilic_result_destroy(result);
      ilic_session_destroy(session);
      return 3;
   }
   fwrite(json,1,result_length,stdout);
   fputc('\n',stdout);

   int success = contains(json,result_length,"\"success\":true");
   ilic_result_destroy(result);
   ilic_session_destroy(session);
   return success ? 0 : 4;
}
