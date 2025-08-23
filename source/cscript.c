#include "cscript.h"
#include "cscript_platform.h"
#include "cscript_watcher.h"
#include "cscript_log.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_ERROR_LEN 256
static char g_last_error[MAX_ERROR_LEN] = {0};

struct cscript_module {
    cscript_lib_handle handle;
    char* path;
    unsigned long long loaded_mtime_ns;

    // Cached optional hooks
    cscript_on_load_fn on_load;
    cscript_on_unload_fn on_unload;
    cscript_on_update_fn on_update;
};

// --- Internal Helper Functions ---

static void set_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(g_last_error, MAX_ERROR_LEN, fmt, args);
    va_end(args);
    g_last_error[MAX_ERROR_LEN - 1] = '\0'; // Ensure null termination
}

static void cache_hooks(cscript_module* mod) {
    if (!mod || !mod->handle) return;
    mod->on_load = (cscript_on_load_fn)cscript_platform_get_symbol(mod->handle, "on_load");
    mod->on_unload = (cscript_on_unload_fn)cscript_platform_get_symbol(mod->handle, "on_unload");
    mod->on_update = (cscript_on_update_fn)cscript_platform_get_symbol(mod->handle, "on_update");
}

// --- Public API Implementation ---

const char* cscript_last_error(void) {
    return g_last_error;
}

cscript_module* cscript_load(const char* path) {
    if (!path) {
        set_error("Path cannot be null.");
        return NULL;
    }

    cscript_module* mod = (cscript_module*)calloc(1, sizeof(cscript_module));
    if (!mod) {
        set_error("Failed to allocate memory for module.");
        return NULL;
    }

    size_t path_len = strlen(path);
    mod->path = (char*)malloc(path_len + 1);
    if (!mod->path) {
        set_error("Failed to allocate memory for path string.");
        free(mod);
        return NULL;
    }
    memcpy(mod->path, path, path_len + 1);

    mod->handle = cscript_platform_load_library(path);
    if (!mod->handle) {
        set_error("Failed to load library '%s': %s", path, cscript_platform_last_error());
        free(mod->path);
        free(mod);
        return NULL;
    }

    if (cscript_watcher_get_mtime(path, &mod->loaded_mtime_ns) != 0) {
        CSCRIPT_LOG_WARN("Could not read modification time for '%s'. Hot-reloading may not work.", path);
        mod->loaded_mtime_ns = 0;
    }

    cache_hooks(mod);

    if (mod->on_load) {
        mod->on_load(0); // 0 = cold load
    }

    CSCRIPT_LOG_INFO("Module '%s' loaded.", path);
    return mod;
}

void cscript_unload(cscript_module* mod) {
    if (!mod) return;

    if (mod->handle) {
        if (mod->on_unload) {
            mod->on_unload(0); // 0 = final unload
        }
        cscript_platform_unload_library(mod->handle);
        CSCRIPT_LOG_INFO("Module '%s' unloaded.", mod->path);
    }

    free(mod->path);
    free(mod);
}

int cscript_bind_functions(cscript_module* mod, const cscript_bind* binds, int count) {
    if (!mod || !mod->handle) {
        set_error("Module is not loaded.");
        return 0;
    }
    if (!binds) return 0;

    int success_count = 0;
    for (int i = 0; i < count; ++i) {
        if (binds[i].name && binds[i].out_function_ptr) {
            void* sym = cscript_platform_get_symbol(mod->handle, binds[i].name);
            if (sym) {
                *binds[i].out_function_ptr = sym;
                success_count++;
            } else {
                CSCRIPT_LOG_WARN("Failed to bind function '%s': symbol not found.", binds[i].name);
                *binds[i].out_function_ptr = NULL;
            }
        }
    }

    if (success_count != count) {
        set_error("Failed to bind %d out of %d functions.", count - success_count, count);
    }

    return success_count;
}

int cscript_reload_if_changed(cscript_module* mod) {
    if (!mod || !mod->path) {
        set_error("Invalid module provided.");
        return -1; // Error
    }

    unsigned long long current_mtime_ns = 0;
    if (cscript_watcher_get_mtime(mod->path, &current_mtime_ns) != 0) {
        CSCRIPT_LOG_WARN("Could not get current mtime for '%s'. Cannot check for reload.", mod->path);
        return -1; // Error
    }

    if (current_mtime_ns == 0 || current_mtime_ns <= mod->loaded_mtime_ns) {
        return 0; // Not changed
    }

    CSCRIPT_LOG_INFO("Module '%s' has changed, attempting reload.", mod->path);

    if (mod->on_unload) {
        mod->on_unload(1); // 1 = pre-reload
    }

    cscript_platform_unload_library(mod->handle);
    mod->handle = cscript_platform_load_library(mod->path);

    if (!mod->handle) {
        set_error("Failed to reload library '%s': %s", mod->path, cscript_platform_last_error());
        CSCRIPT_LOG_ERROR("Reload failed for '%s'. Module is now in an unloaded state.", mod->path);
        mod->loaded_mtime_ns = 0;
        return -1; // Error
    }

    mod->loaded_mtime_ns = current_mtime_ns;
    cache_hooks(mod);

    if (mod->on_load) {
        mod->on_load(1); // 1 = reload
    }

    CSCRIPT_LOG_INFO("Module '%s' reloaded successfully.", mod->path);
    return 1; // Reloaded
}

unsigned long long cscript_last_loaded_mtime(const cscript_module* mod) {
    if (!mod) return 0;
    return mod->loaded_mtime_ns;
}

int cscript_is_loaded(const cscript_module* mod) {
    return (mod && mod->handle) ? 1 : 0;
}
