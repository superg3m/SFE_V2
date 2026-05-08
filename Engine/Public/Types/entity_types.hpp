#pragma once

#include "../../Core/core.hpp"
#include "render_types.hpp"

#include <typeindex>

struct EngineAPI;

struct EntityHandle {
	Handle handle = Handle::invalid();
	static EntityHandle invalid() {
		return Handle::invalid();
	}

	EntityHandle(Handle handle) {
		this->handle = handle;
	}

	bool operator==(EntityHandle other) const {
		return this->handle == other.handle;
	}
};

struct Engine;
struct Entity;
struct Component {
	Entity* owner = nullptr;
	Component(int i) {}

	virtual void update(EngineAPI* engine, float dt) = 0;

	template<typename T>
	static int get_component_id() {
		static int component_id = Component::next_component_id++;
		return component_id;
	}

protected:
	Component() = default;
	static int next_component_id;
};

struct CameraComponent : public Component {
	using Component::Component;

	float fov = 60.0f;
	float near_plane = 0.1f;
	float far_plane = 1000.0f;

	CameraComponent(Entity* owner, int max_health);
	void update(EngineAPI* engine, float dt) override {};
	Mat4 get_view_matrix(EngineAPI* engine);
	Mat4 get_projection_matrix(float aspect_ratio);
};

struct HealthComponent : public Component {
	using Component::Component;

	int current_health = 0;
	int max_health = 0;

	HealthComponent(Entity* owner, int max_health);
	void update(EngineAPI* engine, float dt) override {};
	void Damage(int dmg);
};

struct StatusComponent : public Component {
	using Component::Component;

	bool burnable = true;
	bool burning = false;
	// damage per tick isn't a great idea because high framerates have more ticks
	// should be like burn_dmg_per_second = 1;
	// then you can do Math::remap(burn_timer.elapsed)
	int burn_dmg_per_tick = 0;
	Timer burn_timer = {};

	bool stunnable = true;
	bool stunned = false;
	Timer status_timer = {};

	StatusComponent(Entity* owner, bool burnable, bool stunnable);
	void update(EngineAPI* engine, float dt) override {};

	void StartBurn(float duration, int burn_dmg_per_tick);
	void StopBurn();
};

struct PlayerControllerComponent : public Component {
	using Component::Component;

	PlayerControllerComponent(Entity* owner);
	void update(EngineAPI* engine, float dt) override;
};

struct MeshComponent : public Component {
	using Component::Component;
	MeshHandle mesh = MeshHandle::invalid();
	Mat4 model = Mat4::identity();
	int instance_count = 1;

	bool should_render = true;
	bool render_aabb = false;
	RasterizerDescription rasterizer_description = {};

	MeshComponent(Entity* owner, MeshHandle mesh, int instance_count);
	void update(EngineAPI* engine, float dt) override;
};

struct SkyboxComponent : public Component {
	using Component::Component;

	MaterialHandle material = MaterialHandle::invalid();

	SkyboxComponent(Entity* owner, MaterialHandle material);
	void update(EngineAPI* engine, float dt) override;
};

#define COMPONENT(T)     \
T _##T = T(1);           \
template<>               \
T* get_my_component<T>() { \
	return &_##T;        \
}                        \


struct Transform {
	Vec3 position = Vec3(0);
	Quat rotation = Quat::identity();
	Vec3 scale    = Vec3(1);

	Mat4 get_matrix() const {
		return Mat4::transform(this->scale, this->rotation, this->position);
	}

	void set_matrix(Mat4 m) {
		Mat4::decompose(m, &this->position, &this->rotation, &this->scale);
	}
};

struct Entity {
	EntityHandle self = EntityHandle::invalid();
	EntityHandle parent = EntityHandle::invalid();
	Vector<EntityHandle> children = {};
	Hashmap<std::type_index, Component*> components; // TODO(Jovanni): Test this to make sure it works

	String name = {};
	Transform transform = {};
	bool active = true;

	Entity() = default;

	void update(EngineAPI* engine, float dt);
	void set_rotation_euler(Vec3 euler) {
		this->transform.rotation = Quat::from_euler(euler);
	}

	void set_rotation_euler(float x, float y, float z) {
		this->transform.rotation = Quat::from_euler(x, y, z);
	}

	Mat4 get_local_transform() const {
		return this->transform.get_matrix();
	};

	template<typename T>
	T* get_component() {
		return this->has_component<T>() ? this->get_my_component<T>() : nullptr;
	}

	template<typename T>
	bool has_component() {
		return this->components.has(typeid(T));
	}

	template<typename T, typename... Rest>
	bool has_components() {
		return has_component<T>() && has_components<Rest...>();
	}

	template<typename T, typename... Args>
	void add_component(Args&&... args) {
		T* c = this->get_my_component<T>();
		new (c) T(this, std::forward<Args>(args)...);
		this->components.put(typeid(T), c);
	}

	template<typename T>
	void remove_component() {
		T* c = this->get_my_component<T>();
		this->components.remove(typeid(T));
	}

	template<typename T> 
	T* get_my_component();

	COMPONENT(HealthComponent)
	COMPONENT(MeshComponent)
	COMPONENT(StatusComponent)
	COMPONENT(SkyboxComponent)
};
