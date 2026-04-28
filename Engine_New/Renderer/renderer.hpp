#pragma once

#include "common.hpp"
#include "OpenGL/backend.hpp"

using B = OpenGL;
using Texture = typename B::Texture;
using TextureHandle = Handle<Texture>;

using VertexBuffer = typename B::VertexBuffer;
using VertexBufferHandle = Handle<VertexBuffer>;

using IndexBuffer = typename B::IndexBuffer;
using IndexBufferHandle = Handle<IndexBuffer>;

using CommandBuffer = typename B::CommandBuffer;
using CommandBufferHandle = Handle<CommandBuffer>;

using Pipeline = typename B::Pipeline;
using PipelineHandle = Handle<Pipeline>;

using Shader = typename B::Shader;
using ShaderHandle = Handle<Shader>;

using Material = typename B::Material;
using MaterialHandle = Handle<Material>;

using Mesh = typename B::Mesh;
using MeshHandle = Handle<Mesh>;

using MeshEntry = typename B::MeshEntry;

using VertexArrayObject = typename B::VertexArrayObject;
using VertexArrayObjectHandle = Handle<VertexArrayObject>;

using BindingValue = typename B::BindingValue;

struct RenderCommand {
	PipelineHandle pipeline;
    MaterialHandle material;
	VertexArrayObject vao;
	VertexBuffer main_vbo;
	Vector<VertexBufferHandle> extra_vbos;
	IndexBuffer ebo;
	MeshEntry mesh_entry;
	Mat4 model = Mat4::identity();
	Mat4 view = Mat4::identity();
	Mat4 projection = Mat4::identity();
    u32 instance_count = 1;
};

struct Renderer {
	PipelineHandle opaque_pipeline;
	PipelineHandle opaque_wireframe_pipeline;
	PipelineHandle shadow_pipeline;
	PipelineHandle post_effects_pipeline;
	PipelineHandle skybox_pipeline;
	PipelineHandle transparent_pipeline;

    Vector<RenderCommand> opaque_commands;
	Vector<RenderCommand> opaque_wireframe_commands;
	Vector<RenderCommand> shadow_commands;
	Vector<RenderCommand> post_effects_commands;
	Vector<RenderCommand> skybox_commands;
	Vector<RenderCommand> transparent_commands;
 
    static Renderer create(Allocator permanent_allocator, Allocator frame_allocator) {
        Renderer ret = {};

		PipelineDescriptor opaque_pipeline_descriptor = PipelineDescriptor(
			RasterizerState{
				.cull_enabled = false,
				.cull_face_back = true,
				.ccw_winding = true,
				.fill = true
			}, 
			DepthState{

			}, 
			BlendState{
				.enabled = false
			}
		);

		PipelineDescriptor opaque_wireframe_pipeline_descriptor = PipelineDescriptor(
			RasterizerState{
				.cull_enabled = false,
				.cull_face_back = true,
				.ccw_winding = true,
				.fill = false
			}, 
			DepthState{

			}, 
			BlendState{
				.enabled = false
			}
		);

		/*
		PipelineDescriptor pipeline_particle_description = PipelineDescriptor(
			VertexLayout({
				{4, pipeline_description.layout.stride, BufferStrideTypeInfo::VEC3, true},
				{5, pipeline_description.layout.stride + sizeof(Vec3),   BufferStrideTypeInfo::VEC3, true},
			}),
			RasterizerState(), 
			DepthState(), 
			BlendState()
		);
		*/;

		ret.opaque_pipeline = ret.create_pipeline(opaque_pipeline_descriptor);
		ret.opaque_wireframe_pipeline = ret.create_pipeline(opaque_wireframe_pipeline_descriptor);
		// this->shadow_pipeline = renderer.create_pipeline();
		// this->post_effects_pipeline = renderer.create_pipeline(pipeline_description);
		// this->skybox_pipeline = renderer.create_pipeline(pipeline_description);
		// this->transparent_pipeline = renderer.create_pipeline(pipeline_description);

		ret.opaque_commands = Vector<RenderCommand>(frame_allocator);
		ret.shadow_commands = Vector<RenderCommand>(frame_allocator);
		ret.post_effects_commands = Vector<RenderCommand>(frame_allocator);
		ret.skybox_commands = Vector<RenderCommand>(frame_allocator);
		ret.transparent_commands = Vector<RenderCommand>(frame_allocator);

        return ret;
	}

