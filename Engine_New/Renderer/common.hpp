#pragma once

#include "../../Core/core.hpp"

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

struct VertexAttribute {
	size_t offset = 0; // offset in bytes of the actual memeber! OFFSET_OF(Vertex, aPosition)
	u32 location;
	BufferStrideTypeInfo type;
	bool instanced = false;
};

struct VertexLayout {
	u32 stride;
	u32 stride_in_floats; // stride / sizeof(float)
	Vector<VertexAttribute> attributes;

	static VertexLayout& PNT();
	static VertexLayout create(Vector<VertexAttribute> attributes);
};

struct DepthState {
	bool depth_testing = true;
	bool depth_write = true;
	// func
};

struct BlendState {
	bool enabled = true;
	// source = src_alpha;
	// destination = one_minus_source_alpha;
};

struct RasterizerState {
	// bool enabled = true;
	// bool cull_face = Back;
	// bool front_face = CCW;
	// bool fill = true;

	RasterizerState create();
};

struct PipelineDescriptor {
	VertexLayout layout;
	RasterizerState rasterizer;
	DepthState depth;
	BlendState blend;
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