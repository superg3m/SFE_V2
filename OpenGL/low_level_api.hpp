#include <vector>
#include <string>
#include <glad/glad.h>

#include "../types.hpp"

namespace OpenGL {
    typedef GLuint VAO;
    typedef GLuint VBO;
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
    void upload_vertex_data_to_gpu(VAO vao, std::vector<float> vertices, std::vector<u32> indicies = {});
    TextureID load_texture(std::string path, u8* data = nullptr, size_t data_size = 0);
}