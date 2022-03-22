#pragma once

//* Define any of the next macros to set the log level.
//* If multiple log levels are defined, the one with higher level will be used.
//+ These macros can be defined in here, before including the Log.hpp file, of via your build system (like in this case, cmake)

// #define LOG_LEVEL_TRACE
// #define LOG_LEVEL_DEBUG
// #define LOG_LEVEL_INFO
// #define LOG_LEVEL_WARNING
// #define LOG_LEVEL_ERROR
// #define LOG_LEVEL_CRITICAL
// #define LOG_LEVEL_OFF

#include "spdlog/spdlog.h"

#include <string.h>

#define LEVEL_TRACE 0
#define LEVEL_DEBUG 1
#define LEVEL_INFO 2
#define LEVEL_WARNING 3
#define LEVEL_ERROR 4
#define LEVEL_CRITICAL 5
#define LEVEL_OFF 6

#ifdef LOG_LEVEL_TRACE
#define LOG_LEVEL LEVEL_TRACE
#endif
#ifdef LOG_LEVEL_DEBUG
#define LOG_LEVEL LEVEL_DEBUG
#endif
#ifdef LOG_LEVEL_INFO
#define LOG_LEVEL LEVEL_INFO
#endif
#ifdef LOG_LEVEL_WARNING
#define LOG_LEVEL LEVEL_WARNING
#endif
#ifdef LOG_LEVEL_ERROR
#define LOG_LEVEL LEVEL_ERROR
#endif
#ifdef LOG_LEVEL_CRITICAL
#define LOG_LEVEL LEVEL_CRITICAL
#endif
#ifdef LOG_LEVEL_OFF
#define LOG_LEVEL LEVEL_OFF
#endif

class Log {
private:
    static std::shared_ptr<spdlog::logger> log;

public:
    static void Init(const std::string& loggerName = "CORE",
                     // spdlog::level::level_enum level = spdlog::level::trace,
                     const std::string& pattern = "%^[%d-%m-%Y %H:%M:%S] [%l] %n: %v%$");  //* <color>[day-month-year hour-min-sec] [logLevel] loggerName: string</color>

    static void SetPattern(const std::string& pattern);

    static inline std::shared_ptr<spdlog::logger> GetLogger() { return log; }
};

// File + line: https://stackoverflow.com/questions/19343205/c-concatenating-file-and-line-macros/19343239
#define _S1(x) #x
#define _S2(x) _S1(x)
#define __FILE_LINE__ "(" __FILE__ ":" _S2(__LINE__) ")"
// https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
#ifdef WIN32
#define __FILE_LINE_RELATIVE__ "(" + std::string{(strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)} + ":" _S2(__LINE__) ")"
#else
#define __FILE_LINE_RELATIVE__ "(" + std::string{(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)} + ":" _S2(__LINE__) ")"
#endif


#if LOG_LEVEL <= LEVEL_TRACE
#define LOG_TRACE(message, ...) Log::GetLogger()->trace(message, __VA_ARGS__);
#define LOGIF_TRACE(condition, message, ...) if (condition) LOG_TRACE(message,  __VA_ARGS__)
#else
#define LOG_TRACE(message, ...) (void)0
#define LOGIF_TRACE(condition, message, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_DEBUG
#define LOG_DEBUG(message, ...) Log::GetLogger()->debug(message, __VA_ARGS__);
#define LOGIF_DEBUG(condition, message, ...) if (condition) LOG_DEBUG(message,  __VA_ARGS__)

#else
#define LOG_DEBUG(message, ...) (void)0
#define LOGIF_DEBUG(condition, message, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_INFO
#define LOG_INFO(message, ...) Log::GetLogger()->info(message, __VA_ARGS__);
#define LOGIF_INFO(condition, message, ...) if (condition) LOG_INFO(message,  __VA_ARGS__)
#else
#define LOG_INFO(message, ...) (void)0
#define LOGIF_INFO(condition, message, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_WARNING
#define LOG_WARN(message, ...) Log::GetLogger()->warn(message, __VA_ARGS__);
#define LOGIF_WARN(condition, message, ...) if (condition) LOG_WARN(message,  __VA_ARGS__)
#else
#define LOG_WARN(message, ...) (void)0
#define LOGIF_WARN(condition, message, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_ERROR
#define LOG_ERROR(message, ...) Log::GetLogger()->error(message " " __FILE_LINE_RELATIVE__, __VA_ARGS__);
#define LOGIF_ERROR(condition, message, ...) if (condition) LOG_ERROR(message,  __VA_ARGS__)
#else
#define LOG_ERROR(message, ...) (void)0
#define LOGIF_ERROR(condition, message, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_CRITICAL
#define LOG_CRITICAL(message, ...) Log::GetLogger()->critical(message " " __FILE_LINE_RELATIVE__, __VA_ARGS__);
#define LOGIF_CRITICAL(condition, message, ...) if (condition) LOG_CRITICAL(message,  __VA_ARGS__)
#define ASSERT(condition, message, ...) if (condition) { LOG_CRITICAL(message, __VA_ARGS__) assert(false); }
#else
#define LOG_CRITICAL(message, ...) (void)0
#define LOGIF_CRITICAL(condition, message, ...) (void)0
#define ASSERT() (void)0
#endif