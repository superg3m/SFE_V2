#pragma once

#include <core.hpp>
#include <texture.hpp>

struct MaterialKey {
    std::string name; // backpack.obj | uniform_green_color
    u32 material_index = 0; // for like backpack.obj and they have multiple
    
    bool operator==(const MaterialKey& other) const {
        return (this->name == other.name) && (this->material_index == other.material_index);
    }
};

auto material_key_hash = [](const MaterialKey& a){
    return std::hash<std::string>()(a.name) ^ std::hash<u32>()(a.material_index);
};

auto material_key_equal = [](const MaterialKey& a, const MaterialKey& b) {
    return a == b;
};

namespace OpenGL {
    struct Shader;
    enum class BindingValueType {
        BOOL,
        INTEGER,
        FLOAT,
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
            Texture sampler2d_binding;
            Texture cubemap_binding;
            glm::vec2 vector2_binding;
            glm::vec3 vector3_binding;
            glm::vec4 vector4_binding;
            glm::mat4 mat4_binding;
        };
    };

    struct Material {
        static constexpr const char* DIFFUSE_TEXTURE = "uDiffuseTexture"; // texture unit = aiTextureType_DIFFUSE

        static Material create(Shader* shader) {
            Material ret = {};
            ret.shader = shader;

            return ret;
        }

        Shader* shader = nullptr;
        std::unordered_map<std::string, BindingValue> bindings;

        void set_bool(const char* name, bool value);
        void set_int(const char* name, int value);
        void set_float(const char* name, float value);
        void set_texture(const char* name, Texture texture);
        void set_texture_cube(const char* name, Texture texture);
        void set_vec2(const char* name, const glm::vec2& value);
        void set_vec2(const char* name, float x, float y);
        void set_vec3(const char* name, const glm::vec3& value);
        void set_vec3(const char* name, float x, float y, float z);
        void set_vec4(const char* name, const glm::vec4& value);
        void set_vec4(const char* name, float x, float y, float z, float w);
        void set_mat4(const char* name, const glm::mat4& mat);
    };
}

using MaterialMap = std::unordered_map<MaterialKey, OpenGL::Material, decltype(material_key_hash), decltype(material_key_equal)>;