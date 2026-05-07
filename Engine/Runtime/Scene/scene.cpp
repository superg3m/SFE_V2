#include "scene.hpp"
#include "../EntityManager/entity_manager.hpp"
#include "../engine.hpp"

Scene Scene::create(EntityManager* manager) {
	Scene ret = {};
	ret.memory = manager->memory;
	ret.root = manager->entities.acquire();
	ret.active_camera = Camera::create(0, 0, 10);

	Entity& root_slot = manager->get(ret.root.handle);
	root_slot.self = ret.root;
	root_slot.name = STR("ROOT");
	root_slot.parent = EntityHandle::invalid();

	return ret;
}
 
void Scene::update(EngineAPI* api, float dt) {
	Entity& root_slot = api->manager.get(this->root.handle);
	root_slot.update(api, dt);
}