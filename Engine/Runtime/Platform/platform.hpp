#pragma once

#include "../Core/core.hpp"
#include "platform_api.hpp"

struct PlatformSystem {
	bool init();
	void shutdown();

	Allocator get_allocator();
};