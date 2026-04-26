#pragma once

#include <initializer_list>

#include "../Basic/basic.hpp"
#include "../Memory/memory.hpp"
#include "../Assert/assert.hpp"
#include "../Logger/logger.hpp"
#include "../Hashing/hashing.hpp"

template <typename T>
struct Vector {
	u64 count = 0;
	u64 capacity = DEFAULT_CAPACITY;
	T* data = nullptr;
	Allocator allocator = Allocator::invalid();
    
   	Vector(Allocator allocator = Allocator::invalid(), u64 capacity = DEFAULT_CAPACITY, u64 count = 0) {
		this->count = count;
        this->capacity = capacity;
        this->data = nullptr;
        this->allocator = allocator;
	}
    
    Vector(std::initializer_list<T> list, Allocator allocator = Allocator::invalid()) {
        this->count = list.size();
        this->capacity = (this->count * 2) ? (this->count * 2) : DEFAULT_CAPACITY;
        this->allocator = allocator;

        if (list.size() == 0) return;

        if (this->allocator == Allocator::invalid()) {
            this->allocator = Allocator::general();
        }

		this->count = list.size();
		this->capacity = this->count * 2;
		this->data = (T*)this->allocator.malloc(this->capacity * sizeof(T), alignof(T));
		Memory::copy(this->data, this->capacity * sizeof(T), list.begin(), list.size() * sizeof(T));
	}

    static void copy(Vector<T>* destination, Vector<T>* source) {
        RUNTIME_ASSERT(destination->data == nullptr);
        destination->capacity = source->capacity;
        destination->count = source->count;
        if (source->data == nullptr) return;

        size_t allocation_size = destination->capacity * sizeof(T);
        destination->data = (T*)destination->allocator.malloc(allocation_size, alignof(T));
        Memory::copy(destination->data, allocation_size, source->data, allocation_size);
    }

    void reserve(u64 count) {
        const size_t total_allocation_size = sizeof(T) * this->capacity;
        const size_t old_allocation_size = sizeof(T) * this->count;
        const size_t new_allocation_size = sizeof(T) * count;
        if (new_allocation_size > total_allocation_size) {
            this->data = (T*)this->allocator.malloc(this->data, old_allocation_size, new_allocation_size, alignof(T));
            this->capacity = count;
        }
    }

	void resize(u64 count) {
        const size_t total_allocation_size = sizeof(T) * this->capacity;
        const size_t old_allocation_size = sizeof(T) * this->count;
        const size_t new_allocation_size = sizeof(T) * count;
        if ((this->data == nullptr) || new_allocation_size > total_allocation_size) {
            this->data = (T*)this->allocator.realloc(this->data, old_allocation_size, new_allocation_size, alignof(T));
        }

        this->count = count;
        this->capacity = count;
    }

	void append(T value) {
        if (this->allocator == Allocator::invalid()) {
            this->allocator = Allocator::general();
        }

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

	void stable_remove(int index) {
        RUNTIME_ASSERT_MSG(this->count > 0, "You may not remove if the vector is empty!\n");
        RUNTIME_ASSERT_MSG((index >= 0) && (this->count > index), "index is outside of bounds!\n");

        for (int i = index; i < this->count - 1; i++) {
            this->data[i] = this->data[i + 1];
        }

        this->count -= 1;
    }

	void unstable_swapback_remove(int index) {
		RUNTIME_ASSERT_MSG(this->count > 0, "You may not remove if the vector is empty!\n");
        RUNTIME_ASSERT_MSG((index >= 0) && (this->count > index), "index is outside of bounds!\n");

		this->data[index] = this->data[--this->count];
	}

    int find(T element) {
        for (int i = 0; i < this->count; i++) {
            if (this->data[i] == element) {
                return i;
            }
        }

        return -1;
    }

	T* begin() { return this->data; }
	T* end() { return this->data + this->count; }
	const T* begin() const { return this->data; }
	const T* end() const { return this->data + this->count; }
	
 	T operator[](int i) const {
		RUNTIME_ASSERT_MSG((i >= 0) && (this->count > i), "index: %d is outside of bounds! [0, %lld]\n", i, this->count);

		return this->data[i];
	}

	T& operator[](int i) {
		RUNTIME_ASSERT_MSG((i >= 0) && (this->count > i), "index: %d is outside of bounds! [0, %lld]\n", i, this->count);

		return this->data[i];
	}

	bool operator==(const Vector<T> &right) const {
		if (this->count != right.count) {
			return false;
		}

		for (int i = 0; i < this->count; i++) {
			if (this->data[i] != right.data[i]) {
				return false;
			}
		}

		return true;
	}

	void clear() {
		this->count = 0;
	}
}; INVARIENT_TRIVAL_TYPE(Vector<int>);