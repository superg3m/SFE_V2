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