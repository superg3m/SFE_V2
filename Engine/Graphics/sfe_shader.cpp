#include "../../../Core/DataStructure/sfe_ds.hpp"
#include "../../../Core/String/sfe_string.hpp"
#include "../../../Platform/sfe_platform.hpp"
#include "sfe_shader.hpp"

#include "sfe_gl_check.hpp"

namespace SFE {
    Shader::Shader(DS::Vector<const char*> shader_paths) {
        this->program_id = glCreateProgram();
        this->shader_paths = shader_paths;

        DS::Vector<unsigned int> shader_sourceIDs = DS::Vector<unsigned int>(shader_paths.count()); 
        for (const char* path : shader_paths) {
            unsigned int shader_source_id = this->shaderSourceCompile(path);
            glAttachShader(this->program_id, shader_source_id);
            shader_sourceIDs.push(shader_source_id);
        }
        glLinkProgram(this->program_id);

        GLint success = false;
        glGetProgramiv(this->program_id, GL_LINK_STATUS, &success);
        if (!success) {
            char info_log[1028] = {0};
            glGetProgramInfoLog(this->program_id, 512, NULL, info_log);
            LOG_ERROR("LINKING_FAILED {%s}\n", shader_paths[0]);
            LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);
        }

        for (int i = 0; i < shader_sourceIDs.count(); i++) {
            glDeleteShader(shader_sourceIDs[i]);
        }

        for (auto entries : this->uniforms) {
            if (!entries.filled || entries.dead) continue;

            Memory::Free((char*)entries.key);
        }

