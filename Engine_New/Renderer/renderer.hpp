#pragma once

#include "common.hpp"
#include "OpenGL/backend.hpp"

template <typename B> // Backend
struct Renderer {
    using Texture = typename B::Texture;
    using TextureHandle = Handle<Texture>;
    using CommandBuffer = typename B::CommandBuffer;

    B backend;

    CommandBuffer begin_frame(u32 framebuffer = 0) {
        return backend.begin_frame();
    }

    CommandBuffer end_frame() {
        return backend.end_frame();
    }

    TextureHandle create_texture(const char* path, TextureDescription& desc) {
        TextureHandle handle = backend.textures.aquire();
        Texture& texture = backend.textures.get(handle);
        texture = Texture::create_texture(path, desc);

        return handle;
    }

    TextureHandle create_texture(Vector<const char*> cube_map_texture_paths) {
        TextureHandle handle = backend.textures.aquire();
        Texture& texture = backend.textures.get(handle);
        texture = Texture::create_texture(cube_map_texture_paths);

        return handle;
    }

    /*
    BufferHandle CreateBuffer(const BufferDesc& desc, const void* data) {
        uint32_t idx = buffers.Allocate();
        auto& slot = buffers.slots[idx];

        uint32_t id;
        glGenBuffers(1, &id);

        GLenum target = GL_ARRAY_BUFFER;
        if (desc.usage == BufferUsage::Index)
            target = GL_ELEMENT_ARRAY_BUFFER;

        glBindBuffer(target, id);
        glBufferData(target, desc.size, data, GL_STATIC_DRAW);

        slot.data.id = id;
        slot.data.usage = desc.usage;

        return { idx, slot.generation };
    }
    */
};