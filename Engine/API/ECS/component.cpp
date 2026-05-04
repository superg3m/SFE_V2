#include "component.hpp"
#include "entity.hpp"
#include "../api.hpp"
#include "../../Vendor/vendor.hpp"

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

MeshComponent::MeshComponent(Entity* owner, MeshHandle mesh, int entry_index, bool should_render_mesh) {
	this->owner = owner;
	this->mesh = mesh;
	this->entry_index = entry_index;
	this->should_render_mesh = should_render_mesh;
}

void MeshComponent::update(Engine* engine, float dt) {
	engine->renderer.draw_mesh(this->pipeline, this->mesh, this->owner->get_world_transform(&engine->scene), engine->get_view_matrix(), engine->get_projection_matrix());
}

/*
void MeshComponent::update(float dt) {
	Mat4 view          = Mat4(1.0f);
	Mat4 projection    = Mat4(1.0f);

	OpenGL::RenderCommandOpaque command = {};
	command.vao = this->vao;
	command.entry = this->entry;
	command.material = this->material;
	command.model = this->owner->get_world_transform();
	command.view = view;
	command.projection = projection;

	if (this->should_render_mesh) {
		this->queue->submit(command);
	}

	if (this->should_render_aabb_mesh) {
		OpenGL::RenderCommandOpaque aabb_command = {};
		aabb_command.shader = this->shader;
		aabb_command.mesh = this->aabb_mesh;
		aabb_command.model = this->owner->get_world_transform();
		aabb_command.view = view; // TODO(Jovanni): this shouldn't get supplied it should use active camera
		aabb_command.projection = projection; // TODO(Jovanni): this shouldn't get supplied it should use active camera

		this->queue->submit(aabb_command);
	}
}
*/