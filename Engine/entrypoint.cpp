#include "engine.hpp"

extern Engine* engine;
extern Hashmap<String, String>* string_intern_map;

int main() {
	constexpr int PROGRAM_MEMORY_CAPACITY = MB(50);
	Allocator platform_allocator = Platform::get_allocator();
	void* program_memory = platform_allocator.malloc(PROGRAM_MEMORY_CAPACITY, alignof(u8));
	Arena program_arena = Arena::fixed(program_memory, PROGRAM_MEMORY_CAPACITY);
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

	engine = (Engine*)permanent_arena_allocator.malloc(sizeof(Engine), alignof(Engine));
	*engine = {}; 
	if (!engine->init(permanent_arena_allocator, frame_arena_allocator)) {
		return -1;
	}

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