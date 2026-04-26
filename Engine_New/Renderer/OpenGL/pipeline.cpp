#include "backend.hpp"

INTERNAL_LINKAGE void bind_vertex_attribute(int location, u32 stride, VertexAttribute attribute) {
	bool is_integer = (attribute.type == BufferStrideTypeInfo::INT) || (attribute.type == BufferStrideTypeInfo::IVEC4);
	bool is_matrix = attribute.type == BufferStrideTypeInfo::MAT4;
	GLenum gl_type  = is_integer ? GL_INT : GL_FLOAT;

	int max_attributes = 0;
	gl_error_check(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes));
	RUNTIME_ASSERT_MSG(max_attributes > location, "Location outside range");

	if (is_matrix) {
		for (int i = 0; i < 4; i++) {
			RUNTIME_ASSERT_MSG(max_attributes > location + i,
				"You are trying to use a location that is outside of the max vertex attributes: %d",
				max_attributes
			);

			gl_error_check(glEnableVertexAttribArray(location + i));
			gl_error_check(glVertexAttribPointer(location + i, (int)BufferStrideTypeInfo::VEC4, GL_FLOAT, false, stride, (void*)(attribute.offset + (sizeof(Vec4) * i))));
			gl_error_check(glVertexAttribDivisor(location + i, attribute.instanced));
		}
	} else {
		gl_error_check(glEnableVertexAttribArray(location));
		if (is_integer) {
			gl_error_check(glVertexAttribIPointer(location, (int)attribute.type, gl_type, stride, (void*)attribute.offset));
		} else {
			gl_error_check(glVertexAttribPointer(location, (int)attribute.type, gl_type, false, stride, (void*)attribute.offset));
		}

		gl_error_check(glVertexAttribDivisor(location, attribute.instanced));
	}
}

OpenGL::Pipeline OpenGL::Pipeline::create(PipelineDescriptor desc) {
	OpenGL::Pipeline ret = {};

	glGenVertexArrays(1, &ret.vao);
	glBindVertexArray(ret.vao);

	for (VertexAttribute attribute : desc.layout.attributes) {
		bind_vertex_attribute(attribute.location, desc.layout.stride, attribute);
	}

	glBindVertexArray(0);

	return ret;
}