#include <sfe.hpp>

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

    // Renderer<Vulkan> renderer = Renderer<Vulkan>::create();
    /*
    float vertices[];
    BufferHandle vbo = renderer.CreateBuffer({
        .size = sizeof(vertices),
        .usage = BufferUsage::Vertex
    }, vertices);

    VertexLayout particleLayout = {
        .stride = sizeof(QuadVertex),
        .attributes = {
            // per-vertex (mesh)
            { .location = 0, .format = VEC3, .offset = 0, .instanced = false },

            // per-instance position
            { .location = 8, .format = VEC3, .offset = 0, .inputRate = true },

            // per-instance color
            { .location = 9, .format = VEC3, .offset = 0, .inputRate = true }
        }
    };

    PipelineHandle particlePipe = renderer.CreatePipeline({
        .layout = particleLayout,
        .blend = { .enabled = true },
        .depth = { .depth_test = true, .depth_write = false }
    });
s
    // Frame loop
    auto texture renderer.create_texture(path, texture_description);

    auto cmd = renderer.begin_frame(window, framebuffer);
        cmd.bind_pipeline(particlePipe);
        cmd.bind_vertex_buffer(0, quadVBO);
        cmd.bind_vertex_buffer(1, particlePosVBO);
        cmd.bind_vertex_buffer(2, particleColorVBO);
        cmd.draw_index_instanced(particle_mesh.index_count, PARTICLE_COUNT);
    renderer.end_frame(window);

    glfwSwapBuffers(engine.window);
    glfwPollEvents();
    */

    Renderer<OpenGL> renderer = {};
    Vector<Vertex> quad_vertices = Vector<Vertex>({
        //         Position                      Normal                 UV
        Vertex{Vec3( 0.5f,  0.5f, 0.0f),  Vec3(1.0f, 0.0f, 0.0f),  Vec2(1, 1)}, // top right
        Vertex{Vec3( 0.5f, -0.5f, 0.0f),  Vec3(0.0f, 1.0f, 0.0f),  Vec2(1, 0)}, // bottom right
        Vertex{Vec3(-0.5f, -0.5f, 0.0f),  Vec3(0.0f, 0.0f, 1.0f),  Vec2(0, 0)}, // bottom left
        Vertex{Vec3(-0.5f,  0.5f, 0.0f),  Vec3(1.0f, 1.0f, 0.0f),  Vec2(0, 1)}  // top left 
    }, arena_allocator); 

    Vector<u32> quad_indices = Vector<u32>({
        0, 1, 3,
        1, 2, 3
    }, arena_allocator);

    VertexLayout layout = VertexLayout({
        {0, OFFSET_OF(Vertex, aPosition), BufferStrideTypeInfo::VEC3},
        {1, OFFSET_OF(Vertex, aNormal), BufferStrideTypeInfo::VEC3},
        {2, OFFSET_OF(Vertex, aTexCoord), BufferStrideTypeInfo::VEC2},
    });
    RasterizerState rasterizer = RasterizerState();
    DepthState depth = DepthState();
    BlendState blend = BlendState();
    PipelineDescriptor pipeline_description = PipelineDescriptor(layout, rasterizer, depth, blend);
    auto pipeline = renderer.create_pipeline(pipeline_description);

    auto shader = renderer.create_shader({"../../Scenes/ParticleScene/Shaders/singularity.vert", "../../Scenes/ParticleScene/Shaders/singularity.frag"});

    auto vbo = renderer.create_vertex_buffer(pipeline, quad_vertices, false);
    auto ebo = renderer.create_index_buffer(pipeline, quad_indices);

    auto cmd = renderer.begin_frame();
        renderer.bind_pipeline(cmd, pipeline, shader);
        renderer.bind_vertex_buffer(cmd, vbo);
        renderer.bind_index_buffer(cmd, ebo);
        // renderer.draw_index_instanced(particle_mesh.index_count, PARTICLE_COUNT);
    // renderer.end_frame();

    return 0;
}