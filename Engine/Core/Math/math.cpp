#include "math.hpp"

Vec2::Vec2() {
	this->x = 0.0f;
	this->y = 0.0f;
}

Vec2::Vec2(float fill) {
	this->x = fill;
	this->y = fill;
}

Vec2::Vec2(float x, float y) {
	this->x = x;
	this->y = y;
}

float Vec2::magnitude() const {
	return sqrtf((this->x * this->x) + (this->y * this->y));
}

float Vec2::magnitude_squared() const {
	return (this->x * this->x) + (this->y * this->y);
}

Vec2 Vec2::normalize() const{
	Vec2 ret(0, 0);
	const float magnitude = this->magnitude();
	if (magnitude == 0) {
		return Vec2(0, 0);
	}

	ret.x = this->x / magnitude;
	ret.y = this->y / magnitude;

	return ret;
}

Vec2 Vec2::scale(float scale) const {
	return Vec2(this->x * scale, this->y * scale);
}

Vec2 Vec2::scale(Vec2 s) const {
	return Vec2(this->x * s.x, this->y * s.y);
}

Vec2 Vec2::scale(float scale_x, float scale_y) const {
	return Vec2(this->x * scale_x, this->y * scale_y);
}

float Vec2::dot(Vec2 a, Vec2 b) {
	return (a.x * b.x) + (a.y * b.y);
}

float Vec2::distance(Vec2 a, Vec2 b) {
	return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y));
}

float Vec2::distance_squared(Vec2 a, Vec2 b) {
	return SQUARED(b.x - a.x) + SQUARED(b.y - a.y);
}

Vec2 Vec2::closest(Vec2 a, Vec2 b, Vec2 target) {
	float a_distance = Vec2::distance_squared(a, target);
	float b_distance = Vec2::distance_squared(b, target);

	if (a_distance <= b_distance) {
		return a;
	}

	return b;
}

Vec2 Vec2::lerp(Vec2 a, Vec2 b, float t) {
	Vec2 ab = (b - a);
	return a + (ab.scale(t));
}

Vec2 Vec2::move_toward(Vec2 a, Vec2 b, float dt) {
	Vec2 ret = Vec2(0);
	ret.x = Math::move_toward(a.x, b.x, dt);
	ret.y = Math::move_toward(a.y, b.y, dt);
	
	return ret;
}

Vec2 Vec2::euler(float yaw, float pitch) {
	Vec2 ret = Vec2(0, 0);
	ret.x = cosf(DEGREES_TO_RAD(yaw)) * cosf(DEGREES_TO_RAD(pitch));
	ret.y = sinf(DEGREES_TO_RAD(pitch));

	return ret;
}

Vec2 Vec2::operator+(const Vec2 &right) {
	return Vec2(this->x + right.x, this->y + right.y);
}
Vec2& Vec2::operator+=(const Vec2 &right) {
	this->x += right.x;
	this->y += right.y;

	return *this;
}

Vec2 Vec2::operator-(const Vec2 &right) {
	return Vec2(this->x - right.x, this->y - right.y);
}
Vec2& Vec2::operator-=(const Vec2 &right) {
	this->x -= right.x;
	this->y -= right.y;

	return *this;
}

Vec2 Vec2::operator*(const Vec2 &right) {
	return Vec2(this->x * right.x, this->y * right.y);
}

Vec2& Vec2::operator*=(const Vec2 &right) {
	this->x *= right.x;
	this->y *= right.y;

	return *this;
}

Vec2 Vec2::operator/(const Vec2 &right) {
	return Vec2(this->x / right.x, this->y / right.y);
}
Vec2& Vec2::operator/=(const Vec2 &right) {
	this->x /= right.x;
	this->y /= right.y;

	return *this;
}

bool Vec2::operator==(const Vec2 &right) {
	return NEAR_ZERO(this->x - right.x) && NEAR_ZERO(this->y - right.y);
}

bool Vec2::operator!=(const Vec2 &right) {
	return !(*this == right);
}

// --

Vec3::Vec3() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}

Vec3::Vec3(float fill) {
	this->x = fill;
	this->y = fill;
	this->z = fill;
}

Vec3::Vec3(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Vec3::Vec3(Vec2 v, float z) {
	this->x = v.x;
	this->y = v.y;
	this->z = z;
}

Vec3::Vec3(Vec4 v) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

float Vec3::magnitude() {
	return sqrtf(SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z));
}

float Vec3::magnitude_squared() {
	return SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z);
}

