#include "input.hpp"
#include "../../Vendor/vendor.hpp"
#include "../engine.hpp"

struct GLFW_Context {
	int glfw_key_map[KEY_CODE_COUNT] = {-1};
	GLFWwindow* window = nullptr;
};

extern Engine* engine;
GLFW_Context ctx;

INTERNAL_LINKAGE KeyState updated_key_state(KeyState state, bool down) {
	if (down) {
		KeyState new_state = (state == KeyState::UP || state == KeyState::RELEASED) ? KeyState::PRESSED : KeyState::DOWN;
		return new_state;
	} else {
		KeyState new_state = (state == KeyState::DOWN || state == KeyState::PRESSED) ? KeyState::RELEASED : KeyState::UP;
		return new_state;
	}
}

void mouse(GLFWwindow* window, double mouse_x, double mouse_y) {
	engine->input.input_state.previous_mouse = engine->input.input_state.current_mouse;
	engine->input.input_state.current_mouse = Vec2(mouse_x, mouse_y);

	Vec2 delta = Vec2(
		engine->input.input_state.current_mouse.x - engine->input.input_state.previous_mouse.x,
		engine->input.input_state.previous_mouse.y - engine->input.input_state.current_mouse.y
	);

	if (engine->window.capture_mouse) {
		engine->scene.active_camera.process_mouse_movement(delta.x, delta.y);
	}
}


void scroll(GLFWwindow* window, double xoffset, double yoffset) {
	// engine->scene.active_camera.process_mouse_scroll((float)yoffset);
}

