#include "backend.hpp"
#include <stb_image.h>
#include <float.h>

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
		const Vertex& v = vertices[i];

		x_min = MIN(x_min, v.aPosition.x);
		y_min = MIN(y_min, v.aPosition.y);
		z_min = MIN(z_min, v.aPosition.z);

		x_max = MAX(x_max, v.aPosition.x);
		y_max = MAX(y_max, v.aPosition.y);
		z_max = MAX(z_max, v.aPosition.z);
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

INTERNAL_LINKAGE bool load_assimp_texture(OpenGL* backend, Material* material, int i, const aiScene* scene, String directory, aiTextureType ai_texture_type, String name, TextureDescription desc) {
	INVARIENT_STRING_STRUCT_IS_HAS_NULL_TERMINTOR(name);
	const aiMaterial* ai_material = scene->mMaterials[i];
	if (ai_material->GetTextureCount(ai_texture_type) <= 0) {
		return false;
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
		Handle texture_handle = backend->textures.acquire();
		OpenGL::Texture& texture = backend->textures.get(texture_handle);
		if (ai_texture) {
			int width, height, nrChannel = 0;
			u8* image_data = stbi_load_from_memory((u8*)ai_texture->pcData, ai_texture->mWidth, &width, &height, &nrChannel, 0);
			texture = OpenGL::Texture::load_from_memory(image_data, width, height, nrChannel, desc);
			LOG_DEBUG("Material: %d | has embedded Texture of type: %s\n", i, name.data);
		} else {
			LOG_DEBUG("Material: %d | has external texture of type: %s\n", i, name.data);
			texture = OpenGL::Texture::load_from_file(String::create(filename, length), desc);
		}

		material->set_uniform(name, TextureHandle(texture_handle));
	} else {
		LOG_ERROR("Failed to get texture path for material: %d | type: %s\n", i, name);
	}

	return true;
}

void OpenGL::Mesh::setup(Vector<Vertex>& vertices, Vector<u32>& indices) {
	this->aabb = calculate_aabb(vertices, 0, vertices.count);
	this->vertex_count = vertices.count;
	this->index_count = indices.count;

	this->vao = VertexArrayObject::create();
	this->vbo = VertexBuffer::create(this->vao, VertexLayout::PNTC(), vertices.data, vertices.count, sizeof(Vertex));
	this->ebo = IndexBuffer::create(this->vao, indices);
}

