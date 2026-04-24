#pragma once

#include <initializer_list>

#include "basic.hpp"
#include "memory.hpp"
#include "assert.hpp"
#include "hashing.hpp"

template <typename T>
struct Vector {
	u64 count = 0;
	u64 capacity = 0;
	T* data = nullptr;
	Allocator allocator = Allocator::invalid();

	Vector() {
		this->count = 0;
		this->capacity = 0;
		this->data = nullptr;
		this->allocator = Allocator::invalid();
	}

	Vector(Allocator allocator, std::initializer_list<T> list) {
        this->count = list.size();
        this->capacity = (this->count * 2) ? (this->count * 2) : DEFAULT_CAPACITY;
        this->allocator = allocator;

        if (list.size() == 0) return;

		this->count = list.size();
		this->capacity = this->count * 2;
		this->data = (T*)this->allocator.malloc(this->capacity * sizeof(T), alignof(T));
		Memory::copy(this->data, this->capacity * sizeof(T), list.begin(), list.size() * sizeof(T));
	}

	Vector(Allocator allocator, u64 capacity, u64 count = 0) {
		this->count = count;
        this->capacity = capacity;
        this->data = nullptr;
        this->allocator = allocator;
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

	void vector_append(Vector<T>* v, T value) {
        if (!v->data) { 
            v->data = (T*)v->allocator.malloc(v->capacity * sizeof(T), alignof(T));
        }

        if (v->count == v->capacity) {
            size_t old_allocation_size = (v->capacity * sizeof(T));
            v->capacity *= 2;
            size_t new_allocation_size = (v->capacity * sizeof(T));
            v->data = (T*)v->allocator.realloc(v->data, old_allocation_size, new_allocation_size, alignof(T));
        }

        v->data[v->count++] = value;
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

// -- 

/*
template <typename T>
struct Stack {
	Stack(u64 capacity = 1) {
		this->m_count = 0;
		this->m_capacity = capacity;

		this->m_data = (T*)Memory::Malloc(this->m_capacity * sizeof(T));
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
*/

// -- 

using HashFunction = u64(const void*, size_t);
using EqualFunction = bool(const void*, size_t, const void*, size_t);

template <typename K>
constexpr HashFunction* compile_time_get_hash_function() {
	if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
		return Hashing::cstring_hash;
	} else if (CompileTime<K>::TYPE_IS_STRING) {
		return Hashing::string_view_hash;
	} else if (CompileTime<K>::TYPE_IS_TRIVIAL || CompileTime<K>::TYPE_IS_POINTER) {
		return Hashing::siphash;
	} else {
		RUNTIME_ASSERT(false);
		return nullptr;
	}
}

template<typename K>
u64 hashmap_safe_hash(HashFunction* hash_func, K key) {
	if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
		return hash_func((void*)key, 0);
	} else if constexpr (CompileTime<K>::TYPE_IS_STRING) {
		return hash_func(&key, 0);
	} else if constexpr (CompileTime<K>::TYPE_IS_TRIVIAL && !CompileTime<K>::TYPE_IS_POINTER) {
		return hash_func(&key, sizeof(K));
	} else if constexpr (CompileTime<K>::TYPE_IS_POINTER) {
		return hash_func((void*)&key, sizeof(K));
	} else {
		RUNTIME_ASSERT_MSG(false, "WE ARE IN HELL?\n");
		return 0;
	}
}

template <typename K, typename V>
struct HashmapEntry {
    K key = {};
    V value = {};
    bool filled = false;
    bool dead = false;

    bool is_valid() const {
        return filled && !dead;
    }
};

template <typename K, typename V>
struct Hashmap {
    static_assert(
        CompileTime<K>::TYPE_IS_TRIVIAL || 
        CompileTime<K>::TYPE_IS_POINTER || 
        CompileTime<K>::TYPE_IS_CSTRING ||
        CompileTime<K>::TYPE_IS_STRING
    );

    u64 count = 0;
    u64 dead_count = 0;
    u64 capacity = 0;
    HashmapEntry<K, V>* entries = nullptr;
    HashFunction* hash_func = nullptr;
    Allocator allocator = Allocator::invalid();

    struct Iterator {
        HashmapEntry<K, V>* ptr;
        HashmapEntry<K, V>* end;

        Iterator(HashmapEntry<K, V>* p, HashmapEntry<K, V>* e)
            : ptr(p), end(e) {
            skip();
        }

        void skip() {
            while ((ptr != end) && !ptr->is_valid()) {
                ++ptr;
            }
        }

        HashmapEntry<K, V>& operator*() const {
            return *ptr;
        }

