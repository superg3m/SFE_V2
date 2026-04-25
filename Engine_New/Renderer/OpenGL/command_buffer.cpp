#include "backend.hpp"



void OpenGL::CommandBuffer::bind_pipeline(OpenGL& backend, Handle<OpenGL::Shader> shader, Handle<OpenGL::Pipeline> pipeline_handle) {
	Pipeline& p = backend.pipelines.get(pipeline_handle);
	gl_error_check(glUseProgram(p.shader_program));

	/*
	if (p.raster.cull_enabled) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	} else {
		glDisable(GL_CULL_FACE);
	}
	*/

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// 3. Depth state
	if (p.depth.depth_testing) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	} else {
		glDisable(GL_DEPTH_TEST);
	}

	glDepthMask(p.depth.depth_write ? GL_TRUE : GL_FALSE);

	// 4. Blend state
	if (p.blend.enabled) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		glDisable(GL_BLEND);
	}

	glBindVertexArray(p.vao);
}
void OpenGL::CommandBuffer::bind_vertex_buffer(Handle<VertexBuffer> vbo) {}
void OpenGL::CommandBuffer::bind_index_buffer(Handle<IndexBuffer> ebo) {}
void OpenGL::CommandBuffer::draw_vertices(u32 vertex_base, u32 vertex_count) {}
void OpenGL::CommandBuffer::draw_indexed(u32 index_base, u32 index_count) {}

void OpenGL::CommandBuffer::begin_frame(u32 framebuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void OpenGL::CommandBuffer::end_frame() {
	
}