OpenGL::Mesh OpenGL::Model::process_mesh(MemoryContext memory, OpenGL* backend, Hashmap<int, MaterialHandle>& map, aiMesh* ai_mesh, const aiScene* scene, Mat4 parent_transform) {
	MeshHandle mesh_handle = MeshHandle(backend->meshes.acquire(), map.get(ai_mesh->mMaterialIndex));
	OpenGL::Mesh& mesh = backend->meshes.get(mesh_handle.handle);
	mesh.self = mesh_handle;
	mesh.index_count = ai_mesh->mNumFaces * 3;
	mesh.vertex_count = ai_mesh->mNumVertices;
	mesh.original_material = map.get(ai_mesh->mMaterialIndex);
	mesh.name = String::create(String::allocate(memory.permanent_allocator, ai_mesh->mName.C_Str(), ai_mesh->mName.length), ai_mesh->mName.length); // is this cstr stable?

	// TODO(Jovanni): Pass memory context 
	Vector<Vertex> vertices = Vector<Vertex>(memory.frame_allocator, ai_mesh->mNumVertices);
	Vector<u32> indices = Vector<u32>(memory.frame_allocator, ai_mesh->mNumFaces * 3);

	unsigned int meshPrimitiveType;
	if (ai_mesh->mPrimitiveTypes & aiPrimitiveType_POINT) {
		meshPrimitiveType = 1;
		mesh.draw_type = GL_POINTS;
	} else if (ai_mesh->mPrimitiveTypes & aiPrimitiveType_LINE) {
		meshPrimitiveType = 2;
		mesh.draw_type = GL_LINES;
	} else if (ai_mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) {
		meshPrimitiveType = 3;
		mesh.draw_type = GL_TRIANGLES;
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

	// TODO(Jovanni): I don't understand why I no longer have to compensate for the parent transform? Probably i'm being dumb?
	{ // Geometry Start
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		Vertex v = {};
		for (unsigned int j = 0; j < ai_mesh->mNumVertices; j++) {
			const aiVector3D& ai_position = ai_mesh->mVertices[j];

			Vec4 transformed_position = /* parent_transform * */ Vec4(ai_position.x, ai_position.y, ai_position.z, 1.0f);
			v.aPosition = Vec3(transformed_position.x, transformed_position.y, transformed_position.z);

			if (ai_mesh->mNormals) {
				const aiVector3D& pNormal = ai_mesh->mNormals[j];
				Vec4 transformed_normal = /* parent_transform * */ Vec4(pNormal.x, pNormal.y, pNormal.z, 0.0f); // W component is 0 for vectors
				v.aNormal = Vec3(transformed_normal.x, transformed_normal.y, transformed_normal.z).normalize(); // Normalize after transform
			} else {
				aiVector3D Normal(0.0f, 1.0f, 0.0f);
				v.aNormal = Vec3(Normal.x, Normal.y, Normal.z);
			}

			const aiVector3D& uv = ai_mesh->HasTextureCoords(0) ? ai_mesh->mTextureCoords[0][j] : Zero3D;
			v.aTexCoord = Vec2(uv.x, uv.y);

			vertices.append(v);
		}

		for (unsigned int j = 0; j < ai_mesh->mNumFaces; j++) {
			const aiFace& Face = ai_mesh->mFaces[j];
			if (Face.mNumIndices == 3) {
				indices.append(Face.mIndices[0]);
				indices.append(Face.mIndices[1]);
				indices.append(Face.mIndices[2]);
			} else {
				LOG_ERROR("Mesh '%s' has non-triangular face with %d indices. Skipping.\n", ai_mesh->mName.C_Str(), Face.mNumIndices);
			}
		}
	} // Geometry End

	mesh.setup(vertices, indices);
	return mesh;
}

void OpenGL::Model::process_node(MemoryContext memory, OpenGL* backend, Hashmap<int, MaterialHandle>& map, aiNode* node, const aiScene* scene, Mat4 parent_transform) {
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

		this->meshes.append(this->process_mesh(memory, backend, map, mesh, scene, parent_transform));
	}

	Mat4 new_parent_transform = parent_transform * convert_assimp_matrix_to_glm(node->mTransformation);
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		this->process_node(memory, backend, map, node->mChildren[i], scene, new_parent_transform);
	}
}

OpenGL::Mesh OpenGL::Mesh::cube(MaterialHandle material) {
	Vector<Vertex> cube_vertices = {
		// Back face (z = -0.5)
		Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(0,0,-1), Vec2(0,0)}, // 0
		Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(0,0,-1), Vec2(1,1)}, // 1
		Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(0,0,-1), Vec2(1,0)}, // 2
		Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(0,0,-1), Vec2(0,1)}, // 3

		// Front face (z = +0.5)
		Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(0,0,1), Vec2(0,0)}, // 4
		Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(0,0,1), Vec2(1,0)}, // 5
		Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(0,0,1), Vec2(1,1)}, // 6
		Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(0,0,1), Vec2(0,1)}, // 7

		// Left face (x = -0.5)
		Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(-1,0,0), Vec2(0,1)}, // 8
		Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(-1,0,0), Vec2(1,1)}, // 9
		Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(-1,0,0), Vec2(1,0)}, // 10
		Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(-1,0,0), Vec2(0,0)}, // 11

		// Right face (x = +0.5)
		Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(1,0,0), Vec2(0,1)}, // 12
		Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(1,0,0), Vec2(1,0)}, // 13
		Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(1,0,0), Vec2(1,1)}, // 14
		Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(1,0,0), Vec2(0,0)}, // 15

		// Bottom face (y = -0.5)
		Vertex{Vec3(-0.5f,-0.5f,-0.5f), Vec3(0,-1,0), Vec2(0,1)}, // 16
		Vertex{Vec3( 0.5f,-0.5f,-0.5f), Vec3(0,-1,0), Vec2(1,1)}, // 17
		Vertex{Vec3( 0.5f,-0.5f, 0.5f), Vec3(0,-1,0), Vec2(1,0)}, // 18
		Vertex{Vec3(-0.5f,-0.5f, 0.5f), Vec3(0,-1,0), Vec2(0,0)}, // 19

		// Top face (y = +0.5)
		Vertex{Vec3(-0.5f, 0.5f,-0.5f), Vec3(0,1,0), Vec2(0,1)}, // 20
		Vertex{Vec3( 0.5f, 0.5f, 0.5f), Vec3(0,1,0), Vec2(1,0)}, // 21
		Vertex{Vec3( 0.5f, 0.5f,-0.5f), Vec3(0,1,0), Vec2(1,1)}, // 22
		Vertex{Vec3(-0.5f, 0.5f, 0.5f), Vec3(0,1,0), Vec2(0,0)}, // 23
	};

	Vector<u32> cube_indices = {
		0,  1,  2,  1,  0,  3,  // Front
		4,  5,  6,  6,  7,  4,  // Back

		8,  9,  10, 10, 11, 8,  // Left
		12, 13, 14, 13, 12, 15, // Right

		16, 17, 18, 18, 19, 16, // Bottom
		20, 21, 22, 21, 20, 23  // Top
	};


	Mesh ret = {.draw_type = GL_TRIANGLES, .original_material = material};
	ret.setup(cube_vertices, cube_indices);

	return ret;
}

