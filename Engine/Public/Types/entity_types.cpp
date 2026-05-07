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

MeshComponent::MeshComponent(Entity* owner, MeshHandle mesh, int entry_index, int instance_count, bool should_render_mesh) {
	this->owner = owner;
	this->mesh = mesh;
	this->entry_index = entry_index;
	this->instance_count = instance_count;
	this->should_render_mesh = should_render_mesh;
}

void MeshComponent::update(EngineAPI* engine, float dt) {
	engine->renderer.draw_mesh(this->mesh, this->entry_index, engine->manager.get_world_transform(this->owner->self), this->instance_count);
}

SkyboxComponent::SkyboxComponent(Entity* owner, MaterialHandle material) {
	this->owner = owner;
	this->material = material;
}

void SkyboxComponent::update(EngineAPI* engine, float dt) {
	engine->renderer.draw_skybox(this->material);
}