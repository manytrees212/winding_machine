#include "logger.h"
#include <stdio.h>
#include <string.h> // for strlen
#include <stdarg.h> // for va_list, va_start, va_end
#include <stddef.h>
#include <inttypes.h>

static Logger_OutputFunction_t s_output_func = NULL;

void Logger_Init(Logger_OutputFunction_t output_func){
    s_output_func = output_func;
}

void Logger_Log(LogLevel_t level, const char* file, uint32_t line,
                const char* fmt, ...){

    if (s_output_func == NULL || level > LOG_LEVEL){
        return;
    }

    char buffer[512];
    int offset = 0;

    const char* level_str;
    switch(level){
        case LOG_LEVEL_ERROR: level_str = "ERROR"; break;
        case LOG_LEVEL_WARN:  level_str = "WARN";  break;
        case LOG_LEVEL_INFO:  level_str = "INFO";  break;
        case LOG_LEVEL_DEBUG: level_str = "DEBUG"; break;
        default: return;
    }

    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    int written = snprintf(buffer, sizeof(buffer), "[%s] %s:%"PRIu32": ",
                      level_str, file, line);
    if (written < 0){
        return;
    }
    //if buffer is too small for written
    if ((size_t)written >= sizeof(buffer)){
        // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
        snprintf(buffer, sizeof(buffer), "[%s] Truncated log", level_str);
        s_output_func(buffer, strlen(buffer));
        return;
    }

    offset = written;

    va_list args;
    va_start(args, fmt);
    // NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
    vsnprintf(buffer + offset, sizeof(buffer) - offset, fmt, args);
    va_end(args);

    s_output_func(buffer, strlen(buffer));
}
