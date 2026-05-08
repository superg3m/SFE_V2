#include "entity_manager.hpp"

// NOTE(Jovanni):
// 1. You may not reparent to yourself
// 2. You may not reparent to to your current parent
// 3. A parent my not have a parent of a child or decendent
bool EntityManager::set_parent(EntityHandle entity, EntityHandle parent) {
	return this->API().set_parent(entity, parent);
}

Entity& EntityManager::create_entity(String name, EntityHandle parent) {
	return this->API().create_entity(name, parent);
}

void EntityManager::create_entity_from_model(Renderer<OpenGL>* renderer, EntityHandle entity, ModelHandle model, int instance_count) {
	OpenGL::Model& model_slot = renderer->get(model);
	if (model_slot.meshes.count == 1) {
		Entity& entity_slot = this->get(entity);
		entity_slot.add_component<MeshComponent>(model_slot.meshes[0].self, instance_count);
		return;
	}

	for (int i = 0; i < model_slot.meshes.count; i++) {
		OpenGL::Mesh& mesh = model_slot.meshes[i];
		Entity& child = this->create_entity(mesh.name, entity);
		child.add_component<MeshComponent>(mesh.self, instance_count);
	}
}

/*
Entity& EntityManager::create_entity_from_mesh(Renderer<OpenGL>* renderer, String name, EntityHandle parent, MeshHandle mesh) {
	EntityHandle head_entity = this->entities.acquire();
	Entity& entity_slot = this->get(head_entity);
	entity_slot.self = head_entity;
	entity_slot.name = name;
	this->set_parent(head_entity, parent);

	OpenGL::Mesh mesh_slot = renderer->get(mesh);

	// EntityHandle entity = this->entities.acquire();
	// Entity& entity_slot = this->get(entity);
	// entity_slot.self = entity;
	// entity_slot.name = name;
	// this->set_parent(entity, parent);

	// return entity;
}
*/

Entity& EntityManager::get(EntityHandle entity) {
	return this->entities.get(entity.handle);
}

Mat4 EntityManager::get_world_transform(EntityHandle entity) {
	return this->API().get_world_transform(entity);
}

void EntityManager::set_world_transform(EntityHandle entity, Mat4 world_transform) {
	return this->API().set_world_transform(entity, world_transform);
}