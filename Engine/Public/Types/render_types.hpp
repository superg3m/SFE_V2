#pragma once

#include "../Core/core.hpp"

// --------------------------------------------------
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

	bool operator==(MaterialHandle& other) const {
		return this->handle == other.handle;
	}
};

struct MeshHandle {
	Handle handle = Handle::invalid();
	MaterialHandle original_material = MaterialHandle::invalid();
	MeshHandle(Handle handle, MaterialHandle material) {
		this->handle = handle;
		this->original_material = material; // NOTE(Jovanni): Material associated with this mesh
	}
	
	static MeshHandle invalid() {
		return {Handle::invalid(), MaterialHandle::invalid()};
	}
};

struct ModelHandle {
	Handle handle = Handle::invalid();
	ModelHandle(Handle handle) {
		this->handle = handle;
	}
	
	static ModelHandle invalid() {
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
// --------------------------------------------------


// --------------------------------------------------
enum class TextureSamplerType {
	SAMPLER_2D,
	CUBEMAP_3D
};

struct TextureDescription {
	bool pixel_perfect = false;
	bool vertical_flip = false;
	TextureSamplerType type = TextureSamplerType::SAMPLER_2D;
};

enum class MaterialType {
	PBR,
	UNLIT
};

struct MaterialDescription {
	MaterialType type = MaterialType::PBR;
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

	BindingValue() {}

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

/* SHADER REPRESENTATION
struct Material {
	sampler2D albedo;
	float metallic;
	float roughness;
	float ao;

	float opacity = 1.0; // 1.0 fully opaque, 0.0 full transparent
};
*/

#define MATERIAL_ALBEDO_TEXTURE_UNIFORM_NAME "uMaterial.albedo"
#define MATERIAL_HAS_ALBEDO_UNIFORM_NAME "uMaterial.has_albedo"

#define MATERIAL_METALLIC_UNIFORM_NAME "uMaterial.metallic"
#define MATERIAL_ROUGHNESS_UNIFORM_NAME "uMaterial.roughness"
#define MATERIAL_OPACITY_UNIFORM_NAME "uMaterial.opacity"

struct Material {
	MaterialType type = MaterialType::PBR; 
	MaterialHandle self = MaterialHandle::invalid();
	Hashmap<String, BindingValue> bindings;

	float opacity = 1.0; // 1.0 fully opaque, 0.0 full transparent
	float metallic = 1.0; // 1.0 fully metallic, 0.0 not metallic
	float roughness = 1.0; // 1.0 fully rough, 0.0 not rough

	static Material create(MaterialHandle self, Allocator allocator, MaterialType type);
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

	static void copy(Material* dest, Material* source) {
		*dest = *source;
		Hashmap<String, BindingValue>::copy(&dest->bindings, &source->bindings);
	}
};
// --------------------------------------------------

// --------------------------------------------------
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
// --------------------------------------------------


// --------------------------------------------------
struct RasterizerDescription {
	bool cull_enabled = false;
	bool cull_face_back = true;
	bool ccw_winding = true;
	bool wireframe = false;
};

// --------------------------------------------------


// --------------------------------------------------
struct Texture {
	u32 width = 0;
	u32 height = 0;
	TextureSamplerType type = TextureSamplerType::SAMPLER_2D;
	Handle internal; // the backend owns this
};
// --------------------------------------------------