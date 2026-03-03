#include "../Core/sfe_core.hpp"
#include <glad/glad.h>

struct OpenGL_API {
	Random::Seed seed;
	Math::Mat4 test;

	PFNGLCLEARCOLORPROC clearColor;
	PFNGLCLEARPROC clear;
};