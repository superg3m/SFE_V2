#pragma once

#include <cstdarg>
#include <cstdio>

#include "../Basic/basic.hpp"
#include "../Memory/memory.hpp"

struct String {
    const char* data = nullptr;
    u64 length = 0;

    static String create(const char* data, u64 length);
    static String intern(void* string_intern_map, const char* data, u64 length);
    bool operator==(const String& other) const;
    bool operator!=(const String& other) const;

    static char* allocate(Allocator allocator, const char* s1, u64 length);
    static char* sprintf(Allocator allocator, u64 &out_buffer_length, const char* fmt ...);
    static char* sprintf(Allocator allocator, u64 &out_buffer_length, const char* fmt, va_list args);
    static void sprintf(char* buffer, size_t buffer_capacity, u64 &out_buffer_length, const char* fmt, ...);
    static void sprintf(char* buffer, size_t buffer_capacity, u64 &out_buffer_length, const char* fmt, va_list args);

    static u64 cstr_length(const char* c_string);
    static bool equal(const char* s1, u64 s1_length, const char* s2, u64 s2_length);
    static bool equal(const char* s1, const char* s2);
    static bool contains(const char* s1, u64 s1_length, const char* contains, u64 contains_length);

    static s64 index_of(const char* str, u64 str_length, const char* substring, u64 substring_length);
    static s64 last_index_of(const char* str, u64 str_length, const char* substring, u64 substring_length);
    static bool starts_with(const char* str, u64 str_length, const char* starts_with, u64 starts_with_length);
    static bool ends_with(const char* str, u64 str_length, const char* ends_with, u64 ends_with_length);

    static void copy(char* s1, size_t s1_capacity, const char* s2, u64 s2_length);
    static void append(char* str, u64 &str_length_out, size_t str_capacity, const char* to_append, u64 to_append_length);
    static void append(char* str, u64 &str_length_out, size_t str_capacity, char to_append);
    static void insert(char* str, u64 &str_length_out, size_t str_capacity, const char* to_insert, u64 to_insert_length, u64 index);
    static void insert(char* str, u64 &str_length_out, size_t str_capacity, char to_insert, u64 index);

    static String between_delimiters(const char* str, u64 str_length, const char* start_delimitor, u64 start_delimitor_length, const char* end_delimitor, u64 end_delimitor_length);
};

#define STR(s) (String{s, sizeof(s) - 1})
#define STR_INTERN(s) (String::intern(string_intern_map, s, sizeof(s) - 1))