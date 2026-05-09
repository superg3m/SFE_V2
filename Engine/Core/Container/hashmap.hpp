#pragma once

#include "../Basic/basic.hpp"
#include "../Memory/memory.hpp"
#include "../Assert/assert.hpp"
#include "../Logger/logger.hpp"
#include "../Hashing/hashing.hpp"
#include "../String/string.hpp"
#include <initializer_list>

using HashFunction = u64(const void*, size_t);
using EqualFunction = bool(const void*, size_t, const void*, size_t);

template<typename T>
struct CompileTime {
	static constexpr bool TYPE_IS_CSTRING = std::is_same_v<T, char*> || std::is_same_v<T, const char*>;
	static constexpr bool TYPE_IS_STRING = std::is_same_v<T, String>;
	static constexpr bool TYPE_IS_TRIVIAL = std::is_trivially_copyable_v<T>;
	static constexpr bool TYPE_IS_POINTER = std::is_pointer_v<T>;
};

// NOTE(Jovanni):
/*
I have depricated const char* keys because I have found that when working accross dll boundaries
some sort of String (length based string struct) is necessary in order to support string interning.
if you have a const char* string literal from the dll and the dll is umapped / unloaded from you process
space you are in for a world of hurt. 

The remedy for this seemingly intractable problem is the have a dedicated string region in your program that
is purely used for unique string content. Then have a Hashmap<String, void*> cached_string 

Ok antoher thing thats really annoying across dll boundarires is the hash_fucntion on the actualy hashmap
basically if I load a dll and make a map it will use the data segmenet from the dll and when its unloaded that pointer is no longer valid.
there really isn't a good way I can think of to fix this other than to remove the ability to have runtime hash function...
*/

template <typename K>
constexpr HashFunction* compile_time_get_hash_function() {
	if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
		STATIC_ASSERT(false); // getting read to depricate cstring keys in favor of String{data, length}
		return Hashing::cstring_hash;
	} else if (CompileTime<K>::TYPE_IS_STRING) {
		return Hashing::string_view_hash;
	} else if (CompileTime<K>::TYPE_IS_TRIVIAL || CompileTime<K>::TYPE_IS_POINTER) {
		return Hashing::siphash;
	} else {
		RUNTIME_ASSERT_MSG(false, "WE ARE IN HELL?\n");
		return 0;// K::hash_function;
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
	u64 capacity = DEFAULT_CAPACITY;
	HashmapEntry<K, V>* entries = nullptr;
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
		if (!this->entries) {
			return Iterator(entries + capacity, entries + capacity);
		}

		return Iterator(entries, entries + capacity);
	}

	Iterator end() {
		return Iterator(entries + capacity, entries + capacity);
	}

	float load_factor() const {
		return static_cast<float>(dead_count + count) / static_cast<float>(capacity);
	}

	Hashmap(Allocator allocator = Allocator::invalid(), u64 capacity = DEFAULT_CAPACITY) {
		this->allocator = allocator;
		this->capacity = capacity;
	}

	Hashmap(std::initializer_list<KeyValuePair<K, V>> list, Allocator allocator = Allocator::invalid()) {
		this->allocator = allocator;
		this->capacity = DEFAULT_CAPACITY;
	
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

		HashFunction* hash_func = compile_time_get_hash_function<K>();
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

		HashFunction* hash_func = compile_time_get_hash_function<K>(); 
		u64 hash = hashmap_safe_hash(hash_func, key);
		s64 index = resolve_collision(key, hash % capacity);
		if (index == -1) return false;

		return entries[index].is_valid();
	}

	void put(const K& key, const V& value) {
		if (this->allocator == Allocator::invalid()) {
			this->allocator = Allocator::general();
		}

		if (!entries || load_factor() >= DEFAULT_LOAD_FACTOR) {
			this->grow_rehash();
		}

		HashFunction* hash_func = compile_time_get_hash_function<K>(); 
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

	V& operator[](const K& key) {
		RUNTIME_ASSERT_MSG(has(key), "Key doesn't exist\n");

		HashFunction* hash_func = compile_time_get_hash_function<K>(); 
		u64 hash = hashmap_safe_hash(hash_func, key);
		s64 index = resolve_collision(key, hash % capacity);
		RUNTIME_ASSERT(index != -1);

		return entries[index].value;
	}

	V get(const K& key) {
		RUNTIME_ASSERT_MSG(has(key), "Key doesn't exist\n");

		HashFunction* hash_func = compile_time_get_hash_function<K>(); 
		u64 hash = hashmap_safe_hash(hash_func, key);
		s64 index = resolve_collision(key, hash % capacity);
		RUNTIME_ASSERT(index != -1);

		return entries[index].value;
	}

	V remove(const K& key) {
		RUNTIME_ASSERT_MSG(has(key), "Key doesn't exist\n");

		HashFunction* hash_func = compile_time_get_hash_function<K>(); 
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

		if (this->entries) {
			Memory::zero(entries, sizeof(HashmapEntry<K, V>) * capacity);
		}
	}

	static void copy(Hashmap<K, V>* destination, Hashmap<K, V>* source) {
		RUNTIME_ASSERT(source);
		RUNTIME_ASSERT(destination);

		Memory::copy(destination, sizeof(Hashmap<K, V>), source, sizeof(Hashmap<K, V>));
		size_t allocation_size = source->capacity * sizeof(HashmapEntry<K, V>);
		destination->entries = (HashmapEntry<K, V>*)destination->allocator.malloc(allocation_size, alignof(HashmapEntry<K, V>));
		Memory::copy(destination->entries, allocation_size, source->entries, allocation_size);
	}
};