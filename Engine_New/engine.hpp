#pragma once

#include "sfe.hpp"

struct Engine;
typedef void(ApplicationInitalizeFunc)(Engine* engine);
typedef void(ApplicationUpdateFunc)(float dt);
typedef void(ApplicationRenderFunc)(Renderer* renderer, float dt);

struct Engine {
	GLFWwindow* window;
	Input input;
	Renderer renderer;

	Allocator permenant_allocator;
	Allocator frame_allocator;

	ApplicationInitalizeFunc* application_init = nullptr;
	ApplicationUpdateFunc*    application_update = nullptr;
	ApplicationRenderFunc*    application_render = nullptr;

	bool init(Allocator permenant_allocator, Allocator frame_allocator);
	void update(float dt);
	void render(float dt);
};