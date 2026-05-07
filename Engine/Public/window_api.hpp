#pragma once

struct WindowAPI {
	int& WINDOW_WIDTH;
	int& WINDOW_HEIGHT;
	int& FRAMEBUFFER_WIDTH;
	int& FRAMEBUFFER_HEIGHT;

	bool& close;
	bool& capture_mouse;

	WindowAPI(
        int&  WINDOW_WIDTH,
        int&  WINDOW_HEIGHT,
        int&  FRAMEBUFFER_WIDTH,
        int&  FRAMEBUFFER_HEIGHT,
        bool& close,
        bool& capture_mouse
    ) :
        WINDOW_WIDTH    (WINDOW_WIDTH),
        WINDOW_HEIGHT   (WINDOW_HEIGHT),
        FRAMEBUFFER_WIDTH (FRAMEBUFFER_WIDTH),
        FRAMEBUFFER_HEIGHT(FRAMEBUFFER_HEIGHT),
        close         (close),
        capture_mouse (capture_mouse)
    {}
};