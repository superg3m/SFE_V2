#pragma once

#include "../Basic/basic.hpp"

namespace Random {
	constexpr u32 STATE_VECTOR_LENGTH = 624;
	constexpr u32 STATE_VECTOR_M      = 397;

	struct Seed {
		u32 mt[STATE_VECTOR_LENGTH];
		u32 index = 0;
	};

	Seed generate_seed(u32 seed); // non-zero seeds work
	u32 generate_u32(Seed* seed);
	float generate_f32(Seed* seed); // 0 to 1 0.12190845 for example
	float generate_range(Seed* seed, float min, float max); // min to max for example
}