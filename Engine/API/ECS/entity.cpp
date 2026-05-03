#include "entity.hpp"
#include "scene.hpp"

void Entity::reparent(Scene* scene) {
	scene->set_parent(this->self, parent);
}

EntityHandle Entity::find_by_name(Scene* scene, String name) {
	if (this->name == name) {
		return this->self;
	}

	for (EntityHandle child : this->children) {
		Entity& child_slot = scene->entities.get(child.handle);
		EntityHandle result = child_slot.find_by_name(scene, name);
		if (result != EntityHandle::invalid()) {
			return result;
		}
	}

	return EntityHandle::invalid();
}