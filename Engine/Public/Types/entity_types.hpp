#pragma once

#include "../../Core/core.hpp"
#include "render_types.hpp"

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

constexpr uint64_t fnv1a(const char* str) {
	uint64_t hash = 14695981039346656037ull;

	while (*str) {
		hash ^= (uint64_t)(unsigned char)*str++;
		hash *= 1099511628211ull;
	}

	return hash;
}

struct Engine;
struct Entity;
struct Component {
	Entity* owner = nullptr;
	Component(int i) {}

	virtual void update(EngineAPI* engine, float dt) = 0;

	template<typename T>
	static int get_component_id(const char* component_name) {
		return fnv1a(component_name);
	}

protected:
	Component() = default;
};

enum class CameraDirection {
	UP,
	DOWN,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct CameraComponent : public Component {
	using Component::Component;

	float fov = 65.0f;
	float near_plane = 0.1f;
	float far_plane = 1000.0f;
	float mouse_sensitivity = 0.1f;
	float movement_speed = 10.0f;

	float yaw   = -90.0f;
	float pitch = 0.0f;

	Vec3 world_up = Vec3(0, 1, 0);
	Vec3 up = {};
	Vec3 front = {};
	Vec3 right = {};

	CameraComponent(Entity* owner, Vec3 position);

	void update(EngineAPI* engine, float dt) override;
	void lookat(Vec3 target_position);
	void lookat(float x, float y, float z);
	Mat4 get_view_matrix();
	Mat4 get_perspective_matrix(float aspect);
	void process_keyboard(CameraDirection direction, float deltaTime);
	void process_mouse_movement(Vec2 delta, bool contrain_pitch);
	void process_mouse_scroll(float yoffset);
};

struct FirstPersonCameraControllerComponent : public Component {
	using Component::Component;

	FirstPersonCameraControllerComponent(Entity* owner);
	void update(EngineAPI* engine, float dt) override;
};

struct PointLightComponent : public Component {
	using Component::Component;

    Vec3 position;
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;

	PointLightComponent(Entity* owner, Vec3 position);

	void update(EngineAPI* engine, float dt) override {};
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

#define COMPONENT(T)       			\
T _##T = T(1);             			\
template<>                 			\
T* get_my_component<T>() { 			\
	return &_##T;          			\
}                          			\
template<>							\
const char* component_name<T>() {	\
	return STRINGIFY(T);         	\
}                                	\

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
	Hashmap<int, Component*> components; // TODO(Jovanni): Test this to make sure it works

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
		return this->components.has(Component::get_component_id<T>(this->component_name<T>()));
	}

	template<typename T, typename... Rest>
	bool has_components() {
		return has_component<T>() && has_components<Rest...>();
	}

	template<typename T, typename... Args>
	void add_component(Args&&... args) {
		T* c = this->get_my_component<T>();
		new (c) T(this, std::forward<Args>(args)...);
		this->components.put(Component::get_component_id<T>(this->component_name<T>()), c);
	}

	template<typename T>
	void remove_component() {
		T* c = this->get_my_component<T>();
		this->components.remove(Component::get_component_id<T>(this->component_name<T>()));
	}

	template<typename T> 
	T* get_my_component();

	template<typename T>
	const char* component_name();

	COMPONENT(HealthComponent)
	COMPONENT(MeshComponent)
	COMPONENT(StatusComponent)
	COMPONENT(SkyboxComponent)
	COMPONENT(CameraComponent)
	COMPONENT(FirstPersonCameraControllerComponent)
};
