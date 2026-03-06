#include "../Core/sfe_core.hpp"

namespace SFE {
	struct Shader;
	struct Material;

	struct GraphicsAPI {
		Shader* CreateShader(DS::Vector<const char*> shader_paths);

		void BindShaderProgram(Shader* shader);
		void BindMaterial(Material* material);
	};
}
