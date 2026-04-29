#include "../../../Core/core.hpp"
#include "../Handle/handle.hpp"

enum class BindingValueType {
	BOOL,
	INTEGER,
	FLOAT,
	TEXTURE_HANDLE,
	SAMPLER_2D,
	CUBEMAP,
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
        } else if constexpr (std::is_same_v<T, Handle<Texture>>) {
            this->type = BindingValueType::TEXTURE_HANDLE;
            this->texture_handle = value;
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
	static constexpr const char* DIFFUSE_TEXTURE = "uDiffuseTexture"; // texture unit = aiTextureType_DIFFUSE

	ShaderHandle shader = ShaderHandle::invalid();
	Hashmap<const char*, BindingValue> bindings;

	// Material() = default;
	Material create(ShaderHandle shader) {
		this->shader = shader;
	}

	void set_uniform(const char* name, BindingValue value) {
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
				RUNTIME_ASSERT(false);
			} break;

			case BindingValueType::SAMPLER_2D:
			case BindingValueType::CUBEMAP: {
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
	void set_bool(const char* name, bool value);
	void set_int(const char* name, int value);
	void set_float(const char* name, float value);
	void set_texture(const char* name, TextureHandle texture);
	void set_vec2(const char* name, const Vec2& value);
	void set_vec2(const char* name, float x, float y);
	void set_vec3(const char* name, const Vec3& value);
	void set_vec3(const char* name, float x, float y, float z);
	void set_vec4(const char* name, const Vec4& value);
	void set_vec4(const char* name, float x, float y, float z, float w);
	void set_mat4(const char* name, const Mat4& mat);
};
