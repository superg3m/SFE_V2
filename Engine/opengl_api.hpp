#pragma once

#include <core.hpp>
#include <opengl.hpp>

namespace OpenGL {
    // void render_model(Model& model, Shader* shader);
    void render_vao(VAO vao, MeshEntry data);
}