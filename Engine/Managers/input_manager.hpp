#pragma once

#include <core.hpp>

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

typedef void (*CALLBACK)();
struct Profile {
    const char* name;
    CALLBACK callback;
    bool active;
};

struct InputManager {
    std::vector<Profile> profiles;
    std::unordered_map<KeyCode, KeyState> input_state;
    void* platform_ctx; // GLFW: GLFWwindow* | Winapi: HANDLE

    glm::vec2 current_mouse;
    glm::vec2 previous_mouse;

    void init() {
        this->input_state = {
            #define X(name, value) {name, KeyState::UP},
                X_ASCII_KEYCODE
            #undef X

            #define X(name) {name, KeyState::UP},
                X_COMPLEX_KEYCODE
            #undef X
        };
    }

    void poll() {
        for (const auto [key, value] : input_state) {
            KeyCode code = key;
            KeyState state = value;

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

    bool get_key(KeyCode code, KeyState state) {
        if (!input_state.count(code)) {
            LOG_WARN("Pressed a key and it is not mapped yet: %c\n", code);
            return false;
        }

        KeyState actual_state = input_state[code];
        return state & actual_state;
    }

    bool get_key_up(KeyCode code) {
        return get_key(code, KeyState::UP);
    }
    bool get_key_pressed(KeyCode code) {
        return get_key(code, KeyState::PRESSED);
    }
    bool get_key_down(KeyCode code) {
        return get_key(code, KeyState::DOWN);
    }
    bool get_key_released(KeyCode code) {
        return get_key(code, KeyState::RELEASED);
    }

    void create_profile(const char* key, CALLBACK callback, bool active = true)  {
        Profile ret;
        ret.name = key;
        ret.callback = callback;
        ret.active = active;
        profiles.push_back(ret);
    }

    void delete_profile(const char* key)  {
        for (int i = 0; i < profiles.size(); i++) {
            Profile profile = profiles[i];
            if (strcmp(profile.name, key) == 0) {
                profiles.erase(this->profiles.begin() + i);
                return;
            }
        }

        LOG_DEBUG("Could not find profile: %s\n", key);
    }

    void toggle_profile(const char* key, bool toggle)  {
        for (int i = 0; i < profiles.size(); i++) {
            Profile* profile = &profiles[i];
            if (strcmp(profile->name, key) == 0) {
                profile->active = toggle;
                return;
            }
        }

        LOG_DEBUG("Could not find profile: %s\n", key);
    }

    void enable_profile(const char* key)  {
        for (int i = 0; i < profiles.size(); i++) {
            Profile* profile = &profiles[i];
            if (strcmp(profile->name, key) == 0) {
                profile->active = true;
                return;
            }
        }

        LOG_DEBUG("Could not find profile: %s\n", key);
    }

    void disable_profile(const char* key) {
        for (int i = 0; i < profiles.size(); i++) {
            Profile* profile = &profiles[i];
            if (strcmp(profile->name, key) == 0) {
                profile->active = false;
                return;
            }
        }

        LOG_DEBUG("Could not find profile: %s\n", key);
    }

    void update_input_code(KeyCode code, bool down) {
        KeyState state = input_state[code];
        if (down) {
            KeyState new_state = (state == KeyState::UP || state == KeyState::RELEASED) ? KeyState::PRESSED : KeyState::DOWN;
            input_state[code] = new_state;
        } else {
            KeyState new_state = (state == KeyState::DOWN || state == KeyState::PRESSED) ? KeyState::RELEASED : KeyState::UP;
            input_state[code] = new_state; 
        }
    }

    void update_mouse_position(float x, float y) {
        this->previous_mouse = this->current_mouse;
        this->current_mouse = glm::vec2(x, y);
    }
};