#pragma once

namespace Math {
    typedef struct Vec3 Vec3;

    float Lerp(float a, float b, float t);
    float InverseLerp(float a, float b, float value);
    Vec3 Barycentric(Vec3 a, Vec3 b, Vec3 c, float u, float v);
    float Remap(float x, float s_min, float s_max, float e_min, float e_max);
    float MoveToward(float current, float target, float delta);
    float Smoothstep(float edge0, float edge1, float x);
    float Smootherstep(float edge0, float edge1, float x);
    int Mod(int a, int b);
} 
