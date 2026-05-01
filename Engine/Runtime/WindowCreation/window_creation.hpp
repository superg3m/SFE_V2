#include "../../Core/core.hpp"

struct Window {
	void* ctx;
	static void create(const int WIDTH, const int HEIGHT, const char* name, bool vsync = true);
	bool should_close();
	void close();
};