#pragma once

#include "../../Core/core.hpp"

struct DepthState {
	bool depth_testing = true;
	bool depth_write = true;
	// func
};

struct BlendState {
	bool enabled = false;
	// source = src_alpha;
	// destination = one_minus_source_alpha;
};

struct RasterizerState {
	bool cull_enabled = true;
	bool cull_face_back = true;
	bool ccw_winding = true;
	bool fill = true;
};

struct Pipeline {
	RasterizerState rasterizer = {};
	DepthState depth = {};
	BlendState blend = {};
	
	bool operator==(Pipeline& other) const {
		return Memory::equal(this, sizeof(Pipeline), &other, sizeof(Pipeline));
	}
};

enum class TextureSamplerType {
	SAMPLER_2D,
	CUBEMAP_3D
};

struct TextureDescription {
	bool pixel_perfect = false;
	bool vertical_flip = true;
	TextureSamplerType type = TextureSamplerType::SAMPLER_2D;
};

struct CommandBufferHandle {
	Handle handle = Handle::invalid();
	CommandBufferHandle(Handle handle) {
		this->handle = handle;
	}

	static CommandBufferHandle invalid() {
		return {Handle::invalid()};
	}
};

struct TextureHandle {
	Handle handle = Handle::invalid();
	TextureHandle(Handle handle) {
		this->handle = handle;
	}

	static TextureHandle invalid() {
		return {Handle::invalid()};
	}
};

struct MaterialHandle {
	Handle handle = Handle::invalid();
	MaterialHandle(Handle handle) {
		this->handle = handle;
	}

	static MaterialHandle invalid() {
		return {Handle::invalid()};
	}
};

struct ShaderHandle {
	Handle handle = Handle::invalid();
	ShaderHandle() = default;
	ShaderHandle(Handle handle) {
		this->handle = handle;
	}

	static ShaderHandle invalid() {
		return {Handle::invalid()};
	}
};

struct MeshHandle {
	Handle handle = Handle::invalid();
	MeshHandle(Handle handle) {
		this->handle = handle;
	}
	
	static MeshHandle invalid() {
		return {Handle::invalid()};
	}
};

struct VertexBufferHandle {
	Handle handle = Handle::invalid();
	VertexBufferHandle(Handle handle) {
		this->handle = handle;
	}

	static VertexBufferHandle invalid() {
		return {Handle::invalid()};
	}
};

struct IndexBufferHandle {
	Handle handle = Handle::invalid();
	IndexBufferHandle(Handle handle) {
		this->handle = handle;
	}

	static IndexBufferHandle invalid() {
		return {Handle::invalid()};
	}
};

enum class BindingValueType {
	BOOL,
	INTEGER,
	FLOAT,
	TEXTURE_HANDLE,
	VECTOR2,
	VECTOR3,
	VECTOR4,
	MAT4,
};

struct BindingValue {
	BindingValueType type;
	union {
		bool boolean_binding;
		int integer_binding;
		float float_binding;
		TextureHandle texture_binding;
		Vec2 vector2_binding;
		Vec3 vector3_binding;
		Vec4 vector4_binding;
		Mat4 mat4_binding;
	};

	template<typename T>
	BindingValue(T value) {
		if constexpr (std::is_same_v<T, bool>) {
			this->type = BindingValueType::BOOL;
			this->boolean_binding = value;
		} else if constexpr (std::is_same_v<T, int>) {
			this->type = BindingValueType::INTEGER;
			this->integer_binding = value;
		} else if constexpr (std::is_same_v<T, float>) {
			this->type = BindingValueType::FLOAT;
			this->float_binding = value;
		} else if constexpr (std::is_same_v<T, TextureHandle>) {
			this->type = BindingValueType::TEXTURE_HANDLE;
			this->texture_binding = value;
		} else if constexpr (std::is_same_v<T, Vec2>) {
			this->type = BindingValueType::VECTOR2;
			this->vector2_binding = value;
		} else if constexpr (std::is_same_v<T, Vec3>) {
			this->type = BindingValueType::VECTOR3;
			this->vector3_binding = value;
		} else if constexpr (std::is_same_v<T, Vec4>) {
			this->type = BindingValueType::VECTOR4;
			this->vector4_binding = value;
		} else if constexpr (std::is_same_v<T, Mat4>) {
			this->type = BindingValueType::MAT4;
			this->mat4_binding = value;
		} else {
			STATIC_ASSERT(false, "Unsupported BindingValue type");
		}
	}
};

struct Material {
	static constexpr String DIFFUSE_TEXTURE = STR("uDiffuseTexture"); // texture unit = aiTextureType_DIFFUSE
	ShaderHandle shader = ShaderHandle::invalid();
	Hashmap<String, BindingValue> bindings;

	static Material create(Allocator allocator, ShaderHandle shader);
	void set_uniform(String name, BindingValue value);
	void set_bool(String name, bool value);
	void set_int(String name, int value);
	void set_float(String name, float value);
	void set_texture(String name, TextureHandle texture);
	void set_vec2(String name, const Vec2& value);
	void set_vec2(String name, float x, float y);
	void set_vec3(String name, const Vec3& value);
	void set_vec3(String name, float x, float y, float z);
	void set_vec4(String name, const Vec4& value);
	void set_vec4(String name, float x, float y, float z, float w);
	void set_mat4(String name, const Mat4& mat);
};

