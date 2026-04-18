#include <asset_manager.hpp>

OpenGL::Texture AssetManager::load_texture(std::string name, OpenGL::TextureUnit texture_unit, const char* asset_path) {
    if (this->textures.count(name)) {
        return this->textures[name];
    }

    std::string relative_path = std::string("../../Assets/Textures/") + asset_path;
    this->textures[name] = OpenGL::Texture::load_from_file(texture_unit, relative_path.c_str());
    return this->textures[name];
}

/*
OpenGL::Texture AssetManager::load_material(const char* asset_path) {
    std::string relative_path = std::string(ASSET_ROOT"/Materials") + asset_path;
    if (this->textures.count(relative_path)) {
        return this->textures[relative_path];
    }

    this->materials[relative_path] = OpenGL::Material::load_from_file(relative_path.c_str());
    return this->textures[relative_path];
}
*/

OpenGL::Shader AssetManager::load_shader(std::string name, std::vector<std::string> shader_paths) {
    std::string relative_path = std::string("../../Assets/Shaders/") + shader_paths[0];
    if (this->shaders.count(name)) {
        return this->shaders[name];
    }

    for (int i = 0; i < shader_paths.size(); i++) {
        shader_paths[i] = std::string("../../Assets/Shaders/") + shader_paths[i];
    }

    this->shaders[name] = OpenGL::Shader::create(shader_paths);
    return this->shaders[name];
}

OpenGL::Mesh AssetManager::load_mesh(const char* asset_path, OpenGL::Shader* shader) {
    std::string relative_path = std::string("../../Assets/Models/") + asset_path;
    if (this->meshes.count(relative_path)) {
        return this->meshes[relative_path];
    }

    this->meshes[relative_path] = OpenGL::Mesh::load_from_file(this->materials, shader, relative_path);
    return this->meshes[relative_path];
}

void AssetManager::reload_shaders() {
    for (auto& [k, v] : this->shaders) {
        v.compile();
    }
}