#pragma once

#include "common.hpp"
#include "OpenGL/backend.hpp"
// #include "Vulkan/backend.hpp"
// #include "DirectX/backend.hpp"

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

using MeshEntry = typename B::MeshEntry;
using MeshEntryHandle = Handle<MeshEntry>;

using Mesh = typename B::Mesh;
using MeshHandle = Handle<Mesh>;

using VertexLayout = typename B::VertexLayout;
using VertexLayoutHandle = Handle<VertexLayout>;

using BindingValue = typename B::BindingValue;

struct Renderer {
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

    VertexLayoutHandle create_layout(VertexLayout new_layout) {
        VertexLayoutHandle handle = backend.layouts.acquire();
        VertexLayout& layout = backend.layouts.get(handle);
        layout = new_layout;

        return handle;
    }

    VertexLayoutHandle create_layout(Vector<VertexAttribute> attributes) {
        VertexLayoutHandle handle = backend.layouts.acquire();
        VertexLayout& layout = backend.layouts.get(handle);
        layout = VertexLayout(attributes);

        return handle;
    }

    void bind_layout(VertexLayoutHandle handle) {
        VertexLayout& layout = backend.layouts.get(handle);
        this->backend.bind_layout(layout);
    }

    template<typename T>
    VertexBufferHandle create_vertex_buffer(VertexLayoutHandle layout_handle, Vector<T>& buffer, bool dynamic = false) {
        VertexLayout& layout = backend.layouts.get(layout_handle);
        VertexBufferHandle handle = backend.vbos.acquire();
        VertexBuffer& vbo = backend.vbos.get(handle);
        vbo = VertexBuffer::create(layout, buffer, dynamic);

        return handle;
    }

    template<typename T>
    void update_vertex_buffer(VertexLayoutHandle layout_handle, VertexBufferHandle handle, Vector<T>& data, u32 offset = 0) {
        VertexLayout& layout = backend.layouts.get(layout_handle);
        VertexBuffer& vbo = backend.vbos.get(handle);
        vbo->update_buffer(layout, data, offset);
    }

    IndexBufferHandle create_index_buffer(VertexLayoutHandle layout_handle, Vector<u32>& buffer, bool dynamic = false) {
        VertexLayout& layout = backend.layouts.get(layout_handle);
        IndexBufferHandle handle = backend.ebos.acquire();
        IndexBuffer& ebo = backend.ebos.get(handle);
        ebo = IndexBuffer::create(layout, buffer, dynamic);

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

    MeshHandle create_mesh(Vector<VertexAttributes layout_handle, Handle<Material> material_handle, Vector<Vertex>& vertices, Vector<u32> indicies = {}) {
        MeshHandle handle = backend.meshes.acquire();
        Mesh& mesh = backend.meshes.get(handle);
        VertexLayout& layout = backend.layouts.get(layout_handle);
        mesh = Mesh::create(layout, material_handle, vertices, indicies);

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

    void draw_entry_mesh(VertexLayoutHandle layout_handle, MeshEntryHandle mesh_handle, u32 instance_count = 1) {
        MeshEntry& entry = this->backend.mesh_entries.get(mesh_handle);
        this->bind_layout(layout_handle);
        this->bind_material(entry.material_handle);
        if (entry.index_count) {
            this->backend.draw_indices(entry.vertex_base, entry.index_base, entry.index_count, instance_count);
        } else {
            this->backend.draw_vertices(entry.vertex_base, entry.vertex_count, instance_count);
        }
    }

    void draw_mesh(MeshHandle mesh_handle, u32 instance_count = 1) {
        Mesh& mesh = this->backend.meshes.get(mesh_handle);
        this->backend.bind_layout(mesh.layout);
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