        Iterator& operator++() {
            ++ptr;
            skip();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return ptr != other.ptr;
        }
    };

    Iterator begin() {
        return Iterator(entries, entries + capacity);
    }

    Iterator end() {
        return Iterator(entries + capacity, entries + capacity);
    }

    float load_factor() const {
        return static_cast<float>(dead_count + count) / static_cast<float>(capacity);
    }

    Hashmap(Allocator allocator, u64 capacity = DEFAULT_CAPACITY, HashFunction* hash_func = compile_time_get_hash_function<K>()) {
		this->allocator = allocator;
		this->capacity = capacity;
		this->hash_func = hash_func;
	}

    Hashmap(Allocator allocator, std::initializer_list<KeyValuePair<K, V>> list, HashFunction* hf = compile_time_get_hash_function<K>()) {
		this->allocator = allocator;
		this->capacity = DEFAULT_CAPACITY;
		this->hash_func = hash_func;
	
        if (list.size() == 0) return;

        capacity = list.size() * 2;
        for (const auto& pair : list) {
            this->put(pair.key, pair.value);
        }
    }

    s64 resolve_collision(const K& key, u64 initial_hash_index) {
        s64 canonical_hash_index = initial_hash_index;

        u64 visited_count = 0;
        while (true) {
            auto* entry = &entries[canonical_hash_index];
            if (!entry->filled) {
                break;
            }

            bool equality_match = false;
            if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
                equality_match = String::equal(key, entry->key);
            } else {
                equality_match = (key == entry->key);
            }

            if (equality_match) {
                break;
            }

            if (visited_count > capacity) {
                return -1;
            }

            ++visited_count;
            canonical_hash_index = (canonical_hash_index + 1) % capacity;
        }

        return canonical_hash_index;
    }

    void grow_rehash() {
        if (!entries) {
            RUNTIME_ASSERT(capacity);
            entries = (HashmapEntry<K, V>*)allocator.malloc(capacity * sizeof(HashmapEntry<K, V>), alignof(HashmapEntry<K, V>));
            Memory::zero(entries, capacity * sizeof(HashmapEntry<K, V>));
            return;
        }

        u64 old_capacity = capacity;
        auto* old_entries = entries;

        capacity *= 2;
        size_t new_size = capacity * sizeof(HashmapEntry<K, V>);
        entries = (HashmapEntry<K, V>*)allocator.malloc(new_size, alignof(HashmapEntry<K, V>));
        Memory::zero(entries, new_size);

        for (u64 i = 0; i < old_capacity; ++i) {
            auto& old_entry = old_entries[i];
            if (!old_entry.filled || old_entry.dead) continue;

            u64 hash = hashmap_safe_hash<K>(hash_func, old_entry.key);
            s64 index = resolve_collision(old_entry.key, hash % capacity);
            RUNTIME_ASSERT(index != -1);

            entries[index] = old_entry;
        }

        allocator.free(old_entries);
    }

    bool has(const K& key) {
        if (!entries) return false;

        u64 hash = hashmap_safe_hash(hash_func, key);
        s64 index = resolve_collision(key, hash % capacity);
        if (index == -1) return false;

        return entries[index].is_valid();
    }

    void put(const K& key, const V& value) {
        if (!entries || load_factor() >= DEFAULT_LOAD_FACTOR) {
            this->grow_rehash();
        }

        u64 hash = hashmap_safe_hash<K>(hash_func, key);
        s64 index = resolve_collision(key, hash % capacity);
        RUNTIME_ASSERT(index != -1);

        auto* entry = &entries[index];
        if (!entry->filled || entry->dead) {
            ++count;
        }

        entry->key = key;
        entry->value = value;
        entry->filled = true;
        entry->dead = false;
    }

    V get(const K& key) {
        RUNTIME_ASSERT_MSG(has(key), "Key doesn't exist\n");

        u64 hash = hashmap_safe_hash(hash_func, key);
        s64 index = resolve_collision(key, hash % capacity);
        RUNTIME_ASSERT(index != -1);

        return entries[index].value;
    }

    V remove(const K& key) {
        RUNTIME_ASSERT_MSG(has(key), "Key doesn't exist\n");

        u64 hash = hashmap_safe_hash(hash_func, key);
        s64 index = resolve_collision(key, hash % capacity);
        RUNTIME_ASSERT(index != -1);

        auto* entry = &entries[index];

        --count;
        ++dead_count;
        entry->dead = true;

        return entry->value;
    }

    void clear() {
        count = 0;
        dead_count = 0;
        Memory::zero(entries, sizeof(HashmapEntry<K, V>) * capacity);
    }
};