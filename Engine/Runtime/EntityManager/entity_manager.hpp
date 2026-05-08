#pragma once

#include "../Public/entity_manager_api.hpp"
#include "../Renderer/renderer.hpp"

struct EntityManager {
	MemoryContext memory = {};
	Registry<Entity, 256> entities = {};

	static EntityManager create(MemoryContext memory) {
		EntityManager ret = {};
		ret.memory = memory;

		return ret;
	}

	bool set_parent(EntityHandle entity, EntityHandle parent);
	Entity& create_entity(String name, EntityHandle parent);
	void create_entity_from_model(Renderer<OpenGL>* renderer, EntityHandle entity, ModelHandle mesh, int instance_count);
	Entity& get(EntityHandle entity);

	Mat4 get_world_transform(EntityHandle entity);
	void set_world_transform(EntityHandle entity, Mat4 world_transform);

	void execute_deferred_requests(Renderer<OpenGL>* renderer, EngineAPI* api) {
		for (EntityManagerRequest& request : api->manager.deferred_requests) {
			switch (request.type) {
				case EntityManagerRequestType::CREATE_ENTITY_FROM_MODEL: {
					this->create_entity_from_model(renderer, request.create_entity_from_model.user, request.create_entity_from_model.model, request.create_entity_from_model.instance_count);
				} break;

				default: {
					RUNTIME_ASSERT(false);
				} break;
			}
		}

		api->manager.deferred_requests.clear();
	}

	EntityManagerAPI API() {
		EntityManagerAPI api = EntityManagerAPI::create(this->memory, &this->entities);
		return api;
	}

	template<typename T>
	Vector<EntityHandle> query_component_list() {
		return this->API().query_component_list<T>();
	}

	template<typename ...Components>
	Vector<EntityHandle> query_component_set_list() {
		return this->API().query_component_set_list<Components...>();
	}
};