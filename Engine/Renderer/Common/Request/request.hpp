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
    SHADER_RECOMPILE,

	DRAW_CALL
};

struct TextureRequest {
    TextureHandle user = TextureHandle::invalid();
    u32 texture_unit = INT_MAX;
    const char* path = nullptr;
	TextureDescription description = {};
    Vector<const char*> cubemap_paths = {};

    TextureRequest() = default;

    TextureRequest(TextureHandle user, u32 texture_unit, const char* path, TextureDescription description) {
        this->user = user;
        this->texture_unit = texture_unit;
        this->path = path;
        this->description = description;
    }

    TextureRequest(TextureHandle user, u32 texture_unit, Vector<const char*> cubemap_paths) {
        this->user = user;
        this->texture_unit = texture_unit;
        this->cubemap_paths = cubemap_paths;
    }
};

//  buffer.count * sizeof(T), buffer.data,
struct VertexBufferRequest {
    VertexBufferHandle user = VertexBufferHandle::invalid();

    MeshHandle mesh = MeshHandle::invalid();
    VertexLayout layout = {};
    void* data = nullptr; // make copy and free? would be easiest
    int count = 0;
    size_t element_size = 0; // sizeof(T)
    bool dynamic = false;
    size_t offset = 0;

    VertexBufferRequest() = default;

    template<typename T>
    VertexBufferRequest(VertexBufferHandle user, MeshHandle mesh, VertexLayout layout, Vector<T> buffer, bool dynamic) {
        this->user = user;
        this->mesh = mesh;
        this->layout = layout;
        this->data = buffer.data;
        this->count = buffer.count;
        this->element_size = sizeof(T);
        this->dynamic = dynamic;
    }

    VertexBufferRequest(VertexBufferHandle user, MeshHandle mesh, VertexLayout layout, void* data, int count, size_t element_size, bool dynamic) {
        this->user = user;
        this->mesh = mesh;
        this->layout = layout;
        this->data = data;
        this->count = count;
        this->element_size = element_size;
        this->dynamic = dynamic;
    }

    template<typename T>
    VertexBufferRequest(VertexBufferHandle user, MeshHandle mesh, Vector<T> buffer, size_t offset = 0) {
        this->user = user;
        this->mesh = mesh;
        this->data = buffer.data;
        this->count = buffer.count;
        this->element_size = sizeof(T);
        this->offset = offset;
    }

    VertexBufferRequest(VertexBufferHandle user, MeshHandle mesh, void* data, int count, size_t element_size, size_t offset = 0) {
        this->user = user;
        this->mesh = mesh;
        this->data = data;
        this->count = count;
        this->element_size = element_size;
        this->offset = offset;
    }
};

//  buffer.count * sizeof(T), buffer.data,
struct IndexBufferRequest {
    MeshHandle mesh = MeshHandle::invalid();
    u32* data = nullptr;
    size_t count = 0;
    bool dynamic = false;
};

struct DrawCallRequest {
	Pipeline pipeline = {};
	MeshHandle mesh = MeshHandle::invalid(); // mesh_entries, vao, vbo, ebo, material
	Mat4 model = Mat4::identity();
	Mat4 view = Mat4::identity();
	Mat4 projection = Mat4::identity();
	int instance_count = 1;
};

struct MeshRequest {
    MeshHandle user = MeshHandle::invalid();
    MaterialHandle material = MaterialHandle::invalid();
    ShaderHandle shader = ShaderHandle::invalid();
    const char* path = nullptr;

    MeshRequest(MeshHandle user, MaterialHandle material) {
        this->user = user;
        this->material = material;
    }

    MeshRequest(MeshHandle user, ShaderHandle shader, const char* path) {
        this->user = user;
        this->shader = shader;
        this->path = path;
    }
};

struct ShaderRequest {
    ShaderHandle user;
    Vector<const char*> shader_paths = {};
};


// TODO(Jovanni): instead of user call it self?

// NOTE(Jovanni): user.handle means it will get updated by the backend
struct Request {
	RequestType type;
	union {
		TextureRequest texture;
        VertexBufferRequest vbo;
        MeshRequest mesh;
        ShaderRequest shader;
		DrawCallRequest draw_call;
	};

    static Request create_texture(TextureHandle user, u32 texture_unit, const char* path, TextureDescription description) {
        Request ret = {};
        ret.type = RequestType::TEXTURE2D_LOAD;
        ret.texture = TextureRequest(user, texture_unit, path, description);

        return ret;
    }

    static Request create_texture(TextureHandle user, u32 texture_unit, Vector<const char*> cubemap_paths) {
        Request ret = {};
        ret.type = RequestType::TEXTURE3D_LOAD;
        ret.texture = TextureRequest(user, texture_unit, cubemap_paths);

        return ret;
    }

    template<typename T>
    static Request create_vertex_buffer(VertexBufferHandle user, MeshHandle mesh, VertexLayout layout, Vector<T>& buffer, bool dynamic = false) {
        Request ret = {};
        ret.type = RequestType::TEXTURE3D_LOAD;
        ret.texture = VertexBufferRequest(user, layout, mesh, buffer, dynamic);

        return ret;
    }

    static Request create_mesh(MeshHandle user, ShaderHandle shader, const char* path) {
        Request ret = {};
        ret.type = RequestType::MESH_LOAD;
        ret.mesh = MeshRequest(user, shader, path);

        return ret;
    }

    static Request create_mesh_cube(MeshHandle user, MaterialHandle material) {
        Request ret = {};
        ret.type = RequestType::MESH_CUBE_CREATE;
        ret.mesh = ret.mesh = MeshRequest(user, material);

        return ret;
    }

    static Request create_shader(ShaderHandle user, Vector<const char*> shader_paths) {
        Request ret = {};
        ret.type = RequestType::SHADER_CREATE;
        ret.shader.user = user;
        ret.shader.shader_paths = shader_paths;

        return ret;
    }

    static Request shader_recompile(ShaderHandle user) {
        Request ret = {};
        ret.type = RequestType::SHADER_RECOMPILE;
        ret.shader.user = user;

        return ret;
    }
};