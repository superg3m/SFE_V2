#pragma once

#include <glad/glad.h>
#include <core.hpp>
#include <gpu_buffers.hpp>
#include <material.hpp>

glm::mat4 convert_assimp_matrix_to_glm(const aiMatrix4x4& from);

namespace OpenGL {
    void load_assimp_texture(MaterialMap& materials, MaterialKey material_key, int i, const aiScene* scene, std::string directory, aiTextureType ai_texture_type, const char* name) {
        const aiMaterial* ai_material = scene->mMaterials[i];
        if (ai_material->GetTextureCount(ai_texture_type) <= 0) {
            return;
        }

        aiString str;
        if (ai_material->GetTexture(ai_texture_type, 0, &str, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
            const char* texture_path = str.C_Str();
            std::string filename = directory + "/" + texture_path;

            const aiTexture* ai_texture = scene->GetEmbeddedTexture(str.C_Str());
            if (ai_texture) {
                int width, height, nrChannel = 0;
                u8* image_data = stbi_load_from_memory((u8*)ai_texture->pcData, ai_texture->mWidth, &width, &height, &nrChannel, 0);
                Texture texture = Texture::load_from_memory(ai_texture_type, image_data, width, height, nrChannel);

                LOG_DEBUG("Material: %d | has embedded Texture of type: %s\n", i, Material::DIFFUSE_TEXTURE);
                materials[material_key].set_texture(Material::DIFFUSE_TEXTURE, texture);
            } else {
                LOG_DEBUG("Material: %d | has external texture of type: %s\n", i, Material::DIFFUSE_TEXTURE);
                materials[material_key].set_texture(Material::DIFFUSE_TEXTURE, Texture::load_from_file(ai_texture_type, filename.c_str()));
            }
        } else {
            LOG_ERROR("Failed to get texture path for material: %d | type: %s\n", i, Material::DIFFUSE_TEXTURE);
        }
    }

    const float MAGIC_NUMBER = -123450510.0f;
    const int RESERVED_VERTEX_ATTRIBUTE_LOCATIONS = 8;

    struct Vertex {
        glm::vec3 aPosition    = glm::vec3(MAGIC_NUMBER);   // location 0
        glm::vec3 aNormal      = glm::vec3(MAGIC_NUMBER);   // location 1
        glm::vec2 aTexCoord    = glm::vec2(MAGIC_NUMBER);   // location 2
    };

    // mesh is just geometry the actual material is something you submit with it
    struct Mesh {
        VAO vao;
        VBO vbo;
        EBO ebo;
        std::vector<MeshEntry> entries;
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
            ret.entries.push_back(MeshEntry::create(VertexLayout::PNT(), ret.vertices, ret.indices, GL_TRIANGLES));  
            ret.setup();

            return ret;
        }

        static Mesh AABB(AABB aabb) {
            // glm::vec3 center = aabb.getCenter();
            glm::vec3 extents = aabb.get_extents();
            float length = extents.x * 2.0f;
            float height = extents.y * 2.0f;
            float width  = extents.z * 2.0f;

            // bottom and top
            //  4          2
            //  ------------
            //  |          |
            //  |          |
            //  |          |
            //  ------------
            //  0          1

            glm::vec3 bottom_0 = aabb.min;
            glm::vec3 bottom_1 = glm::vec3(aabb.min.x + length, aabb.min.y, aabb.min.z);
            glm::vec3 bottom_2 = glm::vec3(aabb.min.x + length, aabb.min.y, aabb.min.z + width);
            glm::vec3 bottom_3 = glm::vec3(aabb.min.x, aabb.min.y, aabb.min.z + width);

            glm::vec3 top_0 = glm::vec3(aabb.min.x, aabb.min.y + height, aabb.min.z);
            glm::vec3 top_1 = glm::vec3(aabb.min.x + length, aabb.min.y + height, aabb.min.z);
            glm::vec3 top_2 = aabb.max;
            glm::vec3 top_3 = glm::vec3(aabb.min.x, aabb.min.y + height, aabb.min.z + width);

            std::vector<Vertex> aabb_vertices = {
                // Bottom face
                Vertex{glm::vec3(bottom_0), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(bottom_1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(bottom_1), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(bottom_2), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(bottom_2), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(bottom_3), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(bottom_3), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(bottom_0), glm::vec3(0, 0, 0), glm::vec2(0, 0)},

                // Top glm::vec3(face
                Vertex{glm::vec3(top_0), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(top_1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(top_1), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(top_2), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(top_2), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(top_3), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(top_3), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(top_0), glm::vec3(0, 0, 0), glm::vec2(0, 0)},

                // Vertical edges
                Vertex{glm::vec3(bottom_0), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(top_0), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(bottom_1), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(top_1), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(bottom_2), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(top_2), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(bottom_3), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(top_3), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
            };

            Mesh ret;
            ret.vertices = aabb_vertices;
            ret.entries.push_back(MeshEntry::create(VertexLayout::PNT(), ret.vertices, ret.indices, GL_LINES));  
            ret.setup();

            return ret;
        }

        static Mesh AABB() {
            std::vector<Vertex> aabb_vertices = {
                // Bottom face
                Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},

                // Top glm::vec3(face
                Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 0.5f, 0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 0.5f, 0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 0.5f, 0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 0.5f, 0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},

                // Vertical edges
                Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 0.5f, 0.5f, -0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3( 0.5f, 0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
                Vertex{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)}, Vertex{glm::vec3(-0.5f, 0.5f,  0.5f), glm::vec3(0, 0, 0), glm::vec2(0, 0)},
            };

            Mesh ret;
            ret.vertices = aabb_vertices;
            ret.entries.push_back(MeshEntry::create(VertexLayout::PNT(), ret.vertices, ret.indices, GL_LINES));  
            ret.setup();

            return ret;
        }

        static Mesh load_from_file(MaterialMap& materials, Shader* shader, std::string path) {
            Mesh ret = {};
            Assimp::Importer importer;
            unsigned int assimp_flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
            const aiScene* scene = importer.ReadFile(path, assimp_flags);
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                LOG_ERROR("ASSIMP ERROR: %s\n", importer.GetErrorString());
            }

            unsigned int total_vertex_count = 0;
            unsigned int total_index_count = 0;
            for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
                total_vertex_count += scene->mMeshes[i]->mNumVertices;
                total_index_count  += scene->mMeshes[i]->mNumFaces * 3;
            }

            ret.vertices.reserve(total_vertex_count);
            ret.indices.reserve(total_index_count);

            int index = path.find_last_of("/");
            if (index == std::string::npos) {
                index = path.find_last_of("\\");
                RUNTIME_ASSERT(index != std::string::npos);
            }

            std::string directory = path.substr(0, index);
            for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
                const aiMaterial* ai_material = scene->mMaterials[i];
                MaterialKey material_key = {};
                material_key.name = path;
                material_key.material_index = i;
                materials[material_key].shader = shader;

                /*
                aiColor4D ambient_color(0.0f, 0.0f, 0.0f, 0.0f);
                if (ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ambient_color) == AI_SUCCESS) {
                    this->materials[i].ambient_color.r = ambient_color.r;
                    this->materials[i].ambient_color.g = ambient_color.g;
                    this->materials[i].ambient_color.b = ambient_color.b;
                }


                aiColor3D diffuse_color(0.0f, 0.0f, 0.0f);
                if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color) == AI_SUCCESS) {
                    this->materials[i].diffuse_color.r = diffuse_color.r;
                    this->materials[i].diffuse_color.g = diffuse_color.g;
                    this->materials[i].diffuse_color.b = diffuse_color.b;
                }

                aiColor3D specular_color(0.0f, 0.0f, 0.0f);
                if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, specular_color) == AI_SUCCESS) {
                    this->materials[i].specular_color.r = specular_color.r;
                    this->materials[i].specular_color.g = specular_color.g;
                    this->materials[i].specular_color.b = specular_color.b;
                }

                float opacity = 1.0f;
                if (ai_material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS) {
                    this->materials[i].opacity = opacity;
                } else {
                    // LOG_WARN("Mesh Failed opacity matkey?\n");
                }
                */

                load_assimp_texture(materials, material_key, i, scene,  directory, aiTextureType_DIFFUSE, Material::DIFFUSE_TEXTURE);
            }

            MaterialKey material_key = {};
            material_key.name = path;

            ret.process_node(material_key, scene->mRootNode, scene, convert_assimp_matrix_to_glm(scene->mRootNode->mTransformation));
            ret.setup();

            ret.vertices.clear();
            ret.vertices.shrink_to_fit();

            ret.indices.clear();
            ret.indices.shrink_to_fit();

            return ret;
        }

        private:
            std::vector<Vertex> vertices;
            std::vector<u32> indices;
            void process_node(MaterialKey material_key, aiNode* node, const aiScene* scene, glm::mat4 parent_transform);
            MeshEntry process_mesh(MaterialKey material_key, aiMesh* ai_mesh, const aiScene* scene, glm::mat4 parent_transform);
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