#include "backend.hpp"

Vector<String> OpenGL::Shader::fallback_paths = Vector<String>({
	STR("../../../Game/Assets/Shaders/fallback.vert"),
	STR("../../../Game/Assets/Shaders/fallback.frag"),
});

u32 OpenGL::Shader::create_shader_program(Vector<String> shader_paths) {
	u32 shader_program_id = glCreateProgram();

	u32 shader_source_count = 0;
	u32 shader_source_ids[8] = {};
	for (String path : shader_paths) {
		INVARIENT_STRING_STRUCT_IS_HAS_NULL_TERMINTOR(path);
		u32 shader_source_id = this->shader_source_compile(path);
		gl_error_check(glAttachShader(shader_program_id, shader_source_id));
		shader_source_ids[shader_source_count++] = shader_source_id;
	}
	gl_error_check(glLinkProgram(shader_program_id));

	int success = false;
	gl_error_check(glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success));
	if (!success) {
		char info_log[1028] = {0};
		gl_error_check(glGetProgramInfoLog(shader_program_id, 512, NULL, info_log));
		LOG_ERROR("LINKING_FAILED {%s}\n", shader_paths[0].data);
		LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);

		if (!using_fallback) {
			LOG_INFO("Using Fallback\n", info_log);
			this->using_fallback = true;

			for (int i = 0; i < shader_source_count; i++) {
				gl_error_check(glDeleteShader(shader_source_ids[i]));
			}

			return OpenGL::Shader::create_shader_program(fallback_paths);
		}
		
		return 0;
	}

	for (int i = 0; i < shader_source_count; i++) {
		gl_error_check(glDeleteShader(shader_source_ids[i]));
	}

	int uniform_count = 0;
	gl_error_check(glGetProgramiv(shader_program_id, GL_ACTIVE_UNIFORMS, &uniform_count));
	for (int i = 0; i < uniform_count; i++) {
		int size;
		GLenum type;
		const GLsizei name_max_size = 256;
		GLchar name[name_max_size];
		GLsizei name_length;
		gl_error_check(glGetActiveUniform(shader_program_id, (u32)i, name_max_size, &name_length, &size, &type, name));

		int location = gl_error_check(glGetUniformLocation(shader_program_id, name));
		UniformDesc value = UniformDesc{type, location};
		this->uniforms.put(String::create(name, name_length), value);
	}

	return shader_program_id;
}

void OpenGL::Shader::compile() {
	this->uniforms.clear();
	this->using_fallback = false;
	this->program_id = this->create_shader_program(this->shader_paths);
}

static const char* gl_enum_to_string(GLenum type) {
	switch (type) {
		case GL_BOOL:          return "bool";
		case GL_INT:           return "int";
		case GL_FLOAT:         return "float";
		case GL_FLOAT_VEC2:    return "vec2";
		case GL_FLOAT_VEC3:    return "vec3";
		case GL_FLOAT_VEC4:    return "vec4";
		case GL_FLOAT_MAT4:    return "mat4";
		case GL_SAMPLER_2D:    return "sampler2D";
		case GL_SAMPLER_CUBE:  return "samplerCube";
		
		default:               return "unknown";
	}
}

