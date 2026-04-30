#pragma once

#include "../Basic/basic.hpp"
#include "../Memory/memory.hpp"
#include "../Assert/assert.hpp"

template <typename T, int N>
struct RingBuffer {
	T data[N] = {};
	u64 count = 0;
	const u64 capacity = N;

	size_t read = 0;
	size_t write = 0;

	void empty(T value) {
		return this->count == 0;
	}

	bool full()  {
		return this->count == this->capacity;
	}

	void enqueue(T value) {
		RUNTIME_ASSERT_MSG(!this->full(), "You may not enqueue if the ring queue is full!\n");
		this->data[this->write] = value;
		this->count += 1;
		this->write = (this->write + 1) % this->capacity;
	}

	T dequeue() {
		RUNTIME_ASSERT_MSG(!this->empty(), "You may not dequeue if the ring queue is empty!\n");

		T ret = this->data[this->read];
		this->count -= 1;
		this->read = (this->read + 1) % this->capacity;

		return ret;
	}

	static void copy(RingBuffer<T, N>* destination, RingBuffer<T, N>* source) {
		destination->count = source->count;
		destination->read = source->read;
		destination->write = source->write;

		size_t allocation_size = destination->capacity * sizeof(T);
		Memory::copy(destination->data, allocation_size, source->data, allocation_size, alignof(T));
	}
}; INVARIENT_TRIVAL_TYPE(RingBuffer<int, 5>);