#include "material.hpp"

void Material::set_bool(const char* name, bool value) {
    BindingValue binding = BindingValue(value);
    this->bindings.put(name, binding);
}

void Material::set_int(const char* name, int value) {
    BindingValue binding = BindingValue(value);
    this->bindings.put(name, binding);
}

void Material::set_float(const char* name, float value) {
    BindingValue binding = BindingValue(value);
    this->bindings.put(name, binding);
}

void Material::set_texture(const char* name, Texture value) {
    BindingValue binding = BindingValue(value);
    this->bindings.put(name, binding);
}

void Material::set_vec2(const char* name, const Vec2& value) {
    BindingValue binding = BindingValue(value);
    this->bindings.put(name, binding);
}

void Material::set_vec2(const char* name, float x, float y) {
    BindingValue binding = BindingValue(Vec2(x, y));
    this->bindings.put(name, binding);
}

void Material::set_vec3(const char* name, const Vec3& value) {
    BindingValue binding = BindingValue(value);
    this->bindings.put(name, binding);
}

void Material::set_vec3(const char* name, float x, float y, float z) {
    BindingValue binding = BindingValue(Vec3(x, y, z));
    this->bindings.put(name, binding);
}

void Material::set_vec4(const char* name, const Vec4& value) {
    BindingValue binding = BindingValue(value);
    this->bindings.put(name, binding);
}

void Material::set_vec4(const char* name, float x, float y, float z, float w) {
    BindingValue binding = BindingValue(Vec4(x, y, z, w));
    this->bindings.put(name, binding);
}

void Material::set_mat4(const char* name, const Mat4& value) {
    BindingValue binding = BindingValue(value);
    this->bindings.put(name, binding);
}