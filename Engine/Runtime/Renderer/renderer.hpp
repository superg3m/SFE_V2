#pragma once

#include "renderer_api.hpp"
#include "OpenGL/backend.hpp"

template<typename B>
struct Renderer {
	B backend = {};

	static VertexBufferHandle acquire_vbo_handle(void* b) {
		B* backend = (B*)b;
		return backend->vbos.acquire();
	}

	static MeshHandle acquire_mesh_handle(void* b) {
		B* backend = (B*)b; 
		return backend->meshes.acquire();
	}

	static ShaderHandle acquire_shader_handle(void* b) {
		B* backend = (B*)b; 
		return backend->shaders.acquire();
	}

	static MaterialHandle acquire_material_handle(void* b, ShaderHandle shader) {
		B* backend = (B*)b;
		Handle material = backend->materials.acquire();
		Material& material_slot = backend->materials.get(material);
		material_slot = Material(shader);

		return material;
	}

	static TextureHandle acquire_texture_handle(void* b) {
		B* backend = (B*)b; 
		return backend->textures.acquire();
	}

	static void material_set_uniform(void* b, MaterialHandle material, String name, BindingValue value) {
		B* backend = (B*)b; 
		Material& material_slot = backend->materials.get(material.handle);
		material_slot.set_uniform(name, value);
	}

	static void execute_requests(void* b, Vector<Request>& requests, MemoryContext memory) {
		B* backend = (B*)b; 
		backend->execute_requests(requests, memory);
	}

	RenderAPI API(MemoryContext memory) {
		RenderAPI api = {};
		api.b = &this->backend;
		api.memory = memory;
		api._private_acquire_vbo_handle = &acquire_vbo_handle;
		api._private_acquire_mesh_handle = &acquire_mesh_handle;
		api._private_acquire_shader_handle = &acquire_shader_handle;
		api._private_acquire_material_handle = &acquire_material_handle;
		api._private_acquire_texture_handle = &acquire_texture_handle;
		api._private_material_set_uniform = &material_set_uniform;
		api._private_execute_requests = &execute_requests;

		return api;
	}


	// static RenderAPI Vulkan();
	// static RenderAPI Dx12();

