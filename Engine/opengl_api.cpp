#include "opengl_api.hpp"
#include <stb/stb_image.h>

void _GL_CHECK_ERROR(const char *file, int line) {
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
        
        RUNTIME_ASSERT_MSG(false, "%s | %s:%d\n", error, file, line);
    }
}

namespace OpenGL {
    void bind_vertex_attribute(int &location, bool instanced, stride_t stride, VertexLayoutElement desc) {
        bool is_integer = (desc.type == BufferStrideTypeInfo::INT) || (desc.type == BufferStrideTypeInfo::IVEC4);
        bool is_matrix = desc.type == BufferStrideTypeInfo::MAT4;
        GLenum gl_type  = is_integer ? GL_INT : GL_FLOAT;

        int max_attributes = 0;
        gl_check_error(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes));
        RUNTIME_ASSERT_MSG(max_attributes > location, "Location outside range");

        if (is_matrix) {
            for (int i = 0; i < 4; i++) {
                RUNTIME_ASSERT_MSG(max_attributes > location + i,
                    "You are trying to use a location that is outside of the max vertex attributes: %d",
                    max_attributes
                );

                gl_check_error(glEnableVertexAttribArray(location + i));
                gl_check_error(glVertexAttribPointer(location + i, (GLint)BufferStrideTypeInfo::VEC4, GL_FLOAT, false, stride, (void*)(desc.offset + (sizeof(glm::vec4) * i))));
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

    stride_t compute_stride_from_layout(std::vector<VertexLayoutElement>& layout) {
        stride_t stride = 0;
        for (VertexLayoutElement desc : layout) {
            stride += (stride_t)desc.type * sizeof(float);
        }

        return stride;
    }

    void VertexBufferObject::bind() {
        gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    }

    void ElementBufferObject::bind() {
        gl_check_error(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
    }

    VertexArrayObject VertexArrayObject::create(std::vector<VertexBufferObject> vbos, std::vector<ElementBufferObject> ebos) {
        VertexArrayObject ret = {};
        gl_check_error(glGenVertexArrays(1, &ret.id));
        ret.bind_buffers(vbos, ebos);

        return ret;
    }

    void VertexArrayObject::bind_buffers(std::vector<VertexBufferObject> vbos, std::vector<ElementBufferObject> ebos) {
        this->bind();
        for (VertexBufferObject& vbo : vbos) {
            vbo.bind();
        }

        for (ElementBufferObject& ebo : ebos) {
            ebo.bind();
        }
    }

    // pass like a renderer in here to control the currently bound vao, reduce call overhead
    void VertexArrayObject::bind() {
        gl_check_error(glBindVertexArray(this->id)); // want to check if already bound
    }

    // -- 

    void Texture::flip_vertically_in_place(u8* data, int width, int height) {
        for (int y = 0; y < height / 2; y++) {
            for (int x = 0; x < width; x++) {
                std::swap(data[y * width + x], data[(height - 1 - y) * width + x]);
            }
        }
    }

    Texture Texture::load_from_file(const char* path, bool pixel_perfect, bool should_free) {
        RUNTIME_ASSERT_MSG(platform_file_exists(path), "Texture path: '%s' doesn't exist!\n", path);
        
        GLenum MIPMAP_TYPE = pixel_perfect ? GL_NEAREST : GL_LINEAR;
        GLenum TEXTURE_VERTICAL_FLIP = true; // GET_BIT(texture_flags, 1);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(TEXTURE_VERTICAL_FLIP);
        u8* data = stbi_load(path, &width, &height, &nrChannels, 0);
        stbi_set_flip_vertically_on_load(false);

        Texture ret = Texture::load_from_memory(data, width, height, nrChannels, pixel_perfect);

        if (should_free) {
            stbi_image_free(data);
            data = nullptr;
        }

        ret.data = data;

        if (ret.id == 0) {
            LOG_ERROR("TextureLoader | id is invalid!\n");
        }
        
        return ret;
    }

    Texture Texture::load_from_memory(const u8* data, int width, int height, int nrChannels, bool pixel_perfect) {
        if (!data || width <= 0 || height <= 0 || nrChannels == 0) {
            RUNTIME_ASSERT_MSG(false, "TextureLoader | Invalid input data for loadTextureFromMemory!\n");
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
        if (nrChannels == 1) {
            internal_format = GL_R8;
            pixel_format = GL_RED;
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
        } else if (nrChannels == 3) {
            internal_format = GL_RGB8;
            pixel_format = GL_RGB;
        } else if (nrChannels == 4) {
            internal_format = GL_RGBA8;
            pixel_format = GL_RGBA;
        } else {
            RUNTIME_ASSERT_MSG(false, "Unsupported channel count");
        }

        gl_check_error(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data));
        gl_check_error(glGenerateMipmap(GL_TEXTURE_2D));

        if (texture == 0) {
            RUNTIME_ASSERT_MSG(false, "TextureLoader | id is invalid for memory texture!\n");
        }

        Texture ret;
        ret.id = texture;
        ret.width = width;
        ret.height = height;

        gl_check_error(glBindTexture(GL_TEXTURE_2D, 0));

        return ret;
    }

    // {right, left, top, bottom, front, back}
    Texture Texture::load_cube_map(std::vector<const char*> texture_paths) {
        TextureID texture = 0;
        gl_check_error(glGenTextures(1, &texture));
        gl_check_error(glBindTexture(GL_TEXTURE_CUBE_MAP, texture));
        gl_check_error(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        gl_check_error(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        gl_check_error(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        gl_check_error(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        gl_check_error(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

        unsigned char *data = nullptr;  
        for(int i = 0; i < texture_paths.size(); i++) {
            RUNTIME_ASSERT_MSG(platform_file_exists(texture_paths[i]), "Texture path: '%s' doesn't exist!\n", texture_paths[i]);
            int width, height, nrChannels;
            data = stbi_load(texture_paths[i], &width, &height, &nrChannels, 0);
            
            GLenum format = 0;
            if (nrChannels == 1) {
                format = GL_RED;
            } else if (nrChannels == 3) {
                format = GL_RGB;
            } else if (nrChannels == 4) {
                format = GL_RGBA;
            } else {
                LOG_ERROR("TextureLoader | Failed to pick a stb format, most likely related to assimp, try to link your libraries in a different order\n");
            }

            if (data) {
                gl_check_error(glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                    0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
                ));
            } else {
                LOG_ERROR("TextureLoader | Failed to load texture\n");
            }

            stbi_image_free(data);
        }

        Texture ret;
        ret.id = texture;
        ret.type = TextureSamplerType::CUBEMAP_3D;

        gl_check_error(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));

        return ret;
    }

    void Texture::free_data() {
        RUNTIME_ASSERT_MSG(this->data, "trying to free data that has already been freed or is nullptr\n");

        stbi_image_free(this->data);
        this->data = nullptr;
    }
    
    void render_vao_with_textures(VertexArrayObject vao, DrawData data, std::vector<Texture> textures = {}) {
        vao.bind();

        for (int i = 0; i < textures.size(); i++) {
            gl_check_error(glActiveTexture(GL_TEXTURE0 + i));
            gl_check_error(glBindTexture(GL_TEXTURE_2D, textures[i].id));
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

    GLuint Shader::create_shader_program(std::vector<const char*> shader_paths) {
        GLuint program_id = glCreateProgram();
        this->shader_paths = shader_paths;

        std::vector<GLuint> shader_source_ids = std::vector<GLuint>(shader_paths.size()); 
        for (const char* path : shader_paths) {
            GLuint shader_source_id = this->shader_source_compile(path);
            glAttachShader(program_id, shader_source_id);
            shader_source_ids.push_back(shader_source_id);
        }
        glLinkProgram(program_id);

        GLint success = false;
        glGetProgramiv(program_id, GL_LINK_STATUS, &success);
        if (!success) {
            char info_log[1028] = {0};
            glGetProgramInfoLog(program_id, 512, NULL, info_log);
            LOG_ERROR("LINKING_FAILED {%s}\n", shader_paths[0]);
            LOG_ERROR("%s -- --------------------------------------------------- --\n", info_log);
        }

        for (int i = 0; i < shader_source_ids.size(); i++) {
            glDeleteShader(shader_source_ids[i]);
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

        return program_id;
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
        // RUNTIME_ASSERT_MSG(extension_index == string::npos, "Missing extension (.vert, .frag)\n");
        
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
        gl_check_error(glGetShaderiv(source_id, GL_COMPILE_STATUS, &success));
        if (!success) {
            gl_check_error(glGetShaderInfoLog(source_id, 1024, NULL, info_log));
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
        gl_check_error(glShaderSource(source_id, 1, &shader_source, NULL));
        gl_check_error(glCompileShader(source_id));

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

        GLint location = glGetUniformLocation(this->program_id, name);
        if (location >= 0) {
            this->uniforms.insert(std::make_pair(name, UniformDesc{type, location})); // this type might be wrong, but theres no great robust way to do arrays that I know of...
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
        gl_check_error(glUniform1i(this->get_uniform_location(name, GL_BOOL), (int)value));
    }
    void Shader::set_int(const char* name, int value) {
        this->use();
        gl_check_error(glUniform1i(this->get_uniform_location(name, GL_INT), value));
    }
    void Shader::set_texture(const char* name, int texture_unit, Texture texture) {
        RUNTIME_ASSERT(texture.type == TextureSamplerType::SAMPLER_2D);

        this->use();
        gl_check_error(glUniform1i(this->get_uniform_location(name, GL_SAMPLER_2D), texture_unit));
    }
    void Shader::set_texture_cube(const char* name, int texture_unit, Texture texture) {
        RUNTIME_ASSERT(texture.type == TextureSamplerType::CUBEMAP_3D);

        this->use();
        gl_check_error(glUniform1i(this->get_uniform_location(name, GL_SAMPLER_CUBE), texture_unit));
    }
    void Shader::set_float(const char* name, float value) {
        this->use();
        gl_check_error(glUniform1f(this->get_uniform_location(name, GL_FLOAT), value));
    }
    void Shader::set_vec2(const char* name, const glm::vec2& value) {
        this->use();
        gl_check_error(glUniform2fv(this->get_uniform_location(name, GL_FLOAT_VEC2), 1, glm::value_ptr(value)));
    }
    void Shader::set_vec2(const char* name, float x, float y) {
        this->use();
        gl_check_error(glUniform2f(this->get_uniform_location(name, GL_FLOAT_VEC2), x, y));
    }
    void Shader::set_vec3(const char* name, const glm::vec3& value) {
        this->use();
        gl_check_error(glUniform3fv(this->get_uniform_location(name, GL_FLOAT_VEC3), 1, glm::value_ptr(value)));
    }
    void Shader::set_vec3(const char* name, float x, float y, float z) {
        this->use();
        gl_check_error(glUniform3f(this->get_uniform_location(name, GL_FLOAT_VEC3), x, y, z));
    }
    void Shader::set_vec4(const char* name, const glm::vec4& value) {
        this->use();
        gl_check_error(glUniform4fv(this->get_uniform_location(name, GL_FLOAT_VEC4), 1, glm::value_ptr(value)));
    }
    void Shader::set_vec4(const char* name, float x, float y, float z, float w) {
        this->use();
        gl_check_error(glUniform4f(this->get_uniform_location(name, GL_FLOAT_VEC4), x, y, z, w));
    }
    void Shader::set_mat4(const char* name, const glm::mat4& mat) {
        this->use();
        gl_check_error(glUniformMatrix4fv(this->get_uniform_location(name, GL_FLOAT_MAT4), 1, GL_FALSE, glm::value_ptr(mat)));
    }
}