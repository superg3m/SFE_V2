#pragma once

#include <array>

#include "sfe_vector.hpp"

namespace Math {
    typedef struct Quat Quat;

    // Matrices are ROW-MAJOR
    struct Mat4 {
        std::array<Vec4, 4> v;

        Mat4();
        Mat4(Vec4 r0, Vec4 r1, Vec4 r2, Vec4 r3);
        Mat4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33
        );

        Mat4 transpose();
        Mat4 inverse(bool &success);

        static Mat4 Identity();
        static Mat4 FromColumnMajor(const float mat[16]);
        static Mat4 Scale(Mat4 mat, float scale);
        static Mat4 Scale(Mat4 mat, Vec3 s);
        static Mat4 Scale(Mat4 mat, float scale_x, float scale_y, float scale_z);
        static Mat4 Rotate(Mat4 mat, float theta, Vec3 axis);
        static Mat4 Rotate(Mat4 mat, float theta, float rot_x, float rot_y, float rot_z);
        static Mat4 Rotate(Mat4 mat, Quat quat);
        static Mat4 Translate(Mat4 mat, Vec3 t);
        static Mat4 Translate(Mat4 mat, float x, float y, float z);
        static Mat4 Transform(Vec3 s, float theta, Vec3 axis, Vec3 t);
        static Mat4 Transform(Vec3 s, Math::Quat r, Vec3 t);
        static Mat4 InverseTransform(Vec3 s, float theta, Vec3 axis, Vec3 t);
        static void Decompose(Mat4 mat, Vec3* out_position, Quat* out_orientation, Vec3* out_scale);
        
        static Mat4 Perspective(float fov_degrees, float aspect, float near_plane, float far_plane);
        static Mat4 Orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane);
        static Mat4 Lookat(Vec3 position, Vec3 target, Vec3 world_up);

        Vec4 operator*(const Vec4 &right);
        Mat4 operator*(const Mat4 &right);
        Mat4& operator*=(const Mat4 &right);

        bool operator==(const Mat4 &right);
        bool operator!=(const Mat4 &right);

        void print() const;
    };
}