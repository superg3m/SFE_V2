#pragma once

#include "common.hpp"
#include "OpenGL/backend.hpp"

template <typename B> // Backend
struct Renderer {
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

    using MeshEntry = typename B::MeshEntry;
    using MeshEntryHandle = Handle<MeshEntry>;

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

    template<typename T>
    VertexBufferHandle create_vertex_buffer(PipelineHandle pipeline_handle, Vector<T>& buffer, bool dynamic = false) {
        Pipeline& pipeline = backend.pipelines.get(pipeline_handle);
        VertexBufferHandle handle = backend.vbos.acquire();
        VertexBuffer& vbo = backend.vbos.get(handle);
        vbo = VertexBuffer::create(pipeline, buffer, dynamic);

        return handle;
    }

    template<typename T>
    void update_vertex_buffer(PipelineHandle pipeline_handle, VertexBufferHandle handle, Vector<T>& data, u32 offset = 0) {
        Pipeline& pipeline = backend.pipelines.get(pipeline_handle);
        VertexBuffer& vbo = backend.vbos.get(handle);
        vbo->update_buffer(pipeline, data, offset);
    }

    IndexBufferHandle create_index_buffer(PipelineHandle pipeline_handle, Vector<u32>& buffer, bool dynamic = false) {
        Pipeline& pipeline = backend.pipelines.get(pipeline_handle);
        IndexBufferHandle handle = backend.ebos.acquire();
        IndexBuffer& ebo = backend.ebos.get(handle);
        ebo = IndexBuffer::create(pipeline, buffer, dynamic);

        return handle;
    }

    MaterialHandle create_material(ShaderHandle shader_handle) {
        MaterialHandle handle = backend.materials.acquire();
        Material& material = backend.materials.get(handle);
        material = Material(shader_handle);

        return handle;
    }

    void material_set_mat4(MaterialHandle handle, Hashmap<const char*, Mat4> bindings) {
        Material& material = backend.materials.get(handle);
        for (auto entry : bindings) {
            material.set_mat4(entry.key, entry.value);
        }
    }

    void material_set_texture(MaterialHandle handle, Hashmap<const char*, TextureHandle> bindings) {
        Material& material = backend.materials.get(handle);
        for (auto entry : bindings) {
            Texture& texture = this->backend.textures.get(entry.value);
            material.set_texture(entry.key, texture);
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

    void bind_vertex_buffer(CommandBufferHandle cmd_handle, VertexBufferHandle vbo_handle) {
        CommandBuffer& cmd = backend.command_buffers.get(cmd_handle);
        VertexBuffer& vbo = backend.vbos.get(vbo_handle);

        cmd.bind_vertex_buffer(vbo);
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

    template<typename T>
    MeshEntryHandle create_mesh_entry(PipelineHandle pipeline_handle, MaterialHandle material_handle, Vector<T>& vertex_data, Vector<u32> indices = {}, u32 vertex_base = 0, u32 index_base = 0) {
        MeshEntryHandle handle = backend.mesh_entries.acquire();
        MeshEntry& mesh_entry = backend.mesh_entries.get(handle);
        Pipeline& pipeline = backend.pipelines.get(pipeline_handle);
        mesh_entry = MeshEntry::create(pipeline.layout, material_handle, vertex_data, indices, vertex_base, index_base);

        return handle;
    }
    
    ShaderHandle create_shader(Vector<const char*> shader_paths) {
        ShaderHandle handle = backend.shaders.acquire();
        Shader& shader = backend.shaders.get(handle);
        shader = Shader(shader_paths);

        return handle;
    }

    void recompile(ShaderHandle shader_handle) {
        Shader& shader = backend.shaders.get(shader_handle);
        shader.compile();
    }

    // TODO(Jovanni): For now just allow triangles, but later parameterize this?
    void draw_vertices(u32 vertex_base, u32 vertex_count, u32 instance_count = 1) {
        backend.draw_vertices(vertex_base, vertex_count);
    }

    // TODO(Jovanni): For now just allow triangles, but later parameterize this?
    void draw_indices(u32 vertex_base, u32 index_base, u32 index_count, u32 instance_count = 1) {
        backend.draw_indices(vertex_base, index_base, index_count, instance_count);
    }

    void draw_mesh_entry(MeshEntryHandle mesh_entry_handle, u32 instance_count = 1) {
        MeshEntry& entry = backend.mesh_entries.get(mesh_entry_handle);
        this->bind_material(entry.material_handle);

        if (entry.index_count) {
            backend.draw_indices(entry.vertex_base, entry.index_base, entry.index_count, instance_count);
        } else {
            backend.draw_vertices(entry.vertex_base, entry.vertex_count, instance_count);
        }
    }
};