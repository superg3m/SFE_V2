#include "entity.hpp"
#include "../Core/core.hpp"

struct EntityManager {
	Registry<Entity, 256> registry;
	Handle<Entity> create_entity();
};