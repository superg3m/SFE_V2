#pragma once

#include <array>
#include "../Basic/basic.hpp"

namespace Math {
    // Date: May 18, 2025
    // NOTE(Jovanni): Visualize these at: https://easinfgs.net/

    template <typename T>
    inline T average(T* array, int count) {
        T accumlator = 0;
        for (int i = 0; i < count; i++) {
            accumlator += array[i];
        }

        T average = (T)accumlator / (T)count;

        return average;
    }

    inline float lerp(float a, float b, float t) {
        return a + ((b - a) * t);
    }

    inline float inverse_lerp(float a, float b, float value) {
        if (NEAR_ZERO(a - b)) {
            return 0.0f; // Avoid division by zero
        }

        return (value - a) / (b - a);
    }

    inline float remap(float x, float s_min, float s_max, float e_min, float e_max) {
        x = CLAMP(x, s_min, s_max);
        float s_ratio = (x - s_min) / (s_max - s_min);
        
        return e_min + (s_ratio * (e_max - e_min));
    }

    inline float move_toward(float current, float target, float delta) {
        float diff = target - current;

        if (fabsf(diff) <= delta) {
            return target;
        }

        return current + (diff > 0 ? delta : -delta);
    }

    inline int mod(int a, int b) {
        int rem = a % b;
        return (rem < 0) ? (rem + b) : rem;
    }
}

typedef struct Vec4 Vec4;
struct Vec2 {
	float x;
	float y;
	
	Vec2();
	Vec2(float fill);
	Vec2(float x, float y);
	float magnitude() const;
	float magnitude_squared() const;
	Vec2 normalize() const;
	Vec2 scale(float scale) const;
	Vec2 scale(Vec2 s) const;
	Vec2 scale(float scale_x, float scale_y) const;

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
	static float dot(Vec2 a, Vec2 b);
	static float distance(Vec2 a, Vec2 b);
	static float distance_squared(Vec2 a, Vec2 b);
	static Vec2 closest(Vec2 a, Vec2 b, Vec2 target = Vec2(0));
	static Vec2 lerp(Vec2 a, Vec2 b, float t);
	static Vec2 move_toward(Vec2 a, Vec2 b, float dt);
	static Vec2 euler(float yaw, float pitch);

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

struct Vec2;
struct Vec4;

struct Vec3 {
    union {
        struct { float x, y, z; };
        struct { float r, g, b; };
    };

    Vec3();
    Vec3(float fill);
    Vec3(float x, float y, float z);
    Vec3(Vec2 v, float z);
    Vec3(Vec4 v);

    float magnitude();
    float magnitude_squared();
    Vec3 normalize();

    Vec3 scale(float scale) const;
    Vec3 scale(Vec3 s) const;
    Vec3 scale(float scale_x, float scale_y, float scale_z) const;

    static float distance(Vec3 a, Vec3 b);
    static float distance_squared(Vec3 a, Vec3 b);
    static Vec3 closest(Vec3 a, Vec3 b, Vec3 target = Vec3(0));
    static float dot(Vec3 a, Vec3 b);
    static Vec3 lerp(Vec3 a, Vec3 b, float t);
    static Vec3 move_toward(Vec3 a, Vec3 b, float dt);
    static Vec3 cross(Vec3 A, Vec3 B);
    static Vec3 euler(float yaw, float pitch);

    Vec3 operator+(const Vec3 &right) const;
    Vec3& operator+=(const Vec3 &right);
    Vec3 operator-(const Vec3 &right) const;
    Vec3& operator-=(const Vec3 &right);
    Vec3 operator*(const Vec3 &right) const;
    Vec3& operator*=(const Vec3 &right);
    Vec3 operator/(const Vec3 &right) const;
    Vec3& operator/=(const Vec3 &right);
    bool operator==(const Vec3 &right) const;
    bool operator!=(const Vec3 &right) const;
};

struct Vec4 {
    union {
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
    };

    Vec4();
    Vec4(float fill);
    Vec4(float x, float y, float z, float w);
    Vec4(Vec3 v, float w);

    float magnitude();
    float magnitude_squared();
    Vec4 normalize();

    Vec4 scale(float scale) const;
    Vec4 scale(Vec4 s) const;
    Vec4 scale(float scale_x, float scale_y, float scale_z, float scale_w) const;

    static float distance(Vec4 a, Vec4 b);
    static float distance_squared(Vec4 a, Vec4 b);
    static Vec4 closest(Vec4 a, Vec4 b, Vec4 target = Vec4(0));
    static float dot(Vec4 a, Vec4 b);
    static Vec4 lerp(Vec4 a, Vec4 b, float t);
    static Vec4 move_toward(Vec4 a, Vec4 b, float dt);

    Vec4 operator+(const Vec4 &right) const;
    Vec4& operator+=(const Vec4 &right);
    Vec4 operator-(const Vec4 &right) const;
    Vec4& operator-=(const Vec4 &right);
    Vec4 operator*(const Vec4 &right) const;
    Vec4& operator*=(const Vec4 &right);
    Vec4 operator/(const Vec4 &right) const;
    Vec4& operator/=(const Vec4 &right);
    bool operator==(const Vec4 &right) const;
    bool operator!=(const Vec4 &right) const;
};

struct IVec4 {
	union {
        struct { int x, y, z, w; };
        struct { int r, g, b, a; };
    };

