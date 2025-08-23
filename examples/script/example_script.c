#include "cscript_export.h"
#include "cscript_log.h"

// A simple static variable to demonstrate state changes.
static int call_count = 0;

CSCRIPT_API void on_load(int reason) {
    if (reason == 0) {
        CSCRIPT_LOG_INFO("Script loaded (cold). Initializing state.");
    } else {
        CSCRIPT_LOG_INFO("Script reloaded. Re-initializing state.");
    }
    // Reset state on load/reload to demonstrate lifecycle.
    call_count = 0;
}

CSCRIPT_API void on_unload(int reason) {
    if (reason == 0) {
        CSCRIPT_LOG_INFO("Script unloading (final).");
    } else {
        CSCRIPT_LOG_INFO("Script unloading for reload.");
    }
}

CSCRIPT_API void on_update(double dt) {
    call_count++;
    // This log is DEBUG level to avoid spamming the console too much.
    CSCRIPT_LOG_DEBUG("Script on_update called (dt=%.4f). Total calls: %d", dt, call_count);
}

CSCRIPT_API int add(int a, int b) {
    int result = a + b;
    CSCRIPT_LOG_DEBUG("Script add(%d, %d) called, returning %d.", a, b, result);
    return result;
}
