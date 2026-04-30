#include "material.hpp"

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