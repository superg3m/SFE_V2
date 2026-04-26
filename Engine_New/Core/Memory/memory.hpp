#pragma once

#include "../Basic/basic.hpp"

namespace Memory {
    void zero(void* data, size_t data_size_in_bytes);
    void copy(void* destination, size_t destination_size, const void* source, size_t source_size);
    bool equal(const void* buffer_one, size_t b1_size, const void* buffer_two, size_t b2_size);

    template <typename T>
    void swap(T &a, T &b) {
        T temp = a;
        a = b;
        b = temp;
    }
}

typedef void* (AllocateFunction)(void* ctx, size_t allocation_size, size_t alignment);
typedef void* (ReallocFunction)(void* ctx, void* data, size_t old_allocation_size, size_t new_allocation_size, size_t alignment);
typedef void  (FreeFunction)(void* ctx, void* data);

struct Allocator {
	void* ctx = nullptr;
	AllocateFunction* malloc_cb = nullptr;
	FreeFunction* free_cb = nullptr;

	void* malloc(size_t allocation_size, size_t alignment);
	void* realloc(void* data, size_t old_allocation_size, size_t new_allocation_size, size_t alignment);
	void free(void* data);

    static Allocator invalid();
	static Allocator general();

	bool operator==(Allocator other) const;
	bool operator!=(Allocator other) const;
};

// TODO(Jovanni): add linked list memory pages flag
enum ArenaFlag {
	ARENA_FLAG_FIXED        = 0x1,
	ARENA_FLAG_CIRCULAR     = 0x2
};

struct Arena {
	int flags = 0;
	size_t used = 0;
	size_t capacity = 0;
	u8* base_address = nullptr;

	static Arena create(void* memory, size_t allocation_size, int flags);
	static Arena fixed(void* memory, size_t allocation_size);
	static Arena circular(void* memory, size_t allocation_size);
	Allocator to_allocator();

	void* push(size_t allocation_size, size_t alignment); // malloc
	void* realloc(void* data, size_t old_allocation_size, size_t new_allocation_size, size_t alignment); // realloc
	void pop_to(size_t new_used);

	template<typename T>
	void* push_type() {
		return this->push(sizeof(T), alignof(T));
	} 
};

struct Temp {
	size_t used_save_point = 0;
	Arena* arena = nullptr;

	static Temp begin(Arena* arena) {
		Temp ret = {};
		ret.used_save_point = arena->used;
		ret.arena = arena;
		
		return ret;
	}

	void end() {
		this->arena->pop_to(this->used_save_point);
		this->arena = nullptr;
	}
};