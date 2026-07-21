#pragma once

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32)
#  define ILIC_CAPI_EXPORT __declspec(dllexport)
#else
#  define ILIC_CAPI_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

ILIC_CAPI_EXPORT uint32_t ilic_abi_version(void);
ILIC_CAPI_EXPORT const char *ilic_version(void);
/* Memory returned by ilic_alloc must be released with ilic_free. */
ILIC_CAPI_EXPORT void *ilic_alloc(size_t length);
ILIC_CAPI_EXPORT void ilic_free(void *memory);

/* Session functions copy URI and UTF-8 source bytes; caller buffers remain caller-owned. */
ILIC_CAPI_EXPORT uint32_t ilic_session_create(void);
ILIC_CAPI_EXPORT void ilic_session_destroy(uint32_t session);
ILIC_CAPI_EXPORT int32_t ilic_session_put_source(uint32_t session,
   const char *uri,size_t uri_length,const uint8_t *utf8,size_t utf8_length,uint64_t document_version);
ILIC_CAPI_EXPORT int32_t ilic_session_remove_source(uint32_t session,
   const char *uri,size_t uri_length);

ILIC_CAPI_EXPORT uint32_t ilic_compile(uint32_t session,
   const char *request_json,size_t request_length);
ILIC_CAPI_EXPORT uint32_t ilic_parse(uint32_t session,
   const char *request_json,size_t request_length);
ILIC_CAPI_EXPORT uint32_t ilic_analyze(uint32_t session,
   const char *request_json,size_t request_length);
ILIC_CAPI_EXPORT uint32_t ilic_compile_and_analyze(uint32_t session,
   const char *request_json,size_t request_length);
ILIC_CAPI_EXPORT uint32_t ilic_format(uint32_t session,
   const char *request_json,size_t request_length);

/* Result JSON remains valid until ilic_result_destroy is called for the handle. */
ILIC_CAPI_EXPORT const char *ilic_result_json(uint32_t result,size_t *result_length);
ILIC_CAPI_EXPORT void ilic_result_destroy(uint32_t result);

#ifdef __cplusplus
}
#endif
