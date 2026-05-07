#pragma once

#include "../Types/entity_types.hpp"

enum class EntityManagerRequestType {
	CREATE_ENTITY_FROM_MESH,
};

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

struct EntityManagerRequest {
	EntityManagerRequestType type;
	union {
		CreateEntityFromMeshRequest create_entity_from_mesh;
	};
};

#define ENTITY_MANAGER_REQUEST_CREATE_ENTITY_FROM_MESH(entity, mesh, instance_count) EntityManagerRequest{EntityManagerRequestType::CREATE_ENTITY_FROM_MESH, CreateEntityFromMeshRequest::create(entity, mesh, instance_count)}
