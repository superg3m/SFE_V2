#include "low_level_api.hpp"

namespace OpenGL {
    VAO create_vao() {
        VAO ret = 0;
        glGenVertexArrays(1, &ret);
        glBindVertexArray(ret);

        return ret;
    }

    void upload_vertex_data_to_gpu(VAO vao, std::vector<float> vertices, std::vector<u32> indicies = {});
    TextureID load_texture(std::string path, u8* data = nullptr, size_t data_size = 0);
}