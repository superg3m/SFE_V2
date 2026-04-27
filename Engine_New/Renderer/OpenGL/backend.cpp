#include "backend.hpp"

#include "pipeline.cpp"
#include "texture.cpp"
#include "command_buffer.cpp"
#include "shader.cpp"
#include "material.cpp"

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

AABB calculate_aabb(Vector<Vertex>& vertices, int base_vertex, int vertex_count) {
	float x_min = FLT_MAX;
	float y_min = FLT_MAX;
	float z_min = FLT_MAX;

	float x_max = FLT_MIN;
	float y_max = FLT_MIN;
	float z_max = FLT_MIN;
	for (int i = base_vertex; i < base_vertex + vertex_count; i++) {
		const Vertex v = vertices[i];
		float x = v.aPosition.x;
		float y = v.aPosition.y;
		float z = v.aPosition.z;

		if (x_min > x) {
			x_min = x;
		} else if (x_max < x) {
			x_max = x;
		}

		if (y_min > y) {
			y_min = y;
		} else if (y_max < y) {
			y_max = y;
		}

		if (z_min > z) {
			z_min = z;
		} else if (z_max < z) {
			z_max = z;
		}
	}

	Vec3 center  = Vec3(
		(x_max + x_min) / 2.0f,
		(y_max + y_min) / 2.0f,
		(z_max + z_min) / 2.0f
	);

	Vec3 extents = Vec3(
		(x_max - x_min) / 2.0f,
		(y_max - y_min) / 2.0f,
		(z_max - z_min) / 2.0f
	);

	return AABB::from_center_extents(center, extents);
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