#pragma once

#include "../Common/sfe_common.hpp"

namespace Random {
    const u32 STATE_VECTOR_LENGTH = 624;
    const u32 STATE_VECTOR_M      = 397;

    struct Seed {
        u32 mt[STATE_VECTOR_LENGTH];
        u32 index = 0;
    };

    Seed GenerateSeed(u32 seed); // non-zero seeds work
    u32 GenerateU32(Seed* seed);
    float GenerateF32(Seed* seed); // 0 to 1 0.12190845 for example
    float GenerateRange(Seed* seed, float min, float max); // min to max for example
}

