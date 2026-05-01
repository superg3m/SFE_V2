#include "engine.hpp"
#include "Editor/editor.hpp"

extern Hashmap<String, String>* string_intern_map;
extern Editor* editor;
extern Engine* engine;

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

int main() {
	constexpr int PROGRAM_MEMORY_CAPACITY = MB(50);
	Allocator platform_allocator = Platform::get_allocator();
	void* program_memory = platform_allocator.malloc(PROGRAM_MEMORY_CAPACITY, alignof(u8));
	
	Allocator program_arena_allocator = program_arena.to_allocator();

	constexpr int STRING_MEMORY_CAPACITY = MB(10);
	void* string_memory = program_arena.push(STRING_MEMORY_CAPACITY, alignof(u8));
	Arena string_arena = Arena::fixed(string_memory, STRING_MEMORY_CAPACITY);
	Allocator string_arena_allocator = string_arena.to_allocator();

	constexpr int PERMANENT_MEMORY_CAPACITY = MB(10);
	void* permenant_memory = program_arena.push(PERMANENT_MEMORY_CAPACITY, alignof(u8));
	Arena permanent_arena = Arena::fixed(permenant_memory, PERMANENT_MEMORY_CAPACITY);
	Allocator permanent_arena_allocator = permanent_arena.to_allocator();

	constexpr int FRAME_MEMORY_CAPACITY = MB(10);
	void* frame_memory = program_arena.push(FRAME_MEMORY_CAPACITY, alignof(u8));
	Arena frame_arena = Arena::fixed(frame_memory, FRAME_MEMORY_CAPACITY);
	Allocator frame_arena_allocator = frame_arena.to_allocator();

	string_intern_map = (Hashmap<String, String>*)permanent_arena_allocator.malloc(sizeof(Hashmap<String, String>), alignof(Hashmap<String, String>));
	*string_intern_map = Hashmap<String, String>(string_arena_allocator);

	editor = (Editor*)permanent_arena_allocator.malloc(sizeof(Editor), alignof(Editor));
	*editor = {};

	engine = (Engine*)permanent_arena_allocator.malloc(sizeof(Engine), alignof(Engine));
	*engine = {}; 
	if (!engine->init(permanent_arena_allocator, frame_arena_allocator)) {
		return -1;
	}

	// while (!PlatformSystem::window_should_close())
	
	// engine.run();

	// get this glfw code out of here! Just do engine.run();
	float dt = 0.0f;
	float previous_time = glfwGetTime();
	float accumulator = 0.0f;
	while (!glfwWindowShouldClose(engine->window)) {
		Temp frame_temp = Temp::begin(&frame_arena);
			float current_time = glfwGetTime();
			dt = current_time - previous_time;
			previous_time = current_time;
			accumulator += dt * 10;

			engine->update(dt);
			engine->render(dt);

			glfwSwapBuffers(engine->window);
			glfwPollEvents();
		frame_temp.end();
	}

	platform_allocator.free(program_memory);

	return 0;
}