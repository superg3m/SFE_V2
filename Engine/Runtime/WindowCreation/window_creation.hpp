#pragma once

#include "../../Public/window_api.hpp"

struct Window {
	void* ctx = nullptr;
	int WINDOW_WIDTH = 0;
	int WINDOW_HEIGHT = 0;
	int FRAMEBUFFER_WIDTH = 0;
	int FRAMEBUFFER_HEIGHT = 0;

	bool close = false;
	bool capture_mouse = false;
	
	static Window create(const int WIDTH, const int HEIGHT, const char* name, bool vsync = true);
	bool should_close();
	void pump_messages();

	WindowAPI API() {
		return WindowAPI(
			this->WINDOW_WIDTH,
			this->WINDOW_HEIGHT,
			this->FRAMEBUFFER_WIDTH,
			this->FRAMEBUFFER_HEIGHT,
			this->close, 
			this->capture_mouse
		);
	}
};