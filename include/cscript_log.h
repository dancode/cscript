#ifndef CSCRIPT_LOG_H
#define CSCRIPT_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CSCRIPT_LOG_LEVEL_ERROR 0
#define CSCRIPT_LOG_LEVEL_WARN  1
#define CSCRIPT_LOG_LEVEL_INFO  2
#define CSCRIPT_LOG_LEVEL_DEBUG 3

void cscript_log_set_level(int level);
void cscript_log(int level, const char* file, int line, const char* fmt, ...);

#define CSCRIPT_LOG(level, ...) cscript_log(level, __FILE__, __LINE__, __VA_ARGS__)

#define CSCRIPT_LOG_ERROR(...) CSCRIPT_LOG(CSCRIPT_LOG_LEVEL_ERROR, __VA_ARGS__)
#define CSCRIPT_LOG_WARN(...)  CSCRIPT_LOG(CSCRIPT_LOG_LEVEL_WARN, __VA_ARGS__)
#define CSCRIPT_LOG_INFO(...)  CSCRIPT_LOG(CSCRIPT_LOG_LEVEL_INFO, __VA_ARGS__)
#define CSCRIPT_LOG_DEBUG(...) CSCRIPT_LOG(CSCRIPT_LOG_LEVEL_DEBUG, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // CSCRIPT_LOG_H