	/*


	Vector<Request> requests;
	static Renderer create(Allocator permanent_allocator, Allocator frame_allocator) {
		Renderer ret = {};
		ret.permanent_allocator = permanent_allocator;
		ret.frame_allocator = frame_allocator;
		ret.requests = Vector<Request>(ret.frame_allocator);

		return ret;
	}

	TextureHandle create_texture(u32 texture_unit, String path, TextureDescription& desc) {
		TextureHandle texture = this->backend.textures.acquire();
		Request request = Request::create_texture(texture, texture_unit, path, desc);
		this->requests.append(request);

		return texture;
	}

	TextureHandle create_texture(u32 texture_unit, std::initializer_list<String> cube_map_texture_paths) {
		TextureHandle texture = this->backend.textures.acquire();
		Request request = Request::create_texture(texture, texture_unit, Vector<String>(cube_map_texture_paths, this->frame_allocator));
		this->requests.append(request);

		return texture;
	}

	template<typename T>
	VertexBufferHandle create_vertex_buffer(MeshHandle mesh, VertexLayout layout, Vector<T>& buffer, bool dynamic = false) {
		VertexBufferHandle vbo = this->backend.vbos.acquire();
		Request request = Request::create_vertex_buffer(vbo, mesh, layout, buffer, dynamic);
		this->requests.append(request);

		return vbo;
	}
	
	template<typename T>
	void update_vertex_buffer(MeshHandle mesh, VertexBufferHandle vbo, Vector<T>& buffer, u32 offset = 0) {
		const size_t TOTAL_ALLOCATION_SIZE = sizeof(T) * buffer.count;
		void* frame_temp_data = this->frame_allocator.malloc(TOTAL_ALLOCATION_SIZE, alignof(T)); 
		Memory::copy(frame_temp_data, TOTAL_ALLOCATION_SIZE, buffer.data, TOTAL_ALLOCATION_SIZE);

		// NOTE(Jovanni): ill automatically get freed after the request is processed

		Request request = Request{
			.type = RequestType::VBO_UPDATE,
			.vbo = VertexBufferRequest{
				.user = vbo,
				.mesh = mesh,
				.data = frame_temp_data,
				.count = buffer.count,
				.element_size = sizeof(T),
				.offset = offset
			},
		};
		this->requests.append(request);
	}
	

	IndexBufferHandle create_index_buffer(MeshHandle mesh, Vector<u32>& buffer, bool dynamic = false) {
		IndexBufferHandle ebo = this->backend.ebos.acquire();

		Request request = Request{
			.type = RequestType::VBO_UPDATE,
			.ebo = IndexBufferRequest{
				.user = ebo,
				.mesh = mesh,
				.data = buffer.data,
				.count = buffer.count,
				.dynamic = dynamic
			},
		};
		this->requests.append(request);
		return ebo;
	}

	void bind_vertex_buffer(MeshHandle mesh, VertexBufferHandle vbo) {
		Request request = Request::bind_vertex_buffer(vbo, mesh);
		this->requests.append(request);
	}

	MeshHandle create_mesh(ShaderHandle shader, String path) {
		MeshHandle mesh = this->backend.meshes.acquire();
		Request request = Request::create_mesh(mesh, shader, path);
		this->requests.append(request);
		return mesh;
	}

	MeshHandle create_mesh_cube(MaterialHandle material) {
		MeshHandle mesh = this->backend.meshes.acquire();
		Request request = Request::create_mesh_cube(mesh, material);
		this->requests.append(request);
		return mesh;
	}

	ShaderHandle create_shader(std::initializer_list<String> shader_paths) {
		ShaderHandle shader = this->backend.shaders.acquire();
		Request request = Request::create_shader(shader, Vector<String>(shader_paths, this->permanent_allocator));
		this->requests.append(request);
		return shader;
	}

	MaterialHandle create_material(ShaderHandle shader) {
		MaterialHandle material = this->backend.materials.acquire();
		Material& material_slot = this->backend.materials.get(material.handle);
		material_slot = Material(this->permanent_allocator, shader);

		return material;
	}

	void draw_mesh(Pipeline pipeline, MeshHandle mesh, Mat4 model, Mat4 view, Mat4 projection, u32 instance_count = 1) {
		DrawCallRequest draw_request = {};
		draw_request.pipeline = pipeline;
		draw_request.mesh = mesh;
		draw_request.model = model;
		draw_request.view = view;
		draw_request.projection = projection;
		draw_request.instance_count = instance_count;

		Request request = {};
		request.type = RequestType::DRAW_CALL;
		request.draw_call = draw_request;
		this->requests.append(request);
	}

	void material_set_uniforms(MaterialHandle material, std::initializer_list<KeyValuePair<String, BindingValue>> uniforms) {
		Material& material_slot = this->backend.materials.get(material.handle);
		for (auto& entry : uniforms) {
			String k = entry.key;
			BindingValue v = entry.value;
			material_slot.set_uniform(k, v);
		}
	}

	void shader_recompile(ShaderHandle shader) {
		Request request = Request::shader_recompile(shader);
		this->requests.append(request);
	}
	*/
	
	// TODO(Jovanni): Account for framebuffer
	// CommandBufferHandle begin_frame(u32 framebuffer = 0);
	// void end_frame(CommandBufferHandle handle);
	// void bind_pipeline(CommandBufferHandle cmd_handle, PipelineHandle pipeline_handle);
	// void bind_material(MaterialHandle material_handle);
	// void bind_vertex_buffer(CommandBufferHandle cmd_handle, VertexBuffer vbo);
	// void bind_vertex_buffer(CommandBufferHandle cmd_handle, VertexBufferHandle vbo_handle);
	// void bind_index_buffer(CommandBufferHandle cmd_handle, IndexBuffer ebo);
	// void bind_index_buffer(CommandBufferHandle cmd_handle, IndexBufferHandle ebo_handle);

	// MeshHandle create_mesh(Handle<Material> material_handle, Vector<Vertex>& vertices, Vector<u32> indicies = {});
	// MeshHandle create_mesh(ShaderHandle shader_handle, const char* path);
	// ShaderHandle create_shader(Vector<const char*> shader_paths);
	// void recompile(ShaderHandle shader_handle);
};