Vec3 Vec3::normalize() {
	Vec3 ret(0, 0, 0);
	float m = magnitude();
	if (m == 0) return Vec3(0,0,0);

	ret.x = this->x / m;
	ret.y = this->y / m;
	ret.z = this->z / m;
	return ret;
}

Vec3 Vec3::scale(float s) const {
	return Vec3(this->x * s, this->y * s, this->z * s);
}

Vec3 Vec3::scale(Vec3 s) const {
	return Vec3(this->x * s.x, this->y * s.y, this->z * s.z);
}

Vec3 Vec3::scale(float sx, float sy, float sz) const {
	return Vec3(this->x * sx, this->y * sy, this->z * sz);
}

float Vec3::distance(Vec3 a, Vec3 b) {
	return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y) + SQUARED(b.z - a.z));
}

float Vec3::distance_squared(Vec3 a, Vec3 b) {
	return SQUARED(b.x - a.x) + SQUARED(b.y - a.y) + SQUARED(b.z - a.z);
}

Vec3 Vec3::closest(Vec3 a, Vec3 b, Vec3 target) {
	float ad = distance_squared(a, target);
	float bd = distance_squared(b, target);
	return (ad <= bd) ? a : b;
}

float Vec3::dot(Vec3 a, Vec3 b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 Vec3::lerp(Vec3 a, Vec3 b, float t) {
	Vec3 ab = (b - a);
	return a + ab.scale(t);
}

Vec3 Vec3::move_toward(Vec3 a, Vec3 b, float dt) {
	Vec3 r(0);
	r.x = Math::move_toward(a.x, b.x, dt);
	r.y = Math::move_toward(a.y, b.y, dt);
	r.z = Math::move_toward(a.z, b.z, dt);
	return r;
}

Vec3 Vec3::cross(Vec3 A, Vec3 B) {
	return Vec3(
		A.y * B.z - A.z * B.y,
		A.z * B.x - A.x * B.z,
		A.x * B.y - A.y * B.x
	);
}

Vec3 Vec3::euler(float yaw, float pitch) {
	Vec3 r(0,0,0);
	r.x = cosf(DEGREES_TO_RAD(yaw)) * cosf(DEGREES_TO_RAD(pitch));
	r.y = sinf(DEGREES_TO_RAD(pitch));
	r.z = sinf(DEGREES_TO_RAD(yaw)) * cosf(DEGREES_TO_RAD(pitch));
	return r;
}

Vec3 Vec3::operator+(const Vec3 &r) const {
	return Vec3(this->x + r.x, this->y + r.y, this->z + r.z);
}

Vec3& Vec3::operator+=(const Vec3 &r) {
	this->x += r.x; 
	this->y += r.y; 
	this->z += r.z;

	return *this;
}

Vec3 Vec3::operator-(const Vec3 &r) const {
	return Vec3(this->x - r.x, this->y - r.y, this->z - r.z);
}

Vec3& Vec3::operator-=(const Vec3 &r) {
	this->x -= r.x; 
	this->y -= r.y;
	this->z -= r.z;

	return *this;
}

Vec3 Vec3::operator*(const Vec3 &r) const {
	return Vec3(this->x * r.x, this->y * r.y, this->z * r.z);
}

Vec3& Vec3::operator*=(const Vec3 &r) {
	x *= r.x; 
	y *= r.y; 
	z *= r.z;

	return *this;
}

Vec3 Vec3::operator/(const Vec3 &r) const {
	return Vec3(x/r.x, y/r.y, z/r.z);
}

Vec3& Vec3::operator/=(const Vec3 &r) {
	this->x /= r.x;
	this->y /= r.y; 
	this->z /= r.z;
	return *this;
}

bool Vec3::operator==(const Vec3 &r) const {
	return NEAR_ZERO(this->x - r.x) && NEAR_ZERO(this->y - r.y) && NEAR_ZERO(this->z - r.z);
}

bool Vec3::operator!=(const Vec3 &r) const {
	return !(*this == r);
}

// --

Vec4::Vec4() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->w = 0.0f;
}

Vec4::Vec4(float fill) {
	this->x = fill;
	this->y = fill;
	this->z = fill;
	this->w = fill;
}

Vec4::Vec4(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vec4::Vec4(Vec3 v, float w) {
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = w;
}

float Vec4::magnitude() {
	return sqrtf(SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w));
}

float Vec4::magnitude_squared() {
	return SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w);
}

Vec4 Vec4::normalize() {
	float m = magnitude();
	if (m == 0) return Vec4(0);

	return Vec4(this->x / m, this->y / m, this->z / m, this->w / m);
}

