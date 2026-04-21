
#pragma once

#include <core.hpp>
#include <material.hpp>
#include <shader.hpp>
#include <mesh.hpp>

/*
enum class AssetType {
    MESH
    SHADER,
    MATERIAL,
    SOUND
};

struct Asset {
    AssetType type;
    union {
        OpenGL::Mesh mesh;
        OpenGL::Shader shader;
        OpenGL::Texture texture;
        OpenGL::Material material;
        OpenGL::Sound sound;
    };
};

typedef Handle<Asset> AssetHandle;

struct AssetManager {
    Registry<Asset> registry;
    T* get(AssetHandle handle) {
        return registry.get(handle);
    }

    OpenGL::Texture load_texture(std::string name, OpenGL::TextureUnit texture_unit, const char* asset_path);
    // OpenGL::Texture load_material(const char* asset_path);
    OpenGL::Shader load_shader(std::string name, std::vector<std::string> shader_paths);
    OpenGL::Mesh load_mesh(const char* asset_path, OpenGL::Shader* shader);

    void reload_shaders() {
        for (Slot<Asset>* slot : registry.slots) {
            if (slot.data.type == SHADER) {
                slot.data.shader.compile();
            }
        }
    }
}

*/

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