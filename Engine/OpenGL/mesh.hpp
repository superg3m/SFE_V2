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

    struct MeshEntry {
        GLenum draw_type = GL_TRIANGLES;
        GLuint vertex_count = 0;
        GLuint index_count = 0;
        u32 base_vertex  = 0; // starting offset to next vertex in the vertex buffer
        u32 base_index   = 0; // offset to next index in the index buffer
        u32 material_index = 0;

        template<typename T>
        static MeshEntry create(VertexLayout& layout, std::vector<T>& vertex_data, std::vector<GLuint> indices = {}, GLenum draw_type = GL_TRIANGLES, u32 base_vertex = 0, u32 base_index = 0, u32 material_index = 0) {
            MeshEntry ret = {};
            ret.draw_type = draw_type;
            ret.vertex_count = vertex_data.size() / layout.stride_in_floats;
            ret.index_count = indices.size();
            ret.base_vertex  = base_vertex;
            ret.base_index   = base_index;
            ret.material_index = material_index;

            return ret;
        }
    };

    // mesh is just geometry the actual material is something you submit with it
    struct Mesh {
        VertexArrayObject vao;
        VertexBufferObject vbo;
        ElementBufferObject ebo;
        std::vector<MeshEntry> meshes;
        std::vector<Material> materials;

        template<typename T>
        static Mesh create(VertexLayout& layout, std::vector<T>& vertex_data, std::vector<GLuint> indices = {}, GLenum draw_type = GL_TRIANGLES, u32 base_vertex = 0, u32 base_index = 0) {
            Mesh ret = {};
            ret.vao = VertexArrayObject::create();
            ret.vbo = VertexBufferObject::allocate(ret.vao, layout, vertex_data);
            ret.ebo = ElementBufferObject::allocate(ret.vao, indices);
            
            return ret;
        }

        // TODO(Complete this): actually maybe not because this is pretty much an entity thing?
        static Mesh load_from_file(std::string path);

    private:
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        void process_node(aiNode* node, const aiScene* scene, glm::mat4 parent_transform);
        MeshEntry process_mesh(aiMesh* ai_mesh, const aiScene* scene, glm::mat4 parent_transform);
        void setup();
    };

    /*
    struct Model {
        VertexArrayObject vao;
        std::vector<Mesh> meshes;
        std::vector<Material> materials;

        static Model cube(const char* path, Material material = {});
        static Model load_from_file(const char* path);
    };
    */
}