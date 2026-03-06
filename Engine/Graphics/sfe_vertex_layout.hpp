#include "../Core/sfe_core.hpp"

struct VertexElement {
	unsigned int index; // vertex shader layout positon index
	unsigned int count;
	unsigned int type; // GL_FLOAT
	unsigned int offset;
};

struct VertexLayout {
	DS::Vector<VertexElement> elements;
	unsigned int stride; 
};