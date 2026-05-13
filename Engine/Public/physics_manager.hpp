#pragma once

#include <vendor.hpp>
#include "Types/physics_types.hpp"

struct PhysicsManager {
	btBroadphaseInterface* broadphase = nullptr;
	btDefaultCollisionConfiguration* collision_config;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* world;

	void add_rigid_body(RigidBody* rigidbody) {
		RUNTIME_ASSERT(rigidbody);

		if (rigidbody->body) {
			this->world->addRigidBody(rigidbody->body, btBroadphaseProxy::StaticFilter, btBroadphaseProxy::AllFilter);
			rigidbody->added_to_world = true;
		}
	}

	void remove_rigid_body(RigidBody* rigidbody) {
		RUNTIME_ASSERT(rigidbody);

		if (rigidbody->body) {
			this->world->removeRigidBody(rigidbody->body);
			rigidbody->added_to_world = false;
		}
	}

	static PhysicsManager create() {
		PhysicsManager ret = {};
		ret.broadphase = new btDbvtBroadphase();
		ret.collision_config = new btDefaultCollisionConfiguration();
		ret.dispatcher = new btCollisionDispatcher(ret.collision_config);
		ret.solver = new btSequentialImpulseConstraintSolver();
		ret.world = new btDiscreteDynamicsWorld(ret.dispatcher, ret.broadphase, ret.solver, ret.collision_config);

		ret.world->setGravity(btVector3(0, -9.8f, 0));

		return ret;
	}

	void update(float dt) {
		btScalar fixed_timestep = 1.0f / 60.0f;
		int substep_count = 4;
		this->world->stepSimulation(dt, substep_count, fixed_timestep);
	}
};