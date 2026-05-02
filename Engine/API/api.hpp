#pragma once

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
	bool reloaded_dll = false;
	void* application_state = nullptr;

	Mat4 get_view_matrix() {
		return this->camera.get_view_matrix();
	}

	Mat4 get_projection_matrix() {
		float aspect = (float)this->window.WINDOW_WIDTH / (float)this->window.WINDOW_HEIGHT;
		return Mat4::perspective(this->camera.zoom, aspect, 0.1f, 1000.0f);
	}
};