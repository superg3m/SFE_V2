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

    B backend;

    TextureHandle create_texture(const char* path, TextureDescription& desc) {
        TextureHandle handle = backend.textures.acquire();
        Texture& texture = backend.textures.get(handle);
        texture = Texture::load_from_file(path, desc);

        return handle;
    }

    TextureHandle create_texture(Vector<const char*> cube_map_texture_paths) {
        TextureHandle handle = backend.textures.acquire();
        Texture& texture = backend.textures.get(handle);
        texture = Texture::load_cube_map(cube_map_texture_paths);

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
    }

    void bind_pipeline(CommandBufferHandle cmd_handle, PipelineHandle pipeline_handle, ShaderHandle shader_handle) {
        CommandBuffer& cmd = backend.command_buffers.get(cmd_handle);
        Pipeline& pipeline = backend.pipelines.get(pipeline_handle);
        Shader& shader = backend.shaders.get(shader_handle);

        cmd.bind_pipeline(pipeline, shader);
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

    void draw_vertices(u32 vertex_base, u32 vertex_count) {

    }

    void draw_indexed(u32 index_base, u32 index_count) {

    }
};