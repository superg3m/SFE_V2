#pragma once

#include <core.hpp>
#include <component.hpp>
#include <typeindex>

#define COMPONENT(T)     \
T _##T = T(1);           \
template<>               \
T* GetMyComponent<T>() { \
	return &_##T;        \
}                        \

struct Transform {
    glm::vec3 position = glm::vec3(0);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale    = glm::vec3(0);
};

struct Entity {
    u32 id; // the actual index/unique id into an entity array owned by the engine
	const char* name;
    bool alive = true; 
	bool active = true;
    
	Entity* parent;
	std::vector<Entity*> children; 
	std::unordered_map<std::type_index, Component*> components; 
    Transform transform;

    void reparent(); // TODO(Jovanni): yeah gotta write this...

    void update(float dt) {
        for (const auto [k, v] : this->components) {
            v->update(dt);
        }
    }

    void set_rotation_euler(glm::vec3 euler) {
        this->transform.rotation = glm::quat(euler);
    }

    void set_rotation_euler(float x, float y, float z) {
        this->transform.rotation = glm::quat(glm::vec3(x, y, z));
    }

    glm::mat4 get_local_transform() const {
        glm::mat4 matrix = glm::mat4(1);
        matrix = glm::translate(matrix, this->transform.position);
        matrix = matrix * glm::mat4_cast(this->transform.rotation);
        matrix = glm::scale(matrix, this->transform.scale);

        return matrix;
    };

    glm::mat4 get_world_transform() const {
        if (this->parent) {
            return this->get_world_transform() * this->get_local_transform();
        }
        
        return this->get_local_transform();
    }

    static Entity* load_gltf(const std::string& path);

    template<typename T> 
	T* GetMyComponent();

    COMPONENT(HealthComponent)
	COMPONENT(MeshComponent)
	COMPONENT(StatusComponent)

    template<typename T>
    T* GetComponent() {
        return this->HasComponent<T>() ? this->GetMyComponent<T>() : nullptr;
    }

    template<typename T>
    bool HasComponent() {
        return this->components.count(typeid(T));
    }

	template<typename T, typename... Args>
	void AddComponent(Args&&... args) {
        T* c = this->GetMyComponent<T>();
        new (c) T(this, std::forward<Args>(args)...);
        this->components.insert(std::make_pair(typeid(T), c));
	}

	template<typename T>
	void RemoveComponent() {
		T* c = this->GetMyComponent<T>();
        auto it = this->components.find(typeid(T));
        if (it != this->components.end()) {
            this->components.erase(it);
        }
	}

private:
	Entity(const char* name) {
		this->name = name;
	}
};
