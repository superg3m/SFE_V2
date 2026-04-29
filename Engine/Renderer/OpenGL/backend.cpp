#include "backend.hpp"

#include "pipeline.cpp"
#include "texture.cpp"
#include "command_buffer.cpp"
#include "shader.cpp"
#include "mesh.cpp"

#include <float.h>

void _GL_ERROR_CHECK(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        const char* error = nullptr;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        
        RUNTIME_ASSERT_MSG(false, "%s | %s:%d\n", error, file, line);
    }
}

void bind_vertex_attribute(int location, u32 stride, VertexAttribute attribute) {
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

/*
void OpenGL::draw(Vector<RenderObject>& objects) {
	OpenGL::CommandBuffer cmd = renderer.BeginFrame();

	for (auto& obj : objects) {
		cmd.BindPipeline(obj.pipeline);
		cmd.BindVertexBuffer(obj.vbo);
		cmd.BindIndexBuffer(obj.ibo);
		cmd.BindTexture(0, obj.albedo);
		cmd.SetUniform("uModel", obj.model);

		cmd.DrawIndexed(36);
	}

	renderer.EndFrame(cmd);
}
*/