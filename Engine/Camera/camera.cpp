#include "camera.hpp"

void Camera::update() {
	Vec3 new_front = Vec3::euler(this->yaw, this->pitch);
	this->front = new_front.normalize();
	this->right = Vec3::cross(this->front, this->world_up).normalize();
	this->up    = Vec3::cross(this->right, this->front).normalize();
}

Camera Camera::create(Vec3 position) {
	Camera ret = {}; 
	ret.position = position;
	ret.update();

	return ret;
}

Camera Camera::create(float x, float y, float z) {
	Camera ret = {}; 
	ret.position = Vec3(x, y, z);
	ret.update();

	return ret;
}

void Camera::lookat(Vec3 target_position) {
	Vec3 target_direction = (target_position - this->position).normalize(); 
	this->yaw   = RAD_TO_DEGREES(atan2(target_direction.z, target_direction.x));
	this->pitch = RAD_TO_DEGREES(asin(target_direction.y));
	
	this->update();
}

void Camera::lookat(float x, float y, float z) {
	Vec3 target_direction = (Vec3(x, y, z) - this->position).normalize(); 
	this->yaw   = RAD_TO_DEGREES(atan2(target_direction.z, target_direction.x));
	this->pitch = RAD_TO_DEGREES(asin(target_direction.y));
	
	this->update();
}

Mat4 Camera::get_view_matrix() {
	return Mat4::lookat(this->position, this->position + this->front, this->up);
}

void Camera::process_keyboard(CameraDirection direction, float deltaTime) {
	float velocity = this->movement_speed * deltaTime;

	if (direction == CameraDirection::UP) {
		this->position = this->position + this->world_up.scale(velocity);
	}

	if (direction == CameraDirection::DOWN) {
		this->position = this->position + this->world_up.scale(-velocity);
	}

	if (direction == CameraDirection::FORWARD) {
		this->position = this->position + this->front.scale(velocity);
	}
	
	if (direction == CameraDirection::BACKWARD) {
		this->position = this->position + this->front.scale(-velocity);
	}
	
	if (direction == CameraDirection::LEFT) {
		this->position = this->position + this->right.scale(-velocity);
	}
	
	if (direction == CameraDirection::RIGHT) {
		this->position = this->position + this->right.scale(velocity);
	}
}

void Camera::process_mouse_movement(float xoffset, float yoffset, bool contrain_pitch) {
	xoffset *= this->mouse_sensitivity;
	yoffset *= this->mouse_sensitivity;

	this->yaw   += xoffset;
	this->pitch += yoffset;

	if (contrain_pitch) {
		if (this->pitch > 89.0f) {
			this->pitch = 89.0f;
		} else if (this->pitch < -89.0f) {
			this->pitch = -89.0f;
		}
	}

	this->update();
}

void Camera::process_mouse_scroll(float yoffset) {
	this->zoom -= yoffset;
	if (this->zoom < 1.0f) {
		this->zoom = 1.0f;
	}

	if (this->zoom > 45.0f) {
		this->zoom = 45.0f;
	}
}