#pragma once

#include "../Core/sfe_core.hpp"

namespace SFE {
	struct VertexElement {
		unsigned int index; // vertex shader layout positon index
		unsigned int count; // the number of types so if count = 3 then its gonna be 3 floats if type is GL_FLOAT
		unsigned int type; // GL_FLOAT
		u64 offset;
	};

	struct VertexLayout {
		DS::Vector<VertexElement> elements;
		u64 stride; 
	};
}