	void submit(RenderCommand command) {
		if (command.pipeline == this->opaque_pipeline) {
			this->opaque_commands.append(command);
		} else if (command.pipeline == this->opaque_wireframe_pipeline) {
			this->opaque_wireframe_commands.append(command);
		} else if (command.pipeline == this->shadow_pipeline) {
			this->shadow_commands.append(command);
		} else if (command.pipeline == this->post_effects_pipeline) {
			this->post_effects_commands.append(command);
		} else if (command.pipeline == this->skybox_pipeline) {
			this->skybox_commands.append(command);
		} else if (command.pipeline == this->transparent_pipeline) {
			this->transparent_commands.append(command);
		} else {
			RUNTIME_ASSERT(false);
		}
	}

    void draw(Allocator& temp_allocator) {
		// Mat4 view = Mat4::identity(); // camera.get_view_matrix();
		// Mat4 projection = Mat4::identity(); // camera.get_view_matrix();

		CommandBufferHandle cmd = this->begin_frame();
			this->bind_pipeline(cmd, this->opaque_pipeline);
			for (RenderCommand& command : this->opaque_commands) {
				this->bind_vertex_array(cmd, command.vao);
				this->bind_vertex_buffer(cmd, command.main_vbo);
                for (VertexBufferHandle vbo : command.extra_vbos) {
			        this->bind_vertex_buffer(cmd, vbo);
                }
				this->bind_index_buffer(cmd, command.ebo);
				this->material_set_uniforms(command.material, Hashmap<const char*, BindingValue>({
					{"uModel", command.model},
					{"uView", command.view},
					{"uProjection", command.projection},
				}, temp_allocator));
				this->draw_mesh_entry(cmd, command.vao, command.mesh_entry, command.instance_count);
			}

			this->bind_pipeline(cmd, this->opaque_wireframe_pipeline);
			for (RenderCommand& command : this->opaque_wireframe_commands) {
				this->bind_vertex_array(cmd, command.vao);
				this->bind_vertex_buffer(cmd, command.main_vbo);
                for (VertexBufferHandle vbo : command.extra_vbos) {
			        this->bind_vertex_buffer(cmd, vbo);
                }
				this->bind_index_buffer(cmd, command.ebo);
				this->material_set_uniforms(command.material, Hashmap<const char*, BindingValue>({
					{"uModel", command.model},
					{"uView", command.view},
					{"uProjection", command.projection},
				}, temp_allocator));
				this->draw_mesh_entry(cmd, command.vao, command.mesh_entry, command.instance_count);
			}
		this->end_frame(cmd);

		/*
		cmd = renderer.begin_frame();
			renderer.bind_pipeline(cmd, this->shadow_pipeline);
			for (RenderCommand& command : this->shadow_commands) {
				renderer.bind_vertex_buffer(cmd, command.vbo);
				renderer.bind_index_buffer(cmd, command.ebo);
				MeshEntry& mesh_entry = renderer.backend.mesh_entries.get(command.mesh_entry_handle);
				renderer.material_set_mat4(mesh_entry.material_handle, Hashmap<const char*, Mat4>({
					{"uModel", command.model},
					{"uView", view},
					{"uProjection", projection},
				}, allocator));
				renderer.draw_mesh_entry(command.mesh_entry_handle);
			}
		renderer.end_frame(cmd);

		cmd = renderer.begin_frame();
			renderer.bind_pipeline(cmd, this->post_effects_pipeline);
			for (RenderCommand& command : this->post_effects_commands) {
				renderer.bind_vertex_buffer(cmd, command.vbo);
				renderer.bind_index_buffer(cmd, command.ebo);
				MeshEntry& mesh_entry = renderer.backend.mesh_entries.get(command.mesh_entry_handle);
				renderer.material_set_mat4(mesh_entry.material_handle, Hashmap<const char*, Mat4>({
					{"uModel", command.model},
					{"uView", view},
					{"uProjection", projection},
				}, allocator));
				renderer.draw_mesh_entry(command.mesh_entry_handle);
			}
		renderer.end_frame(cmd);

		cmd = renderer.begin_frame();
			renderer.bind_pipeline(cmd, this->skybox_pipeline);
			for (RenderCommand& command : this->skybox_commands) {
				Mat4 skybox_view = view;
				skybox_view.v[0].w = 0.0f;
				skybox_view.v[1].w = 0.0f;
				skybox_view.v[2].w = 0.0f;

				renderer.bind_vertex_buffer(cmd, command.vbo);
				renderer.bind_index_buffer(cmd, command.ebo);
				MeshEntry& mesh_entry = renderer.backend.mesh_entries.get(command.mesh_entry_handle);
				renderer.material_set_mat4(mesh_entry.material_handle, Hashmap<const char*, Mat4>({
					{"uModel", command.model},
					{"uView", skybox_view},
					{"uProjection", projection},
				}, allocator));
				renderer.draw_mesh_entry(command.mesh_entry_handle);
			}
		renderer.end_frame(cmd);

		cmd = renderer.begin_frame();
			renderer.bind_pipeline(cmd, this->transparent_pipeline);
			for (RenderCommand& command : this->transparent_commands) {
				Mat4 skybox_view = view;
				skybox_view.v[0].w = 0.0f;
				skybox_view.v[1].w = 0.0f;
				skybox_view.v[2].w = 0.0f;

				renderer.bind_vertex_buffer(cmd, command.vbo);
				renderer.bind_index_buffer(cmd, command.ebo);
				MeshEntry& mesh_entry = renderer.backend.mesh_entries.get(command.mesh_entry_handle);
				renderer.material_set_mat4(mesh_entry.material_handle, Hashmap<const char*, Mat4>({
					{"uModel", command.model},
					{"uView", skybox_view},
					{"uProjection", projection},
				}, allocator));
				renderer.draw_mesh_entry(command.mesh_entry_handle);
			}
		renderer.end_frame(cmd);
		*/
		
		this->opaque_commands.clear();
		this->opaque_wireframe_commands.clear();
		this->shadow_commands.clear();
		this->post_effects_commands.clear();
		this->skybox_commands.clear();
		this->transparent_commands.clear();
	}

