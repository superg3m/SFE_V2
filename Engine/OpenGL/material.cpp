#include <material.hpp>

std::function<std::size_t(const MaterialKey&)> MaterialKey::material_key_hash = [](const MaterialKey& a){
    return std::hash<std::string>()(a.name) ^ std::hash<u32>()(a.material_index);
};

std::function<bool(const MaterialKey&, const MaterialKey&)> MaterialKey::material_key_equal = [](const MaterialKey& a, const MaterialKey& b) {
    return a == b;
};

bool operator<(const MaterialKey& k1, const MaterialKey& k2) {
    return k1.material_index < k2.material_index;
}

namespace OpenGL {
    void Material::set_bool(const char* name, bool value) {
        BindingValue binding = {};
        binding.type = BindingValueType::BOOL;
        binding.boolean_binding = value;
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_int(const char* name, int value) {
        BindingValue binding = {};
        binding.type = BindingValueType::INTEGER;
        binding.integer_binding = value;
        this->bindings.insert(std::make_pair(name, binding));
    }

    void Material::set_float(const char* name, float value) {
        BindingValue binding = {};
        binding.type = BindingValueType::FLOAT;
        binding.float_binding = value;
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_texture(const char* name, Texture texture) {
        BindingValue binding = {};
        binding.type = BindingValueType::SAMPLER_2D;
        binding.sampler2d_binding = texture;
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_texture_cube(const char* name, Texture texture) {
        BindingValue binding = {};
        binding.type = BindingValueType::CUBEMAP;
        binding.cubemap_binding = texture;
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_vec2(const char* name, const glm::vec2& value) {
        BindingValue binding = {};
        binding.type = BindingValueType::VECTOR2;
        binding.vector2_binding = value;
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_vec2(const char* name, float x, float y) {
        BindingValue binding = {};
        binding.type = BindingValueType::VECTOR2;
        binding.vector2_binding = glm::vec2(x, y);
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_vec3(const char* name, const glm::vec3& value) {
        BindingValue binding = {};
        binding.type = BindingValueType::VECTOR3;
        binding.vector3_binding = value;
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_vec3(const char* name, float x, float y, float z) {
        BindingValue binding = {};
        binding.type = BindingValueType::VECTOR3;
        binding.vector3_binding = glm::vec3(x, y, z);
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_vec4(const char* name, const glm::vec4& value) {
        BindingValue binding = {};
        binding.type = BindingValueType::VECTOR4;
        binding.vector4_binding = value;
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_vec4(const char* name, float x, float y, float z, float w) {
        BindingValue binding = {};
        binding.type = BindingValueType::VECTOR4;
        binding.vector4_binding = glm::vec4(x, y, z, w);
        this->bindings.insert(std::make_pair(name, binding));
    }
    void Material::set_mat4(const char* name, const glm::mat4& mat) {
        BindingValue binding = {};
        binding.type = BindingValueType::MAT4;
        binding.mat4_binding = mat;
        this->bindings.insert(std::make_pair(name, binding));
    }
}
