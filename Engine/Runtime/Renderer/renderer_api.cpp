#pragma once

#include "renderer_api.hpp"

Material Material::create(Allocator allocator, ShaderHandle shader) {
	Material ret = {};

	ret.shader = shader;
	ret.bindings = Hashmap<String, BindingValue>(allocator);
}

void Material::set_uniform(String name, BindingValue value) {
	switch (value.type) {
		case BindingValueType::BOOL: {
			this->set_bool(name, value.boolean_binding);
		} break;

		case BindingValueType::INTEGER: {
			this->set_int(name, value.integer_binding);
		} break;

		case BindingValueType::FLOAT: {
			this->set_float(name, value.float_binding);
		} break;

		case BindingValueType::TEXTURE_HANDLE: {
			this->set_texture(name, value.texture_binding);
		} break;

		case BindingValueType::VECTOR2: {
			this->set_vec2(name, value.vector2_binding);
		} break;

		case BindingValueType::VECTOR3: {
			this->set_vec3(name, value.vector3_binding);
		} break;

		case BindingValueType::VECTOR4: {
			this->set_vec4(name, value.vector4_binding);
		} break;

		case BindingValueType::MAT4: {
			this->set_mat4(name, value.mat4_binding);
		} break;
	}
}

void Material::set_bool(String name, bool value) {
	this->bindings.put(name, value);
}

void Material::set_int(String name, int value) {
	this->bindings.put(name, value);
}

void Material::set_float(String name, float value) {
	this->bindings.put(name, value);
}

void Material::set_texture(String name, TextureHandle value) {
	this->bindings.put(name, value);
}

void Material::set_vec2(String name, const Vec2& value) {
	this->bindings.put(name, value);
}

void Material::set_vec2(String name, float x, float y) {
	this->bindings.put(name, Vec2(x, y));
}

void Material::set_vec3(String name, const Vec3& value) {
	this->bindings.put(name, value);
}

void Material::set_vec3(String name, float x, float y, float z) {
	this->bindings.put(name, Vec3(x, y, z));
}

void Material::set_vec4(String name, const Vec4& value) {
	this->bindings.put(name, value);
}

void Material::set_vec4(String name, float x, float y, float z, float w) {
	this->bindings.put(name, Vec4(x, y, z, w));
}

void Material::set_mat4(String name, const Mat4& value) {
	this->bindings.put(name, value);
}

struct TextureRequest {
	TextureHandle user = TextureHandle::invalid();
	u32 texture_unit = INT_MAX;
	String path = {};
	TextureDescription description = {};
	Vector<String> cubemap_paths = {};

	TextureRequest() = default;
	TextureRequest(TextureHandle user, u32 texture_unit, String path, TextureDescription description) {
		this->user = user;
		this->texture_unit = texture_unit;
		this->path = path;
		this->description = description;
	}

	TextureRequest(TextureHandle user, u32 texture_unit, Vector<String> cubemap_paths) {
		this->user = user;
		this->texture_unit = texture_unit;
		this->cubemap_paths = cubemap_paths;
	}
};

//  buffer.count * sizeof(T), buffer.data,
struct VertexBufferRequest {
	VertexBufferHandle user = VertexBufferHandle::invalid();

	MeshHandle mesh = MeshHandle::invalid();
	VertexLayout layout = {};
	void* data = nullptr; // make copy and free? would be easiest
	int count = 0;
	size_t element_size = 0; // sizeof(T)
	bool dynamic = false;
	size_t offset = 0;

	VertexBufferRequest() = default;
	VertexBufferRequest(VertexBufferHandle user, MeshHandle mesh, VertexLayout layout, void* data, int count, size_t element_size, bool dynamic) {
		this->user = user;
		this->mesh = mesh;
		this->layout = layout;
		this->data = data;
		this->count = count;
		this->element_size = element_size;
		this->dynamic = dynamic;
	}

	template<typename T>
	VertexBufferRequest(VertexBufferHandle user, MeshHandle mesh, VertexLayout layout, Vector<T> buffer, bool dynamic) {
		this->user = user;
		this->mesh = mesh;
		this->layout = layout;
		this->data = buffer.data;
		this->count = buffer.count;
		this->element_size = sizeof(T);
		this->dynamic = dynamic;
	}

