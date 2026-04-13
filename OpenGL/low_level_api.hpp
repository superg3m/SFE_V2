#include <vector>
#include <map>
#include <string>
#include <glad/glad.h>

#include "../types.hpp"

namespace OpenGL {
    typedef GLuint VAO;
    typedef GLuint VBO;
    typedef GLuint EBO;
    typedef GLuint TextureID;

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
        bool instanced = false;
        size_t offset = 0; // offset in bytes of the actual memeber! OFFSET_OF(Vertex, aPosition)
        BufferStrideTypeInfo type;
    };

    struct DrawData {
        GLenum draw_type = GL_TRIANGLES;
        GLuint vertex_count = 0;
        GLuint index_count = 0;
        u32 base_vertex  = 0; // starting offset to next vertex in the vertex buffer
        u32 base_index   = 0; // offset to next index in the index buffer
    };

    VAO create_vao();
    void bind_vao(VAO vao);
    VBO create_vbo(VAO vao, std::vector<VertexLayoutElement> layout, u8* data, GLsizeiptr data_size, GLenum gl_usage = GL_STATIC_DRAW);
    EBO create_ebo(std::vector<GLuint> indicies, GLenum gl_usage = GL_STATIC_DRAW)
    TextureID load_texture_from_memory(u8* data, int width, int height, int channels, bool pixel_perfect = false);
    TextureID load_texture_from_path(std::string path, int &width, int &height, int &channels, bool pixel_perfect = false);
    void update_vbo_data(VBO vbo, GLintptr offset, u8* data, GLsizeiptr data_size, GLuint stride);

    struct UniformDesc {
        GLenum type;
        GLint location;
    };

    struct Shader {
        unsigned int program_id = 0;

        static Shader create(std::vector<const char*> shader_paths);

        void use() const;
        void set_model(glm::mat4 &model);
        void set_view(glm::mat4 &view);
        void set_projection(glm::mat4 &projection);

        void set_bool(const char* name, bool value);
        void set_int(const char* name, int value);
        void set_float(const char* name, float value);
        void set_texture(const char* name, int texture_unit, TextureID texture);
        void set_texture_cube(const char* name, int texture_unit, TextureID texture);
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

        GLenum typeFromPath(std::string path);
        void checkCompileError(unsigned int source_id, const char* path);
        unsigned int shaderSourceCompile(const char* path);
        unsigned int getUniformLocation(const char* name, GLenum type);
    };
}