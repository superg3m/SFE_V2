
#pragma once

#include <core.hpp>
#include <texture.hpp>

namespace OpenGL {
    /*
    struct Material {
        sampler2D diffuse_texture
        float opacity;
    }
    */

    struct Material {
        static constexpr const char* DIFFUSE_TEXTURE = "diffuse_texture"; // texture unit = aiTextureType_DIFFUSE

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
    };
}