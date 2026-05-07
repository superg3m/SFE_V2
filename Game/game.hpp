#pragma once

#include <Public/api.hpp>

struct AppState {
	VertexBufferHandle instance_cube_vbo = VertexBufferHandle::invalid();
	MaterialHandle material = MaterialHandle::invalid();
	MeshHandle cube_mesh = MeshHandle::invalid();
	MeshHandle backpack_mesh = MeshHandle::invalid();

	TextureHandle container_texture = TextureHandle::invalid();
	TextureHandle face_texture = TextureHandle::invalid();

	TextureHandle skybox_day = TextureHandle::invalid();
	TextureHandle skybox_night = TextureHandle::invalid();
	MaterialHandle skybox_material = MaterialHandle::invalid();
	
	float sky_blend = 0.0f; // 0 .. 1 day or night
	float accumulator = 0.0;
	Vector<Mat4> cube_translations;

	Timer timer = {};

	// editor
};