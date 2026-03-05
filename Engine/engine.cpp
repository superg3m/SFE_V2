#include "engine.hpp"
#include "application.hpp"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace SFE {
	GLFWwindow* GLFW_INIT(int width, int height, const char* name) {
		if (!glfwInit()) {
			LOG_ERROR("Failed to init glfw\n");

			return nullptr;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);

		#ifdef __APPLE__
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		#endif

		GLFWwindow* window = glfwCreateWindow(width, height, name, nullptr, nullptr);
		if (window == nullptr) {
			LOG_ERROR("Failed to create GLFW window\n");
			glfwTerminate();

			return nullptr;
		}

		glfwMakeContextCurrent(window);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			LOG_ERROR("Failed to initialize GLAD\n");
			glfwTerminate();

			return nullptr;
		}

		glfwSwapInterval(1);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
		glEnable(GL_MULTISAMPLE);

		return window;
	}

	bool Engine::Initalize(float width, float height, const char* name) {
		if (this->app == nullptr) {
			return false;
		}

		this->window = GLFW_INIT(width, height, name);
		if (this->window == nullptr) {
			LOG_ERROR("Failed to initialize GLFW and GLAD\n");

			return false;
		}

		InputManager& input = this->GetInputManager();
		if (!input.Initialize(this->window)) {
			LOG_ERROR("Failed to initialize input manager\n");

			return false;
		}
		
		return this->app->Initalize();
	}
	
	void Engine::Run() {
		if (this->app == nullptr) {
			return;
		}

		this->previous_time = std::chrono::high_resolution_clock::now();
		while (!glfwWindowShouldClose(this->window) && !this->app->ShouldClose()) {
			this->input.Poll();
			glfwPollEvents();

			auto current_time = std::chrono::high_resolution_clock::now();
			float dt = std::chrono::duration<float>(current_time - this->previous_time).count();
			this->previous_time = current_time;

			this->app->Update(dt);

			glClearColor(0.2f, 0.8f, 0.2f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glfwSwapBuffers(this->window);
		}
	}
	
	void Engine::Shutdown() {
		if (this->app) {
			this->app->Shutdown();
			this->app = nullptr;
			
			glfwTerminate();
			this->window = nullptr;
		}
	}

	void Engine::SetApplication(Application* app) {
		this->app = app;
	}

	Application* Engine::GetApplication() {
		return this->app;
	}

	Engine& Engine::GetInstance() {
		static Engine instance;
		return instance;
	} 

	InputManager& Engine::GetInputManager() {
		return this->input;
	}
}