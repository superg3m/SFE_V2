#pragma once

#include "entity.hpp"
#include "../Core/core.hpp"

struct EntityManager {
	MemoryContext memory = {};
	Registry<Entity, 256> entities = {};
	Hashmap<std::type_index, Vector<EntityHandle>> cached_component_lists = {};

	EntityManager create(MemoryContext memory) {
		EntityManager ret = {};  
		ret.memory = memory;
	}

	EntityHandle create_entity(String name) {
		EntityHandle entity = this->entities.acquire();
		Entity& entity_slot = this->entities.get(entity.handle); 
		entity_slot.name = name;

		return entity;
	}

	template<typename T>
	Vector<EntityHandle> QueryComponentList() {
		/* 
		// THis won't work until I actually enforce an entity needs to know about the mangaer, so when it adds a component itcan addit it here too...
		// that actually means one entity manager allowed 
		if (this->cached_component_lists.has(typeid(T))) {
			return this->cached_component_lists.get(typeid(T));
		}

		this->cached_component_lists.get(typeid(T));
		*/

		// TODO(Jovanni): Obviously this is unbelievably slow, but I can cache it
		Vector<Entity> ret = {};
		for (int i = 0; i < this->entity_count; i++) {
			Entity* entity = &this->entities[i];
			if (entity->HasComponent<T>()) {
				ret.push_back(entity);
			}
		}

		return ret;
	}

	template<typename ...Components>
	Vector<EntityHandle> QueryComponentSetList() {
		// TODO(Jovanni): Obviously this is unbelievably slow
		Vector<Entity> ret = {};
		for (int i = 0; i < this->entity_count; i++) {
			Entity* entity = &this->entities[i];
			if (entity->HasComponents<Components>()) {
				ret.push_back(entity);
			}
		}

		return ret;
	}
};

/*
- [] entity manager, should have like all the entities[256], should have:
	Entity* create_entity() // give you a return &entites[next++]
	// realisitically this hsould give you a handle but one thing at a time batman
	std::vector<Entity*> QueryComponentList<MeshComponent>() 
	std::vector<Entity*> QuerySetComponentList<MeshComponent, GravityComponent...>()
*/