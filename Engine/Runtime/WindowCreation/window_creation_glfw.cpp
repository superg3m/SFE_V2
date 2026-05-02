#include "window_creation.hpp"
#include "../Core/core.hpp"
#include "../../Vendor/vendor.hpp"
#include "../../API/api.hpp"
#include "../Renderer/OpenGL/backend.hpp"

extern Engine* engine;

void framebuffer_size_callback(GLFWwindow* window, int frame_buffer_width, int frame_buffer_height) {
	engine->window.FRAMEBUFFER_WIDTH  = frame_buffer_width;
	engine->window.FRAMEBUFFER_HEIGHT = frame_buffer_height;
	
	// TODO(Jovanni): This sucks it should be abstracted away but for right now who cares
	// realistically an event system would be nice, I should build one...
	// EVENT_FIRE(FRAMEBUFFER_RESIZE, {
	// 		.framebuffer_width = frame_buffer_width, 
	// 		.framebuffer_height = frame_buffer_height, 
	// })
	gl_error_check(glViewport(0, 0, frame_buffer_width, frame_buffer_height)); 

	// seems like this will be a constant battle might just be worth making renderer and engine a singleton...
	// Renderer::CreatePickingFrameBuffer(Renderer::FRAME_BUFFER_WIDTH, Renderer::FRAME_BUFFER_HEIGHT);
}

void window_size_callback(GLFWwindow* window, int window_width, int window_height) {
	engine->window.WINDOW_WIDTH = window_width;
	engine->window.WINDOW_HEIGHT = window_height;
}

Window Window::create(const int WIDTH, const int HEIGHT, const char* name, bool vsync) {
	Window ret = {};
	RUNTIME_ASSERT_MSG(glfwInit(), "Failed to init glfw\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	ret.WINDOW_WIDTH = WIDTH;
	ret.WINDOW_HEIGHT = HEIGHT;
	ret.ctx = glfwCreateWindow(WIDTH, HEIGHT, name, nullptr, nullptr);
	GLFWwindow* window = (GLFWwindow*)ret.ctx;
	RUNTIME_ASSERT_MSG(window != nullptr, "Failed to create GLFW window\n");

	glfwMakeContextCurrent(window);
	RUNTIME_ASSERT_MSG(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Failed to initialize GLAD\n");
	glfwSwapInterval(vsync);

	glfwSetInputMode(window, GLFW_CURSOR, ret.capture_mouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	return ret;
}

bool Window::should_close() {
	GLFWwindow* window = (GLFWwindow*)this->ctx;
	return this->close || glfwWindowShouldClose(window);
}

void Window::pump_messages() {
	GLFWwindow* window = (GLFWwindow*)this->ctx;

	glfwPollEvents();
	glfwSwapBuffers(window);
}