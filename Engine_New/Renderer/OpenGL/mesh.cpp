#include "backend.hpp"
#include <stb_image.h>

INTERNAL_LINKAGE Mat4 convert_assimp_matrix_to_glm(const aiMatrix4x4& ai_matrix) {
	Mat4 ret = Mat4::identity();

	ret.v[0].x = ai_matrix.a1; ret.v[1].x = ai_matrix.b1; ret.v[2].x = ai_matrix.c1; ret.v[3].x = ai_matrix.d1; 
	ret.v[0].y = ai_matrix.a2; ret.v[1].y = ai_matrix.b2; ret.v[2].y = ai_matrix.c2; ret.v[3].y = ai_matrix.d2;
	ret.v[0].z = ai_matrix.a3; ret.v[1].z = ai_matrix.b3; ret.v[2].z = ai_matrix.c3; ret.v[3].z = ai_matrix.d3;
	ret.v[0].w = ai_matrix.a4; ret.v[1].w = ai_matrix.b4; ret.v[2].w = ai_matrix.c4; ret.v[3].w = ai_matrix.d4;

	return ret;
}

INTERNAL_LINKAGE AABB calculate_aabb(Vector<Vertex>& vertices, int base_vertex, int vertex_count) {
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

	Vec3 center  = Vec3(
		(x_max + x_min) / 2.0f,
		(y_max + y_min) / 2.0f,
		(z_max + z_min) / 2.0f
	);

	Vec3 extents = Vec3(
		(x_max - x_min) / 2.0f,
		(y_max - y_min) / 2.0f,
		(z_max - z_min) / 2.0f
	);

	return AABB::from_center_extents(center, extents);
}

INTERNAL_LINKAGE void load_assimp_texture(OpenGL::Material& material, int i, const aiScene* scene, String directory, aiTextureType ai_texture_type, const char* name) {
	const aiMaterial* ai_material = scene->mMaterials[i];
	if (ai_material->GetTextureCount(ai_texture_type) <= 0) {
		return;
	}

	aiString str;
	if (ai_material->GetTexture(ai_texture_type, 0, &str, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
		const char* texture_path = str.C_Str();
		char filename[512] = {};
		u64 length = 0; 
		String::append(filename, length, 512, directory.data, directory.length);
		String::append(filename, length, 512, '/');
		String::append(filename, length, 512,  texture_path, String::cstr_length(texture_path));

		const aiTexture* ai_texture = scene->GetEmbeddedTexture(str.C_Str());
		if (ai_texture) {
			int width, height, nrChannel = 0;
			u8* image_data = stbi_load_from_memory((u8*)ai_texture->pcData, ai_texture->mWidth, &width, &height, &nrChannel, 0);
			OpenGL::Texture texture = OpenGL::Texture::load_from_memory(ai_texture_type, image_data, width, height, nrChannel, {});

			LOG_DEBUG("Material: %d | has embedded Texture of type: %s\n", i, name);
			material.set_uniform(name, texture);
		} else {
			LOG_DEBUG("Material: %d | has external texture of type: %s\n", i, name);
			material.set_uniform(name, OpenGL::Texture::load_from_file(ai_texture_type, filename, {}));
		}
	} else {
		LOG_ERROR("Failed to get texture path for material: %d | type: %s\n", i, name);
	}
}

OpenGL::MeshEntry OpenGL::MeshEntry::create(VertexLayout layout, Handle<OpenGL::Material> material_handle, Vector<Vertex>& vertex_data, Vector<u32> indices, u32 vertex_base, u32 index_base, GLenum draw_type) {
    MeshEntry ret = {};
    ret.draw_type = draw_type;
    ret.vertex_count = (vertex_data.count * sizeof(Vertex)) / layout.stride;
    ret.index_count = indices.count;
    ret.vertex_base  = vertex_base;
    ret.index_base   = index_base;
    ret.material_handle = material_handle;
    ret.aabb = calculate_aabb(vertex_data, vertex_base, ret.vertex_count);

    return ret;
}

OpenGL::MeshEntry OpenGL::Mesh::process_mesh(Hashmap<int, Handle<Material>>& map, aiMesh* ai_mesh, const aiScene* scene, Mat4 parent_transform) {
    OpenGL::MeshEntry entry = {};
    entry.vertex_base = (unsigned int)this->vertices.count;
    entry.index_base = (unsigned int)this->indices.count;
    entry.index_count = ai_mesh->mNumFaces * 3;
    entry.vertex_count = ai_mesh->mNumVertices;
    entry.material_handle = map.get(ai_mesh->mMaterialIndex);
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

            Vec4 transformed_position = parent_transform * Vec4(ai_position.x, ai_position.y, ai_position.z, 1.0f);
            v.aPosition = Vec3(transformed_position.x, transformed_position.y, transformed_position.z);

            if (ai_mesh->mNormals) {
                const aiVector3D& pNormal = ai_mesh->mNormals[j];
                Vec4 transformed_normal = parent_transform * Vec4(pNormal.x, pNormal.y, pNormal.z, 0.0f); // W component is 0 for vectors
                v.aNormal = Vec3(transformed_normal.x, transformed_normal.y, transformed_normal.z).normalize(); // Normalize after transform
            } else {
                aiVector3D Normal(0.0f, 1.0f, 0.0f);
                v.aNormal = Vec3(Normal.x, Normal.y, Normal.z);
            }

            const aiVector3D& uv = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][j] : Zero3D;
            v.aTexCoord = Vec2(uv.x, uv.y);

            this->vertices.append(v);
        }

        for (unsigned int j = 0; j < ai_mesh->mNumFaces; j++) {
            const aiFace& Face = ai_mesh->mFaces[j];
            if (Face.mNumIndices == 3) {
                this->indices.append(Face.mIndices[0]);
                this->indices.append(Face.mIndices[1]);
                this->indices.append(Face.mIndices[2]);
            } else {
                LOG_ERROR("Mesh '%s' has non-triangular face with %d indices. Skipping.\n", ai_mesh->mName.C_Str(), Face.mNumIndices);
            }
        }
    } // Geometry End

    return entry;
}

