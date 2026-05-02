#include "../Core/core.hpp"
#include "window_creation.hpp"
#include "../../Vendor/vendor.hpp"

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

/*
bool Window::should_close() {
	GLFW_WindowContext* context = (GLFW_WindowContext*)this->ctx; 
	return glfwWindowShouldClose(context->window);
}
*/

/*
void framebuffer_size_callback(GLFWwindow* window, int frame_buffer_width, int frame_buffer_height) {
	engine->renderer.FRAME_BUFFER_WIDTH  = frame_buffer_width;
	engine->renderer.FRAME_BUFFER_HEIGHT = frame_buffer_height;
	gl_error_check(glViewport(0, 0, frame_buffer_width, frame_buffer_height));

	// seems like this will be a constant battle might just be worth making renderer and engine a singleton...
	// Renderer::CreatePickingFrameBuffer(Renderer::FRAME_BUFFER_WIDTH, Renderer::FRAME_BUFFER_HEIGHT);
}

void window_size_callback(GLFWwindow* window, int window_width, int window_height) {
	engine->renderer.WINDOW_WIDTH = window_width;
	engine->renderer.WINDOW_HEIGHT = window_height;
}
*/