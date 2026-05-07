#include "string.hpp"
#include "../Memory/memory.hpp"
#include "../Assert/assert.hpp"
#include "../Container/hashmap.hpp"

String String::create(const char* data, u64 length) {
	return {data, length};
}

String String::intern(Arena* string_arena, void* string_intern_map, const char* data, u64 length) {
	Hashmap<String, String>* intern_map = (Hashmap<String, String>*)string_intern_map;
	RUNTIME_ASSERT(string_intern_map);
	String ret = {};
	String key = {data, length};
	if (intern_map->has(key)) {
		String value = intern_map->get(key);
		ret.data = value.data;
		ret.length = value.length;
		return ret;
	}

	String safe_key = {String::allocate(string_arena->to_allocator(), data, length), length};
	String value = safe_key;
	intern_map->put(safe_key, value);

	return value;
}

bool String::operator==(const String& other) const {
	if (this->data == other.data && this->length == other.length) {
		return true;
	}

	return Memory::equal(this->data, this->length, other.data, other.length);
}

bool String::operator!=(const String& other) const {
	return !(*this == other);
}

char* String::allocate(Allocator allocator, const char* s1, u64 length) {
	char* ret = (char*)allocator.malloc(length + 1, alignof(char));
	Memory::copy(ret, length, s1, length);

	return ret;
}

char* String::sprintf(Allocator allocator, u64 &out_buffer_length, const char* fmt ...) {
	va_list args, copy_args;
	va_start(args, fmt);
		va_copy(copy_args, args);
		u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, copy_args) + 1; // +1 for null terminator
		va_end(copy_args);

		char* buffer = (char*)allocator.malloc(allocation_ret, alignof(char));

		va_copy(copy_args, args);
		vsnprintf(buffer, allocation_ret, fmt, copy_args);
		va_end(copy_args);
	va_end(args);

	out_buffer_length = allocation_ret - 1;

	return buffer;
}

char* String::sprintf(Allocator allocator, u64 &out_buffer_length, const char* fmt, va_list args) {
	va_list args_copy;
	va_copy(args_copy, args);
	u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for null terminator
	va_end(args_copy);

	char* buffer = (char*)allocator.malloc(allocation_ret, alignof(char));

	va_copy(args_copy, args);
	vsnprintf(buffer, allocation_ret, fmt, args_copy);
	va_end(args_copy);

	out_buffer_length = allocation_ret - 1;

	return buffer;
}

void String::sprintf(char* buffer, size_t buffer_capacity, u64 &out_buffer_length, const char* fmt, ...) {
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

void String::sprintf(char* buffer, size_t buffer_capacity, u64 &out_buffer_length, const char* fmt, va_list args) {
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

u64 String::cstr_length(const char* c_string) {
	u64 ret = 0;
	while (*c_string++ != '\0') {
		ret += 1;
	} 

	return ret;
}

bool String::equal(const char* s1, u64 s1_length, const char* s2, u64 s2_length) {
	return Memory::equal(s1, s1_length, s2, s2_length);
}

bool String::equal(const char* s1, const char* s2) {
	return Memory::equal(s1, String::cstr_length(s1), s2, String::cstr_length(s2));
}

s64 String::index_of(const char* str, u64 str_length, const char* substring, u64 substring_length) {
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

		String current_view = String(str + i, substring_length);
		if (String::equal(substring, substring_length, current_view.data, current_view.length)) {
			ret_index = (s64)i;
			break;
		}
	}

	return ret_index; // returns -1 if not found
}

bool String::contains(const char* str, u64 str_length, const char* contains, u64 contains_length) {
	RUNTIME_ASSERT(str);
	RUNTIME_ASSERT(contains);

	return String::index_of(str, str_length, contains, contains_length) != -1;  
}

s64 String::last_index_of(const char* str, u64 str_length, const char* substring, u64 substring_length) {
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

		String current_view = String(str + i, substring_length);
		if (String::equal(current_view.data, current_view.length, substring, substring_length)) {
			ret_index = (s64)i;
		}
	}

	return ret_index; // returns -1 if not found
}

