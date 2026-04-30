#include <sfe.hpp>

struct AppState {
	ShaderHandle cube_shader = ShaderHandle::invalid();
	MaterialHandle material = MaterialHandle::invalid();
	VertexBufferHandle instance_cube_vbo = VertexBufferHandle::invalid();
	MeshHandle cube_mesh = MeshHandle::invalid();

	ShaderHandle backpack_shader = ShaderHandle::invalid();
	MeshHandle backpack_mesh = MeshHandle::invalid();

	TextureHandle container_texture = TextureHandle::invalid();
	TextureHandle face_texture = TextureHandle::invalid();

	Pipeline opaque_pipeline = {};
	Pipeline opaque_wireframe_pipeline = {};
	bool use_opaque_pipeline = true;
	float accumulator = 0.0;

	Vector<Mat4> cube_translations;

	Timer timer = {};
};

extern "C" /*__declspec(dllexport)*/ void application_init(Engine* engine) {
	engine->application_state = engine->permenant_allocator.malloc(sizeof(AppState), alignof(AppState));
	AppState* app = (AppState*)engine->application_state;
	*app = {};

	app->opaque_pipeline = Pipeline{
		.rasterizer = {
			.fill = true
		},
		.depth = {
			.depth_testing = true,
			.depth_write = true
		},
		.blend = {
			.enabled = false
		}
	};

	app->opaque_wireframe_pipeline = Pipeline{
		.rasterizer = {
			.fill = false
		},
		.depth = {
			.depth_testing = true,
			.depth_write = true
		},
		.blend = {
			.enabled = false
		}
	};

	TextureDescription desc = {};
	app->container_texture = engine->renderer.create_texture(0, "../../../Game/Assets/Textures/container.jpg", desc);
	app->face_texture = engine->renderer.create_texture(1, "../../../Game/Assets/Textures/awesomeface.png", desc);

	app->cube_shader = engine->renderer.create_shader({"../../../Game/Assets/Shaders/cube.vert", "../../../Game/Assets/Shaders/cube.frag"});
	app->material = engine->renderer.create_material(app->cube_shader);
	app->cube_mesh = engine->renderer.create_mesh_cube(app->material);

	app->backpack_shader = engine->renderer.create_shader({"../../../Game/Assets/Shaders/model.vert", "../../../Game/Assets/Shaders/model.frag"});
	app->backpack_mesh = engine->renderer.create_mesh(app->backpack_shader, "../../../Game/Assets/Models/Backpack/backpack.obj");

	app->cube_translations = Vector<Mat4>(engine->permenant_allocator);
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2) {
		for (int x = -10; x < 10; x += 2) {
			Vec3 translation;
			translation.x = (float)x * 2.0f + offset;
			translation.y = (float)y * 2.0f + offset;
			translation.z = sin(x + y) * 10;
			app->cube_translations.append(Mat4::translate(Mat4::identity(), translation).transpose());
		}
	}

	VertexLayout layout = VertexLayout({
		VertexAttribute{4, 0, BufferStrideTypeInfo::MAT4, true},
	});

	app->instance_cube_vbo = engine->renderer.create_vertex_buffer(app->cube_mesh, layout, app->cube_translations, true);

	app->timer.start(5.0f);
}

extern "C" /*__declspec(dllexport)*/ void application_update(Engine* engine, float dt) {
	AppState* app = (AppState*)engine->application_state;

	app->accumulator += dt * 100;

	// active_scene.update(dt);
	if (engine->input.get_key_pressed(KEY_ESCAPE)) {
		glfwSetWindowShouldClose(engine->window, true);
	}

	if (engine->input.get_key_pressed(KEY_C)) {
		engine->mouse_captured = !engine->mouse_captured;
		glfwSetInputMode(engine->window, GLFW_CURSOR, engine->mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	if (engine->input.get_key_pressed(KEY_R)) {
		engine->renderer.shader_recompile(app->cube_shader);
	}

	if (engine->input.get_key(KEY_SPACE, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::UP, dt);
	}

	if (engine->input.get_key(KEY_CTRL, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::DOWN, dt);
	}

	if (engine->input.get_key(KEY_W, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::FORWARD, dt); 
	}

	if (engine->input.get_key(KEY_A, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::LEFT, dt); 
	}

	if (engine->input.get_key(KEY_S, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::BACKWARD, dt);
	}

	if (engine->input.get_key(KEY_D, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::RIGHT, dt); 
	}
}

extern "C" /*__declspec(dllexport)*/ void application_render(Engine* engine, float dt) {
	AppState* app = (AppState*)engine->application_state;

	if(app->timer.tick(dt)) {
		app->use_opaque_pipeline = !app->use_opaque_pipeline;
		app->timer.reset();
	}

	Mat4 model = Mat4::rotate(Mat4::identity(), Quat::from_euler(app->accumulator, app->accumulator, 0));
	Mat4 view = engine->get_view_matrix();
	Mat4 projection = engine->get_projection_matrix();
	engine->renderer.material_set_uniforms(app->material, {{
		{"uContainer", app->container_texture},
		{"uFace", app->face_texture},
	}, engine->frame_allocator});

	Pipeline pipeline = app->use_opaque_pipeline ? app->opaque_pipeline : app->opaque_wireframe_pipeline;
	// engine->renderer.bind_vertex_buffer(app->cube_mesh, app->instance_cube_vbo);
	// engine->renderer.draw_mesh(pipeline, app->cube_mesh, model, view, projection, app->cube_translations.count);

	model = Mat4::translate(model, -5, 0, 0);
	pipeline = !app->use_opaque_pipeline ? app->opaque_pipeline : app->opaque_wireframe_pipeline;
	// engine->renderer.draw_mesh(pipeline, app->backpack_mesh, model, view, projection);

	/*
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
	*/
}

// https://www.youtube.com/watch?v=9R2rRLbBkHU
// https://www.youtube.com/watch?v=QAeRxfeFAo0

/*

- [] remove common and put everything in the renderer
	the renderer is agnostic so no types in there from opengl only handles
- [] remove all constructors except for containers
- [] remove hte type fromm handles, add TextureHandle and all that jazz as seperate structs.
- [] remove gl_error_check in favor of the callback

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