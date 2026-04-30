#pragma once

#include "input.hpp"
#undef GLFW_INCLUDE_NONE 
#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>

struct GLFW_CTX {
	Input* input;
	GLFWwindow* window;
	Hashmap<int, KeyCode> glfw_to_key_code;
	GLFWkeyfun cb_keyboard = nullptr;
	GLFWmousebuttonfun cb_mouse_button = nullptr;
	GLFWcursorposfun cb_mouse_move = nullptr;
};

bool INPUT_GLFW_SETUP(Input* input, GLFWwindow* window, GLFWkeyfun key_cb = nullptr, GLFWmousebuttonfun mouse_button_cb = nullptr, GLFWcursorposfun mouse_move_cb = nullptr);