// NOTE(Jovanni): this is size in float
enum class BufferStrideTypeInfo {
	BOOL = 1,
	INT = 1,
	FLOAT = 1, 
	VEC2 = 2,
	VEC3 = 3,
	VEC4 = 4, 
	IVEC4 = 4, 
	MAT4 = 16
};

const float MAGIC_NUMBER = -123450510.0f;
const int RESERVED_VERTEX_ATTRIBUTE_LOCATIONS = 4;

struct Vertex {
	Vec3 aPosition    = Vec3(MAGIC_NUMBER);   // location 0
	Vec3 aNormal      = Vec3(MAGIC_NUMBER);   // location 1
	Vec2 aTexCoord    = Vec2(MAGIC_NUMBER);   // location 2
	Vec3 aColor       = Vec3(MAGIC_NUMBER);   // location 3
};

struct VertexAttribute {
	u32 location = 0;
	size_t offset = 0; // offset in bytes of the actual memeber! OFFSET_OF(Vertex, aPosition)
	BufferStrideTypeInfo type;
	bool instanced = false;
};

struct VertexLayout {
	u32 stride;
	u32 stride_in_floats; // stride / sizeof(float)
	Vector<VertexAttribute> attributes;

	static VertexLayout& PNTC() {
		static VertexLayout layout = VertexLayout({
			VertexAttribute{0, OFFSET_OF(Vertex, aPosition), BufferStrideTypeInfo::VEC3, false},
			VertexAttribute{1, OFFSET_OF(Vertex, aNormal), BufferStrideTypeInfo::VEC3, false},
			VertexAttribute{2, OFFSET_OF(Vertex, aTexCoord), BufferStrideTypeInfo::VEC2, false},
			VertexAttribute{3, OFFSET_OF(Vertex, aColor), BufferStrideTypeInfo::VEC3, false},
		});

		return layout;
	}

	VertexLayout() = default;
	VertexLayout(Vector<VertexAttribute> attributes) {
		this->stride = 0;
		for (VertexAttribute desc : attributes) {
			this->stride += (u32)desc.type * sizeof(float);
		}

		this->stride_in_floats = this->stride / sizeof(float);
		this->attributes = attributes;
	}
};

enum class RequestType {
	MESH_LOAD,
	MESH_CUBE_CREATE,
	MESH_AABB_CREATE,

	TEXTURE2D_LOAD,
	TEXTURE3D_LOAD,
	VBO_CREATE,
	VBO_BIND,
	VBO_UPDATE,
	EBO_UPDATE,

	SHADER_CREATE,
	SHADER_RECOMPILE,

	DRAW_CALL
};

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

	DrawCallRequest(Pipeline pipeline, MeshHandle mesh, Mat4 model, Mat4 view, Mat4 projection, int instance_count = 1) {
		this->pipeline = pipeline;
		this->mesh = mesh;
		this->model = model;
		this->view = view;
		this->projection = projection;
		this->instance_count =instance_count ;
	}
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

struct ShaderRequest {
	ShaderHandle user;
	Vector<String> shader_paths = {};

	ShaderRequest(ShaderHandle user, Vector<String> shader_paths) {
		this->user = user;
		this->shader_paths = shader_paths;
	}
};

// TODO(Jovanni): instead of user call it self?
// NOTE(Jovanni): user.handle means it will get updated by the backend
struct Request {
	RequestType type;
	union {
		TextureRequest texture;
		VertexBufferRequest vbo;
		IndexBufferRequest ebo;
		MeshRequest mesh;
		ShaderRequest shader;
		DrawCallRequest draw_call;
	};

	static Request create_texture(TextureHandle user, u32 texture_unit, String path, TextureDescription description) {
		Request ret = {};
		ret.type = RequestType::TEXTURE2D_LOAD;
		ret.texture = TextureRequest(user, texture_unit, path, description);

		return ret;
	}

	static Request create_texture(TextureHandle user, u32 texture_unit, Vector<String> cubemap_paths) {
		Request ret = {};
		ret.type = RequestType::TEXTURE3D_LOAD;
		ret.texture = TextureRequest(user, texture_unit, cubemap_paths);

		return ret;
	}

	template<typename T>
	static Request create_vertex_buffer(VertexBufferHandle user, MeshHandle mesh, VertexLayout layout, Vector<T>& buffer, bool dynamic = false) {
		Request ret = {};
		ret.type = RequestType::VBO_CREATE;
		ret.vbo = VertexBufferRequest(user, mesh, layout, buffer, dynamic);

		return ret;
	}

	static Request bind_vertex_buffer(VertexBufferHandle user, MeshHandle mesh) {
		Request ret = {};
		ret.type = RequestType::VBO_BIND;
		ret.vbo = VertexBufferRequest(user, mesh);

		return ret;
	}


