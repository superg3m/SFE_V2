#pragma once

#include "../../Core/core.hpp"

struct Texture;
struct Shader;
struct Pipeline;
struct VertexBuffer;
struct IndexBuffer;

using ShaderRegistry  = Registry<Shader, 256>;
using TextureRegistry = Registry<Texture, 256>;
using PipelineRegistry = Registry<Pipeline, 256>;
using VertexBufferRegistry = Registry<VertexBuffer, 256>;
using IndexBufferRegistry = Registry<IndexBuffer, 256>;

using TextureHandle = Handle<Texture>;
using ShaderHandle = Handle<Shader>;
using PipelineHandle = Handle<Pipeline>;
using VertexBufferHandle = Handle<VertexBuffer>;
using IndexBufferHandle = Handle<IndexBuffer>;

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

// TODO(Jovanni): COMMOMN 
struct VertexAttribute {
	u32 offset = 0; // offset in bytes of the actual memeber! OFFSET_OF(Vertex, aPosition)
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
	ShaderHandle vertex;
	ShaderHandle fragment;
	VertexLayout layout;

	RasterizerState rasterizer;
	DepthState depth;
	BlendState blend;
};

struct RenderObject {
    VertexBufferHandle vbo;
    IndexBufferHandle ebo;
    Vector<TextureHandle> albedo; // maybe this is material
    PipelineHandle pipeline;
    Mat4 model;
};

struct CommandBuffer  {
	void bind_pipeline(PipelineDescriptor pipeline);
	void bind_vertex_buffer(Handle<VertexBuffer> vbo);
	void bind_index_buffer(Handle<IndexBuffer> ebo);
	void draw_vertices(u32 vertex_base, u32 vertex_count);
	void draw_indexed(u32 index_base, u32 index_count);
};

void _GL_ERROR_CHECK(const char* file, int line);

#define ENABLE_GL_ERROR_CHECK
#if defined(ENABLE_GL_ERROR_CHECK)
    #define gl_error_check(glCall) glCall; _GL_ERROR_CHECK(__FILE__, __LINE__) 
#else
    #define gl_error_check(glCall) glCall
#endif