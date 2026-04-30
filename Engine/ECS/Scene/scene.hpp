#include <core.hpp>

struct Entity; 
struct EntityManager; 
struct Scene {
	void update(float dt);
	void clear();

	Entity* create_entity(EntityManager* manager, const char* name, Entity* parent = nullptr);
	bool set_parent(Entity* entity, Entity* parent);

	std::vector<Entity*> root;
	Entity* main_camera = nullptr;
};