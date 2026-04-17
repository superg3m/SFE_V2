#include <entity.hpp>

struct EntityManager {
    Entity entities[256];
    int next_entity_index = 0;

    Entity* create_entity() {
        return &this->entities[this->next_entity_index++];
    }
};