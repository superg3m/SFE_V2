#pragma once

#include "../Core/core.hpp"
#include "../Runtime/Input/input_api.hpp"
#include "../Runtime/WindowCreation/window_creation.hpp"
#include "../Runtime/Renderer/renderer_api.hpp"

struct Engine {
	MemoryContext memory = {};
	InputState input = {};
	RenderAPI renderer = {};
	Window window = {};
	bool reloaded_dll = false;
	void* application_state = nullptr;
};