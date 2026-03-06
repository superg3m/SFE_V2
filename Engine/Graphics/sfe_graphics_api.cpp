#include "sfe_graphics_api.hpp"
#include "sfe_shader.hpp"
#include "sfe_material.hpp"

namespace SFE {
	Shader* GraphicsAPI::CreateShader(DS::Vector<const char*> shader_paths) {
		return new Shader(shader_paths);
	}

	void GraphicsAPI::BindShaderProgram(Shader* shader) {
		if (shader) {
			shader->use();
		}
	}

	void GraphicsAPI::BindMaterial(Material* material) {
		if (material) {
			material->bind();
		}
	}
}