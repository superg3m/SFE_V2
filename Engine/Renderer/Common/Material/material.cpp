#include "material.hpp"

void Material::set_bool(const char* name, bool value) {
    this->bindings.put(name, value);
}

void Material::set_int(const char* name, int value) {
    this->bindings.put(name, value);
}

void Material::set_float(const char* name, float value) {
    this->bindings.put(name, value);
}

void Material::set_texture(const char* name, TextureHandle value) {
    LOG_ERROR("SET_TEXTURE???\n");
    this->bindings.put(name, value);
    LOG_ERROR("AFTER SET_TEXTURE???\n");
}

void Material::set_vec2(const char* name, const Vec2& value) {
    this->bindings.put(name, value);
}

void Material::set_vec2(const char* name, float x, float y) {
    this->bindings.put(name, Vec2(x, y));
}

void Material::set_vec3(const char* name, const Vec3& value) {
    this->bindings.put(name, value);
}

void Material::set_vec3(const char* name, float x, float y, float z) {
    this->bindings.put(name, Vec3(x, y, z));
}

void Material::set_vec4(const char* name, const Vec4& value) {
    this->bindings.put(name, value);
}

void Material::set_vec4(const char* name, float x, float y, float z, float w) {
    this->bindings.put(name, Vec4(x, y, z, w));
}

void Material::set_mat4(const char* name, const Mat4& value) {
    this->bindings.put(name, value);
}