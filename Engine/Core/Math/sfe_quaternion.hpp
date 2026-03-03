#pragma once

#include "sfe_vector.hpp"
#include "sfe_matrix.hpp"

namespace Math {
    struct Quat {
        float w;
        Vec3 v;

        Quat();
        Quat(float theta, Vec3 axis);
        Quat(float theta, float x, float y, float z);

        Quat inverse();
        Quat normalize();
        Quat scale(float scale);
        Mat4 getMat4();
        void getAngleAxis(float &theta, Vec3 &vec);

        static Quat Identity();
        static Quat Literal(float w, Vec3 axis);
        static Quat Literal(float w, float x, float y, float z);
        static Quat FromEuler(Vec3 euler_angles_degrees);
        static Quat FromEuler(float theta_x, float theta_y, float theta_z);
        static Quat FromAngleAxis(float w, Vec3 axis);
        static Quat FromRotationMatrix(const float m[16]);
        static Quat FromRotationMatrix(Mat4 mat);
        static Quat SLerp(Quat q, Quat r, float t);
        static float Dot(Quat a, Quat b);

        Quat operator+(const Quat &right);
        Quat& operator+=(const Quat &right);
        
        Quat operator-(const Quat &right);
        Quat& operator-=(const Quat &right);

        Quat operator*(const Quat &right);
        Quat& operator*=(const Quat &right);

        Vec3 operator*(const Vec3 &right);

        bool operator==(const Quat &right);
        bool operator!=(const Quat &right);
    };
} 


