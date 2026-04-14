#pragma once

#include <glad/glad.h>
#include <core.hpp>
#include <gpu_buffers.hpp>
#include <material.hpp>

namespace OpenGL {
    const float MAGIC_NUMBER = -123450510.0f;
    const int RESERVED_VERTEX_ATTRIBUTE_LOCATIONS = 8;

    struct Vertex {
        glm::vec3 aPosition    = glm::vec3(MAGIC_NUMBER);   // location 0
        glm::vec3 aNormal      = glm::vec3(MAGIC_NUMBER);   // location 1
        glm::vec2 aTexCoord    = glm::vec2(MAGIC_NUMBER);   // location 2
    };

    struct DrawData {
        GLenum draw_type = GL_TRIANGLES;
        GLuint vertex_count = 0;
        GLuint index_count = 0;
        u32 base_vertex  = 0; // starting offset to next vertex in the vertex buffer
        u32 base_index   = 0; // offset to next index in the index buffer

        template<typename T>
        static DrawData create(VertexLayout& layout, std::vector<T>& vertex_data, std::vector<GLuint> indices = {}, GLenum draw_type = GL_TRIANGLES, u32 base_vertex = 0, u32 base_index = 0) {
            DrawData ret = {};
            ret.draw_type = draw_type;
            ret.vertex_count = vertex_data.size() / layout.stride_in_floats;
            ret.index_count = indices.size();
            ret.base_vertex  = base_vertex;
            ret.base_index   = base_index;

            return ret;
        }
    };

    // mesh is just geometry the actual material is something you submit with it
    struct Mesh {
        VertexArrayObject vao;
        DrawData data;

        template<typename T>
        static Mesh create(VertexLayout& layout, const std::vector<T>& vertex_data) {
            Mesh ret = {};
            DrawData::create()

            return ret;
        }
    };

    struct Model {
        VertexArrayObject vao;
        std::vector<Mesh> meshes;
        std::vector<Material> materials;

        static Model cube(const char* path, Material material = {});
        static Model load_from_file(const char* path);
    };
}