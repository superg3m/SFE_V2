#pragma once

#include <shader.hpp>
#include <mesh.hpp>
#include <material.hpp>

namespace OpenGL {
    struct RenderCommand {
        Shader* shader;
        Mesh* mesh;
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };

    struct RenderQueue {
        std::vector<RenderCommand> commands;

        void submit(RenderCommand command) {
            this->commands.push_back(command);
        }

        void draw() {
            for (RenderCommand& command : this->commands) {
                Shader* shader = command.shader;
                Mesh* mesh = command.mesh;

                mesh->vao.bind();
                shader->use();
                shader->set_model(command.model);
                shader->set_view(command.view);
                shader->set_projection(command.projection);
                for (DrawData& data : mesh->meshes) {
                    command.shader->set_material(&mesh->materials[data.material_index]);
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

            this->commands.clear();
        }
    };
}