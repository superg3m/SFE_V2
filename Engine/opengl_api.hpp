#include <vector>
#include <map>
#include <string>
#include <glad/glad.h>
#include <core.hpp>

void _GL_CHECK_ERROR(const char* file, int line);

#define GL_ERROR_CHECK
#if defined(GL_ERROR_CHECK)
    #define gl_check_error(glCall) glCall; _GL_CHECK_ERROR(__FILE__, __LINE__) 
#else
    #define gl_check_error(glCall) glCall
#endif

namespace OpenGL {
    typedef GLuint stride_t;
    using TextureID = GLuint;

    enum class BufferStrideTypeInfo {
        BOOL = 1,
        INT = 1,
        FLOAT = 1, 
        VEC2 = 2,
        VEC3 = 3,
        VEC4 = 4, 
        IVEC4 = 4, 
        MAT4 = 16
    };

    struct VertexLayoutElement {
        size_t offset = 0; // offset in bytes of the actual memeber! OFFSET_OF(Vertex, aPosition)
        BufferStrideTypeInfo type;
    };

    void bind_vertex_attribute(int &location, bool instanced, stride_t stride, VertexLayoutElement desc);

    struct VertexBufferObject {
        GLuint id;
        GLenum gl_usage;
        template<typename T>
        static VertexBufferObject allocate(std::vector<VertexLayoutElement> layout, std::vector<T> buffer, GLenum gl_usage = GL_STATIC_DRAW, bool instanced = false) {
            VertexBufferObject ret = {};
            ret.gl_usage = gl_usage;

            gl_check_error(glGenBuffers(1, &ret.id));
            gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, ret.id));
            gl_check_error(glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(T), buffer.data(), gl_usage));

            int location = 0;
            for (VertexLayoutElement desc : layout) {
                bind_vertex_attribute(location, instanced, sizeof(T), desc);
            }

            return ret;
        }

        template<typename T>
        void update_buffer(std::vector<T> buffer, GLsizeiptr offset = 0) {
            RUNTIME_ASSERT(gl_usage == GL_DYNAMIC_DRAW);

            #if 0
                this->bind();
                void *ptr = gl_check_error(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
                size_t buffer_size = sizeof(T) * buffer.count();
                Memory::Copy(ptr, buffer_size, buffer.data(), buffer_size);
                gl_check_error(glUnmapBuffer(GL_ARRAY_BUFFER));
            #else
                gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, this->id));
                gl_check_error(glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(T) * buffer.count(), buffer.data()));
            #endif
        }

        void bind();
    };

    struct ElementBufferObject {
        GLuint id;
        GLenum gl_usage;
        static ElementBufferObject allocate(std::vector<GLuint> indices, GLenum gl_usage = GL_STATIC_DRAW) {
            ElementBufferObject ret = {};
            ret.gl_usage = gl_usage;

            gl_check_error(glGenBuffers(1, &ret.id));
            gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, ret.id));
            gl_check_error(glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), gl_usage));

            return ret;
        }
        void bind();
    };

    struct VertexArrayObject {
        GLuint id;
        static VertexArrayObject create(std::vector<VertexBufferObject> vbos, std::vector<ElementBufferObject> ebos = {});
        void bind_buffers(std::vector<VertexBufferObject> vbos, std::vector<ElementBufferObject> ebos = {});
        void bind();
    };


    struct DrawData {
        GLenum draw_type = GL_TRIANGLES;
        GLuint vertex_count = 0;
        GLuint index_count = 0;
        u32 base_vertex  = 0; // starting offset to next vertex in the vertex buffer
        u32 base_index   = 0; // offset to next index in the index buffer
    };

    enum class TextureSamplerType {
        SAMPLER_2D,
        CUBEMAP_3D
    };

    struct Texture {
        unsigned int id = 0;
        unsigned int width = 0;
        unsigned int height = 0;
        u8* data = nullptr; // if you use should_free = false then you must free this yourself with free_data()
        TextureSamplerType type = TextureSamplerType::SAMPLER_2D;
        // sampler2d or cubemap

        static void flip_vertically_in_place(u8* data, int width, int height);
        static Texture load_from_file(const char* path, bool pixel_perfect = false, bool should_free = true);
        static Texture load_from_memory(const u8* data, int width, int height, int channels, bool pixel_perfect = false);
        static Texture load_cube_map(std::vector<const char*> cube_map_texture_paths);

        void free_data();
    };

    struct UniformDesc {
        GLenum type;
        GLint location;
    };

    struct Shader {
        unsigned int program_id = 0;

        static Shader create(std::vector<const char*> shader_paths);
        void compile();

        void use() const;
        void set_model(glm::mat4 &model);
        void set_view(glm::mat4 &view);
        void set_projection(glm::mat4 &projection);

        void set_bool(const char* name, bool value);
        void set_int(const char* name, int value);
        void set_float(const char* name, float value);
        void set_texture(const char* name, int texture_unit, Texture texture);
        void set_texture_cube(const char* name, int texture_unit, Texture texture);
        void set_vec2(const char* name, const glm::vec2& value);
        void set_vec2(const char* name, float x, float y);
        void set_vec3(const char* name, const glm::vec3& value);
        void set_vec3(const char* name, float x, float y, float z);
        void set_vec4(const char* name, const glm::vec4& value);
        void set_vec4(const char* name, float x, float y, float z, float w);
        void set_mat4(const char* name, const glm::mat4& mat);
    private:
        std::vector<const char*> shader_paths;
        std::map<std::string, UniformDesc> uniforms;

        GLuint create_shader_program(std::vector<const char*> shader_paths);
        GLenum type_from_path(std::string path);
        void check_compile_error(unsigned int source_id, const char* path);
        unsigned int shader_source_compile(const char* path);
        unsigned int get_uniform_location(const char* name, GLenum type);
    };
}