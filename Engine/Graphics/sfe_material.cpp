#include "sfe_material.hpp"

namespace SFE {
	void Material::setShaderProgram(Shader* shader) {
		this->shader = shader;
	}

	void Material::setParam(const char* name, float value) {
		this->floatParams.put(name, value);
	}

	void Material::bind() {
		if (!this->shader) return;

		this->shader->use();
		for (const auto &entry : this->floatParams) {
			this->shader->setFloat(entry.key, entry.value);
		}
	}
}