#include <opengl_api.hpp>

namespace OpenGL {
    void render_model(Model& model, Shader* shader) {
        model.vao.bind();
        for (int i = 0; i < model.meshes.size(); i++) {
            Mesh& mesh = model.meshes[i];
            Material& material = model.materials[mesh.material_index];

            shader->set_material(material);
            if (mesh.data.index_count > 0) {
                gl_check_error(glDrawElementsBaseVertex(
                    mesh.data.draw_type, mesh.data.index_count, GL_UNSIGNED_INT, 
                    (void*)(sizeof(unsigned int) * mesh.data.base_index), 
                    mesh.data.base_vertex
                ));
            } else {
                gl_check_error(glDrawArrays(
                    mesh.data.draw_type,
                    mesh.data.base_vertex,
                    mesh.data.vertex_count
                ));
            }
        }
    }

    void render_vao_with_textures(VertexArrayObject vao, DrawData data, std::vector<Texture> textures) {
        vao.bind();

        for (int i = 0; i < textures.size(); i++) {
            gl_check_error(glActiveTexture(GL_TEXTURE0 + textures[i].texture_unit));
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
}