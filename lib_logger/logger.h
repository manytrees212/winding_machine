#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

typedef enum {
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_INFO  = 3,
    LOG_LEVEL_DEBUG = 4
} LogLevel_t;

typedef void (*Logger_OutputFunction_t)(const char* str, size_t len);

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_DEBUG //max level
#endif

void Logger_Init(Logger_OutputFunction_t output_func);

void Logger_Log(LogLevel_t level, const char* file, uint32_t line,
                const char* fmt, ...);

#if (LOG_LEVEL >= LOG_LEVEL_ERROR)
#define LOG_ERROR(fmt, ...) \
Logger_Log(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt"\n", ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...) ((void)0)
#endif


#if (LOG_LEVEL >= LOG_LEVEL_WARN)
#define LOG_WARN(fmt, ...) \
    Logger_Log(LOG_LEVEL_WARN, __FILE__, __LINE__, fmt"\n", ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...) ((void)0)
#endif


#if (LOG_LEVEL >= LOG_LEVEL_INFO)
#define LOG_INFO(fmt, ...) \
Logger_Log(LOG_LEVEL_INFO, __FILE__, __LINE__, fmt"\n", ##__VA_ARGS__)
#else
#define LOG_LEVEL_INFO(fmt, ...) ((void)0)
#endif


#if (LOG_LEVEL >= LOG_LEVEL_DEBUG)
#define LOG_DEBUG(fmt, ...) \
Logger_Log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt"\n", ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif //LOGGER_H
