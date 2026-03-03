#include "../Math/sfe_math.hpp"
#include "sfe_random.hpp"

namespace Random {
    const int UPPER_MASK       = 0x80000000;
    const int LOWER_MASK       = 0x7fffffff;
    const int TEMPERING_MASK_B = 0x9d2c5680;
    const int TEMPERING_MASK_C = 0xefc60000;
    u32 MAX_MASK = (u32)0xffffffff;

    inline static void _seeding(Seed* r, u32 seed) {
        r->mt[0] = seed & MAX_MASK;
        while (r->index++ < STATE_VECTOR_LENGTH) {
            r->mt[r->index] = (6069 * r->mt[r->index-1]) & MAX_MASK;
        }
    }

    Seed GenerateSeed(u32 seed) {
        Seed rand;
        _seeding(&rand, seed);
        return rand;
    }

    u32 GenerateU32(Seed* r) {
        u32 y;
        static u32 mag[2] = {0x0, 0x9908b0df};
        if (r->index >= STATE_VECTOR_LENGTH || r->index < 0) {
            int32_t kk;
            if (r->index >= STATE_VECTOR_LENGTH+1 || r->index < 0) {
                _seeding(r, 4357);
            }

            for (kk=0; kk<STATE_VECTOR_LENGTH-STATE_VECTOR_M; kk++) {
                y = (r->mt[kk] & UPPER_MASK) | (r->mt[kk+1] & LOWER_MASK);
                r->mt[kk] = r->mt[kk+STATE_VECTOR_M] ^ (y >> 1) ^ mag[y & 0x1];
            }

            for (; kk<STATE_VECTOR_LENGTH-1; kk++) {
                y = (r->mt[kk] & UPPER_MASK) | (r->mt[kk+1] & LOWER_MASK);
                r->mt[kk] = r->mt[kk+(STATE_VECTOR_M-STATE_VECTOR_LENGTH)] ^ (y >> 1) ^ mag[y & 0x1];
            }

            y = (r->mt[STATE_VECTOR_LENGTH-1] & UPPER_MASK) | (r->mt[0] & LOWER_MASK);
            r->mt[STATE_VECTOR_LENGTH-1] = r->mt[STATE_VECTOR_M-1] ^ (y >> 1) ^ mag[y & 0x1];
            r->index = 0;
        }

        y = r->mt[r->index++];
        y ^= (y >> 11);
        y ^= (y << 7) & TEMPERING_MASK_B;
        y ^= (y << 15) & TEMPERING_MASK_C;
        y ^= (y >> 18);

        return y;
    }

    float GenerateF32(Seed* r) {
        u32 value = GenerateU32(r);
        return (float)value / (float)MAX_MASK;
    }

    float GenerateRange(Seed* r, float min, float max) {
        float value = GenerateF32(r);
        return Math::Remap(value, 0, 1, min, max);
    }
}