void OpenGL::Mesh::process_node(Hashmap<int, Handle<Material>>& map, aiNode* node, const aiScene* scene, Mat4 parent_transform) {
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

        this->entries.append(this->process_mesh(map, mesh, scene, parent_transform));
    }

    Mat4 new_parent_transform = parent_transform * convert_assimp_matrix_to_glm(node->mTransformation);
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        this->process_node(map, node->mChildren[i], scene, new_parent_transform);
    }
}

void OpenGL::Mesh::setup() {
    this->aabb = calculate_aabb(this->vertices, 0, this->vertices.count);
    for (MeshEntry& entry : entries) {
        entry.aabb = calculate_aabb(this->vertices, entry.vertex_base, entry.vertex_count);
    }

    this->vao = VertexArrayObject::create();
    this->vbo = VertexBuffer::create(this->vao, VertexLayout::PNTC(), this->vertices);
    this->ebo = IndexBuffer::create(this->vao, this->indices);
}


OpenGL::Mesh OpenGL::Mesh::create(Handle<Material> material_handle, Vector<Vertex>& vertices, Vector<u32> indices, GLenum draw_type, u32 vertex_base, u32 index_base ) {
	Mesh ret = {};
    ret.vertices = vertices;
    ret.indices = indices;
    ret.entries.append(MeshEntry::create(VertexLayout::PNTC(), material_handle, vertices, indices, vertex_base, index_base, draw_type));
    ret.setup();
	
	return ret;
}