    B backend;

    TextureHandle create_texture(u32 texture_unit, const char* path, TextureDescription& desc) {
        TextureHandle handle = backend.textures.acquire();
        Texture& texture = backend.textures.get(handle);
        texture = Texture::load_from_file(texture_unit, path, desc);

        return handle;
    }

    TextureHandle create_texture(u32 texture_unit, Vector<const char*> cube_map_texture_paths) {
        TextureHandle handle = backend.textures.acquire();
        Texture& texture = backend.textures.get(handle);
        texture = Texture::load_cube_map(texture_unit, cube_map_texture_paths);

        return handle;
    }

    void bind_vertex_array(CommandBufferHandle cmd_handle, VertexArrayObject vao) {
        CommandBuffer& cmd = backend.command_buffers.get(cmd_handle);
        cmd.bind_vertex_array(vao);
    }

    void bind_vertex_array(CommandBufferHandle cmd_handle, VertexArrayObjectHandle vao_handle) {
        CommandBuffer& cmd = backend.command_buffers.get(cmd_handle);
        VertexArrayObject& vao = backend.vaos.get(vao_handle);
        cmd.bind_vertex_array(vao);
    }

    VertexArrayObjectHandle create_vertex_array_object() {
        VertexArrayObjectHandle handle = backend.vaos.acquire();
        VertexArrayObject& vao = backend.vaos.get(handle);
        vao = VertexArrayObject::create();

        return handle;
    }

    template<typename T>
    VertexBufferHandle create_vertex_buffer(MeshHandle mesh_handle, VertexLayout layout, Vector<T>& buffer, bool dynamic = false) {
        Mesh& mesh = backend.meshes.get(mesh_handle);
        VertexBufferHandle handle = backend.vbos.acquire();
        VertexBuffer& vbo = backend.vbos.get(handle);
        vbo = VertexBuffer::create(mesh.vao, layout, buffer, dynamic);

        return handle;
    }

    template<typename T>
    VertexBufferHandle create_vertex_buffer(VertexArrayObjectHandle vao_handle, VertexLayout layout, Vector<T>& buffer, bool dynamic = false) {
        VertexArrayObject& vao = backend.vaos.get(vao_handle);
        VertexBufferHandle handle = backend.vbos.acquire();
        VertexBuffer& vbo = backend.vbos.get(handle);
        vbo = VertexBuffer::create(vao, layout, buffer, dynamic);

        return handle;
    }

    
    template<typename T>
    void update_vertex_buffer(VertexArrayObject vao, VertexBufferHandle vbo_handle, Vector<T>& data, u32 offset = 0) {
        VertexBuffer& vbo = backend.vbos.get(vbo_handle);
        vbo.update_buffer(vao, data, offset);
    }

    template<typename T>
    void update_vertex_buffer(VertexArrayObjectHandle vao_handle, VertexBufferHandle handle, Vector<T>& data, u32 offset = 0) {
        VertexArrayObject& vao = backend.vaos.get(vao_handle);
        VertexBuffer& vbo = backend.vbos.get(handle);
        vbo.update_buffer(vao, data, offset);
    }