    IVec4();
    IVec4(int fill);
    IVec4(int x, int y, int z, int w);

    float magnitude();
    float magnitude_squared();
    IVec4 normalize();

    IVec4 scale(int scale) const;
    IVec4 scale(IVec4 s) const;
    IVec4 scale(int scale_x, int scale_y, int scale_z, int scale_w) const;

    static float distance(IVec4 a, IVec4 b);
    static float distance_squared(IVec4 a, IVec4 b);
    static IVec4 closest(IVec4 a, IVec4 b, IVec4 target = IVec4(0));
    static float dot(IVec4 a, IVec4 b);
    static IVec4 lerp(IVec4 a, IVec4 b, float t);
    static IVec4 move_toward(IVec4 a, IVec4 b, float dt);

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

struct AABB {
    Vec3 min;
    Vec3 max;

    AABB();
    AABB(Vec3 min, Vec3 max);
    AABB(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);

    static AABB from_center_extents(Vec3 center, Vec3 extents);
    static bool line_intersection(AABB aabb, Vec3 p0, Vec3 p1);  // p1 is treated as direction (p0 + t*p1)

    Vec3 center() const;
    Vec3 extents() const;
};

typedef struct Quat Quat;

// Matrices are ROW-MAJOR
struct Mat4 {
    std::array<Vec4, 4> v;

    Mat4();
	Mat4(float f);
    Mat4(Vec4 r0, Vec4 r1, Vec4 r2, Vec4 r3);
    Mat4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    );

    Mat4 transpose() const;

    static Mat4 identity();
    static Mat4 from_column_major(const float mat[16]);

    static Mat4 scale(Mat4 mat, float scale);
    static Mat4 scale(Mat4 mat, Vec3 s);
    static Mat4 scale(Mat4 mat, float sx, float sy, float sz);

    static Mat4 rotate(Mat4 mat, float theta, Vec3 axis);
    static Mat4 rotate(Mat4 mat, float theta, float x, float y, float z);
    static Mat4 rotate(Mat4 mat, Quat quat);

    static Mat4 translate(Mat4 mat, Vec3 t);
    static Mat4 translate(Mat4 mat, float x, float y, float z);

    static Mat4 transform(Vec3 s, float theta, Vec3 axis, Vec3 t);
    static Mat4 transform(Vec3 s, Quat r, Vec3 t);
    static Mat4 inverse_transform(Vec3 s, float theta, Vec3 axis, Vec3 t);

    static void decompose(Mat4 mat, Vec3* out_pos, Quat* out_rot, Vec3* out_scale);

    static Mat4 perspective(float fov_deg, float aspect, float near_plane, float far_plane);
    static Mat4 orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane);
    static Mat4 lookat(Vec3 position, Vec3 target, Vec3 up);

    Mat4 inverse(bool& success) const;

    Mat4 operator*(const Mat4& right) const;
    Vec4 operator*(const Vec4& right) const;
    Mat4& operator*=(const Mat4& right);
    bool operator==(const Mat4& right) const;
    bool operator!=(const Mat4& right) const;

    void print() const;
};

struct Quat {
	float w;
	Vec3 v;

	Quat();
	Quat(float w, Vec3 axis);
	Quat(float w, float x, float y, float z);
	Quat inverse();
	Quat scale(float scale);
	Quat normalize();
	Mat4 get_rotation_matrix();
	void angle_axis(float &theta, Vec3 &vec);

	static Quat identity();
	static Quat literal(float w, Vec3 axis);
	static Quat literal(float w, float x, float y, float z);
	static Quat from_euler(float theta_x, float theta_y, float theta_z);
	static Quat from_euler(Vec3 euler_angles_degrees);
	static Quat from_angle_axis(float angle, Vec3 axis);
	static Quat from_rotation_matrix(const float m[16]);
	static Quat from_rotation_matrix(Mat4 mat);
	static Quat slerp(Quat q, Quat r, float t);
	static float dot(Quat a, Quat b);

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

inline Mat4 Mat4::transform(Vec3 s, Quat r, Vec3 t) {
	Mat4 scale_matrix = Mat4::scale(Mat4::identity(), s);
	Mat4 rotation_matrix = Mat4::rotate(Mat4::identity(), r);
	Mat4 translation_matrix = Mat4::translate(Mat4::identity(), t);

	return translation_matrix * rotation_matrix * scale_matrix;
}

inline void Mat4::decompose(Mat4 mat, Vec3* out_position, Quat* out_orientation, Vec3* out_scale) {
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

	Quat orientation = Quat::identity();
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
		orientation = Quat::from_rotation_matrix(rotation_matrix);

		/*
		// TRS = M
		// R = T⁻¹ * M * S⁻¹
		// R = T⁻¹ * (TRS) * S⁻¹
		// R = T⁻¹ * TR * I
		// R = I * R * I
		// R = R
		Vec3 inverse_scale = Vec3(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
		Mat4 inverse_scale_matrix = Mat4::scale(Mat4::identity(), inverse_scale);
		Mat4 inverse_translation_matrix = Mat4::translate(Mat4::identity(), translation.scale(-1));
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