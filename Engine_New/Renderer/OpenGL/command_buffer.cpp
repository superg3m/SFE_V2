#include "backend.hpp"

void OpenGL::CommandBuffer::bind_pipeline(OpenGL::Pipeline pipeline) {
	if (pipeline.rasterizer.cull_enabled) {
		gl_error_check(glEnable(GL_CULL_FACE));
		gl_error_check(glCullFace(pipeline.rasterizer.cull_face_back ? GL_BACK : GL_FRONT));
	} else {
		gl_error_check(glDisable(GL_CULL_FACE));
	}

	gl_error_check(glFrontFace(pipeline.rasterizer.ccw_winding ? GL_CCW : GL_CW));
	gl_error_check(glPolygonMode(GL_FRONT_AND_BACK, pipeline.rasterizer.fill ? GL_FILL : GL_LINE));

	if (pipeline.depth.depth_testing) {
		gl_error_check(glEnable(GL_DEPTH_TEST));
		gl_error_check(glDepthFunc(GL_LESS));
	} else {
		gl_error_check(glDisable(GL_DEPTH_TEST));
	}

	gl_error_check(glDepthMask(pipeline.depth.depth_write ? GL_TRUE : GL_FALSE));

	if (pipeline.blend.enabled) {
		gl_error_check(glEnable(GL_BLEND));
		gl_error_check(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	} else {
		gl_error_check(glDisable(GL_BLEND));
	}
}

void OpenGL::CommandBuffer::bind_vertex_buffer(OpenGL::VertexBuffer vbo) {
	gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, vbo.id));
}

void OpenGL::CommandBuffer::bind_index_buffer(IndexBuffer ebo) {
	gl_error_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.id));
}

void OpenGL::CommandBuffer::draw_vertices(u32 vertex_base, u32 vertex_count) {}
void OpenGL::CommandBuffer::draw_indexed(u32 index_base, u32 index_count) {}

void OpenGL::CommandBuffer::begin_frame(u32 framebuffer) {
	// glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	gl_error_check(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT));
	gl_error_check(glClearColor(0.5f, 0.2f, 0.2f, 1));
}

void OpenGL::CommandBuffer::end_frame() {
	
}