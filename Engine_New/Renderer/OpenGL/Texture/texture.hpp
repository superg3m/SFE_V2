#pragma once

#include <core.hpp>
#include <glad/glad.h>

namespace OpenGL {
    typedef u32 TextureID;
    typedef u32 TextureUnit;
    enum class TextureSamplerType {
        SAMPLER_2D,
        CUBEMAP_3D
    };

    struct Texture {
        TextureUnit texture_unit;
        TextureID id = 0;
        u32 width = 0;
        u32 height = 0;
        u8* data = nullptr; // if you use should_free = false then you must free this yourself with free_data()
        TextureSamplerType type = TextureSamplerType::SAMPLER_2D;

        static void flip_vertically_in_place(u8* data, int width, int height);
        static Texture load_from_file(TextureUnit texture_unit, const char* path, bool pixel_perfect = false, bool should_free = true);
        static Texture load_from_memory(TextureUnit texture_unit, const u8* data, int width, int height, int channels, bool pixel_perfect = false);
        static Texture load_cube_map(TextureUnit texture_unit, std::vector<const char*> cube_map_texture_paths);

        void free_data();
    };
}