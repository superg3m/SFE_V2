#include "input_glfw.hpp"

INTERNAL_LINKAGE GLFW_CTX ctx;

bool INPUT_GLFW_SETUP(Input* input, GLFWwindow* window, GLFWkeyfun key_cb, GLFWmousebuttonfun mouse_button_cb, GLFWcursorposfun mouse_move_cb) {
	input->platform_ctx = (void*)&ctx;
	ctx.input = input;
	ctx.window = window;
	ctx.cb_keyboard = key_cb;
	ctx.cb_mouse_button = mouse_button_cb;
	ctx.cb_mouse_move = mouse_move_cb;
	ctx.glfw_to_key_code = {{
		// Letters
		{GLFW_KEY_A, KEY_A},
		{GLFW_KEY_B, KEY_B},
		{GLFW_KEY_C, KEY_C},
		{GLFW_KEY_D, KEY_D},
		{GLFW_KEY_E, KEY_E},
		{GLFW_KEY_F, KEY_F},
		{GLFW_KEY_G, KEY_G},
		{GLFW_KEY_H, KEY_H},
		{GLFW_KEY_I, KEY_I},
		{GLFW_KEY_J, KEY_J},
		{GLFW_KEY_K, KEY_K},
		{GLFW_KEY_L, KEY_L},
		{GLFW_KEY_M, KEY_M},
		{GLFW_KEY_N, KEY_N},
		{GLFW_KEY_O, KEY_O},
		{GLFW_KEY_P, KEY_P},
		{GLFW_KEY_Q, KEY_Q},
		{GLFW_KEY_R, KEY_R},
		{GLFW_KEY_S, KEY_S},
		{GLFW_KEY_T, KEY_T},
		{GLFW_KEY_U, KEY_U},
		{GLFW_KEY_V, KEY_V},
		{GLFW_KEY_W, KEY_W},
		{GLFW_KEY_X, KEY_X},
		{GLFW_KEY_Y, KEY_Y},
		{GLFW_KEY_Z, KEY_Z},
		{GLFW_KEY_EQUAL, KEY_EQUAL},
		{GLFW_KEY_MINUS, KEY_MINUS},
		{GLFW_KEY_PERIOD, KEY_PERIOD},

		// Numbers
		{GLFW_KEY_0, KEY_0},
		{GLFW_KEY_1, KEY_1},
		{GLFW_KEY_2, KEY_2},
		{GLFW_KEY_3, KEY_3},
		{GLFW_KEY_4, KEY_4},
		{GLFW_KEY_5, KEY_5},
		{GLFW_KEY_6, KEY_6},
		{GLFW_KEY_7, KEY_7},
		{GLFW_KEY_8, KEY_8},
		{GLFW_KEY_9, KEY_9},

		// Common control keys
		{GLFW_KEY_SPACE,     KEY_SPACE},
		{GLFW_KEY_ENTER,     KEY_ENTER},
		{GLFW_KEY_ESCAPE,    KEY_ESCAPE},
		{GLFW_KEY_TAB,       KEY_TAB},
		{GLFW_KEY_BACKSPACE, KEY_BACKSPACE},

		// Arrow keys
		{GLFW_KEY_LEFT,  KEY_LEFT},
		{GLFW_KEY_RIGHT, KEY_RIGHT},
		{GLFW_KEY_UP,    KEY_UP},
		{GLFW_KEY_DOWN, KEY_DOWN},

		// Modifier keys
		{GLFW_KEY_LEFT_CONTROL,  KEY_CTRL},
		{GLFW_KEY_RIGHT_CONTROL, KEY_CTRL},
		{GLFW_KEY_LEFT_SHIFT,    KEY_SHIFT},
		{GLFW_KEY_RIGHT_SHIFT,   KEY_SHIFT},
		{GLFW_KEY_LEFT_ALT,      KEY_ALT},
		{GLFW_KEY_RIGHT_ALT,     KEY_ALT},

		// Function keys
		{GLFW_KEY_F1,  KEY_F1},
		{GLFW_KEY_F2,  KEY_F2},
		{GLFW_KEY_F3,  KEY_F3},
		{GLFW_KEY_F4,  KEY_F4},
		{GLFW_KEY_F5,  KEY_F5},
		{GLFW_KEY_F6,  KEY_F6},
		{GLFW_KEY_F7,  KEY_F7},
		{GLFW_KEY_F8,  KEY_F8},
		{GLFW_KEY_F9,  KEY_F9},
		{GLFW_KEY_F10, KEY_F10},
		{GLFW_KEY_F11, KEY_F11},
		{GLFW_KEY_F12, KEY_F12},

		// Mouse buttons
		{GLFW_MOUSE_BUTTON_LEFT,   MOUSE_BUTTON_LEFT},
		{GLFW_MOUSE_BUTTON_RIGHT,  MOUSE_BUTTON_RIGHT},
		{GLFW_MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_MIDDLE}
	}, input->input_state.allocator};
	
	glfwSetKeyCallback(window, [](GLFWwindow*, int key, int scancode, int action, int mods) {
		if (ctx.cb_keyboard) {
			ctx.cb_keyboard((GLFWwindow*)ctx.window, key, scancode, action, mods);  
		}

		if (!ctx.glfw_to_key_code.has(key)) {
			LOG_WARN("Pressed a glfw key and it is not mapped yet: %s\n", glfwGetKeyName(key, scancode));
			return;
		}

		KeyCode cb_code = ctx.glfw_to_key_code.get(key);
		ctx.input->update_input_code(cb_code, action != GLFW_RELEASE);
		for (const auto profile : ctx.input->profiles) {
			if (!profile.active) {
				continue;
			}

			if (profile.callback) {
				profile.callback();
			}
		}
	});

	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		if (ctx.cb_mouse_button) {
			ctx.cb_mouse_button(window, button, action, mods);   
		}

		KeyCode cb_code = ctx.glfw_to_key_code.get(button);
		ctx.input->update_input_code(cb_code, action != GLFW_RELEASE);
		for (const auto profile : ctx.input->profiles) {
			if (!profile.active) {
				continue;
			}

			if (profile.callback) {
				profile.callback();
			}
		}
	});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		ctx.input->update_mouse_position(static_cast<float>(xpos), static_cast<float>(ypos));
		if (ctx.cb_mouse_move) {
			ctx.cb_mouse_move(window, xpos, ypos);
		}
	});

	return true;
}