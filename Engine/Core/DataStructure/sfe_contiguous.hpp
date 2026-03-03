#pragma once

#include <initializer_list>
#include <type_traits>

#include "../Memory/sfe_memory.hpp"
#include "../Common/sfe_common.hpp"

namespace DS {
    template <typename T>
    struct Vector {
        Vector() {
            this->m_count = 0;
            this->m_capacity = 0;
            this->m_data = nullptr;
        }

        Vector(std::initializer_list<T> list) {
            this->m_count = list.size();
            this->m_capacity = this->m_count * 2;

            this->m_data = (T*)Memory::Malloc(this->m_capacity * sizeof(T));
            Memory::Copy(this->m_data, this->m_capacity * sizeof(T), list.begin(), list.size() * sizeof(T));
        }

        Vector(u64 capacity, u64 count = 0) {
            this->m_count = count;
            this->m_capacity = capacity;

            this->m_data = (T*)Memory::Malloc(this->m_capacity * sizeof(T));
        }

        // Prevent copy
        Vector(const Vector& other) {
            // LOG_WARN("PERFORMING A VECTOR COPY!\n");

            if (other.m_count == 0) return;

            RUNTIME_ASSERT(this->m_data == nullptr);

            this->m_count = other.m_count;
            this->m_capacity = other.m_capacity;

            this->m_data = (T*)Memory::Malloc(this->m_capacity * sizeof(T));
            Memory::Copy(this->m_data, this->m_capacity * sizeof(T), other.m_data,  this->m_capacity * sizeof(T));
        }

        Vector& operator=(const Vector& other) {
            // LOG_WARN("PERFORMING A VECTOR COPY!\n");

            // RUNTIME_ASSERT(this->m_data == nullptr);
            if (other.m_data == nullptr) return *this;

            this->m_count = other.m_count;
            this->m_capacity = other.m_capacity;
            this->m_data = (T*)Memory::Malloc(this->m_capacity * sizeof(T));
            Memory::Copy(this->m_data, this->m_capacity * sizeof(T), other.m_data,  this->m_capacity * sizeof(T));

            return *this;
        }

        // Prevent move
        Vector(Vector&&) = delete;
        Vector& operator=(Vector&& other) {
            this->destory();

            this->m_count = other.m_count;
            this->m_capacity = other.m_capacity;
            this->m_data = other.m_data;

            // Leave other in a invalid empty state
            other.m_count    = 0;
            other.m_capacity = 0;
            other.m_data     = nullptr;

            return *this;
        }

        ~Vector() {
            this->destory();
        }

        void reserve(u64 count) {
            const size_t total_allocation_size = sizeof(T) * this->m_capacity;

            const size_t old_allocation_size = sizeof(T) * this->m_count;
            const size_t new_allocation_size = sizeof(T) * count;

            if (new_allocation_size > total_allocation_size) {
                this->m_data = (T*)Memory::Realloc(this->m_data, old_allocation_size, new_allocation_size);
                this->m_capacity = count;
            }
        }

        void resize(u64 count) {
            const size_t total_allocation_size = sizeof(T) * this->m_capacity;

            const size_t old_allocation_size = sizeof(T) * this->m_count;
            const size_t new_allocation_size = sizeof(T) * count;
            if (new_allocation_size > total_allocation_size) {
                this->m_data = (T*)Memory::Realloc(this->m_data, old_allocation_size, new_allocation_size);
            }

            this->m_count = count;
            this->m_capacity = count;
        }

        void push(T value) {
            if (this->m_capacity < this->m_count + 1) {
                this->grow();
            }

            this->m_data[this->m_count] = value;
            this->m_count += 1;
        }

        void unstableSwapbackRemove(int i) {
            RUNTIME_ASSERT_MSG(this->m_count > 0, "You may not remove if the vector is empty!\n");
            RUNTIME_ASSERT_MSG((i >= 0) && (this->m_count > i), "index is outside of bounds!\n");

            this->m_data[i] = this->m_data[this->m_count--];
        }

        T* begin() { 
            return m_data; 
        }
        T* end() { 
            return m_data + m_count; 
        }
        const T* begin() const { 
            return m_data; 
        }
        const T* end() const { 
            return m_data + m_count; 
        }

        u64 count() const {
            return this->m_count;
        }

        u64 capacity() const {
            return this->m_capacity;
        }

        T* data() const {
            return this->m_data;
        }
        
        T operator[](int i) const {
            RUNTIME_ASSERT_MSG((i >= 0) && (this->m_count > i), "index: %d is outside of bounds! [0, %lld]\n", i, this->m_count);

            return this->m_data[i];
        }

        T& operator[](int i) {
            RUNTIME_ASSERT_MSG((i >= 0) && (this->m_count > i), "index: %d is outside of bounds! [0, %lld]\n", i, this->m_count);

            return this->m_data[i];
        }

