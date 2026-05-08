#pragma once

#include "../../Core/core.hpp"
#include "../Types/render_types.hpp"

enum class RequestType {
	MESH_LOAD,
	MESH_CUBE_CREATE,

	TEXTURE2D_LOAD,
	TEXTURE3D_LOAD,
	VBO_CREATE,
	VBO_BIND,
	VBO_UPDATE,
	EBO_UPDATE,

	SHADER_RECOMPILE,
	MATERIAL_SET_UNIFORM,

	DRAW_CALL,
	DRAW_AABB,
	DRAW_SKYBOX,
};

struct TextureRequest {
	String path = {};
	TextureHandle user = TextureHandle::invalid();
	TextureDescription description = {};
	Vector<String> cubemap_paths = {};

	TextureRequest() = default;
	static TextureRequest create(TextureHandle user, String path, TextureDescription description) {
		TextureRequest ret = {};
		ret.user = user;
		ret.path = path;
		ret.description = description;
		
		return ret;
	}

	static TextureRequest create(TextureHandle user, Vector<String> cubemap_paths) {
		TextureRequest ret = {};
		ret.user = user;
		ret.cubemap_paths = cubemap_paths;
		
		return ret;
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
	static VertexBufferRequest create(VertexBufferHandle user, MeshHandle mesh, VertexLayout layout, void* data, int count, size_t element_size, bool dynamic) {
		VertexBufferRequest ret = {};
		ret.user = user;
		ret.mesh = mesh;
		ret.layout = layout;
		ret.data = data;
		ret.count = count;
		ret.element_size = element_size;
		ret.dynamic = dynamic;

		return ret;
	}
 
	static VertexBufferRequest update(VertexBufferHandle user, MeshHandle mesh, void* data, int count, size_t element_size, size_t offset = 0) {
		VertexBufferRequest ret = {};
		ret.user = user;
		ret.mesh = mesh;
		ret.data = data;
		ret.count = count;
		ret.element_size = element_size;
		ret.offset = offset;

		return ret;
	}
};

//  buffer.count * sizeof(T), buffer.data,
struct IndexBufferRequest {
	IndexBufferHandle user = IndexBufferHandle::invalid();
	MeshHandle mesh = MeshHandle::invalid();
	u32* data = nullptr;
	size_t count = 0;
	bool dynamic = false;
};

struct MaterialRequest {
	MaterialHandle user = MaterialHandle::invalid();
	String name = {};
	BindingValue value;

	static MaterialRequest create(MaterialHandle user, String name, BindingValue value) {
		MaterialRequest ret = {};
		ret.user = user;
		ret.name = name;
		ret.value = value;

		return ret;
	}
};

struct MeshRequest {
	MeshHandle user = MeshHandle::invalid();
	MaterialHandle material = MaterialHandle::invalid();
	String path = {};

	static MeshRequest create(MeshHandle user, MaterialHandle material) {
		MeshRequest ret = {};
		ret.user = user;
		ret.material = material;
		
		return ret;
	}

	static MeshRequest create(MeshHandle user, String path) {
		MeshRequest ret = {};
		ret.user = user;
		ret.path = path;
		
		return ret;
	}
};

struct DrawCallRequest {
	MeshHandle mesh = MeshHandle::invalid(); // mesh_entries, vao, vbo, ebo, material
	RasterizerDescription rasterizer_description = {};
	Mat4 model = Mat4::identity();
	int instance_count = 1;

	// for aabb
	static DrawCallRequest create(MeshHandle mesh, Mat4 model, int instance_count = 1) {
		DrawCallRequest ret = {};
		ret.mesh = mesh;
		ret.model = model;
		ret.instance_count = instance_count;

		return ret;
	}

	static DrawCallRequest create(MeshHandle mesh, RasterizerDescription desc, Mat4 model, int instance_count = 1) {
		DrawCallRequest ret = {}; 	
		ret.mesh = mesh;
		ret.rasterizer_description = desc;
		ret.model = model;
		ret.instance_count = instance_count;

		return ret;
	}
};


struct DrawSkyboxRequest {
	MaterialHandle material = MaterialHandle::invalid();

	static DrawSkyboxRequest create(MaterialHandle material) {
		DrawSkyboxRequest ret = {}; 	
		ret.material = material;

		return ret;
	}
};

struct RenderRequest {
	RequestType type;
	union {
		TextureRequest texture;
		VertexBufferRequest vbo;
		IndexBufferRequest ebo;
		MeshRequest mesh;
		MaterialRequest material;
		DrawCallRequest draw_call;
		DrawSkyboxRequest draw_skybox;
	};
};

#define RENDER_REQUEST_TEXTURE2D_LOAD(texture, path, desc) RenderRequest{.type = RequestType::TEXTURE2D_LOAD, .texture = TextureRequest::create(texture, path, desc)}
#define RENDER_REQUEST_TEXTURE3D_LOAD(texture, paths)      RenderRequest{.type = RequestType::TEXTURE3D_LOAD, .texture = TextureRequest::create(texture, paths)}

#define RENDER_REQUEST_VERTEX_BUFFER_CREATE(vbo, mesh, layout, data, count, element_size, dynamic) RenderRequest{.type = RequestType::VBO_CREATE, .vbo = VertexBufferRequest::create(vbo, mesh, layout, data, count, element_size, dynamic)}
#define RENDER_REQUEST_VERTEX_BUFFER_UPDATE(vbo, mesh, data, count, element_size)                  RenderRequest{.type = RequestType::VBO_UPDATE, .vbo = VertexBufferRequest::update(vbo, mesh, data, count, element_size)}

#define RENDER_REQUEST_MESH_LOAD(mesh, path) RenderRequest{.type = RequestType::MESH_LOAD, .mesh = MeshRequest::create(mesh, path)}
#define RENDER_REQUEST_MESH_CREATE_CUBE(mesh, material) RenderRequest{.type = RequestType::MESH_CUBE_CREATE, .mesh = MeshRequest::create(mesh, material)}
#define RENDER_REQUEST_DRAW_CALL(mesh, desc, model, instance_count) RenderRequest{.type = RequestType::DRAW_CALL, .draw_call = DrawCallRequest::create(mesh, desc, model, instance_count)}
#define RENDER_REQUEST_DRAW_AABB(mesh, model) RenderRequest{.type = RequestType::DRAW_AABB, .draw_call = DrawCallRequest::create(mesh, model)}
#define RENDER_REQUEST_DRAW_SKYBOX(texture) RenderRequest{.type = RequestType::DRAW_SKYBOX, .draw_skybox = DrawSkyboxRequest::create(texture)}