#pragma once

#include "sfe_shader.hpp"
#include "../Core/sfe_core.hpp"

namespace SFE {
	struct Material {
		void setShaderProgram(Shader* shader);
		void setParam(const char*, float value);
		void bind();

		Shader* shader;
	private:
		
		DS::Hashmap<const char*, float> floatParams;
	};
}
