#pragma once

#include <cstdarg>
#include <cstdio>

#include "../Common/sfe_common.hpp"
#include "../DataStructure/sfe_view.hpp"

namespace String {
    char* Allocate(const char* s1, u64 length);
    char* Sprintf(u64 &out_buffer_length, const char* fmt ...);
    char* Sprintf(u64 &out_buffer_length, const char* fmt, va_list args);
    void Sprintf(char* buffer, size_t buffer_capacity, u64 &out_buffer_length, const char* fmt, ...);
    void Sprintf(char* buffer, size_t buffer_capacity, u64 &out_buffer_length, const char* fmt, va_list args);

    u64 Length(const char* c_string);
    bool Equal(const char* s1, u64 s1_length, const char* s2, u64 s2_length);
    bool Equal(const char* s1, const char* s2);
    bool Equal(DS::View<char> s1, DS::View<char> s2);
    bool Contains(const char* s1, u64 s1_length, const char* contains, u64 contains_length);

    s64 IndexOf(const char* str, u64 str_length, const char* substring, u64 substring_length);
    s64 LastIndexOf(const char* str, u64 str_length, const char* substring, u64 substring_length);
    bool StartsWith(const char* str, u64 str_length, const char* starts_with, u64 starts_with_length);
    bool EndsWith(const char* str, u64 str_length, const char* ends_with, u64 ends_with_length);

    void Copy(char* s1, size_t s1_capacity, const char* s2, u64 s2_length);
    void Append(char* str, u64 &str_length_out, size_t str_capacity, const char* to_append, u64 to_append_length);
    void Append(char* str, u64 &str_length_out, size_t str_capacity, char to_append);
    void Insert(char* str, u64 &str_length_out, size_t str_capacity, const char* to_insert, u64 to_insert_length, u64 index);
    void Insert(char* str, u64 &str_length_out, size_t str_capacity, char to_insert, u64 index);
}