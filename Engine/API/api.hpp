#pragma once

#include "ECS/ecs.hpp"
#include "../Core/core.hpp"
#include "../Runtime/Input/input_api.hpp"
#include "../Runtime/WindowCreation/window_creation.hpp"
#include "../Runtime/Renderer/renderer_api.hpp"

struct Engine {
	MemoryContext memory = {};
	InputState input = {};
	RenderAPI renderer = {};
	Window window = {};
	Camera camera = {};
	Scene scene = {};
	bool reloaded_dll = false;
	void* application_state = nullptr;

	static Engine create(MemoryContext memory, InputState input, RenderAPI renderer, Window window) {
		Engine ret = {};
		ret.memory = memory;
		ret.input = input;
		ret.renderer = renderer;
		ret.window = window;

		ret.scene = Scene::create(memory);

		return ret;
	}

	Mat4 get_view_matrix() {
		return this->camera.get_view_matrix();
	}

	Mat4 get_projection_matrix() {
		float aspect = (float)this->window.WINDOW_WIDTH / (float)this->window.WINDOW_HEIGHT;
		return Mat4::perspective(this->camera.zoom, aspect, 0.1f, 1000.0f);
	}
};