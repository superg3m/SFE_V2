#include "../Core/core.hpp"
#include "../Runtime/Input/input_api.hpp"
#include "../Runtime/WindowCreation/window_creation.hpp"
#include "../Runtime/Renderer/renderer_api.hpp"

struct MemoryContext {
	Arena permanent_arena;
	Arena frame_arena;
	// Arena transient_arena;

	Allocator permanent_allocator;
	Allocator frame_allocator;
	// Allocator transient_allocator;
};

struct Engine {
	MemoryContext memory;
	InputState input;
	// RendererAPI renderer;
	Window window;
	bool reloaded_dll = false;
	void* application_state = nullptr;
};