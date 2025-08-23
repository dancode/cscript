#include "cscript_platform.h"
#include <stddef.h> // For NULL

static const char* g_posix_stub_error = "POSIX platform is a stub and not yet implemented.";

cscript_lib_handle cscript_platform_load_library(const char* path) {
    (void)path; // Unused parameter
    return NULL;
}

void* cscript_platform_get_symbol(cscript_lib_handle h, const char* name) {
    (void)h;      // Unused parameter
    (void)name;   // Unused parameter
    return NULL;
}

void cscript_platform_unload_library(cscript_lib_handle h) {
    (void)h; // Unused parameter
}

const char* cscript_platform_last_error(void) {
    return g_posix_stub_error;
}
