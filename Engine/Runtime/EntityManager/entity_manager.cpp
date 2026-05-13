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

void EntityManager::add_mesh(EntityHandle parent, OpenGL::Mesh& mesh, int instance_count) {
	Entity& entity = this->create_entity(mesh.name, parent);
	if (mesh.sub_meshes.count) {
		entity.add_component<MeshComponent>(mesh.self, instance_count);
	}

	for (OpenGL::Mesh& child_mesh : mesh.children) {
		add_mesh(entity.self, child_mesh, instance_count);
	}
}

void EntityManager::create_entity_from_model(Renderer<OpenGL>* renderer, EntityHandle entity, ModelHandle model, int instance_count) {
	OpenGL::Model& model_slot = renderer->get(model);

	for (int i = 0; i < model_slot.meshes.count; i++) {
		OpenGL::Mesh& mesh = model_slot.meshes[i];
		this->add_mesh(entity, mesh, instance_count);
	}
}

Entity& EntityManager::get(EntityHandle entity) {
	return this->entities.get(entity.handle);
}

Mat4 EntityManager::get_world_transform(EntityHandle entity) {
	return this->API().get_world_transform(entity);
}

void EntityManager::set_world_transform(EntityHandle entity, Mat4 world_transform) {
	return this->API().set_world_transform(entity, world_transform);
}