#pragma once

#include "input_state.hpp"

struct InputSystem {
	InputState input_state = {};
	InputAPI API();

	static InputSystem create(void* ctx);
	void poll();
};