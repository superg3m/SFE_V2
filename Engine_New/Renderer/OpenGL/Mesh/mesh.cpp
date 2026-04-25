#include <mesh.hpp>

glm::mat4 convert_assimp_matrix_to_glm(const aiMatrix4x4& from) {
    glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

namespace OpenGL {
    AABB calculate_aabb(const std::vector<Vertex>& vertices, int base_vertex, int vertex_count) {
        float x_min = FLT_MAX;
        float y_min = FLT_MAX;
        float z_min = FLT_MAX;

        float x_max = FLT_MIN;
        float y_max = FLT_MIN;
        float z_max = FLT_MIN;
        for (int i = base_vertex; i < base_vertex + vertex_count; i++) {
            const Vertex v = vertices[i];
            float x = v.aPosition.x;
            float y = v.aPosition.y;
            float z = v.aPosition.z;

            if (x_min > x) {
                x_min = x;
            } else if (x_max < x) {
                x_max = x;
            }

            if (y_min > y) {
                y_min = y;
            } else if (y_max < y) {
                y_max = y;
            }

            if (z_min > z) {
                z_min = z;
            } else if (z_max < z) {
                z_max = z;
            }
        }

        glm::vec3 center  = glm::vec3(
            (x_max + x_min) / 2.0f,
            (y_max + y_min) / 2.0f,
            (z_max + z_min) / 2.0f
        );

        glm::vec3 extents = glm::vec3(
            (x_max - x_min) / 2.0f,
            (y_max - y_min) / 2.0f,
            (z_max - z_min) / 2.0f
        );

        return AABB::from_center_extents(center, extents);
    }

    MeshEntry Mesh::process_mesh(MaterialKey material_key, aiMesh* ai_mesh, const aiScene* scene, glm::mat4 parent_transform) {
        material_key.material_index = ai_mesh->mMaterialIndex;
        
        MeshEntry entry = {};
        entry.base_vertex = (unsigned int)this->vertices.size();
        entry.base_index = (unsigned int)this->indices.size();
        entry.material_key = material_key;
        entry.index_count = ai_mesh->mNumFaces * 3;
        entry.vertex_count = ai_mesh->mNumVertices;
        unsigned int meshPrimitiveType;
        if (ai_mesh->mPrimitiveTypes & aiPrimitiveType_POINT) {
            meshPrimitiveType = 1;
            entry.draw_type = GL_POINTS;
        } else if (ai_mesh->mPrimitiveTypes & aiPrimitiveType_LINE) {
            meshPrimitiveType = 2;
            entry.draw_type = GL_LINES;
        } else if (ai_mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) {
            meshPrimitiveType = 3;
            entry.draw_type = GL_TRIANGLES;
        } else if (ai_mesh->mPrimitiveTypes & aiPrimitiveType_POLYGON) {
            LOG_ERROR(
                "We only support drawing triangle, line, or point meshes. "
                "This mesh contained polygons, and we are skipping it. "
                "To resolve this problem, ensure that the file is loaded "
                "with aiProcess_Triangulate to force ASSIMP to triangulate "
                "the model.\n"
            );

            RUNTIME_ASSERT(false);
        } else {
            RUNTIME_ASSERT_MSG(false, "Unknown primitive type in mesh.\n");
        }

        { // Geometry Start
            const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
            Vertex v = {};
            for (unsigned int j = 0; j < ai_mesh->mNumVertices; j++) {
                const aiVector3D& ai_position = ai_mesh->mVertices[j];

                glm::vec4 transformed_position = parent_transform * glm::vec4(ai_position.x, ai_position.y, ai_position.z, 1.0f);
                v.aPosition = glm::vec3(transformed_position.x, transformed_position.y, transformed_position.z);

                if (ai_mesh->mNormals) {
                    const aiVector3D& pNormal = ai_mesh->mNormals[j];
                    glm::vec4 transformed_normal = parent_transform * glm::vec4(pNormal.x, pNormal.y, pNormal.z, 0.0f); // W component is 0 for vectors
                    v.aNormal = glm::normalize(glm::vec3(transformed_normal.x, transformed_normal.y, transformed_normal.z)); // Normalize after transform
                } else {
                    aiVector3D Normal(0.0f, 1.0f, 0.0f);
                    v.aNormal = glm::vec3(Normal.x, Normal.y, Normal.z);
                }

                const aiVector3D& uv = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][j] : Zero3D;
                v.aTexCoord = glm::vec2(uv.x, uv.y);

                this->vertices.push_back(v);
            }

            for (unsigned int j = 0; j < ai_mesh->mNumFaces; j++) {
                const aiFace& Face = ai_mesh->mFaces[j];
                if (Face.mNumIndices == 3) {
                    this->indices.push_back(Face.mIndices[0]);
                    this->indices.push_back(Face.mIndices[1]);
                    this->indices.push_back(Face.mIndices[2]);
                } else {
                    LOG_ERROR("Mesh '%s' has non-triangular face with %d indices. Skipping.\n", ai_mesh->mName.C_Str(), Face.mNumIndices);
                }
            }
        } // Geometry End

        return entry;
    }

    void Mesh::process_node(MaterialKey material_key, aiNode* node, const aiScene* scene, glm::mat4 parent_transform) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            RUNTIME_ASSERT_MSG(mesh->mPrimitiveTypes != 0, "Primitive type not set by ASSIMP in mesh.\n");
            if ((mesh->mPrimitiveTypes & (mesh->mPrimitiveTypes-1)) != 0) {
                LOG_ERROR(
                    "This mesh has more than one primitive"
                    "type in it. The model should be loaded with the "
                    "aiProcess_SortByPType flag set.\n"
                );

                RUNTIME_ASSERT(false);
            }

            this->entries.push_back(this->process_mesh(material_key, mesh, scene, parent_transform));
        }

        glm::mat4 new_parent_transform = parent_transform * convert_assimp_matrix_to_glm(node->mTransformation);
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            this->process_node(material_key, node->mChildren[i], scene, new_parent_transform);
        }
    }

    void Mesh::setup() {
        this->aabb = calculate_aabb(this->vertices, 0, this->vertices.size());
        for (MeshEntry& entry : entries) {
            entry.aabb = calculate_aabb(this->vertices, entry.base_vertex, entry.vertex_count);
        }

        this->vao = VAO::create();
        this->vbo = VBO::allocate(this->vao, VertexLayout::PNT(), this->vertices);
        this->ebo = EBO::allocate(this->vao, this->indices);
    }
}