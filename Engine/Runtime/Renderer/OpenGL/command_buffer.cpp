#include "backend.hpp"

void OpenGL::CommandBuffer::bind_rasterizer_description(RasterizerDescription desc) {
	if (desc.cull_enabled) {
		gl_error_check(glEnable(GL_CULL_FACE));
		gl_error_check(glCullFace(desc.cull_face_back ? GL_BACK : GL_FRONT));
	} else {
		gl_error_check(glDisable(GL_CULL_FACE));
	}

	gl_error_check(glFrontFace(desc.ccw_winding ? GL_CCW : GL_CW));
	gl_error_check(glPolygonMode(GL_FRONT_AND_BACK, desc.wireframe ? GL_LINE : GL_FILL));
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
	gl_error_check(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer)); // TODO(Jovanni): CACHE THIS

	gl_error_check(glClearColor(0.2f, 0.2f, 0.2f, 1));
	gl_error_check(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT));
}

void OpenGL::CommandBuffer::end_frame() {
	
}