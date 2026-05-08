#pragma once

#include "../Core/core.hpp"
#include "Requests/render_requests.hpp"
#include "Types/render_types.hpp"
// TODO(Jovanni): I shoulsn't let you create shaders anymore really it should be premade shaders and 
// the only thing you change is hte material.
/* 
MaterialDescription(
	ShaderType::PBR,
	{"albedo": texture} 
)
*/

typedef VertexBufferHandle(AcquireVertexBufferFunc)(void* b);
// typedef MeshHandle(AcquireMeshFunc)(void* b);
typedef ModelHandle(AcquireModelFunc)(void* b);
typedef TextureHandle(AcquireTextureFunc)(void* b);

// TODO(Jovanni): instead of user call it self?
// NOTE(Jovanni): user.handle means it will get updated by the backend

struct RenderAPI {
	void* b = nullptr;

	MemoryContext memory = {};
	Vector<RenderRequest> deferred_requests = {};
	Registry<Material, 256>* materials = {};
	AcquireVertexBufferFunc* _private_acquire_vbo_handle = nullptr;
    // AcquireMeshFunc*         _private_acquire_mesh_handle = nullptr;
    AcquireModelFunc*        _private_acquire_model_handle = nullptr;
    AcquireTextureFunc*      _private_acquire_texture_handle = nullptr;

	RenderAPI(MemoryContext memory, void* b, Registry<Material, 256>* materials, AcquireVertexBufferFunc* _private_acquire_vbo_handle, AcquireModelFunc* _private_acquire_model_handle, AcquireTextureFunc* _private_acquire_texture_handle) {
		this->b = b;
		this->memory = memory;
		this->materials = materials;
		this->_private_acquire_vbo_handle = _private_acquire_vbo_handle;
		// this->_private_acquire_mesh_handle = _private_acquire_mesh_handle;
		this->_private_acquire_model_handle = _private_acquire_model_handle;
		this->_private_acquire_texture_handle = _private_acquire_texture_handle;
	}

	template<typename T>
	VertexBufferHandle create_vbo(MeshHandle mesh, VertexLayout layout, Vector<T>& buffer, bool dynamic = false) {
		VertexBufferHandle vbo = this->_private_acquire_vbo_handle(this->b);
		RenderRequest request = RENDER_REQUEST_VERTEX_BUFFER_CREATE(vbo, mesh, layout, buffer.data, buffer.count, sizeof(T), dynamic);
		this->deferred_requests.append(request);

		return vbo;
	}

	/*
	void bind_vbo(VertexBufferHandle vbo, MeshHandle mesh) {
		RenderRequest request = RENDER_REQUEST_BIND_VERTEX_BUFFER RenderRequest::bind_vertex_buffer(vbo, mesh);
		this->requests.append(request);
	}
	*/

	/*
	MeshHandle create_mesh_cube(MaterialHandle material) {
		MeshHandle mesh = this->_private_acquire_mesh_handle(this->b);
		RenderRequest request = RENDER_REQUEST_MESH_CREATE_CUBE(mesh, material);
		this->deferred_requests.append(request);

		return mesh;
	}

	MeshHandle create_mesh_aabb(MaterialHandle material) {
		MeshHandle mesh = this->_private_acquire_mesh_handle(this->b);
		RenderRequest request = RENDER_REQUEST_MESH_CREATE_CUBE(mesh, material);
		this->deferred_requests.append(request);

		return mesh;
	}
	*/

	/*
	void shader_recompile(ShaderHandle shader) {
		RenderRequest request = RENDER_REQUSTREC RenderRequest::shader_recompile(shader);
		this->requests.append(request);
	}
	*/

	ModelHandle create_model(String path, TextureDescription desc = {}) {
		ModelHandle model = this->_private_acquire_model_handle(this->b);
		RenderRequest request = RENDER_REQUEST_MODEL_LOAD(model, path, desc);
		this->deferred_requests.append(request);
		
		return model;
	}

	TextureHandle create_texture(String path, TextureDescription description = {}) {
		TextureHandle texture = this->_private_acquire_texture_handle(this->b);
		RenderRequest request = RENDER_REQUEST_TEXTURE2D_LOAD(texture, path, description);
		this->deferred_requests.append(request);
		return texture;
	}


	// {right, left, top, bottom, front, back}
	TextureHandle create_texture(Vector<String> paths) {
		TextureHandle texture = this->_private_acquire_texture_handle(this->b);
		RenderRequest request = RENDER_REQUEST_TEXTURE3D_LOAD(texture, paths);
		this->deferred_requests.append(request);
		return texture;
	}


	Material& create_material(MaterialType type = MaterialType::PBR) {
		MaterialHandle material = this->materials->acquire();
		Material& material_slot = this->materials->get(material.handle);
		material_slot = Material::create(material, this->memory.permanent_allocator, type);

		return material_slot;
	}

	void draw_mesh(MeshHandle mesh, RasterizerDescription desc, Mat4 model, u32 instance_count = 1) {
		RenderRequest request = RENDER_REQUEST_DRAW_CALL(mesh, desc, model, instance_count);
		this->deferred_requests.append(request);
	}

	/*
	void draw_aabb(MeshHandle mesh, Mat4 model) {
		RenderRequest request = RENDER_REQUEST_DRAW_AABB(mesh, model);
		this->deferred_requests.append(request);
	}

	void draw_mesh(MeshHandle mesh, RasterizerDescription desc, Mat4 model, u32 instance_count = 1) {
		RenderRequest request = RENDER_REQUEST_DRAW_CALL(mesh, desc, model, instance_count);
		this->deferred_requests.append(request);
	}

	void draw_skybox(MaterialHandle material) {
		RenderRequest request = RENDER_REQUEST_DRAW_SKYBOX(material);
		this->deferred_requests.append(request);
	}
	*/
};