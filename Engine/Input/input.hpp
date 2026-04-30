#pragma once

#include "../Core/core.hpp"

#define X_ASCII_KEYCODE \
	X(KEY_A,      'A')  \
	X(KEY_B,      'B')  \
	X(KEY_C,      'C')  \
	X(KEY_D,      'D')  \
	X(KEY_E,      'E')  \
	X(KEY_F,      'F')  \
	X(KEY_G,      'G')  \
	X(KEY_H,      'H')  \
	X(KEY_I,      'I')  \
	X(KEY_J,      'J')  \
	X(KEY_K,      'K')  \
	X(KEY_L,      'L')  \
	X(KEY_M,      'M')  \
	X(KEY_N,      'N')  \
	X(KEY_O,      'O')  \
	X(KEY_P,      'P')  \
	X(KEY_Q,      'Q')  \
	X(KEY_R,      'R')  \
	X(KEY_S,      'S')  \
	X(KEY_T,      'T')  \
	X(KEY_U,      'U')  \
	X(KEY_V,      'V')  \
	X(KEY_W,      'W')  \
	X(KEY_X,      'X')  \
	X(KEY_Y,      'Y')  \
	X(KEY_Z,      'Z')  \
	X(KEY_0,      '0')  \
	X(KEY_1,      '1')  \
	X(KEY_2,      '2')  \
	X(KEY_3,      '3')  \
	X(KEY_4,      '4')  \
	X(KEY_5,      '5')  \
	X(KEY_6,      '6')  \
	X(KEY_7,      '7')  \
	X(KEY_8,      '8')  \
	X(KEY_9,      '9')  \
	X(KEY_EQUAL,  '=')  \
	X(KEY_MINUS,  '-')  \
	X(KEY_PERIOD, '.')  \

#define X_COMPLEX_KEYCODE  \
	X(KEY_SPACE)           \
	X(KEY_ENTER)           \
	X(KEY_ESCAPE)          \
	X(KEY_TAB)             \
	X(KEY_BACKSPACE)       \
	X(KEY_LEFT)            \
	X(KEY_RIGHT)           \
	X(KEY_UP)              \
	X(KEY_DOWN)            \
	X(KEY_CTRL)            \
	X(KEY_SHIFT)           \
	X(KEY_ALT)             \
	X(KEY_F1)              \
	X(KEY_F2)              \
	X(KEY_F3)              \
	X(KEY_F4)              \
	X(KEY_F5)              \
	X(KEY_F6)              \
	X(KEY_F7)              \
	X(KEY_F8)              \
	X(KEY_F9)              \
	X(KEY_F10)             \
	X(KEY_F11)             \
	X(KEY_F12)             \
	X(MOUSE_BUTTON_LEFT)   \
	X(MOUSE_BUTTON_RIGHT)  \
	X(MOUSE_BUTTON_MIDDLE) \

enum KeyCode : uint16_t {
	#define X(name, value) name = value,
		X_ASCII_KEYCODE
	#undef X

	___ = 404, // to not have any duplicates
	#define X(name) name,
		X_COMPLEX_KEYCODE
	#undef X
};

enum KeyState : uint8_t {
	UP       = 0x1,
	PRESSED  = 0x2,
	DOWN     = 0x4,
	RELEASED = 0x8,
};

inline constexpr bool operator&(KeyState lhs, KeyState rhs) {
	return static_cast<bool>((static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs)) != 0);
}

inline constexpr KeyState operator|(KeyState lhs, KeyState rhs) {
	return static_cast<KeyState>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

typedef void (*FUNCTION_CALLBACK)();
struct Profile {
	const char* name;
	FUNCTION_CALLBACK callback;
	bool active;
};

struct Input {
	Vector<Profile> profiles;
	Hashmap<KeyCode, KeyState> input_state;
	void* platform_ctx = nullptr; // GLFW: GLFWwindow* | Winapi: HANDLE

	Vec2 current_mouse = Vec2(0);
	Vec2 previous_mouse = Vec2(0);

	void init(Allocator allocator);
	void poll();
	bool get_key(KeyCode code, KeyState state);
	bool get_key_up(KeyCode code);
	bool get_key_pressed(KeyCode code);
	bool get_key_down(KeyCode code);
	bool get_key_released(KeyCode code);
	void create_profile(const char* key, FUNCTION_CALLBACK callback, bool active = true);
	void delete_profile(const char* key);
	void toggle_profile(const char* key, bool toggle);
	void enable_profile(const char* key);
	void disable_profile(const char* key);
	void update_input_code(KeyCode code, bool down);
	void update_mouse_position(float x, float y);
};