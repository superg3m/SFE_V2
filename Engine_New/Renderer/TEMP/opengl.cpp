#include "opengl.hpp"
#include <glad/glad.h>

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

OpenGL::Pipeline OpenGL::Pipeline::create(PipelineDescriptor& desc) {
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	for (VertexAttribute attribute : desc.layout.attributes) {
		bind_vertex_attribute(attribute.location, desc.layout.stride, attribute);
	}

	glBindVertexArray(0);
}

void bind_pipeline(OpenGL::Pipeline& p) {
	glUseProgram(p.program);

	// 2. Raster state
	if (p.raster.cullEnabled) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	} else {
		glDisable(GL_CULL_FACE);
	}

	glFrontFace(GL_CCW);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// 3. Depth state
	if (p.depth.testEnabled) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	} else {
		glDisable(GL_DEPTH_TEST);
	}

	glDepthMask(p.depth.writeEnabled ? GL_TRUE : GL_FALSE);

	// 4. Blend state
	if (p.blend.enabled) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glDisable(GL_BLEND);
	}

	// 5. (Optional) bind VAO if you cached one
	if (p.vao != 0) {
		glBindVertexArray(p.vao);
	}
}

void OpenGL::CommandBuffer::bind_pipeline(PipelineDescriptor pipeline) {}
void OpenGL::CommandBuffer::bind_vertex_buffer(Handle<VertexBuffer> vbo) {}
void OpenGL::CommandBuffer::bind_index_buffer(Handle<IndexBuffer> ebo) {}
void OpenGL::CommandBuffer::draw_vertices(u32 vertex_base, u32 vertex_count) {}
void OpenGL::CommandBuffer::draw_indexed(u32 index_base, u32 index_count) {}

OpenGL::CommandBuffer OpenGL::CommandBuffer::begin_frame(u32 framebuffer) {
	OpenGL::CommandBuffer cmd = {};
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	return cmd;
}

void OpenGL::CommandBuffer::end_frame() {
	
}

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