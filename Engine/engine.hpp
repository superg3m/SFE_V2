#pragma once

#include "Runtime/runtime.hpp"

struct Engine {
	MemoryContext memory;
	InputSystem input;
	Renderer<OpenGL> renderer;
	EntityManager manager;
	PhysicsManager physics;
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
		ret->physics = PhysicsManager::create();
 		ret->scene = Scene::create(&ret->manager);

		return ret;
	}

	Mat4 get_view_matrix() {
		if (this->scene.camera != EntityHandle::invalid()) {
			LOCAL_PERSIST EngineAPI api = this->API();

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
	
	EngineAPI API() {
		return EngineAPI(
			this->memory,
			this->renderer.API(),
			this->input.API(),
			this->manager.API(),
			&this->physics,
			this->window.API(),
			this->scene.API(),
			this->app, 
			this->reloaded_dll
		);
	}
};