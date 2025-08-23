#include "cscript_watcher.h"
#include <stddef.h> // For NULL

int cscript_watcher_get_mtime(const char* path, unsigned long long* out_ns) {
    (void)path; // Unused parameter
    if (out_ns) {
        *out_ns = 0;
    }
    // Return a non-zero value to indicate failure, as this is a stub.
    return -1;
}
