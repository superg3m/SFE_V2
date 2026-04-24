#pragma once

#include "basic.hpp"

namespace Hashing {
	u64 zero_hash(const void* source, size_t source_size);
	u64 siphash(const void* source, size_t source_size);
	u64 cstring_hash(const void* str, size_t str_length);
	bool cstring_equality(const void* c1, size_t c1_size, const void* c2, size_t c2_size);
	u64 string_view_hash(const void* view, size_t str_length);
	bool string_view_equality(const void* c1, size_t c1_size, const void* c2, size_t c2_size);
}
