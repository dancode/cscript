#ifndef CSCRIPT_H
#define CSCRIPT_H

#ifdef __cplusplus
extern "C" {
#endif

// Forward declare the opaque struct
typedef struct cscript_module cscript_module;

typedef struct cscript_bind {
    const char* name;
    void** out_function_ptr;
} cscript_bind;

// Lifecycle hook function pointer types
typedef void (*cscript_on_load_fn)(int reason);
typedef void (*cscript_on_unload_fn)(int reason);
typedef void (*cscript_on_update_fn)(double dt);

// Main API functions
cscript_module* cscript_load(const char* path);
void cscript_unload(cscript_module* mod);

int cscript_bind_functions(cscript_module* mod, const cscript_bind* binds, int count);
int cscript_reload_if_changed(cscript_module* mod);

const char* cscript_last_error(void);
unsigned long long cscript_last_loaded_mtime(const cscript_module* mod);
int cscript_is_loaded(const cscript_module* mod);

#ifdef __cplusplus
}
#endif

#endif // CSCRIPT_H
