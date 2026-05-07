#pragma once

#include "Types/types.hpp"

struct SceneAPI {
	EntityHandle& root;
	Camera& active_camera;

	SceneAPI(EntityHandle& root, Camera& active_camera) : root(root), active_camera(active_camera) {}
};