Vec4 Vec4::scale(float s) const {
	return Vec4(this->x * s, this->y * s, this->z * s, this->w * s);
}

Vec4 Vec4::scale(Vec4 s) const {
	return Vec4(this->x * s.x, this->y * s.y, this->z * s.z, this->w * s.w);
}

Vec4 Vec4::scale(float sx, float sy, float sz, float sw) const {
	return Vec4(this->x * sx, this->y * sy, this->z * sz, this->w * sw);
}

float Vec4::distance(Vec4 a, Vec4 b) {
	return sqrtf(
		SQUARED(b.x - a.x) +
		SQUARED(b.y - a.y) +
		SQUARED(b.z - a.z) +
		SQUARED(b.w - a.w)
	);
}

float Vec4::distance_squared(Vec4 a, Vec4 b) {
	return
		SQUARED(b.x - a.x) +
		SQUARED(b.y - a.y) +
		SQUARED(b.z - a.z) +
		SQUARED(b.w - a.w);
}

Vec4 Vec4::closest(Vec4 a, Vec4 b, Vec4 target) {
	float ad = distance_squared(a, target);
	float bd = distance_squared(b, target);
	return (ad <= bd) ? a : b;
}

float Vec4::dot(Vec4 a, Vec4 b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

Vec4 Vec4::lerp(Vec4 a, Vec4 b, float t) {
	Vec4 ab = (b - a);
	return a + ab.scale(t);
}

Vec4 Vec4::move_toward(Vec4 a, Vec4 b, float dt) {
	Vec4 r = Vec4(0);
	r.x = Math::move_toward(a.x, b.x, dt);
	r.y = Math::move_toward(a.y, b.y, dt);
	r.z = Math::move_toward(a.z, b.z, dt);
	r.w = Math::move_toward(a.w, b.w, dt);

	return r;
}

Vec4 Vec4::operator+(const Vec4 &r) const {
	return Vec4(this->x + r.x, this->y + r.y, this->z + r.z, this->w + r.w);
}

Vec4& Vec4::operator+=(const Vec4 &r) {
	this->x += r.x; 
	this->y += r.y; 
	this->z += r.z;
	this->w += r.w;

	return *this;
}

Vec4 Vec4::operator-(const Vec4 &r) const {
	return Vec4(this->x - r.x, this->y - r.y, this->z - r.z, this->w - r.w);
}

Vec4& Vec4::operator-=(const Vec4 &r) {
	this->x -= r.x; 
	this->y -= r.y; 
	this->z -= r.z; 
	this->w -= r.w;

	return *this;
}

Vec4 Vec4::operator*(const Vec4 &r) const {
	return Vec4(this->x * r.x, this->y * r.y, this->z * r.z, this->w * r.w);
}

Vec4& Vec4::operator*=(const Vec4 &r) {
	this->x *= r.x; 
	this->y *= r.y; 
	this->z *= r.z; 
	this->w *= r.w;

	return *this;
}

Vec4 Vec4::operator/(const Vec4 &r) const {
	return Vec4(this->x / r.x, this->y / r.y, this->z / r.z, this->w / r.w);
}

Vec4& Vec4::operator/=(const Vec4 &r) {
	this->x /= r.x; 
	this->y /= r.y;
	this->z /= r.z;
	this->w /= r.w;

	return *this;
}

bool Vec4::operator==(const Vec4 &r) const {
	return NEAR_ZERO(x-r.x) &&
		   NEAR_ZERO(y-r.y) &&
		   NEAR_ZERO(z-r.z) &&
		   NEAR_ZERO(w-r.w);
}

bool Vec4::operator!=(const Vec4 &r) const {
	return !(*this == r);
}

// -- 

IVec4::IVec4() {
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
	this->w = 0.0f;
}

IVec4::IVec4(int fill) {
	this->x = fill;
	this->y = fill;
	this->z = fill;
	this->w = fill;
}

IVec4::IVec4(int x, int y, int z, int w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

float IVec4::magnitude() {
	return sqrtf(SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w));
}

float IVec4::magnitude_squared() {
	return SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w);
}

IVec4 IVec4::normalize() {
	float m = magnitude();
	if (m == 0) return IVec4(0);

	return IVec4(this->x / m, this->y / m, this->z / m, this->w / m);
}

IVec4 IVec4::scale(int s) const {
	return IVec4(this->x * s, this->y * s, this->z * s, this->w * s);
}

IVec4 IVec4::scale(IVec4 s) const {
	return IVec4(this->x * s.x, this->y * s.y, this->z * s.z, this->w * s.w);
}

