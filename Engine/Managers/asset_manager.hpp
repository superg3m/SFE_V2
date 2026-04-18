
#pragma once

#include <core.hpp>
#include <material.hpp>
#include <shader.hpp>
#include <mesh.hpp>

struct AssetManager {
    std::unordered_map<std::string, OpenGL::Shader> shaders;
    std::unordered_map<std::string, OpenGL::Mesh> meshes;
    std::unordered_map<std::string, OpenGL::Texture> textures;
    MaterialMap materials;

    void init() {
        this->materials = MaterialMap(8, material_key_hash, material_key_equal);
        this->shaders.reserve(64);
        this->meshes.reserve(64);
        this->textures.reserve(64);
        this->materials.reserve(64);
    }

    OpenGL::Texture load_texture(std::string name, OpenGL::TextureUnit texture_unit, const char* asset_path);
    // OpenGL::Texture load_material(const char* asset_path);
    OpenGL::Shader load_shader(std::string name, std::vector<std::string> shader_paths);
    OpenGL::Mesh load_mesh(const char* asset_path, OpenGL::Shader* shader);
    void reload_shaders();
};