	template<typename T>
	VertexBufferRequest(VertexBufferHandle user, MeshHandle mesh, Vector<T> buffer, size_t offset = 0) {
		this->user = user;
		this->mesh = mesh;
		this->data = buffer.data;
		this->count = buffer.count;
		this->element_size = sizeof(T);
		this->offset = offset;
	}

	VertexBufferRequest(VertexBufferHandle user, MeshHandle mesh, void* data, int count, size_t element_size, size_t offset = 0) {
		this->user = user;
		this->mesh = mesh;
		this->data = data;
		this->count = count;
		this->element_size = element_size;
		this->offset = offset;
	}

	VertexBufferRequest(VertexBufferHandle user, MeshHandle mesh) {
		this->user = user;
		this->mesh = mesh;
	}
};

//  buffer.count * sizeof(T), buffer.data,
struct IndexBufferRequest {
	IndexBufferHandle user = IndexBufferHandle::invalid();
	MeshHandle mesh = MeshHandle::invalid();
	u32* data = nullptr;
	size_t count = 0;
	bool dynamic = false;
};

struct DrawCallRequest {
	Pipeline pipeline = {};
	MeshHandle mesh = MeshHandle::invalid(); // mesh_entries, vao, vbo, ebo, material
	Mat4 model = Mat4::identity();
	Mat4 view = Mat4::identity();
	Mat4 projection = Mat4::identity();
	int instance_count = 1;
};

struct MeshRequest {
	MeshHandle user = MeshHandle::invalid();
	MaterialHandle material = MaterialHandle::invalid();
	ShaderHandle shader = ShaderHandle::invalid();
	String path = {};

	MeshRequest(MeshHandle user, MaterialHandle material) {
		this->user = user;
		this->material = material;
	}

	MeshRequest(MeshHandle user, ShaderHandle shader, String path) {
		this->user = user;
		this->shader = shader;
		this->path = path;
	}
};

// TODO(Jovanni): instead of user call it self?
// NOTE(Jovanni): user.handle means it will get updated by the backend
struct Request {
Request Request::create_texture(TextureHandle user, u32 texture_unit, String path, TextureDescription description) {
	Request ret = {};
	ret.type = RequestType::TEXTURE2D_LOAD;
	ret.texture = TextureRequest(user, texture_unit, path, description);

	return ret;
}

Request Request::create_texture(TextureHandle user, u32 texture_unit, Vector<String> cubemap_paths) {
	Request ret = {};
	ret.type = RequestType::TEXTURE3D_LOAD;
	ret.texture = TextureRequest(user, texture_unit, cubemap_paths);

	return ret;
}

template<typename T>
Request Request::create_vertex_buffer(VertexBufferHandle user, MeshHandle mesh, VertexLayout layout, Vector<T>& buffer, bool dynamic = false) {
	Request ret = {};
	ret.type = RequestType::VBO_CREATE;
	ret.vbo = VertexBufferRequest(user, mesh, layout, buffer, dynamic);

	return ret;
}

Request Request::bind_vertex_buffer(VertexBufferHandle user, MeshHandle mesh) {
	Request ret = {};
	ret.type = RequestType::VBO_BIND;
	ret.vbo = VertexBufferRequest(user, mesh);

	return ret;
}


Request Request::create_mesh(MeshHandle user, ShaderHandle shader, String path) {
	Request ret = {};
	ret.type = RequestType::MESH_LOAD;
	ret.mesh = MeshRequest(user, shader, path);

	return ret;
}

Request Request::create_mesh_cube(MeshHandle user, MaterialHandle material) {
	Request ret = {};
	ret.type = RequestType::MESH_CUBE_CREATE;
	ret.mesh = ret.mesh = MeshRequest(user, material);

	return ret;
}

Request Request::create_shader(ShaderHandle user, Vector<String> shader_paths) {
	Request ret = {};
	ret.type = RequestType::SHADER_CREATE;
	ret.shader.user = user;
	ret.shader.shader_paths = shader_paths;

	return ret;
}

Request Request::shader_recompile(ShaderHandle user) {
	Request ret = {};
	ret.type = RequestType::SHADER_RECOMPILE;
	ret.shader.user = user;

	return ret;
}
};