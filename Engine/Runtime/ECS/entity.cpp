#include "entity.hpp"
#include "scene.hpp"

void Entity::reparent(Scene* scene) {
	scene->set_parent(this->self, parent);
}