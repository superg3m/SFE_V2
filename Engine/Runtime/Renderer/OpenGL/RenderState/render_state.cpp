#include <render_state.hpp>
#include <glad/glad.h>
#include <gl_error_check.hpp>

namespace OpenGL {
	void RenderState::bind_vao_or_used_cached(u32 incoming_vao_id) {
		if (this->BOUND_VAO_ID == incoming_vao_id) {
			return;
		}

		BOUND_VAO_ID = incoming_vao_id;
		gl_error_check(glBindVertexArray(incoming_vao_id));
	}

	void RenderState::bind_shader_program_or_use_cached(u32 incoming_shader_program_id) {
		if (this->BOUND_PROGRAM_ID == incoming_shader_program_id) {
			return;
		}

		BOUND_PROGRAM_ID = incoming_shader_program_id;
		gl_error_check(glUseProgram(incoming_shader_program_id));
	}

	void RenderState::set_depth_test_or_use_cached(bool incoming_depth_test) {
		if (this->DEPTH_TEST == incoming_depth_test) {
			return;
		}

		this->DEPTH_TEST = incoming_depth_test;
		if (DEPTH_TEST) {
			gl_error_check(glEnable(GL_DEPTH_TEST));
		} else {
			gl_error_check(glDisable(GL_DEPTH_TEST));
		}
	}
	
	void RenderState::set_stencil_test_or_use_cached(bool incoming_sencil_test) {
		if (this->STENCIL == incoming_sencil_test) {
			return;
		}

		this->STENCIL = incoming_sencil_test;
		if (this->STENCIL) {
			gl_error_check(glEnable(GL_STENCIL_TEST));
		} else {
			gl_error_check(glDisable(GL_STENCIL_TEST));
		}
	}

	void RenderState::set_blending_or_use_cached(bool incoming_blending) {
		if (this->BLENDING == incoming_blending) {
			return;
		}

		this->BLENDING = incoming_blending;
		if (this->BLENDING) {
			gl_error_check(glEnable(GL_BLEND));
		} else {
			gl_error_check(glDisable(GL_BLEND));
		}
	}

	void RenderState::set_wireframe_or_use_cached(bool incoming_wireframe) {
		if (this->WIREFRAME == incoming_wireframe) {
			return;
		}

		this->WIREFRAME = incoming_wireframe;
		GLenum mode = this->WIREFRAME ? GL_LINE : GL_FILL;
		gl_error_check(glPolygonMode(GL_FRONT_AND_BACK, mode));
	}
}
