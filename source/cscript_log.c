#include "cscript_log.h"
#include <stdio.h>
#include <time.h>

// Default log level is INFO
static int g_log_level = CSCRIPT_LOG_LEVEL_INFO;

void cscript_log_set_level(int level) {
    g_log_level = level;
}

void cscript_log(int level, const char* file, int line, const char* fmt, ...) {
    if (level > g_log_level) {
        return;
    }

    // Get current time
    time_t timer;
    char time_buffer[26];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(time_buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    // Get log level string
    const char* level_str = "UNKNOWN";
    switch(level) {
        case CSCRIPT_LOG_LEVEL_ERROR: level_str = "ERROR"; break;
        case CSCRIPT_LOG_LEVEL_WARN:  level_str = "WARN";  break;
        case CSCRIPT_LOG_LEVEL_INFO:  level_str = "INFO";  break;
        case CSCRIPT_LOG_LEVEL_DEBUG: level_str = "DEBUG"; break;
    }

    // Print log prefix to stderr
    fprintf(stderr, "[%s] [%-5s] [%s:%d] ", time_buffer, level_str, file, line);

    // Print the actual log message
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    // Print newline and flush the stream
    fprintf(stderr, "\n");
    fflush(stderr);
}
