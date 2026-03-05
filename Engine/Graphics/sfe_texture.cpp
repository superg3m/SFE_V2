#include <glad/glad.h>
#include <stb_image.h>

#include "./sfe_texture.hpp"
#include "./sfe_gl_check.hpp"
#include "../../Core/Common/sfe_common.hpp"
#include "../../Platform/sfe_platform.hpp"
#include "../../Core/Memory/sfe_memory.hpp"

Texture::Texture() {}
void Texture::FlipVerticallyInPlace(u8* data, int width, int height) {
    for (int y = 0; y < height / 2; y++) {
        for (int x = 0; x < width; x++) {
            Memory::Swap(data[y * width + x], data[(height - 1 - y) * width + x]);
        }
    }
}

Texture Texture::LoadFromFile(const char* path, bool should_free) {
    RUNTIME_ASSERT_MSG(Platform::FilePathExists(path), "Texture path: '%s' doesn't exist!\n", path);
    
    GLenum MIPMAP_TYPE = GL_LINEAR; // GET_BIT(texture_flags, 0) ? GL_NEAREST : GL_LINEAR;
    GLenum TEXTURE_VERTICAL_FLIP = true; // GET_BIT(texture_flags, 1);

    unsigned int texture = 0;
    glCheckError(glGenTextures(1, &texture));
    glCheckError(glBindTexture(GL_TEXTURE_2D, texture)); 
    glCheckError(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    glCheckError(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    glCheckError(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MIPMAP_TYPE));
    glCheckError(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MIPMAP_TYPE));

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(TEXTURE_VERTICAL_FLIP);
    u8 *data = stbi_load(path, &width, &height, &nrChannels, 0);

    GLenum internal_format = 0;
    GLenum pixel_format = 0;
    if (nrChannels == 1) {
        internal_format = GL_R8;
        pixel_format   = GL_RED;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
    } else if (nrChannels == 3) {
        internal_format = GL_RGB8;
        pixel_format = GL_RGB;
    } else if (nrChannels == 4) {
        internal_format = GL_RGBA8;
        pixel_format = GL_RGBA;
    } else {
        RUNTIME_ASSERT_MSG(false, "Unsupported channel count");
    }

    if (data) {
        glCheckError(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data));
        glCheckError(glGenerateMipmap(GL_TEXTURE_2D));
    } else {
        LOG_ERROR("TextureLoader | Failed to load texture\n");
    }

    if (should_free) {
        stbi_image_free(data);
        data = nullptr;
    }

    stbi_set_flip_vertically_on_load(false);

    if (texture == 0) {
        LOG_ERROR("TextureLoader | id is invalid!\n");
    }

    Texture ret;
    ret.id = texture;
    ret.width = width;
    ret.height = height;
    ret.data = data;

    glCheckError(glBindTexture(GL_TEXTURE_2D, 0));
    
    return ret;
}

Texture Texture::LoadFromMemory(const u8* data, int width, int height, int nrChannels, bool pixel_perfect) {
    if (!data || width <= 0 || height <= 0 || nrChannels == 0) {
        RUNTIME_ASSERT_MSG(false, "TextureLoader | Invalid input data for loadTextureFromMemory!\n");
    }

    GLenum MIPMAP_TYPE = pixel_perfect ? GL_NEAREST : GL_LINEAR;

    unsigned int texture = 0;
    glCheckError(glGenTextures(1, &texture));
    glCheckError(glBindTexture(GL_TEXTURE_2D, texture));
    glCheckError(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    glCheckError(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    glCheckError(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MIPMAP_TYPE));
    glCheckError(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MIPMAP_TYPE));

    GLenum internal_format = 0;
    GLenum pixel_format = 0;
    if (nrChannels == 1) {
        internal_format = GL_R8;
        pixel_format = GL_RED;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
    } else if (nrChannels == 3) {
        internal_format = GL_RGB8;
        pixel_format = GL_RGB;
    } else if (nrChannels == 4) {
        internal_format = GL_RGBA8;
        pixel_format = GL_RGBA;
    } else {
        RUNTIME_ASSERT_MSG(false, "Unsupported channel count");
    }

    glCheckError(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data));
    glCheckError(glGenerateMipmap(GL_TEXTURE_2D));

    if (texture == 0) {
        RUNTIME_ASSERT_MSG(false, "TextureLoader | id is invalid for memory texture!\n");
    }

    Texture ret;
    ret.id = texture;
    ret.width = width;
    ret.height = height;

    glCheckError(glBindTexture(GL_TEXTURE_2D, 0));

    return ret;
}

// {right, left,  top, bottom, front, back}
Texture Texture::LoadCubeMap(DS::Vector<const char*> texture_paths) {
    unsigned int texture;
    glCheckError(glGenTextures(1, &texture));
    glCheckError(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
    glCheckError(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glCheckError(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glCheckError(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glCheckError(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glCheckError(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    unsigned char *data;  
    for(unsigned int i = 0; i < texture_paths.count(); i++) {
        RUNTIME_ASSERT_MSG(Platform::FilePathExists(texture_paths[i]), "Texture path: '%s' doesn't exist!\n", texture_paths[i]);
        int width, height, nrChannels;
        data = stbi_load(texture_paths[i], &width, &height, &nrChannels, 0);
        
        GLenum format = 0;
        if (nrChannels == 1) {
            format = GL_RED;
        } else if (nrChannels == 3) {
            format = GL_RGB;
        } else if (nrChannels == 4) {
            format = GL_RGBA;
        } else {
            LOG_ERROR("TextureLoader | Failed to pick a stb format, most likely related to assimp, try to link your libraries in a different order\n");
        }

        if (data) {
            glCheckError(glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
            ));
        } else {
            LOG_ERROR("TextureLoader | Failed to load texture\n");
        }

        stbi_image_free(data);
    }

    Texture ret;
    ret.id = texture;
    ret.type = TextureSamplerType::CUBEMAP3D;

    glCheckError(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

    return ret;
}

void Texture::freeData() {
    RUNTIME_ASSERT_MSG(this->data, "trying to free data that has already been freed or is nullptr\n");

    stbi_image_free(this->data);
    this->data = nullptr;
}