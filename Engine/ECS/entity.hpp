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

struct Entity {
    u32 id; // the actual index / unique id into an entity array owned by the engine
	const char* name;
    
	Entity* parent;
	std::vector<Entity*> children; 
	std::map<std::type_index, Component*> components; 
	bool alive = true; 
	bool active = true;

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
