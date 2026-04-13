#include <vector>
#include <string>
#include <glad/glad.h>

#include "../types.hpp"

namespace OpenGL {
    typedef GLuint VAO;
    typedef GLuint VBO;
    typedef GLuint TextureID;

    VAO create_vao();
    void upload_vertex_data_to_gpu(VAO vao, std::vector<float> vertices, std::vector<u32> indicies = {});
    TextureID load_texture(std::string path, u8* data = nullptr, size_t data_size = 0);
}