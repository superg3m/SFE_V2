#include "render_types.hpp"

Material Material::create(MaterialHandle self, Allocator allocator, MaterialType type) {
	Material ret = {};

	ret.self = self;
	ret.type = type;
	ret.bindings = Hashmap<String, BindingValue>(allocator);

	return ret;
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