        GLint uniform_count = 0;
        glGetProgramiv(this->program_id, GL_ACTIVE_UNIFORMS, &uniform_count);
        this->uniforms = DS::Hashmap<const char*, UniformDesc>(MAX(1, uniform_count));
        for (int i = 0; i < uniform_count; i++) {
            GLint size;
            GLenum type;
            const GLsizei name_max_size = 256;
            GLchar name[name_max_size];
            GLsizei name_length;
            glGetActiveUniform(this->program_id, (GLuint)i, name_max_size, &name_length, &size, &type, name);

            GLint location = glGetUniformLocation(this->program_id, name);
            this->uniforms.put(String::Allocate(name, name_length), UniformDesc{type, location});
        }
    }

    static const char* glEnumToString(GLenum type) {
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

    GLenum Shader::typeFromPath(const char* shader_source_path) {
        u64 shader_path_length = String::Length(shader_source_path);
        s64 extension_index = String::LastIndexOf(shader_source_path, shader_path_length, STRING_LIT_ARG("."));
        RUNTIME_ASSERT_MSG(extension_index != -1, "Missing extension (.vert, .frag)\n");
        DS::View<char> extension = DS::View<char>(shader_source_path + extension_index, shader_path_length - (u64)extension_index);

        if (String::Contains(extension.data, extension.length, STRING_LIT_ARG(".vert"))) {
            return GL_VERTEX_SHADER;
        } else if (String::Contains(extension.data, extension.length, STRING_LIT_ARG(".frag"))) {
            return GL_FRAGMENT_SHADER;
        } else if (String::Contains(extension.data, extension.length, STRING_LIT_ARG(".gs"))) {
            return GL_GEOMETRY_SHADER;
        }

        RUNTIME_ASSERT_MSG(false, "Unsupported extension: %.*s | Expected: (.vert, .frag, .gs)\n", extension.length, extension.data);
        return GL_INVALID_ENUM;
    }

    void Shader::checkCompileError(unsigned int source_id, const char* path) {
        int success;
        char info_log[1024];
        glCheckError(glGetShaderiv(source_id, GL_COMPILE_STATUS, &success));
        if (!success) {
            glCheckError(glGetShaderInfoLog(source_id, 1024, NULL, info_log));
            LOG_ERROR("ERROR::SHADER_COMPILATION_ERROR {%s}\n", path);
            LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);
        }
    }

    unsigned int Shader::shaderSourceCompile(const char* path) {
        std::size_t file_size = 0;
        Error error = Error::SUCCESS;
        GLchar* shader_source = (GLchar*)Platform::ReadEntireFile(path, file_size, error);
        RUNTIME_ASSERT_MSG(error == Error::SUCCESS, "Shader Error: %s\n", path, getErrorString(error));

        GLenum type = this->typeFromPath(path);
        unsigned int source_id = glCreateShader(type);
        glCheckError(glShaderSource(source_id, 1, &shader_source, NULL));
        glCheckError(glCompileShader(source_id));

        this->checkCompileError(source_id, path);

        Memory::Free(shader_source);
        return source_id;
    }

    unsigned int Shader::getUniformLocation(const char* name, GLenum type) {
        if (this->uniforms.has(name)) {
            UniformDesc expected = this->uniforms.get(name);
            if (expected.type != type) {
                LOG_ERROR("Shader {%s} Uniform: '%s' type missmatch | Expected: %s | Got: %s\n", this->shader_paths[0], name, glEnumToString(expected.type), glEnumToString(type));
                return (unsigned int)-1;
            }

            return expected.location;
        }

        GLint location = glGetUniformLocation(this->program_id, name);
        if (location >= 0) {
            this->uniforms.put(name, UniformDesc{type, location}); // this type might be wrong, but theres no great robust way to do arrays that I know of...
        } else if (location == -1) {
            LOG_ERROR("Shader {%s} Uniform: '%s' does not exists\n", this->shader_paths[0], name);
        }

        return location;
    }

    void Shader::use() const {
        glUseProgram(this->program_id);
    }

    // TODO(Jovanni): Make this use the locations instead of string lookups
    void Shader::setModel(Math::Mat4 &model) {
        this->use();
        this->setMat4("uModel", model);
    }

    void Shader::setView(Math::Mat4 &view) {
        this->use();
        this->setMat4("uView", view);
    }

    void Shader::setProjection(Math::Mat4 &projection) {
        this->use();
        this->setMat4("uProjection", projection);
    }

    void Shader::setBool(const char* name, bool value) {
        this->use();
        glCheckError(glUniform1i(this->getUniformLocation(name, GL_BOOL), (int)value));
    }
    void Shader::setInt(const char* name, int value) {
        this->use();
        glCheckError(glUniform1i(this->getUniformLocation(name, GL_INT), value));
    }
    void Shader::setTexture2D(const char* name, int texture_unit, Texture &texture) {
        RUNTIME_ASSERT(texture.type == TextureSamplerType::SAMPLER2D);

        this->use();

        glCheckError(glActiveTexture(GL_TEXTURE0 + texture_unit));
        glCheckError(glBindTexture(GL_TEXTURE_2D, texture.id));
        this->setInt(this->getUniformLocation(name, GL_SAMPLER_2D), texture_unit);
    }
    void Shader::setTexture3D(const char* name, int texture_unit, Texture &texture) {
        RUNTIME_ASSERT(texture.type == TextureSamplerType::CUBEMAP3D);

        this->use();

        glCheckError(glActiveTexture(GL_TEXTURE0 + texture_unit));
        glCheckError(glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id));
        this->setInt(this->getUniformLocation(name, GL_SAMPLER_CUBE), texture_unit);
    }
    void Shader::setFloat(const char* name, float value) {
        this->use();
        glCheckError(glUniform1f(this->getUniformLocation(name, GL_FLOAT), value));
    }
    void Shader::setVec2(const char* name, const Math::Vec2& value) {
        this->use();
        glCheckError(glUniform2fv(this->getUniformLocation(name, GL_FLOAT_VEC2), 1, &value.x));
    }
    void Shader::setVec2(const char* name, float x, float y) {
        this->use();
        glCheckError(glUniform2f(this->getUniformLocation(name, GL_FLOAT_VEC2), x, y));
    }
    void Shader::setVec3(const char* name, const Math::Vec3& value) {
        this->use();
        glCheckError(glUniform3fv(this->getUniformLocation(name, GL_FLOAT_VEC3), 1, &value.x));
    }
    void Shader::setVec3(const char* name, float x, float y, float z) {
        this->use();
        glCheckError(glUniform3f(this->getUniformLocation(name, GL_FLOAT_VEC3), x, y, z));
    }
    void Shader::setVec4(const char* name, const Math::Vec4& value) {
        this->use();
        glCheckError(glUniform4fv(this->getUniformLocation(name, GL_FLOAT_VEC4), 1, &value.x));
    }
    void Shader::setVec4(const char* name, float x, float y, float z, float w) {
        this->use();
        glCheckError(glUniform4f(this->getUniformLocation(name, GL_FLOAT_VEC4), x, y, z, w));
    }
    void Shader::setMat4(const char* name, const Math::Mat4& mat) {
        this->use();
        glCheckError(glUniformMatrix4fv(this->getUniformLocation(name, GL_FLOAT_MAT4), 1, GL_TRUE, &mat.v[0].x));
    }

    // protected
    void Shader::setBool(unsigned int location, bool value) {
        this->use();
        glCheckError(glUniform1i(location, (int)value));
    }
    void Shader::setInt(unsigned int location, int value) {
        this->use();
        glCheckError(glUniform1i(location, value));
    }
    void Shader::setFloat(unsigned int location, float value) {
        this->use();
        glCheckError(glUniform1f(location, value));
    }
    void Shader::setVec2(unsigned int location, const Math::Vec2& value) {
        this->use();
        glCheckError(glUniform2fv(location, 1, &value.x));
    }
    void Shader::setVec2(unsigned int location, float x, float y) {
        this->use();
        glCheckError(glUniform2f(location, x, y));
    }
    void Shader::setVec3(unsigned int location, const Math::Vec3& value) {
        this->use();
        glCheckError(glUniform3fv(location, 1, &value.x));
    }
    void Shader::setVec3(unsigned int location, float x, float y, float z) {
        this->use();
        glCheckError(glUniform3f(location, x, y, z));
    }
    void Shader::setVec4(unsigned int location, const Math::Vec4& value) {
        this->use();
        glCheckError(glUniform4fv(location, 1, &value.x));
    }
    void Shader::setVec4(unsigned int location, float x, float y, float z, float w) {
        this->use();
        glCheckError(glUniform4f(location, x, y, z, w));
    }
    void Shader::setMat4(unsigned int location, const Math::Mat4& mat) {
        this->use();
        glCheckError(glUniformMatrix4fv(location, 1, GL_TRUE, &mat.v[0].x));
    }
}