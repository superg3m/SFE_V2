#include <shader.hpp>
#include <gl_error_check.hpp>
#include <render_state.hpp>

namespace OpenGL {
    u32 Shader::create_shader_program(std::vector<const char*> shader_paths) {
        u32 shader_program_id = glCreateProgram();
        this->shader_paths = shader_paths;

        std::vector<u32> shader_source_ids;
        for (const char* path : shader_paths) {
            u32 shader_source_id = this->shader_source_compile(path);
            glAttachShader(shader_program_id, shader_source_id);
            shader_source_ids.push_back(shader_source_id);
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

        for (int i = 0; i < shader_source_ids.size(); i++) {
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
            std::string str_key = std::string(name, name_length);
            UniformDesc value = UniformDesc{type, location};
            this->uniforms.insert(std::make_pair(str_key, value));
        }

        return shader_program_id;
    }

    Shader Shader::create(std::vector<const char*> shader_paths) {
        Shader ret = {};
        ret.program_id = 0;
        ret.shader_paths = shader_paths;
        ret.compile();

        return ret;
    }

    void Shader::compile() {
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

    GLenum Shader::type_from_path(std::string shader_source_path) {
        size_t extension_index = shader_source_path.find_last_of(".");
        RUNTIME_ASSERT_MSG(extension_index != std::string::npos, "Missing extension (.vert, .frag)\n");
        
        std::string extension = std::string(shader_source_path.c_str() + extension_index);
        if (extension.find(".vert") != std::string::npos) {
            return GL_VERTEX_SHADER;
        } else if (extension.find(".frag") != std::string::npos) {
            return GL_FRAGMENT_SHADER;
        } else if (extension.find(".gs") != std::string::npos) {
            return GL_GEOMETRY_SHADER;
        }

        RUNTIME_ASSERT_MSG(false, "Unsupported extension: %s | Expected: (.vert, .frag, .gs)\n", extension.c_str());
        return GL_INVALID_ENUM;
    }

    void Shader::check_compile_error(unsigned int source_id, const char* path) {
        int success;
        char info_log[1024];
        gl_error_check(glGetShaderiv(source_id, GL_COMPILE_STATUS, &success));
        if (!success) {
            gl_error_check(glGetShaderInfoLog(source_id, 1024, NULL, info_log));
            LOG_ERROR("ERROR::SHADER_COMPILATION_ERROR {%s}\n", path);
            LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);
        }
    }

    unsigned int Shader::shader_source_compile(const char* path) {
        size_t file_size = 0;
        Error error = Error::SUCCESS;
        GLchar* shader_source = (GLchar*)platform_read_entire_file(path, file_size, error);
        RUNTIME_ASSERT_MSG(error == Error::SUCCESS, "Shader Error: %s\n", path, error_get_string(error));

        GLenum type = this->type_from_path(path);
        unsigned int source_id = glCreateShader(type);
        gl_error_check(glShaderSource(source_id, 1, &shader_source, NULL));
        gl_error_check(glCompileShader(source_id));

        this->check_compile_error(source_id, path);
        free(shader_source);

        return source_id;
    }

    unsigned int Shader::get_uniform_location(const char* name, GLenum type) {
        if (this->uniforms.count(name)) {
            UniformDesc expected = this->uniforms[name];
            if (expected.type != type) {
                LOG_ERROR("Shader {%s} Uniform: '%s' type missmatch | Expected: %s | Got: %s\n", this->shader_paths[0], name, gl_enum_to_string(expected.type), gl_enum_to_string(type));
                return (unsigned int)-1;
            }

            return expected.location;
        }

        int location = glGetUniformLocation(this->program_id, name);
        this->uniforms.insert(std::make_pair(name, UniformDesc{type, location})); // this type might be wrong, but theres no great robust way to do arrays that I know of...
        if (location == -1) {
            LOG_ERROR("Shader {%s} Uniform: '%s' does not exists\n", this->shader_paths[0], name);
        }

        return location;
    }

    void Shader::use(RenderState* render_state) const {
        render_state->bind_shader_program_or_use_cached(this->program_id);
    }

    // TODO(Jovanni): Make this use the locations instead of string lookups
    void Shader::set_model(glm::mat4 &model) {
        this->set_mat4("uModel", model);
    }

    void Shader::set_view(glm::mat4 &view) {
        this->set_mat4("uView", view);
    }

    void Shader::set_projection(glm::mat4 &projection) {
        this->set_mat4("uProjection", projection);
    }

    void Shader::set_bool(const char* name, bool value) {
        gl_error_check(glUniform1i(this->get_uniform_location(name, GL_BOOL), (int)value));
    }
    void Shader::set_int(const char* name, int value) {
        gl_error_check(glUniform1i(this->get_uniform_location(name, GL_INT), value));
    }
    void Shader::set_texture(const char* name, Texture texture) {
        RUNTIME_ASSERT(texture.type == TextureSamplerType::SAMPLER_2D);

        gl_error_check(glActiveTexture(GL_TEXTURE0 + texture.texture_unit));
        gl_error_check(glBindTexture(GL_TEXTURE_2D, texture.id));

        gl_error_check(glUniform1i(this->get_uniform_location(name, GL_SAMPLER_2D), texture.texture_unit));
    }
    void Shader::set_texture_cube( const char* name, Texture texture) {
        RUNTIME_ASSERT(texture.type == TextureSamplerType::CUBEMAP_3D);

        gl_error_check(glActiveTexture(GL_TEXTURE0 + texture.texture_unit));
        gl_error_check(glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id));

        gl_error_check(glUniform1i(this->get_uniform_location(name, GL_SAMPLER_CUBE), texture.texture_unit));
    }
    void Shader::set_float(const char* name, float value) {
        gl_error_check(glUniform1f(this->get_uniform_location(name, GL_FLOAT), value));
    }
    void Shader::set_vec2(const char* name, const glm::vec2& value) {
        gl_error_check(glUniform2fv(this->get_uniform_location(name, GL_FLOAT_VEC2), 1, &value[0]));
    }
    void Shader::set_vec2(const char* name, float x, float y) {
        gl_error_check(glUniform2f(this->get_uniform_location(name, GL_FLOAT_VEC2), x, y));
    }
    void Shader::set_vec3(const char* name, const glm::vec3& value) {
        gl_error_check(glUniform3fv(this->get_uniform_location(name, GL_FLOAT_VEC3), 1, &value[0]));
    }
    void Shader::set_vec3(const char* name, float x, float y, float z) {
        gl_error_check(glUniform3f(this->get_uniform_location(name, GL_FLOAT_VEC3), x, y, z));
    }
    void Shader::set_vec4(const char* name, const glm::vec4& value) {
        gl_error_check(glUniform4fv(this->get_uniform_location(name, GL_FLOAT_VEC4), 1, &value[0]));
    }
    void Shader::set_vec4(const char* name, float x, float y, float z, float w) {
        gl_error_check(glUniform4f(this->get_uniform_location(name, GL_FLOAT_VEC4), x, y, z, w));
    }
    void Shader::set_mat4(const char* name, const glm::mat4& mat) {
        gl_error_check(glUniformMatrix4fv(this->get_uniform_location(name, GL_FLOAT_MAT4), 1, GL_FALSE, &mat[0][0]));
    }
}