bool String::starts_with(const char* str, u64 str_length, const char* starts_with, u64 starts_with_length) {
	RUNTIME_ASSERT(str);
	RUNTIME_ASSERT(starts_with);

	if (str_length == 0 && starts_with_length == 0) {
		return true;
	} else if (str[0] != starts_with[0] || str_length < starts_with_length) {
		return false;
	}
	
	if (String::equal(str, starts_with_length, starts_with, starts_with_length)) {
		return true;
	}

	return false;
}

bool String::ends_with(const char* str, u64 str_length, const char* ends_with, u64 ends_with_length) {
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

	if (String::equal(str + start_index, ends_with_length, ends_with, ends_with_length)) {
		return true;
	}

	return false;
}

void String::copy(char* s1, size_t s1_capacity, const char* s2, u64 s2_length) {
	Memory::zero(s1, s1_capacity);
	Memory::copy(s1, s1_capacity, s2, s2_length);
}

void String::insert(char* str, u64 &str_length_out, size_t str_capacity, const char* to_insert, u64 to_insert_length, u64 index) {
	RUNTIME_ASSERT(str);
	RUNTIME_ASSERT(to_insert);

	u64 new_length = str_length_out + to_insert_length;
	RUNTIME_ASSERT_MSG(new_length < str_capacity, "ckg_str_insert: str_capacity is %lld but new valid cstring capacity is %d + %d + 1(null_term)= %d\n", str_capacity, str_length_out, to_insert_length, new_length + 1);
	
	u8* move_source_ptr = (u8*)(str + index);
	u8* move_dest_ptr = (u8*)(move_source_ptr + to_insert_length);

	Memory::copy(move_dest_ptr, str_capacity - (index + to_insert_length), move_source_ptr, str_length_out - index);
	u8* copy_dest_ptr = (u8*)(str + index);
	Memory::copy(copy_dest_ptr, str_capacity, to_insert, to_insert_length);

	str_length_out += to_insert_length;
}

void String::insert(char* str, u64& str_length_out, size_t str_capacity, char to_insert, u64 index) {
	RUNTIME_ASSERT(str);
	RUNTIME_ASSERT(to_insert);

	u64 to_insert_length = 1;
	bool expression = (str_length_out + to_insert_length) < str_capacity;
	RUNTIME_ASSERT_MSG(expression, "ckg_str_insert_char: str overflow new_capacity_required: %d >= current_capacity: %lld\n", str_length_out + to_insert_length, str_capacity);

	char* source_ptr = str + index;
	Memory::copy(source_ptr + 1, str_capacity - (index + 1), source_ptr, str_length_out - index);
	str[index] = to_insert;

	str_length_out += 1;
}

void String::append(char* str, u64 &out_str_length, size_t str_capacity, const char* to_append, u64 to_append_length) {
	String::insert(str, out_str_length, str_capacity, to_append, to_append_length, out_str_length);
}

void String::append(char* str, u64 &out_str_length, size_t str_capacity, char to_append) {
	String::insert(str, out_str_length, str_capacity, to_append, out_str_length);
}

String String::between_delimiters(const char* str, u64 str_length, const char* start_delimitor, u64 start_delimitor_length, const char* end_delimitor, u64 end_delimitor_length) {
	RUNTIME_ASSERT(str);
	RUNTIME_ASSERT(start_delimitor);
	RUNTIME_ASSERT(end_delimitor);
	RUNTIME_ASSERT(!String::equal(start_delimitor, start_delimitor_length, end_delimitor, end_delimitor_length));

	s64 start_delimitor_index = String::index_of(str, str_length, start_delimitor, start_delimitor_length); 
	s64 end_delimitor_index = String::index_of(str, str_length, end_delimitor, end_delimitor_length);
	if (start_delimitor_index == -1 || end_delimitor_index == -1) {
		return String(nullptr, 0);
	}

	if (start_delimitor_index == -1 || end_delimitor_index == -1) {
		return String(nullptr, 0);
	} else if (start_delimitor_index > end_delimitor_index) {
		return String(nullptr, 0); // The start delimtor is after the end delimitor
	}
	
	u64 start_str_index = (u64)((u64)start_delimitor_index + start_delimitor_length);
	String ret = String(str + start_str_index, (u64)end_delimitor_index - start_str_index);

	return ret;
}