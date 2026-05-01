#include "../Core/core.hpp"

#if defined(PLATFORM_WINDOWS)
	#include "platform_win32.cpp"
#else
	#include "platform_unix.cpp"
#endif

#include "platform_glfw.cpp"