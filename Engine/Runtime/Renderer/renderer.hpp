#pragma once

#include "../../Public/api.hpp"
#include "OpenGL/backend.hpp"

template<typename B>
struct Renderer {
	MemoryContext memory;
	B backend;

	static Renderer<B> create(MemoryContext memory) {
		Renderer<B> ret = {};
		ret.memory = memory;
		ret.backend = B::create();

		return ret;
	}

	typename B::Texture& get(TextureHandle texture) {
		return this->backend.textures.get(texture.handle);
	}

	typename B::Mesh& get(MeshHandle mesh) {
		return this->backend.meshes.get(mesh.handle);
	}

	Material& get(MaterialHandle material) {
		return this->backend.materials.get(material.handle);
	}

	static VertexBufferHandle acquire_vbo_handle(void* b) {
		B* backend = (B*)b;
		return backend->vbos.acquire();
	}

	static MeshHandle acquire_mesh_handle(void* b) {
		B* backend = (B*)b; 
		return backend->meshes.acquire();
	}

	static MaterialHandle acquire_material_handle(void* b) {
		B* backend = (B*)b;
		Handle material = backend->materials.acquire();

		return material;
	}

	static TextureHandle acquire_texture_handle(void* b) {
		B* backend = (B*)b; 
		return backend->textures.acquire();
	}

	RenderAPI API() {
		return RenderAPI(this->memory, &this->backend, &this->backend.materials, &acquire_vbo_handle, &acquire_mesh_handle, &acquire_texture_handle);
	}

	void sync(RenderAPI* api) {
		// this->backend.execute_defered_request(api.deferred_requests);
	}

	void execute_deferred_requests(EngineAPI* api) {
		this->backend.execute_defered_request(api);
		api->renderer.deferred_requests.clear();
	}
};