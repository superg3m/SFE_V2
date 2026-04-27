#pragma once

#include "../../Core/core.hpp"

// NOTE(Jovanni): this is size in float
enum class BufferStrideTypeInfo {
	BOOL = 1,
	INT = 1,
	FLOAT = 1, 
	VEC2 = 2,
	VEC3 = 3,
	VEC4 = 4, 
	IVEC4 = 4, 
	MAT4 = 16
};

const float MAGIC_NUMBER = -123450510.0f;
const int RESERVED_VERTEX_ATTRIBUTE_LOCATIONS = 3;

struct Vertex {
	Vec3 aPosition    = Vec3(MAGIC_NUMBER);   // location 0
	Vec3 aNormal      = Vec3(MAGIC_NUMBER);   // location 1
	Vec2 aTexCoord    = Vec2(MAGIC_NUMBER);   // location 2
};

struct VertexAttribute {
	u32 location = 0;
	size_t offset = 0; // offset in bytes of the actual memeber! OFFSET_OF(Vertex, aPosition)
	BufferStrideTypeInfo type;
	bool instanced = false;
};

struct DepthState {
	bool depth_testing = true;
	bool depth_write = true;
	// func
};

struct BlendState {
	bool enabled = false;
	// source = src_alpha;
	// destination = one_minus_source_alpha;
};

struct RasterizerState {
	bool cull_enabled = true;
	bool cull_face_back = true;
	bool ccw_winding = true;
	bool fill = true;
};

struct PipelineDescriptor {
	RasterizerState rasterizer;
	DepthState depth;
	BlendState blend;

	PipelineDescriptor(RasterizerState rasterizer, DepthState depth, BlendState blend) {
		this->rasterizer = rasterizer;
		this->depth = depth;
		this->blend = blend;
	}
};

enum class BindingValueType {
	BOOL,
	INTEGER,
	FLOAT,
	TEXTURE_HANDLE,
	SAMPLER_2D,
	CUBEMAP,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	MAT4,
};

/*
struct RenderObject {
    VertexBufferHandle vbo;
    IndexBufferHandle ebo;
    Vector<TextureHandle> albedo; // maybe this is material
    PipelineHandle pipeline;
    Mat4 model;
};
*/

enum class TextureSamplerType {
	SAMPLER_2D,
	CUBEMAP_3D
};

struct TextureDescription {
    bool pixel_perfect = false;
    bool vertical_flip = true;
	TextureSamplerType type = TextureSamplerType::SAMPLER_2D;
};