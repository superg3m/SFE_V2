#include "core.hpp"

static const char* ERROR_STRINGS[(int)Error::COUNT] = {
    STRINGIFY(ERROR_SUCCESS),
    STRINGIFY(ERROR_RESOURCE_NOT_FOUND),
    STRINGIFY(ERROR_RESOURCE_TOO_BIG),
    STRINGIFY(ERROR_NULL_PARAMETER),
    STRINGIFY(ERROR_INVALID_PARAMETER),
};

const char* error_get_string(Error error_code) {
    RUNTIME_ASSERT(((int)error_code >= 0) && ((int)error_code < (int)Error::COUNT));

    return ERROR_STRINGS[(int)error_code];
}

void LOG_OUTPUT(LogLevel log_level, const char* message, ...) {
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
        RED_BACKGROUND,
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

// -- 

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

// --

u8* platform_read_entire_file(const char* file_name, size_t& out_file_size, Error& error) {
    FILE* file_handle = fopen(file_name, "r");
    if (file_handle == nullptr) {
        LOG_ERROR("Invalid file_handle, the file_name/path is likely wrong: platform_read_entire_file(%s)\n", file_name);
        error = Error::RESOURCE_NOT_FOUND;

        return nullptr;
    }

    if (fseek(file_handle, 0L, SEEK_END) != 0) {
        LOG_ERROR("fseek failed: platform_read_entire_file(%s)\n", file_name);
        error = Error::RESOURCE_NOT_FOUND;
        fclose(file_handle);
        return nullptr;
    }

    out_file_size = ftell(file_handle);
    if (out_file_size == -1L) {
        LOG_ERROR("ftell failed: platform_read_entire_file(%s)\n", file_name);
        error = Error::RESOURCE_NOT_FOUND;
        fclose(file_handle);
        return nullptr;
    }

    rewind(file_handle);
    if (ferror(file_handle)) {
        LOG_ERROR("rewind() failed: platform_read_entire_file(%s)\n", file_name);
        error = Error::RESOURCE_NOT_FOUND;
        fclose(file_handle);
        
        return nullptr;
    }

    u8* file_data = (u8*)malloc((size_t)out_file_size + 1); // +1 for null terminator
    if (fread(file_data, out_file_size, 1, file_handle) != 1) {
        LOG_ERROR("fread() failed: platform_read_entire_file(%s)\n", file_name);
        error = Error::RESOURCE_NOT_FOUND;
        free(file_data);
        fclose(file_handle);

        return nullptr;
    }

    fclose(file_handle);

    out_file_size = out_file_size + 1;

    return file_data;
}

bool platform_file_exists(const char* path) {
    FILE *fptr = fopen(path, "r");

    if (fptr == nullptr) {
        return false;
    }

    fclose(fptr);

    return true;
}