InputSystem InputSystem::create(void* window) {
	InputSystem ret = {};

	ctx.window = (GLFWwindow*)window;
	ctx.glfw_key_map[KEY_A] = GLFW_KEY_A;
	ctx.glfw_key_map[KEY_B] = GLFW_KEY_B;
	ctx.glfw_key_map[KEY_C] = GLFW_KEY_C;
	ctx.glfw_key_map[KEY_D] = GLFW_KEY_D;
	ctx.glfw_key_map[KEY_E] = GLFW_KEY_E;
	ctx.glfw_key_map[KEY_F] = GLFW_KEY_F;
	ctx.glfw_key_map[KEY_G] = GLFW_KEY_G;
	ctx.glfw_key_map[KEY_H] = GLFW_KEY_H;
	ctx.glfw_key_map[KEY_I] = GLFW_KEY_I;
	ctx.glfw_key_map[KEY_J] = GLFW_KEY_J;
	ctx.glfw_key_map[KEY_K] = GLFW_KEY_K;
	ctx.glfw_key_map[KEY_L] = GLFW_KEY_L;
	ctx.glfw_key_map[KEY_M] = GLFW_KEY_M;
	ctx.glfw_key_map[KEY_N] = GLFW_KEY_N;
	ctx.glfw_key_map[KEY_O] = GLFW_KEY_O;
	ctx.glfw_key_map[KEY_P] = GLFW_KEY_P;
	ctx.glfw_key_map[KEY_Q] = GLFW_KEY_Q;
	ctx.glfw_key_map[KEY_R] = GLFW_KEY_R;
	ctx.glfw_key_map[KEY_S] = GLFW_KEY_S;
	ctx.glfw_key_map[KEY_T] = GLFW_KEY_T;
	ctx.glfw_key_map[KEY_U] = GLFW_KEY_U;
	ctx.glfw_key_map[KEY_V] = GLFW_KEY_V;
	ctx.glfw_key_map[KEY_W] = GLFW_KEY_W;
	ctx.glfw_key_map[KEY_X] = GLFW_KEY_X;
	ctx.glfw_key_map[KEY_Y] = GLFW_KEY_Y;
	ctx.glfw_key_map[KEY_Z] = GLFW_KEY_Z;

	ctx.glfw_key_map[KEY_EQUAL] = GLFW_KEY_EQUAL;
	ctx.glfw_key_map[KEY_MINUS] = GLFW_KEY_MINUS;
	ctx.glfw_key_map[KEY_PERIOD] = GLFW_KEY_PERIOD;

	ctx.glfw_key_map[KEY_0] = GLFW_KEY_0;
	ctx.glfw_key_map[KEY_1] = GLFW_KEY_1;
	ctx.glfw_key_map[KEY_2] = GLFW_KEY_2;
	ctx.glfw_key_map[KEY_3] = GLFW_KEY_3;
	ctx.glfw_key_map[KEY_4] = GLFW_KEY_4;
	ctx.glfw_key_map[KEY_5] = GLFW_KEY_5;
	ctx.glfw_key_map[KEY_6] = GLFW_KEY_6;
	ctx.glfw_key_map[KEY_7] = GLFW_KEY_7;
	ctx.glfw_key_map[KEY_8] = GLFW_KEY_8;
	ctx.glfw_key_map[KEY_9] = GLFW_KEY_9;

	ctx.glfw_key_map[KEY_SPACE]     = GLFW_KEY_SPACE;
	ctx.glfw_key_map[KEY_ENTER]     = GLFW_KEY_ENTER;
	ctx.glfw_key_map[KEY_ESCAPE]    = GLFW_KEY_ESCAPE;
	ctx.glfw_key_map[KEY_TAB]       = GLFW_KEY_TAB;
	ctx.glfw_key_map[KEY_BACKSPACE] = GLFW_KEY_BACKSPACE;

	ctx.glfw_key_map[KEY_LEFT]  = GLFW_KEY_LEFT;
	ctx.glfw_key_map[KEY_RIGHT] = GLFW_KEY_RIGHT;
	ctx.glfw_key_map[KEY_UP]    = GLFW_KEY_UP;
	ctx.glfw_key_map[KEY_DOWN]  = GLFW_KEY_DOWN;

	ctx.glfw_key_map[KEY_CTRL]  = GLFW_KEY_LEFT_CONTROL; // default side
	ctx.glfw_key_map[KEY_SHIFT] = GLFW_KEY_LEFT_SHIFT;
	ctx.glfw_key_map[KEY_ALT]   = GLFW_KEY_LEFT_ALT;

	ctx.glfw_key_map[KEY_F1]  = GLFW_KEY_F1;
	ctx.glfw_key_map[KEY_F2]  = GLFW_KEY_F2;
	ctx.glfw_key_map[KEY_F3]  = GLFW_KEY_F3;
	ctx.glfw_key_map[KEY_F4]  = GLFW_KEY_F4;
	ctx.glfw_key_map[KEY_F5]  = GLFW_KEY_F5;
	ctx.glfw_key_map[KEY_F6]  = GLFW_KEY_F6;
	ctx.glfw_key_map[KEY_F7]  = GLFW_KEY_F7;
	ctx.glfw_key_map[KEY_F8]  = GLFW_KEY_F8;
	ctx.glfw_key_map[KEY_F9]  = GLFW_KEY_F9;
	ctx.glfw_key_map[KEY_F10] = GLFW_KEY_F10;
	ctx.glfw_key_map[KEY_F11] = GLFW_KEY_F11;
	ctx.glfw_key_map[KEY_F12] = GLFW_KEY_F12;

	ctx.glfw_key_map[MOUSE_BUTTON_LEFT] = GLFW_MOUSE_BUTTON_LEFT;
	ctx.glfw_key_map[MOUSE_BUTTON_RIGHT] = GLFW_MOUSE_BUTTON_RIGHT;
	ctx.glfw_key_map[MOUSE_BUTTON_MIDDLE] = GLFW_MOUSE_BUTTON_MIDDLE;

	glfwSetCursorPosCallback((GLFWwindow*)window, mouse);
	glfwSetScrollCallback((GLFWwindow*)window, scroll);

	return ret;
}

void InputSystem::poll() {
	static bool previous_captured_mouse = engine->window.capture_mouse;
	if (previous_captured_mouse != engine->window.capture_mouse) {
		previous_captured_mouse = engine->window.capture_mouse;
		glfwSetInputMode((GLFWwindow*)engine->window.ctx, GLFW_CURSOR, engine->window.capture_mouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}

	for (int key_code = 0 ; key_code < KEY_CODE_COUNT; key_code++) {
		KeyState key_state = this->input_state.keys[key_code];
		if (key_state == KeyState::PRESSED) {
			this->input_state.keys[key_code] = updated_key_state(key_state, true);
		} else if (key_state == KeyState::RELEASED) {
			this->input_state.keys[key_code] = updated_key_state(key_state, false);
		}

		int glfw_key_code = ctx.glfw_key_map[key_code];
		/*
		if (ctx.glfw_key_map[glfw_key_code] == -1) {
			LOG_WARN("Missing glfw mapping | Code: %d\n", key_code);
		}
		*/

		int glfw_state = glfwGetKey(ctx.window, glfw_key_code);
		this->input_state.keys[key_code] = updated_key_state(key_state, glfw_state != GLFW_RELEASE);
	}
}