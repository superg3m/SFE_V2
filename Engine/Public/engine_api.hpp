#pragma once

#include "../Core/core.hpp"
#include "render_api.hpp"
#include "input_api.hpp"
#include "entity_manager_api.hpp"
#include "window_api.hpp"
#include "scene_api.hpp"

struct EngineAPI {
	MemoryContext memory = {};
	RenderAPI renderer;
	InputAPI input;
	EntityManagerAPI manager;
	WindowAPI window;
	SceneAPI scene;
	
	void*& app;
	bool& reloaded_dll;

	EngineAPI(
		MemoryContext memory,
		RenderAPI     renderer,
		InputAPI      input,
		EntityManagerAPI manager,
		WindowAPI     window,
		SceneAPI      scene,

		void*& app,
		bool& reloaded_dll
	) :
		memory       (memory),
		renderer     (renderer),
		input        (input),
		manager      (manager),
		window       (window),
		scene        (scene),
		app          (app),
		reloaded_dll (reloaded_dll)
	{}
		
	Mat4 get_view_matrix() {
		return this->scene.active_camera.get_view_matrix();
	}

	Mat4 get_projection_matrix() {
		float aspect = (float)this->window.WINDOW_WIDTH / (float)this->window.WINDOW_HEIGHT;
		return Mat4::perspective(this->scene.active_camera.zoom, aspect, 0.1f, 1000.0f);
	}
};