// TODO(Jovanni): think about shader header
/*
- [] Shader Header is a really interesting idea
	u8* shader_header_data = Platform::read_entire_file("shader_header.h")
	u8* shader_vert_source = Platform::read_entire_file("shader.vert")
	u8* shader_frag_source = Platform::read_entire_file("shader.frag")
	String::append(shader_header_data, length, shader_vert_source);
	String::append(shader_header_data, length, shader_frag_source);

	shader_header.h: {
		vec3 CalcDirLight(const DirLight light, const vec3 normal, const vec3 viewDir, const vec2 texCoord) {
			vec3 lightDir = normalize(-light.direction); // Directional light direction is inverse of uniform value

			// Diffuse calculation
			float diffuse_factor = max(dot(normal, lightDir), 0.0f);

			// Specular calculation
			vec3 reflectDir = reflect(-lightDir, normal); // Light direction, not negative light direction
			float specular_factor = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);

			// Get material colors
			vec3 diffuse_mat_color = getDiffuseColor(texCoord);
			vec3 specular_mat_color = getSpecularColor(texCoord);

			// Combine light and material components
			vec3 ambient = light.ambient * (uMaterial.ambient_color + diffuse_mat_color); // Ambient usually uses diffuse color as well
			vec3 diffuse = light.diffuse * diffuse_factor * diffuse_mat_color;
			vec3 specular = light.specular * specular_factor * specular_mat_color;

			return (ambient + diffuse + specular);
		}

		vec3 CalcPointLight(const PointLight light, const vec3 normal, const vec3 fragPos, const vec3 viewDir, const vec2 texCoord) {
			vec3 lightDir = normalize(light.position - fragPos);

			// Diffuse calculation
			float diffuse_factor = max(dot(normal, lightDir), 0.0f);

			// Specular calculation
			vec3 reflectDir = reflect(-lightDir, normal);
			float specular_factor = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);

			// Attenuation calculation (fixed float suffixes)
			float distance = length(light.position - fragPos);
			float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

			// Get material colors
			vec3 diffuse_mat_color = getDiffuseColor(texCoord);
			vec3 specular_mat_color = getSpecularColor(texCoord);

			// Combine light and material components
			vec3 ambient = light.ambient * (uMaterial.ambient_color + diffuse_mat_color);
			vec3 diffuse = light.diffuse * diffuse_factor * diffuse_mat_color;
			vec3 specular = light.specular * specular_factor * specular_mat_color;

			// Apply attenuation
			ambient *= attenuation;
			diffuse *= attenuation;
			specular *= attenuation;

			return (ambient + diffuse + specular);
		}

		vec3 CalcSpotLight(const SpotLight light, const vec3 normal, const vec3 fragPos, const vec3 viewDir, const vec2 texCoord) {
			vec3 lightDir = normalize(light.position - fragPos);

			// Diffuse calculation
			float diffuse_factor = max(dot(normal, lightDir), 0.0f);

			// Specular calculation
			vec3 reflectDir = reflect(-lightDir, normal);
			float specular_factor = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);

			// Attenuation calculation (fixed float suffixes)
			float distance = length(light.position - fragPos);
			float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

			// Spotlight effect
			float theta = dot(lightDir, normalize(-light.direction)); // Angle between fragment-to-light and spotlight direction
			float epsilon = light.cutOff - light.outerCutOff; // Blending range
			float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f); // Smooth step for falloff

			// Get material colors
			vec3 diffuse_mat_color = getDiffuseColor(texCoord);
			vec3 specular_mat_color = getSpecularColor(texCoord);

			// Combine light and material components
			vec3 ambient = light.ambient * (uMaterial.ambient_color + diffuse_mat_color);
			vec3 diffuse = light.diffuse * diffuse_factor * diffuse_mat_color;
			vec3 specular = light.specular * specular_factor * specular_mat_color;

			// Apply attenuation and spotlight intensity
			ambient *= attenuation * intensity;
			diffuse *= attenuation * intensity;
			specular *= attenuation * intensity;

			return (ambient + diffuse + specular);
		}
	}
*/
GLenum OpenGL::Shader::type_from_path(String shader_source_path) {
	s64 extension_index = String::last_index_of(shader_source_path.data, shader_source_path.length, STRING_LIT_ARG("."));
	RUNTIME_ASSERT_MSG(extension_index != -1, "Missing extension (.vert, .frag)\n");
	
	String extension = String(shader_source_path.data + extension_index, shader_source_path.length - extension_index);
	if (String::contains(extension.data, extension.length,  STRING_LIT_ARG(".vert"))) {
		return GL_VERTEX_SHADER;
	} else if (String::contains(extension.data, extension.length,  STRING_LIT_ARG(".frag"))) {
		return GL_FRAGMENT_SHADER;
	} else if (String::contains(extension.data, extension.length,  STRING_LIT_ARG(".gs"))) {
		return GL_GEOMETRY_SHADER;
	}

	RUNTIME_ASSERT_MSG(false, "Unsupported extension: %s | Expected: (.vert, .frag, .gs)\n", shader_source_path);
	return GL_INVALID_ENUM;
}

