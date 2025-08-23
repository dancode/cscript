#include "cscript_platform.h"
#include <windows.h>

// Use a thread-local buffer for the last error message to ensure thread safety.
static __declspec(thread) char g_last_error_buffer[256];

cscript_lib_handle cscript_platform_load_library(const char* path) {
    return LoadLibraryA(path);
}

void* cscript_platform_get_symbol(cscript_lib_handle h, const char* name) {
    return (void*)GetProcAddress((HMODULE)h, name);
}

void cscript_platform_unload_library(cscript_lib_handle h) {
    if (h) {
        FreeLibrary((HMODULE)h);
    }
}

const char* cscript_platform_last_error(void) {
    DWORD error_code = GetLastError();
    if (error_code == 0) {
        g_last_error_buffer[0] = '\0';
        return g_last_error_buffer;
    }

    DWORD len = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        g_last_error_buffer,
        sizeof(g_last_error_buffer),
        NULL
    );

    // Remove trailing newline characters from the formatted message.
    if (len > 0) {
        if (g_last_error_buffer[len - 1] == '\n') {
            g_last_error_buffer[len - 1] = '\0';
            if (len > 1 && g_last_error_buffer[len - 2] == '\r') {
                g_last_error_buffer[len - 2] = '\0';
            }
        }
    }

    return g_last_error_buffer;
}
