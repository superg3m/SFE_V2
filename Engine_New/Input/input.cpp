#include "input.hpp"

void Input::init(Allocator allocator) {
	this->profiles = Vector<Profile>(allocator);
	this->input_state = Hashmap<KeyCode, KeyState>(allocator, {
		#define X(name, value) {name, KeyState::UP},
			X_ASCII_KEYCODE
		#undef X

		#define X(name) {name, KeyState::UP},
			X_COMPLEX_KEYCODE
		#undef X
	});
}

void Input::poll() {
	for (const auto entry : input_state) {
		KeyCode code = entry.key;
		KeyState state = entry.value;

		if (state == KeyState::PRESSED) {
			this->update_input_code(code, true);
		} else if (state == KeyState::RELEASED) {
			this->update_input_code(code, false);
		}
	}

	for (const Profile& profile : profiles) {
		if (!profile.active) {
			continue;
		}

		if (profile.callback) {
			profile.callback();
		}
	}
}

bool Input::get_key(KeyCode code, KeyState state) {
	if (!input_state.has(code)) {
		LOG_WARN("Pressed a key and it is not mapped yet: %c\n", code);
		return false;
	}

	KeyState actual_state = input_state.get(code);
	return state & actual_state;
}

bool Input::get_key_up(KeyCode code) {
	return get_key(code, KeyState::UP);
}
bool Input::get_key_pressed(KeyCode code) {
	return get_key(code, KeyState::PRESSED);
}
bool Input::get_key_down(KeyCode code) {
	return get_key(code, KeyState::DOWN);
}
bool Input::get_key_released(KeyCode code) {
	return get_key(code, KeyState::RELEASED);
}

void Input::create_profile(const char* key, FUNCTION_CALLBACK callback, bool active)  {
	Profile ret;
	ret.name = key;
	ret.callback = callback;
	ret.active = active;

	this->profiles.append(ret);
}

void Input::delete_profile(const char* key)  {
	for (int i = 0; i < this->profiles.count; i++) {
		Profile profile = this->profiles[i];
		if (String::equal(profile.name, key)) {
			this->profiles.unstable_swapback_remove(i);
			return;
		}
	}

	LOG_DEBUG("Could not find profile: %s\n", key);
}

void Input::toggle_profile(const char* key, bool toggle)  {
	for (int i = 0; i < this->profiles.count; i++) {
		Profile* profile = &this->profiles[i];
		if (String::equal(profile->name, key)) {
			profile->active = toggle;
			return;
		}
	}

	LOG_DEBUG("Could not find profile: %s\n", key);
}

void Input::enable_profile(const char* key)  {
	for (int i = 0; i < this->profiles.count; i++) {
		Profile* profile = &this->profiles[i];
		if (String::equal(profile->name, key)) {
			profile->active = true;
			return;
		}
	}

	LOG_DEBUG("Could not find profile: %s\n", key);
}

void Input::disable_profile(const char* key) {
	for (int i = 0; i < this->profiles.count; i++) {
		Profile* profile = &this->profiles[i];
		if (String::equal(profile->name, key)) {
			profile->active = false;
			return;
		}
	}

	LOG_DEBUG("Could not find profile: %s\n", key);
}

void Input::update_input_code(KeyCode code, bool down) {
	KeyState state = input_state.get(code);
	if (down) {
		KeyState new_state = (state == KeyState::UP || state == KeyState::RELEASED) ? KeyState::PRESSED : KeyState::DOWN;
		input_state.put(code, new_state);
	} else {
		KeyState new_state = (state == KeyState::DOWN || state == KeyState::PRESSED) ? KeyState::RELEASED : KeyState::UP;
		input_state.put(code, new_state); 
	}
}

void Input::update_mouse_position(float x, float y) {
	this->previous_mouse = this->current_mouse;
	this->current_mouse = Vec2(x, y);
}