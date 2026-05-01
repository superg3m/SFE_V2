#include "window_creation.hpp"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

struct GLFW_WindowContext {
	GLFWwindow* window = nullptr;
};

void Window::create(const int WIDTH, const int HEIGHT, const char* name, bool vsync) {
	Window ret = {};
	RUNTIME_ASSERT_MSG(glfwInit(), "Failed to init glfw\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	GLFW_WindowContext* context = (GLFW_WindowContext*)ret.ctx; 
	context->window = glfwCreateWindow(WIDTH, HEIGHT, name, nullptr, nullptr);
	RUNTIME_ASSERT_MSG(context->window != nullptr, "Failed to create GLFW window\n");

	glfwMakeContextCurrent(context->window);
	RUNTIME_ASSERT_MSG(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD\n");
	glfwSwapInterval(vsync);
}

bool Window::should_close() {
	GLFW_WindowContext* context = (GLFW_WindowContext*)this->ctx; 
	return glfwWindowShouldClose(context->window);
}

void Window::close() {
	GLFW_WindowContext* context = (GLFW_WindowContext*)this->ctx; 
	return glfwSetWindowShouldClose(context->window, true);
}