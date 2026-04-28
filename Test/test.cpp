#include <sfe.hpp>
#include <GLFW/glfw3.h>

Camera camera = Camera(0, 1, 10);
bool mouse_captured = true;

void mouse(GLFWwindow* window, double mouse_x, double mouse_y) {
	static bool first = true;
	static float last_mouse_x;
	static float last_mouse_y;

	if (first) {
		last_mouse_x = mouse_x;
		last_mouse_y = mouse_y;
		first = false;
		return;
	}

	float xoffset = mouse_x - last_mouse_x;
	float yoffset = last_mouse_y - mouse_y;

	last_mouse_x = mouse_x;
	last_mouse_y = mouse_y;

	if (mouse_captured) {
		camera.process_mouse_movement(xoffset, yoffset);
	}
}

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
	glfwSetInputMode(window, GLFW_CURSOR, mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

	gl_error_check(glEnable(GL_MULTISAMPLE));
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT); 
	// glEnable(GL_FRAMEBUFFER_SRGB);

	return window;
}

struct Engine {
	GLFWwindow* window;
	Input input;
	Renderer renderer;
	
	bool init(Allocator permenant_allocator, Allocator frame_allocator) {
		this->window = GLFW_INIT();
		if (!this->window) {
			return false;
		}

		this->input.init(permenant_allocator);
		if (!INPUT_GLFW_SETUP(&this->input, this->window, nullptr, nullptr, mouse)) {
			return false;
		}

		this->renderer = Renderer::create(permenant_allocator, frame_allocator);
		return true;
	}

