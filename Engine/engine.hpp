#pragma once

#include "sfe.hpp"
#include "Renderer/OpenGL/backend.hpp"

struct Engine;
typedef void(ApplicationInitalizeFunc)(Engine* engine, Hashmap<String, String>* string_intern_map);
typedef void(ApplicationUpdateFunc)(Engine* engine, Hashmap<String, String>* string_intern_map, float dt);
typedef void(ApplicationRenderFunc)(Engine* engine, Hashmap<String, String>* string_intern_map, float dt);

struct Engine {
	GLFWwindow* window;
	Renderer<OpenGL> renderer;
	Input input;

	Camera camera = Camera(0, 1, 10);
	bool mouse_captured = false;
	bool reloaded_dll;
	void* application_state;

	Allocator permenant_allocator;
	Allocator string_allocator;
	Allocator frame_allocator;

	Mat4 get_view_matrix();
	Mat4 get_projection_matrix();

	ApplicationInitalizeFunc* application_init = nullptr;
	ApplicationUpdateFunc*    application_update = nullptr;
	ApplicationRenderFunc*    application_render = nullptr;

	bool init(Allocator permenant_allocator, Allocator frame_allocator);
	void update(float dt);
	void render(float dt);
};