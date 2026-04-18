#pragma once

#include <entity.hpp>

/*
- [] entity manager, should have like all the entities[256], should have:
    Entity* create_entity() // give you a return &entites[next++]
    // realisitically this hsould give you a handle but one thing at a time batman
    std::vector<Entity*> QueryComponentList<MeshComponent>() 
    std::vector<Entity*> QuerySetComponentList<MeshComponent, GravityComponent...>()
*/

constexpr int ENTITY_MAX_COUNT = 256;
struct EntityManager {
    Entity entities[ENTITY_MAX_COUNT];
    int entity_count = 0;

    Entity* create_entity() {
        RUNTIME_ASSERT(entity_count >= ENTITY_MAX_COUNT);

        return &this->entities[this->entity_count++];
    }

    template<typename T>
    std::vector<Entity*> QueryComponentList() {
        std::vector<Entity*> ret;
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



