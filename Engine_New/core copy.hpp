#pragma once

// This is gonna be like ckg (bro I can't fuckign believe it took this long to get to this point like fuck me)
// literally the thing that I learned a year ago or two at this point i'm refucking learning.
// fuck c++ fuck your devilish temptations

#define CORE_TYPES
#define CORE_DEFINES
#define CORE_LOGGING
#define CORE_ASSERT
#define CORE_MEMORY
#define CORE_MEMORY_ARENA
#define CORE_MATH
#define CORE_HANDLE_REGISTRY
#define CORE_STRING
#define CORE_HASHING
#define CORE_RANDOM
#define CORE_DATA_STRUCTURES
#define CORE_PLATFORM
#define CORE_FRONTEND

#if defined(CORE_IMPL)
    #define CORE_TYPES_IMPL
    #define CORE_DEFINES_IMPL
    #define CORE_LOGGING_IMPL
    #define CORE_ASSERT_IMPL
    #define CORE_MATH_IMPL
    #define CORE_STRING_IMPL
    #define CORE_MEMORY_IMPL
    #define CORE_MEMORY_ALLOCATOR_IMPL
    #define CORE_MEMORY_ARENA_IMPL
    #define CORE_HASHING_IMPL
    #define CORE_RANDOM_IMPL
    #define CORE_COLLECTION_IMPL
    #define CORE_CONTAINER_IMPL
    #define CORE_PLATFORM_IMPL
    #define CORE_FRONTEND_IMPL
#endif

#if defined(CORE_RANDOM)
    constexpr u32 STATE_VECTOR_LENGTH = 624;
    constexpr u32 STATE_VECTOR_M      = 397;

    struct Seed {
        u32 mt[STATE_VECTOR_LENGTH];
        u32 index = 0;
    };

    Seed random_generate_seed(u32 seed); // non-zero seeds work
    u32 random_generate_u32(Seed* seed);
    float random_generate_f32(Seed* seed); // 0 to 1 0.12190845 for example
    float random_generate_range(Seed* seed, float min, float max); // min to max for example
#endif