	static Request create_mesh(MeshHandle user, ShaderHandle shader, String path) {
		Request ret = {};
		ret.type = RequestType::MESH_LOAD;
		ret.mesh = MeshRequest(user, shader, path);

		return ret;
	}

	static Request create_mesh_cube(MeshHandle user, MaterialHandle material) {
		Request ret = {};
		ret.type = RequestType::MESH_CUBE_CREATE;
		ret.mesh = MeshRequest(user, material);

		return ret;
	}

	static Request create_shader(ShaderHandle user, Vector<String> shader_paths) {
		Request ret = {};
		ret.type = RequestType::SHADER_CREATE;
		ret.shader.user = user;
		ret.shader.shader_paths = shader_paths;

		return ret;
	}

	static Request shader_recompile(ShaderHandle user) {
		Request ret = {};
		ret.type = RequestType::SHADER_RECOMPILE;
		ret.shader.user = user;

		return ret;
	}

	static Request create_draw_call(Pipeline pipeline, MeshHandle mesh, Mat4 model, Mat4 view, Mat4 projection, u32 instance_count = 1) {
		Request ret = {};
		ret.type = RequestType::DRAW_CALL;
		ret.draw_call = DrawCallRequest(pipeline, mesh, model, view, projection, instance_count);

		return ret;
	}
};

typedef VertexBufferHandle(AcquireVertexBufferFunc)(void* b);
typedef MeshHandle(AcquireMeshFunc)(void* b);
typedef ShaderHandle(AcquireShaderFunc)(void* b);
typedef MaterialHandle(AcquireMaterialFunc)(void* b, ShaderHandle shader);
typedef TextureHandle(AcquireTextureFunc)(void* b);
typedef void(ExecuteRequests)(void* b, Vector<Request>& requests, MemoryContext memory);

struct RenderAPI {
	void* b = nullptr;
	Vector<Request> requests = {};
	MemoryContext memory = {};

	AcquireVertexBufferFunc* _private_acquire_vbo_handle = nullptr;
    AcquireMeshFunc*         _private_acquire_mesh_handle = nullptr;
    AcquireShaderFunc*       _private_acquire_shader_handle = nullptr;
    AcquireMaterialFunc*     _private_acquire_material_handle = nullptr;
    AcquireTextureFunc*      _private_acquire_texture_handle = nullptr;
	ExecuteRequests* 		 _private_execute_requests = nullptr;

	template<typename T>
	VertexBufferHandle create_vbo(MeshHandle mesh, VertexLayout layout, Vector<T>& buffer, bool dynamic = false) {
		VertexBufferHandle vbo = this->_private_acquire_vbo_handle(this->b);
		Request request = Request::create_vertex_buffer(vbo, mesh, layout, buffer, dynamic);
		this->requests.append(request);

		return vbo;
	}

	MeshHandle create_mesh(ShaderHandle shader, String path) {
		MeshHandle mesh = this->_private_acquire_mesh_handle(this->b);
		Request request = Request::create_mesh(mesh, shader, path);
		this->requests.append(request);
		return mesh;
	}

	MeshHandle create_mesh_cube(MaterialHandle material) {
		MeshHandle mesh = this->_private_acquire_mesh_handle(this->b);
		Request request = Request::create_mesh_cube(mesh, material);
		this->requests.append(request);
		return mesh;
	}

	ShaderHandle create_shader(std::initializer_list<String> shader_paths) {
		ShaderHandle shader = this->_private_acquire_shader_handle(this->b);
		Request request = Request::create_shader(shader, Vector<String>(shader_paths, this->memory.permanent_allocator));
		this->requests.append(request);
		return shader;
	}

	TextureHandle create_texture(u32 texture_unit, String path, TextureDescription description) {
		TextureHandle texture = this->_private_acquire_texture_handle(this->b);
		Request request = Request::create_texture(texture, texture_unit, path, description);
		this->requests.append(request);
		return texture;
	}

	MaterialHandle create_material(ShaderHandle shader) {
		MaterialHandle material = this->_private_acquire_material_handle(this->b, shader);
		return material;
	}

	void execute_request() {
		this->_private_execute_requests(this->b, this->requests, this->memory);
		this->requests.clear();
	}

	// material_set_uniforms

	void draw_mesh(Pipeline pipeline, MeshHandle mesh, Mat4 model, Mat4 view, Mat4 projection, u32 instance_count = 1) {
		Request request = Request::create_draw_call(pipeline, mesh, model, view, projection, instance_count);
		this->requests.append(request);
	}

	/*
	void material_set_uniforms(MaterialHandle material, std::initializer_list<KeyValuePair<String, BindingValue>> uniforms) {
		Material& material_slot = this->backend.materials.get(material.handle);
		for (auto& entry : uniforms) {
			String k = entry.key;
			BindingValue v = entry.value;
			material_slot.set_uniform(k, v);
		}
	}

	void shader_recompile(ShaderHandle shader) {
		Request request = Request::shader_recompile(shader);
		this->requests.append(request);
	}
	*/
};