#include <scene.hpp>
#include <entity.hpp>
#include <entity_manager.hpp>

Scene Scene::create(EntityManager* manager) {
	Scene ret = {};
	ret.root = manager->create_entity(STR("ROOT"));
}
 
void Scene::update(float dt) {
	for (int i = 0; i < this->root.size(); i++) {
		Entity* child = this->root[i];
		if (child->alive) {
			child->update(dt);
		} else {
			this->root.erase(this->root.begin() + i);
		} 
	}
}

bool Scene::set_parent(EntityManager* manager, EntityHandle entity, EntityHandle new_parent) {
	// NOTE(Jovanni):
	// 1. You may not reparent to yourself
	// 2. You may not reparent to to your current parent
	// 3. A parent my not have a parent of a child or decendent

	Entity& root_slot = manager->entities.get(this->root.handle);
	Entity& entity_slot = manager->entities.get(entity.handle);
	Entity& current_parent_slot = manager->entities.get(entity_slot.parent.handle);
	Entity& new_parent_slot = manager->entities.get(new_parent.handle);

	bool current_parent_root = root_slot == current_parent_slot;
	bool new_parent_root = root_slot == new_parent_slot;
	if (current_parent_root && new_parent_root) {
		s32 index = root_slot.children.find(entity);
		if (index == -1) {
			root_slot.children.append(entity);
			return true;
		}

		return false;
	}

	bool self_parent = entity_slot == new_parent_slot;
	bool same_parent = current_parent_slot == new_parent_slot;
	if (self_parent || same_parent) return false;

	if (new_parent_root) {
		if (current_parent_root) {
			EntityHandle current_entity = new_parent;
			while (current_entity != this->root) {
				if (current_entity == entity) {
					// NOTE(Jovanni): new_parent is a decendent of current_parent;
					return false;
				}

				current_entity = manager->entities.get(current_entity.handle).parent;
			}

			s32 index = current_parent_slot.children.find(entity);
			RUNTIME_ASSERT_MSG(index != -1, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
			current_parent_slot.children.unstable_swapback_remove(index);
		} else {
			EntityHandle current_entity = new_parent;
			while (current_entity != this->root) {
				if (current_entity == entity) {
					// NOTE(Jovanni): new_parent is a decendent of current_parent;
					return false;
				}

				current_entity = manager->entities.get(current_entity.handle).parent;
			}

			const auto it = std::find(this->root.begin(), this->root.begin(), entity);
			bool found = it != this->root.end();
			RUNTIME_ASSERT_MSG(found, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
			this->root.erase(it);
		}

		new_parent->children.push_back(entity);
	} else {    
		RUNTIME_ASSERT_MSG(current_parent, "The case where its just create or already root parented should already be handled\n");

		const auto it = std::find(current_parent->children.begin(), current_parent->children.begin(), entity);
		bool found = it != this->root.end();
		RUNTIME_ASSERT_MSG(found, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
		current_parent->children.erase(it);
		this->root.push_back(entity);
	}

	entity->parent = new_parent;

	return true;
}

Entity* Scene::create_entity(EntityManager* manager, String name, EntityHandle parent) {
	Entity* entity = manager->create_entity();
	entity->name = name;
	this->set_parent(entity, parent);

	return entity;
}