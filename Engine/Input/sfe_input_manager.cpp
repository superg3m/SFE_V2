#include <algorithm>
#include "sfe_input_manager.hpp"
#include "../engine.hpp"

namespace SFE {
    void InputManager::UpdateInputCode(KeyCode code, bool down) {
        KeyState state = this->input_state.get(code);
        if (down) {
            KeyState new_state = (state == KeyState::UP || state == KeyState::RELEASED) ? KeyState::PRESSED : KeyState::DOWN;
            this->input_state.put(code, new_state);
        } else {
            KeyState new_state = (state == KeyState::DOWN || state == KeyState::PRESSED) ? KeyState::RELEASED : KeyState::UP;
            this->input_state.put(code, new_state); 
        }
    }

    void InputManager::UpdateMousePosition(float x, float y) {
        this->mouse_x = x;
        this->mouse_y = y;
    }

    float InputManager::GetMouseX() { 
        return this->mouse_x; 
    }

    float InputManager::GetMouseY() { 
        return this->mouse_y; 
    }

    bool InputManager::Initialize(GLFWwindow* window) {
        this->profiles = DS::Vector<Profile>(1);
        this->input_state = {
            #define X(name, value) {name, KeyState::UP},
                X_ASCII_KEYCODE
            #undef X

            #define X(name) {name, KeyState::UP},
                X_COMPLEX_KEYCODE
            #undef X
        };

        return this->GLFW_SETUP(window);
    }

    void InputManager::Poll() {
        for (const auto entry : input_state) {
            if (!entry.filled || entry.dead) continue;

            KeyCode code = entry.key;
            KeyState state = entry.value;

            if (state == KeyState::PRESSED) {
                this->UpdateInputCode(code, true);
            } else if (state == KeyState::RELEASED) {
                this->UpdateInputCode(code, false);
            }
        }

        for (const auto profile : profiles) {
            if (!profile.active) {
                continue;
            }

            if (profile.callback) {
                profile.callback();
            }
        }
    }

    bool InputManager::GetKey(KeyCode code, KeyState state) {
        if (!input_state.has(code)) {
            LOG_WARN("Pressed a key and it is not mapped yet: %c\n", code);
            return false;
        }

        KeyState actual_state = input_state.get(code);
        return state & actual_state;
    }

    bool InputManager::GetKeyUp(KeyCode code) {
        return GetKey(code, KeyState::UP);
    }

    bool InputManager::GetKeyPressed(KeyCode code) {
        return GetKey(code, KeyState::PRESSED);
    }

    bool InputManager::GetKeyDown(KeyCode code) {
        return GetKey(code, KeyState::DOWN);
    }

    bool InputManager::GetKeyReleased(KeyCode code) {
        return GetKey(code, KeyState::RELEASED);
    }

    void InputManager::CreateProfile(const char* key, CALLBACK callback, bool active) {
        Profile ret;
        ret.name = key;
        ret.callback = callback;
        ret.active = active;
        profiles.push(ret);
    }

    void InputManager::DeleteProfile(const char* key) {
        for (int i = 0; i < profiles.count(); i++) {
            Profile profile = profiles[i];
            if (String::Equal(profile.name, key)) {
                profiles.unstableSwapbackRemove(i);
                return;
            }
        }

        LOG_DEBUG("Could not find profile: %s\n", key);
    }

    void InputManager::ToggleProfile(const char* key, bool toggle) {
        for (int i = 0; i < profiles.count(); i++) {
            Profile* profile = &profiles[i];
            if (String::Equal(profile->name, key)) {
                profile->active = toggle;
                return;
            }
        }

        LOG_DEBUG("Could not find profile: %s\n", key);
    }

    void InputManager::EnableProfile(const char* key) {
        for (int i = 0; i < profiles.count(); i++) {
            Profile* profile = &profiles[i];
            if (String::Equal(profile->name, key)) {
                profile->active = true;
                return;
            }
        }

        LOG_DEBUG("Could not find profile: %s\n", key);
    }

    void InputManager::DisableProfile(const char* key) {
        for (int i = 0; i < profiles.count(); i++) {
            Profile* profile = &profiles[i];
            if (String::Equal(profile->name, key)) {
                profile->active = false;
                return;
            }
        }

        LOG_DEBUG("Could not find profile: %s\n", key);
    }

    bool InputManager::GLFW_SETUP(GLFWwindow* window) {
        this->glfw_to_key_code = {
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
        };
        
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            InputManager& input = Engine::GetInstance().GetInputManager();

            if (input.cb_keyboard) {
                input.cb_keyboard(window, key, scancode, action, mods);  
            }

            if (!input.glfw_to_key_code.has(key)) {
                LOG_WARN("Pressed a glfw key and it is not mapped yet: %s\n", glfwGetKeyName(key, scancode));

                return;
            }

            KeyCode cb_code = input.glfw_to_key_code.get(key);
            input.UpdateInputCode(cb_code, action != GLFW_RELEASE);
            for (const auto profile : input.profiles) {
                if (!profile.active) {
                    continue;
                }

                if (profile.callback) {
                    profile.callback();
                }
            }
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            InputManager& input = Engine::GetInstance().GetInputManager();

            if (input.cb_mouse_button) {
                input.cb_mouse_button(window, button, action, mods);   
            }

            KeyCode cb_code = input.glfw_to_key_code.get(button);
            input.UpdateInputCode(cb_code, action != GLFW_RELEASE);
            for (const auto profile : input.profiles) {
                if (!profile.active) {
                    continue;
                }

                if (profile.callback) {
                    profile.callback();
                }
            }
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
            InputManager& input = Engine::GetInstance().GetInputManager();

            input.UpdateMousePosition(static_cast<float>(xpos), static_cast<float>(ypos));
            if (input.cb_mouse_move) {
                input.cb_mouse_move(window, xpos, ypos);
            }
        });

        return true;
    }
}