#include "low_level_api.hpp"
#include <stb/stb_image.h>

static void _gl_check_error(const char *file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        const char* error = nullptr;
        switch (errorCode) {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        
        printf("%s | %s:%d\n", error, file, line);
    }
}

#define GL_ERROR_CHECK
#if defined(GL_ERROR_CHECK)
    #define gl_check_error(glCall) glCall; _gl_check_error(__FILE__, __LINE__) 
#else
    #define gl_check_error(glCall) glCall
#endif

static void bind_vertex_attribute(int &location, bool instanced, s64 stride, OpenGL::VertexLayoutElement desc) {
    bool is_integer = (desc.type == OpenGL::BufferStrideTypeInfo::INT) || (desc.type == OpenGL::BufferStrideTypeInfo::IVEC4);
    bool is_matrix = desc.type == OpenGL::BufferStrideTypeInfo::MAT4;
    GLenum gl_type  = is_integer ? GL_INT : GL_FLOAT;

    int max_attributes = 0;
    gl_check_error(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes));
    // RUNTIME_ASSERT_MSG(max_attributes > location, "Location outside range");

    if (is_matrix) {
        for (int i = 0; i < 4; i++) {
            /*
            RUNTIME_ASSERT_MSG(max_attributes > location + i,
                "You are trying to use a location that is outside of the max vertex attributes: %d",
                max_attributes
            );
            */

            gl_check_error(glEnableVertexAttribArray(location + i));
            gl_check_error(glVertexAttribPointer(location + i, (GLint)OpenGL::BufferStrideTypeInfo::VEC4, GL_FLOAT, false, stride, (void*)(desc.offset + (sizeof(glm::vec4) * i))));
            gl_check_error(glVertexAttribDivisor(location + i, instanced));
        }

        location += 4;
    } else {
        gl_check_error(glEnableVertexAttribArray(location));
        if (is_integer) {
            gl_check_error(glVertexAttribIPointer(location, (GLint)desc.type, gl_type, stride, (void*)desc.offset));
        } else {
            gl_check_error(glVertexAttribPointer(location, (GLint)desc.type, gl_type, false, stride, (void*)desc.offset));
        }

        gl_check_error(glVertexAttribDivisor(location, instanced));
        location += 1;
    }
}

static GLsizeiptr compute_stride_from_layout(std::vector<OpenGL::VertexLayoutElement>& layout) {
    GLsizeiptr stride = 0;
    for (OpenGL::VertexLayoutElement desc : layout) {
        stride += (GLsizeiptr)desc.type * sizeof(float);
    }

    return stride;
}

namespace OpenGL {
    VAO create_vao() {
        VAO vao = 0;
        glGenVertexArrays(1, &vao);

        return vao;
    }

    void bind_vao(VAO vao) {
        gl_check_error(glBindVertexArray(vao)); // want to check if already bound
    }

    VBO create_vbo(VAO vao, std::vector<VertexLayoutElement> layout, u8* buffer_data, int buffer_count, GLenum gl_usage) {
        GLsizeiptr stride = compute_stride_from_layout(layout);

        VBO vbo;
        gl_check_error(glGenBuffers(1, &vbo));
        gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        gl_check_error(glBufferData(GL_ARRAY_BUFFER, buffer_count * stride, buffer_data, gl_usage));

        int location = 0;
        for (VertexLayoutElement desc : layout) {
            bind_vertex_attribute(location, desc.instanced, stride, desc);
        }

        return vbo;
    }

