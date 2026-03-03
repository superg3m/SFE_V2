#include "../Common/sfe_common.hpp"
#include "sfe_aabb.hpp"

namespace Math {
    AABB AABB::FromCenterExtents(Vec3 center, Vec3 extents) {
        return AABB(center - extents, center + extents);
    }

    bool AABB::Intersection(AABB aabb, Vec3 p0, Vec3 p1) {
        float tmin = -10000;
        float tmax = 10000;
    
        // X coordinate
        if (fabsf(p1.x) > EPSILON) {
            float t1 = (aabb.min.x - p0.x) / p1.x;
            float t2 = (aabb.max.x - p0.x) / p1.x;
    
            tmin = MAX(tmin, MIN(t1, t2));
            tmax = MIN(tmax, MAX(t1, t2));
        }
    
        // Y coordinate
        if (fabs(p1.y) > EPSILON) {
            float t1 = (aabb.min.y - p0.y) / p1.y;
            float t2 = (aabb.max.y - p0.y) / p1.y;
    
            tmin = MAX(tmin, MIN(t1, t2));
            tmax = MIN(tmax, MAX(t1, t2));
        }
    
        // Z coordinate
        if (fabs(p1.z) > EPSILON) {
            float t1 = (aabb.min.z - p0.z) / p1.z;
            float t2 = (aabb.max.z - p0.z) / p1.z;
    
            tmin = MAX(tmin, MIN(t1, t2));
            tmax = MIN(tmax, MAX(t1, t2));
        }
    
    
        if (tmax > tmin && tmax > 0.0) {
            return true;
        } else {
            return false;
        }
    }

    AABB::AABB() {
        this->min = Vec3(0, 0, 0);
        this->max = Vec3(0, 0, 0);
    }

    AABB::AABB(Vec3 min, Vec3 max) {
        this->min = min;
        this->max = max;
    }

    AABB::AABB(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) {
        this->min.x = min_x;
        this->min.y = min_y;
        this->min.z = min_z;

        this->max.x = max_x;
        this->max.y = max_y;
        this->max.z = max_z;
    }

    Vec3 AABB::getCenter() {
        Vec3 extents = this->getExtents();
        return Vec3(min.x + extents.x, min.y + extents.y, min.z + extents.z);
    }

    Vec3 AABB::getExtents() {
        return (max - min).scale(0.5f);
    }
}
