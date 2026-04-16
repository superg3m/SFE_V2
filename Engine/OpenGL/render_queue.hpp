#pragma once

#include <shader.hpp>
#include <mesh.hpp>
#include <material.hpp>

namespace OpenGL {
    struct RenderCommandOpaque {
        Shader* shader = nullptr;
        VAO* vao = nullptr;
        MeshEntry* entry = nullptr;
        Material* material = nullptr;

        glm::mat4 model = glm::mat4(1);
        glm::mat4 view = glm::mat4(1); // TODO(Jovanni): eventually this will be handled by active camera! SO WE WILL REMOVE THIS
        glm::mat4 projection = glm::mat4(1); // (Jovanni): eventually this will be handled by active camera! SO WE WILL REMOVE THIS
        bool render_mesh_wireframe = false;
    };

    struct RenderCommandSkybox {
        Shader* shader = nullptr;
        Mesh* mesh = nullptr;
        glm::mat4 model = glm::mat4(1);
        glm::mat4 projection = glm::mat4(1); // (Jovanni): eventually this will be handled by active camera! SO WE WILL REMOVE THIS
    };

    struct RenderQueue {
        std::vector<RenderCommandOpaque> opaque_commands;
        std::vector<RenderCommandSkybox> skybox_commands;
        // std::vector<RenderCommandTransparent> transparent_commands;

        void submit(RenderCommandOpaque command) {
            this->opaque_commands.push_back(command);
        }

        void submit(RenderCommandSkybox command) {
            this->skybox_commands.push_back(command);
        }

        void draw_entry_commands() {
            for (RenderCommandOpaque& command : this->opaque_commands) {
                Shader* shader = command.shader;
                VAO* vao = command.vao;  // instead of mesh
                Material* material = command.material; // instead of mesh
                MeshEntry* entry = command.entry; // instead of mesh

                // this needs some OpenGL::RenderState so I don't call into the opengl code if I don't have to
                glPolygonMode(GL_FRONT_AND_BACK, command.render_mesh_wireframe ? GL_LINE : GL_FILL);

                vao->bind();
                shader->use();

                // glm::mat4 view = active_camera.get_view_matrix()
                // glm::mat4 projection = active_camera.get_projection_matrix()

                shader->set_model(command.model);
                shader->set_view(command.view);
                shader->set_projection(command.projection);
                if (material) {
                    command.shader->set_material(material);
                }
            
                if (entry->index_count > 0) {
                    gl_check_error(glDrawElementsBaseVertex(
                        entry->draw_type, entry->index_count, GL_UNSIGNED_INT, 
                        (void*)(sizeof(unsigned int) * entry->base_index), 
                        entry->base_vertex
                    ));
                } else {
                    gl_check_error(glDrawArrays(
                        entry->draw_type,
                        entry->base_vertex,
                        entry->vertex_count
                    ));
                }
            }

            this->opaque_commands.clear();
        }

        /*
        void draw_opaque_commands() {
            for (RenderCommandOpaque& command : this->opaque_commands) {
                Shader* shader = command.shader;
                Mesh* mesh = command.mesh;

                // this needs some OpenGL::RenderState so I don't call into the opengl code if I don't have to
                glPolygonMode(GL_FRONT_AND_BACK, command.render_mesh_wireframe ? GL_LINE : GL_FILL);

                mesh->vao.bind();
                shader->use();

                // camera.get_view_matrix()
                // camera.get_projection_matrix()

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

            this->opaque_commands.clear();
        }

        void draw_skybox_commands() {
            for (RenderCommandSkybox& command : this->skybox_commands) {
                Shader* shader = command.shader;
                Mesh* mesh = command.mesh;

                glm::mat4 view = glm::mat3(command.model);

                mesh->vao.bind();
                shader->use();
                shader->set_model(command.model);
                shader->set_view(view);
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

            this->skybox_commands.clear();
        }
        */

        void draw() {
            this->draw_entry_commands();
            // this->draw_opaque_commands();
            // this->draw_skybox_commands();
            // this->draw_transparent_commands();
        }
    };
}