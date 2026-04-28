#pragma once

#include "entity.hpp"
#include "../Core/core.hpp"

struct EntityManager {
	Registry<Entity, 256> registry;
	Handle<Entity> create_entity();

	template<typename T>
    Handle<Entity> QueryComponentList() {
        Vector<Entity> ret;
        for (int i = 0; i < this->entity_count; i++) {
            Entity* entity = &this->entities[i];
            if (entity->HasComponent<T>()) {
                ret.push_back(entity);
            }
        }

        return ret;
    }

    /*
    template<typename ...Components>
    std::vector<Entity*> QuerySetComponentList() {
        std::vector<Entity*> ret;
        for (int i = 0; i < this->entity_count; i++) {
            Entity* entity = &this->entities[i];
            ret.push_back(entity);
        }

        return ret;
    }
    */
};

/*
- [] entity manager, should have like all the entities[256], should have:
    Entity* create_entity() // give you a return &entites[next++]
    // realisitically this hsould give you a handle but one thing at a time batman
    std::vector<Entity*> QueryComponentList<MeshComponent>() 
    std::vector<Entity*> QuerySetComponentList<MeshComponent, GravityComponent...>()
*/