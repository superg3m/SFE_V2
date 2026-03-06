#include "sfe_material.hpp"

namespace SFE {
	void Material::setShaderProgram(Shader* shader) {
		this->shader = shader;
	}

	void Material::setParam(const char* name, float value) {
		this->floatParams.put(name, value);
	}

	void Material::bind() {
		for (const auto &entry : this->floatParams) {
			this->shader->setFloat(entry.key, entry.value);
		}
	}
}