#include "../DataStructure/sfe_view.hpp"
#include "../Memory/sfe_memory.hpp"
#include "sfe_string.hpp"

namespace String {
    char* Allocate(const char* s1, u64 length) {
        char* ret = (char*)Memory::Malloc(length + 1);
        Memory::Copy(ret, length, s1, length);

        return ret;
    }

    char* Sprintf(u64 &out_buffer_length, const char* fmt ...) {
        va_list args, copy_args;
        va_start(args, fmt);
            va_copy(copy_args, args);
            u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, copy_args) + 1; // +1 for null terminator
            va_end(copy_args);

            char* buffer = (char*)Memory::Malloc(allocation_ret);

            va_copy(copy_args, args);
            vsnprintf(buffer, allocation_ret, fmt, copy_args);
            va_end(copy_args);
        va_end(args);

        out_buffer_length = allocation_ret - 1;

        return buffer;
    }
    
    char* Sprintf(u64 &out_buffer_length, const char* fmt, va_list args) {
        va_list args_copy;
        va_copy(args_copy, args);
        u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for null terminator
        va_end(args_copy);

        char* buffer = (char*)Memory::Malloc(allocation_ret);

        va_copy(args_copy, args);
        vsnprintf(buffer, allocation_ret, fmt, args_copy);
        va_end(args_copy);

        out_buffer_length = allocation_ret - 1;

        return buffer;
    }

    void Sprintf(char* buffer, size_t buffer_capacity, u64 &out_buffer_length, const char* fmt, ...) {
        va_list args, copy_args;
        va_start(args, fmt);
            va_copy(copy_args, args);
                u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, copy_args) + 1; // +1 for null terminator
                RUNTIME_ASSERT(allocation_ret < buffer_capacity);
            va_end(copy_args);

            va_copy(copy_args, args);
                vsnprintf(buffer, allocation_ret, fmt, copy_args);
            va_end(copy_args);
        va_end(args);

        out_buffer_length = allocation_ret - 1;
    }

    void Sprintf(char* buffer, size_t buffer_capacity, u64 &out_buffer_length, const char* fmt, va_list args) {
        va_list args_copy;
        va_copy(args_copy, args);
        u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for null terminator
        RUNTIME_ASSERT(allocation_ret < buffer_capacity);
        va_end(args_copy);

        va_copy(args_copy, args);
        vsnprintf((char*)buffer, allocation_ret, fmt, args_copy);
        va_end(args_copy);

        out_buffer_length = allocation_ret - 1;
    }

    u64 Length(const char* c_string) {
        u64 ret = 0;
        while (*c_string++ != '\0') {
            ret += 1;
        } 

        return ret;
    }

    bool Equal(const char* s1, u64 s1_length, const char* s2, u64 s2_length) {
        return Memory::Equal(s1, s1_length, s2, s2_length);
    }

    bool Equal(const char* s1, const char* s2) {
        return Memory::Equal(s1, String::Length(s1), s2, String::Length(s2));
    }

    bool Equal(DS::View<char> s1, DS::View<char> s2) {
        return Memory::Equal(s1.data, s1.length, s2.data, s2.length);
    }

    s64 IndexOf(const char* str, u64 str_length, const char* substring, u64 substring_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(substring);

        if (str_length == 0 && substring_length == 0) {
            return 0;
        } else if (substring_length == 0) {
            return -1;
        } else if (str_length == 0) {
            return -1;
        }

        if (substring_length > str_length) {
            return -1;
        }
        
        s64 ret_index = -1;
        for (u64 i = 0; i <= str_length - substring_length; i++) {
            if (ret_index != -1) {
                break;
            }
            
            if (str[i] != substring[0]) {
                continue;
            }

            DS::View<char> current_view = DS::View(str + i, substring_length);
            if (String::Equal(substring, substring_length, current_view.data, current_view.length)) {
                ret_index = (s64)i;
                break;
            }
        }

        return ret_index; // returns -1 if not found
    }

    bool Contains(const char* str, u64 str_length, const char* contains, u64 contains_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(contains);

        return String::IndexOf(str, str_length, contains, contains_length) != -1;  
    }

    s64 LastIndexOf(const char* str, u64 str_length, const char* substring, u64 substring_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(substring);

        if (str_length == 0 && substring_length == 0) {
            return 0;
        } else if (substring_length == 0) {
            return -1;
        } else if (str_length == 0) {
            return -1;
        }

        if (substring_length > str_length) {
            return -1;
        }
        
        s64 ret_index = -1;
        for (u64 i = 0; i <= (str_length - substring_length); i++) {
            if (str[i] != substring[0]) {
                continue;
            }

            DS::View<char> current_view = DS::View(str + i, substring_length);
            if (String::Equal(current_view.data, current_view.length, substring, substring_length)) {
                ret_index = (s64)i;
            }
        }

        return ret_index; // returns -1 if not found
    }

    bool StartsWith(const char* str, u64 str_length, const char* starts_with, u64 starts_with_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(starts_with);

        if (str_length == 0 && starts_with_length == 0) {
            return true;
        } else if (str[0] != starts_with[0] || str_length < starts_with_length) {
            return false;
        }
        
        if (String::Equal(str, starts_with_length, starts_with, starts_with_length)) {
            return true;
        }

        return false;
    }

    bool EndsWith(const char* str, u64 str_length, const char* ends_with, u64 ends_with_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(ends_with);

        if ((str_length == 0) || (ends_with_length == 0)) {
            return true;
        } else if (str_length < ends_with_length) {
            return false;
        }

        u64 start_index = MAX(str_length - 1, 0) - MAX(ends_with_length - 1, 0);
        if (str[start_index] != ends_with[0]) {
            return false;
        }

        if (String::Equal(str + start_index, ends_with_length, ends_with, ends_with_length)) {
            return true;
        }

        return false;
    }

    void Copy(char* s1, size_t s1_capacity, const char* s2, u64 s2_length) {
        Memory::Zero(s1, s1_capacity);
        Memory::Copy(s1, s1_capacity, s2, s2_length);
    }

    void Insert(char* str, u64 &str_length_out, size_t str_capacity, const char* to_insert, u64 to_insert_length, u64 index) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(to_insert);

        u64 new_length = str_length_out + to_insert_length;
        RUNTIME_ASSERT_MSG(new_length < str_capacity, "ckg_str_insert: str_capacity is %lld but new valid cstring capacity is %d + %d + 1(null_term)= %d\n", str_capacity, str_length_out, to_insert_length, new_length + 1);
        
        u8* move_source_ptr = (u8*)(str + index);
        u8* move_dest_ptr = (u8*)(move_source_ptr + to_insert_length);

        Memory::Copy(move_dest_ptr, str_capacity - (index + to_insert_length), move_source_ptr, str_length_out - index);
        u8* copy_dest_ptr = (u8*)(str + index);
        Memory::Copy(copy_dest_ptr, str_capacity, to_insert, to_insert_length);

        str_length_out += to_insert_length;
    }

    void Insert(char* str, u64& str_length_out, size_t str_capacity, char to_insert, u64 index) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(to_insert);

        u64 to_insert_length = 1;
        bool expression = (str_length_out + to_insert_length) < str_capacity;
        RUNTIME_ASSERT_MSG(expression, "ckg_str_insert_char: str overflow new_capacity_required: %d >= current_capacity: %lld\n", str_length_out + to_insert_length, str_capacity);

        char* source_ptr = str + index;
        Memory::Copy(source_ptr + 1, str_capacity - (index + 1), source_ptr, str_length_out - index);
        str[index] = to_insert;

        str_length_out += 1;
    }

    void Append(char* str, u64 &out_str_length, size_t str_capacity, const char* to_append, u64 to_append_length) {
        String::Insert(str, out_str_length, str_capacity, to_append, to_append_length, out_str_length);
    }

    void Append(char* str, u64 &out_str_length, size_t str_capacity, char to_append) {
        String::Insert(str, out_str_length, str_capacity, to_append, out_str_length);
    }
}