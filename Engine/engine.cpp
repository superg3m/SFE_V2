#include "engine.hpp"

INTERNAL_LINKAGE Platform::DLL* dll = nullptr;
INTERNAL_LINKAGE const char* dll_name = "game.dll";
INTERNAL_LINKAGE const char* temp_dll_name = "application_temp.dll";
INTERNAL_LINKAGE Platform::FileTime last_write_time = {};

Engine* engine = nullptr;
extern Hashmap<String, String>* string_intern_map;

void mouse(GLFWwindow* window, double mouse_x, double mouse_y) {
	static bool first = true;
	static float last_mouse_x;
	static float last_mouse_y;

	if (first) {
		last_mouse_x = mouse_x;
		last_mouse_y = mouse_y;
		first = false;
		return;
	}

	float xoffset = mouse_x - last_mouse_x;
	float yoffset = last_mouse_y - mouse_y;

	last_mouse_x = mouse_x;
	last_mouse_y = mouse_y;

	if (engine->mouse_captured) {
		engine->camera.process_mouse_movement(xoffset, yoffset);
	}
}

void load_application_function_pointers(ApplicationInitalizeFunc** application_init, ApplicationUpdateFunc** application_update, ApplicationRenderFunc** application_render) {
	if (dll) {
		Platform::free_dll(dll);
		*application_update = nullptr;
		*application_render = nullptr;
		dll    = nullptr;
	}

	Platform::copy_file(dll_name, temp_dll_name, true);
	last_write_time = Platform::get_file_modification_time(temp_dll_name);

	Error err = Error::SUCCESS;
	dll = Platform::load_dll(temp_dll_name, err);
	RUNTIME_ASSERT_MSG(err == Error::SUCCESS, "Failed to load dll | %s\n", error_get_string(err));

	err = Error::SUCCESS;
	if (application_init) {
		*application_init = (ApplicationInitalizeFunc*)Platform::get_function_address(dll, "application_init");
	}

	*application_update = (ApplicationUpdateFunc*)Platform::get_function_address(dll, "application_update");
	*application_render = (ApplicationRenderFunc*)Platform::get_function_address(dll, "application_render");

	RUNTIME_ASSERT(*application_update && *application_render);
}

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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	engine->camera.process_mouse_scroll((float)yoffset);
}

GLFWwindow* GLFW_INIT(const int WIDTH, const int HEIGHT) {
	RUNTIME_ASSERT_MSG(glfwInit(), "Failed to init glfw\n");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr) {
		LOG_ERROR("Failed to create GLFW window\n");
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		LOG_ERROR("Failed to initialize GLAD\n");
		glfwTerminate();
		exit(-1);
	}

	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, engine->mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	gl_error_check(glEnable(GL_MULTISAMPLE));
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT); 
	// glEnable(GL_FRAMEBUFFER_SRGB);

	return window;
}

Mat4 Engine::get_view_matrix() {
	return this->camera.get_view_matrix();
}

Mat4 Engine::get_projection_matrix() {
	float aspect = (float)this->renderer.WINDOW_WIDTH / (float)this->renderer.WINDOW_HEIGHT;
	return Mat4::perspective(this->camera.zoom, aspect, 0.1f, 1000.0f);
}

bool Engine::init(Allocator permenant_allocator, Allocator frame_allocator) {
	this->permenant_allocator = permenant_allocator;
	this->frame_allocator = frame_allocator;

	this->renderer = Renderer<OpenGL>::create(this->permenant_allocator, this->frame_allocator);
	this->window = GLFW_INIT(this->renderer.WINDOW_WIDTH, this->renderer.WINDOW_HEIGHT);
	if (!this->window) {
		return false;
	}

	this->input.init(this->permenant_allocator);
	if (!INPUT_GLFW_SETUP(&this->input, this->window, nullptr, nullptr, mouse)) {
		return false;
	}
	
	load_application_function_pointers(&this->application_init, &this->application_update, &this->application_render);
	if (application_init) application_init(this, string_intern_map);

	return true;
}

void Engine::update(float dt) {
	Platform::FileTime new_time = Platform::get_file_modification_time(dll_name);
	if (Platform::compare_file_modification_time(new_time, last_write_time) == false) {
		load_application_function_pointers(nullptr, &this->application_update, &this->application_render);
		this->reloaded_dll = true;
	}

	if (this->application_update) application_update(this, string_intern_map, dt);

	input.poll();
}

void Engine::render(float dt) {
	if (this->application_render) application_render(this, string_intern_map, dt);
	
	this->renderer.backend.resolve_requests(this->renderer.requests, this->frame_allocator);
	this->renderer.requests.clear();

	this->reloaded_dll = false;
}