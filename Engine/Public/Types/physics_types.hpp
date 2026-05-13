#pragma once

#include "../../Core/core.hpp"
#include <vendor.hpp>

struct Collider {
	btCollisionShape* shape;
};

struct BoxColider : public Collider {
	BoxColider(Vec3 extents) {
		Vec3 half_extents = extents * 0.5f;
		this->shape = new btBoxShape(btVector3(half_extents.x, half_extents.y, half_extents.z));
	}
};

struct SphereColider : public Collider {
	SphereColider(float radius) {
		this->shape = new btSphereShape(radius);
	}
};

struct CapsuleColider : public Collider {
	CapsuleColider(float radius, float height) {
		this->shape = new btCapsuleShape(radius, height);
	}
};

enum class PhysicsBodyType {
	STATIC,
	DYNAMIC,
	KINEMATIC,
};

struct RigidBody {
	PhysicsBodyType type = PhysicsBodyType::STATIC;
	Collider* collider = nullptr;
	btRigidBody* body = nullptr;

	float mass = 0.0f;
	float friction = 0.5f;
	float added_to_world = false;

	RigidBody(PhysicsBodyType type, Collider* collider, float mass, float friction) {
		this->type = type;
		this->collider = collider;
		this->mass = mass;
		this->friction = friction;

		btVector3 inertia = btVector3(0, 0, 0);
		if (this->type == PhysicsBodyType::DYNAMIC && mass > 0.0f && this->collider->shape) {
			this->collider->shape->calculateLocalInertia(mass, inertia);
		}

		btTransform transform;
		transform.setIdentity();
		btDefaultMotionState* motion_state = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo info = btRigidBody::btRigidBodyConstructionInfo(
			(this->type == PhysicsBodyType::DYNAMIC) ? mass : 0.0f,
			motion_state, this->collider->shape, inertia
		);

		this->body = new btRigidBody(info);
		this->body->setFriction(friction);

		if (this->type == PhysicsBodyType::KINEMATIC) {
			this->body->setCollisionFlags(this->body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			this->body->setActivationState(DISABLE_DEACTIVATION);
		}
	}

	void set_position(Vec3 pos) {
		if (!this->body) return;

		btTransform& transform = this->body->getWorldTransform();
		transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
		if (this->body->getMotionState()) {
			this->body->getMotionState()->setWorldTransform(transform);
		}

		this->body->setWorldTransform(transform);
		this->body->clearForces();
		body->setLinearVelocity(btVector3(0,0,0));
    	body->setAngularVelocity(btVector3(0,0,0));
		this->body->activate(true);
	}
	Vec3 get_position() {
		btVector3 pos = this->body->getWorldTransform().getOrigin();
		return Vec3(pos.x(), pos.y(), pos.z());
	}

	void set_rotation(Quat rotation) {
		if (!this->body) return;

		btTransform& transform = this->body->getWorldTransform();
		transform.setRotation(btQuaternion(rotation.v.x, rotation.v.y, rotation.v.z, rotation.w));
		if (this->body->getMotionState()) {
			this->body->getMotionState()->setWorldTransform(transform);
		}

		this->body->activate(true);
		this->body->setWorldTransform(transform);
		this->body->clearForces();
	}

	Quat get_rotation() {
		btQuaternion quat = this->body->getWorldTransform().getRotation();
		return Quat::literal(quat.w(), Vec3(quat.x(), quat.y(), quat.z()));
	}
};