    IndexBufferHandle create_index_buffer(VertexArrayObjectHandle vao_handle, Vector<u32>& buffer, bool dynamic = false) {
        VertexArrayObject& vao = backend.vaos.get(vao_handle);
        IndexBufferHandle handle = backend.ebos.acquire();
        IndexBuffer& ebo = backend.ebos.get(handle);
        ebo = IndexBuffer::create(vao, buffer, dynamic);

        return handle;
    }

    MaterialHandle create_material(ShaderHandle shader_handle) {
        MaterialHandle handle = backend.materials.acquire();
        Material& material = backend.materials.get(handle);
        material = Material(shader_handle);

        return handle;
    }

    void material_set_uniforms(MaterialHandle handle, Hashmap<const char*, BindingValue> bindings) {
        Material& material = backend.materials.get(handle);
        for (auto entry : bindings) {
            const char* key = entry.key;
            BindingValue value = entry.value;

            if (value.type == BindingValueType::TEXTURE_HANDLE) {
                Texture& texture = this->backend.textures.get(value.texture_handle);
                material.set_uniform(key, texture);
            } else {
                material.set_uniform(key, value);
            }
        }
    }

    // TODO(Jovanni): Account for framebuffer
    CommandBufferHandle begin_frame(u32 framebuffer = 0) {
        CommandBufferHandle handle = backend.command_buffers.acquire();
        CommandBuffer& cmd = backend.command_buffers.get(handle);
        cmd.begin_frame();

        return handle;
    }

    void end_frame(CommandBufferHandle handle) {
        CommandBuffer& cmd = backend.command_buffers.get(handle);
        cmd.end_frame();

        backend.command_buffers.release(handle);
    }

    void bind_pipeline(CommandBufferHandle cmd_handle, PipelineHandle pipeline_handle) {
        CommandBuffer& cmd = backend.command_buffers.get(cmd_handle);
        Pipeline& pipeline = backend.pipelines.get(pipeline_handle);

        cmd.bind_pipeline(pipeline);
    }

    void bind_material(MaterialHandle material_handle) {
        RUNTIME_ASSERT(material_handle != MaterialHandle::invalid());

        Material& material = backend.materials.get(material_handle);
        Shader& shader = backend.shaders.get(material.shader_handle);
        shader.use();
        shader.set_material(&material);
    }

    void bind_vertex_buffer(CommandBufferHandle cmd_handle, VertexBuffer vbo) {
        CommandBuffer& cmd = backend.command_buffers.get(cmd_handle);
        cmd.bind_vertex_buffer(vbo);
    }

    void bind_vertex_buffer(CommandBufferHandle cmd_handle, VertexBufferHandle vbo_handle) {
        CommandBuffer& cmd = backend.command_buffers.get(cmd_handle);
        VertexBuffer& vbo = backend.vbos.get(vbo_handle);
        cmd.bind_vertex_buffer(vbo);
    }

    void bind_index_buffer(CommandBufferHandle cmd_handle, IndexBuffer ebo) {
        CommandBuffer& cmd = backend.command_buffers.get(cmd_handle);
        cmd.bind_index_buffer(ebo);
    }

    void bind_index_buffer(CommandBufferHandle cmd_handle, IndexBufferHandle ebo_handle) {
        CommandBuffer& cmd = backend.command_buffers.get(cmd_handle);
        IndexBuffer& ebo = backend.ebos.get(ebo_handle);
        cmd.bind_index_buffer(ebo);
    }

    PipelineHandle create_pipeline(PipelineDescriptor desc) {
        PipelineHandle handle = backend.pipelines.acquire();
        Pipeline& pipeline = backend.pipelines.get(handle);
        pipeline = Pipeline::create(desc);

        return handle;
    }

    MeshHandle create_mesh_cube(Handle<Material> material_handle) {
        MeshHandle handle = backend.meshes.acquire();
        Mesh& mesh = backend.meshes.get(handle);
        mesh = Mesh::cube(material_handle);

        return handle;
    }

    MeshHandle create_mesh_aabb(Handle<Material> material_handle) {
        MeshHandle handle = backend.meshes.acquire();
        Mesh& mesh = backend.meshes.get(handle);
        mesh = Mesh::axis_aligned_bounding_box(material_handle);

        return handle;
    }

    MeshHandle create_mesh_aabb(Handle<Material> material_handle, AABB aabb) {
        MeshHandle handle = backend.meshes.acquire();
        Mesh& mesh = backend.meshes.get(handle);
        mesh = Mesh::axis_aligned_bounding_box(material_handle, aabb);

        return handle;
    }

