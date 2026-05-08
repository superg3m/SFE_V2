#pragma once

#include "../Types/entity_types.hpp"

enum class EntityManagerRequestType {
	// CREATE_ENTITY_FROM_MESH,
	CREATE_ENTITY_FROM_MODEL,
};

/*
struct CreateEntityFromMeshRequest {
	EntityHandle user = EntityHandle::invalid();
	MeshHandle mesh = MeshHandle::invalid();
	int instance_count = 1;

	CreateEntityFromMeshRequest() = default;
	static CreateEntityFromMeshRequest create(EntityHandle user, MeshHandle mesh, int instance_count) {
		CreateEntityFromMeshRequest ret = {};
		ret.user = user;
		ret.mesh = mesh;
		ret.instance_count = instance_count;
		
		return ret;
	}
};
*/

struct CreateEntityFromModelRequest {
	EntityHandle user = EntityHandle::invalid();
	ModelHandle model = ModelHandle::invalid();
	int instance_count = 1;

	CreateEntityFromModelRequest() = default;
	static CreateEntityFromModelRequest create(EntityHandle user, ModelHandle model, int instance_count) {
		CreateEntityFromModelRequest ret = {};
		ret.user = user;
		ret.model = model;
		ret.instance_count = instance_count;
		
		return ret;
	}
};

struct EntityManagerRequest {
	EntityManagerRequestType type;
	union {
		// CreateEntityFromMeshRequest create_entity_from_mesh;
		CreateEntityFromModelRequest create_entity_from_model;
	};
};

// #define ENTITY_MANAGER_REQUEST_CREATE_ENTITY_FROM_MESH(entity, mesh, instance_count) EntityManagerRequest{.type = EntityManagerRequestType::CREATE_ENTITY_FROM_MESH, .create_entity_from_mesh = CreateEntityFromMeshRequest::create(entity, mesh, instance_count)}
#define ENTITY_MANAGER_REQUEST_CREATE_ENTITY_FROM_MODEL(entity, model, instance_count) EntityManagerRequest{.type = EntityManagerRequestType::CREATE_ENTITY_FROM_MODEL, .create_entity_from_model = CreateEntityFromModelRequest::create(entity, model, instance_count)}