OpenGL::Mesh OpenGL::Mesh::skybox_cube(MaterialHandle material) {
	Vector<Vertex> cube_vertices = {
		// Back face (z = -1.0)
		Vertex{Vec3(-1.0f,-1.0f,-1.0f), Vec3(0,0,-1), Vec2(0,0)}, // 0
		Vertex{Vec3( 1.0f, 1.0f,-1.0f), Vec3(0,0,-1), Vec2(1,1)}, // 1
		Vertex{Vec3( 1.0f,-1.0f,-1.0f), Vec3(0,0,-1), Vec2(1,0)}, // 2
		Vertex{Vec3(-1.0f, 1.0f,-1.0f), Vec3(0,0,-1), Vec2(0,1)}, // 3

		// Front face (z = +1.0)
		Vertex{Vec3(-1.0f,-1.0f, 1.0f), Vec3(0,0,1), Vec2(0,0)}, // 4
		Vertex{Vec3( 1.0f,-1.0f, 1.0f), Vec3(0,0,1), Vec2(1,0)}, // 5
		Vertex{Vec3( 1.0f, 1.0f, 1.0f), Vec3(0,0,1), Vec2(1,1)}, // 6
		Vertex{Vec3(-1.0f, 1.0f, 1.0f), Vec3(0,0,1), Vec2(0,1)}, // 7

		// Left face (x = -1.0)
		Vertex{Vec3(-1.0f, 1.0f, 1.0f), Vec3(-1,0,0), Vec2(0,1)}, // 8
		Vertex{Vec3(-1.0f, 1.0f,-1.0f), Vec3(-1,0,0), Vec2(1,1)}, // 9
		Vertex{Vec3(-1.0f,-1.0f,-1.0f), Vec3(-1,0,0), Vec2(1,0)}, // 10
		Vertex{Vec3(-1.0f,-1.0f, 1.0f), Vec3(-1,0,0), Vec2(0,0)}, // 11

		// Right face (x = +1.0)
		Vertex{Vec3( 1.0f, 1.0f, 1.0f), Vec3(1,0,0), Vec2(0,1)}, // 12
		Vertex{Vec3( 1.0f,-1.0f,-1.0f), Vec3(1,0,0), Vec2(1,0)}, // 13
		Vertex{Vec3( 1.0f, 1.0f,-1.0f), Vec3(1,0,0), Vec2(1,1)}, // 14
		Vertex{Vec3( 1.0f,-1.0f, 1.0f), Vec3(1,0,0), Vec2(0,0)}, // 15

		// Bottom face (y = -1.0)
		Vertex{Vec3(-1.0f,-1.0f,-1.0f), Vec3(0,-1,0), Vec2(0,1)}, // 16
		Vertex{Vec3( 1.0f,-1.0f,-1.0f), Vec3(0,-1,0), Vec2(1,1)}, // 17
		Vertex{Vec3( 1.0f,-1.0f, 1.0f), Vec3(0,-1,0), Vec2(1,0)}, // 18
		Vertex{Vec3(-1.0f,-1.0f, 1.0f), Vec3(0,-1,0), Vec2(0,0)}, // 19

		// Top face (y = +1.0)
		Vertex{Vec3(-1.0f, 1.0f,-1.0f), Vec3(0,1,0), Vec2(0,1)}, // 20
		Vertex{Vec3( 1.0f, 1.0f, 1.0f), Vec3(0,1,0), Vec2(1,0)}, // 21
		Vertex{Vec3( 1.0f, 1.0f,-1.0f), Vec3(0,1,0), Vec2(1,1)}, // 22
		Vertex{Vec3(-1.0f, 1.0f, 1.0f), Vec3(0,1,0), Vec2(0,0)}, // 23
	};

	Vector<u32> cube_indices = {
		0,  1,  2,  1,  0,  3,  // Front
		4,  5,  6,  6,  7,  4,  // Back

		8,  9,  10, 10, 11, 8,  // Left
		12, 13, 14, 13, 12, 15, // Right

		16, 17, 18, 18, 19, 16, // Bottom
		20, 21, 22, 21, 20, 23  // Top
	};


	Mesh ret = {.draw_type = GL_TRIANGLES, .original_material = material};
	ret.setup(cube_vertices, cube_indices);

	return ret;
}

