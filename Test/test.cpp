#include <sfe.hpp>
#include <GLFW/glfw3.h>

using B = OpenGL;
using Texture = typename B::Texture;
using TextureHandle = Handle<Texture>;

using VertexBuffer = typename B::VertexBuffer;
using VertexBufferHandle = Handle<VertexBuffer>;

using IndexBuffer = typename B::IndexBuffer;
using IndexBufferHandle = Handle<IndexBuffer>;

using CommandBuffer = typename B::CommandBuffer;
using CommandBufferHandle = Handle<CommandBuffer>;

using Pipeline = typename B::Pipeline;
using PipelineHandle = Handle<Pipeline>;

using Shader = typename B::Shader;
using ShaderHandle = Handle<Shader>;

using Material = typename B::Material;
using MaterialHandle = Handle<Material>;

using MeshEntry = typename B::MeshEntry;
using MeshEntryHandle = Handle<MeshEntry>;


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

const int WIDTH = 800;
const int HEIGHT = 800;

GLFWwindow* GLFW_INIT() {
    RUNTIME_ASSERT_MSG(glfwInit(), "Failed to init glfw\n");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        LOG_ERROR("Failed to create GLFW window\n");
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD\n");
        glfwTerminate();
        exit(-1);
    }

    glfwSwapInterval(1);
    // glfwSetInputMode(window, GLFW_CURSOR, mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

    gl_error_check(glEnable(GL_MULTISAMPLE));
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT); 
    // glEnable(GL_FRAMEBUFFER_SRGB);

    return window;
}

struct Engine {
    GLFWwindow* window;
    Input input;

    bool init(Allocator allocator) {
        this->window = GLFW_INIT();
        if (!this->window) {
            return false;
        }

        this->input.init(allocator);
        if (!INPUT_GLFW_SETUP(&this->input, this->window)) {
            return false;
        }

        return true;
    }

