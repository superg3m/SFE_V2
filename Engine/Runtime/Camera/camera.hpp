#pragma once

#include "../Core/core.hpp"

enum class CameraDirection {
	UP,
	DOWN,
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float DEFAULTED_YAW         = -90.0f;
const float DEFAULTED_PITCH       =  0.0f;
const float DEFAULTED_SPEED       =  10.0f;
const float DEFAULTED_SENSITIVITY =  0.1f;
const float DEFAULTED_ZOOM        =  65.0f;

struct Camera {
	Vec3 position = Vec3(0.0f);
	Vec3 front;
	Vec3 up = Vec3(0, 1, 0);
	Vec3 right;
	Vec3 world_up = Vec3(0, 1, 0);
	
	float yaw = DEFAULTED_YAW;
	float pitch = DEFAULTED_PITCH;
	float movement_speed = DEFAULTED_SPEED;
	float mouse_sensitivity = DEFAULTED_SENSITIVITY;
	float zoom = DEFAULTED_ZOOM;

	static Camera create(Vec3 position);
	static Camera create(float x, float y, float z);
	Mat4 get_view_matrix();
	void lookat(Vec3 target_position);
	void lookat(float x, float y, float z);
	void process_keyboard(CameraDirection direction, float deltaTime);
	void process_mouse_movement(float xoffset, float yoffset, bool contrain_pitch = true);
	void process_mouse_scroll(float yoffset);

private:
	void update();
};