OpenGL::Mesh OpenGL::Mesh::axis_aligned_bounding_box(MaterialHandle material, AABB aabb) {
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

	Vector<u32> aabb_indices = {};

	Mesh ret = {.draw_type = GL_LINES, .original_material = material};
	ret.setup(aabb_vertices, aabb_indices);

	return ret;
}

OpenGL::Mesh OpenGL::Mesh::axis_aligned_bounding_box(MaterialHandle material) {
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

	Vector<u32> aabb_indices = {};

	Mesh ret = {.draw_type = GL_LINES, .original_material = material};
	ret.setup(aabb_vertices, aabb_indices);

	return ret;
}

OpenGL::Model OpenGL::Model::load_from_file(MemoryContext memory, OpenGL* backend, String path, TextureDescription desc) {
	Model ret = {};
	Assimp::Importer importer;
	unsigned int assimp_flags = (
		aiProcess_Triangulate|aiProcess_FlipUVs | 
		aiProcess_PreTransformVertices|aiProcess_GenSmoothNormals | 
		aiProcess_JoinIdenticalVertices
	);
	// aiProcess_PreTransformVertices|

	INVARIENT_STRING_STRUCT_IS_HAS_NULL_TERMINTOR(path);
	const aiScene* scene = importer.ReadFile(path.data, assimp_flags);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		LOG_ERROR("ASSIMP ERROR: %s\n", importer.GetErrorString());
	}

	ret.meshes.reserve(scene->mNumMeshes);

	u64 path_length = String::cstr_length(path.data);
	int index = String::last_index_of(path.data, path_length, STRING_LIT_ARG("/"));
	if (index == -1) {
		index = String::last_index_of(path.data, path_length, STRING_LIT_ARG("\\"));
		RUNTIME_ASSERT(index != -1);
	}

	String directory = String(path.data, index);
	Hashmap<int, MaterialHandle> material_index_to_material_handle = {};
	for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial* ai_material = scene->mMaterials[i];
		Handle material_handle = backend->materials.acquire();
		Material& material = backend->materials.get(material_handle);
		material = Material::create(material_handle, memory.permanent_allocator, MaterialType::PBR);
		material.set_bool(STR(MATERIAL_HAS_ALBEDO_UNIFORM_NAME), load_assimp_texture(backend, &material, i, scene, directory, aiTextureType_DIFFUSE, STR(MATERIAL_ALBEDO_TEXTURE_UNIFORM_NAME), desc));

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
		*/

		if (ai_material->Get(AI_MATKEY_OPACITY, material.opacity) == AI_SUCCESS) {
			LOG_INFO("SUCCESS_MESH_HAS_OPACITY: %f\n", material.opacity);
		} else {
			LOG_WARN("Mesh Failed opacity matkey\n");
		}

		aiString alpha_mode;
		if (ai_material->Get(AI_MATKEY_GLTF_ALPHAMODE, alpha_mode) == AI_SUCCESS) {
			if (alpha_mode == aiString("BLEND")) {
				material.opacity = 0.20f;
			}
		}

		material_index_to_material_handle.put(i, MaterialHandle(material_handle));
	}

	ret.process_node(memory, backend, material_index_to_material_handle, scene->mRootNode, scene, convert_assimp_matrix_to_glm(scene->mRootNode->mTransformation));



	return ret;
}