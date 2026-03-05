#pragma once

#include <array>
#include "../../Core/Common/sfe_common.hpp"

namespace Math {
    // Date: May 18, 2025
    // NOTE(Jovanni): Visualize these at: https://easinfgs.net/
    /*
    float ease_in_sinfe(float t);
    float ease_out_sinfe(float t);
    float ease_in_out_sinfe(float t);
    float ease_in_quad(float t);
    float ease_out_quad(float t);
    float ease_in_cubic(float t);
    float ease_out_cubic(float t);
    float ease_in_out_cubic(float t);
    float ease_in_quart(float t);
    float ease_out_quart(float t);
    float ease_in_out_quart(float t);
    float ease_in_quint(float t);
    float ease_out_quint(float t);
    float ease_in_out_quint(float t);
    float ease_in_expo(float t);
    float ease_out_expo(float t);
    float ease_in_out_expo(float t);
    float ease_in_circ(float t);
    float ease_out_circ(float t);
    float ease_in_out_circ(float t);
    float ease_in_back(float t);
    float ease_out_back(float t);
    float ease_in_out_back(float t);
    float ease_in_elastic(float t);
    float ease_out_elastic(float t);
    float ease_in_out_elastic(float t);
    float ease_in_bounce(float t);
    float ease_out_bounce(float t);
    float ease_in_out_bounce(float t);
    */

    template <typename T>
    inline T Average(T* array, int count) {
        T accumlator = 0;
        for (int i = 0; i < count; i++) {
            accumlator += array[i];
        }

        T average = (T)accumlator / (T)count;

        return average;
    }

    inline float Lerp(float a, float b, float t) {
        return a + ((b - a) * t);
    }

    inline float InverseLerp(float a, float b, float value) {
        if (NEAR_ZERO(a - b)) {
            return 0.0f; // Avoid division by zero
        }

        return (value - a) / (b - a);
    }

    inline float Remap(float x, float s_min, float s_max, float e_min, float e_max) {
        x = CLAMP(x, s_min, s_max);
        float s_ratio = (x - s_min) / (s_max - s_min);
        
        return e_min + (s_ratio * (e_max - e_min));
    }

    inline float MoveToward(float current, float target, float delta) {
        float diff = target - current;

        if (fabsf(diff) <= delta) {
            return target;
        }

        return current + (diff > 0 ? delta : -delta);
    }

    inline int Mod(int a, int b) {
        int rem = a % b;
        return (rem < 0) ? (rem + b) : rem;
    }

    typedef struct Vec4 Vec4;
    struct Vec2 {
        float x;
        float y;

        Vec2() {
            this->x = 0.0f;
            this->y = 0.0f;
        }

        Vec2(float fill) {
            this->x = fill;
            this->y = fill;
        }

        Vec2(float x, float y) {
            this->x = x;
            this->y = y;
        }

        float magnitude() {
            return sqrtf((this->x * this->x) + (this->y * this->y));
        }

        float magnitudeSquared() {
            return (this->x * this->x) + (this->y * this->y);
        }

        Vec2 normalize() {
            Vec2 ret(0, 0);
            const float magnitude = this->magnitude();
            if (magnitude == 0) {
                return Vec2(0,0);
            }

            ret.x = this->x / magnitude;
            ret.y = this->y / magnitude;

            return ret;
        }

        Vec2 scale(float scale) const {
            return Vec2(this->x * scale, this->y * scale);
        }

        Vec2 scale(Vec2 s) const {
            return Vec2(this->x * s.x, this->y * s.y);
        }

        Vec2 scale(float scale_x, float scale_y) const {
            return Vec2(this->x * scale_x, this->y * scale_y);
        }

        /**
         * @brief NOTE(Jovanni): This only applies if both a and b vectors are normalized:
         * -1: the vectors are 180 degrees from eachother in other words they vectors are pointing in opposite directions
         *  0: the vectors are perpendicular or orthogonal to eachother
         *  1: the vectors are going the same direction
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static float Dot(Vec2 a, Vec2 b) {
            return (a.x * b.x) + (a.y * b.y);
        }

        static float Distance(Vec2 a, Vec2 b) {
            return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y));
        }

        static float DistanceSquared(Vec2 a, Vec2 b) {
            return SQUARED(b.x - a.x) + SQUARED(b.y - a.y);
        }

        /**
         * @brief Returns either vector a or b depending on which is closest to the target
         */
        static Vec2 Closest(Vec2 a, Vec2 b, Vec2 target = Vec2(0)) {
            float a_distance = Vec2::DistanceSquared(a, target);
            float b_distance = Vec2::DistanceSquared(b, target);

            if (a_distance <= b_distance) {
                return a;
            }

            return b;
        }

        static Vec2 Lerp(Vec2 a, Vec2 b, float t) {
            Vec2 ab = (b - a);
            return a + (ab.scale(t));
        }

        static Vec2 MoveToward(Vec2 a, Vec2 b, float dt) {
            Vec2 ret = Vec2(0);
            ret.x = Math::MoveToward(a.x, b.x, dt);
            ret.y = Math::MoveToward(a.y, b.y, dt);
            
            return ret;
        }

        static Vec2 Euler(float yaw, float pitch) {
            Vec2 ret = Vec2(0, 0);
            ret.x = cosf(DEGREES_TO_RAD(yaw)) * cosf(DEGREES_TO_RAD(pitch));
            ret.y = sinf(DEGREES_TO_RAD(pitch));

            return ret;
        }

        Vec2 operator+(const Vec2 &right) {
            return Vec2(this->x + right.x, this->y + right.y);
        }
        Vec2& operator+=(const Vec2 &right) {
            this->x += right.x;
            this->y += right.y;

            return *this;
        }

        Vec2 operator-(const Vec2 &right) {
            return Vec2(this->x - right.x, this->y - right.y);
        }
        Vec2& operator-=(const Vec2 &right) {
            this->x -= right.x;
            this->y -= right.y;

            return *this;
        }

        Vec2 operator*(const Vec2 &right) {
            return Vec2(this->x * right.x, this->y * right.y);
        }

        Vec2& operator*=(const Vec2 &right) {
            this->x *= right.x;
            this->y *= right.y;

            return *this;
        }

