
#pragma once

#include "../../Core/core.hpp"

#define X_ASCII_KEYCODE				\
	X(KEY_SPACE,			' ')	\
	X(KEY_EXCLAMATION_MARK, '!')	\
	X(KYE_QUOTE,			'\"')	\
	X(KEY_POUND,			'#')	\
	X(KEY_DOLLAR,			'$')	\
	X(KEY_PERCENT,			'%')	\
	X(AMPERSAND,			'&')	\
	X(KEY_BACKSLASH,		'\'')	\
	X(KEY_LEFT_PAREN,		'(')	\
	X(KEY_RIGHT_PAREN,		')')	\
	X(KEY_STAR,				'*')	\
	X(KEY_PLUS,				'+')	\
	X(KEY_COMMA,			',')	\
	X(KEY_MINUS,			'-')	\
	X(KEY_PERIOD,			'.')	\
	X(KEY_FORWARD_SLASH,	'/')	\
	X(KEY_0,				'0')	\
	X(KEY_1,				'1')	\
	X(KEY_2,				'2')	\
	X(KEY_3,				'3')	\
	X(KEY_4,				'4')	\
	X(KEY_5,				'5')	\
	X(KEY_6,				'6')	\
	X(KEY_7,				'7')	\
	X(KEY_8,				'8')	\
	X(KEY_9,				'9')	\
	X(KEY_COLON,			':')	\
	X(KEY_SEMI_COLON,		';')	\
	X(KEY_LESS_THAN,		'<')	\
	X(KEY_EQUAL,			'=')	\
	X(KEY_GREATER_THAN,		'>')	\
	X(KEY_QUESTION_MARK,	'?')	\
	X(KEY_AT_SYMBOL,		'@')	\
	X(KEY_A,				'A')	\
	X(KEY_B,				'B')	\
	X(KEY_C,				'C')	\
	X(KEY_D,				'D')	\
	X(KEY_E,				'E')	\
	X(KEY_F,				'F')	\
	X(KEY_G,				'G')	\
	X(KEY_H,				'H')	\
	X(KEY_I,				'I')	\
	X(KEY_J,				'J')	\
	X(KEY_K,				'K')	\
	X(KEY_L,				'L')	\
	X(KEY_M,				'M')	\
	X(KEY_N,				'N')	\
	X(KEY_O,				'O')	\
	X(KEY_P,				'P')	\
	X(KEY_Q,				'Q')	\
	X(KEY_R,				'R')	\
	X(KEY_S,				'S')	\
	X(KEY_T,				'T')	\
	X(KEY_U,				'U')	\
	X(KEY_V,				'V')	\
	X(KEY_W,				'W')	\
	X(KEY_X,				'X')	\
	X(KEY_Y,				'Y')	\
	X(KEY_Z,				'Z')	\
	X(KEY_LEFT_BRACKET,		'[')	\
	X(KEY_FORWARDSLASH,		'\\')	\
	X(KEY_RIGHT_BRACKET,	']')	\
	X(KEY_CARROT,			'^')	\
	X(KEY_UNDERSCORE,		'_')	\
	X(KEY_LEFT_CURLY,		'{')	\
	X(KEY_PIPE,				'|')	\
	X(KEY_RIGHT_CURLY,		'}')	\
	X(KEY_TILDA,			'~')	\

#define X_COMPLEX_KEYCODE  \
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

enum KeyCode {
	#define X(name, value) name = value,
		X_ASCII_KEYCODE
	#undef X

	#define X(name) name,
		X_COMPLEX_KEYCODE
	#undef X

	KEY_CODE_COUNT
};

enum KeyState {
	UP       = 0x1,
	PRESSED  = 0x2,
	DOWN     = 0x4,
	RELEASED = 0x8,
};

inline constexpr bool operator&(KeyState lhs, KeyState rhs) {
	return static_cast<bool>((static_cast<u8>(lhs) & static_cast<u8>(rhs)) != 0);
}

inline constexpr KeyState operator|(KeyState lhs, KeyState rhs) {
	return static_cast<KeyState>(static_cast<u8>(lhs) | static_cast<u8>(rhs));
}

struct InputState {
	KeyState keys[KEY_CODE_COUNT] = {UP};

	Vec2 current_mouse = Vec2(0);
	Vec2 previous_mouse = Vec2(0);
};