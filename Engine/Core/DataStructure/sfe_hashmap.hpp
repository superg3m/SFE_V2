#include <initializer_list>
#include <type_traits>

#include "../Memory/sfe_memory.hpp"
#include "../String/sfe_string.hpp"
#include "../Hashing/sfe_hashing.hpp"
#include "sfe_view.hpp"

#define HASHMAP_DEFAULT_getLoadFactor 0.7f

namespace DS {
    typedef u64(HashFunction)(const void*, size_t);
    typedef bool(EqualFunction)(const void*, size_t, const void*, size_t);

    template <typename K, typename V>
    struct Hashmap {
        struct HashmapEntry {
            K key;
            V value;
            bool filled = false;
            bool dead = false;
        };

        struct InitPair {
            K key;
            V value;
        };

        Hashmap() {
            this->m_count = 0;
            this->m_capacity = 0;
            this->m_entries = nullptr;
        }

        Hashmap(u64 capacity) {
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            constexpr bool key_is_cstring = std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            constexpr bool key_is_string_view = std::is_same_v<K, DS::View<char>> || std::is_same_v<K, DS::View<const char>>;
            
            STATIC_ASSERT((key_is_trivial && !key_is_pointer) || key_is_cstring || key_is_string_view);

            this->m_count = 0;
            this->m_capacity = capacity;
            this->m_entries = (HashmapEntry*)Memory::Malloc(this->m_capacity * sizeof(HashmapEntry));

            if constexpr (key_is_trivial && !key_is_pointer) {
                this->m_hash_func = Hashing::siphash24;
                this->m_equal_func = Memory::Equal;
            } else if constexpr (key_is_cstring) {
                this->m_hash_func = Hashing::cstringHash;
                this->m_equal_func = Hashing::cstringEquality;
            } else if constexpr (key_is_string_view) {
                this->m_hash_func = Hashing::stringViewHash;
                this->m_equal_func = Hashing::stringViewEquality;
            }
        }

        Hashmap(std::initializer_list<InitPair> list) {
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            constexpr bool key_is_cstring = std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            constexpr bool key_is_string_view = std::is_same_v<K, DS::View<char>> || std::is_same_v<K, DS::View<const char>>;

            STATIC_ASSERT((key_is_trivial && !key_is_pointer) || key_is_cstring || key_is_string_view);

            this->m_count = list.size();
            this->m_capacity = this->m_count * 2;
            this->m_entries = (HashmapEntry*)Memory::Malloc(this->m_capacity * sizeof(HashmapEntry));

            if constexpr (key_is_trivial && !key_is_pointer) {
                this->m_hash_func = Hashing::siphash24;
                this->m_equal_func = Memory::Equal;
            } else if constexpr (key_is_cstring) {
                this->m_hash_func = Hashing::cstringHash;
                this->m_equal_func = Hashing::cstringEquality;
            } else if constexpr (key_is_string_view) {
                this->m_hash_func = Hashing::stringViewHash;
                this->m_equal_func = Hashing::stringViewEquality;
            }

            for (InitPair pair : list) {
                this->put(pair.key, pair.value);
            }
        }

        Hashmap(HashFunction* hash_func, EqualFunction* equal_func, u64 capacity = 1){
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            STATIC_ASSERT(key_is_trivial && !key_is_pointer);

            this->m_count = 0;
            this->m_capacity = capacity;

            this->m_entries = (HashmapEntry*)Memory::Malloc(this->m_capacity * sizeof(HashmapEntry));

            this->m_hash_func = hash_func;
            this->m_equal_func = equal_func;
        }

        Hashmap& operator=(const Hashmap& other) {
            if (this != &other) {
                if (other.m_entries == nullptr) return *this;

                if (this->m_entries) {
                    Memory::Free(m_entries);
                }

                this->m_count = other.m_count;
                this->m_capacity = other.m_capacity;
                this->m_dead_count = other.m_dead_count;
                this->m_hash_func = other.m_hash_func;
                this->m_equal_func = other.m_equal_func;

                this->m_entries = (HashmapEntry*)Memory::Malloc(m_capacity * sizeof(HashmapEntry));
                Memory::Copy(this->m_entries, this->m_capacity * sizeof(HashmapEntry), other.m_entries,  this->m_capacity * sizeof(HashmapEntry));
            }

            return *this;
        }

        void put(K key, V value) {
            RUNTIME_ASSERT(this->m_entries); 

            if (this->getLoadFactor() >= HASHMAP_DEFAULT_getLoadFactor) {
                this->growRehash();
            }

            u64 hash = this->safeHash(key);
            s64 index = this->resolveCollision(key, hash % this->m_capacity);
            RUNTIME_ASSERT(index != -1);

            HashmapEntry* entry = &this->m_entries[index];
            if (!entry->filled || entry->dead) {
                this->m_count += 1;
            }

            entry->key = key;
            entry->value = value;
            entry->filled = true;
        }

        bool has(K key) const {
            RUNTIME_ASSERT(this->m_entries && this->m_hash_func && this->m_equal_func); 

            u64 hash = this->safeHash(key);
            s64 index = this->resolveCollision(key, hash % this->m_capacity);
            if (index == -1) {
                return false;
            }

            HashmapEntry* entry = &this->m_entries[index];

            return entry->filled && !entry->dead;
        }

