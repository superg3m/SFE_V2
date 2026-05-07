#pragma once

#include "Runtime/runtime.hpp"

struct Engine {
	MemoryContext memory;
	InputSystem input;
	Renderer<OpenGL> renderer;
	EntityManager manager;
	Window window;
	Scene scene;

	void* app = nullptr;
	bool reloaded_dll = false;

	static Engine* create(MemoryContext memory, int window_width, int window_height, const char* window_name) {
		Engine* ret = (Engine*)memory.permanent_allocator.malloc(sizeof(Engine), alignof(Engine));
		ret->memory = memory;
		ret->window = Window::create(window_width, window_height, window_name);
		ret->input = InputSystem::create(ret->window.ctx);
		ret->renderer = Renderer<OpenGL>::create(memory);
 		ret->manager = EntityManager::create(memory);
 		ret->scene = Scene::create(&ret->manager);

		return ret;
	}

	Mat4 get_view_matrix() {
		return this->scene.active_camera.get_view_matrix();
	}

	Mat4 get_projection_matrix() {
		float aspect = (float)this->window.WINDOW_WIDTH / (float)this->window.WINDOW_HEIGHT;
		return Mat4::perspective(this->scene.active_camera.zoom, aspect, 0.1f, 1000.0f);
	}
	
	EngineAPI API() {
		return EngineAPI(
			this->memory,
			this->renderer.API(),
			this->input.API(),
			this->manager.API(),
			this->window.API(),
			this->scene.API(),
			this->app, 
			this->reloaded_dll
		);
	}
};