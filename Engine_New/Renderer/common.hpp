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

static u32 compute_stride_from_elements(Vector<VertexAttribute>& layout) {
	u32 stride = 0;
	for (VertexAttribute desc : layout) {
		stride += (u32)desc.type * sizeof(float);
	}

	return stride;
}

struct VertexLayout {
	u32 stride;
	u32 stride_in_floats; // stride / sizeof(float)
	Vector<VertexAttribute> attributes;

	/*
	static VertexLayout& PNT() {
        static VertexLayout layout = VertexLayout({
            {OFFSET_OF(Vertex, aPosition), BufferStrideTypeInfo::VEC3},
            {OFFSET_OF(Vertex, aNormal), BufferStrideTypeInfo::VEC3},
            {OFFSET_OF(Vertex, aTexCoord), BufferStrideTypeInfo::VEC2},
        });

        return layout;
    }
	*/

	VertexLayout(Vector<VertexAttribute> attributes) {
        this->stride = 0;
		for (VertexAttribute desc : attributes) {
			this->stride += (u32)desc.type * sizeof(float);
		}

        this->stride_in_floats = this->stride / sizeof(float);
    }
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

	// RasterizerState create();
};

struct PipelineDescriptor {
	VertexLayout layout;
	RasterizerState rasterizer;
	DepthState depth;
	BlendState blend;

	PipelineDescriptor(VertexLayout layout, RasterizerState rasterizer, DepthState depth, BlendState blend) : layout(layout) {
		this->rasterizer = rasterizer;
		this->depth = depth;
		this->blend = blend;
	}
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