        Vec2 operator/(const Vec2 &right) {
            return Vec2(this->x / right.x, this->y / right.y);
        }
        Vec2& operator/=(const Vec2 &right) {
            this->x /= right.x;
            this->y /= right.y;

            return *this;
        }

        bool operator==(const Vec2 &right) {
            return NEAR_ZERO(this->x - right.x) && NEAR_ZERO(this->y - right.y);
        }

        bool operator!=(const Vec2 &right) {
            return !(*this == right);
        }
    };

    typedef struct Vec4 Vec4;
    struct Vec3 {
        union {
            struct {
                float x;
                float y;
                float z;
            };

            struct {
                float r;
                float g;
                float b;
            };
        };

        Vec3() {
            this->x = 0.0f;
            this->y = 0.0f;
            this->z = 0.0f;
        }

        Vec3(float fill) {
            this->x = fill;
            this->y = fill;
            this->z = fill;
        }

        Vec3(float x, float y, float z) {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        Vec3(Vec2 v, float z) {
            this->x = v.x;
            this->y = v.y;
            this->z = z;
        }

        Vec3(Vec4 v);

        float magnitude() {
            return sqrtf(SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z));
        }

        float magnitudeSquared() {
            return SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z);
        }

        Vec3 normalize() {
            Vec3 ret(0, 0, 0);
            const float magnitude = this->magnitude();
            if (magnitude == 0) {
                return Vec3(0, 0, 0);
            }

            ret.x = this->x / magnitude;
            ret.y = this->y / magnitude;
            ret.z = this->z / magnitude;

            return ret;
        }

        Vec3 scale(float scale) const {
            return Vec3(this->x * scale, this->y * scale, this->z * scale);
        }

        Vec3 scale(Vec3 s) const {
            return Vec3(this->x * s.x, this->y * s.y, this->z * s.z);
        }

        Vec3 scale(float scale_x, float scale_y, float scale_z) const {
            return Vec3(this->x * scale_x, this->y * scale_y, this->z * scale_z);
        }

