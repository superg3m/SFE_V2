#include "Core/core.hpp"
#include "API/api.hpp"
#include "Editor/editor.hpp"
#include "Runtime/runtime.hpp"

Engine* engine = nullptr;
extern Hashmap<String, String>* string_intern_map;

constexpr int PROGRAM_MEMORY_CAPACITY = MB(50);
constexpr int PERMANENT_MEMORY_CAPACITY = MB(10);
constexpr int FRAME_MEMORY_CAPACITY = MB(10);

INTERNAL_LINKAGE Platform::DLL* dll = nullptr;
INTERNAL_LINKAGE const char* dll_name = "game.dll";
INTERNAL_LINKAGE const char* temp_dll_name = "application_temp.dll";
INTERNAL_LINKAGE Platform::FileTime last_write_time = {};

typedef void(ApplicationInitalizeFunc)(Engine* engine, Hashmap<String, String>* string_intern_map);
typedef void(ApplicationUpdateFunc)(Engine* engine, Hashmap<String, String>* string_intern_map, float dt);
typedef void(ApplicationRenderFunc)(Engine* engine, Hashmap<String, String>* string_intern_map, float dt);

/*
Mat4 Engine::get_view_matrix() {
	return this->camera.get_view_matrix();
}

Mat4 Engine::get_projection_matrix() {
	float aspect = (float)this->renderer.WINDOW_WIDTH / (float)this->renderer.WINDOW_HEIGHT;
	return Mat4::perspective(this->camera.zoom, aspect, 0.1f, 1000.0f);
}
*/
void load_application_function_pointers(ApplicationInitalizeFunc** application_init, ApplicationUpdateFunc** application_update, ApplicationRenderFunc** application_render) {
	if (dll) {
		Platform::free_dll(dll);
		*application_update = nullptr;
		*application_render = nullptr;
		dll    = nullptr;
	}

	Platform::copy_file(dll_name, temp_dll_name, true);
	last_write_time = Platform::get_file_modification_time(dll_name);

	Error err = Error::SUCCESS;
	dll = Platform::load_dll(temp_dll_name, err);
	RUNTIME_ASSERT_MSG(err == Error::SUCCESS, "Failed to load dll | %s\n", error_get_string(err));

	err = Error::SUCCESS;
	if (application_init) {
		*application_init = (ApplicationInitalizeFunc*)Platform::get_function_address(dll, "application_init");
	}

	*application_update = (ApplicationUpdateFunc*)Platform::get_function_address(dll, "application_update");
	*application_render = (ApplicationRenderFunc*)Platform::get_function_address(dll, "application_render");

	RUNTIME_ASSERT(*application_update && *application_render);
}

int main() {
	Allocator platform_allocator = Platform::get_allocator();

	void* program_memory = platform_allocator.malloc(PROGRAM_MEMORY_CAPACITY, alignof(u8));
	Arena program_arena = Arena::fixed(program_memory, PROGRAM_MEMORY_CAPACITY);
	Allocator program_arena_allocator = program_arena.to_allocator();

	MemoryContext memory = {};
	memory.permanent_arena = Arena::fixed(program_arena.push(PERMANENT_MEMORY_CAPACITY, alignof(u8)), PERMANENT_MEMORY_CAPACITY);
	memory.permanent_allocator = memory.permanent_arena.to_allocator();
	memory.frame_arena = Arena::fixed(program_arena.push(FRAME_MEMORY_CAPACITY, alignof(u8)), FRAME_MEMORY_CAPACITY);
	memory.frame_allocator = memory.frame_arena.to_allocator();

	Platform::init(); // I hate this inconsistency, maybe call it MemorySystem, PlatformSystem, InputSystem, WindowCreationSystem
	{
		engine = (Engine*)memory.permanent_allocator.malloc(sizeof(Engine), alignof(Engine));
		*engine = {};

		string_intern_map = (Hashmap<String, String>*)memory.permanent_allocator.malloc(sizeof(Hashmap<String, String>), alignof(Hashmap<String, String>));
		*string_intern_map = Hashmap<String, String>(memory.permanent_allocator);
	}

	InputSystem input = {};
	Renderer<OpenGL> renderer = {};

	engine->memory = memory;
	engine->window = Window::create(800, 600, "HelloWorld");
	engine->renderer = renderer.API(memory);
	engine->camera = Camera::create(0, 0, 10);

	input.init(engine->window.ctx);

	INTERNAL_LINKAGE ApplicationInitalizeFunc* application_init = nullptr;
	INTERNAL_LINKAGE ApplicationUpdateFunc*    application_update = nullptr;
	INTERNAL_LINKAGE ApplicationRenderFunc*    application_render = nullptr;

	Editor editor = {};
	editor.init(engine);
	load_application_function_pointers(&application_init, &application_update, &application_render);
	application_init(engine, string_intern_map);

	float dt = 0.0f;
	float previous_time = Platform::get_seconds_elapsed();
	while (!engine->window.should_close()) {
		float current_time = Platform::get_seconds_elapsed();
		dt = current_time - previous_time;
		previous_time = current_time;
			
		input.poll();
		engine->input = input.input_state;

		Platform::FileTime new_time = Platform::get_file_modification_time(dll_name);
		if (Platform::compare_file_modification_time(new_time, last_write_time) == false) {
			load_application_function_pointers(nullptr, &application_update, &application_render);
			engine->reloaded_dll = true;
		}

		if (application_update) application_update(engine, string_intern_map, dt);
		if (application_render) application_render(engine, string_intern_map, dt);
		engine->renderer.execute_request();
		editor.render(engine, &renderer);
		
		engine->reloaded_dll = false;
		engine->window.pump_messages();
	}
	
	platform_allocator.free(program_memory);

	return 0;
}

