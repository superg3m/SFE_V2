#include "engine.hpp"

INTERNAL_LINKAGE Platform::DLL* dll = nullptr;
INTERNAL_LINKAGE const char* dll_name = "game.dll";
INTERNAL_LINKAGE const char* temp_dll_name = "application_temp.dll";
INTERNAL_LINKAGE Platform::FileTime last_write_time = {};

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
}

void framebuffer_size_callback(GLFWwindow* window, int frame_buffer_width, int frame_buffer_height) {
    Renderer::FRAME_BUFFER_WIDTH  = frame_buffer_width;
    Renderer::FRAME_BUFFER_HEIGHT = frame_buffer_height;
    glViewport(0, 0, frame_buffer_width, frame_buffer_height);

	// seems like this will be a constant battle might just be worth making renderer and engine a singleton...
    // Renderer::CreatePickingFrameBuffer(Renderer::FRAME_BUFFER_WIDTH, Renderer::FRAME_BUFFER_HEIGHT);
}

void window_size_callback(GLFWwindow* window, int window_width, int window_height) {
    Renderer::WINDOW_WIDTH = window_width;
    Renderer::WINDOW_HEIGHT = window_height;
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gl_error_check(glEnable(GL_MULTISAMPLE));
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT); 
	// glEnable(GL_FRAMEBUFFER_SRGB);

	return window;
}

bool Engine::init(Allocator permenant_allocator, Allocator frame_allocator) {
	this->permenant_allocator = permenant_allocator;
	this->frame_allocator = frame_allocator;

	this->renderer = Renderer::create(this->permenant_allocator, this->frame_allocator);
	this->window = GLFW_INIT(Renderer::WINDOW_WIDTH, Renderer::WINDOW_HEIGHT);
	if (!this->window) {
		return false;
	}

	load_application_function_pointers(&this->application_init, &this->application_update, &this->application_render);
	if (application_init) application_init(this);

	return true;
}

void Engine::update(float dt) {
	Platform::FileTime new_time = Platform::get_file_modification_time(dll_name);
	if (Platform::compare_file_modification_time(new_time, last_write_time) == false) {
		load_application_function_pointers(nullptr, &this->application_update, &this->application_render);
		this->reloaded_dll = true;
	}

	if (this->application_update) application_update(this, dt);

	input.poll();
}

void Engine::render(float dt) {
	if (this->application_render) application_render(this, dt);
	this->reloaded_dll = false;
	
	this->renderer.draw(this->frame_allocator);
}