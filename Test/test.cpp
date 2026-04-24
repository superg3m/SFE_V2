#include <core.hpp>

struct Point {
    int x;
    int y;

    static u64 hash(const void* source, size_t source_size) {
        Point* p = (Point*)source;

        return p->x ^ p->y << 3;
    }

    bool operator==(Point other) const {
        return (this->x == other.x) && (this->y == other.y);
    }
};

// ---------- Tests ----------
void test_basic_put_get(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    map.put(1, 10);
    map.put(2, 20);
    RUNTIME_ASSERT(map.has(1));
    RUNTIME_ASSERT(map.has(2));
    RUNTIME_ASSERT(map.get(1) == 10);
    RUNTIME_ASSERT(map.get(2) == 20);
    LOG_INFO("test_basic_put_get passed\n");
}
void test_overwrite(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    map.put(1, 10);
    map.put(1, 99);
    RUNTIME_ASSERT(map.has(1));
    RUNTIME_ASSERT(map.get(1) == 99);
    LOG_INFO("test_overwrite passed\n");
}
void test_remove(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    map.put(42, 100);
    RUNTIME_ASSERT(map.has(42));
    int val = map.remove(42);
    RUNTIME_ASSERT(val == 100);
    RUNTIME_ASSERT(!map.has(42));
    LOG_INFO("test_remove passed\n");
}
struct BadHash {
    static u64 hash(const void* data, size_t) {
        return 0; // force collisions
    }
};

void test_collisions(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator, 4, (HashFunction*)BadHash::hash);
    map.put(1, 100);
    map.put(2, 200);
    map.put(3, 300);
    RUNTIME_ASSERT(map.get(1) == 100);
    RUNTIME_ASSERT(map.get(2) == 200);
    RUNTIME_ASSERT(map.get(3) == 300);
    LOG_INFO("test_collisions passed\n");
}
void test_rehashing(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    for (int i = 0; i < 4; i++) {
        map.put(i, i * 10);
    }
    for (int i = 0; i < 4; i++) {
        RUNTIME_ASSERT(map.has(i));
        RUNTIME_ASSERT(map.get(i) == i * 10);
    }
    LOG_INFO("test_rehashing passed\n");
}
void test_string_keys(Allocator allocator) {
    Hashmap<const char*, int> map = Hashmap<const char*, int>(allocator);
    map.put("apple+pen", 1);
    map.put("banana", 2);
    RUNTIME_ASSERT(map.has("apple+pen"));
    RUNTIME_ASSERT(map.has("banana"));
    RUNTIME_ASSERT(map.get("apple+pen") == 1);
    RUNTIME_ASSERT(map.get("banana") == 2);
    LOG_INFO("test_string_keys passed\n");
}
void test_dead_entries(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    map.put(1, 10);
    map.put(2, 20);
    map.remove(1);
    RUNTIME_ASSERT(!map.has(1));
    RUNTIME_ASSERT(map.has(2));
    RUNTIME_ASSERT(map.get(2) == 20);
    map.put(1, 99);
    RUNTIME_ASSERT(map.get(1) == 99);
    LOG_INFO("test_dead_entries passed\n");
}
void test_stress(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    for (int i = 0; i < 1000; i++) {
        map.put(i, i);
    }
    for (int i = 0; i < 1000; i++) {
        RUNTIME_ASSERT(map.has(i));
        RUNTIME_ASSERT(map.get(i) == i);
    }
    for (int i = 0; i < 1000; i++) {
        map.remove(i);
    }
    for (int i = 0; i < 1000; i++) {
        RUNTIME_ASSERT(!map.has(i));
    }
    LOG_INFO("test_stress passed\n");
}

void test_put_get_large_values(Allocator allocator) {
    Hashmap<int, double> map = Hashmap<int, double>(allocator);
    for (int i = 0; i < 500; ++i) {
        map.put(i, static_cast<double>(i) * 3.14);
    }

    for (int i = 0; i < 500; ++i) {
        RUNTIME_ASSERT(map.has(i));
        RUNTIME_ASSERT(map.get(i) == static_cast<double>(i) * 3.14);
    }

    LOG_INFO("test_put_get_large_values passed\n");
}

void test_remove_nonexistent(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    map.put(1, 10);
    // map.remove(2);
    // RUNTIME_ASSERT(!removed);
    RUNTIME_ASSERT(map.has(1));
    LOG_INFO("test_remove_nonexistent passed\n");
}

void test_has_nonexistent(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    map.put(1, 10);
    RUNTIME_ASSERT(!map.has(2));
    LOG_INFO("test_has_nonexistent passed\n");
}

