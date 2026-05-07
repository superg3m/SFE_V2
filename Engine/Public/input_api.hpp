#pragma once

#include "../Runtime/Input/input_state.hpp"

struct InputAPI {
	InputState& input_state;

	InputAPI(InputState& input_state) : input_state(input_state) {}

	bool get_key(KeyCode code, KeyState state) {
		return (this->input_state.keys[code] & state) != 0;
	}

	bool get_key_up(KeyCode code) {
		return (this->input_state.keys[code] & UP) != 0;
	}

	bool get_key_pressed(KeyCode code) {
		return (this->input_state.keys[code] & PRESSED) != 0;
	}

	bool get_key_down(KeyCode code) {
		return (this->input_state.keys[code] & DOWN) != 0;
	}

	bool get_key_released(KeyCode code) {
		return (this->input_state.keys[code] & RELEASED) != 0;
	}
};