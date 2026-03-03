#include <stdio.h>
#include <stdarg.h>

#include "sfe_types.hpp"
#include "sfe_assert.hpp"
#include "sfe_logger.hpp"

void MACRO_RUNTIME_ASSERT(bool expression, const char* function, const char* file, int line) {
    if (!expression) {                             
        //stack_trace_dump(function, file, line);                               
        char msg[] = "Func: %s, File: %s:%d\n";          
        LOG_FATAL(msg, function, file, line);
        CRASH();
    }                                                         
}

void MACRO_RUNTIME_ASSERT_MSG(bool expression, const char* function, const char* file, int line, const char* msg, ...) {  
    if (!(expression)) {            
        #define BUFFER_LENGTH 256
        char message[BUFFER_LENGTH] = {0};

        va_list args;
        va_start(args, msg);
            va_list args_copy;
            va_copy(args_copy, args);
                size_t message_size = (size_t)vsnprintf(nullptr, 0, msg, args_copy) + 1; // +1 for null terminator
                RUNTIME_ASSERT(message_size < BUFFER_LENGTH);
            va_end(args_copy);

            va_copy(args_copy, args);
                vsnprintf(message, message_size, msg, args_copy);
            va_end(args_copy); 
        va_end(args);   

        u64 message_length = message_size - 1;

        // stack_trace_dump(function, file, line);
        LOG_FATAL("%.*s", message_length, message);                                 
        char func_file_msg[] = "Func: %s, File: %s:%d\n";          
        LOG_FATAL(func_file_msg, function, file, line);

        CRASH();                                                
    }                                                                                    
}