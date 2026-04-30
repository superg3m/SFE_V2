#pragma once

#include <core.hpp>

namespace OpenGL {
	struct RenderState {
		s32 BOUND_VAO_ID = -1;
		s32 BOUND_PROGRAM_ID = -1;
		bool DEPTH_TEST = false;
		bool STENCIL = false;
		bool BLENDING = false;
		bool WIREFRAME = false;
		int DRAW_CALL_COUNT = 0;

		void bind_vao_or_used_cached(u32 incoming_vao_id);
		void bind_shader_program_or_use_cached(u32 incoming_shader_program_id);
		void set_depth_test_or_use_cached(bool incoming_depth_test);
		void set_stencil_test_or_use_cached(bool incoming_sencil_test);
		void set_blending_or_use_cached(bool incoming_blending);
		void set_wireframe_or_use_cached(bool incoming_wireframe);
	};
}