    void update(float dt) {
        // active_scene.update(dt);

        if (input.get_key(KEY_A, PRESSED)) {
            LOG_ERROR("WOW\n");
        }

        if (input.get_key_pressed(KEY_R)) {
            // this->asset_manager.reload_shaders();
        }

        if (input.get_key_pressed(KEY_ESCAPE)) {
            glfwSetWindowShouldClose(this->window, true);
        }

        input.poll();
    }
};

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

    Engine engine = {};
    if (!engine.init(arena_allocator)) {
        return -1;
    }

    Renderer<B> renderer = {};
    Vector<Vertex> cube_vertices = {
        // Back face (z = -0.5)
        Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(0,0,-1), Vec2(0,0)},
        Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(0,0,-1), Vec2(1,1)},
        Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(0,0,-1), Vec2(1,0)},

        Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(0,0,-1), Vec2(1,1)},
        Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(0,0,-1), Vec2(0,0)},
        Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(0,0,-1), Vec2(0,1)},

        // Front face (z = +0.5)
        Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(0,0,1), Vec2(0,0)},
        Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(0,0,1), Vec2(1,0)},
        Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(0,0,1), Vec2(1,1)},

        Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(0,0,1), Vec2(1,1)},
        Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(0,0,1), Vec2(0,1)},
        Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(0,0,1), Vec2(0,0)},

        // Left face (x = -0.5)
        Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(-1,0,0), Vec2(1,0)},
        Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(-1,0,0), Vec2(1,1)},
        Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(-1,0,0), Vec2(0,1)},

        Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(-1,0,0), Vec2(0,1)},
        Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(-1,0,0), Vec2(0,0)},
        Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(-1,0,0), Vec2(1,0)},

        // Right face (x = +0.5)
        Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(1,0,0), Vec2(1,0)},
        Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(1,0,0), Vec2(0,1)},
        Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(1,0,0), Vec2(1,1)},

        Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(1,0,0), Vec2(0,1)},
        Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(1,0,0), Vec2(1,0)},
        Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(1,0,0), Vec2(0,0)},

        // Bottom face (y = -0.5)
        Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(0,-1,0), Vec2(0,1)},
        Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(0,-1,0), Vec2(1,1)},
        Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(0,-1,0), Vec2(1,0)},

        Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(0,-1,0), Vec2(1,0)},
        Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(0,-1,0), Vec2(0,0)},
        Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(0,-1,0), Vec2(0,1)},

        // Top face (y = +0.5)
        Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(0,1,0), Vec2(0,1)},
        Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(0,1,0), Vec2(1,0)},
        Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(0,1,0), Vec2(1,1)},

        Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(0,1,0), Vec2(1,0)},
        Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(0,1,0), Vec2(0,1)},
        Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(0,1,0), Vec2(0,0)},
    };

    Vector<u32> cube_indices = {
        0,  2,  1,  2,  0,  3,  // Front
        4,  6,  5,  6,  4,  7,  // Back
        8,  10, 9,  10, 8,  11, // Left
        12, 14, 13, 14, 12, 15, // Right
        16, 18, 17, 18, 16, 19, // Bottom
        20, 22, 21, 22, 20, 23  // Top
    };

    PipelineDescriptor pipeline_description = PipelineDescriptor(
        VertexLayout({
            {0, OFFSET_OF(Vertex, aPosition), BufferStrideTypeInfo::VEC3, false},
            {1, OFFSET_OF(Vertex, aNormal),   BufferStrideTypeInfo::VEC3, false},
            {2, OFFSET_OF(Vertex, aTexCoord), BufferStrideTypeInfo::VEC2, false},
        }),
        RasterizerState{
            .cull_enabled = true,
	        .cull_face_back = true,
            .ccw_winding = true,
	        .fill = true
        }, 
        DepthState{

        }, 
        BlendState{
            .enabled = false
        }
    );

    /*
    PipelineDescriptor pipeline_particle_description = PipelineDescriptor(
        VertexLayout({
            {4, pipeline_description.layout.stride, BufferStrideTypeInfo::VEC3, true},
            {5, pipeline_description.layout.stride + sizeof(Vec3),   BufferStrideTypeInfo::VEC3, true},
        }),
        RasterizerState(), 
        DepthState(), 
        BlendState()
    );
    */;

    ShaderHandle shader = renderer.create_shader({"../../Assets/Shaders/cube.vert", "../../Assets/Shaders/cube.frag"});
    PipelineHandle pipeline = renderer.create_pipeline(pipeline_description);
    VertexBufferHandle vbo = renderer.create_vertex_buffer(pipeline, cube_vertices);
    // IndexBufferHandle ebo = renderer.create_index_buffer(pipeline, cube_indices);
    MaterialHandle material = renderer.create_material(shader);

    TextureDescription texture_desc = {};
    TextureHandle container_texture = renderer.create_texture(0, "../../Assets/Textures/container.jpg", texture_desc);
    TextureHandle face_texture = renderer.create_texture(1, "../../Assets/Textures/awesomeface.png", texture_desc);

    MeshEntryHandle mesh_entry = renderer.create_mesh_entry(pipeline, cube_vertices, {}, 0, 0, material);

    float dt = 0.0f;
    float previous_time = glfwGetTime();
    float accumulator = 0.0f;
    while (!glfwWindowShouldClose(engine.window)) {
        Temp frame_temp = Temp::begin(&arena);

        float current_time = glfwGetTime();
        dt = current_time - previous_time;
        previous_time = current_time;
        accumulator += dt * 50;

        engine.update(dt);

        Mat4 model         = Mat4(1.0f);
        Mat4 view          = Mat4(1.0f);
        Mat4 projection    = Mat4(1.0f);
        Quat rotation = Quat::from_euler(Vec3(accumulator, accumulator, 0.0f));
        model = Mat4::scale(model, Vec3((sin((float)glfwGetTime()) + 2), 1, 1));
        model = Mat4::rotate(model, rotation);
        view  = Mat4::translate(view, Vec3(0.0f, 0.0f, -5.0f));
        projection = Mat4::perspective(45.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        
        renderer.material_set_mat4(material, Hashmap<const char*, Mat4>({
            {"uModel", model},
            {"uView", view},
            {"uProjection", projection},
        }, arena_allocator)); // make this more explicit tbh that you are using hte frame allocator

        renderer.material_set_texture(material, Hashmap<const char*, TextureHandle>({
            {"uContainer", container_texture},
            {"uFace", face_texture},
        }, arena_allocator)); // make this more explicit tbh that you are using hte frame allocator

        auto cmd = renderer.begin_frame();
            renderer.bind_pipeline(cmd, pipeline, shader);
            renderer.bind_material(material);
            renderer.bind_vertex_buffer(cmd, vbo);
            // renderer.bind_index_buffer(cmd, ebo);
            renderer.draw_mesh_entry(mesh_entry);
        renderer.end_frame(cmd);

        glfwSwapBuffers(engine.window);
        glfwPollEvents();

        frame_temp.end();
    }

    return 0;
}

// probably take a look at the ::create(...) stuff see if you can get away with not doing that?
// Entity stuff
// MeshHandle
// renderer.draw_mesh()