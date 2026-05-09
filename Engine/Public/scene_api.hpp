#pragma once

#include "Types/types.hpp"

struct SceneAPI {
	EntityHandle& root;
	EntityHandle& camera;

	SceneAPI(EntityHandle& root, EntityHandle& camera) : root(root), camera(camera) {}
};