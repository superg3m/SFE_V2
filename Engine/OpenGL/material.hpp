
#include <core.hpp>
#include <texture.hpp>
#include <shader.hpp>

namespace OpenGL {
    struct Material {
        std::map<std::string, bool> boolean_uniforms;
        std::map<std::string, int> integer_uniforms;
        std::map<std::string, float> float_uniforms;
        std::map<std::string, Texture> sampler2d_uniforms;
        std::map<std::string, Texture> cubemap_uniforms;
        std::map<std::string, glm::vec2> vector2_uniforms;
        std::map<std::string, glm::vec3> vector3_uniforms;
        std::map<std::string, glm::vec4> vector4_uniforms;
        std::map<std::string, glm::mat4> mat4_uniforms;

        void set_bool(const char* name, bool value) {this->boolean_uniforms.insert(std::make_pair(name, value));}
        void set_int(const char* name, int value) {this->integer_uniforms.insert(std::make_pair(name, value));}
        void set_float(const char* name, float value) {this->float_uniforms.insert(std::make_pair(name, value));}
        void set_texture(const char* name, Texture texture) {this->sampler2d_uniforms.insert(std::make_pair(name, texture));}
        void set_texture_cube(const char* name, Texture texture) {this->cubemap_uniforms.insert(std::make_pair(name, texture));}
        void set_vec2(const char* name, const glm::vec2& value) {this->vector2_uniforms.insert(std::make_pair(name, value));}
        void set_vec2(const char* name, float x, float y) {this->vector2_uniforms.insert(std::make_pair(name, glm::vec2(x, y)));}
        void set_vec3(const char* name, const glm::vec3& value) {this->vector3_uniforms.insert(std::make_pair(name, value));}
        void set_vec3(const char* name, float x, float y, float z) {this->vector3_uniforms.insert(std::make_pair(name, glm::vec3(x, y, z)));}
        void set_vec4(const char* name, const glm::vec4& value) {this->vector4_uniforms.insert(std::make_pair(name, value));}
        void set_vec4(const char* name, float x, float y, float z, float w) {this->vector4_uniforms.insert(std::make_pair(name, glm::vec4(x, y, z, w)));}
        void set_mat4(const char* name, const glm::mat4& mat) {this->mat4_uniforms.insert(std::make_pair(name, mat));}

        // TODO(Jovanni): this is so fucking slow but for now its ok I will switch to my own hashmap later
        // TODO(Jovanni): this is so fucking slow but for now its ok I will switch to my own hashmap later
        // TODO(Jovanni): this is so fucking slow but for now its ok I will switch to my own hashmap later
        // TODO(Jovanni): this is so fucking slow but for now its ok I will switch to my own hashmap later
        void set_uniforms(Shader* shader) {
            for (const auto [k, v] : this->boolean_uniforms) {
                shader->set_bool(k.c_str(), v);
            }

            for (const auto [k, v] : this->integer_uniforms) {
                shader->set_int(k.c_str(), v);
            }

            for (const auto [k, v] : this->float_uniforms) {
                shader->set_float(k.c_str(), v);
            }

            for (const auto [k, v] : this->sampler2d_uniforms) {
                shader->set_texture(k.c_str(), v);
            }

            for (const auto [k, v] : this->cubemap_uniforms) {
                shader->set_texture_cube(k.c_str(), v);
            }

            for (const auto [k, v] : this->vector2_uniforms) {
                shader->set_vec2(k.c_str(), v);
            }

            for (const auto [k, v] : this->vector3_uniforms) {
                shader->set_vec3(k.c_str(), v);
            }

            for (const auto [k, v] : this->vector4_uniforms) {
                shader->set_vec4(k.c_str(), v);
            }

            for (const auto [k, v] : this->mat4_uniforms) {
                shader->set_mat4(k.c_str(), v);
            }
        }
    };
}