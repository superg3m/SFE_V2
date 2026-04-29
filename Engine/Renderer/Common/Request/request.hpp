#pragma once

#include "../Handle/handle.hpp"
#include "../Texture/texture.hpp"
#include "../Vertex/vertex.hpp"

enum class RequestType {
	MESH_LOAD,
	MESH_CUBE_CREATE,
	MESH_AABB_CREATE,

	TEXTURE2D_LOAD,
	TEXTURE3D_LOAD,
	VBO_CREATE,
	VBO_UPDATE,
	EBO_UPDATE,

    SHADER_CREATE,

	DRAW_CALL
};

struct TextureRequest {
    u32 texture_unit = INT_MAX;
    const char* path = nullptr;
	TextureDescription description = {};
    Vector<const char*> cubemap_paths = {};
};

//  buffer.count * sizeof(T), buffer.data,
struct VertexBufferRequest {
    MeshHandle mesh_handle = MeshHandle::invalid();
    VertexLayout layout = {};
    void* data = nullptr; // make copy and free? would be easiest
    size_t count = 0;
    size_t element_size = 0; // sizeof(T)
    bool dynamic = false;
    size_t offset = 0;
};

//  buffer.count * sizeof(T), buffer.data,
struct IndexBufferRequest {
    MeshHandle mesh_handle = MeshHandle::invalid();
    u32* data = nullptr;
    size_t count = 0;
    bool dynamic = false;
};

struct DrawCallRequest {
	Pipeline pipeline;
	MeshHandle mesh; // mesh_entries, vao, vbo, ebo, material
	Mat4 Model;
	int instance_count;
};

struct MeshRequest {
    MaterialHandle material = MaterialHandle::invalid();
    const char* path = nullptr;
};

struct ShaderRequest {
    Vector<const char*> shader_paths = {};
};

struct Request {
	RequestType type;
	union {
		TextureRequest texture;
        VertexBufferRequest vbo;
        MeshRequest mesh;
        ShaderRequest shader;
		DrawCallRequest draw_call;
	};

	Handle handle;
};