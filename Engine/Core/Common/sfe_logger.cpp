
#include <stdarg.h>
#include <stdio.h>

#include "sfe_common.hpp"
#include "sfe_types.hpp"
#include "sfe_assert.hpp"

//Regular text
#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_PURPLE "\033[0;35m"

//Regular basferound
#define RED_BAIONROUND "\033[41m"
#define COLOR_RESET "\033[0m"

void log_output(LogLevel log_level, const char* message, ...) {
    #define OUT_MESSAGE_BUFFER_LENGTH KB(4)
    char out_message[OUT_MESSAGE_BUFFER_LENGTH] = {0};

    va_list args;
    va_start(args, message);
        va_list args_copy;
        va_copy(args_copy, args);
            size_t out_message_size = (size_t)vsnprintf(nullptr, 0, message, args_copy) + 1; // +1 for null terminator
            RUNTIME_ASSERT(out_message_size < OUT_MESSAGE_BUFFER_LENGTH);
        va_end(args_copy);

        va_copy(args_copy, args);
            vsnprintf(out_message, out_message_size, message, args_copy);
        va_end(args_copy); 
    u64 out_message_length = out_message_size - 1;
    va_end(args);

    const char* log_level_format[LOG_LEVEL_COUNT] = {
        RED_BAIONROUND,
        COLOR_RED,
        COLOR_YELLOW,
        COLOR_GREEN,
        COLOR_BLUE,
        COLOR_RESET
    };

    bool found = out_message[out_message_length - 1] == '\n';
    printf("%s%.*s%s", log_level_format[log_level], (int)(out_message_length - found), out_message, COLOR_RESET);
    if (found) {
        printf("\n");
    }
}