        bool operator==(const Vector<T> &right) const {
            if (this->m_count != right.m_count) {
                return false;
            }

            for (int i = 0; i < this->m_count; i++) {
                if (this->m_data[i] != right.m_data[i]) {
                    return false;
                }
            }

            return true;
        }
        
        void grow() {
            size_t old_allocation_size = (this->m_capacity * sizeof(T));
            this->m_capacity *= 2;
            size_t new_allocation_size = (this->m_capacity * sizeof(T));
            this->m_data = (T*)Memory::Realloc(this->m_data, old_allocation_size, new_allocation_size);
        }

        void clear() {
            this->m_count = 0;
        }

        void shrink_to_fit() {
            this->m_capacity = this->m_count;
            size_t shrunken_allocation_size = (this->m_capacity * sizeof(T));

            this->m_data = (T*)Memory::Realloc(this->m_data, shrunken_allocation_size, shrunken_allocation_size);
        }
    private:
        T* m_data = nullptr;
        u64 m_count = 0;
        u64 m_capacity = 0;

        void destory() {
            if (this->m_data) {
                Memory::Free(this->m_data);
            }

            this->m_count = 0;
            this->m_capacity = 0;
            this->m_data = nullptr;
        }
    };

    template <typename T>
    struct Stack {
        Stack(u64 capacity = 1) {
            this->m_count = 0;
            this->m_capacity = capacity;

            this->m_data = (T*)Memory::Malloc(this->m_capacity * sizeof(T));
        }

        ~Stack() {
            this->free();
        }

        void free() {
            if (this->m_data) {
                for (size_t i = 0; i < m_count; ++i) {
                    this->m_data[i].~T();
                }
            }

            Memory::Free(this->m_data);

            this->m_data = nullptr;
            this->m_count = 0;
            this->m_capacity = 0;
        }

        T peek() const {
            RUNTIME_ASSERT_MSG(!this->empty(), "You may not peek if the stack is empty!\n");

            return this->m_data[this->count() - 1];
        }

        void push(T value) {
            if (this->m_capacity < this->m_count + 1) {
                this->grow();
            }

            this->m_data[this->m_count] = value;
            this->m_count += 1;
        }

        T pop() {
            RUNTIME_ASSERT_MSG(!this->empty(), "You may not pop if the stack is empty!\n");

            this->m_count -= 1;
            return this->m_data[this->m_count];
        }

        bool empty() const {
            return this->m_count == 0;
        }

        u64 count() const {
            return this->m_count;
        }

        u64 capacity() const {
            return this->m_capacity;
        }

        void grow() {
            if (this->m_capacity == 0 || this->m_data == nullptr) {
                // this->m_count = 0;
                // this->m_capacity = capacity;

                // this->m_data = (T*)Memory::Malloc(this->m_capacity * sizeof(T));
                // return;
            }

            size_t old_allocation_size = (this->m_capacity * sizeof(T));
            this->m_capacity *= 2;
            size_t new_allocation_size = (this->m_capacity * sizeof(T));
            this->m_data = (T*)Memory::Realloc(this->m_data, old_allocation_size, new_allocation_size);
        }
    private:
        T* m_data = nullptr;
        u64 m_count = 0;
        u64 m_capacity = 0;
    };
    
    template <typename T>
    struct RingQueue {
        RingQueue(size_t capacity = 1) {
            RUNTIME_ASSERT(capacity > 0);
            
            this->m_count = 0;
            this->m_capacity = capacity;
            this->m_data = (T*)Memory::Malloc(this->m_capacity * sizeof(T));
        }

        ~RingQueue() {
            Memory::Free(this->m_data);

            this->m_data = nullptr;
            this->m_count = 0;
            this->m_capacity = 0;

        }

        void enqueue(T value) {
            RUNTIME_ASSERT_MSG(!this->full(), "You may not enqueue if the ring queue is full!\n");

            this->m_data[this->m_write] = value;
            this->m_count += 1;
            this->m_write = (this->m_write + 1) % this->m_capacity;
        }
        
        T dequeue() {
            RUNTIME_ASSERT_MSG(!this->empty(), "You may not dequeue if the ring queue is empty!\n");

            T ret = this->m_data[this->m_read];
            this->m_count -= 1;
            this->m_read = (this->m_read + 1) % this->m_capacity;

            return ret;
        }

        bool empty() const {
            return this->m_count == 0;
        }

        bool full() const {
            return this->m_count == this->m_capacity;
        }

        u64 count() const {
            return this->m_count;
        }

        u64 capacity() const {
            return this->m_capacity;
        }

    private:
        T* m_data = nullptr;
        u64 m_count = 0;
        u64 m_capacity = 0;

        size_t m_read = 0;
        size_t m_write = 0;
    };
}