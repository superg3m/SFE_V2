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
                for (MeshEntry& entry : mesh->meshes) {
                    if (mesh->materials.size()) {
                        command.shader->set_material(&mesh->materials[entry.material_index]);
                    }
                        
                    if (entry.index_count > 0) {
                        gl_check_error(glDrawElementsBaseVertex(
                            entry.draw_type, entry.index_count, GL_UNSIGNED_INT, 
                            (void*)(sizeof(unsigned int) * entry.base_index), 
                            entry.base_vertex
                        ));
                    } else {
                        gl_check_error(glDrawArrays(
                            entry.draw_type,
                            entry.base_vertex,
                            entry.vertex_count
                        ));
                    }
                }
            }

            this->commands.clear();
        }
    };
}