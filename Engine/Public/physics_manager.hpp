#pragma once

#include <vendor.hpp>

struct PhysicsManager {
	btBroadphaseInterface* broadphase = nullptr;
	btDefaultCollisionConfiguration collision_config;
	btCollisionDispatcher dispatcher;
	btSequentialImpulseConstraintSolver solver;
	btDiscreteDynamicsWorld world;

	PhysicsManager create() {
		this->broadphase = new btDbvtBroadphase();
		this->collision_config = btDefaultCollisionConfiguration();
		this->dispatcher = btCollisionDispatcher(&this->collision_config);
		this->solver = btSequentialImpulseConstraintSolver();
		this->world = btDiscreteDynamicsWorld(&this->dispatcher, this->broadphase, &this->solver, &this->collision_config);

		this->world.setGravity(btVector3(0, -9.8f, 0));
	}

	void update(float dt) {
		btScalar fixed_timestep = 1.0f / 60.0f;
		int substep_count = 4;
		this->world.stepSimulation(dt, substep_count, fixed_timestep);
	}
};