#include "low_level_api.hpp"
#include <stb/stb_image.h>

void _gl_check_error(const char *file, int line) {
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

namespace OpenGL {
    VAO create_vao() {
        VAO vao = 0;
        glGenVertexArrays(1, &vao);

        return vao;
    }

    void bind_vertex_attribute(int &location, bool instanced, s64 stride, VertexLayoutElement desc) {
        bool is_integer = (desc.type == BufferStrideTypeInfo::INT) || (desc.type == BufferStrideTypeInfo::IVEC4);
        bool is_matrix = desc.type == BufferStrideTypeInfo::MAT4;
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

    size_t compute_stride_from_layout(std::vector<VertexLayoutElement>& layout) {
        size_t stride = 0;
        for (VertexLayoutElement desc : layout) {
            stride += (size_t)desc.type * sizeof(float);
        }

        return stride;
    }

    // I will probably end up changing this to 
    // u8* vertices, int vertex_count (The advantage of this is Vertex* vertices I can just cast this an its no problem) 
    // total_buffer_size = stride * vertex_count
    void upload_vertex_data_to_gpu(VAO vao, std::vector<VertexLayoutElement> layout, std::vector<float> vertices, GLenum gl_usage = GL_STATIC_DRAW, std::vector<GLuint> indicies = {}) {
        gl_check_error(glBindVertexArray(vao)); // want to check if already bound

        GLuint vbo;
        gl_check_error(glGenBuffers(1, &vbo));
        gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        gl_check_error(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), gl_usage));

        int location = 0;
        size_t stride = compute_stride_from_layout(layout);
        for (VertexLayoutElement desc : layout) {
            bind_vertex_attribute(location, desc.instanced, stride, desc);
        }

        GLuint ebo;
        gl_check_error(glGenBuffers(1, &ebo));
        gl_check_error(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
        gl_check_error(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), vertices.data(), gl_usage));
    }

    TextureID load_texture_from_memory(u8* data, int width, int height, int channels, bool pixel_perfect = false) {
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

    TextureID load_texture_from_path(std::string path, int &width, int &height, int &channels, bool pixel_perfect = false) {
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
}