void OpenGL::Shader::check_compile_error(unsigned int source_id, String path) {
	int success;
	char info_log[1024];

	gl_error_check(glGetShaderiv(source_id, GL_COMPILE_STATUS, &success));
	if (!success) {
		gl_error_check(glGetShaderInfoLog(source_id, 1024, NULL, info_log));
		INVARIENT_STRING_STRUCT_IS_HAS_NULL_TERMINTOR(path);
		LOG_ERROR("ERROR::SHADER_COMPILATION_ERROR {%s}\n", path.data);
		LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);
	}
}

unsigned int OpenGL::Shader::shader_source_compile(String path) {
	size_t file_size = 0;
	Error error = Error::SUCCESS;
	Allocator allocator = Allocator::general();
	INVARIENT_STRING_STRUCT_IS_HAS_NULL_TERMINTOR(path);
	GLchar* shader_source = (GLchar*)Platform::read_entire_file(allocator, path.data, file_size, error);
	RUNTIME_ASSERT_MSG(error == Error::SUCCESS, "Shader Error: %s | Err: %s\n", path.data, error_get_string(error));

	GLenum type = this->type_from_path(path);
	unsigned int source_id = glCreateShader(type);
	gl_error_check(glShaderSource(source_id, 1, &shader_source, NULL));
	gl_error_check(glCompileShader(source_id));

	allocator.free(shader_source);
	this->check_compile_error(source_id, path);
	
	return source_id;
}

unsigned int OpenGL::Shader::get_uniform_location(String name, GLenum type) {
	if (this->uniforms.has(name)) {
		UniformDesc expected = this->uniforms.get(name);
		if (expected.type != type) {
			LOG_ERROR("Shader {%s} Uniform: '%s' type missmatch | Expected: %s | Got: %s\n", this->shader_paths[0].data, name.data, gl_enum_to_string(expected.type), gl_enum_to_string(type));
			return (unsigned int)-1;
		}

		return expected.location;
	}

	INVARIENT_STRING_STRUCT_IS_HAS_NULL_TERMINTOR(name);
	int location = gl_error_check(glGetUniformLocation(this->program_id, name.data));
	this->uniforms.put(name, UniformDesc{type, location}); // this type might be wrong, but theres no great robust way to do arrays that I know of...
	if (location == -1) {
		LOG_ERROR("Shader {%s} Uniform: '%s' does not exists\n", this->shader_paths[0].data, name.data);
	}

	return location;
}

void OpenGL::Shader::use() const {
	gl_error_check(glUseProgram(this->program_id));
}

void OpenGL::Shader::set_model(Mat4 &model) {
	this->set_mat4(STR("uModel"), model);
}

void OpenGL::Shader::set_view(Mat4 &view) {
	this->set_mat4(STR("uView"), view);
}

void OpenGL::Shader::set_projection(Mat4 &projection) {
	this->set_mat4(STR("uProjection"), projection);
}

