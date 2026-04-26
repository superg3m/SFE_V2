#include "backend.hpp"
#include "../../Platform/platform.hpp"

u32 OpenGL::Shader::create_shader_program(Vector<const char*> shader_paths) {
    u32 shader_program_id = glCreateProgram();
    this->shader_paths = shader_paths;

    u32 shader_source_count = 0;
    u32 shader_source_ids[8] = {};
    for (const char* path : shader_paths) {
        u32 shader_source_id = this->shader_source_compile(path);
        glAttachShader(shader_program_id, shader_source_id);
        shader_source_ids[shader_source_count++] = shader_source_id;
    }
    glLinkProgram(shader_program_id);

    int success = false;
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[1028] = {0};
        glGetProgramInfoLog(shader_program_id, 512, NULL, info_log);
        LOG_ERROR("LINKING_FAILED {%s}\n", shader_paths[0]);
        LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);
    }

    for (int i = 0; i < shader_source_count; i++) {
        glDeleteShader(shader_source_ids[i]);
    }

    int uniform_count = 0;
    glGetProgramiv(shader_program_id, GL_ACTIVE_UNIFORMS, &uniform_count);
    for (int i = 0; i < uniform_count; i++) {
        int size;
        GLenum type;
        const GLsizei name_max_size = 256;
        GLchar name[name_max_size];
        GLsizei name_length;
        glGetActiveUniform(shader_program_id, (u32)i, name_max_size, &name_length, &size, &type, name);

        int location = glGetUniformLocation(shader_program_id, name);
        UniformDesc value = UniformDesc{type, location};
        this->uniforms.put(name, value);
    }

    return shader_program_id;
}

