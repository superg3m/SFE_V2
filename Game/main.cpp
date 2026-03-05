#include <sfe.hpp>

/*
#undef APIENTRY
#include <windows.h>
#undef CopyFile

FILETIME get_last_write_time(const char* path) {
    WIN32_FILE_ATTRIBUTE_DATA data;
    if (GetFileAttributesExA(path, GetFileExInfoStandard, &data)) {
        return data.ftLastWriteTime;
    }
    FILETIME zero = {0};
    return zero;
}

BOOL filetime_changed(FILETIME a, FILETIME b) {
    return CompareFileTime(&a, &b) != 0;
}

GLFWwindow* GLFW_INIT(int WIDTH, int HEIGHT) {
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

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    glEnable(GL_MULTISAMPLE);  

    return window;
}

typedef void(ClearColorFuncT)(OpenGL_API* gl);

int main(int argc, char** argv) {
	Memory::GeneralAllocator allocator = Memory::GeneralAllocator();
	Memory::bindAllocator(&allocator);

    GLFWwindow* window = GLFW_INIT(800, 800);
	if (window == nullptr) {
		return 1;
	}

	const char* dll_name = "Game.dll";
	const char* temp_dll_name = "GameTemp.dll";
	FILETIME last_write_time = get_last_write_time(dll_name);

	Error error = Error::SUCCESS;
	Platform::DLL game_dll = nullptr;
	ClearColorFuncT* clear_color = nullptr;

	OpenGL_API gl;
	gl.seed = Random::GenerateSeed(35);
	gl.clearColor = glad_glClearColor;
	gl.clear = glad_glClear;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	
		{
			FILETIME new_time = get_last_write_time(dll_name);
			if (!game_dll || filetime_changed(new_time, last_write_time)) {
				last_write_time = new_time;

				if (game_dll) {
					Platform::freeDLL(game_dll, error);
					game_dll = nullptr;
					clear_color = nullptr;
				}

				Platform::CopyFile(dll_name, temp_dll_name, true);

				game_dll = Platform::LoadDLL(temp_dll_name, error);
				if (error != Error::SUCCESS) {
					LOG_ERROR("%s\n", getErrorString(error));
					return 1;
				}

				clear_color = (ClearColorFuncT*)Platform::GetProcAddress(game_dll, "clear_color", error);
				if (error != Error::SUCCESS) {
					LOG_ERROR("%s\n", getErrorString(error));
					return 1;
				}

				gl.test.print();
			}

		}

		if (clear_color) {
			clear_color(&gl);
		}
     
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
*/

#include "game.hpp"

int main() {
	Game* game = new Game;
	SFE::Engine& engine = SFE::Engine::GetInstance();
	engine.SetApplication(game);

	if (engine.Initalize(800, 800, "LearningOpenGL")){
		engine.Run();
	}

	engine.Shutdown();

	return 0;
}