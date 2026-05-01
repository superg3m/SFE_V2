#pragma once

#include "input_api.hpp"

struct InputSystem {
	InputState input_state = {};

	void init(void* ctx);
	void poll();
};