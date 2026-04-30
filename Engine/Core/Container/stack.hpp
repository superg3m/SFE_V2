#pragma once

#include "../Basic/basic.hpp"
#include "../Memory/memory.hpp"
#include "../Assert/assert.hpp"

template <typename T>
struct Stack {
	T* data;
	u64 count;
	u64 capacity;
	Allocator allocator;

	Stack(Allocator allocator) {
		this->allocator = allocator;
		this->capacity = DEFAULT_CAPACITY;
	}

	bool empty() {
		return this->count == 0;
	}

	void push(T value) {
		if (!this->data) { 
            this->data = (T*)this->allocator.malloc(this->capacity * sizeof(T), alignof(T));
        }

        if (this->count == this->capacity) {
            size_t old_allocation_size = (this->capacity * sizeof(T));
            this->capacity *= 2;
            size_t new_allocation_size = (this->capacity * sizeof(T));
            this->data = (T*)this->allocator.realloc(this->data, old_allocation_size, new_allocation_size, alignof(T));
        }

		this->data[this->count++] = value;
	}

	T stack_pop() {
		RUNTIME_ASSERT_MSG(!this->empty(), "You may not pop if the stack is empty!\n");
		return this->data[--this->count];
	}

	T peek() {
		RUNTIME_ASSERT_MSG(!this->empty(), "You may not peek if the stack is empty!\n");
		return this->data[this->count - 1];
	}

	static void copy(Stack* destination, Stack* source) {
		destination->capacity = source->capacity;
		destination->count = source->count;

		size_t allocation_size = destination->capacity * sizeof(T);
		destination->data = (T*)destination->allocator.malloc(allocation_size, alignof(T));
		Memory::copy(destination->data, allocation_size, source->data, allocation_size);
	}
}; INVARIENT_TRIVAL_TYPE(Stack<int>);