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
        if (euler.y > 89.0f) {
            euler.y = 89.0f;
        } else if (euler.y < -89.0f) {
            euler.y = -89.0f;
        }
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
void FreeCameraComponent::update(EngineAPI* engine, float dt) {
	// engine->input.input_state.mouse_delta * sensitivity * dt;
	// engine->input.input_state.mouse_delta * sensitivity * dt;
	/*
	if (inputManager.IsMousePositionChanged()) {

		float deltaX = currentPos.x - oldPos.x;
		float deltaY = currentPos.y - oldPos.y;

		// rot around Y axis
		float yDeltaAngle = -deltaX * m_sensitivity * deltaTime;
		m_yRot += yDeltaAngle;
		glm::quat yRot = glm::angleAxis(glm::radians(m_yRot), glm::vec3(0.0f, 1.0f, 0.0f));

		// rot around X axis
		float xDeltaAngle = -deltaY * m_sensitivity * deltaTime;
		m_xRot += xDeltaAngle;
		m_xRot = std::clamp(m_xRot, -89.0f, 89.0f);
		glm::quat xRot = glm::angleAxis(glm::radians(m_xRot), glm::vec3(1.0f, 0.0f, 0.0f));

		rotation = glm::normalize(yRot * xRot);

		m_owner->SetRotation(rotation);
	}
	*/

	if (engine->input.get_key(KEY_SPACE, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::UP, dt);
	}

	if (engine->input.get_key(KEY_CTRL, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::DOWN, dt);
	}

	if (engine->input.get_key(KEY_W, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::FORWARD, dt); 
	}

	if (engine->input.get_key(KEY_A, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::LEFT, dt); 
	}

	if (engine->input.get_key(KEY_S, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::BACKWARD, dt);
	}

	if (engine->input.get_key(KEY_D, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::RIGHT, dt); 
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