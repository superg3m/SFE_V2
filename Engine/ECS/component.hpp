#pragma once
#include <core.hpp>

struct Entity;
namespace OpenGL {
	struct Mesh;
	struct MeshEntry;

	struct Shader;
	struct Material;
	struct RenderQueue;
	struct VAO;
}

struct Component {
	Entity* owner = nullptr;
	Component(int i) {}

	virtual void update(float dt) = 0;

	template<typename T>
	static int GetComponentID() {
		static int component_id = Component::next_component_id++;
		return component_id;
	}

protected:
	Component() = default;
	static int next_component_id;
};

struct HealthComponent : public Component {
	using Component::Component;

	int current_health = 0;
	int max_health = 0;

    HealthComponent(Entity* owner, int max_health);
    void update(float dt) override;

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
    void update(float dt) override;

	void StartBurn(float duration, int burn_dmg_per_tick);
	void StopBurn();
};

struct PlayerControllerComponent : public Component {
	using Component::Component;

    PlayerControllerComponent(Entity* owner);
    void update(float dt) override;
};

struct MeshComponent : public Component {
	using Component::Component;

	OpenGL::VAO* vao = nullptr;
	OpenGL::MeshEntry* entry = nullptr;
	OpenGL::Material* material = nullptr;
	OpenGL::RenderQueue* queue;

	bool should_render_mesh = true;
	bool render_mesh_wireframe = false;

	// OpenGL::Mesh* aabb_mesh;
	// bool should_render_mesh_aabb = false;

    MeshComponent(Entity* owner, OpenGL::RenderQueue* queue, OpenGL::Shader* shader, OpenGL::VAO* vao, OpenGL::MeshEntry* entry, OpenGL::Material* material);
    void update(float dt) override;
};

/*

struct CameraComponent : public Component {
	using Component::Component;

    float fov = 60.0f;
    float near_plane = 0.1f;
    float far_plane = 1000.0f;

    CameraComponent(Entity* owner, bool burnable, bool stunnable);
    void update(float dt) override;

    Math::Mat4 GetViewMatrix() {
        bool success = false;
        Math::Mat4 view = this->owner->GetWorldTrasform().inverse(success);
        if (!success) {
            LOG_ERROR("Failed to inverse camera world trasform\n");
        }

        return view;
    }

    Math::Mat4 GetProjectionMatrix(float aspect_ratio) {
        return Math::Mat4::Perspective(this->fov, aspect_ratio, this->near_plane, this->far_plane);
    }
};
*/