    EBO create_ebo(std::vector<GLuint> indicies, GLenum gl_usage) {
        EBO ebo;
         gl_check_error(glGenBuffers(1, &ebo));
        gl_check_error(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
        gl_check_error(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), indicies.data(), gl_usage));
    }

    void update_vbo_data(VBO vbo, GLintptr offset, u8* data, GLsizeiptr data_size, GLuint stride) {
        #if 0
            this->bind();
            void *ptr = gl_check_error(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
            size_t buffer_size = sizeof(T) * buffer.count();
            Memory::Copy(ptr, buffer_size, buffer.data(), buffer_size);
            gl_check_error(glUnmapBuffer(GL_ARRAY_BUFFER));
        #else
            gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, vbo));
            gl_check_error(glBufferSubData(GL_ARRAY_BUFFER, offset, data_size, data));
        #endif
    }

    TextureID load_texture_from_memory(u8* data, int width, int height, int channels, bool pixel_perfect) {
        if (!data || width <= 0 || height <= 0 || channels <= 0) {
            // RUNTIME_ASSERT_MSG(false, "TextureLoader | Invalid input data for loadTextureFromMemory!\n");
        }

        GLenum MIPMAP_TYPE = pixel_perfect ? GL_NEAREST : GL_LINEAR;

        TextureID texture = 0;
        gl_check_error(glGenTextures(1, &texture));
        gl_check_error(glBindTexture(GL_TEXTURE_2D, texture));
        gl_check_error(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
        gl_check_error(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
        gl_check_error(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MIPMAP_TYPE));
        gl_check_error(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MIPMAP_TYPE));

        GLenum internal_format = 0;
        GLenum pixel_format = 0;
        if (channels == 1) {
            internal_format = GL_R8;
            pixel_format = GL_RED;
            gl_check_error(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
            gl_check_error(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED));
            gl_check_error(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED));
            gl_check_error(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED));
            gl_check_error(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED));
        } else if (channels == 3) {
            internal_format = GL_RGB8;
            pixel_format = GL_RGB;
        } else if (channels == 4) {
            internal_format = GL_RGBA8;
            pixel_format = GL_RGBA;
        } else {
            // RUNTIME_ASSERT_MSG(false, "Unsupported channel count");
        }

        gl_check_error(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data));
        gl_check_error(glGenerateMipmap(GL_TEXTURE_2D));

        gl_check_error(glBindTexture(GL_TEXTURE_2D, 0));

        return texture;
    }

    TextureID load_texture_from_path(std::string path, int &width, int &height, int &channels, bool pixel_perfect) {
        // RUNTIME_ASSERT_MSG(platform_file_path_exists(path), "Texture path: '%s' doesn't exist!\n", path);

        GLenum MIPMAP_TYPE = pixel_perfect ? GL_NEAREST : GL_LINEAR;
        GLenum TEXTURE_VERTICAL_FLIP = true; // GET_BIT(texture_flags, 1);
        
        stbi_set_flip_vertically_on_load(TEXTURE_VERTICAL_FLIP);
        u8* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        stbi_set_flip_vertically_on_load(false);

        TextureID texture = load_texture_from_memory(data, width, height, channels, pixel_perfect);
        stbi_image_free(data);
       
        return texture;
    }
    
    void render_vao_with_textures(VAO vao, DrawData data, std::vector<TextureID> textures = {}) {
        gl_check_error(glBindVertexArray(vao));

        for (int i = 0; i < textures.size(); i++) {
            gl_check_error(glActiveTexture(GL_TEXTURE0 + i));
            gl_check_error(glBindTexture(GL_TEXTURE_2D, textures[i]));
        }

        if (data.index_count > 0) {
            gl_check_error(glDrawElementsBaseVertex(
                data.draw_type, data.index_count, GL_UNSIGNED_INT, 
                (void*)(sizeof(unsigned int) * data.base_index), 
                data.base_vertex
            ));
        } else {
            gl_check_error(glDrawArrays(
                data.draw_type,
                data.base_vertex,
                data.vertex_count
            ));
        }
    }

    // --- shader ---

    Shader Shader::create(std::vector<const char*> shader_paths) {
        Shader ret = {};

        ret.program_id = glCreateProgram();
        ret.shader_paths = shader_paths;

        std::vector<unsigned int> shader_source_ids = std::vector<unsigned int>(shader_paths.size()); 
        for (const char* path : shader_paths) {
            unsigned int shader_source_id = ret.shaderSourceCompile(path);
            glAttachShader(this->program_id, shader_source_id);
            shader_source_ids.push_back(shader_source_id);
        }
        glLinkProgram(this->program_id);

        GLint success = false;
        glGetProgramiv(this->program_id, GL_LINK_STATUS, &success);
        if (!success) {
            char info_log[1028] = {0};
            glGetProgramInfoLog(this->program_id, 512, NULL, info_log);
            // LOG_ERROR("LINKING_FAILED {%s}\n", shader_paths[0]);
            // LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);
        }

        for (int i = 0; i < shader_source_ids.size(); i++) {
            glDeleteShader(shader_source_ids[i]);
        }

        if (this->uniforms.size()) {
            this->uniforms.clear();
        }

        GLint uniform_count = 0;
        glGetProgramiv(this->program_id, GL_ACTIVE_UNIFORMS, &uniform_count);
        for (int i = 0; i < uniform_count; i++) {
            GLint size;
            GLenum type;
            const GLsizei name_max_size = 256;
            GLchar name[name_max_size];
            GLsizei name_length;
            glGetActiveUniform(this->program_id, (GLuint)i, name_max_size, &name_length, &size, &type, name);

            GLint location = glGetUniformLocation(this->program_id, name);
            
            std::string str_key = std::string(name, name_length);
            UniformDesc value = UniformDesc{type, location};
            this->uniforms.insert(std::make_pair(str_key, value));
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

    GLenum Shader::typeFromPath(std::string shader_source_path) {
        size_t extension_index = shader_source_path.find_last_of(".");
        // RUNTIME_ASSERT_MSG(extension_index == string::npos, "Missing extension (.vert, .frag)\n");
        
        std::string extension = std::string(shader_source_path.c_str() + extension_index);
        


        if (extension.find(".vert") != std::string::npos) {
            return GL_VERTEX_SHADER;
        } else if (extension.find(".frag") != std::string::npos) {
            return GL_FRAGMENT_SHADER;
        } else if (extension.find(".gs") != std::string::npos) {
            return GL_GEOMETRY_SHADER;
        }

        // RUNTIME_ASSERT_MSG(false, "Unsupported extension: %.*s | Expected: (.vert, .frag, .gs)\n", extension.length, extension.data);
        return GL_INVALID_ENUM;
    }

    void Shader::checkCompileError(unsigned int source_id, const char* path) {
        int success;
        char info_log[1024];
        gl_check_error(glGetShaderiv(source_id, GL_COMPILE_STATUS, &success));
        if (!success) {
            gl_check_error(glGetShaderInfoLog(source_id, 1024, NULL, info_log));
            LOG_ERROR("ERROR::SHADER_COMPILATION_ERROR {%s}\n", path);
            LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);
        }
    }

    unsigned int Shader::shaderSourceCompile(const char* path) {
        /*
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
        */

       return 0;
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
        this->use();
        gl_check_error(glUniform1i(this->getUniformLocation(name, GL_BOOL), (int)value));
    }
    void Shader::set_int(const char* name, int value) {
        this->use();
        gl_check_error(glUniform1i(this->getUniformLocation(name, GL_INT), value));
    }
    void Shader::set_texture(const char* name, int texture_unit, TextureID texture) {
        // RUNTIME_ASSERT(texture.type == TextureSamplerType::SAMPLER2D);
        this->use();
        gl_check_error(glUniform1i(this->getUniformLocation(name, GL_SAMPLER_2D), texture_unit));
    }
    void Shader::set_texture_cube(const char* name, int texture_unit, TextureID texture) {
        // RUNTIME_ASSERT(texture.type == TextureSamplerType::CUBEMAP3D);
        this->use();
        gl_check_error(glUniform1i(this->getUniformLocation(name, GL_SAMPLER_CUBE), texture_unit));
    }
    void Shader::set_float(const char* name, float value) {
        this->use();
        gl_check_error(glUniform1f(this->getUniformLocation(name, GL_FLOAT), value));
    }
    void Shader::set_vec2(const char* name, const glm::vec2& value) {
        this->use();
        gl_check_error(glUniform2fv(this->getUniformLocation(name, GL_FLOAT_VEC2), 1, glm::value_ptr(value)));
    }
    void Shader::set_vec2(const char* name, float x, float y) {
        this->use();
        gl_check_error(glUniform2f(this->getUniformLocation(name, GL_FLOAT_VEC2), x, y));
    }
    void Shader::set_vec3(const char* name, const glm::vec3& value) {
        this->use();
        gl_check_error(glUniform3fv(this->getUniformLocation(name, GL_FLOAT_VEC3), 1, glm::value_ptr(value)));
    }
    void Shader::set_vec3(const char* name, float x, float y, float z) {
        this->use();
        gl_check_error(glUniform3f(this->getUniformLocation(name, GL_FLOAT_VEC3), x, y, z));
    }
    void Shader::set_vec4(const char* name, const glm::vec4& value) {
        this->use();
        gl_check_error(glUniform4fv(this->getUniformLocation(name, GL_FLOAT_VEC4), 1, glm::value_ptr(value)));
    }
    void Shader::set_vec4(const char* name, float x, float y, float z, float w) {
        this->use();
        gl_check_error(glUniform4f(this->getUniformLocation(name, GL_FLOAT_VEC4), x, y, z, w));
    }
    void Shader::set_mat4(const char* name, const glm::mat4& mat) {
        this->use();
        gl_check_error(glUniformMatrix4fv(this->getUniformLocation(name, GL_FLOAT_MAT4), 1, GL_FALSE, glm::value_ptr(mat)));
    }
}