IVec4 IVec4::scale(int sx, int sy, int sz, int sw) const {
	return IVec4(this->x * sx, this->y * sy, this->z * sz, this->w * sw);
}

float IVec4::distance(IVec4 a, IVec4 b) {
	return sqrtf(
		SQUARED(b.x - a.x) +
		SQUARED(b.y - a.y) +
		SQUARED(b.z - a.z) +
		SQUARED(b.w - a.w)
	);
}

float IVec4::distance_squared(IVec4 a, IVec4 b) {
	return
		SQUARED(b.x - a.x) +
		SQUARED(b.y - a.y) +
		SQUARED(b.z - a.z) +
		SQUARED(b.w - a.w);
}

IVec4 IVec4::closest(IVec4 a, IVec4 b, IVec4 target) {
	float ad = distance_squared(a, target);
	float bd = distance_squared(b, target);
	return (ad <= bd) ? a : b;
}

float IVec4::dot(IVec4 a, IVec4 b) {
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

IVec4 IVec4::lerp(IVec4 a, IVec4 b, float t) {
	IVec4 ab = (b - a);
	return a + ab.scale(t);
}

IVec4 IVec4::move_toward(IVec4 a, IVec4 b, float dt) {
	IVec4 r = IVec4(0);
	r.x = Math::move_toward(a.x, b.x, dt);
	r.y = Math::move_toward(a.y, b.y, dt);
	r.z = Math::move_toward(a.z, b.z, dt);
	r.w = Math::move_toward(a.w, b.w, dt);

	return r;
}

IVec4 IVec4::operator+(const IVec4 &r) {
	return IVec4(this->x + r.x, this->y + r.y, this->z + r.z, this->w + r.w);
}

IVec4& IVec4::operator+=(const IVec4 &r) {
	this->x += r.x; 
	this->y += r.y; 
	this->z += r.z;
	this->w += r.w;

	return *this;
}

IVec4 IVec4::operator-(const IVec4 &r) {
	return IVec4(this->x - r.x, this->y - r.y, this->z - r.z, this->w - r.w);
}

IVec4& IVec4::operator-=(const IVec4 &r) {
	this->x -= r.x; 
	this->y -= r.y; 
	this->z -= r.z; 
	this->w -= r.w;

	return *this;
}

IVec4 IVec4::operator*(const IVec4 &r) {
	return IVec4(this->x * r.x, this->y * r.y, this->z * r.z, this->w * r.w);
}

IVec4& IVec4::operator*=(const IVec4 &r) {
	this->x *= r.x; 
	this->y *= r.y; 
	this->z *= r.z; 
	this->w *= r.w;

	return *this;
}

IVec4 IVec4::operator/(const IVec4 &r) {
	return IVec4(this->x / r.x, this->y / r.y, this->z / r.z, this->w / r.w);
}

IVec4& IVec4::operator/=(const IVec4 &r) {
	this->x /= r.x; 
	this->y /= r.y;
	this->z /= r.z;
	this->w /= r.w;

	return *this;
}

bool IVec4::operator==(const IVec4 &r) {
	return NEAR_ZERO(x-r.x) &&
		   NEAR_ZERO(y-r.y) &&
		   NEAR_ZERO(z-r.z) &&
		   NEAR_ZERO(w-r.w);
}

bool IVec4::operator!=(const IVec4 &r) {
	return !(*this == r);
}

// -- 

AABB::AABB() : min(0.0f, 0.0f, 0.0f), max(0.0f, 0.0f, 0.0f) {}

AABB::AABB(Vec3 min, Vec3 max) : min(min), max(max) {}

AABB::AABB(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) {
	min.x = min_x;
	min.y = min_y;
	min.z = min_z;

	max.x = max_x;
	max.y = max_y;
	max.z = max_z;
}

AABB AABB::from_center_extents(Vec3 center, Vec3 extents) {
	return AABB(center - extents, center + extents);
}

bool AABB::line_intersection(AABB aabb, Vec3 p0, Vec3 p1) {
	float tmin = -10000.0f;
	float tmax =  10000.0f;

	if (fabsf(p1.x) > EPSILON) {
		float t1 = (aabb.min.x - p0.x) / p1.x;
		float t2 = (aabb.max.x - p0.x) / p1.x;

		tmin = MAX(tmin, MIN(t1, t2));
		tmax = MIN(tmax, MAX(t1, t2));
	}

	if (fabsf(p1.y) > EPSILON) {
		float t1 = (aabb.min.y - p0.y) / p1.y;
		float t2 = (aabb.max.y - p0.y) / p1.y;

		tmin = MAX(tmin, MIN(t1, t2));
		tmax = MIN(tmax, MAX(t1, t2));
	}

	if (fabsf(p1.z) > EPSILON) {
		float t1 = (aabb.min.z - p0.z) / p1.z;
		float t2 = (aabb.max.z - p0.z) / p1.z;

		tmin = MAX(tmin, MIN(t1, t2));
		tmax = MIN(tmax, MAX(t1, t2));
	}

	return (tmax > tmin && tmax > 0.0f);
}

Mat4 AABB::to_transform_matrix4() {
	Vec3 scale = this->max - this->min;
	Vec3 center = this->center();
	Mat4 transform = Mat4::identity();
	transform = Mat4::scale(transform, scale);
	transform = Mat4::translate(transform, center);

	return transform;
}

Vec3 AABB::center() const {
	Vec3 e = extents();
	return Vec3(min.x + e.x, min.y + e.y, min.z + e.z);
}

Vec3 AABB::extents() const {
	return (max - min).scale(0.5f);
}

// -- 

Mat4::Mat4() {
	v = {
		Vec4(1,0,0,0),
		Vec4(0,1,0,0),
		Vec4(0,0,1,0),
		Vec4(0,0,0,1)
	};
}

Mat4::Mat4(float f) {
	v = {
		Vec4(f,0,0,0),
		Vec4(0,f,0,0),
		Vec4(0,0,f,0),
		Vec4(0,0,0,f)
	};
}

Mat4::Mat4(Vec4 r0, Vec4 r1, Vec4 r2, Vec4 r3) {
	v = {r0, r1, r2, r3};
}

Mat4::Mat4(
	float m00,float m01,float m02,float m03,
	float m10,float m11,float m12,float m13,
	float m20,float m21,float m22,float m23,
	float m30,float m31,float m32,float m33
) {
	v[0] = Vec4(m00, m01, m02, m03);
	v[1] = Vec4(m10, m11, m12, m13);
	v[2] = Vec4(m20, m21, m22, m23);
	v[3] = Vec4(m30, m31, m32, m33);
}

// --- Basic ---

Mat4 Mat4::transpose() const {
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

Mat4 Mat4::identity() { return Mat4(); }

Mat4 Mat4::from_column_major(const float m[16]) {
	return Mat4(
		m[0],m[4],m[8], m[12],
		m[1],m[5],m[9], m[13],
		m[2],m[6],m[10],m[14],
		m[3],m[7],m[11],m[15]
	);
}

// --- Transform helpers ---

Mat4 Mat4::scale(Mat4 mat, float s) {
	return scale(mat, Vec3(s));
}

Mat4 Mat4::scale(Mat4 mat, Vec3 s) {
	Mat4 Scale(
		s.x,0,0,0,
		0,s.y,0,0,
		0,0,s.z,0,
		0,0,0,1
	);
	return Scale * mat;
}

Mat4 Mat4::scale(Mat4 mat, float sx,float sy,float sz){
	return scale(mat, Vec3(sx,sy,sz));
}

Mat4 Mat4::rotate(Mat4 mat, float theta, Vec3 axis) {
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

Mat4 Mat4::rotate(Mat4 mat, Quat quat) {
	float theta;
	Vec3 axis;
	quat.to_angle_axis(theta, axis);
	return Mat4::rotate(mat, theta, axis);
}

Mat4 Mat4::rotate(Mat4 mat, float theta, float x,float y,float z){
	return rotate(mat, theta, Vec3(x,y,z));
}

Mat4 Mat4::translate(Mat4 mat, Vec3 t){
	Mat4 T(
		1,0,0,t.x,
		0,1,0,t.y,
		0,0,1,t.z,
		0,0,0,1
	);
	return T * mat;
}

Mat4 Mat4::translate(Mat4 mat,float x,float y,float z){
	return translate(mat, Vec3(x,y,z));
}

Mat4 Mat4::transform(Vec3 s,float theta,Vec3 axis,Vec3 t){
	return translate(identity(),t) *
		   rotate(identity(),theta,axis) *
		   scale(identity(),s);
}

Mat4 Mat4::inverse_transform(Vec3 s,float theta,Vec3 axis,Vec3 t){
	return scale(identity(), Vec3(1/s.x,1/s.y,1/s.z)) *
		   rotate(identity(),theta,axis).transpose() *
		   translate(identity(), t.scale(-1));
}

// --- Projection ---

Mat4 Mat4::perspective(float fov,float aspect,float n,float f){
	float r = DEGREES_TO_RAD(fov);
	float t = tanf(r/2)*n;

	float A = n/(t*aspect);
	float B = n/t;
	float C = -(f+n)/(f-n);
	float D = -(2*f*n)/(f-n);

	return Mat4(
		A,0,0,0,
		0,B,0,0,
		0,0,C,D,
		0,0,-1,0
	);
}

Mat4 Mat4::orthographic(float l,float r,float b,float t,float n,float f){
	return Mat4(
		2/(r-l),0,0,-(r+l)/(r-l),
		0,2/(t-b),0,-(t+b)/(t-b),
		0,0,-2/(f-n),-(f+n)/(f-n),
		0,0,0,1
	);
}

Mat4 Mat4::lookat(Vec3 pos,Vec3 target,Vec3 up){
	Vec3 f = (pos-target).normalize();
	Vec3 r = Vec3::cross(up,f).normalize();
	Vec3 u = Vec3::cross(f,r);

	Mat4 R(
		r.x,r.y,r.z,0,
		u.x,u.y,u.z,0,
		f.x,f.y,f.z,0,
		0,0,0,1
	);

	return R * translate(identity(), pos.scale(-1));
}

// --- Inverse ---

#define DETERMINATE_HELPER(a, b, c, d, e, f, g, h, i) (a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g))
Mat4 Mat4::inverse(bool& success) const {
	success = false;

	float m00 = this->v[0].x, m01 = this->v[0].y, m02 = this->v[0].z, m03 = this->v[0].w;
	float m10 = this->v[1].x, m11 = this->v[1].y, m12 = this->v[1].z, m13 = this->v[1].w;
	float m20 = this->v[2].x, m21 = this->v[2].y, m22 = this->v[2].z, m23 = this->v[2].w;
	float m30 = this->v[3].x, m31 = this->v[3].y, m32 = this->v[3].z, m33 = this->v[3].w;

	float c00 = DETERMINATE_HELPER(m11, m12, m13, m21, m22, m23, m31, m32, m33);
	float c01 = DETERMINATE_HELPER(m10, m12, m13, m20, m22, m23, m30, m32, m33);
	float c02 = DETERMINATE_HELPER(m10, m11, m13, m20, m21, m23, m30, m31, m33);
	float c03 = DETERMINATE_HELPER(m10, m11, m12, m20, m21, m22, m30, m31, m32);

	float det = m00 * c00 - m01 * c01 + m02 * c02 - m03 * c03;
	if (NEAR_ZERO(det)) {
		return Mat4::identity();
	}

	float invDet = 1.0f / det;
	Mat4 inv;

	// Row 0
	inv.v[0].x = invDet * c00;
	inv.v[0].y = invDet * (-DETERMINATE_HELPER(m01, m02, m03, m21, m22, m23, m31, m32, m33));
	inv.v[0].z = invDet * DETERMINATE_HELPER(m01, m02, m03, m11, m12, m13, m31, m32, m33);
	inv.v[0].w = invDet * (-DETERMINATE_HELPER(m01, m02, m03, m11, m12, m13, m21, m22, m23));

	// Row 1
	inv.v[1].x = invDet * (-c01);
	inv.v[1].y = invDet * DETERMINATE_HELPER(m00, m02, m03, m20, m22, m23, m30, m32, m33);
	inv.v[1].z = invDet * (-DETERMINATE_HELPER(m00, m02, m03, m10, m12, m13, m30, m32, m33));
	inv.v[1].w = invDet * DETERMINATE_HELPER(m00, m02, m03, m10, m12, m13, m20, m22, m23);

	// Row 2
	inv.v[2].x = invDet * c02;
	inv.v[2].y = invDet * (-DETERMINATE_HELPER(m00, m01, m03, m20, m21, m23, m30, m31, m33));
	inv.v[2].z = invDet * DETERMINATE_HELPER(m00, m01, m03, m10, m11, m13, m30, m31, m33);
	inv.v[2].w = invDet * (-DETERMINATE_HELPER(m00, m01, m03, m10, m11, m13, m20, m21, m23));

	// Row 3
	inv.v[3].x = invDet * (-c03);
	inv.v[3].y = invDet * DETERMINATE_HELPER(m00, m01, m02, m20, m21, m22, m30, m31, m32);
	inv.v[3].z = invDet * (-DETERMINATE_HELPER(m00, m01, m02, m10, m11, m12, m30, m31, m32));
	inv.v[3].w = invDet * DETERMINATE_HELPER(m00, m01, m02, m10, m11, m12, m20, m21, m22);

	success = true;

	return inv;
}
#undef DETERMINATE_HELPER

Mat4 Mat4::operator*(const Mat4& right) const {
	Mat4 C = Mat4(0);

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

Vec4 Mat4::operator*(const Vec4& right) const {
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

Mat4& Mat4::operator*=(const Mat4& right){
	*this = *this * right;
	return *this;
}

bool Mat4::operator==(const Mat4& right) const {
	return (this->v[0] == right.v[0]) && (this->v[1] == right.v[1]) && (this->v[2] == right.v[2]) && (this->v[3] == right.v[3]);
}

bool Mat4::operator!=(const Mat4& r) const {
	return !(*this==r);
}

void Mat4::print() const {
	printf(
		"[%.2f %.2f %.2f %.2f]\n"
		"[%.2f %.2f %.2f %.2f]\n"
		"[%.2f %.2f %.2f %.2f]\n"
		"[%.2f %.2f %.2f %.2f]\n",
		v[0].x, v[0].y, v[0].z, v[0].w,
		v[1].x, v[1].y, v[1].z, v[1].w,
		v[2].x, v[2].y, v[2].z, v[2].w,
		v[3].x, v[3].y, v[3].z, v[3].w
	);
}

// -- 

Quat::Quat() {
	this->w = 1;
	this->v = Vec3(0, 0, 0);
}

Quat::Quat(float w, Vec3 axis) {
	float radians = DEGREES_TO_RAD(w);
	this->w = cosf(radians / 2.0f);
	if (NEAR_ZERO(this->w)) {
		this->w = 0.0f;
	}

	axis = axis.normalize();
	this->v = axis.scale(sinf(radians / 2.0f));
}

Quat::Quat(float w, float x, float y, float z) {
	*this = Quat(w, Vec3(x, y, z));
}

Quat Quat::inverse() {
	Quat ret(1, 0, 0, 0);

	float magnitude_squared = SQUARED(this->w) + Vec3::dot(this->v, this->v);
	if (magnitude_squared == 0.0f) { 
		return Quat::identity();
	}

	ret.w = this->w / magnitude_squared;
	ret.v = this->v.scale(-1.0f / magnitude_squared);

	return ret;
}

Quat Quat::scale(float scale) {
	Quat ret;

	ret.w   = this->w   * scale;
	ret.v.x = this->v.x * scale;
	ret.v.y = this->v.y * scale;
	ret.v.z = this->v.z * scale;

	return ret;
}

Quat Quat::normalize() {
	Vec4 temp = Vec4(this->w, this->v.x, this->v.y, this->v.z).normalize();
	
	Quat ret;
	ret.w = temp.x;
	ret.v.x = temp.y;
	ret.v.y = temp.z;
	ret.v.z = temp.w;

	return ret;
}

Mat4 Quat::get_rotation_matrix() {
	Mat4 result = Mat4::identity();

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

void Quat::to_angle_axis(float &theta, Vec3 &vec) {
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

// NOTE(Jovanni): AI, because I just want to work on the engine. I will come back to understand this and rewrite it. 
Vec3 Quat::to_euler() {
	Quat quat = this->normalize();

	float x = quat.v.x;
	float y = quat.v.y;
	float z = quat.v.z;
	float w = quat.w;

	Vec3 euler = {};

	// Roll (X axis)
	float sinr_cosp = 2.0f * (w * x + y * z);
	float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
	euler.x = atan2f(sinr_cosp, cosr_cosp);

	// Pitch (Y axis)
	float sinp = 2.0f * (w * y - z * x);

	if (fabsf(sinp) >= 1.0f) {
		euler.y = copysignf(PI * 0.5f, sinp);
	} else {
		euler.y = asinf(sinp);
	}

	// Yaw (Z axis)
	float siny_cosp = 2.0f * (w * z + x * y);
	float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
	euler.z = atan2f(siny_cosp, cosy_cosp);

	// Convert to degrees
	euler.x = RAD_TO_DEGREES(euler.x);
	euler.y = RAD_TO_DEGREES(euler.y);
	euler.z = RAD_TO_DEGREES(euler.z);

	return euler;
}

Quat Quat::identity() {
	return {1, 0, 0, 0};
}

Quat Quat::literal(float w, Vec3 axis) {
	Quat ret;
	ret.w = w;
	ret.v = axis;

	return ret;
}

Quat Quat::literal(float w, float x, float y, float z) {
	Quat ret;
	ret.w = w;
	ret.v = Vec3(x, y, z);

	return ret;
}

Quat Quat::from_euler(float theta_x, float theta_y, float theta_z) {
	return from_euler(Vec3(theta_x, theta_y, theta_z));
}

Quat Quat::from_euler(Vec3 euler_angles_degrees) {
	float roll_rad_half = DEGREES_TO_RAD(euler_angles_degrees.x) * 0.5f;
	float pitch_rad_half = DEGREES_TO_RAD(euler_angles_degrees.y) * 0.5f;
	float yaw_rad_half = DEGREES_TO_RAD(euler_angles_degrees.z) * 0.5f;

	float cx = cosf(roll_rad_half);
	float sx = sinf(roll_rad_half);
	float cy = cosf(pitch_rad_half);
	float sy = sinf(pitch_rad_half);
	float cz = cosf(yaw_rad_half);
	float sz = sinf(yaw_rad_half);

	Quat q = Quat::identity();
	q.w = cx * cy * cz + sx * sy * sz;
	q.v.x = sx * cy * cz - cx * sy * sz;
	q.v.y = cx * sy * cz + sx * cy * sz;
	q.v.z = cx * cy * sz - sx * sy * cz;

	return q;
}

Quat Quat::from_angle_axis(float angle, Vec3 axis) {
	float half_angle = DEGREES_TO_RAD(angle) * 0.5f;
	float sinf_half = sinf(half_angle);
	float cosf_half = cosf(half_angle);

	Quat q = Quat::identity();
	axis = axis.normalize();

	q.w     = cosf_half;
	q.v.x   = axis.x * sinf_half;
	q.v.y   = axis.y * sinf_half;
	q.v.z   = axis.z * sinf_half;

	return q;
}

Quat Quat::from_rotation_matrix(const float m[16]) {
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

Quat Quat::from_rotation_matrix(Mat4 mat) {
	return Quat::from_rotation_matrix(&mat.v[0].x);
}

Quat Quat::slerp(Quat q, Quat r, float t) {
	q = q.normalize();
	r = r.normalize();
	float dot = Quat::dot(q, r);

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

float Quat::dot(Quat a, Quat b) {
	float dot = a.w   * b.w   +
				a.v.x * b.v.x +
				a.v.y * b.v.y +
				a.v.z * b.v.z;

	return dot;
}

Quat Quat::operator+(const Quat &right) {
	Quat ret = Quat::identity();

	ret.w = this->w + right.w;
	ret.v = this->v + right.v;

	return ret;
}
Quat& Quat::operator+=(const Quat &right) {
	*this = *this + right;
	return *this;
}

Quat Quat::operator-(const Quat &right) {
	Quat ret = Quat::identity();

	ret.w = this->w - right.w;
	ret.v = this->v - right.v;

	return ret;
}
Quat& Quat::operator-=(const Quat &right) {
	*this = *this - right;
	return *this;
}

Quat Quat::operator*(const Quat &right) {
	Quat ret = Quat::identity();
	ret.w = (this->w * right.w) - Vec3::dot(this->v, right.v);
	ret.v = (this->v.scale(right.w) + right.v.scale(this->w)) + Vec3::cross(this->v, right.v);
	
	return ret;
}
Quat& Quat::operator*=(const Quat &right) {
	*this = *this * right;
	return *this;
}

Vec3 Quat::operator*(const Vec3 &right) {
	Quat q = *this;
	Quat p = Quat::literal(0, right);

	return ((q * p) * q.inverse()).v;
}

bool Quat::operator==(const Quat &right) {
	return NEAR_ZERO(this->w - right.w) && (this->v == right.v);
}
bool Quat::operator!=(const Quat &right) {
	return !(*this == right);
}

Vec3 screenspace_to_worldspace_raycast(double x_screen_space_pos, double y_screen_space_pos, Mat4 view, Mat4 projection, u32 WINDOW_WIDTH, u32 WINDOW_HEIGHT) {
	float x = ((2.0f * x_screen_space_pos) / WINDOW_WIDTH) - 1.0f; // normalizing to (-1, 1) from (0, 1)
	float y = 1.0f - ((2.0f * y_screen_space_pos) / WINDOW_HEIGHT);  // normalizing to (-1, 1) from (0, 1) | Also flipping vertically
	float z = 1.0f;
	Vec3 ndc = Vec3(x, y, z);
	Vec4 clip_space = Vec4(ndc.x, ndc.y, ndc.z, 1.0f);

	bool success = false; 
	Mat4 inverse_projection = projection.inverse(success);

	Vec4 view_space = inverse_projection * clip_space;
	view_space = Vec4(view_space.x, view_space.y, view_space.z, 0.0f);

	success = false; 
	Mat4 inverse_view = view.inverse(success);
	Vec4 world_space = inverse_view * view_space;
	return Vec3(world_space).normalize();
}