        V get(K key) const {
            RUNTIME_ASSERT_MSG(this->has(key), "Key doesn't exist\n");

            u64 hash = this->safeHash(key);
            s64 index = this->resolveCollision(key, hash % this->m_capacity);
            RUNTIME_ASSERT(index != -1);
            
            HashmapEntry* entry = &this->m_entries[index];


            return entry->value;
        }

        V remove(K key) {
            RUNTIME_ASSERT_MSG(this->has(key), "Key doesn't exist\n");

            u64 hash = this->safeHash(key);
            s64 index = this->resolveCollision(key, hash % this->m_capacity);
            RUNTIME_ASSERT(index != -1);
            
            HashmapEntry* entry = &this->m_entries[index];

            this->m_count -= 1;
            this->m_dead_count += 1;
            entry->dead = true;

            return entry->value;
        }

        void clear() {
            this->m_count = 0;
            this->m_dead_count = 0;
            Memory::Zero(this->m_entries, sizeof(HashmapEntry) * this->m_capacity);
        }

        u64 count() {
            return this->m_count;
        }

        HashmapEntry* begin() { 
            return m_entries; 
        }
        HashmapEntry* end() { 
            return m_entries + m_capacity; 
        }
        const HashmapEntry* begin() const { 
            return m_entries; 
        }
        const HashmapEntry* end() const { 
            return m_entries + m_capacity; 
        }

    private:
        u64 m_count = 0;
        u64 m_capacity = 0;
        u64 m_dead_count = 0;
        HashmapEntry* m_entries = nullptr;
        HashFunction* m_hash_func = nullptr;
        EqualFunction* m_equal_func = nullptr;

        void growRehash() {
            u64 old_capacity = this->m_capacity;
            HashmapEntry* old_entries = this->m_entries;

            this->m_capacity *= 2;
            size_t new_allocation_size = (this->m_capacity * sizeof(HashmapEntry));
            this->m_entries = (HashmapEntry*)Memory::Malloc(new_allocation_size);
            Memory::Zero(this->m_entries, new_allocation_size);

            // rehash
            for (u64 i = 0; i < old_capacity; i++) {
                HashmapEntry old_entry = old_entries[i];
                if (!old_entry.filled || old_entry.dead) {
                    continue;
                }

                u64 hash = this->safeHash(old_entry.key);
                s64 index = this->resolveCollision(old_entry.key, hash % this->m_capacity);
                RUNTIME_ASSERT(index != -1);

                this->m_entries[index] = old_entry;
            }

            Memory::Free(old_entries);
        }

        s64 resolveCollision(K key, u64 inital_hash_index) const {
            s64 cannonical_hash_index = inital_hash_index;

            u64 visited_count = 0;
            while (true) {
                HashmapEntry* entry = &this->m_entries[cannonical_hash_index];
                if (!entry->filled) {
                    break;
                }

                bool equality_match = this->safeEquality(key, entry->key);
                if (equality_match) {
                    break;
                }

                if (visited_count > this->m_capacity) {
                    return -1;
                }

                visited_count += 1;
                cannonical_hash_index += 1;
                cannonical_hash_index = cannonical_hash_index % this->m_capacity;
            }

            return cannonical_hash_index;
        }

        float getLoadFactor() {
            return (float)(this->m_dead_count + this->m_count) / (float)this->m_capacity;
        }
        
        #define NOT_USED 0

        u64 safeHash(K key) const {
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            constexpr bool key_is_cstring = std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            constexpr bool key_is_string_view = std::is_same_v<K, DS::View<char>> || std::is_same_v<K, DS::View<const char>>;

            if constexpr (key_is_trivial && !key_is_pointer) {
                return this->m_hash_func(&key, sizeof(K));
            } else if constexpr (key_is_cstring) {
                return this->m_hash_func((void*)key, NOT_USED);
            } else if constexpr (key_is_string_view) {
                return this->m_hash_func(&key, NOT_USED);
            } else {
                RUNTIME_ASSERT_MSG(false, "WE ARE IN HELL?\n");
                return 0;
            }
        }

        bool safeEquality(K k1, K k2) const {
            constexpr bool key_is_trivial = std::is_trivially_copyable_v<K>;
            constexpr bool key_is_pointer = std::is_pointer_v<K>;
            constexpr bool key_is_cstring = std::is_same_v<K, char*> || std::is_same_v<K, const char*>;
            constexpr bool key_is_string_view = std::is_same_v<K, DS::View<char>> || std::is_same_v<K, DS::View<const char>>;

            if constexpr (key_is_trivial && !key_is_pointer) {
                return this->m_equal_func(&k1, sizeof(K), &k2, sizeof(K));
            } else if constexpr (key_is_cstring) {
                return this->m_equal_func(k1, NOT_USED, k2, NOT_USED);
            } else if constexpr (key_is_string_view) {
                return this->m_equal_func(&k1, NOT_USED, &k2, NOT_USED);
            } else {
                RUNTIME_ASSERT_MSG(false, "WE ARE IN HELL?\n");
                return false;
            }
        }

        #undef NOT_USED
    };
}