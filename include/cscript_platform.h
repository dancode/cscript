#ifndef CSCRIPT_PLATFORM_H
#define CSCRIPT_PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void* cscript_lib_handle;

cscript_lib_handle cscript_platform_load_library(const char* path);
void* cscript_platform_get_symbol(cscript_lib_handle h, const char* name);
void cscript_platform_unload_library(cscript_lib_handle h);
const char* cscript_platform_last_error(void);

#ifdef __cplusplus
}
#endif

#endif // CSCRIPT_PLATFORM_H
