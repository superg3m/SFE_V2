#pragma once

#include "entity_handle.hpp"

// TODO(Jovanni): I actually think i'm going ot breka think into a EntityManager
// and then entity actulayy doesn't have anything aother than datat no methods so
// manager.set_matrix(entity, matrix);


struct Scene {
	MemoryContext memory = {};
	Registry<Entity, 256> entities = {};
	// Hashmap<std::type_index, Vector<EntityHandle>> cached_component_lists = {};
	EntityHandle root = EntityHandle::invalid();
	EntityHandle main_camera = EntityHandle::invalid();

	static Scene create(MemoryContext memory);
	void update(float dt);
	EntityHandle create_entity(String name, EntityHandle parent);
	bool set_parent(EntityHandle entity, EntityHandle parent);

	template<typename T>
	Vector<EntityHandle> QueryComponentList() {
		/* 
		// TODO(Jovanni): THis won't work until I actually enforce an entity needs to know about the mangaer, so when it adds a component itcan addit it here too...
		// that actually means one entity manager allowed 
		if (this->cached_component_lists.has(typeid(T))) {
			return this->cached_component_lists.get(typeid(T));
		}

		this->cached_component_lists.get(typeid(T));
		*/

		// TODO(Jovanni): Obviously this is unbelievably slow, but I can cache it
		Vector<EntityHandle> ret = this->entities.handle_list();
		for (int i = 0; i < ret.count; i++) {
			Entity& entity_slot = this->entities.get(ret[i]);
			if (!entity_slot.HasComponent<T>()) {
				ret.unstable_swapback_remove(i);
			}
		}

		return ret;
	}

	template<typename ...Components>
	Vector<EntityHandle> QueryComponentSetList() {
		// TODO(Jovanni): Obviously this is unbelievably slow
		Vector<EntityHandle> ret = this->entities.handle_list();
		for (int i = 0; i < ret.count; i++) {
			Entity& entity_slot = this->entities.get(ret[i]);
			if (!entity_slot.HasComponents<Components...>()) {
				ret.unstable_swapback_remove(i);
			}
		}

		return ret;
	}
};