#ifndef __LOG_H__
#define __LOG_H__

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

#if LOG_LEVEL <= LEVEL_TRACE
#define LOG_TRACE(str, ...) Log::GetLogger()->trace(str, __VA_ARGS__);
#else
#define LOG_TRACE(str, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_DEBUG
#define LOG_DEBUG(str, ...) Log::GetLogger()->debug(str, __VA_ARGS__);
#else
#define LOG_DEBUG(str, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_INFO
#define LOG_INFO(str, ...) Log::GetLogger()->info(str, __VA_ARGS__);
#else
#define LOG_INFO(str, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_WARNING
#define LOG_WARN(str, ...) Log::GetLogger()->warn(str, __VA_ARGS__);
#else
#define LOG_WARN(str, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_ERROR
#define LOG_ERROR(str, ...) Log::GetLogger()->error(str " " __FILE_LINE__, __VA_ARGS__);
#else
#define LOG_ERROR(str, ...) (void)0
#endif

#if LOG_LEVEL <= LEVEL_CRITICAL
#define LOG_CRITICAL(str, ...) Log::GetLogger()->critical(str " " __FILE_LINE__, __VA_ARGS__);
#else
#define LOG_CRITICAL(str, ...) (void)0
#endif

#endif // __LOG_H__