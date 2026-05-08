#include "Core/core.hpp"
#include "Public/api.hpp"
#include "Editor/editor.hpp"
#include "Runtime/runtime.hpp"

Engine* engine = nullptr;

constexpr int PROGRAM_MEMORY_CAPACITY = MB(50);
constexpr int PERMANENT_MEMORY_CAPACITY = MB(15);
constexpr int STRING_MEMORY_CAPACITY = MB(15);
constexpr int FRAME_MEMORY_CAPACITY = MB(15);

INTERNAL_LINKAGE Platform::DLL* dll = nullptr;
INTERNAL_LINKAGE const char* dll_name = "game.dll";
INTERNAL_LINKAGE const char* temp_dll_name = "application_temp.dll";
INTERNAL_LINKAGE Platform::FileTime last_write_time = {};

typedef void(ApplicationInitalizeFunc)(EngineAPI* engine, Arena* string_arena, Hashmap<String, String>* string_intern_map);
typedef void(ApplicationUpdateFunc)(EngineAPI* engine, Arena* string_arena, Hashmap<String, String>* string_intern_map, float dt);
typedef void(ApplicationRenderFunc)(EngineAPI* engine, Arena* string_arena, Hashmap<String, String>* string_intern_map, float dt);

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
	Platform::init();
	Allocator platform_allocator = Platform::get_allocator();

	void* program_memory = platform_allocator.malloc(PROGRAM_MEMORY_CAPACITY, alignof(u8));
	Arena program_arena = Arena::fixed(program_memory, PROGRAM_MEMORY_CAPACITY);
	Allocator program_arena_allocator = program_arena.to_allocator();

	MemoryContext memory = {};
	memory.permanent_arena = Arena::fixed(program_arena.push(PERMANENT_MEMORY_CAPACITY, alignof(u8)), PERMANENT_MEMORY_CAPACITY);
	memory.permanent_allocator = memory.permanent_arena.to_allocator();
	memory.frame_arena = Arena::fixed(program_arena.push(FRAME_MEMORY_CAPACITY, alignof(u8)), FRAME_MEMORY_CAPACITY);
	memory.frame_allocator = memory.frame_arena.to_allocator();

	Arena string_arena = Arena::fixed(program_arena.push(STRING_MEMORY_CAPACITY, alignof(u8)), STRING_MEMORY_CAPACITY);
	Allocator string_allocator = string_arena.to_allocator();

	// TODO(Jovanni): Think if I still need engine and string_intern to be weird globals, or if I can get away with stack allocated stuff here?
	engine = Engine::create(memory, 800, 600, "HelloWorld");
	Hashmap<String, String> string_intern_map = Hashmap<String, String>(memory.permanent_allocator);

	EngineAPI api = engine->API();
	Editor editor = Editor::create(engine);

	ApplicationInitalizeFunc* application_init = nullptr;
	ApplicationUpdateFunc* application_update = nullptr;
	ApplicationRenderFunc* application_render = nullptr;

	RUNTIME_ASSERT(Platform::file_exists(dll_name));
	load_application_function_pointers(&application_init, &application_update, &application_render);
	application_init(&api, &string_arena, &string_intern_map);

	/*
	TODO(Jovanni): Actually implement this
	CameraData camera_data = {
		.view = view,
		.persepctive_projection = perspective,
		.ortho_projection = ortho
	}
	*/

	float dt = 0.0f;
	float previous_time = Platform::get_seconds_elapsed();
	while (!engine->window.should_close()) {
		Temp temp = Temp::begin(&memory.frame_arena);
			float current_time = Platform::get_seconds_elapsed();
			dt = current_time - previous_time;
			previous_time = current_time;

			Platform::FileTime new_time = Platform::get_file_modification_time(dll_name);
			if (Platform::compare_file_modification_time(new_time, last_write_time) == false) {
				load_application_function_pointers(nullptr, &application_update, &application_render);
				engine->reloaded_dll = true;
			}

			engine->input.poll();
			engine->scene.update(&api, dt);
			if (application_update) application_update(&api, &string_arena, &string_intern_map, dt);
			if (application_render) application_render(&api, &string_arena, &string_intern_map, dt);
			engine->renderer.execute_deferred_requests(&api); // NOTE(Jovanni): All render requests have to be deferred
			engine->manager.execute_deferred_requests(&engine->renderer, &api); // NOTE(Jovanni): Might have to unpack like render objects for example.
			editor.render(engine, &engine->renderer);
			gl_error_check((void)0); // ImGUI is throwing an opengl error

			engine->reloaded_dll = false;
			engine->window.pump_messages();
		temp.end();
	}

	return 0;
}