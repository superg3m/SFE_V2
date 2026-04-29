#include <sfe.hpp>

struct AppState {
	MeshHandle backpack_mesh_handle = MeshHandle::invalid();
	Camera camera = Camera(0, 1, 10);
	bool mouse_captured = true;
	bool use_opaque_pipeline = true;
};

AppState app;

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

	if (app.mouse_captured) {
		app.camera.process_mouse_movement(xoffset, yoffset);
	}
}

extern "C" __declspec(dllexport) void application_init(Engine* engine) {
	engine->application_state = engine->permenant_allocator.malloc(sizeof(AppState), alignof(AppState));
	Memory::copy(engine->application_state, sizeof(AppState), &app, sizeof(AppState));

	glfwMakeContextCurrent(engine->window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		LOG_ERROR("Failed to initialize GLAD\n");
		glfwTerminate();
		exit(-1);
	}

	engine->input.init(engine->permenant_allocator);
	if (!INPUT_GLFW_SETUP(&engine->input, engine->window, nullptr, nullptr, mouse)) {
		return; // return false;
	}

	ShaderHandle model_shader = engine->renderer.create_shader({"../../../Game/Assets/Shaders/model.vert", "../../../Game/Assets/Shaders/model.frag"});
	app.backpack_mesh_handle = engine->renderer.create_mesh(model_shader, "../../../Game/Assets/Models/backpack/backpack.obj");
}

extern "C" __declspec(dllexport) void application_update(Engine* engine, float dt) {
	if (engine->reloaded_dll) {
		Memory::copy(&app, sizeof(AppState), engine->application_state, sizeof(AppState));
	}

	// active_scene.update(dt);
	if (engine->input.get_key_pressed(KEY_ESCAPE)) {
		glfwSetWindowShouldClose(engine->window, true);
	}

	if (engine->input.get_key_pressed(KEY_C)) {
		app.mouse_captured = !app.mouse_captured;
		glfwSetInputMode(engine->window, GLFW_CURSOR, app.mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	if (engine->input.get_key(KEY_SPACE, PRESSED|DOWN)) {
		app.camera.process_keyboard(CameraDirection::UP, dt);
	}

	if (engine->input.get_key(KEY_CTRL, PRESSED|DOWN)) {
		app.camera.process_keyboard(CameraDirection::DOWN, dt);
	}

	if (engine->input.get_key(KEY_W, PRESSED|DOWN)) {
		app.camera.process_keyboard(CameraDirection::FORWARD, dt); 
	}

	if (engine->input.get_key(KEY_A, PRESSED|DOWN)) {
		app.camera.process_keyboard(CameraDirection::LEFT, dt); 
	}

	if (engine->input.get_key(KEY_S, PRESSED|DOWN)) {
		app.camera.process_keyboard(CameraDirection::BACKWARD, dt);
	}

	if (engine->input.get_key(KEY_D, PRESSED|DOWN)) {
		app.camera.process_keyboard(CameraDirection::RIGHT, dt); 
	}
}

extern "C" __declspec(dllexport) void application_render(Engine* engine, float dt) {
	Mesh backpack_mesh = engine->renderer.backend.meshes.get(app.backpack_mesh_handle);
	for (MeshEntry& entry : backpack_mesh.entries) {
		RenderCommand command = {};
		command.pipeline = app.use_opaque_pipeline ? engine->renderer.opaque_pipeline : engine->renderer.opaque_wireframe_pipeline;
		command.material = entry.material_handle;
		command.vao = backpack_mesh.vao;
		command.main_vbo = backpack_mesh.vbo;
		command.extra_vbos = Vector<VertexBufferHandle>({}, engine->frame_allocator);
		command.ebo = backpack_mesh.ebo;
		command.mesh_entry = entry;
		command.model = Mat4::identity();
		command.view = app.camera.get_view_matrix();
		command.projection = Mat4::perspective(app.camera.zoom, (float)Renderer::WINDOW_WIDTH / (float)Renderer::WINDOW_HEIGHT, 0.1f, 1000.0f);
		command.instance_count = 1;

		// engine->renderer.submit(command);
	}

	Memory::copy(engine->application_state, sizeof(AppState), &app, sizeof(AppState));
}

// https://www.youtube.com/watch?v=9R2rRLbBkHU
// https://www.youtube.com/watch?v=QAeRxfeFAo0

/*

- [] remove gl_error_check in favor of the callback

static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                         GLsizei length, const GLchar* message, const void* user)
{
  if(severity == GL_DEBUG_SEVERITY_LOW || 
     severity == GL_DEBUG_SEVERITY_MEDIUM ||
     severity == GL_DEBUG_SEVERITY_HIGH)
  {
    SM_ASSERT(false, "OpenGL Error: %s", message);
  }
  else
  {
    SM_TRACE((char*)message);
  }
}

glDebugMessageCallback(&gl_debug_callback, nullptr);

// so I think that a lot of my problems stem from having to strictly type handles so maybe we can work on that to be easier
// by removing the need for a handle to be templated, so its now just Handle

enum RequestType {
	TextureLoad
};

struct TextureRequest {
	const char* path;
};

struct DrawMeshRequest {
	PipelineHandle pipeline;
	MeshHandle mesh; // mesh_entries, vao, vbo, ebo
	Mat4 Model;
	int instance_count;
};

struct DrawMeshEntryRequest {
	PipelineHandle pipeline;
	MeshEntryHandle mesh_entry; // material, vao, vbo, ebo
	Mat4 Model;
};

struct Request {
	RequestType type;
	union {
		TextureRequest texture;
	};

	Handle handle;
};

Handle shader = 
renderer.create_mesh(shader);

struct TextureHandle {
	Handle handle;
};

template<typename B>
Renderer {
	B backend;

	TextureHandle create_textures(const char* path) {
		Handle handle = backend.textures.acquire();
		Request(TEXTURE_LOAD, handle);
	
		return MeshHandle(handle);
	}

	// after you do this it will go into the backend on the exe code not dll so no context issues.
	// from there you could be able to call into the backend and do whatever weird thing you have to do.
	grant_requets();

	Vector<Request> requests;
}

struct OpenGL {
	GLuint vao;
	GLuint shader_program;
	// depth all that jazz

	struct Texture;

	void handle_requst(Request& request) {
		switch (request.type) {
			case TEXTURE_LOAD: {
				Texture& texture = backend.textures.get(request.handle);
				Texture::create(request.texture.path)
			} break;
		}
	}

	Registry<OpenGL::Texture, 256> textures;
};

OpenGL

// RenderInterface.hpp
// all the data you need for a draw call
struct RenderPacket {
	Material material;
	Mat4 Model;
}

struct RenderData {
	Camera camera;

	int fontHeight;
	Glyph glyphs[127];

	Registry<Material, 256> materials;
	Registry<Transform, 1000> transforms;
	Registry<Transform, 1000> uiTransforms;
};

RenderData* render_data;


struct MaterialHandle {
	Handle handle; // hande to a mterial
};

*/