void OpenGL::Shader::set_bool(String name, bool value) {
	gl_error_check(glUniform1i(this->get_uniform_location(name, GL_BOOL), (int)value));
}
void OpenGL::Shader::set_int(String name, int value) {
	gl_error_check(glUniform1i(this->get_uniform_location(name, GL_INT), value));
}
void OpenGL::Shader::set_texture(String name, int texture_unit, Texture texture) {
	RUNTIME_ASSERT(texture.type == TextureSamplerType::SAMPLER_2D);

	gl_error_check(glActiveTexture(GL_TEXTURE0 + texture_unit));
	gl_error_check(glBindTexture(GL_TEXTURE_2D, texture.id));

	gl_error_check(glUniform1i(this->get_uniform_location(name, GL_SAMPLER_2D), texture_unit));
}
void OpenGL::Shader::set_texture_cube(String name, int texture_unit, Texture texture) {
	RUNTIME_ASSERT(texture.type == TextureSamplerType::CUBEMAP_3D);

	gl_error_check(glActiveTexture(GL_TEXTURE0 + texture_unit));
	gl_error_check(glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id));
 
	gl_error_check(glUniform1i(this->get_uniform_location(name, GL_SAMPLER_CUBE), texture_unit));
}
void OpenGL::Shader::set_float(String name, float value) {
	gl_error_check(glUniform1f(this->get_uniform_location(name, GL_FLOAT), value));
}
void OpenGL::Shader::set_vec2(String name, const Vec2& value) {
	gl_error_check(glUniform2fv(this->get_uniform_location(name, GL_FLOAT_VEC2), 1, &value.x));
}
void OpenGL::Shader::set_vec2(String name, float x, float y) {
	gl_error_check(glUniform2f(this->get_uniform_location(name, GL_FLOAT_VEC2), x, y));
}
void OpenGL::Shader::set_vec3(String name, const Vec3& value) {
	gl_error_check(glUniform3fv(this->get_uniform_location(name, GL_FLOAT_VEC3), 1, &value.x));
}
void OpenGL::Shader::set_vec3(String name, float x, float y, float z) {
	gl_error_check(glUniform3f(this->get_uniform_location(name, GL_FLOAT_VEC3), x, y, z));
}
void OpenGL::Shader::set_vec4(String name, const Vec4& value) {
	gl_error_check(glUniform4fv(this->get_uniform_location(name, GL_FLOAT_VEC4), 1, &value.x));
}
void OpenGL::Shader::set_vec4(String name, float x, float y, float z, float w) {
	gl_error_check(glUniform4f(this->get_uniform_location(name, GL_FLOAT_VEC4), x, y, z, w));
}
void OpenGL::Shader::set_mat4(String name, const Mat4& mat) {
	gl_error_check(glUniformMatrix4fv(this->get_uniform_location(name, GL_FLOAT_MAT4), 1, GL_TRUE, &mat.v[0].x));
}

void OpenGL::Shader::set_material(OpenGL* backend, Material* material) {
	this->set_float(STR(MATERIAL_OPACITY_UNIFORM_NAME), material->opacity);
	this->set_float(STR(MATERIAL_METALLIC_UNIFORM_NAME), material->metallic);
	this->set_float(STR(MATERIAL_ROUGHNESS_UNIFORM_NAME), material->roughness);
	this->set_float(STR(MATERIAL_SHININESS_UNIFORM_NAME), material->shininess);

	int texture_count = 0;
	for (const auto entry : material->bindings) {
		String k = entry.key; 
		BindingValue v = entry.value;

		switch (v.type) {
			case BindingValueType::BOOL: {
				this->set_bool(k, v.boolean_binding);
			} break;

			case BindingValueType::INTEGER: {
				this->set_int(k, v.integer_binding);
			} break;

			case BindingValueType::FLOAT: {
				this->set_float(k, v.float_binding);
			} break;

			case BindingValueType::TEXTURE_HANDLE: {
				Texture& texture = backend->textures.get(v.texture_binding.handle);
				if (texture.type == TextureSamplerType::SAMPLER_2D) {
					this->set_texture(k, texture_count++, texture);
				} else {
					this->set_texture_cube(k, texture_count++, texture);
				}
			} break;

			case BindingValueType::VECTOR2: {
				this->set_vec2(k, v.vector2_binding);
			} break;

			case BindingValueType::VECTOR3: {
				this->set_vec3(k, v.vector3_binding);
			} break;

			case BindingValueType::VECTOR4: {
				this->set_vec4(k, v.vector4_binding);
			} break;

			case BindingValueType::MAT4: {
				this->set_mat4(k, v.mat4_binding);
			} break;
		}
	}
}