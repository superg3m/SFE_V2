#pragma once

#include "../../Public/scene_api.hpp"
// TODO(Jovanni): I actually think i'm going ot breka think into a EntityManager
// and then entity actulayy doesn't have anything aother than datat no methods so
// manager.set_matrix(entity, matrix);
struct EntityManager;
struct Scene {
	MemoryContext memory = {};
	EntityHandle root = EntityHandle::invalid();
	Camera active_camera = Camera(0, 0, 10);

	static Scene create(EntityManager* manager);
	void update(EngineAPI* api, float dt);

	SceneAPI API() {
		return SceneAPI(this->root, this->active_camera);
	}
};