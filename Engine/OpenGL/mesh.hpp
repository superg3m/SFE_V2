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
        u32 vertex_count = 0;
        u32 index_count = 0;
        u32 base_vertex  = 0; // starting offset to next vertex in the vertex buffer
        u32 base_index   = 0; // offset to next index in the index buffer
        u32 material_index = 0;
        AABB aabb;

        template<typename T>
        static MeshEntry create(VertexLayout& layout, std::vector<T>& vertex_data, std::vector<u32> indices = {}, GLenum draw_type = GL_TRIANGLES, u32 base_vertex = 0, u32 base_index = 0, u32 material_index = 0) {
            MeshEntry ret = {};
            ret.draw_type = draw_type;
            ret.vertex_count = (vertex_data.size() * sizeof(T)) / layout.stride;
            ret.index_count = indices.size();
            ret.base_vertex  = base_vertex;
            ret.base_index   = base_index;
            ret.material_index = material_index;

            return ret;
        }
    };

    // mesh is just geometry the actual material is something you submit with it
    struct Mesh {
        VAO vao;
        VBO vbo;
        EBO ebo;
        std::vector<MeshEntry> meshes;
        std::vector<Material> materials;
        AABB aabb;

        template<typename T>
        static Mesh create(VertexLayout& layout, std::vector<T>& vertex_data, std::vector<u32> indices = {}, GLenum draw_type = GL_TRIANGLES, u32 base_vertex = 0, u32 base_index = 0) {
            Mesh ret = {};
            ret.vao = VAO::create();
            ret.vbo = VBO::allocate(ret.vao, layout, vertex_data);
            ret.ebo = EBO::allocate(ret.vao, indices);
            
            return ret;
        }

        static Mesh cube() {
            std::vector<Vertex> cube_vertices = {
                // Front face
                Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0, 0, -1), glm::vec2(0, 0)},
                Vertex{glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0, 0, -1), glm::vec2(1, 0)},
                Vertex{glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec3(0, 0, -1), glm::vec2(1, 1)},
                Vertex{glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0, 0, -1), glm::vec2(0, 1)},

                // Back face
                Vertex{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0, 0, 1), glm::vec2(0, 0)},
                Vertex{glm::vec3( 1.0f, -1.0f, 1.0f), glm::vec3(0, 0, 1), glm::vec2(1, 0)},
                Vertex{glm::vec3( 1.0f,  1.0f, 1.0f), glm::vec3(0, 0, 1), glm::vec2(1, 1)},
                Vertex{glm::vec3(-1.0f,  1.0f, 1.0f), glm::vec3(0, 0, 1), glm::vec2(0, 1)},

                // Left face
                Vertex{glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(-1, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1, 0, 0), glm::vec2(1, 0)},
                Vertex{glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1, 0, 0), glm::vec2(1, 1)},
                Vertex{glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1, 0, 0), glm::vec2(0, 1)},

                // Right face
                Vertex{glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(1, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(1, 0, 0), glm::vec2(1, 0)},
                Vertex{glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec3(1, 0, 0), glm::vec2(1, 1)},
                Vertex{glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec3(1, 0, 0), glm::vec2(0, 1)},

                // Bottom face
                Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0, -1, 0), glm::vec2(0, 1)},
                Vertex{glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0, -1, 0), glm::vec2(1, 1)},
                Vertex{glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(0, -1, 0), glm::vec2(1, 0)},
                Vertex{glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0, -1, 0), glm::vec2(0, 0)},

                // Top face
                Vertex{glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0, 1, 0), glm::vec2(0, 1)},
                Vertex{glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec3(0, 1, 0), glm::vec2(1, 1)},
                Vertex{glm::vec3( 1.0f,  1.0f,  1.0f), glm::vec3(0, 1, 0), glm::vec2(1, 0)},
                Vertex{glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(0, 1, 0), glm::vec2(0, 0)},
            };

            std::vector<unsigned int> cube_indices = {
                0,  1,  2,  2,  3,  0,  // Front
                4,  5,  6,  6,  7,  4,  // Back
                8,  9,  10, 10, 11, 8,  // Left
                12, 13, 14, 14, 15, 12, // Right
                16, 17, 18, 18, 19, 16, // Bottom
                20, 21, 22, 22, 23, 20  // Top
            };


            Mesh ret;
            ret.vertices = cube_vertices;
            ret.indices = cube_indices;
            ret.meshes.push_back(MeshEntry::create(VertexLayout::PNT(), ret.vertices, ret.indices, GL_TRIANGLES));  
            ret.setup();

            return ret;
        }

        static Mesh AABB() {
            std::vector<Vertex> aabb_vertices = {
                // Bottom face
                Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},

                // Top face
                Vertex{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 1.0f, 1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 1.0f, 1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 1.0f, 1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 1.0f, 1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-1.0f, 1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(-1.0f, 1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},

                // Vertical edges
                Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 1.0f, 1.0f, -1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 1.0f, 1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-1.0f, 1.0f,  1.0f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
            };

            Mesh ret;
            ret.vertices = aabb_vertices;
            ret.meshes.push_back(MeshEntry::create(VertexLayout::PNT(), aabb_vertices, {}, GL_LINES));  
            ret.setup();

            return ret;
        }

        // TODO(Complete this): actually maybe not because this is pretty much an entity thing?
        static Mesh load_from_file(Shader* shader, std::string path);

    private:
        std::vector<Vertex> vertices;
        std::vector<u32> indices;
        void process_node(aiNode* node, const aiScene* scene, glm::mat4 parent_transform);
        MeshEntry process_mesh(aiMesh* ai_mesh, const aiScene* scene, glm::mat4 parent_transform);
        void setup();
    };

    /*
    struct Model {
        VAO vao;
        std::vector<Mesh> meshes;
        std::vector<Material> materials;

        static Model cube(const char* path, Material material = {});
        static Model load_from_file(const char* path);
    };
    */
}