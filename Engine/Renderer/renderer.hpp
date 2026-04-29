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
        TextureHandle handle = backend.textures.acquire();
        Request request = Request{
            .type =  RequestType::TEXTURE2D_LOAD,
            .texture = TextureRequest{
                .texture_unit = texture_unit,
                .path = path,
                .description = desc
            }
        };
        this->requests.append(request);
        return handle;
    }

    TextureHandle create_texture(u32 texture_unit, Vector<const char*> cube_map_texture_paths) {
        TextureHandle handle = backend.textures.acquire();
        Request request = Request{
            .type =  RequestType::TEXTURE3D_LOAD,
            .texture = TextureRequest{
                .texture_unit = texture_unit,
                .cubemap_paths = cube_map_texture_paths,
            }
        };
        this->requests.append(request);
        return handle;
    }

    template<typename T>
    VertexBufferHandle create_vertex_buffer(MeshHandle mesh, VertexLayout layout, Vector<T>& buffer, bool dynamic = false) {
        VertexBufferHandle handle = backend.vbos.acquire();
        Request request = Request{
            .type =  RequestType::VBO_CREATE,
            .vbo = VertexBufferRequest{
                .mesh = mesh,
                .layout = layout,
                .data = buffer.data,
                .count = buffer.count,
                .element_size = sizeof(T),
                .dynamic = dynamic
            }
        };
        this->requests.append(request);
        return handle;
    }
    
    template<typename T>
    void update_vertex_buffer(MeshHandle mesh, VertexBufferHandle vbo_handle, Vector<T>& buffer, u32 offset = 0) {
        VertexBufferHandle handle = backend.vbos.acquire();
        const size_t TOTAL_ALLOCATION_SIZE = sizeof(T) * buffer.count;
        void* frame_temp_data = this->frame_allocator.malloc(TOTAL_ALLOCATION_SIZE, alignof(T)); 
        Memory::copy(frame_temp_data, TOTAL_ALLOCATION_SIZE, buffer.data, TOTAL_ALLOCATION_SIZE);

        // NOTE(Jovanni): ill automatically get freed after the request is processed

        Request request = Request{
            .type = RequestType::VBO_UPDATE,
            .vbo = VertexBufferRequest{
                .mesh = mesh,
                .data = frame_temp_data,
                .count = buffer.count,
                .element_size = sizeof(T),
                .offset = offset
            }
        };
        this->requests.append(request);
        return handle;
    }
    

    IndexBufferHandle create_index_buffer(MeshHandle mesh, Vector<u32>& buffer, bool dynamic = false) {
        IndexBufferHandle handle = backend.ebos.acquire();

        Request request = Request{
            .type = RequestType::VBO_UPDATE,
            .vbo = IndexBufferRequest{
                .mesh = mesh,
                .data = buffer.data,
                .count = buffer.count,
                .dynamic = dynamic
            }
        };
        this->requests.append(request);
        return handle;
    }


    // MeshHandle create_mesh_cube(Handle<Material> material_handle);
    // MeshHandle create_mesh_aabb(Handle<Material> material_handle);
    // MeshHandle create_mesh_aabb(Handle<Material> material_handle, AABB aabb);

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