void OpenGL::Shader::compile() {
    this->uniforms.clear();
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

GLenum OpenGL::Shader::type_from_path(const char* shader_source_path) {
    s64 extension_index = String::last_index_of(shader_source_path, String::cstr_length(shader_source_path), STRING_LIT_ARG("."));
    RUNTIME_ASSERT_MSG(extension_index != -1, "Missing extension (.vert, .frag)\n");
    
    String extension = String(shader_source_path + extension_index);
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

void OpenGL::Shader::check_compile_error(unsigned int source_id, const char* path) {
    int success;
    char info_log[1024];
    gl_error_check(glGetShaderiv(source_id, GL_COMPILE_STATUS, &success));
    if (!success) {
        gl_error_check(glGetShaderInfoLog(source_id, 1024, NULL, info_log));
        LOG_ERROR("ERROR::SHADER_COMPILATION_ERROR {%s}\n", path);
        LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);
    }
}

unsigned int OpenGL::Shader::shader_source_compile(const char* path) {
    size_t file_size = 0;
    Error error = Error::SUCCESS;
    GLchar* shader_source = (GLchar*)Platform::read_entire_file(Allocator::general(), path, file_size, error);
    RUNTIME_ASSERT_MSG(error == Error::SUCCESS, "Shader Error: %s\n", path, error_get_string(error));

    GLenum type = this->type_from_path(path);
    unsigned int source_id = glCreateShader(type);
    gl_error_check(glShaderSource(source_id, 1, &shader_source, NULL));
    gl_error_check(glCompileShader(source_id));

    this->check_compile_error(source_id, path);
    free(shader_source);

    return source_id;
}

unsigned int OpenGL::Shader::get_uniform_location(const char* name, GLenum type) {
    if (this->uniforms.has(name)) {
        UniformDesc expected = this->uniforms.get(name);
        if (expected.type != type) {
            LOG_ERROR("Shader {%s} Uniform: '%s' type missmatch | Expected: %s | Got: %s\n", this->shader_paths[0], name, gl_enum_to_string(expected.type), gl_enum_to_string(type));
            return (unsigned int)-1;
        }

        return expected.location;
    }

    int location = glGetUniformLocation(this->program_id, name);
    this->uniforms.put(name, UniformDesc{type, location}); // this type might be wrong, but theres no great robust way to do arrays that I know of...
    if (location == -1) {
        LOG_ERROR("Shader {%s} Uniform: '%s' does not exists\n", this->shader_paths[0], name);
    }

    return location;
}

void OpenGL::Shader::use() const {
    gl_error_check(glUseProgram(this->program_id));
}

// TODO(Jovanni): Make this use the locations instead of string lookups
void OpenGL::Shader::set_model(Mat4 &model) {
    this->set_mat4("uModel", model);
}

void OpenGL::Shader::set_view(Mat4 &view) {
    this->set_mat4("uView", view);
}

void OpenGL::Shader::set_projection(Mat4 &projection) {
    this->set_mat4("uProjection", projection);
}

void OpenGL::Shader::set_bool(const char* name, bool value) {
    gl_error_check(glUniform1i(this->get_uniform_location(name, GL_BOOL), (int)value));
}
void OpenGL::Shader::set_int(const char* name, int value) {
    gl_error_check(glUniform1i(this->get_uniform_location(name, GL_INT), value));
}
void OpenGL::Shader::set_texture(const char* name, Texture texture) {
    // RUNTIME_ASSERT(texture.type == TextureSamplerType::SAMPLER_2D);

    // gl_error_check(glActiveTexture(GL_TEXTURE0 + texture.texture_unit));
    // gl_error_check(glBindTexture(GL_TEXTURE_2D, texture.id));

    // gl_error_check(glUniform1i(this->get_uniform_location(name, GL_SAMPLER_2D), texture.texture_unit));
}
void OpenGL::Shader::set_texture_cube(const char* name, Texture texture) {
    // RUNTIME_ASSERT(texture.type == TextureSamplerType::CUBEMAP_3D);

    // gl_error_check(glActiveTexture(GL_TEXTURE0 + texture.texture_unit));
    // gl_error_check(glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id));

    // gl_error_check(glUniform1i(this->get_uniform_location(name, GL_SAMPLER_CUBE), texture.texture_unit));
}
void OpenGL::Shader::set_float(const char* name, float value) {
    gl_error_check(glUniform1f(this->get_uniform_location(name, GL_FLOAT), value));
}
void OpenGL::Shader::set_vec2(const char* name, const Vec2& value) {
    gl_error_check(glUniform2fv(this->get_uniform_location(name, GL_FLOAT_VEC2), 1, &value.x));
}
void OpenGL::Shader::set_vec2(const char* name, float x, float y) {
    gl_error_check(glUniform2f(this->get_uniform_location(name, GL_FLOAT_VEC2), x, y));
}
void OpenGL::Shader::set_vec3(const char* name, const Vec3& value) {
    gl_error_check(glUniform3fv(this->get_uniform_location(name, GL_FLOAT_VEC3), 1, &value.x));
}
void OpenGL::Shader::set_vec3(const char* name, float x, float y, float z) {
    gl_error_check(glUniform3f(this->get_uniform_location(name, GL_FLOAT_VEC3), x, y, z));
}
void OpenGL::Shader::set_vec4(const char* name, const Vec4& value) {
    gl_error_check(glUniform4fv(this->get_uniform_location(name, GL_FLOAT_VEC4), 1, &value.x));
}
void OpenGL::Shader::set_vec4(const char* name, float x, float y, float z, float w) {
    gl_error_check(glUniform4f(this->get_uniform_location(name, GL_FLOAT_VEC4), x, y, z, w));
}
void OpenGL::Shader::set_mat4(const char* name, const Mat4& mat) {
    gl_error_check(glUniformMatrix4fv(this->get_uniform_location(name, GL_FLOAT_MAT4), 1, GL_TRUE, &mat.v[0].x));
}

void OpenGL::Shader::set_material(Material* material) {
    for (const auto entry : material->bindings) {
        const char* k = entry.key; 
        OpenGL::BindingValue v = entry.value;

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

            case BindingValueType::SAMPLER_2D: {
                this->set_texture(k, v.sampler2d_binding);
            } break;

            case BindingValueType::CUBEMAP: {
                this->set_texture_cube( k, v.cubemap_binding);
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