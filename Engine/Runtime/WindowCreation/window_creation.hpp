#pragma once

struct Window {
	void* ctx = nullptr;
	bool close = false;
	int WINDOW_WIDTH = 0;
	int WINDOW_HEIGHT = 0;
	int FRAMEBUFFER_WIDTH = 0;
	int FRAMEBUFFER_HEIGHT = 0;
	bool capture_mouse = false;
	
	static Window create(const int WIDTH, const int HEIGHT, const char* name, bool vsync = true);
	bool should_close();
	void pump_messages();
};