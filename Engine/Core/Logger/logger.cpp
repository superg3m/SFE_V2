
#include <stdarg.h>
#include <stdio.h>

#include "logger.hpp"
#include "../Assert/assert.hpp"
#include "../Basic/basic.hpp"
#include "../String/string.hpp"

//Regular text
#define COLOR_RED "\033[0;31m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_PURPLE "\033[0;35m"

//Regular background
#define RED_BACKGROUND "\033[41m"
#define COLOR_RESET "\033[0m"

const char* log_level_format[LOG_LEVEL_COUNT] = {
    RED_BACKGROUND,
    COLOR_RED,
    COLOR_YELLOW,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_RESET
};

#define LOGGER_START_DELIM "${"
#define LOGGER_END_DELIM "}"

INTERNAL_LINKAGE bool message_has_special_delmitor(const char* message, u64 message_length) {
    bool start_delimitor_index = String::contains(message, message_length, STRING_LIT_ARG(LOGGER_START_DELIM));
    bool end_delimitor_index = String::contains(message, message_length, STRING_LIT_ARG(LOGGER_END_DELIM));

    return start_delimitor_index && end_delimitor_index;
}

INTERNAL_LINKAGE void special_print_helper(const char* message, u64 message_length, LogLevel log_level) {
    String middle_to_color = String::between_delimiters(message, message_length, STRING_LIT_ARG(LOGGER_START_DELIM), STRING_LIT_ARG(LOGGER_END_DELIM));
    if (!middle_to_color.data) {
        bool found = message[message_length - 1] == '\n';
        printf("%.*s", (int)(message_length - found), message);

        return;
    }

    s64 start_delimitor_index = String::index_of(message, message_length, STRING_LIT_ARG(LOGGER_START_DELIM));
    s64 end_delimitor_index = String::index_of(message, message_length, STRING_LIT_ARG(LOGGER_END_DELIM));

    String left_side_view = String(message, (u64)start_delimitor_index);
    String right_side_view = String(message + ((u64)end_delimitor_index + (sizeof(LOGGER_END_DELIM) - 1)), (u64)message_length);

    printf("%.*s%s%.*s%s", (int)left_side_view.length, left_side_view.data, log_level_format[log_level], (int)middle_to_color.length, middle_to_color.data, COLOR_RESET);

    special_print_helper(right_side_view.data, right_side_view.length, log_level);
}

void LOG_OUTPUT(LogLevel log_level, const char* message, ...) {
    #define OUT_MESSAGE_BUFFER_LENGTH KB(4)
    char out_message[OUT_MESSAGE_BUFFER_LENGTH] = {0};

    va_list args_list;
    va_start(args_list, message);
        va_list args_copy;
        va_copy(args_copy, args_list);
            u64 out_message_length = 0;
            String::sprintf(out_message, OUT_MESSAGE_BUFFER_LENGTH, out_message_length, message, args_copy);
            RUNTIME_ASSERT(out_message_length + 1 < OUT_MESSAGE_BUFFER_LENGTH);
        va_end(args_copy);
    va_end(args_list);

    bool found = out_message[out_message_length - 1] == '\n';
    if (message_has_special_delmitor(out_message, out_message_length)) {
        special_print_helper(out_message, out_message_length, log_level);
    } else {
        printf("%s%.*s%s", log_level_format[log_level], (int)(out_message_length - found), out_message, COLOR_RESET);
    }
    
    if (found) {
        printf("\n");
    }
}