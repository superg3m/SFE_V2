#pragma once

#include "Input/sfe_input_manager.hpp"
#include "Core/Memory/sfe_allocator.hpp"
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

	private:
		Application* app = nullptr;
		GLFWwindow* window = nullptr;

		InputManager input;
		std::chrono::steady_clock::time_point previous_time;

		Engine() = default;
		Engine(Engine &input) = delete;
		Engine(Engine &&input) = delete;
		Engine& operator=(Engine &input) = delete;
		Engine& operator=(Engine &&input) = delete;
	};
}