    MeshHandle create_mesh(Handle<Material> material_handle, Vector<Vertex>& vertices, Vector<u32> indicies = {}) {
        MeshHandle handle = backend.meshes.acquire();
        Mesh& mesh = backend.meshes.get(handle);
        mesh = Mesh::create(material_handle, vertices, indicies);

        return handle;
    }

    MeshHandle create_mesh(ShaderHandle shader_handle, const char* path) {
        MeshHandle handle = backend.meshes.acquire();
        Mesh& mesh = backend.meshes.get(handle);
        mesh = Mesh::load_from_file(this->backend,  shader_handle, path);

        return handle;
    }
    
    ShaderHandle create_shader(Vector<const char*> shader_paths) {
        ShaderHandle handle = backend.shaders.acquire();
        Shader& shader = backend.shaders.get(handle);
        shader = Shader(shader_paths);

        return handle;
    }

    /*
    void bind_vbo(MeshHandle mesh_handle, VertexBufferHandle vbo) {
        Mesh& mesh = backend.meshes.get(mesh_handle);
        mesh.vao.bind();

        Shader& shader = backend.shaders.get(handle);
        shader = Shader(shader_paths);
    }
    */

    void recompile(ShaderHandle shader_handle) {
        Shader& shader = backend.shaders.get(shader_handle);
        shader.compile();
    }

    // TODO(Jovanni): For now just allow triangles, but later parameterize this?
    void draw_vertices(u32 vertex_base, u32 vertex_count, u32 instance_count = 1) {
        this->backend.draw_vertices(vertex_base, vertex_count, instance_count);
    }

    // TODO(Jovanni): For now just allow triangles, but later parameterize this?
    void draw_indices(u32 vertex_base, u32 index_base, u32 index_count, u32 instance_count = 1) {
        this->backend.draw_indices(vertex_base, index_base, index_count, instance_count);
    }

    void draw_mesh_entry(CommandBufferHandle cmd_handle, VertexArrayObject vao, MeshEntry entry, u32 instance_count = 1) {
        CommandBuffer& cmd = this->backend.command_buffers.get(cmd_handle);
        cmd.bind_vertex_array(vao);
        this->bind_material(entry.material_handle);
        if (entry.index_count) {
            this->backend.draw_indices(entry.vertex_base, entry.index_base, entry.index_count, instance_count);
        } else {
            this->backend.draw_vertices(entry.vertex_base, entry.vertex_count, instance_count);
        }
    }

    void draw_mesh_entry(CommandBufferHandle cmd_handle, VertexArrayObjectHandle vao_handle, MeshEntry entry, u32 instance_count = 1) {
        CommandBuffer& cmd = this->backend.command_buffers.get(cmd_handle);
        VertexArrayObject& vao = this->backend.vaos.get(vao_handle);
        cmd.bind_vertex_array(vao);
        this->bind_material(entry.material_handle);
        if (entry.index_count) {
            this->backend.draw_indices(entry.vertex_base, entry.index_base, entry.index_count, instance_count);
        } else {
            this->backend.draw_vertices(entry.vertex_base, entry.vertex_count, instance_count);
        }
    }

    /*
    void draw_mesh_entry(CommandBufferHandle cmd_handle, VertexArrayObjectHandle vao_handle, MeshEntryHandle mesh_entry_handle, u32 instance_count = 1) {
        MeshEntry& entry = this->backend.mesh_entries.get(mesh_entry_handle);
        CommandBuffer& cmd = this->backend.command_buffers.get(cmd_handle);
        VertexArrayObject& vao = this->backend.vaos.get(vao_handle);
        cmd.bind_vertex_array(vao);
        this->bind_material(entry.material_handle);
        if (entry.index_count) {
            this->backend.draw_indices(entry.vertex_base, entry.index_base, entry.index_count, instance_count);
        } else {
            this->backend.draw_vertices(entry.vertex_base, entry.vertex_count, instance_count);
        }
    }
    */

    void draw_mesh(CommandBufferHandle cmd_handle, MeshHandle mesh_handle, u32 instance_count = 1) {
        Mesh& mesh = this->backend.meshes.get(mesh_handle);
        CommandBuffer& cmd = this->backend.command_buffers.get(cmd_handle);
        cmd.bind_vertex_array(mesh.vao);
        for (MeshEntry& entry : mesh.entries) {
            this->bind_material(entry.material_handle);
            if (entry.index_count) {
                this->backend.draw_indices(entry.vertex_base, entry.index_base, entry.index_count, instance_count);
            } else {
                backend.draw_vertices(entry.vertex_base, entry.vertex_count, instance_count);
            }
        }
    }
};