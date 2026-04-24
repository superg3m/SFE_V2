#pragma once

#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_PURPLE "\033[0;35m"

#define RED_BACKGROUND "\033[41m"
#define COLOR_RESET "\033[0m"

#define LOG_WARNING_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if defined(RELEASE)
	#define LOG_DEBUG_ENABLED 0
	#define LOG_TRACE_ENABLED 0
#endif

typedef enum LogLevel {
	LOG_LEVEL_FATAL = 0,
	LOG_LEVEL_ERROR = 1,
	LOG_LEVEL_WARN  = 2,
	LOG_LEVEL_INFO  = 3,
	LOG_LEVEL_DEBUG = 4,
	LOG_LEVEL_TRACE = 5,
	LOG_LEVEL_COUNT = 6
} LogLevel;

void LOG_OUTPUT(LogLevel log_level, const char* message, ...);
#define LOG_FATAL(message, ...) LOG_OUTPUT(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) LOG_OUTPUT(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)

#if LOG_WARNING_ENABLED == 1
	#define LOG_WARN(message, ...) LOG_OUTPUT(LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#else
	#define LOG_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
	#define LOG_INFO(message, ...) LOG_OUTPUT(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
	#define LOG_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
	#define LOG_DEBUG(message, ...) LOG_OUTPUT(LOG_LEVEL_DEBUG , message, ##__VA_ARGS__)
#else
	#define LOG_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
	#define LOG_TRACE(message, ...) LOG_OUTPUT(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#else
	#define LOG_TRACE(message, ...)
#endif

#if defined(RUNTIME_ASSERTS_DISABLED)
	#define RUNTIME_ASSERT(expression)
	#define RUNTIME_ASSERT_MSG(expression)
#else
	void MACRO_RUNTIME_ASSERT(bool expression, const char* function, const char* file, int line);
	void MACRO_RUNTIME_ASSERT_MSG(bool expression, const char* function, const char* file, int line, const char* fmt, ...);
	#define RUNTIME_ASSERT(expression) MACRO_RUNTIME_ASSERT((expression), __func__, __FILE__, __LINE__)
	#define RUNTIME_ASSERT_MSG(expression, message, ...) MACRO_RUNTIME_ASSERT_MSG((expression), __func__, __FILE__, __LINE__, message, ##__VA_ARGS__)
#endif

#define STATIC_ASSERT static_assert
#define INVARIENT_TRIVAL_TYPE(...) STATIC_ASSERT(std::is_trivially_copyable_v<__VA_ARGS__>)