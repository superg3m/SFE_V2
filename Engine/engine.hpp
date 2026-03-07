#pragma once

#include "Input/sfe_input_manager.hpp"
#include "Core/Memory/sfe_allocator.hpp"
#include "Graphics/sfe_graphics_api.hpp"
#include "Graphics/sfe_render_queue.hpp"
#include <chrono>

struct GLFWwindow;

namespace SFE {
	struct Application;
	struct Engine {
		bool Initalize(float width, float height, const char* name);
		void Run();
		void Shutdown();

		static Engine& GetInstance();
		
		void SetApplication(Application* app);
		Application* GetApplication();
		InputManager& GetInputManager();
		GraphicsAPI& GetGraphicsAPI();
		RenderQueue& GetRenderQueue();

	private:
		Application* app = nullptr;
		GLFWwindow* window = nullptr;

		InputManager input;
		GraphicsAPI graphics;
		RenderQueue render_queue;
		std::chrono::steady_clock::time_point previous_time;

		Engine() = default;
		Engine(Engine &input) = delete;
		Engine(Engine &&input) = delete;
		Engine& operator=(Engine &input) = delete;
		Engine& operator=(Engine &&input) = delete;
	};
}