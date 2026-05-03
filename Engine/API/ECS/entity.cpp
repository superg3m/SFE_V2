#include "entity.hpp"
#include "scene.hpp"

void Entity::reparent(Scene* scene) {
	scene->set_parent(this->self, parent);
}

Mat4 Entity::get_world_transform(Scene* scene) const {
	if (this->parent == EntityHandle::invalid()) { // root
		return this->get_local_transform();
	}

	Entity& parent_slot = scene->entities.get(this->parent.handle);
	return parent_slot.get_world_transform(scene) * this->get_local_transform();
}

void Entity::set_world_transform(Scene* scene, Mat4 world_transform) {
	if (parent != EntityHandle::invalid()) {
		Entity& parent_entity_slot = scene->entities.get(this->parent.handle);
		bool success = false;
		Mat4 parent_inverse_world_transform = parent_entity_slot.get_world_transform(scene).inverse(success);
		if (!success) {
			LOG_DEBUG("Failed to invert a matrix\n");
		}

		this->transform.set_matrix(parent_inverse_world_transform * world_transform);
	} else {
		this->transform.set_matrix(world_transform);
	}
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