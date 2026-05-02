#pragma once

#include <API/api.hpp>
#include <Runtime/ECS/ecs.hpp>

struct AppState {
	ShaderHandle cube_shader = ShaderHandle::invalid();
	MaterialHandle material = MaterialHandle::invalid();
	VertexBufferHandle instance_cube_vbo = VertexBufferHandle::invalid();
	MeshHandle cube_mesh = MeshHandle::invalid();

	ShaderHandle backpack_shader = ShaderHandle::invalid();
	MeshHandle backpack_mesh = MeshHandle::invalid();

	TextureHandle container_texture = TextureHandle::invalid();
	TextureHandle face_texture = TextureHandle::invalid();

	Pipeline opaque_pipeline = {};
	Pipeline opaque_wireframe_pipeline = {};
	bool use_opaque_pipeline = true;
	float accumulator = 0.0;

	Vector<Mat4> cube_translations;

	Timer timer = {};

	// editor
	bool show_demo_window = true;
};