#include "cscript.h"
#include "cscript_log.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
// usleep is deprecated but suitable for a simple example.
#define Sleep(x) usleep((x)*1000)
#endif

// Function pointer types for the functions we want to call from the script
typedef int (*add_func_t)(int, int);
typedef void (*on_update_func_t)(double);

int main(int argc, char* argv[]) {
    cscript_log_set_level(CSCRIPT_LOG_LEVEL_DEBUG);

    if (argc < 2) {
        CSCRIPT_LOG_ERROR("Usage: %s <path_to_script_module>", argv[0]);
        return 1;
    }

    const char* script_path = argv[1];
    CSCRIPT_LOG_INFO("Loading script module: %s", script_path);

    cscript_module* mod = cscript_load(script_path);
    if (!mod) {
        CSCRIPT_LOG_ERROR("Failed to load script: %s", cscript_last_error());
        return 1;
    }

    // Pointers to the script's functions
    add_func_t my_add = NULL;
    on_update_func_t my_on_update = NULL;

    // Bind functions from the newly loaded module
    cscript_bind binds[] = {
        {"add", (void**)&my_add},
        {"on_update", (void**)&my_on_update}
    };

    int bound_count = cscript_bind_functions(mod, binds, 2);
    CSCRIPT_LOG_INFO("Bound %d functions.", bound_count);

    if (my_add) {
        CSCRIPT_LOG_INFO("Initial call: add(5, 7) = %d", my_add(5, 7));
    }

    CSCRIPT_LOG_INFO("Entering main loop. Press Ctrl+C to exit.");
    CSCRIPT_LOG_INFO("You can now modify and recompile the script to test hot-reloading.");

    double dt = 0.016; // Fake delta time
    while (1) {
        Sleep(1000); // Check for changes once per second

        int reload_status = cscript_reload_if_changed(mod);
        if (reload_status < 0) {
            CSCRIPT_LOG_ERROR("Error during reload check: %s", cscript_last_error());
            break; // Exit loop on critical error
        }

        if (reload_status > 0) {
            CSCRIPT_LOG_INFO("Module was reloaded!");
            // Pointers are now invalid, so re-bind them
            my_add = NULL;
            my_on_update = NULL;
            bound_count = cscript_bind_functions(mod, binds, 2);
            CSCRIPT_LOG_INFO("Re-bound %d functions.", bound_count);
            if (my_add) {
                CSCRIPT_LOG_INFO("Post-reload call: add(10, 20) = %d", my_add(10, 20));
            }
        }

        // Call the script's update function if it's loaded and the pointer is valid
        if (cscript_is_loaded(mod) && my_on_update) {
            my_on_update(dt);
        }
    }

    CSCRIPT_LOG_INFO("Shutting down.");
    cscript_unload(mod);

    return 0;
}
