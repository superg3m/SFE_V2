#include "../Common/sfe_common.hpp"
#include "sfe_utility.hpp"

namespace Math {
    float Lerp(float a, float b, float t) {
        return a + ((b - a) * t);
    }

    float InverseLerp(float a, float b, float value) {
        if (NEAR_ZERO(a - b)) {
            return 0.0f; // Avoid division by zero
        }

        return (value - a) / (b - a);
    }

    float Remap(float x, float s_min, float s_max, float e_min, float e_max) {
        x = CLAMP(x, s_min, s_max);
        float s_ratio = (x - s_min) / (s_max - s_min);
        
        return e_min + (s_ratio * (e_max - e_min));
    }

    float MoveToward(float current, float target, float delta) {
        float diff = target - current;

        if (fabsf(diff) <= delta) {
            return target;
        }

        return current + (diff > 0 ? delta : -delta);
    }

    int Mod(int a, int b) {
        int rem = a % b;
        return (rem < 0) ? (rem + b) : rem;
    }
}

