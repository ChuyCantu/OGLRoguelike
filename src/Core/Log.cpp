#include "Log.hpp"

#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> Log::log;

void Log::Init(const std::string& loggerName,
               // spdlog::level::level_enum level,
               const std::string& pattern) {
#if LOG_LEVEL < LEVEL_OFF
    spdlog::set_pattern(pattern);
    log = spdlog::stdout_color_mt(loggerName);
#endif

#if LOG_LEVEL == LEVEL_TRACE
    spdlog::set_level(spdlog::level::level_enum::trace);
#endif

#if LOG_LEVEL == LEVEL_DEBUG
    spdlog::set_level(spdlog::level::level_enum::debug);
#endif

#if LOG_LEVEL == LEVEL_INFO
    spdlog::set_level(spdlog::level::level_enum::info);
#endif

#if LOG_LEVEL == LEVEL_WARNING
    spdlog::set_level(spdlog::level::level_enum::warn);
#endif

#if LOG_LEVEL == LEVEL_ERROR
    spdlog::set_level(spdlog::level::level_enum::err);
#endif

#if LOG_LEVEL == LEVEL_CRITICAL
    spdlog::set_level(spdlog::level::level_enum::critical);
#endif

// #if LOG_LEVEL >= LEVEL_OFF
//     spdlog::set_level(spdlog::level::level_enum::off);
// #endif
}

static void SetPattern(const std::string& pattern) {
    spdlog::set_pattern(pattern);
}
