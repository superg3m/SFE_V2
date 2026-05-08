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