#if defined(CORE_DATA_STRUCTURES)
    #include <initializer_list>

    template <typename T>
    struct Vector {
        u64 count;
        u64 capacity;
        T* data;
        Allocator allocator;

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
    }; INVARIENT_TRIVAL_TYPE(Vector<int>);

    template<typename T>
    Vector<T> vector_create(Allocator allocator, u64 capacity = DEFAULT_CAPACITY) {
        Vector<T> v = {};
        v.count = 0;
        v.capacity = capacity;
        v.data = nullptr;
        v.allocator = allocator;

        return v;
    }

    template<typename T>
    Vector<T> vector_create(Allocator a, std::initializer_list<T> list) {
        Vector<T> v = {};
        v.count = list.size();
        v.capacity = (v.count * 2) ? (v.count * 2) : DEFAULT_CAPACITY;
        v.allocator = a;

        if (list.size() == 0) return v;

        v.data = (T*)a.malloc(a.ctx, v.capacity * sizeof(T));
        memory_copy(v.data, v.capacity * sizeof(T), list.begin(), list.size() * sizeof(T));
    }

    template<typename T>
    void vector_grow(Vector<T>* v) {
        if (!v->data) { 
            v->data = (T*)v->allocator.malloc(v->allocator.ctx, v->capacity * sizeof(T));
        }

        // TODO(Jovanni): Make this use an allocator
        if (v->count == v->capacity) {
            size_t old_allocation_size = (v->capacity * sizeof(T));
            v->capacity *= 2;
            size_t new_allocation_size = (v->capacity * sizeof(T));
            v->data = (T*)v->allocator.realloc(v->allocator.ctx, v->data, old_allocation_size, new_allocation_size);
        }
    }

    template<typename T>
    void vector_append(Vector<T>* v, T value) {
        vector_grow(v);

        v->data[v->count] = value;
        v->count += 1;
    }

    /**
     * If this returns -1 then it couldn not find the element
     * If there are multiple occurences of this element return the first one's index
     */
    template<typename T>
    int vector_find(Vector<T>* v, T element) {
        for (int i = 0; i < v->count; i++) {
            if (v->data[i] == element) {
                return i;
            }
        }

        return -1;
    }

    template<typename T>
    void vector_stable_remove(Vector<T>* v, int index) {
        RUNTIME_ASSERT_MSG(v->count > 0, "You may not remove if the vector is empty!\n");
        RUNTIME_ASSERT_MSG((index >= 0) && (v->count > index), "index is outside of bounds!\n");

        for (int i = index; i < v->count - 1; i++) {
            v->data[i] = v->data[i + 1];
        }

        v->count -= 1;
    }

    template<typename T>
    void vector_unstable_remove(Vector<T>* v, int index) {
        RUNTIME_ASSERT_MSG(v->count > 0, "You may not remove if the vector is empty!\n");
        RUNTIME_ASSERT_MSG((index >= 0) && (v->count > index), "index is outside of bounds!\n");

        v->data[index] = v->data[--v->count];
    }

    template<typename T>
    void vector_clear(Vector<T>* v) {
        v->count = 0;
        size_t allocation_size = v->capacity * sizeof(T);
        memory_zero(v->data, allocation_size);
    }

    template<typename T>
    void vector_resize(Vector<T>* v, u64 count) {
        const size_t total_allocation_size = sizeof(T) * v->capacity;

        // const size_t old_allocation_size = sizeof(T) * v->count;
        const size_t new_allocation_size = sizeof(T) * count;
        if ((v->data == nullptr) || new_allocation_size > total_allocation_size) {
            v->data = (T*)v->allocator.realloc(v->allocator.ctx, v->data, new_allocation_size);
        }

        v->count = count;
        v->capacity = count;
    }

    template<typename T>
    void vector_reserve(Vector<T>* v, u64 count) {
        const size_t total_allocation_size = sizeof(T) * v->capacity;

        // const size_t old_allocation_size = sizeof(T) * v->count;
        const size_t new_allocation_size = sizeof(T) * count;

        if (new_allocation_size > total_allocation_size) {
            v->data = (T*)v->allocator.malloc(v->allocator.ctx, v->data, new_allocation_size);
            v->capacity = count;
        }
    }

    template<typename T>
    void vector_copy(Vector<T>* destination, Vector<T>* source) {
        RUNTIME_ASSERT(destination->data == nullptr);

        destination->capacity = source->capacity;
        destination->count = source->count;

        if (source->data == nullptr) return;

        size_t allocation_size = destination->capacity * sizeof(T);
        destination->data = (T*)destination->allocator.malloc(destination->allocator.ctx, allocation_size);
        memory_copy(destination->data, allocation_size, source->data, allocation_size);
    }

    // -- 

    typedef u64(HashFunction)(const void*, size_t);
    typedef bool(EqualFunction)(const void*, size_t, const void*, size_t);

    template <typename K, typename V>
    struct HashmapEntry {
        K key;
        V value;
        bool filled = false;
        bool dead = false;

        bool IsValid() {
            return this->filled && !this->dead;
        }
    };

    template <typename K, typename V>
    struct Hashmap {
        STATIC_ASSERT(
            CompileTime<K>::TYPE_IS_TRIVIAL || 
            CompileTime<K>::TYPE_IS_POINTER || 
            CompileTime<K>::TYPE_IS_CSTRING ||
            CompileTime<K>::TYPE_IS_STRING_VIEW
        );

        u64 count;
        u64 dead_count;
        u64 capacity;
        HashmapEntry<K, V>* entries;
        HashFunction* hash_func;
        Allocator allocator;

        struct Iterator {
            HashmapEntry<K, V>* ptr;
            HashmapEntry<K, V>* end;

            Iterator(HashmapEntry<K, V>* p, HashmapEntry<K, V>* e) : ptr(p), end(e) {
                skip();
            }

            void skip() {
                while ((ptr != end) && !ptr->IsValid()) {
                    ptr++;
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
            return Iterator(this->entries, this->entries + this->capacity);
        }

        Iterator end() {
            return Iterator(this->entries + this->capacity, this->entries + this->capacity);
        }

        float load_factor() const {
            return (float)(this->dead_count + this->count) / (float)this->capacity;
        }
    };

    template <typename K>
    constexpr HashFunction* compile_time_get_hash_function() {
        if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
            return Hashing::cstring_hash;
        } else if (CompileTime<K>::TYPE_IS_STRING_VIEW) {
            return Hashing::string_view_hash;
        } else if (CompileTime<K>::TYPE_IS_TRIVIAL || CompileTime<K>::TYPE_IS_POINTER) {
            return Hashing::siphash;
        } else {
            RUNTIME_ASSERT(false);
            return nullptr;
        }
    }

    /*
    template <typename K>
    constexpr EqualFunction* compile_time_get_equality_function() {
        if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
            return Hashing::cstring_equality;
        } else if (CompileTime<K>::TYPE_IS_STRING_VIEW) {
            return Hashing::string_view_equality;
        } else if (CompileTime<K>::TYPE_IS_TRIVIAL || CompileTime<K>::TYPE_IS_POINTER) {
            return memory_equal;
        } else {
            RUNTIME_ASSERT(false);
            return nullptr;
        }
    }
    */

    template<typename K>
    u64 hashmap_safe_hash(HashFunction* hash_func, K key) {
        if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
            return hash_func((void*)key, 0);
        } else if constexpr (CompileTime<K>::TYPE_IS_STRING_VIEW) {
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

    /*
    template<typename K>
    bool hashmap_safe_equality(EqualFunction equal_func, K k1, K k2) {
        if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
            return equal_func(k1, 0, k2, 0);
        } else if constexpr (CompileTime<K>::TYPE_IS_STRING_VIEW) {
            return equal_func(&k1, 0, &k2, 0);
        } else if constexpr (CompileTime<K>::TYPE_IS_TRIVIAL && !CompileTime<K>::TYPE_IS_POINTER) {
            return equal_func(&k1, sizeof(K), &k2, sizeof(K));
        } else if constexpr (CompileTime<K>::TYPE_IS_POINTER) {
            return equal_func((void*)&k1, sizeof(K), (void*)&k2, sizeof(K));
        } else {
            RUNTIME_ASSERT_MSG(false, "WE ARE IN HELL?\n");
            return false;
        }
    }
    */

    template<typename K, typename V>
    s64 hashmap_resolve_collision(Hashmap<K, V>* h,  K key, u64 inital_hash_index) {
        s64 cannonical_hash_index = inital_hash_index;

        u64 visited_count = 0;
        while (true) {
            HashmapEntry<K, V>* entry = &h->entries[cannonical_hash_index];
            if (!entry->filled) {
                break;
            }

            bool equality_match = false;
            if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
                equality_match = str_equal(key, entry->key);
            } else {
                equality_match = (key == entry->key);
            }
            
            if (equality_match) {
                break;
            }

            if (visited_count > h->capacity) {
                return -1;
            }

            visited_count += 1;
            cannonical_hash_index += 1;
            cannonical_hash_index = cannonical_hash_index % h->capacity;
        }

        return cannonical_hash_index;
    }

    template<typename K, typename V>
    void hashmap_grow_rehash(Hashmap<K, V>* h) {
        if (!h->entries) {
            RUNTIME_ASSERT(h->capacity);
            h->entries = (HashmapEntry<K, V>*)h->allocator.malloc(h->allocator.ctx, h->capacity * sizeof(HashmapEntry<K, V>));
            memory_zero(h->entries, h->capacity * sizeof(HashmapEntry<K, V>));
            return;
        }

        u64 old_capacity = h->capacity;
        HashmapEntry<K, V>* old_entries = h->entries;

        h->capacity *= 2;
        size_t new_allocation_size = (h->capacity * sizeof(HashmapEntry<K, V>));
        h->entries = (HashmapEntry<K, V>*)h->allocator.malloc(h->allocator.ctx, new_allocation_size);
        memory_zero(h->entries, new_allocation_size);

        // rehash
        for (u64 i = 0; i < old_capacity; i++) {
            HashmapEntry<K, V> old_entry = old_entries[i];
            if (!old_entry.filled || old_entry.dead) {
                continue;
            }

            u64 hash = hashmap_safe_hash<K>(h->hash_func, old_entry.key);
            s64 index = hashmap_resolve_collision<K, V>(h, old_entry.key, hash % h->capacity);
            RUNTIME_ASSERT(index != -1);

            h->entries[index] = old_entry;
        }

        h->allocator.free(h->allocator.ctx, old_entries);
    }

    template<typename K, typename V>
    Hashmap<K, V> hashmap_create(
        Allocator allocator,
        u64 capacity = DEFAULT_CAPACITY,
        HashFunction* hash_func = compile_time_get_hash_function<K>()
    ) {
        Hashmap<K, V> h = {};
        h.count = 0;
        h.capacity = capacity;
        h.hash_func = hash_func;
        h.entries = nullptr;
        h.allocator = allocator;

        return h;
    }

    template<typename K, typename V>
    Hashmap<K, V> hashmap_create(
        Allocator allocator,
        std::initializer_list<KeyValuePair<K, V>> list,
        HashFunction* hash_func = compile_time_get_hash_function<K>()
    ) {
        Hashmap<K, V> h = {};
        h.count = 0;
        h.capacity = DEFAULT_CAPACITY;
        h.hash_func = hash_func;
        h.entries = nullptr;
        h.allocator = allocator;

        if (list.size() == 0) return h;

        h.capacity = list.size() * 2;
        for (KeyValuePair<K, V> pair : list) {
            hashmap_put(&h, pair.key, pair.value);
        }

        return h;
    }
    
    template<typename K, typename V>
    bool hashmap_has(Hashmap<K, V>* h, K key) {
        if (h->entries == nullptr) return false;

        u64 hash = hashmap_safe_hash(h->hash_func, key);
        s64 index = hashmap_resolve_collision(h, key, hash % h->capacity);
        if (index == -1) {
            return false;
        }

        HashmapEntry<K, V>* entry = &h->entries[index];
        return entry->IsValid();
    }

    template<typename K, typename V>
    void hashmap_put(Hashmap<K, V>* h, K key, V value) {
        if ((h->entries == nullptr) || (h->load_factor() >= DEFAULT_LOAD_FACTOR)) {
            hashmap_grow_rehash(h);
        }

        u64 hash = hashmap_safe_hash<K>(h->hash_func, key);
        s64 index = hashmap_resolve_collision<K>(h, key, hash % h->capacity);
        RUNTIME_ASSERT(index != -1);

        HashmapEntry<K, V>* entry = &h->entries[index];
        if (!entry->filled || entry->dead) {
            h->count += 1;
        }

        entry->key = key;
        entry->value = value;
        entry->filled = true;
        entry->dead = false;
    }

    template<typename K, typename V>
    V hashmap_get(Hashmap<K, V>* h, K key) {
        RUNTIME_ASSERT_MSG(hashmap_has(h, key), "Key doesn't exist\n");

        u64 hash = hashmap_safe_hash(h->hash_func, key);
        s64 index = hashmap_resolve_collision(h, key, hash % h->capacity);
        RUNTIME_ASSERT(index != -1);
        
        HashmapEntry<K, V>* entry = &h->entries[index];
        return entry->value;
    }

    template<typename K, typename V>
    V hashmap_remove(Hashmap<K, V>* h, K key) {
        RUNTIME_ASSERT_MSG(hashmap_has(h, key), "Key doesn't exist\n");

        u64 hash = hashmap_safe_hash(h->hash_func, key);
        s64 index = hashmap_resolve_collision(h, key, hash % h->capacity);
        RUNTIME_ASSERT(index != -1);
        
        HashmapEntry<K, V>* entry = &h->entries[index];

        h->count -= 1;
        h->dead_count += 1;
        entry->dead = true;

        return entry->value;
    }
    
    template<typename K, typename V>
    void hashmap_clear(Hashmap<K, V>* h) {
        h->count = 0;
        h->dead_count = 0;
        memory_zero(h->entries, sizeof(HashmapEntry<K, V>) * h->capacity);
    }
    
    // --

    template <typename T>
    struct Stack {
        T* data;
        u64 count;
        u64 capacity;
        Allocator allocator;
    }; INVARIENT_TRIVAL_TYPE(Stack<int>);

    template<typename T>
    void stack_grow(Stack<T>* s) {
        if (!s->data) { 
            s->data = (T*)s->allocator.malloc(s->allocator.ctx, s->capacity * sizeof(T));
        }

        // TODO(Jovanni): Make this use an allocator
        if (s->count == s->capacity) {
            size_t old_allocation_size = (s->capacity * sizeof(T));
            s->capacity *= 2;
            size_t new_allocation_size = (s->capacity * sizeof(T));
            s->data = (T*)s->allocator.realloc(s->allocator.ctx, s->data, old_allocation_size, new_allocation_size);
        }
    }

    template<typename T>
    T stack_peek(Stack<T>* s) {
        RUNTIME_ASSERT_MSG(!stack_empty(s), "You may not peek if the stack is empty!\n");
        return s->data[s->count - 1];
    }

    template<typename T>
    bool stack_empty(Stack<T>* s) {
        return s->count == 0;
    }

    template<typename T>
    Stack<T> stack_create(Allocator allocator) {
        Stack<T> s = {};
        s.allocator = allocator;
        s.capacity = DEFAULT_CAPACITY;

        return s;
    }

    template<typename T>
    void stack_push(Stack<T>* s, T value) {
        if (!s->data || (s->count == s->capacity)) {
            stack_grow(s);
        }

        s->data[s->count++] = value;
    }

    template<typename T>
    T stack_pop(Stack<T>* s) {
        RUNTIME_ASSERT_MSG(!stack_empty(s), "You may not pop if the stack is empty!\n");

        return s->data[--s->count];
    }

    template<typename T>
    void stack_copy(Stack<T>* destination, Stack<T>* source) {
        destination->capacity = source->capacity;
        destination->count = source->count;

        size_t allocation_size = destination->capacity * sizeof(T);
        destination->data = (T*)destination->allocator.malloc(destination->allocator.ctx, allocation_size);
        memory_copy(destination->data, allocation_size, source->data, allocation_size);
    }

    // --

    template <typename T, int N>
    struct RingBuffer {
        T data[N] = {};
        u64 count = 0;
        const u64 capacity = N;

        size_t read = 0;
        size_t write = 0;
    }; INVARIENT_TRIVAL_TYPE(RingBuffer<int, 5>);

    template <typename T, int N>
    void ring_buffer_enqueue(RingBuffer<T, N>* b, T value) {
        RUNTIME_ASSERT_MSG(!b->Full(), "You may not enqueue if the ring queue is full!\n");

        b->data[b->write] = value;
        b->count += 1;
        b->write = (b->write + 1) % b->capacity;
    }

    template <typename T, int N>
    T ring_buffer_dequeue(RingBuffer<T, N>* b) {
        RUNTIME_ASSERT_MSG(!b->Empty(), "You may not dequeue if the ring queue is empty!\n");

        T ret = b->data[b->read];
        b->count -= 1;
        b->read = (b->read + 1) % b->capacity;

        return ret;
    }

    template <typename T, int N>
    bool ring_buffer_empty(RingBuffer<T, N>* r) {
        return r->count == 0;
    }

    template <typename T, int N>
    bool ring_buffer_full(RingBuffer<T, N>* r)  {
        return r->count == r->capacity;
    }

    template <typename T, int N>
    void ring_buffer_copy(RingBuffer<T, N>* destination, RingBuffer<T, N>* source) {
        destination->count = source->count;
        destination->read = source->read;
        destination->write = source->write;

        size_t allocation_size = destination->capacity * sizeof(T);
        memory_copy(destination->data, allocation_size, source->data, allocation_size);
    }
#endif

#if defined(CORE_MEMORY_ARENA)
    // TODO(Jovanni): add linked list memory pages flag
    enum ArenaFlag {
        ARENA_FLAG_FIXED        = 0x1,
        ARENA_FLAG_CIRCULAR     = 0x2,
        ARENA_FLAG_STACK_MEMORY = 0x4
    };

    struct Arena {
        int flags = 0;
        size_t used = 0;
        size_t capacity = 0;
        u8 alignment = 0;
        u8* base_address = nullptr;

        Stack<size_t> stack_sizes;
        Stack<size_t>  temp_used_stack;
        Allocator allocator;
    };

    Arena arena_create(void* memory, size_t allocation_size, int flags, u8 alignment);
    void* arena_push(void* arena_ctx, size_t allocation_size);
    void arena_pop(Arena* arena, size_t to_pop);
    void arena_free(void* arena_ctx, void* data);
    void* arena_realloc(void* arena_ctx, void* data, size_t old_allocation_size, size_t new_allocation_size);
    Allocator arena_to_allocator(Arena* arena);

    void arena_begin_temp(Arena* arena);
    void arena_end_temp(Arena* arena);

    #define ArenaCreateFixed(memory, allocation_size, is_stack_memory) arena_create(memory, allocation_size, ARENA_FLAG_FIXED|(is_stack_memory ? ARENA_FLAG_STACK_MEMORY : 0), 8)
    #define ArenaCreateCircular(memory, allocation_size, is_stack_memory) arena_create(memory, allocation_size, ARENA_FLAG_CIRCULAR|(is_stack_memory ? ARENA_FLAG_STACK_MEMORY : 0), 8)
#endif

#if defined(CORE_HANDLE_REGISTRY)
    const s32 INVALID_HANDLE_INDEX = -1;

    template<typename T>
    struct Handle {
        s32 index = INVALID_HANDLE_INDEX;
        u32 generation = 0;

        bool operator==(Handle<T> other) const {
            return this->index == other.index && this->generation == other.generation;
        }

        bool operator!=(Handle<T> other) const {
            return !(*this == other);
        }
    };

    template<typename T>
    Handle<T> handle_create(u32 index, u32 generation) {
        Handle<T> ret = {};
        return ret;
    }

    template<typename T>
    Handle<T> handle_invalid() {
        Handle<T> ret = {};
        ret.index = -1;
        
        return ret;
    }

    // -- 

    template<typename T>
    struct Slot {
        bool allocated = false;
        u32 generation = 0;
        T data;
    };

    template<typename T>
    struct Registry {
        // I need LinkedList because the addresses won't be the same otherwise when this resizes...
        // I want like a freelist
        Vector<Slot<T>> slots;
        Allocator allocator;
    };

    template<typename T>
    Handle<T> registery_aquire_handle(Registry<T>* registry) {
        Handle<T> handle = handle_invalid<T>();
        for (int i = 0; i < registry->slots.count; i++) {
            Slot<T>& slot = registry->slots[i];
            if (slot.allocated == false) {
                handle = handle_create<T>(i, slot.generation);
                slot.allocated = true;
                break;
            }
        }

        RUNTIME_ASSERT(handle != handle_invalid<T>());

        /*
        if (handle == handle_invalid<T>()) {
            registry->slots.push(Slot<T>{});
            handle.index = registry->slots.count() - 1;
            handle.generation = registry->slots[handle.index].generation;
        }
        */

        memory_zero(&registry->slots[handle.index].data, sizeof(T));

        return handle;
    }

    template<typename T>
    bool registry_is_handle_valid(Registry<T>* registry, Handle<T> handle) {
        return handle.index != INVALID_HANDLE_INDEX && registry->slots[handle.index].generation == handle.generation;
    }

    template<typename T>
    Handle<T> registery_release_handle(Registry<T>* registry, Handle<T> handle) {
        RUNTIME_ASSERT(registry);
        RUNTIME_ASSERT(registry_is_handle_valid(registry, handle));

        registry->slots[handle.index].allocated = false;
        registry->slots[handle.index].generation++;
    }

    template<typename T>
    Registry<T> registry_create(Allocator allocator) {
        Registry<T> ret = {};

        ret.slots = vector_create<Slot<T>>(allocator, 100);
        ret.allocator = allocator;
        vector_resize(&ret.slots, 100);

        return ret;
    }

    template<typename T>
    T* registry_get(Registry<T>* registry, Handle<T> handle) {
        RUNTIME_ASSERT(registry);
        RUNTIME_ASSERT(registery_is_valid_handle(registry, handle));

        return &registry->slots[handle.index].data;
    }

    template<typename T>
    Handle<T> registry_find(Registry<T>* registry, T* element) {
        Handle<T> handle = handle_invalid<T>();
        for (int i = 0; i < registry->slots.count; i++) {
            Slot<T>& slot = registry->slots[i];
            if (element == &slot.data && slot.allocated) {
                handle = handle_create<T>(i, slot.generation);
                break;
            }
        }

        return handle;
    }
#endif