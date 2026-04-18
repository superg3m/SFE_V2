/*
#if defined(_WIN32) || defined(WIN32)
    #include <WIN32_IOD.hpp>

    static const std::unordered_map<WPARAM, IOD_InputCode> Win32ToIODInputCode = {
        { 'A', IOD_KEY_A }, { 'B', IOD_KEY_B }, { 'C', IOD_KEY_C }, { 'D', IOD_KEY_D },
        { 'E', IOD_KEY_E }, { 'F', IOD_KEY_F }, { 'G', IOD_KEY_G }, { 'H', IOD_KEY_H },
        { 'I', IOD_KEY_I }, { 'J', IOD_KEY_J }, { 'K', IOD_KEY_K }, { 'L', IOD_KEY_L },
        { 'M', IOD_KEY_M }, { 'N', IOD_KEY_N }, { 'O', IOD_KEY_O }, { 'P', IOD_KEY_P },
        { 'Q', IOD_KEY_Q }, { 'R', IOD_KEY_R }, { 'S', IOD_KEY_S }, { 'T', IOD_KEY_T },
        { 'U', IOD_KEY_U }, { 'V', IOD_KEY_V }, { 'W', IOD_KEY_W }, { 'X', IOD_KEY_X },
        { 'Y', IOD_KEY_Y }, { 'Z', IOD_KEY_Z },

        { '0', IOD_KEY_0 }, { '1', IOD_KEY_1 }, { '2', IOD_KEY_2 }, { '3', IOD_KEY_3 },
        { '4', IOD_KEY_4 }, { '5', IOD_KEY_5 }, { '6', IOD_KEY_6 }, { '7', IOD_KEY_7 },
        { '8', IOD_KEY_8 }, { '9', IOD_KEY_9 },

        { VK_SPACE,      IOD_KEY_SPACE },
        { VK_RETURN,     IOD_KEY_ENTER },
        { VK_ESCAPE,     IOD_KEY_ESCAPE },
        { VK_TAB,        IOD_KEY_TAB },
        { VK_BACK,       IOD_KEY_BACKSPACE },

        { VK_LEFT,       IOD_KEY_LEFT },
        { VK_RIGHT,      IOD_KEY_RIGHT },
        { VK_UP,         IOD_KEY_UP },
        { VK_DOWN,       IOD_KEY_DOWN },

        { VK_CONTROL,    IOD_KEY_CTRL },
        { VK_SHIFT,      IOD_KEY_SHIFT },
        { VK_MENU,       IOD_KEY_ALT },  // VK_MENU == Alt key

        { VK_F1,  IOD_KEY_F1 },  { VK_F2,  IOD_KEY_F2 },  { VK_F3,  IOD_KEY_F3 },
        { VK_F4,  IOD_KEY_F4 },  { VK_F5,  IOD_KEY_F5 },  { VK_F6,  IOD_KEY_F6 },
        { VK_F7,  IOD_KEY_F7 },  { VK_F8,  IOD_KEY_F8 },  { VK_F9,  IOD_KEY_F9 },
        { VK_F10, IOD_KEY_F10 }, { VK_F11, IOD_KEY_F11 }, { VK_F12, IOD_KEY_F12 }
    };


    LRESULT CALLBACK IOD_Win32InputProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        bool found = Win32ToIODInputCode.count(wParam);
        switch (msg) {
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN: {
                if (found) {
                    IOD::UpdateInputCode(Win32ToIODInputCode.at(wParam), true);
                }
            } break;
            case WM_KEYUP:
            case WM_SYSKEYUP: {
                if (found) {
                    IOD::UpdateInputCode(Win32ToIODInputCode.at(wParam), false);
                }
            } break;

            case WM_LBUTTONDOWN: {
                IOD::UpdateInputCode(IOD_MOUSE_BUTTON_LEFT, true);
            } break;
                
            case WM_LBUTTONUP: {
                IOD::UpdateInputCode(IOD_MOUSE_BUTTON_LEFT, false);
            } break;
        }
        
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
#endif
*/