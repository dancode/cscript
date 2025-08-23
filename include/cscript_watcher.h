#ifndef CSCRIPT_WATCHER_H
#define CSCRIPT_WATCHER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the last modification time of a file.
 * @param path The path to the file.
 * @param out_ns Pointer to an unsigned long long to store the modification time in nanoseconds.
 * @return 0 on success, non-zero on failure.
 */
int cscript_watcher_get_mtime(const char* path, unsigned long long* out_ns);

#ifdef __cplusplus
}
#endif

#endif // CSCRIPT_WATCHER_H
