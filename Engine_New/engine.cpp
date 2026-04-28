#include "engine.hpp"

INTERNAL_LINKAGE Platform::DLL* dll = nullptr;
INTERNAL_LINKAGE const char* dll_name = "application.dll";
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
	
	last_write_time = Platform::get_file_modification_time(dll_name);

	Error err = Error::SUCCESS;
    dll = Platform::load_dll(temp_dll_name, err);
    RUNTIME_ASSERT_MSG(err == Error::SUCCESS, "Failed to load dll | %s\n", error_get_string(err));

	Error err = Error::SUCCESS;
	if (application_init) {
		*application_init = (ApplicationInitalizeFunc*)Platform::get_function_address(dll, "application_init", err);
		if (err != Error::SUCCESS) {
			LOG_ERROR("Failed to find application_init | Error: %s", error_get_string(err));
		}
	}

	*application_update = (ApplicationUpdateFunc*)Platform::get_function_address(dll, "application_update", err);
	if (err != Error::SUCCESS) {
		LOG_ERROR("Failed to find application_init | Error: %s", error_get_string(err));
	}

	*application_render = (ApplicationRenderFunc*)Platform::get_function_address(dll, "application_render", err);
	if (err != Error::SUCCESS) {
		LOG_ERROR("Failed to find application_init | Error: %s", error_get_string(err));
	}
}

Camera camera = Camera(0, 1, 10);
bool mouse_captured = true;

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

	if (mouse_captured) {
		camera.process_mouse_movement(xoffset, yoffset);
	}
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
	glfwSetInputMode(window, GLFW_CURSOR, mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

	gl_error_check(glEnable(GL_MULTISAMPLE));
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT); 
	// glEnable(GL_FRAMEBUFFER_SRGB);

	return window;
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

bool Engine::init(Allocator permenant_allocator, Allocator frame_allocator) {
	this->permenant_allocator = permenant_allocator;
	this->frame_allocator = frame_allocator;

	this->window = GLFW_INIT(this->renderer.WINDOW_WIDTH, this->renderer.WINDOW_HEIGHT);
	if (!this->window) {
		return false;
	}

	this->input.init(permenant_allocator);
	if (!INPUT_GLFW_SETUP(&this->input, this->window, nullptr, nullptr, mouse)) {
		return false;
	}

	this->renderer = Renderer::create(permenant_allocator, frame_allocator);

	Error err = Error::SUCCESS;
	Platform::DLL* dll = Platform::load_dll("game.dll", err);
	if (err != Error::SUCCESS) {
		LOG_ERROR("Failed to find game.dll | Error: %s", error_get_string(err));
	} else {
		load_application_function_pointers(&this->application_init, &this->application_update, &this->application_render);
	}

	if (application_init) application_init(this);

	return true;
}

void Engine::update(float dt) {
	// active_scene.update(dt);

	if (input.get_key_pressed(KEY_R)) {
		// this->renderer.reload_shaders();
	}

	if (input.get_key_pressed(KEY_ESCAPE)) {
		glfwSetWindowShouldClose(this->window, true);
	}

	if (input.get_key_pressed(KEY_C)) {
		mouse_captured = !mouse_captured;
		glfwSetInputMode(this->window, GLFW_CURSOR, mouse_captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	if (input.get_key(KEY_SPACE, PRESSED|DOWN)) {
		camera.process_keyboard(CameraDirection::UP, dt);
	}

	if (input.get_key(KEY_CTRL, PRESSED|DOWN)) {
		camera.process_keyboard(CameraDirection::DOWN, dt);
	}

	if (input.get_key(KEY_W, PRESSED|DOWN)) {
		camera.process_keyboard(CameraDirection::FORWARD, dt); 
	}

	if (input.get_key(KEY_A, PRESSED|DOWN)) {
		camera.process_keyboard(CameraDirection::LEFT, dt); 
	}

	if (input.get_key(KEY_S, PRESSED|DOWN)) {
		camera.process_keyboard(CameraDirection::BACKWARD, dt);
	}

	if (input.get_key(KEY_D, PRESSED|DOWN)) {
		camera.process_keyboard(CameraDirection::RIGHT, dt); 
	}

	Platform::FileTime new_time = Platform::get_file_modification_time(dll_name);
	if (Platform::compare_file_modification_time(new_time, last_write_time)) {
		load_application_function_pointers(nullptr, &this->application_update, &this->application_render);
	}

	if (this->application_update) application_update(dt);

	input.poll();
}

void Engine::render(float dt) {
	if (this->application_render) application_render(&this->renderer, dt);

	this->renderer.draw(this->frame_allocator);
}