void test_string_view_keys(Allocator allocator) {
    Hashmap<String, int> map = Hashmap<String, int>(allocator);
    String k1 = String("key1testings", sizeof("key1testings") - 1);
    String k2 = String("key2testingstestings", sizeof("key2testingstestings") - 1);
    String k3 = String("key3testings", sizeof("key3testings") - 1);

    map.put(k1, 100);
    map.put(k2, 200);
    RUNTIME_ASSERT(map.has(k1));
    RUNTIME_ASSERT(map.has(k2));
    RUNTIME_ASSERT(map.get(k1) == 100);
    RUNTIME_ASSERT(map.get(k2) == 200);

    map.remove(k1);
    RUNTIME_ASSERT(!map.has(k1));
    RUNTIME_ASSERT(map.has(k2));

    map.put(k3, 300);
    RUNTIME_ASSERT(map.get(k3) == 300);
    LOG_INFO("test_string_view_keys passed\n");
}

void test_cstring_keys(Allocator allocator) {
    Hashmap<const char*, int> map = Hashmap<const char*, int>(allocator);

    const char* key1 = "DFLKSJDFKDSKLFJDSKLJF";
    const char* key2 = "TSLKFJSFLKSJFLJDSLJFDLSF";
    const char* key3 = "dfgdfSFLKSJFLJDSLJFDLSF";

    map.put(key1, 100);
    map.put(key2, 200);
    map.put(key3, 300);

    RUNTIME_ASSERT(map.get(key1) == 100);
    RUNTIME_ASSERT(map.get(key2) == 200);
    RUNTIME_ASSERT(map.get(key3) == 300);
   
    LOG_INFO("test_string_view_keys passed\n");
}

void test_custom_struct_keys(Allocator allocator) {
    Hashmap<Point, int> map = Hashmap<Point, int>(allocator, 1, Point::hash);
    Point p1 = {1, 2};
    Point p2 = {3, 4};
    Point p3 = {1, 2}; // Same as p1

    map.put(p1, 100);
    map.put(p2, 200);
    RUNTIME_ASSERT(map.has(p1));
    RUNTIME_ASSERT(map.has(p2));
    RUNTIME_ASSERT(map.has(p3)); // Should be true due to custom equality
    RUNTIME_ASSERT(map.get(p1) == 100);
    RUNTIME_ASSERT(map.get(p3) == 100);
    RUNTIME_ASSERT(map.get(p2) == 200);

    map.put(p1, 999); // Overwrite
    RUNTIME_ASSERT(map.get(p3) == 999);

    int val = map.remove(p2);
    RUNTIME_ASSERT(val == 200);
    RUNTIME_ASSERT(!map.has(p2));
    RUNTIME_ASSERT(map.has(p1));
    
    LOG_INFO("test_custom_struct_keys passed\n");
}

void test_edge_cases(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    map.put(1, 1);
    map.put(2, 2);
    map.put(3, 3);
    map.remove(2);
    map.put(4, 4);
    RUNTIME_ASSERT(map.has(1));
    RUNTIME_ASSERT(!map.has(2));
    RUNTIME_ASSERT(map.has(3));
    RUNTIME_ASSERT(map.has(4));
    RUNTIME_ASSERT(map.get(1) == 1);
    RUNTIME_ASSERT(map.get(3) == 3);
    RUNTIME_ASSERT(map.get(4) == 4);
    LOG_INFO("test_edge_cases passed\n");
}

void test_clear(Allocator allocator) {
    Hashmap<int, int> map = Hashmap<int, int>(allocator);
    map.put(1, 10);
    map.put(2, 20);
    map.clear();
    RUNTIME_ASSERT(map.count == 0);
    RUNTIME_ASSERT(!map.has(1));
    RUNTIME_ASSERT(!map.has(2));
    LOG_INFO("test_clear passed\n");
}

int main() {
    constexpr int CAPACITY = KB(98);
    u8 program_memory[CAPACITY];
    Arena arena = Arena::fixed(program_memory, CAPACITY);
    Allocator arena_allocator = arena.to_allocator();

    Temp temp = Temp::begin(&arena); 
        test_basic_put_get(arena_allocator);
        test_overwrite(arena_allocator);
        test_remove(arena_allocator);
        test_collisions(arena_allocator);
        test_rehashing(arena_allocator);
        test_string_keys(arena_allocator);
        test_dead_entries(arena_allocator);
    temp.end();

    temp = Temp::begin(&arena);
        test_stress(arena_allocator);
        test_put_get_large_values(arena_allocator);
        test_remove_nonexistent(arena_allocator);
        test_has_nonexistent(arena_allocator);
        test_string_view_keys(arena_allocator);
        test_cstring_keys(arena_allocator);
        test_custom_struct_keys(arena_allocator);
        test_edge_cases(arena_allocator);
        test_clear(arena_allocator);
    temp.end();

    LOG_INFO("All tests passed\n");
    
    return 0;
}