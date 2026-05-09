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

void CameraComponent::update(EngineAPI* engine, float dt) {
	Mat4 view = this->get_view_matrix(engine); // I mean this is very bad but, it is what it is

	this->front = Vec3(view.v[0].x, view.v[1].x, view.v[2].x).normalize();
	this->right = Vec3::cross(this->front, this->world_up).normalize();
	this->up    = Vec3::cross(this->right, this->front).normalize();
}

Mat4 CameraComponent::get_view_matrix(EngineAPI* engine) {
	bool success = false;
	Mat4 view = engine->manager.get_world_transform(this->owner->self).inverse(success);
	if (!success) {
		LOG_ERROR("failed to invert camera transform\n");
	}

	return view;
}

Mat4 CameraComponent::process_mouse_delta(Vec2 delta, bool contrain_pitch) {
	delta *= this->sensitivity;

	Vec3 euler = this->owner->transform.rotation.to_euler();

	euler.x += delta.x;
	euler.y += delta.y;

    if (contrain_pitch) {
		CLAMP(euler.y, -89.0f, 89.0f);
    }

	this->owner->transform.rotation = Quat::from_euler(euler);
}

Mat4 CameraComponent::get_projection_matrix(float aspect_ratio) {
	return Mat4::perspective(this->fov, aspect_ratio, this->near_plane, this->far_plane);
}

void CameraComponent::process_keyboard(CameraDirection direction, float speed, float dt) {
	float velocity = speed * dt;

	if (direction == CameraDirection::UP) {
		this->owner->transform.position = this->owner->transform.position + this->world_up.scale(velocity);
	}

	if (direction == CameraDirection::DOWN) {
		this->owner->transform.position = this->owner->transform.position + this->world_up.scale(-velocity);
	}

	if (direction == CameraDirection::FORWARD) {
		this->owner->transform.position = this->owner->transform.position + this->front.scale(velocity);
	}
	
	if (direction == CameraDirection::BACKWARD) {
		this->owner->transform.position = this->owner->transform.position + this->front.scale(-velocity);
	}
	
	if (direction == CameraDirection::LEFT) {
		this->owner->transform.position = this->owner->transform.position + this->right.scale(-velocity);
	}
	
	if (direction == CameraDirection::RIGHT) {
		this->owner->transform.position = this->owner->transform.position + this->right.scale(velocity);
	}
}

// TODO(Jovanni): ALRIGHT actually do this
void FirstPersonCameraControllerComponent::update(EngineAPI* engine, float dt) {
	CameraComponent* camera = this->owner->get_component<CameraComponent>();
	RUNTIME_ASSERT(camera);

	camera->process_mouse_delta(engine->input.input_state.mouse_delta * this->sensitivity * dt, true);

	if (engine->input.get_key(KEY_SPACE, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::UP, this->speed, dt);
	}

	if (engine->input.get_key(KEY_CTRL, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::DOWN, this->speed, dt);
	}

	if (engine->input.get_key(KEY_W, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::FORWARD, this->speed, dt); 
	}

	if (engine->input.get_key(KEY_A, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::LEFT, this->speed, dt); 
	}

	if (engine->input.get_key(KEY_S, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::BACKWARD, this->speed, dt);
	}

	if (engine->input.get_key(KEY_D, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::RIGHT, this->speed, dt); 
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