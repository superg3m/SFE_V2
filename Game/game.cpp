#include <sfe.hpp>

extern "C" __declspec(dllexport) void clear_color(OpenGL_API* gl) {
	gl->test = Math::Mat4::Scale(Math::Mat4::Identity(), Random::GenerateRange(&gl->seed, 2, 10));

	gl->clearColor(0.2f, 0.8f, 0.2f, 1.0f);
	gl->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}