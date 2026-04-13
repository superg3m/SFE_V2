#pragma once

#include <core.hpp>
#include <opengl.hpp>

namespace OpenGL {
    void render_model(Model& model, Shader* shader);
    void render_vao_with_textures(VertexArrayObject vao, DrawData data, std::vector<Texture> textures = {});
}