OpenGL::Mesh OpenGL::Mesh::cube(Handle<Material> material_handle) {
	Vector<Vertex> cube_vertices = {
		// Front face
		Vertex{Vec3(-1.0f, -1.0f, -1.0f), Vec3(0, 0, -1), Vec2(0, 0)},
		Vertex{Vec3( 1.0f, -1.0f, -1.0f), Vec3(0, 0, -1), Vec2(1, 0)},
		Vertex{Vec3( 1.0f,  1.0f, -1.0f), Vec3(0, 0, -1), Vec2(1, 1)},
		Vertex{Vec3(-1.0f,  1.0f, -1.0f), Vec3(0, 0, -1), Vec2(0, 1)},

		// Back face
		Vertex{Vec3(-1.0f, -1.0f, 1.0f), Vec3(0, 0, 1), Vec2(0, 0)},
		Vertex{Vec3( 1.0f, -1.0f, 1.0f), Vec3(0, 0, 1), Vec2(1, 0)},
		Vertex{Vec3( 1.0f,  1.0f, 1.0f), Vec3(0, 0, 1), Vec2(1, 1)},
		Vertex{Vec3(-1.0f,  1.0f, 1.0f), Vec3(0, 0, 1), Vec2(0, 1)},

		// Left face
		Vertex{Vec3(-1.0f, -1.0f,  1.0f), Vec3(-1, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(-1.0f, -1.0f, -1.0f), Vec3(-1, 0, 0), Vec2(1, 0)},
		Vertex{Vec3(-1.0f,  1.0f, -1.0f), Vec3(-1, 0, 0), Vec2(1, 1)},
		Vertex{Vec3(-1.0f,  1.0f,  1.0f), Vec3(-1, 0, 0), Vec2(0, 1)},

		// Right face
		Vertex{Vec3( 1.0f, -1.0f, -1.0f), Vec3(1, 0, 0), Vec2(0, 0)},
		Vertex{Vec3( 1.0f, -1.0f,  1.0f), Vec3(1, 0, 0), Vec2(1, 0)},
		Vertex{Vec3( 1.0f,  1.0f,  1.0f), Vec3(1, 0, 0), Vec2(1, 1)},
		Vertex{Vec3( 1.0f,  1.0f, -1.0f), Vec3(1, 0, 0), Vec2(0, 1)},

		// Bottom face
		Vertex{Vec3(-1.0f, -1.0f, -1.0f), Vec3(0, -1, 0), Vec2(0, 1)},
		Vertex{Vec3( 1.0f, -1.0f, -1.0f), Vec3(0, -1, 0), Vec2(1, 1)},
		Vertex{Vec3( 1.0f, -1.0f,  1.0f), Vec3(0, -1, 0), Vec2(1, 0)},
		Vertex{Vec3(-1.0f, -1.0f,  1.0f), Vec3(0, -1, 0), Vec2(0, 0)},

		// Top face
		Vertex{Vec3(-1.0f,  1.0f, -1.0f), Vec3(0, 1, 0), Vec2(0, 1)},
		Vertex{Vec3( 1.0f,  1.0f, -1.0f), Vec3(0, 1, 0), Vec2(1, 1)},
		Vertex{Vec3( 1.0f,  1.0f,  1.0f), Vec3(0, 1, 0), Vec2(1, 0)},
		Vertex{Vec3(-1.0f,  1.0f,  1.0f), Vec3(0, 1, 0), Vec2(0, 0)},
	};

	Vector<unsigned int> cube_indices = {
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
	ret.entries.append(MeshEntry::create(VertexLayout::PNTC(), material_handle, ret.vertices, ret.indices, 0, 0, GL_TRIANGLES));
    ret.setup();

	return ret;
}

OpenGL::Mesh OpenGL::Mesh::axis_aligned_bounding_box(Handle<Material> material_handle, AABB aabb) {
	// Vec3 center = aabb.getCenter();
	Vec3 extents = aabb.extents();
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

	Vec3 bottom_0 = aabb.min;
	Vec3 bottom_1 = Vec3(aabb.min.x + length, aabb.min.y, aabb.min.z);
	Vec3 bottom_2 = Vec3(aabb.min.x + length, aabb.min.y, aabb.min.z + width);
	Vec3 bottom_3 = Vec3(aabb.min.x, aabb.min.y, aabb.min.z + width);

	Vec3 top_0 = Vec3(aabb.min.x, aabb.min.y + height, aabb.min.z);
	Vec3 top_1 = Vec3(aabb.min.x + length, aabb.min.y + height, aabb.min.z);
	Vec3 top_2 = aabb.max;
	Vec3 top_3 = Vec3(aabb.min.x, aabb.min.y + height, aabb.min.z + width);

	Vector<Vertex> aabb_vertices = {
		// Bottom face
		Vertex{Vec3(bottom_0), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(bottom_1), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(bottom_1), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(bottom_2), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(bottom_2), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(bottom_3), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(bottom_3), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(bottom_0), Vec3(0, 0, 0), Vec2(0, 0)},

		// Top Vec3(face
		Vertex{Vec3(top_0), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(top_1), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(top_1), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(top_2), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(top_2), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(top_3), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(top_3), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(top_0), Vec3(0, 0, 0), Vec2(0, 0)},

		// Vertical edges
		Vertex{Vec3(bottom_0), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(top_0), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(bottom_1), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(top_1), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(bottom_2), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(top_2), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(bottom_3), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(top_3), Vec3(0, 0, 0), Vec2(0, 0)},
	};

	Mesh ret;
	ret.vertices = aabb_vertices;
	ret.entries.append(MeshEntry::create(VertexLayout::PNTC(), material_handle, ret.vertices, ret.indices, 0, 0, GL_LINES));  
	ret.setup();

	return ret;
}

OpenGL::Mesh OpenGL::Mesh::axis_aligned_bounding_box(Handle<Material> material_handle) {
	Vector<Vertex> aabb_vertices = {
		// Bottom face
		Vertex{Vec3(-0.5f, -0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3( 0.5f, -0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3( 0.5f, -0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3( 0.5f, -0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3( 0.5f, -0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(-0.5f, -0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(-0.5f, -0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(-0.5f, -0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)},

		// Top Vec3(face
		Vertex{Vec3(-0.5f, 0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3( 0.5f, 0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3( 0.5f, 0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3( 0.5f, 0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3( 0.5f, 0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(-0.5f, 0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(-0.5f, 0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(-0.5f, 0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)},

		// Vertical edges
		Vertex{Vec3(-0.5f, -0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(-0.5f, 0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3( 0.5f, -0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3( 0.5f, 0.5f, -0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3( 0.5f, -0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3( 0.5f, 0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
		Vertex{Vec3(-0.5f, -0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)}, Vertex{Vec3(-0.5f, 0.5f,  0.5f), Vec3(0, 0, 0), Vec2(0, 0)},
	};

	Mesh ret;
	ret.vertices = aabb_vertices;
	ret.entries.append(MeshEntry::create(VertexLayout::PNTC(), material_handle, ret.vertices, ret.indices, 0, 0, GL_LINES));
    ret.setup();

	return ret;
}

OpenGL::Mesh OpenGL::Mesh::load_from_file(OpenGL& backend, Handle<OpenGL::Shader> shader_handle, const char* path) {
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

	u64 path_length = String::cstr_length(path);
	int index = String::last_index_of(path, path_length, STRING_LIT_ARG("/"));
	if (index == -1) {
		index = String::last_index_of(path, path_length, STRING_LIT_ARG("\\"));
		RUNTIME_ASSERT(index != -1);
	}

	String directory = String(path, index);
    Hashmap<int, Handle<Material>> material_index_to_material_handle = {};
	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial* ai_material = scene->mMaterials[i];
		Handle<Material> material_handle = backend.materials.acquire();
		OpenGL::Material& material = backend.materials.get(material_handle);
        material.shader_handle = shader_handle;
		load_assimp_texture(material, i, scene, directory, aiTextureType_DIFFUSE, Material::DIFFUSE_TEXTURE);

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

        material_index_to_material_handle.put(i, material_handle);
	}

	ret.process_node(material_index_to_material_handle, scene->mRootNode, scene, convert_assimp_matrix_to_glm(scene->mRootNode->mTransformation));
    ret.setup();

	ret.vertices.clear();
	ret.indices.clear();

	return ret;
}