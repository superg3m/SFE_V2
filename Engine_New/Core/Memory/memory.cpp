#include <stdlib.h>
#include "memory.hpp"
#include "../Assert/assert.hpp"

INTERNAL_LINKAGE void* general_malloc(void* ctx, size_t allocation_size, size_t alignment) {
	UNUSED(ctx);
	UNUSED(alignment);
	return malloc(allocation_size);
}

INTERNAL_LINKAGE void general_free(void* ctx, void* data) {
	UNUSED(ctx);
	return free(data);
}

bool Allocator::operator==(Allocator other) const {
	return Memory::equal(this, sizeof(Allocator), &other, sizeof(Allocator));
}

bool Allocator::operator!=(Allocator other) const {
	return !(*this == other);
}

void* Allocator::malloc(size_t allocation_size, size_t alignment) {
	void* ret = malloc_cb(this->ctx, allocation_size, alignment);
	Memory::zero(ret, allocation_size);
	return ret;
}

void* Allocator::realloc(void* data, size_t old_allocation_size, size_t new_allocation_size, size_t alignment) {
	void* ret = this->malloc(new_allocation_size, alignment);
	Memory::copy(ret, new_allocation_size, data, old_allocation_size);
	this->free(data);
	return ret;
}

void Allocator::free(void* data) {
	if (free_cb) {
		free_cb(this->ctx, data);
	}
}

Allocator Allocator::invalid() {
	Allocator ret = {};
	return ret;
}

Allocator Allocator::general() {
	Allocator ret = {};
	ret.ctx = nullptr;
	ret.malloc_cb = general_malloc;
	ret.free_cb = general_free;

	LOG_WARN("You are using a general allocator\n");

	return ret;
}

Arena Arena::create(void* memory, size_t allocation_size, int flags) {
	Arena ret = {};

	RUNTIME_ASSERT_MSG(memory, "Memory can't be a null pointer!\n");
	RUNTIME_ASSERT_MSG(allocation_size != 0, "Can't have a zero allocation size!\n");
	int mask = ARENA_FLAG_FIXED | ARENA_FLAG_CIRCULAR;
	RUNTIME_ASSERT_MSG((flags & mask) != mask, "Can't have both a fixed and circular arena!\n");

	ret.flags = flags;
	ret.used = 0;
	ret.capacity = allocation_size;
	ret.base_address = (u8*)memory;

	return ret;
}

Arena Arena::fixed(void* memory, size_t allocation_size) {
	return Arena::create(memory, allocation_size, ARENA_FLAG_FIXED);
}
Arena Arena::circular(void* memory, size_t allocation_size) {
	return Arena::create(memory, allocation_size, ARENA_FLAG_CIRCULAR);
}

INTERNAL_LINKAGE void* arena_malloc(void* ctx, size_t allocation_size, size_t alignment) {
	Arena* arena = (Arena*)ctx;
	return arena->push(allocation_size, alignment);
}

INTERNAL_LINKAGE void* arena_realloc(void* ctx, void* data, size_t old_allocation_size, size_t new_allocation_size, size_t alignment) {
	Arena* arena = (Arena*)ctx;
	return arena->realloc(data, old_allocation_size, new_allocation_size, alignment);
}

Allocator Arena::to_allocator() {
	Allocator ret = {};
	ret.ctx = this;
	ret.malloc_cb = arena_malloc;
	ret.free_cb = nullptr;

	return ret;
}

void* Arena::push(size_t allocation_size, size_t alignment) {
    RUNTIME_ASSERT_MSG(this->base_address, "Allocator is invalid!\n");
	RUNTIME_ASSERT_MSG(allocation_size != 0, "Element size can't be zero!\n");

	if (this->flags & ARENA_FLAG_FIXED) {
		RUNTIME_ASSERT_MSG(this->used + allocation_size <= this->capacity, "Ran out of arena memory!\n");
	} else if (this->flags & ARENA_FLAG_CIRCULAR) {
		if ((this->used + allocation_size > this->capacity)) {
			this->used = 0;
			RUNTIME_ASSERT_MSG(this->used + allocation_size <= this->capacity, "Element size exceeds circular arena allocation capacity!\n");
		}
	}

	u8* ret = this->base_address + this->used;
	size_t previous_used = this->used;
	this->used += allocation_size;
	if ((this->used & (alignment - 1)) != 0) {
		this->used += (alignment - (this->used & (alignment - 1)));
	}

    return ret;
}

void* Arena::realloc(void* data, size_t old_allocation_size, size_t new_allocation_size, size_t alignment) {
	/*
	Arena* arena = (Arena*)arena_ctx;
	RUNTIME_ASSERT(arena);
	RUNTIME_ASSERT(old_allocation_size != 0);
	RUNTIME_ASSERT(new_allocation_size != 0);

	if (arena_is_data_poppable(arena, data)) {
		arena_pop(arena, old_allocation_size);
		return arena_push(arena, new_allocation_size);
	}
	// this data is aligned so if you just do data - old_allocation_size its not correct
	this->base_address + (this->used - old_allocation_size)
	if ()
	*/

	void* ret = this->push(new_allocation_size, alignment);
	Memory::copy(ret, new_allocation_size, data, old_allocation_size);
    return ret;
}

void Arena::pop_to(size_t new_used) {
	this->used = new_used;
}

namespace Memory {
    void zero(void* data, size_t data_size_in_bytes) {
        RUNTIME_ASSERT(data);
        
        for (size_t i = 0; i < data_size_in_bytes; i++) {
            ((u8*)data)[i] = 0;
        }
    }

    void copy(void* destination, size_t destination_size, const void* source, size_t source_size) {
        RUNTIME_ASSERT(source);
        RUNTIME_ASSERT(destination);
        RUNTIME_ASSERT(source_size <= destination_size);
        if (source_size == 0) {
            return;
        }

        u8* src = (u8*)source;
        u8* dst = (u8*)destination;

        bool overlap = dst < src || dst >= src + source_size;
        if (overlap) {
            for (size_t i = 0; i < source_size; i++) {
                dst[i] = src[i];
            }
        } else {
            for (size_t i = source_size; i-- > 0;) {
                dst[i] = src[i];
            }
        }
    }

    bool equal(const void* buffer_one, size_t b1_size, const void* buffer_two, size_t b2_size) {
        RUNTIME_ASSERT(buffer_one);
        RUNTIME_ASSERT(buffer_two);

        if (b1_size != b2_size) {
            return false;
        }

        u8* buffer_one_data = (u8*)buffer_one;
        u8* buffer_two_data = (u8*)buffer_two;
        for (size_t i = 0; i < b1_size; i++) {
            if (buffer_one_data[i] != buffer_two_data[i]) {
                return false;
            }
        }

        return true;
    }
}