#include "entity_types.hpp"
#include "../engine_api.hpp"

void Entity::update(EngineAPI* engine, float dt) {
	if (!this->active) return;
	
	for (const auto& entry : this->components) {
		Component* c = entry.value;
		c->update(engine, dt);
	}

	for (EntityHandle child : this->children) {
		Entity& child_slot = engine->manager.get(child);
		child_slot.update(engine, dt);
	}
}

Mat4 CameraComponent::get_view_matrix(EngineAPI* engine) {
	bool success = false;
	Mat4 view = engine->manager.get_world_transform(this->owner->self).inverse(success);
	if (!success) {
		LOG_ERROR("failed to invert camera transform\n");
	}

	return view;
}

Mat4 CameraComponent::get_projection_matrix(float aspect_ratio) {
	return Mat4::perspective(this->fov, aspect_ratio, this->near_plane, this->far_plane);
}

/*
void PlayerControllerComponent::update(float dt) {
	StatusComponent* status = this->owner->GetComponent<StatusComponent>();
	HealthComponent* health = this->owner->GetComponent<HealthComponent>();
	MeshComponent* mesh = this->owner->GetComponent<MeshComponent>();
	if (status && status->stunned)  {
		return;
	}
	
	if (Input::GetKeyDown(KEY_A)) {
		this->owner.position.x -= 1.0f;
	}
}
*/

MeshComponent::MeshComponent(Entity* owner, MeshHandle mesh, int instance_count) {
	this->owner = owner;
	this->mesh = mesh;
	this->instance_count = instance_count;
}

void MeshComponent::update(EngineAPI* engine, float dt) {
	if (!this->should_render) return;
	engine->renderer.draw_mesh(this->mesh, this->rasterizer_description, engine->manager.get_world_transform(this->owner->self), this->instance_count);

	if (!this->render_aabb) return;
	engine->renderer.draw_aabb(this->mesh, engine->manager.get_world_transform(this->owner->self));
}

SkyboxComponent::SkyboxComponent(Entity* owner, MaterialHandle material) {
	this->owner = owner;
	this->material = material;
}

void SkyboxComponent::update(EngineAPI* engine, float dt) {
	engine->renderer.draw_skybox(this->material);
}