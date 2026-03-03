#pragma once

namespace Math {
    struct Vec2 {
        float x;
        float y;

        Vec2();
        explicit Vec2(float fill);
        explicit Vec2(float x, float y);

        float magnitude();
        float magnitudeSquared();
        Vec2 normalize();
        Vec2 scale(float scale) const;
        Vec2 scale(Vec2 s) const;
        Vec2 scale(float scale_x, float scale_y) const;

        /**
         * @brief The return value tells you:
         * -1: the vectors are 180 degrees from eachother in other words they vectors are pointing in opposite directions
         *  0: the vectors are perpendicular or orthogonal to eachother
         *  1: the vectors are going the same direction
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static float Dot(Vec2 a, Vec2 b);
        static float Distance(Vec2 a, Vec2 b);
        static float DistanceSquared(Vec2 a, Vec2 b);
        static Vec2 Closest(Vec2 a, Vec2 b, Vec2 target = Vec2(0)); // normally the target is the origin
        static Vec2 Lerp(Vec2 a, Vec2 b, float t);
        static Vec2 MoveToward(Vec2 a, Vec2 b, float dt);
        static Vec2 Euler(float yaw, float pitch);

        Vec2 operator+(const Vec2 &right);
        Vec2& operator+=(const Vec2 &right);

        Vec2 operator-(const Vec2 &right);
        Vec2& operator-=(const Vec2 &right);

        Vec2 operator*(const Vec2 &right);
        Vec2& operator*=(const Vec2 &right);

        Vec2 operator/(const Vec2 &right);
        Vec2& operator/=(const Vec2 &right);

        bool operator==(const Vec2 &right);
        bool operator!=(const Vec2 &right);
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

        Vec3();
        explicit Vec3(float fill);
        explicit Vec3(float x, float y, float z);
        explicit Vec3(Vec2 v, float z);
        explicit Vec3(Vec4 v);

        float magnitude();
        float magnitudeSquared();
        Vec3 normalize();
        Vec3 scale(float scale) const;
        Vec3 scale(Vec3 s) const;
        Vec3 scale(float scale_x, float scale_y, float scale_z) const;

        /**
         * @brief The return value tells you:
         * -1: the vectors are 180 degrees from eachother in other words they vectors are pointing in opposite directions
         *  0: the vectors are perpendicular or orthogonal to eachother
         *  1: the vectors are going the same direction
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static float Dot(Vec3 a, Vec3 b);
        static float Distance(Vec3 a, Vec3 b);
        static float DistanceSquared(Vec3 a, Vec3 b);
        static Vec3 Closest(Vec3 a, Vec3 b, Vec3 target = Vec3(0)); // normally the target is the origin
        static Vec3 Lerp(Vec3 a, Vec3 b, float t);
        static Vec3 MoveToward(Vec3 a, Vec3 b, float dt);
        static Vec3 Cross(Vec3 a, Vec3 b);
        static Vec3 Euler(float yaw, float pitch);

        Vec3 operator+(const Vec3 &right);
        Vec3& operator+=(const Vec3 &right);

        Vec3 operator-(const Vec3 &right);
        Vec3& operator-=(const Vec3 &right);

        Vec3 operator*(const Vec3 &right);
        Vec3& operator*=(const Vec3 &right);

        Vec3 operator/(const Vec3 &right);
        Vec3& operator/=(const Vec3 &right);

        bool operator==(const Vec3 &right);
        bool operator!=(const Vec3 &right);
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

        Vec4();
        explicit Vec4(float fill);
        explicit Vec4(float x, float y, float z, float w);
        explicit Vec4(Vec3 v, float w);

        float magnitude();
        float magnitudeSquared();
        Vec4 normalize();
        Vec4 scale(float scale) const;
        Vec4 scale(Vec4 s) const;
        Vec4 scale(float scale_x, float scale_y, float scale_z, float scale_w) const;

        /**
         * @brief The return value tells you:
         * -1: the vectors are 180 degrees from eachother in other words they vectors are pointing in opposite directions
         *  0: the vectors are perpendicular or orthogonal to eachother
         *  1: the vectors are going the same direction
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static float Dot(Vec4 a, Vec4 b);
        static Vec4 Lerp(Vec4 a, Vec4 b, float t);
        static Vec4 MoveToward(Vec4 a, Vec4 b, float dt);
        static float Distance(Vec4 a, Vec4 b);
        static float DistanceSquared(Vec4 a, Vec4 b);
        static Vec4 Closest(Vec4 a, Vec4 b, Vec4 target = Vec4(0)); // normally the target is the origin

        Vec4 operator+(const Vec4 &right);
        Vec4& operator+=(const Vec4 &right);

        Vec4 operator-(const Vec4 &right);
        Vec4& operator-=(const Vec4 &right);

        Vec4 operator*(const Vec4 &right);
        Vec4& operator*=(const Vec4 &right);

        Vec4 operator/(const Vec4 &right);
        Vec4& operator/=(const Vec4 &right);

        bool operator==(const Vec4 &right);
        bool operator!=(const Vec4 &right);
    };

    struct IVec4 {
        union {
            struct {
                int x;
                int y;
                int z;
                int w;
            };
        };

        IVec4();
        explicit IVec4(int fill);
        explicit IVec4(int x, int y, int z, int w);

        float magnitude();
        float magnitudeSquared();
        IVec4 normalize();
        IVec4 scale(int scale) const;
        IVec4 scale(IVec4 s) const;
        IVec4 scale(int scale_x, int scale_y, int scale_z, int scale_w) const;

        /**
         * @brief The return value tells you:
         * -1: the vectors are 180 degrees from eachother in other words they vectors are pointing in opposite directions
         *  0: the vectors are perpendicular or orthogonal to eachother
         *  1: the vectors are going the same direction
         * 
         * @param a 
         * @param b 
         * @return float 
         */
        static float Dot(IVec4 a, IVec4 b);
        static IVec4 Lerp(IVec4 a, IVec4 b, float t);
        static float Distance(IVec4 a, IVec4 b);
        static float DistanceSquared(IVec4 a, IVec4 b);
        static IVec4 Closest(IVec4 a, IVec4 b, IVec4 target = IVec4(0)); // normally the target is the origin

        IVec4 operator+(const IVec4 &right);
        IVec4& operator+=(const IVec4 &right);

        IVec4 operator-(const IVec4 &right);
        IVec4& operator-=(const IVec4 &right);

        IVec4 operator*(const IVec4 &right);
        IVec4& operator*=(const IVec4 &right);

        IVec4 operator/(const IVec4 &right);
        IVec4& operator/=(const IVec4 &right);

        bool operator==(const IVec4 &right);
        bool operator!=(const IVec4 &right);
    };
};