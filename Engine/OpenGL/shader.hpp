#pragma once

#include <glad/glad.h>
#include <vector>
#include <map>
#include <core.hpp>
#include <texture.hpp>
#include <material.hpp>

namespace OpenGL {
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
        void set_texture(const char* name, Texture texture);
        void set_texture_cube(const char* name, Texture texture);
        void set_vec2(const char* name, const glm::vec2& value);
        void set_vec2(const char* name, float x, float y);
        void set_vec3(const char* name, const glm::vec3& value);
        void set_vec3(const char* name, float x, float y, float z);
        void set_vec4(const char* name, const glm::vec4& value);
        void set_vec4(const char* name, float x, float y, float z, float w);
        void set_mat4(const char* name, const glm::mat4& mat);

        // TODO(Jovanni): this is so fucking slow but for now its ok I will switch to my own hashmap later
        // TODO(Jovanni): this is so fucking slow but for now its ok I will switch to my own hashmap later
        // TODO(Jovanni): this is so fucking slow but for now its ok I will switch to my own hashmap later
        // TODO(Jovanni): this is so fucking slow but for now its ok I will switch to my own hashmap later
        void set_material(Material* material) {
            for (const auto [k, v] : material->bindings) {
                switch (v.type) {
                    case BindingValueType::BOOL: {
                        this->set_bool(k.c_str(), v.boolean_binding);
                    } break;

                    case BindingValueType::INTEGER: {
                        this->set_int(k.c_str(), v.integer_binding);
                    } break;

                    case BindingValueType::FLOAT: {
                        this->set_float(k.c_str(), v.float_binding);
                    } break;

                    case BindingValueType::SAMPLER_2D: {
                        this->set_texture(k.c_str(), v.sampler2d_binding);
                    } break;

                    case BindingValueType::CUBEMAP: {
                        this->set_texture_cube(k.c_str(), v.cubemap_binding);
                    } break;

                    case BindingValueType::VECTOR2: {
                        this->set_vec2(k.c_str(), v.vector2_binding);
                    } break;

                    case BindingValueType::VECTOR3: {
                        this->set_vec3(k.c_str(), v.vector3_binding);
                    } break;

                    case BindingValueType::VECTOR4: {
                        this->set_vec4(k.c_str(), v.vector4_binding);
                    } break;

                    case BindingValueType::MAT4: {
                        this->set_mat4(k.c_str(), v.mat4_binding);
                    } break;
                }
            }
        }
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