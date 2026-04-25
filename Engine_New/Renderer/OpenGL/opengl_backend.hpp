#include "../../Core/core.hpp"

enum class TextureSamplerType {
	SAMPLER_2D,
	CUBEMAP_3D
};

struct TextureDescription {
    bool pixel_perfect = false;
    bool vertical_flip = true;
	TextureSamplerType type = TextureSamplerType::SAMPLER_2D;
};

struct Texture {
	u32 id = 0;
	u32 width = 0;
	u32 height = 0;
	TextureSamplerType type = TextureSamplerType::SAMPLER_2D;

	static void flip_vertically_in_place(u8* data, int width, int height);
	static Texture load_from_file(const char* path, TextureDescription& desc);
	static Texture load_from_memory(const u8* data, int width, int height, int channels, TextureDescription& desc);
	static Texture load_cube_map(Vector<const char*> cube_map_texture_paths);
};

using TextureRegistry = Registry<Texture, 256>;
using TextureHandle = Handle<Texture>;

struct OpenGL {
    static TextureHandle create_texture(TextureRegistry* registry, const char* path, TextureDescription& desc) {
        TextureHandle ret = TextureRegistry::aquire_handle(registry);
		Texture& texture = TextureRegistry::get(registry, ret);
		texture = Texture::load_from_file(path, desc);

        return ret;
    }

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

	static TextureHandle create_texture(TextureRegistry* registry, Vector<const char*> cube_map_texture_paths) {
        TextureHandle ret = TextureRegistry::aquire_handle(registry);
		Texture& texture = TextureRegistry::get(registry, ret);
		texture = Texture::load_cube_map(cube_map_texture_paths);

        return ret;
    }
};

struct GLBuffer {
    uint32_t id; // GLuint
    BufferUsage usage;
};