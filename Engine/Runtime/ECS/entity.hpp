#pragma once

#include <core.hpp>
#include <component.hpp>
#include <typeindex>

/*
- [X] entity manager, should have like all the entities[256], should have:
	std::vector<Entity*> QuerySetComponentList<MeshComponent, GravityComponent...>()
- [x] Scene
- [] active camera

- [] probably you can just add a 
	ComponentFlags to an entity this will make serialization trivial.
	Theres also another reason this is nice. I can guarentee the order in which entity 
	data/components will update.
	update.

	You can have an asset handle and then you don’t have to do the weird material key.
	Basically and asset id a tagged union and you can have a registry with a bunch of assets.
	Then the mesh entries get an asset handle to the material maybe?
	
- [] Start shifting the responsibility on the entity system
*/

#define COMPONENT(T)     \
T _##T = T(1);           \
template<>               \
T* GetMyComponent<T>() { \
	return &_##T;        \
}                        \

struct Transform {
	Vec3 position = Vec3(0);
	Quat rotation = Quat::identity();
	Vec3 scale    = Vec3(0);
};

struct Entity {
	u32 id; // the actual index/unique id into an entity array owned by the engine
	const char* name;
	bool alive = true; 
	bool active = true;
	
	Entity* parent;
	Vector<Entity*> children;
	Hashmap<std::type_index, Component*> components; // TODO(Jovanni): Test this to make sure it works
	Transform transform;

	void reparent(); // TODO(Jovanni): yeah gotta write this...

	void update(float dt) {
		for (const auto [k, v] : this->components) {
			v->update(dt);
		}
	}

	void set_rotation_euler(Vec3 euler) {
		this->transform.rotation = Quat::from_euler(euler);
	}

	void set_rotation_euler(float x, float y, float z) {
		this->transform.rotation = Quat::from_euler(x, y, z);
	}

	Mat4 get_local_transform() const {
		Mat4 matrix = Mat4::identity();
		matrix = Mat4::scale(matrix, this->transform.scale);
		matrix = Mat4::rotate(matrix, this->transform.rotation);
		matrix = Mat4::translate(matrix, this->transform.position);

		return matrix;
	};

	Mat4 get_world_transform() const {
		if (this->parent) {
			return this->get_world_transform() * this->get_local_transform();
		}
		
		return this->get_local_transform();
	}

	static Entity* load_gltf(const char* path);

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

	template<typename T> 
	T* GetMyComponent();

	COMPONENT(HealthComponent)
	COMPONENT(MeshComponent)
	COMPONENT(StatusComponent)

private:
	Entity(const char* name) {
		this->name = name;
	}
};
