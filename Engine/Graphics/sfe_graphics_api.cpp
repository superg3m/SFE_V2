#include "sfe_graphics_api.hpp"

namespace SFE {
	Shader GraphicsAPI::CreateShader(DS::Vector<const char*> shader_paths) {
		return Shader(shader_paths);
	}
}