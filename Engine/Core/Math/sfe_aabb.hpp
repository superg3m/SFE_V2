#pragma once

#include "sfe_vector.hpp"

namespace Math {
    struct AABB {
        Vec3 min;
        Vec3 max;

        AABB();
        AABB(Vec3 min, Vec3 max);
        AABB(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);

        Vec3 getCenter();
        Vec3 getExtents();

        static AABB FromCenterExtents(Vec3 center, Vec3 extents);
        static bool Intersection(AABB aabb, Vec3 p0, Vec3 p1);
    };
}