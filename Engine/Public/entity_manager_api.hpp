#pragma once

#include "../Core/core.hpp"
#include "Types/entity_types.hpp"
#include "Requests/entity_manager_requests.hpp"

struct EntityManagerAPI {
	MemoryContext memory = {};
	Vector<EntityManagerRequest> deferred_requests = {};
	Registry<Entity, 256>* entities = nullptr;

	static EntityManagerAPI create(MemoryContext memory, Registry<Entity, 256>* entities) {
		EntityManagerAPI api = {};
		api.memory = memory;
		api.deferred_requests = Vector<EntityManagerRequest>(memory.frame_allocator);
		api.entities = entities;

		return api;
	}

	// NOTE(Jovanni):
	// 1. You may not reparent to yourself
	// 2. You may not reparent to to your current parent
	// 3. A parent my not have a parent of a child or decendent
	bool set_parent(EntityHandle entity, EntityHandle new_parent) {
		Entity& entity_slot = this->get(entity.handle);
		Entity& new_parent_slot = this->get(new_parent.handle);
		if (entity_slot.parent == EntityHandle::invalid()) {
			new_parent_slot.children.append(entity);
			entity_slot.parent = new_parent;
			return true;
		}

		Entity& current_parent_slot = this->get(entity_slot.parent.handle);
		bool self_parent = entity_slot.self == new_parent_slot.self;
		bool same_parent = current_parent_slot.self == new_parent_slot.self;
		if (self_parent || same_parent) return false;

		EntityHandle current_entity = new_parent;
		while (current_entity != EntityHandle::invalid()) {
			if (current_entity == entity) {
				return false; // NOTE(Jovanni): new_parent is a decendent of current_parent;
			}

			current_entity = this->get(current_entity.handle).parent;
		}

		s32 index = current_parent_slot.children.find(entity);
		RUNTIME_ASSERT_MSG(index != -1,"SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
		current_parent_slot.children.unstable_swapback_remove(index);
		new_parent_slot.children.append(entity);

		entity_slot.parent = new_parent;

		return true;
	}

	Entity& get(EntityHandle entity) {
		return this->entities->get(entity.handle);
	}

	Entity& create_entity(String name, EntityHandle parent) {
		EntityHandle entity = this->entities->acquire();
		Entity& entity_slot = this->get(entity);
		entity_slot.name = name;
		entity_slot.self = entity;
		entity_slot.children = Vector<EntityHandle>(this->memory.permanent_allocator);
		entity_slot.components = Hashmap<int, Component *>(this->memory.permanent_allocator);
		this->set_parent(entity, parent);

		return entity_slot;
	}

	/*
	Entity& create_entity_from_mesh(String name, EntityHandle parent, MeshHandle mesh, int instance_count = 1) {
		Entity& entity = this->create_entity(name, parent);
		this->deferred_requests.append(ENTITY_MANAGER_REQUEST_CREATE_ENTITY_FROM_MESH(entity.self, mesh, instance_count));

		return entity;
	}
	*/

	Entity& create_entity_from_model(String name, EntityHandle parent, ModelHandle model, int instance_count = 1) {
		Entity& entity = this->create_entity(name, parent);
		this->deferred_requests.append(ENTITY_MANAGER_REQUEST_CREATE_ENTITY_FROM_MODEL(entity.self, model, instance_count));

		return entity;
	}

	Mat4 get_world_transform(EntityHandle entity) {
		Entity& entity_slot = this->get(entity);
		if (entity_slot.parent == EntityHandle::invalid()) { // root
			return entity_slot.get_local_transform();
		}

		return this->get_world_transform(entity_slot.parent) * entity_slot.get_local_transform();
	}

	void set_world_transform(EntityHandle entity, Mat4 world_transform) {
		Entity& entity_slot = this->get(entity);
		if (entity_slot.parent != EntityHandle::invalid()) { // root
			bool success = false;
			Mat4 parent_inverse_world_transform = this->get_world_transform(entity_slot.parent).inverse(success);
			if (!success) {
				LOG_DEBUG("Failed to invert a matrix\n");
			}

			entity_slot.transform.set_matrix(parent_inverse_world_transform * world_transform);
		} else {
			entity_slot.transform.set_matrix(world_transform);
		}
	}

	template<typename T>
	Vector<Handle> query_component_list() {
		// TODO(Jovanni):
		Vector<Handle> ret = this->entities->handle_list(this->memory.frame_allocator);
		for (int i = 0; i < ret.count; i++) {
			Entity& entity_slot = this->entities->get(ret[i]);
			if (!entity_slot.has_component<T>()) {
				ret.unstable_swapback_remove(i);
			}
		}

		return ret;
	}

	template<typename ...Components>
	Vector<Handle> query_component_set_list() {
		Vector<Handle> ret = this->entities->handle_list(this->memory.frame_allocator);
		for (int i = 0; i < ret.count; i++) {
			Entity& entity_slot = this->entities->get(ret[i]);
			if (!entity_slot.has_components<Components...>()) {
				ret.unstable_swapback_remove(i);
			}
		}

		return ret;
	}
};