/*
struct ECS_API {
    Entity (*create_entity)();
    void (*destroy_entity)(Entity);
    Transform* (*get_transform)(Entity);
};

struct RenderAPI {
    void (*submit_mesh)(MeshHandle, MaterialHandle, Mat4);
};

struct InputAPI {
    bool (*key_down)(KeyCode);
};

struct AssetAPI {
    TextureHandle (*load_texture)(const char*);
};

void application_update(
    EntityComponentSystemAPI* ecs,
    RenderAPI* render,
    InputAPI* input,
    AssetAPI* assets,
    float dt
);

Engine
├─ runtime
│  ├─ core
│  │  ├─ application
│  │  │  ├─ entry_point
│  │  │  ├─ main_loop
│  │  │  └─ lifecycle (init/update/render/shutdown)
│  │  ├─ memory
│  │  │  ├─ allocators
│  │  │  │  ├─ permanent_allocator
│  │  │  │  ├─ frame_allocator
│  │  │  │  ├─ pool_allocator
│  │  │  │  └─ stack_allocator
│  │  │  ├─ tracking (debug, leaks)
│  │  │  └─ alignment/utils
│  │  ├─ time
│  │  │  ├─ clock
│  │  │  ├─ delta_time
│  │  │  └─ frame_pacing
│  │  ├─ threading
│  │  │  ├─ job_system
│  │  │  ├─ worker_threads
│  │  │  ├─ synchronization (mutex, atomics)
│  │  │  └─ task_queue
│  │  ├─ logging
│  │  ├─ asserts
│  │  └─ config
│  │
│  ├─ platform
│  │  ├─ windowing (GLFW/Win32/etc.)
│  │  ├─ input_backend
│  │  ├─ file_io
│  │  ├─ filesystem_watcher (hot reload)
│  │  ├─ dynamic_library (DLL loading/unloading)
│  │  └─ timers/os_services
│  │
│  ├─ renderer
│  │  ├─ frontend
│  │  │  ├─ render_commands / request_queue
│  │  │  ├─ command_buffer
│  │  │  └─ frame_graph (optional)
│  │  ├─ backend
│  │  │  ├─ api (OpenGL/Vulkan/DirectX)
│  │  │  ├─ context
│  │  │  ├─ state_cache
│  │  │  └─ resource_binding
│  │  ├─ resources
│  │  │  ├─ buffers (vbo, ibo, ubo)
│  │  │  ├─ textures
│  │  │  ├─ shaders
│  │  │  ├─ pipelines
│  │  │  └─ framebuffers
│  │  ├─ scene_rendering
│  │  │  ├─ cameras
│  │  │  ├─ lighting
│  │  │  ├─ materials
│  │  │  └─ draw_submission
│  │  └─ debug
│  │     ├─ renderdoc_hooks
│  │     └─ gpu_validation
│  │
│  ├─ ecs
│  │  ├─ entity
│  │  │  ├─ entity_id
│  │  │  └─ versioning
│  │  ├─ components
│  │  │  ├─ transform
│  │  │  ├─ mesh_renderer
│  │  │  ├─ camera
│  │  │  └─ custom_components (game-defined)
│  │  ├─ storage
│  │  │  ├─ sparse_set / dense_arrays
│  │  │  ├─ archetypes (optional)
│  │  │  └─ component_pools
│  │  ├─ systems
│  │  │  ├─ system_scheduler
│  │  │  ├─ queries/views
│  │  │  └─ execution_order
│  │  └─ world
│  │     ├─ entity_registry
│  │     └─ scene_state
│  │
│  ├─ physics
│  │  ├─ collision
│  │  │  ├─ broadphase
│  │  │  └─ narrowphase
│  │  ├─ rigid_bodies
│  │  ├─ constraints
│  │  └─ simulation_step
│  │
│  ├─ input
│  │  ├─ devices
│  │  │  ├─ keyboard
│  │  │  ├─ mouse
│  │  │  └─ controller
│  │  ├─ state (pressed/held/released)
│  │  └─ bindings / mapping
│  │
│  ├─ assets
│  │  ├─ asset_manager
│  │  ├─ loaders
│  │  │  ├─ texture_loader
│  │  │  ├─ mesh_loader
│  │  │  ├─ shader_loader
│  │  │  └─ audio_loader
│  │  ├─ handles / ids
│  │  ├─ caching
│  │  └─ hot_reload
│  │
│  ├─ audio (optional)
│  │  ├─ playback
│  │  ├─ mixing
│  │  └─ spatial_audio
│  │
│  ├─ scripting (optional)
│  │  ├─ dll_interface (your hot-reload layer)
│  │  ├─ bindings_to_ecs
│  │  └─ runtime_api
│  │
│  └─ networking (optional)
│     ├─ sockets
│     ├─ replication
│     └─ serialization
│
├─ editor
│  ├─ editor_core
│  │  ├─ editor_application
│  │  ├─ state (selected_entity, modes)
│  │  ├─ undo_redo
│  │  └─ command_system
│  │
│  ├─ ui (ImGui, etc.)
│  │  ├─ docking
│  │  ├─ panels
│  │  └─ styling
│  │
│  ├─ inspector
│  │  ├─ component_editors
│  │  └─ property_reflection (or manual UI)
│  │
│  ├─ scene_view
│  │  ├─ editor_camera
│  │  ├─ rendering (uses runtime renderer)
│  │  └─ picking (entity selection)
│  │
│  ├─ content_browser
│  │  ├─ filesystem_view
│  │  ├─ asset_importing
│  │  └─ drag_drop
│  │
│  ├─ gizmos
│  │  ├─ translate
│  │  ├─ rotate
│  │  └─ scale
│  │
│  ├─ scene_management
│  │  ├─ scene_loading
│  │  ├─ scene_saving
│  │  └─ prefabs
│  │
│  └─ debug_tools
│     ├─ console
│     ├─ profiler
│     └─ visualizers
│
└─ shared (used by both runtime + editor)
   ├─ math
   │  ├─ vec2/vec3/vec4
   │  ├─ mat4
   │  ├─ quaternion
   │  └─ transforms
   ├─ containers
   │  ├─ vector
   │  ├─ hashmap
   │  ├─ string
   │  └─ handle_types
   ├─ serialization
   │  ├─ binary
   │  └─ json/yaml
   └─ utilities
      ├─ hashing (string interning fits here)
      └─ ids


Game (DLL / hot-reloaded)
├─ entry_points
│  ├─ application_init
│  ├─ application_update
│  └─ application_render
│
├─ gameplay
│  ├─ systems
│  │  ├─ player_system
│  │  ├─ ai_system
│  │  └─ gameplay_rules
│  └─ components
│     └─ game_specific_components
│
├─ scenes
│  ├─ level_1
│  └─ test_scene
│
└─ assets
   ├─ textures
   ├─ meshes
   ├─ shaders
   └─ audio


Engine (exe)
 ├─ creates GLFW window
 ├─ creates OpenGL context
 ├─ calls gladLoadGLLoader
 ├─ owns ALL OpenGL calls
 └─ calls into DLL (game)

 Game (DLL)
 ├─ builds render requests
 └─ calls engine API (not GL)
*/