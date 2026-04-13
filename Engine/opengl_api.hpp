#pragma once

#include <core.hpp>
#include <texture.hpp>
#include <geometry.hpp>
#include <gpu_buffers.hpp>

namespace OpenGL {
    void render_vao_with_textures(VertexArrayObject vao, DrawData data, std::vector<Texture> textures = {});
}