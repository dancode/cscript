#include "cscript_watcher.h"
#include <windows.h>

int cscript_watcher_get_mtime(const char* path, unsigned long long* out_ns) {
    if (!out_ns) {
        return -1; // Invalid argument
    }

    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &file_info)) {
        *out_ns = 0;
        return -1; // Failure to get attributes
    }

    // FILETIME contains a 64-bit value representing the number of
    // 100-nanosecond intervals since January 1, 1601 (UTC).
    ULARGE_INTEGER uli;
    uli.LowPart = file_info.ftLastWriteTime.dwLowDateTime;
    uli.HighPart = file_info.ftLastWriteTime.dwHighDateTime;

    // Convert from 100-nanosecond intervals to nanoseconds by multiplying by 100.
    *out_ns = uli.QuadPart * 100;

    return 0; // Success
}
