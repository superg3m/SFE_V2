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

CameraComponent::CameraComponent(Entity* owner, Vec3 position) {
	this->owner = owner;
	this->owner->transform.position = position;
}

void CameraComponent::update(EngineAPI* engine, float dt) {
	this->front = Vec3::euler(this->yaw, this->pitch).normalize();
	this->right = Vec3::cross(this->front, this->world_up).normalize();
	this->up    = Vec3::cross(this->right, this->front).normalize();
}

void CameraComponent::lookat(Vec3 target_position) {
	Vec3 target_direction = (target_position - this->owner->transform.position).normalize(); 
	this->yaw   = RAD_TO_DEGREES(atan2(target_direction.z, target_direction.x));
	this->pitch = RAD_TO_DEGREES(asin(target_direction.y));
}

void CameraComponent::lookat(float x, float y, float z) {
	Vec3 target_direction = (Vec3(x, y, z) - this->owner->transform.position).normalize(); 
	this->yaw   = RAD_TO_DEGREES(atan2(target_direction.z, target_direction.x));
	this->pitch = RAD_TO_DEGREES(asin(target_direction.y));
}

Mat4 CameraComponent::get_view_matrix() {
	return Mat4::lookat(this->owner->transform.position, this->owner->transform.position + this->front, this->up);
}

Mat4 CameraComponent::get_perspective_matrix(float aspect) {
	return Mat4::perspective(this->fov, aspect, this->near_plane, this->far_plane);
}

void CameraComponent::process_keyboard(CameraDirection direction, float deltaTime) {
	float velocity = this->movement_speed * deltaTime;

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

void CameraComponent::process_mouse_movement(Vec2 delta, bool contrain_pitch) {
	delta *= this->mouse_sensitivity;

	this->yaw   += delta.x;
	this->pitch += delta.y;

	if (contrain_pitch) {
		if (this->pitch > 89.0f) {
			this->pitch = 89.0f;
		} else if (this->pitch < -89.0f) {
			this->pitch = -89.0f;
		}
	}
}

/*
void Camera::process_mouse_scroll(float yoffset) {
	this->zoom -= yoffset;
	if (this->zoom < 1.0f) {
		this->zoom = 1.0f;
	}

	if (this->zoom > 45.0f) {
		this->zoom = 45.0f;
	}
}
*/

FirstPersonCameraControllerComponent::FirstPersonCameraControllerComponent(Entity* owner) {
	this->owner = owner;
}

// TODO(Jovanni): ALRIGHT actually do this
void FirstPersonCameraControllerComponent::update(EngineAPI* engine, float dt) {
	CameraComponent* camera = this->owner->get_component<CameraComponent>();
	RUNTIME_ASSERT(camera);

	if (engine->input.get_key(KEY_SPACE, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::UP, dt);
	}

	if (engine->input.get_key(KEY_CTRL, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::DOWN, dt);
	}

	if (engine->input.get_key(KEY_W, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::FORWARD, dt); 
	}

	if (engine->input.get_key(KEY_A, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::LEFT, dt); 
	}

	if (engine->input.get_key(KEY_S, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::BACKWARD, dt);
	}

	if (engine->input.get_key(KEY_D, PRESSED|DOWN)) {
		camera->process_keyboard(CameraDirection::RIGHT, dt); 
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