	void update(float dt) {
		// active_scene.update(dt);

		if (input.get_key_pressed(KEY_R)) {
			// this->asset_manager.reload_shaders();
		}

		if (input.get_key_pressed(KEY_ESCAPE)) {
			glfwSetWindowShouldClose(this->window, true);
		}

		if (input.get_key_pressed(KEY_C)) {
			mouse_captured = !mouse_captured;
			glfwSetInputMode(this->window, GLFW_CURSOR, mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
		}

		if (input.get_key(KEY_SPACE, PRESSED|DOWN)) {
			camera.process_keyboard(CameraDirection::UP, dt);
		}

		if (input.get_key(KEY_CTRL, PRESSED|DOWN)) {
			camera.process_keyboard(CameraDirection::DOWN, dt);
		}

		if (input.get_key(KEY_W, PRESSED|DOWN)) {
			camera.process_keyboard(CameraDirection::FORWARD, dt); 
		}

		if (input.get_key(KEY_A, PRESSED|DOWN)) {
			camera.process_keyboard(CameraDirection::LEFT, dt); 
		}

		if (input.get_key(KEY_S, PRESSED|DOWN)) {
			camera.process_keyboard(CameraDirection::BACKWARD, dt);
		}

		if (input.get_key(KEY_D, PRESSED|DOWN)) {
			camera.process_keyboard(CameraDirection::RIGHT, dt); 
		}

		input.poll();
	}
};

int main() {
	constexpr int PROGRAM_MEMORY_CAPACITY = KB(250);
	u8 program_memory[PROGRAM_MEMORY_CAPACITY];
	Arena program_arena = Arena::fixed(program_memory, PROGRAM_MEMORY_CAPACITY);
	Allocator program_arena_allocator = program_arena.to_allocator();

	Temp temp = Temp::begin(&program_arena); 
		test_basic_put_get(program_arena_allocator);
		test_overwrite(program_arena_allocator);
		test_remove(program_arena_allocator);
		test_collisions(program_arena_allocator);
		test_rehashing(program_arena_allocator);
		test_string_keys(program_arena_allocator);
		test_dead_entries(program_arena_allocator);
	temp.end();

	temp = Temp::begin(&program_arena);
		test_stress(program_arena_allocator);
		test_put_get_large_values(program_arena_allocator);
		test_remove_nonexistent(program_arena_allocator);
		test_has_nonexistent(program_arena_allocator);
		test_string_view_keys(program_arena_allocator);
		test_cstring_keys(program_arena_allocator);
		test_custom_struct_keys(program_arena_allocator);
		test_edge_cases(program_arena_allocator);
		test_clear(program_arena_allocator);
	temp.end();

	constexpr int PERMANENT_MEMORY_CAPACITY = KB(50);
	u8* permenant_memory = (u8*)program_arena.push(PERMANENT_MEMORY_CAPACITY, alignof(u8));
	Arena permanent_arena = Arena::fixed(permenant_memory, PERMANENT_MEMORY_CAPACITY);
	Allocator permanent_arena_allocator = permanent_arena.to_allocator();

	constexpr int FRAME_MEMORY_CAPACITY = KB(150);
	u8* frame_memory = (u8*)program_arena.push(FRAME_MEMORY_CAPACITY, alignof(u8));
	Arena frame_arena = Arena::fixed(frame_memory, FRAME_MEMORY_CAPACITY);
	Allocator frame_arena_allocator = frame_arena.to_allocator();

	LOG_INFO("All tests passed\n");

	Engine engine = {};
	if (!engine.init(permanent_arena_allocator, frame_arena_allocator)) {
		return -1;
	}

	Vector<Vertex> cube_vertices = {
		// Back face (z = -0.5)
		Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(0,0,-1), Vec2(0,0)}, // 0
		Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(0,0,-1), Vec2(1,1)}, // 1
		Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(0,0,-1), Vec2(1,0)}, // 2
		Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(0,0,-1), Vec2(0,1)}, // 3

		// Front face (z = +0.5)
		Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(0,0,1), Vec2(0,0)}, // 4
		Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(0,0,1), Vec2(1,0)}, // 5
		Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(0,0,1), Vec2(1,1)}, // 6
		Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(0,0,1), Vec2(0,1)}, // 7

		// Left face (x = -0.5)
		Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(-1,0,0), Vec2(0,1)}, // 8
		Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(-1,0,0), Vec2(1,1)}, // 9
		Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(-1,0,0), Vec2(1,0)}, // 10
		Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(-1,0,0), Vec2(0,0)}, // 11

		// Right face (x = +0.5)
		Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(1,0,0), Vec2(0,1)}, // 12
		Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(1,0,0), Vec2(1,0)}, // 13
		Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(1,0,0), Vec2(1,1)}, // 14
		Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(1,0,0), Vec2(0,0)}, // 15

		// Bottom face (y = -0.5)
		Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(0,-1,0), Vec2(0,1)}, // 16
		Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(0,-1,0), Vec2(1,1)}, // 17
		Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(0,-1,0), Vec2(1,0)}, // 18
		Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(0,-1,0), Vec2(0,0)}, // 19

		// Top face (y = +0.5)
		Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(0,1,0), Vec2(0,1)}, // 20
		Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(0,1,0), Vec2(1,0)}, // 21
		Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(0,1,0), Vec2(1,1)}, // 22
		Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(0,1,0), Vec2(0,0)}, // 23
	};

	Vector<u32> cube_indices = {
		0,  1,  2,  1,  0,  3,  // Front
		4,  5,  6,  6,  7,  4,  // Back

		8,  9,  10, 10, 11, 8,  // Left
		12, 13, 14, 13, 12, 15, // Right

		16, 17, 18, 18, 19, 16, // Bottom
		20, 21, 22, 21, 20, 23  // Top
	};

	// TODO(Jovanni): I want some way to convey pipeline is similar in structure to another pipeline
	// maybe I will say PNT_opaque_pipeline

	ShaderHandle model_shader = engine.renderer.create_shader({"../../Assets/Shaders/model.vert", "../../Assets/Shaders/model.frag"});
	ShaderHandle cube_shader = engine.renderer.create_shader({"../../Assets/Shaders/cube.vert", "../../Assets/Shaders/cube.frag"});
	ShaderHandle quad_shader = engine.renderer.create_shader({"../../Assets/Shaders/screen_quad.vert", "../../Assets/Shaders/screen_quad.frag"});

	TextureDescription texture_desc = {};
	TextureHandle container_texture = engine.renderer.create_texture(0, "../../Assets/Textures/container.jpg", texture_desc);
	TextureHandle face_texture = engine.renderer.create_texture(1, "../../Assets/Textures/awesomeface.png", texture_desc);

	MaterialHandle cube_material = engine.renderer.create_material(cube_shader);
	engine.renderer.material_set_uniforms(cube_material, {
		{"uFace", face_texture},
		{"uContainer", container_texture},
	});

	MeshHandle backpack_mesh_handle = engine.renderer.create_mesh(model_shader, "../../Assets/Models/backpack/backpack.obj");
	Vector<Mat4> translations = {};
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2) {
		for (int x = -10; x < 10; x += 2) {
			Vec3 translation;
			translation.x = (float)x * 2.0f + offset;
			translation.y = (float)y * 2.0f + offset;
			translation.z = sin(x + y) * 10;
			translations.append(Mat4::translate(Mat4::identity(), translation).transpose());
		}
	}
	VertexBufferHandle offset_vbo = engine.renderer.create_vertex_buffer(
		backpack_mesh_handle, 
		VertexLayout({
			{4, 0, BufferStrideTypeInfo::MAT4, true} // this is a little confusing maybe figure out a better way...
		}),
		translations, 
		true
	);

	MeshHandle cube_mesh_handle = engine.renderer.create_mesh(cube_material, cube_vertices, cube_indices);

	bool pipeline_switch = true;
	Timer timer = Timer::create();
	timer.start(5.0f);

	float dt = 0.0f;
	float previous_time = glfwGetTime();
	float accumulator = 0.0f;
	while (!glfwWindowShouldClose(engine.window)) {
		Temp frame_temp = Temp::begin(&frame_arena);

		float current_time = glfwGetTime();
		dt = current_time - previous_time;
		previous_time = current_time;
		accumulator += dt * 10;

		engine.update(dt);

		Mat4 model         =  Mat4::identity();
		Mat4 view          =  camera.get_view_matrix();
		Mat4 projection    =  Mat4::perspective(camera.zoom, (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		Quat rotation = Quat::from_euler(Vec3(accumulator, accumulator, 0.0f));
		model = Mat4::scale(model, Vec3((sin(accumulator / 10) + 2), 2, 2));
		model = Mat4::rotate(model, rotation);

		if (timer.tick(dt)) {
			LOG_DEBUG("TICK\n");
			pipeline_switch = !pipeline_switch;
			timer.reset();
		}

		{
			Mesh cube_mesh = engine.renderer.backend.meshes.get(cube_mesh_handle);
			engine.renderer.update_vertex_buffer(cube_mesh.vao, offset_vbo, translations);
			for (MeshEntry& entry : cube_mesh.entries) {
				RenderCommand command = {};
				command.pipeline = pipeline_switch ? engine.renderer.opaque_pipeline : engine.renderer.opaque_wireframe_pipeline;
				command.material = entry.material_handle;
				command.vao = cube_mesh.vao;
				command.main_vbo = cube_mesh.vbo;
				command.extra_vbos = Vector<VertexBufferHandle>({offset_vbo}, frame_arena_allocator);
				command.ebo = cube_mesh.ebo;
				command.mesh_entry = entry;
				command.model = model;
				command.view = view;
				command.projection = projection;
				command.instance_count = 1;

				engine.renderer.submit(command);
			}
		}

		
		model = Mat4::identity();
		Mesh backpack_mesh = engine.renderer.backend.meshes.get(backpack_mesh_handle);
		engine.renderer.update_vertex_buffer(backpack_mesh.vao, offset_vbo, translations);
		for (MeshEntry& entry : backpack_mesh.entries) {
			RenderCommand command = {};
			command.pipeline = pipeline_switch ? engine.renderer.opaque_pipeline : engine.renderer.opaque_wireframe_pipeline;
			command.material = entry.material_handle;
			command.vao = backpack_mesh.vao;
			command.main_vbo = backpack_mesh.vbo;
			command.extra_vbos = Vector<VertexBufferHandle>({offset_vbo}, frame_arena_allocator);
			command.ebo = backpack_mesh.ebo;
			command.mesh_entry = entry;
			command.model = model;
			command.view = view;
			command.projection = projection;
			command.instance_count = translations.count;

			engine.renderer.submit(command);
		}

		engine.renderer.draw(frame_arena_allocator);

		glfwSwapBuffers(engine.window);
		glfwPollEvents();

		frame_temp.end();
	}

	return 0;
}

// abstract the render_queue into the renderer.
// see how hard it is to do instanced stuff

// probably take a look at the ::create(...) stuff see if you can get away with not doing that?
// Entity stuff
// MeshHandle
// renderer.draw_mesh()

// ok it kind of defeats the purpose when I have to use the rendere.backend.registry
// figure out how to fix this...

// this will turn into a dll
// the idea is just to submit commands

// 3d grid of lines