        static float Distance(Vec3 a, Vec3 b) {
            return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y) + SQUARED(b.z - a.z));
        }

        static float DistanceSquared(Vec3 a, Vec3 b) {
            return SQUARED(b.x - a.x) + SQUARED(b.y - a.y) + SQUARED(b.z - a.z);
        } 

        /**
         * @brief Returns either vector a or b depending on which is closest to the target
         */
        static Vec3 Closest(Vec3 a, Vec3 b, Vec3 target = Vec3(0)) {
            float a_distance = Vec3::DistanceSquared(a, target);
            float b_distance = Vec3::DistanceSquared(b, target);

            if (a_distance <= b_distance) {
                return a;
            }

            return b;
        }

        static float Dot(Vec3 a, Vec3 b) {
            return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
        }

        static Vec3 Lerp(Vec3 a, Vec3 b, float t) {
            Vec3 ab = (b - a);
            return a + (ab.scale(t));
        }

        static Vec3 MoveToward(Vec3 a, Vec3 b, float dt) {
            Vec3 ret = Vec3(0);
            ret.x = Math::MoveToward(a.x, b.x, dt);
            ret.y = Math::MoveToward(a.y, b.y, dt);
            ret.z = Math::MoveToward(a.z, b.z, dt);
            
            return ret;
        }

        static Vec3 Cross(Vec3 A, Vec3 B) {
            Vec3 ret(0, 0, 0);
            ret.x = A.y * B.z - A.z * B.y;
            ret.y = A.z * B.x - A.x * B.z;
            ret.z = A.x * B.y - A.y * B.x;

            return ret;
        }

        static Vec3 Euler(float yaw, float pitch) {
            Vec3 ret = Vec3(0, 0, 0);
            ret.x = cosf(DEGREES_TO_RAD(yaw)) * cosf(DEGREES_TO_RAD(pitch));
            ret.y = sinf(DEGREES_TO_RAD(pitch));
            ret.z = sinf(DEGREES_TO_RAD(yaw)) * cosf(DEGREES_TO_RAD(pitch));

            return ret;
        }

        Vec3 operator+(const Vec3 &right) {
            return Vec3(this->x + right.x, this->y + right.y, this->z + right.z);
        }
        Vec3& operator+=(const Vec3 &right) {
            this->x += right.x;
            this->y += right.y;
            this->z += right.z;

            return *this;
        }

        Vec3 operator-(const Vec3 &right) {
            return Vec3(this->x - right.x, this->y - right.y, this->z - right.z);
        }
        Vec3& operator-=(const Vec3 &right) {
            this->x -= right.x;
            this->y -= right.y;
            this->z -= right.z;

            return *this;
        }


        Vec3 operator*(const Vec3 &right) {
            return Vec3(this->x * right.x, this->y * right.y, this->z * right.z);
        }
        Vec3& operator*=(const Vec3 &right) {
            this->x *= right.x;
            this->y *= right.y;
            this->z *= right.z;

            return *this;
        }


        Vec3 operator/(const Vec3 &right) {
            return Vec3(this->x / right.x, this->y / right.y, this->z / right.z);
        }
        Vec3& operator/=(const Vec3 &right) {
            this->x /= right.x;
            this->y /= right.y;
            this->z /= right.z;

            return *this;
        }

        bool operator==(const Vec3 &right) {
            return NEAR_ZERO(this->x - right.x) && NEAR_ZERO(this->y - right.y) && NEAR_ZERO(this->z - right.z);
        }
        bool operator!=(const Vec3 &right) {
            return !(*this == right);
        }
    };

    struct Vec4 {
        union {
            struct {
                float x;
                float y;
                float z;
                float w;
            };

            struct {
                float r;
                float g;
                float b;
                float a;
            };
        };

        Vec4() {
            this->x = 0.0f;
            this->y = 0.0f;
            this->z = 0.0f;
            this->w = 0.0f;
        }

        Vec4(float fill) {
            this->x = fill;
            this->y = fill;
            this->z = fill;
            this->w = fill;
        }

        Vec4(float x, float y, float z, float w) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        Vec4(Vec3 v, float w) {
            this->x = v.x;
            this->y = v.y;
            this->z = v.z;
            this->w = w;
        }

        float magnitude() {
            return sqrtf(SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w));
        }

        float magnitudeSquared() {
            return SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w);
        }

        Vec4 normalize() {
            Vec4 ret(0, 0, 0, 0);
            const float magnitude = this->magnitude();
            if (magnitude == 0) {
                return Vec4(0, 0, 0, 0);
            }

            ret.x = this->x / magnitude;
            ret.y = this->y / magnitude;
            ret.z = this->z / magnitude;
            ret.w = this->w / magnitude;

            return ret;
        }

        Vec4 scale(float scale) const {
            return Vec4(this->x * scale, this->y * scale, this->z * scale, this->w * scale);
        }

        Vec4 scale(Vec4 s) const {
            return Vec4(this->x * s.x, this->y * s.y, this->z * s.z, this->w * s.w);
        }

        Vec4 scale(float scale_x, float scale_y, float scale_z, float scale_w) const {
            return Vec4(this->x * scale_x, this->y * scale_y, this->z * scale_z, this->w * scale_w);
        }

        static float Distance(Vec4 a, Vec4 b) {
            return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y) + SQUARED(b.z - a.z) + SQUARED(b.w - a.w));
        }

        static float DistanceSquared(Vec4 a, Vec4 b) {
            return SQUARED(b.x - a.x) + SQUARED(b.y - a.y) + SQUARED(b.z - a.z) + SQUARED(b.w - a.w);
        }

        /**
         * @brief Returns either vector a or b depending on which is closest to the target
         */
        static Vec4 Closest(Vec4 a, Vec4 b, Vec4 target = Vec4(0)) {
            float a_distance = Vec4::DistanceSquared(a, target);
            float b_distance = Vec4::DistanceSquared(b, target);

            if (a_distance <= b_distance) {
                return a;
            }

            return b;
        }

        /**
         * @brief NOTE(Jovanni): This only applies if both a and b vectors are normalized:
         * -1: the vectors are 180 degrees from eachother in other words they vectors are pointing in opposite directions
         *  0: the vectors are perpendicular or orthogonal to eachother
         *  1: the vectors are going the same direction
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static float Dot(Vec4 a, Vec4 b) {
            return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
        }

        static Vec4 Lerp(Vec4 a, Vec4 b, float t) {
            Vec4 ab = (b - a);
            return a + (ab.scale(t));
        }

        static Vec4 MoveToward(Vec4 a, Vec4 b, float dt) {
            Vec4 ret = Vec4(0);
            ret.x = Math::MoveToward(a.x, b.x, dt);
            ret.y = Math::MoveToward(a.y, b.y, dt);
            ret.z = Math::MoveToward(a.z, b.z, dt);
            ret.w = Math::MoveToward(a.w, b.w, dt);
            
            return ret;
        }

        Vec4 operator+(const Vec4 &right) {
            return Vec4(this->x + right.x, this->y + right.y, this->z + right.z, this->w + right.w);
        }
        Vec4& operator+=(const Vec4 &right) {
            this->x += right.x;
            this->y += right.y;
            this->z += right.z;
            this->w += right.w;

            return *this;
        }

        Vec4 operator-(const Vec4 &right) {
            return Vec4(this->x - right.x, this->y - right.y, this->z - right.z, this->w - right.w);
        }
        Vec4& operator-=(const Vec4 &right) {
            this->x -= right.x;
            this->y -= right.y;
            this->z -= right.z;
            this->w -= right.w;

            return *this;
        }

        Vec4 operator*(const Vec4 &right) {
            return Vec4(this->x * right.x, this->y * right.y, this->z * right.z, this->w * right.w);
        }
        Vec4& operator*=(const Vec4 &right) {
            this->x *= right.x;
            this->y *= right.y;
            this->z *= right.z;
            this->w *= right.w;

            return *this;
        }

        Vec4 operator/(const Vec4 &right) {
            return Vec4(this->x / right.x, this->y / right.y, this->z / right.z, this->w / right.w);
        }
        Vec4& operator/=(const Vec4 &right) {
            this->x /= right.x;
            this->y /= right.y;
            this->z /= right.z;
            this->w /= right.w;

            return *this;
        }

        bool operator==(const Vec4 &right) {
            return NEAR_ZERO(this->x - right.x) && NEAR_ZERO(this->y - right.y) && NEAR_ZERO(this->z - right.z) && NEAR_ZERO(this->w - right.w);
        }
        bool operator!=(const Vec4 &right) {
            return !(*this == right);
        }
    };

    inline Vec3::Vec3(Vec4 v) {
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
    }

    struct IVec4 {
        union {
            struct {
                int x;
                int y;
                int z;
                int w;
            };
        };

        IVec4() {
            this->x = 0.0f;
            this->y = 0.0f;
            this->z = 0.0f;
            this->w = 0.0f;
        }

        IVec4(int fill) {
            this->x = fill;
            this->y = fill;
            this->z = fill;
            this->w = fill;
        }

        IVec4(int x, int y, int z, int w) {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        float magnitude() {
            return sqrtf(SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w));
        }

        float magnitudeSquared() {
            return SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w);
        }

        IVec4 normalize() {
            IVec4 ret(0, 0, 0, 0);
            const float magnitude = this->magnitude();
            if (magnitude == 0) {
                return IVec4(0, 0, 0, 0);
            }

            ret.x = this->x / magnitude;
            ret.y = this->y / magnitude;
            ret.z = this->z / magnitude;
            ret.w = this->w / magnitude;

            return ret;
        }

        IVec4 scale(int scale) const {
            return IVec4(this->x * scale, this->y * scale, this->z * scale, this->w * scale);
        }

        IVec4 scale(IVec4 s) const {
            return IVec4(this->x * s.x, this->y * s.y, this->z * s.z, this->w * s.w);
        }

        IVec4 scale(int scale_x, int scale_y, int scale_z, int scale_w) const {
            return IVec4(this->x * scale_x, this->y * scale_y, this->z * scale_z, this->w * scale_w);
        }

        static float Distance(IVec4 a, IVec4 b) {
            return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y) + SQUARED(b.z - a.z) + SQUARED(b.w - a.w));
        }

        static float DistanceSquared(IVec4 a, IVec4 b) {
            return SQUARED(b.x - a.x) + SQUARED(b.y - a.y) + SQUARED(b.z - a.z) + SQUARED(b.w - a.w);
        }

        static IVec4 Closest(IVec4 a, IVec4 b, IVec4 target = IVec4(0)) {
            float a_distance = IVec4::DistanceSquared(a, target);
            float b_distance = IVec4::DistanceSquared(b, target);

            if (a_distance <= b_distance) {
                return a;
            }

            return b;
        }

        /**
         * @brief NOTE(Jovanni): This only applies if both a and b vectors are normalized:
         * -1: the vectors are 180 degrees from eachother in other words they vectors are pointing in opposite directions
         *  0: the vectors are perpendicular or orthogonal to eachother
         *  1: the vectors are going the same direction
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static float Dot(IVec4 a, IVec4 b) {
            return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
        }

        static IVec4 Lerp(IVec4 a, IVec4 b, float t) {
            IVec4 ab = (b - a);
            return a + (ab.scale(t));
        }

        IVec4 operator+(const IVec4 &right) {
            return IVec4(this->x + right.x, this->y + right.y, this->z + right.z, this->w + right.w);
        }
        IVec4& operator+=(const IVec4 &right) {
            this->x += right.x;
            this->y += right.y;
            this->z += right.z;
            this->w += right.w;

            return *this;
        }

        IVec4 operator-(const IVec4 &right) {
            return IVec4(this->x - right.x, this->y - right.y, this->z - right.z, this->w - right.w);
        }
        IVec4& operator-=(const IVec4 &right) {
            this->x -= right.x;
            this->y -= right.y;
            this->z -= right.z;
            this->w -= right.w;

            return *this;
        }


        IVec4 operator*(const IVec4 &right) {
            return IVec4(this->x * right.x, this->y * right.y, this->z * right.z, this->w * right.w);
        }
        IVec4& operator*=(const IVec4 &right) {
            this->x *= right.x;
            this->y *= right.y;
            this->z *= right.z;
            this->w *= right.w;

            return *this;
        }


        IVec4 operator/(const IVec4 &right) {
            return IVec4(this->x / right.x, this->y / right.y, this->z / right.z, this->w / right.w);
        }
        IVec4& operator/=(const IVec4 &right) {
            this->x /= right.x;
            this->y /= right.y;
            this->z /= right.z;
            this->w /= right.w;

            return *this;
        }

        bool operator==(const IVec4 &right) {
            return NEAR_ZERO(this->x - right.x) && NEAR_ZERO(this->y - right.y) && NEAR_ZERO(this->z - right.z) && NEAR_ZERO(this->w - right.w);
        }
        bool operator!=(const IVec4 &right) {
            return !(*this == right);
        }
    };

    struct AABB {
        Vec3 min;
        Vec3 max;

        static AABB FromCenterExtents(Vec3 center, Vec3 extents) {
            return AABB(center - extents, center + extents);
        }

        static bool Intersection(AABB aabb, Vec3 p0, Vec3 p1) {
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

        AABB() {
            this->min = Vec3(0, 0, 0);
            this->max = Vec3(0, 0, 0);
        }

        AABB(Vec3 min, Vec3 max) {
            this->min = min;
            this->max = max;
        }

        AABB(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) {
            this->min.x = min_x;
            this->min.y = min_y;
            this->min.z = min_z;

            this->max.x = max_x;
            this->max.y = max_y;
            this->max.z = max_z;
        }

        Vec3 getCenter() {
            Vec3 extents = this->getExtents();
            return Vec3(min.x + extents.x, min.y + extents.y, min.z + extents.z);
        }

        Vec3 getExtents() {
            return (max - min).scale(0.5f);
        }
    };

    typedef struct Quat Quat;

    // Matrices are ROW-MAJOR
    struct Mat4 {
        std::array<Vec4, 4> v;

        Mat4() {
            this->v[0] = Vec4(1, 0, 0, 0);
            this->v[1] = Vec4(0, 1, 0, 0);
            this->v[2] = Vec4(0, 0, 1, 0);
            this->v[3] = Vec4(0, 0, 0, 1);
        }

        Mat4(Vec4 r0, Vec4 r1, Vec4 r2, Vec4 r3) {
            this->v = {r0, r1, r2, r3};
        }

        Mat4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
        ) {
            this->v[0] = Vec4(m00, m01, m02, m03);
            this->v[1] = Vec4(m10, m11, m12, m13);
            this->v[2] = Vec4(m20, m21, m22, m23);
            this->v[3] = Vec4(m30, m31, m32, m33);
        }

        Mat4 transpose() {
            Mat4 ret;

            ret.v[0].x = this->v[0].x;
            ret.v[0].y = this->v[1].x;
            ret.v[0].z = this->v[2].x;
            ret.v[0].w = this->v[3].x;

            ret.v[1].x = this->v[0].y;
            ret.v[1].y = this->v[1].y;
            ret.v[1].z = this->v[2].y;
            ret.v[1].w = this->v[3].y;

            ret.v[2].x = this->v[0].z;
            ret.v[2].y = this->v[1].z;
            ret.v[2].z = this->v[2].z;
            ret.v[2].w = this->v[3].z;

            ret.v[3].x = this->v[0].w;
            ret.v[3].y = this->v[1].w;
            ret.v[3].z = this->v[2].w;
            ret.v[3].w = this->v[3].w;      

            return ret;
        }

        static Mat4 Identity() {
            Mat4 ret;
            ret.v = {
                Vec4(1, 0, 0, 0),
                Vec4(0, 1, 0, 0),
                Vec4(0, 0, 1, 0),
                Vec4(0, 0, 0, 1)
            };

            return ret;
        }

        static Mat4 FromColumnMajor(const float mat[16]) {
            Mat4 ret = {
                Vec4{mat[0], mat[4], mat[8], mat[12]},
                Vec4{mat[1], mat[5], mat[9], mat[13]},
                Vec4{mat[2], mat[6], mat[10], mat[14]},
                Vec4{mat[3], mat[7], mat[11], mat[15]},
            };

            return ret;
        }

        static Mat4 Scale(Mat4 mat, float scale) {
            return Mat4::Scale(mat, Vec3(scale, scale, scale));
        }

        static Mat4 Scale(Mat4 mat, Vec3 s) {
            Mat4 scale_matrix;
            scale_matrix.v = {
                Vec4(s.x,  0.0f, 0.0f, 0.0f),
                Vec4(0.0f, s.y,  0.0f, 0.0f),
                Vec4(0.0f, 0.0f, s.z,  0.0f),
                Vec4(0.0f, 0.0f, 0.0f, 1.0f) 
            };

            return scale_matrix * mat;
        }

        static Mat4 Scale(Mat4 mat, float scale_x, float scale_y, float scale_z) {
            return Mat4::Scale(mat, Vec3(scale_x, scale_y, scale_z));
        }

        static Mat4 Rotate(Mat4 mat, float theta, Vec3 axis) {
            float rad = DEGREES_TO_RAD(theta);
            float c = cosf(rad);
            float s = sinf(rad);
            float t = 1.0f - c;

            axis = axis.normalize();
            float x = axis.x;
            float y = axis.y;
            float z = axis.z;

            Mat4 rot;
            rot.v = {
                Vec4(t * x * x + c,     t * x * y - z * s, t * x * z + y * s, 0.0f),
                Vec4(t * x * y + z * s, t * y * y + c,     t * y * z - x * s, 0.0f),
                Vec4(t * x * z - y * s, t * y * z + x * s, t * z * z + c,     0.0f),
                Vec4(0.0f,              0.0f,              0.0f,              1.0f)
            };

            return rot * mat;
        }

        static Mat4 Rotate(Mat4 mat, float theta, float rot_x, float rot_y, float rot_z) {
            return Mat4::Rotate(mat, theta, Vec3(rot_x, rot_y, rot_z));
        }

        static Mat4 Rotate(Mat4 mat, Quat quat);

        static Mat4 Translate(Mat4 mat, Vec3 t) {
            Mat4 translate_matrix;
            translate_matrix.v = {
                Vec4(1.0f, 0.0f, 0.0f, t.x),
                Vec4(0.0f, 1.0f, 0.0f, t.y),
                Vec4(0.0f, 0.0f, 1.0f, t.z),
                Vec4(0.0f, 0.0f, 0.0f, 1.0f)
            };

            return translate_matrix * mat;
        }

        static Mat4 Translate(Mat4 mat, float x, float y, float z) {
            return Mat4::Translate(mat, Vec3(x, y, z));
        }

        static Mat4 Transform(Vec3 s, float theta, Vec3 axis, Vec3 t) {
            Mat4 scale_matrix = Mat4::Scale(Mat4::Identity(), s);
            Mat4 rotation_matrix = Mat4::Rotate(Mat4::Identity(), theta, axis);
            Mat4 translation_matrix = Mat4::Translate(Mat4::Identity(), t);

            return translation_matrix * rotation_matrix * scale_matrix;
        }

        static Mat4 Transform(Vec3 s, Math::Quat r, Vec3 t);

        static Mat4 InverseTransform(Vec3 s, float theta, Vec3 axis, Vec3 t) {
            Mat4 inverse_scale_matrix = Mat4::Scale(Mat4::Identity(), s.scale(1 / s.x, 1 / s.y, 1 / s.z));
            Mat4 inverse_rotation_matrix = Mat4::Rotate(Mat4::Identity(), theta, axis).transpose();
            Mat4 inverse_translation_matrix = Mat4::Translate(Mat4::Identity(), t.scale(-1));

            return inverse_scale_matrix * inverse_rotation_matrix * inverse_translation_matrix;
        }

        static void Decompose(Mat4 mat, Vec3* out_position, Quat* out_orientation, Vec3* out_scale);

        static Mat4 Perspective(float fov_degrees, float aspect, float near_plane, float far_plane) {
            float fov_radians = DEGREES_TO_RAD(fov_degrees);

            const float t = tanf(fov_radians / 2) * near_plane;
            const float b = -t;
            const float r = t * aspect;
            const float l = -t * aspect;

            const float p = (2.0f * near_plane);
            const float A = p / (r - l);
            const float B = p / (t - b);
            const float C = -((far_plane + near_plane) / (far_plane - near_plane));
            const float D = -((p * far_plane) / (far_plane - near_plane));

            Mat4 ret;
            ret.v = {
                Vec4(A,  0,  0,  0),
                Vec4(0,  B,  0,  0),
                Vec4(0,  0,  C,  D),
                Vec4(0,  0, -1,  0)
            };

            return ret;
        }

        // Found at: https://en.wikipedia.org/wiki/Orthographic_projection
        static Mat4 Orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane) {
            const float A = 2.0f / (right - left);
            const float B = 2.0f / (top - bottom);
            const float C = -2.0f / (far_plane - near_plane);
            const float D = -(right + left) / (right - left);
            const float E = -(top + bottom) / (top - bottom);
            const float F = -(far_plane + near_plane) / (far_plane - near_plane);

            Mat4 ret = {
                A,  0,  0,  D,
                0,  B,  0,  E,
                0,  0,  C,  F,
                0,  0,  0,  1
            };

            return ret;
        }

        // Found at: https://www.khronos.org/opengl/wiki/GluLookAt_code
        static Mat4 Lookat(Vec3 position, Vec3 target, Vec3 world_up) {
            Vec3 forward = (position - target).normalize();
            Vec3 right   = Vec3::Cross(world_up, forward).normalize();
            Vec3 up      = Vec3::Cross(forward, right).normalize();

            Mat4 rotation;
            rotation.v = {
                Vec4(right,   0),
                Vec4(up,      0),
                Vec4(forward, 0),
                Vec4(0, 0, 0, 1)
            };
            
            Mat4 translation = Mat4::Translate(Mat4::Identity(), position.scale(-1));

            return rotation * translation;
        }

        float gm_mat3_determinant_helper(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
            return a * (e * i - f * h) -
                    b * (d * i - f * g) +
                    c * (d * h - e * g);
        }

        Mat4 inverse(bool &success) {
            success = false;

            float m00 = this->v[0].x, m01 = this->v[0].y, m02 = this->v[0].z, m03 = this->v[0].w;
            float m10 = this->v[1].x, m11 = this->v[1].y, m12 = this->v[1].z, m13 = this->v[1].w;
            float m20 = this->v[2].x, m21 = this->v[2].y, m22 = this->v[2].z, m23 = this->v[2].w;
            float m30 = this->v[3].x, m31 = this->v[3].y, m32 = this->v[3].z, m33 = this->v[3].w;

            float c00 = gm_mat3_determinant_helper(m11, m12, m13, m21, m22, m23, m31, m32, m33);
            float c01 = gm_mat3_determinant_helper(m10, m12, m13, m20, m22, m23, m30, m32, m33);
            float c02 = gm_mat3_determinant_helper(m10, m11, m13, m20, m21, m23, m30, m31, m33);
            float c03 = gm_mat3_determinant_helper(m10, m11, m12, m20, m21, m22, m30, m31, m32);

            float det = m00 * c00 - m01 * c01 + m02 * c02 - m03 * c03;
            if (NEAR_ZERO(det)) {
                return Mat4::Identity();
            }

            float invDet = 1.0f / det;
            Mat4 inv;

            // Row 0
            inv.v[0].x = invDet * c00;
            inv.v[0].y = invDet * (-gm_mat3_determinant_helper(m01, m02, m03, m21, m22, m23, m31, m32, m33));
            inv.v[0].z = invDet * gm_mat3_determinant_helper(m01, m02, m03, m11, m12, m13, m31, m32, m33);
            inv.v[0].w = invDet * (-gm_mat3_determinant_helper(m01, m02, m03, m11, m12, m13, m21, m22, m23));

            // Row 1
            inv.v[1].x = invDet * (-c01);
            inv.v[1].y = invDet * gm_mat3_determinant_helper(m00, m02, m03, m20, m22, m23, m30, m32, m33);
            inv.v[1].z = invDet * (-gm_mat3_determinant_helper(m00, m02, m03, m10, m12, m13, m30, m32, m33));
            inv.v[1].w = invDet * gm_mat3_determinant_helper(m00, m02, m03, m10, m12, m13, m20, m22, m23);

            // Row 2
            inv.v[2].x = invDet * c02;
            inv.v[2].y = invDet * (-gm_mat3_determinant_helper(m00, m01, m03, m20, m21, m23, m30, m31, m33));
            inv.v[2].z = invDet * gm_mat3_determinant_helper(m00, m01, m03, m10, m11, m13, m30, m31, m33);
            inv.v[2].w = invDet * (-gm_mat3_determinant_helper(m00, m01, m03, m10, m11, m13, m20, m21, m23));

            // Row 3
            inv.v[3].x = invDet * (-c03);
            inv.v[3].y = invDet * gm_mat3_determinant_helper(m00, m01, m02, m20, m21, m22, m30, m31, m32);
            inv.v[3].z = invDet * (-gm_mat3_determinant_helper(m00, m01, m02, m10, m11, m12, m30, m31, m32));
            inv.v[3].w = invDet * gm_mat3_determinant_helper(m00, m01, m02, m10, m11, m12, m20, m21, m22);

            success = true;

            return inv;
        }

        Mat4 operator*(const Mat4 &right) {
            Mat4 C = Mat4(Vec4(0), Vec4(0), Vec4(0), Vec4(0));

            for (int i = 0; i < 4; i++) {
                C.v[i].x += this->v[i].x * right.v[0].x;
                C.v[i].x += this->v[i].y * right.v[1].x;
                C.v[i].x += this->v[i].z * right.v[2].x;
                C.v[i].x += this->v[i].w * right.v[3].x;

                C.v[i].y += this->v[i].x * right.v[0].y;
                C.v[i].y += this->v[i].y * right.v[1].y;
                C.v[i].y += this->v[i].z * right.v[2].y;
                C.v[i].y += this->v[i].w * right.v[3].y;

                C.v[i].z += this->v[i].x * right.v[0].z;
                C.v[i].z += this->v[i].y * right.v[1].z;
                C.v[i].z += this->v[i].z * right.v[2].z;
                C.v[i].z += this->v[i].w * right.v[3].z;
                
                C.v[i].w += this->v[i].x * right.v[0].w;
                C.v[i].w += this->v[i].y * right.v[1].w;
                C.v[i].w += this->v[i].z * right.v[2].w;
                C.v[i].w += this->v[i].w * right.v[3].w;
            }
                
            return C;
        }

        Vec4 operator*(const Vec4 &right) {
            Vec4 ret;
            ret.x += this->v[0].x * right.x;
            ret.x += this->v[0].y * right.y;
            ret.x += this->v[0].z * right.z;
            ret.x += this->v[0].w * right.w;

            ret.y += this->v[1].x * right.x;
            ret.y += this->v[1].y * right.y;
            ret.y += this->v[1].z * right.z;
            ret.y += this->v[1].w * right.w;

            ret.z += this->v[2].x * right.x;
            ret.z += this->v[2].y * right.y;
            ret.z += this->v[2].z * right.z;
            ret.z += this->v[2].w * right.w;
            
            ret.w += this->v[3].x * right.x;
            ret.w += this->v[3].y * right.y;
            ret.w += this->v[3].z * right.z;
            ret.w += this->v[3].w * right.w;
                
            return ret;
        }

        Mat4& operator*=(const Mat4 &right) {
            *this = *this * right;
            return *this;
        }

        bool operator==(const Mat4 &right) {
            return (this->v[0] == right.v[0]) && (this->v[1] == right.v[1]) && (this->v[2] == right.v[2]) && (this->v[3] == right.v[3]);
        }

        bool operator!=(const Mat4 &right) {
            return !(*this == right);
        }

        void print() const {
            LOG_TRACE(
                "---------------------\n"
                "[%.2f %.2f %.2f %.2f]\n"
                "[%.2f %.2f %.2f %.2f]\n"
                "[%.2f %.2f %.2f %.2f]\n"
                "[%.2f %.2f %.2f %.2f]\n"
                "---------------------\n",
                this->v[0].x, this->v[0].y, this->v[0].z, this->v[0].w,
                this->v[1].x, this->v[1].y, this->v[1].z, this->v[1].w,
                this->v[2].x, this->v[2].y, this->v[2].z, this->v[2].w,
                this->v[3].x, this->v[3].y, this->v[3].z, this->v[3].w
            );
        }
    };

    struct Quat {
        float w;
        Vec3 v;

        Quat() {
            this->w = 1;
            this->v = Vec3(0, 0, 0);
        }

        Quat(float theta, Vec3 axis) {
            float radians = DEGREES_TO_RAD(theta);
            this->w = cosf(radians / 2.0f);
            if (NEAR_ZERO(this->w)) {
                this->w = 0.0f;
            }

            axis = axis.normalize();
            this->v = axis.scale(sinf(radians / 2.0f));
        }

        Quat(float theta, float x, float y, float z) {
            *this = Quat(theta, Vec3(x, y, z));
        }

        Quat inverse() {
            Quat ret(1, 0, 0, 0);

            float magnitude_squared = SQUARED(this->w) + Vec3::Dot(this->v, this->v);
            if (magnitude_squared == 0.0f) { 
                return Quat::Identity();
            }

            ret.w = this->w / magnitude_squared;
            ret.v = this->v.scale(-1.0f / magnitude_squared);

            return ret;
        }

        Quat scale(float scale) {
            Quat ret;

            ret.w   = this->w   * scale;
            ret.v.x = this->v.x * scale;
            ret.v.y = this->v.y * scale;
            ret.v.z = this->v.z * scale;

            return ret;
        }

        Quat normalize() {
            Vec4 temp = Vec4(this->w, this->v.x, this->v.y, this->v.z).normalize();
            
            Quat ret;
            ret.w = temp.x;
            ret.v.x = temp.y;
            ret.v.y = temp.z;
            ret.v.z = temp.w;

            return ret;
        }

        static Quat Identity() {
            return {1, 0, 0, 0};
        }

        static Quat Literal(float w, Vec3 axis) {
            Quat ret;
            ret.w = w;
            ret.v = axis;

            return ret;
        }

        static Quat Literal(float w, float x, float y, float z) {
            Quat ret;
            ret.w = w;
            ret.v = Vec3(x, y, z);

            return ret;
        }

        static Quat FromEuler(float theta_x, float theta_y, float theta_z) {
            return FromEuler(Vec3(theta_x, theta_y, theta_z));
        }

        static Quat FromEuler(Vec3 euler_angles_degrees) {
            float roll_rad_half = DEGREES_TO_RAD(euler_angles_degrees.x) * 0.5f;
            float pitch_rad_half = DEGREES_TO_RAD(euler_angles_degrees.y) * 0.5f;
            float yaw_rad_half = DEGREES_TO_RAD(euler_angles_degrees.z) * 0.5f;

            float cx = cosf(roll_rad_half);
            float sx = sinf(roll_rad_half);
            float cy = cosf(pitch_rad_half);
            float sy = sinf(pitch_rad_half);
            float cz = cosf(yaw_rad_half);
            float sz = sinf(yaw_rad_half);

            Quat q = Quat::Identity();
            q.w = cx * cy * cz + sx * sy * sz;
            q.v.x = sx * cy * cz - cx * sy * sz;
            q.v.y = cx * sy * cz + sx * cy * sz;
            q.v.z = cx * cy * sz - sx * sy * cz;

            return q;
        }

        static Quat FromAngleAxis(float angle, Vec3 axis) {
            float half_angle = DEGREES_TO_RAD(angle) * 0.5f;
            float sinf_half = sinf(half_angle);
            float cosf_half = cosf(half_angle);

            Quat q = Quat::Identity();
            axis = axis.normalize();

            q.w     = cosf_half;
            q.v.x   = axis.x * sinf_half;
            q.v.y   = axis.y * sinf_half;
            q.v.z   = axis.z * sinf_half;

            return q;
        }

        static Quat FromRotationMatrix(const float m[16]) {
            Quat q;

            float m00 = m[0],  m01 = m[1],  m02 = m[2];
            float m10 = m[4],  m11 = m[5],  m12 = m[6];
            float m20 = m[8],  m21 = m[9],  m22 = m[10];
            
            float t;
            if (m22 < 0) {
                if (m00 > m11) {
                    t = 1 + m00 - m11 - m22;
                    q.v.x = t;
                    q.v.y = m01 + m10;
                    q.v.z = m02 + m20;
                    q.w = m21 - m12;
                } else {
                    t = 1 - m00 + m11 - m22;
                    q.v.x = m01 + m10;
                    q.v.y = t;
                    q.v.z = m12 + m21;
                    q.w = m02 - m20;
                }
            } else {
                if (m00 < -m11) {
                    t = 1 - m00 - m11 + m22;
                    q.v.x = m02 + m20;
                    q.v.y = m12 + m21;
                    q.v.z = t;
                    q.w = m10 - m01;
                } else {
                    t = 1 + m00 + m11 + m22;
                    q.v.x = m21 - m12;
                    q.v.y = m02 - m20;
                    q.v.z = m10 - m01;
                    q.w = t;
                }
            }
            
            float s = 0.5f / sqrtf(t);
            q.v.x *= s;
            q.v.y *= s;
            q.v.z *= s;
            q.w   *= s;

            return q;
        }

        static Quat FromRotationMatrix(Mat4 mat) {
            return Quat::FromRotationMatrix(&mat.v[0].x);
        }

        static Quat SLerp(Quat q, Quat r, float t) {
            q = q.normalize();
            r = r.normalize();
            float dot = Quat::Dot(q, r);

            if (dot < 0.0f) {
                r = r.scale(-1.0f);
                dot = -dot;
            }

            if (dot > 0.9995f) {
                Quat lerp = q + (r - q).scale(t);
                return lerp.normalize();
            }

            float theta_0 = acosf(dot);
            float theta = theta_0 * t;

            Quat q3 = r - q.scale(dot);
            q3 = q3.normalize();

            Quat term1 = q.scale(cosf(theta));
            Quat term2 = q3.scale(sinf(theta));
            return term1 + term2;
        }

        static float Dot(Quat a, Quat b) {
            float dot = a.w   * b.w   +
                        a.v.x * b.v.x +
                        a.v.y * b.v.y +
                        a.v.z * b.v.z;

            return dot;
        }

        Mat4 getMat4() {
            Mat4 result = Mat4::Identity();

            float x2 = this->v.x * this->v.x;
            float y2 = this->v.y * this->v.y;
            float z2 = this->v.z * this->v.z;

            float xy = this->v.x * this->v.y;
            float xz = this->v.x * this->v.z;
            float yz = this->v.y * this->v.z;
            float xw = this->v.x * this->w;
            float yw = this->v.y * this->w;
            float zw = this->v.z * this->w;

            result.v[0].x = 1.0f - 2.0f * (y2 + z2);  // m00
            result.v[0].y = 2.0f * (xy - zw);         // m01
            result.v[0].z = 2.0f * (xz + yw);         // m02
            result.v[0].w = 0.0f;                     // m03

            result.v[1].x = 2.0f * (xy + zw);         // m10
            result.v[1].y = 1.0f - 2.0f * (x2 + z2);  // m11
            result.v[1].z = 2.0f * (yz - xw);         // m12
            result.v[1].w = 0.0f;                     // m13

            result.v[2].x  = 2.0f * (xz - yw);        // m20
            result.v[2].y  = 2.0f * (yz + xw);        // m21
            result.v[2].z  = 1.0f - 2.0f * (x2 + y2); // m22
            result.v[2].w  = 0.0f;                    // m23

            result.v[3].x = 0.0f;                     // m30
            result.v[3].y = 0.0f;                     // m31
            result.v[3].z = 0.0f;                     // m32
            result.v[3].w = 1.0f;                     // m33

            return result;
        }

        void getAngleAxis(float &theta, Vec3 &vec) {
            Quat quat = this->normalize();
            float sinf_half_theta = quat.v.magnitude();

            if (sinf_half_theta < EPSILON) {
                vec = Vec3(1, 0, 0);
            } else {
                vec = quat.v.scale(1.0f / sinf_half_theta);
            }

            // Clamp w to [-1, 1] to avoid NaNs due to precision issues
            float w = CLAMP(quat.w, -1.0f, 1.0f);
            theta = 2.0f * acosf(w);
            theta = RAD_TO_DEGREES(theta);
        }

        Quat operator+(const Quat &right) {
            Quat ret = Quat::Identity();

            ret.w = this->w + right.w;
            ret.v = this->v + right.v;

            return ret;
        }
        Quat& operator+=(const Quat &right) {
            *this = *this + right;
            return *this;
        }

        Quat operator-(const Quat &right) {
            Quat ret = Quat::Identity();

            ret.w = this->w - right.w;
            ret.v = this->v - right.v;

            return ret;
        }
        Quat& operator-=(const Quat &right) {
            *this = *this - right;
            return *this;
        }

        Quat operator*(const Quat &right) {
            Quat ret = Quat::Identity();
            ret.w = (this->w * right.w) - Vec3::Dot(this->v, right.v);
            ret.v = (this->v.scale(right.w) + right.v.scale(this->w)) + Vec3::Cross(this->v, right.v);
            
            return ret;
        }
        Quat& operator*=(const Quat &right) {
            *this = *this * right;
            return *this;
        }

        Vec3 operator*(const Vec3 &right) {
            Quat q = *this;
            Quat p = Quat::Literal(0, right);

            return ((q * p) * q.inverse()).v;
        }

        bool operator==(const Quat &right) {
            return NEAR_ZERO(this->w - right.w) && (this->v == right.v);
        }
        bool operator!=(const Quat &right) {
            return !(*this == right);
        }
    };

    inline Mat4 Mat4::Rotate(Mat4 mat, Quat quat) {
        float theta;
        Vec3 axis;
        quat.getAngleAxis(theta, axis);
        return Mat4::Rotate(mat, theta, axis);
    }

    inline Mat4 Mat4::Transform(Vec3 s, Math::Quat r, Vec3 t) {
        Mat4 scale_matrix = Mat4::Scale(Mat4::Identity(), s);
        Mat4 rotation_matrix = Mat4::Rotate(Mat4::Identity(), r);
        Mat4 translation_matrix = Mat4::Translate(Mat4::Identity(), t);

        return translation_matrix * rotation_matrix * scale_matrix;
    }

    inline void Mat4::Decompose(Mat4 mat, Vec3* out_position, Quat* out_orientation, Vec3* out_scale) {
        Vec3 translation = Vec3(mat.v[0].w, mat.v[1].w, mat.v[2].w);
        Vec3 scale = Vec3(0);
        {
            Vec3 column1 = Vec3(mat.v[0].x, mat.v[1].x, mat.v[2].x);
            Vec3 column2 = Vec3(mat.v[0].y, mat.v[1].y, mat.v[2].y);
            Vec3 column3 = Vec3(mat.v[0].z, mat.v[1].z, mat.v[2].z);
        
            float scale_x = column1.magnitude();
            float scale_y = column2.magnitude();
            float scale_z = column3.magnitude();
            scale = Vec3(scale_x, scale_y, scale_z);
        }

        Quat orientation = Quat::Identity();
        {
            Vec3 column1 = Vec3(mat.v[0].x, mat.v[1].x, mat.v[2].x);
            Vec3 column2 = Vec3(mat.v[0].y, mat.v[1].y, mat.v[2].y);
            Vec3 column3 = Vec3(mat.v[0].z, mat.v[1].z, mat.v[2].z);
            
            column1 = column1.scale(1.0f / scale.x);
            column2 = column2.scale(1.0f / scale.y);
            column3 = column3.scale(1.0f / scale.z);
            
            Mat4 rotation_matrix = Mat4(
                Vec4{column1.x, column2.x, column3.x, 0},
                Vec4{column1.y, column2.y, column3.y, 0},
                Vec4{column1.z, column2.z, column3.z, 0},
                Vec4{0,         0,         0,         0}
            );
            orientation = Quat::FromRotationMatrix(rotation_matrix);

            /*
            // TRS = M
            // R = T⁻¹ * M * S⁻¹
            // R = T⁻¹ * (TRS) * S⁻¹
            // R = T⁻¹ * TR * I
            // R = I * R * I
            // R = R
            Vec3 inverse_scale = Vec3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
            Mat4 inverse_scale_matrix = Mat4::scale(Mat4::Identity(), inverse_scale);
            Mat4 inverse_translation_matrix = Mat4::Translate(Mat4::Identity(), translation.scale(-1));
            Mat4 rotation_matrix = inverse_translation_matrix * mat * inverse_scale_matrix;
            orientation = Quat::fromRotationMatrix(rotation_matrix);
            */
        }

        if (out_position) {
            *out_position = translation;
        }

        if (out_scale) {
            *out_scale = scale;
        }

        if (out_orientation) {
            *out_orientation = orientation;
        }
    }
}