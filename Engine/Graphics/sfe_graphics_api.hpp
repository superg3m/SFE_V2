#include "sfe_shader.hpp"

namespace SFE {
	struct GraphicsAPI {
		Shader CreateShader(DS::Vector<const char*> shader_paths);
	};
}
