#include "scene.hpp"
#include "entity.hpp"

Scene Scene::create(MemoryContext memory) {
	Scene ret = {};
	ret.memory = memory = memory;
	ret.root = ret.entities.acquire();
	Entity& root_slot = ret.entities.get(ret.root.handle);
	root_slot.self = ret.root;
	root_slot.name = STR("ROOT");
	root_slot.parent = EntityHandle::invalid();

	return ret;
}
 
void Scene::update(float dt) {
	Entity& root_slot = this->entities.get(this->root.handle);
	for (int i = 0; i < root_slot.children.count; i++) {
		EntityHandle child = root_slot.children[i];
		Entity& child_slot = this->entities.get(child.handle);
		if (child_slot.alive) {
			child_slot.update(dt);
		} else {
			root_slot.children.unstable_swapback_remove(i);
		} 
	}
}

bool Scene::set_parent(EntityHandle entity, EntityHandle new_parent) {
	// NOTE(Jovanni):
	// 1. You may not reparent to yourself
	// 2. You may not reparent to to your current parent
	// 3. A parent my not have a parent of a child or decendent
	Entity& root_slot = this->entities.get(this->root.handle);
	Entity& entity_slot = this->entities.get(entity.handle);
	Entity& new_parent_slot = this->entities.get(new_parent.handle);
	if (entity_slot.parent == EntityHandle::invalid()) {
		new_parent_slot.children.append(entity);
		entity_slot.parent = new_parent;
		return true;
	}

	Entity& current_parent_slot = this->entities.get(entity_slot.parent.handle);
	bool self_parent = entity_slot.self == new_parent_slot.self;
	bool same_parent = current_parent_slot.self == new_parent_slot.self;
	if (self_parent || same_parent) return false;

	bool current_parent_root = root_slot.self == current_parent_slot.self;
	bool new_parent_root = root_slot.self == new_parent_slot.self;

	/*
	bool current_parent_root = root_slot.self == current_parent_slot.self;
	bool new_parent_root = root_slot.self == new_parent_slot.self;
	if (current_parent_root && new_parent_root) {
		s32 index = root_slot.children.find(entity);
		if (index == -1) {
			root_slot.children.append(entity);
			return true;
		}

		return false;
	}
	*/

	EntityHandle current_entity = new_parent;
	while (current_entity != this->root) {
		if (current_entity == entity) {
			// NOTE(Jovanni): new_parent is a decendent of current_parent;
			return false;
		}

		current_entity = this->entities.get(current_entity.handle).parent;
	}

	if (new_parent_root) {
		if (!current_parent_root) {
			s32 index = current_parent_slot.children.find(entity);
			RUNTIME_ASSERT_MSG(index != -1, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
			current_parent_slot.children.unstable_swapback_remove(index);
		} else {
			s32 index = root_slot.children.find(entity);
			RUNTIME_ASSERT_MSG(index != -1, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
			root_slot.children.unstable_swapback_remove(index);
		}

		new_parent_slot.children.append(entity);
	} else {   
		s32 index = current_parent_slot.children.find(entity);
		RUNTIME_ASSERT_MSG(index != -1,"SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
		current_parent_slot.children.unstable_swapback_remove(index);
		new_parent_slot.children.append(entity);
	}

	entity_slot.parent = new_parent;

	return true;
}

EntityHandle Scene::create_entity(String name, EntityHandle parent) {
	EntityHandle entity = this->entities.acquire(); 
	Entity& entity_slot = this->entities.get(entity.handle); 
	entity_slot.self = entity;
	entity_slot.name = name;
	this->set_parent(entity, parent);

	return entity;
}