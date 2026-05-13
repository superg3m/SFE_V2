#pragma once

#include "../Core/core.hpp"
#include "render_api.hpp"
#include "input_api.hpp"
#include "entity_manager_api.hpp"
#include "physics_manager.hpp"
#include "window_api.hpp"
#include "scene_api.hpp"

struct EngineAPI {
	MemoryContext memory = {};
	RenderAPI renderer;
	InputAPI input;
	EntityManagerAPI manager;
	PhysicsManager* physics;
	WindowAPI window;
	SceneAPI scene;
	
	void*& app;
	bool& reloaded_dll;

	EngineAPI(
		MemoryContext memory,
		RenderAPI     renderer,
		InputAPI      input,
		EntityManagerAPI manager,
		PhysicsManager* physics,
		WindowAPI     window,
		SceneAPI      scene,

		void*& app,
		bool& reloaded_dll
	) :
		memory       (memory),
		renderer     (renderer),
		input        (input),
		manager      (manager),
		physics      (physics),
		window       (window),
		scene        (scene),
		app          (app),
		reloaded_dll (reloaded_dll)
	{}
		
	Mat4 get_view_matrix() {
		if (this->scene.camera != EntityHandle::invalid()) {
			Entity& camera = this->manager.get(this->scene.camera);
			CameraComponent* camera_component = camera.get_component<CameraComponent>();
			RUNTIME_ASSERT(camera_component);

			return camera_component->get_view_matrix();
		}

		return Mat4::identity();
	}

	Mat4 get_perspective_matrix() {
		float aspect = (float)this->window.WINDOW_WIDTH / (float)this->window.WINDOW_HEIGHT;
		if (this->scene.camera != EntityHandle::invalid()) {
			Entity& camera = this->manager.get(this->scene.camera);
			CameraComponent* camera_component = camera.get_component<CameraComponent>();
			RUNTIME_ASSERT(camera_component);

			return camera_component->get_perspective_matrix(aspect);
		}

		return Mat4::identity();
	}
};