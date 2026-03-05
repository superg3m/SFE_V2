#pragma once

#include "../../Core/Common/sfe_common.hpp"
#include "../../Core/DataStructure/sfe_ds.hpp"

enum class TextureSamplerType {
    SAMPLER2D,
    CUBEMAP3D
};

struct Texture {
    unsigned int id = 0;
    unsigned int width = 0;
    unsigned int height = 0;
    u8* data = nullptr; // if you use should_free = false then you must free this yourself with freeData()
    TextureSamplerType type = TextureSamplerType::SAMPLER2D;
    // sampler2d or cubemap

    Texture();
    static void FlipVerticallyInPlace(u8* data, int width, int height);
    static Texture LoadFromFile(const char* path, bool should_free = true);
    static Texture LoadFromMemory(const u8* data, int width, int height, int nrChannels, bool pixel_perfect = false);
    static Texture LoadCubeMap(DS::Vector<const char*> cube_map_texture_paths);

    void freeData();
};