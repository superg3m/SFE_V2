#pragma once

#include "Common/common.hpp"

// using VertexArrayObject = typename B::VertexArrayObject;
// using VertexArrayObjectHandle = Handle<VertexArrayObject>;

/*
struct RenderCommand {
	MeshHandle mesh;
    PipelineHandle pipeline;
	Mat4 model = Mat4::identity();
	Mat4 view = Mat4::identity();
	Mat4 projection = Mat4::identity();
    u32 instance_count = 1;
};
*/

template<typename B>
struct Renderer {
    B backend;

    int WINDOW_WIDTH = 800;
    int WINDOW_HEIGHT = 800;
    int FRAME_BUFFER_WIDTH = 0;
    int FRAME_BUFFER_HEIGHT = 0;

    Allocator frame_allocator = Allocator::invalid();

    Vector<Request> requests;
    static Renderer create(Allocator permanent_allocator, Allocator frame_allocator) {
        Renderer ret = {};
        ret.frame_allocator = frame_allocator;
        ret.requests = Vector<Request>(ret.frame_allocator);

        return ret;
    }

    TextureHandle create_texture(u32 texture_unit, const char* path, TextureDescription& desc) {
        TextureHandle texture = this->backend.textures.acquire();
        Request request = Request::create_texture(texture, texture_unit, path, desc);
        this->requests.append(request);

        return texture;
    }

    TextureHandle create_texture(u32 texture_unit, Vector<const char*> cube_map_texture_paths) {
        TextureHandle texture = this->backend.textures.acquire();
        Request request = Request::create_texture(texture, texture_unit,cube_map_texture_paths);
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

    MeshHandle create_mesh(ShaderHandle shader, const char *path) {
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

    ShaderHandle create_shader(Vector<const char*> shader_paths) {
        ShaderHandle shader = this->backend.shaders.acquire();
        Request request = Request::create_shader(shader, shader_paths);
        this->requests.append(request);
        return shader;
    }

    MaterialHandle create_material(ShaderHandle shader) {
        MaterialHandle material = this->backend.materials.acquire();
        Material& material_slot = this->backend.materials.get(material.handle);
        material_slot = Material(shader);

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

    void material_set_uniforms(MaterialHandle material, Hashmap<const char*, BindingValue> uniforms) {
        Material& material_slot = this->backend.materials.get(material.handle);
        for (auto& entry : uniforms) {
            const char* k = entry.key;
            BindingValue v = entry.value;
            material_slot.set_uniform(k, v);
        }
    }

    void shader_recompile(ShaderHandle shader) {
        Request request = Request::shader_recompile(shader);
        this->requests.append(request);
    }
    
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