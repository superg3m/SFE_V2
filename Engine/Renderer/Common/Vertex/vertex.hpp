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
const int RESERVED_VERTEX_ATTRIBUTE_LOCATIONS = 4;

struct Vertex {
	Vec3 aPosition    = Vec3(MAGIC_NUMBER);   // location 0
	Vec3 aNormal      = Vec3(MAGIC_NUMBER);   // location 1
	Vec2 aTexCoord    = Vec2(MAGIC_NUMBER);   // location 2
	Vec3 aColor       = Vec3(MAGIC_NUMBER);   // location 3
};

struct VertexAttribute {
	u32 location = 0;
	size_t offset = 0; // offset in bytes of the actual memeber! OFFSET_OF(Vertex, aPosition)
	BufferStrideTypeInfo type;
	bool instanced = false;
};

struct VertexLayout {
	u32 stride;
	u32 stride_in_floats; // stride / sizeof(float)
	Vector<VertexAttribute> attributes;

	static VertexLayout& PNTC() {
		static VertexLayout layout = VertexLayout({
			VertexAttribute{0, OFFSET_OF(Vertex, aPosition), BufferStrideTypeInfo::VEC3, false},
			VertexAttribute{1, OFFSET_OF(Vertex, aNormal), BufferStrideTypeInfo::VEC3, false},
			VertexAttribute{2, OFFSET_OF(Vertex, aTexCoord), BufferStrideTypeInfo::VEC2, false},
			VertexAttribute{3, OFFSET_OF(Vertex, aColor), BufferStrideTypeInfo::VEC3, false},
		});

		return layout;
	}

	VertexLayout() = default;
	VertexLayout(Vector<VertexAttribute> attributes) {
		this->stride = 0;
		for (VertexAttribute desc : attributes) {
			this->stride += (u32)desc.type * sizeof(float);
		}

		this->stride_in_floats = this->stride / sizeof(float);
		this->attributes = attributes;
	}
};