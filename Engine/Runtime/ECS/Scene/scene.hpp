#include <core.hpp>

struct Entity; 
struct EntityManager; 
struct Scene {
	static Scene create(EntityManager* manager);
 
	void update(float dt);
	EntityHandle create_entity(EntityManager* manager, String name, EntityHandle parent = EntityHandle::invalid());
	bool set_parent(EntityManager* manager, EntityHandle entity, EntityHandle parent);

	EntityHandle root = EntityHandle::invalid();
	EntityHandle main_camera = EntityHandle::invalid();
};