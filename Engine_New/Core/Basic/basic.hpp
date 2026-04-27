#pragma once

// This is gonna be like ckg (bro I can't fuckign believe it took this long to get to this point like fuck me)
// literally the thing that I learned a year ago or two at this point i'm refucking learning.
// fuck c++ fuck your devilish temptations

#include <stdint.h>
#include <stdlib.h>
#include <type_traits>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

enum class Error : int {
	SUCCESS = 0,
	RESOURCE_NOT_FOUND,
	RESOURCE_TOO_BIG,
	NULL_PARAMETER,
	INVALID_PARAMETER,
	COUNT
};

/**
 * @brief returns a string literal of the error code
 * 
 * @param error_code 
 * @return const char*
 */
const char* error_get_string(Error error_code);

struct Timer { 
	bool should_tick = false;
	float elapsed = 0.0f; 
	float duration = 0.0f; 

	static Timer create();
	void start(float duration);
	bool tick(float dt);
	void reset();
	void stop();
};

struct String;
template<typename T>
struct CompileTime {
	static constexpr bool TYPE_IS_CSTRING = std::is_same_v<T, char*> || std::is_same_v<T, const char*>;
	static constexpr bool TYPE_IS_STRING = std::is_same_v<T, String>;
	static constexpr bool TYPE_IS_TRIVIAL = std::is_trivially_copyable_v<T>;
	static constexpr bool TYPE_IS_POINTER = std::is_pointer_v<T>;
};

template <typename K, typename V>
struct KeyValuePair {
	K key;
	V value;
};

#define STRINGIFY(entry) #entry
#define GLUE(a, b) a##b
#define KB(value) ((size_t)(value) * 1024L)
#define MB(value) ((size_t)KB(value) * 1024L)
#define GB(value) ((size_t)MB(value) * 1024L)
#define OFFSET_OF(type, member) (size_t)(&(((type*)0)->member))
#define FIRST_DIGIT(number) ((int)number % 10)
#define GET_BIT(number, bit_to_check) ((number & (1 << bit_to_check)) >> bit_to_check)
#define SET_BIT(number, bit_to_set) number |= (1 << bit_to_set);
#define UNSET_BIT(number, bit_to_unset) number &= (~(1 << bit_to_unset));
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define CLAMP(value, min_value, max_value) (MIN(MAX(value, min_value), max_value))
#define ARRARY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define STRING_LIT_ARG(literal) literal, sizeof(literal) - 1
#define CHAR_IS_DIGIT(c) (((c) >= '0') && ((c) <= '9'))
#define CHAR_IS_UPPER(c) (((c) >= 'A') && ((c) <= 'Z'))
#define CHAR_IS_LOWER(c) (((c) >= 'a') && ((c) <= 'z'))
#define CHAR_IS_ALPHA(c) (CHAR_IS_UPPER(c & 0b11011111))
#define CHAR_IS_ALPHA_NUMERIC(c) (CHAR_IS_ALPHA(c) || CHAR_IS_DIGIT(c))
#define DEFAULT_CAPACITY 16
#define DEFAULT_LOAD_FACTOR 0.7f
#define UNUSED(a) (void)a
#define INTERNAL_LINKAGE static
#define LOCAL_PERSIST static
#define IS_POWER_OF_TWO(n) (n > 0 && (n & (n - 1)) == 0)

constexpr float PI = 3.14159265359f;
constexpr float TAU = PI * 2;
constexpr float G = 0.0000000000667430f;
constexpr float EPSILON = 0.0001f;

#define SQUARED(a) ((a) * (a))
#define DEGREES_TO_RAD(degrees) ((degrees)*(PI/180.0f))
#define RAD_TO_DEGREES(rad) ((rad)*(180.0f/PI))
#define NEAR_ZERO(x) (fabs(x) <= EPSILON)
#define NEAR_EQUAL(a, b) (fabs((a) - (b)) <= EPSILON)

#if defined(_WIN32)
	#define PLATFORM_WINDOWS
	#define CRASH() __debugbreak()

	#undef NOMINMAX
	#undef WIN32_LEAN_AND_MEAN

	#define NOMINMAX
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#elif defined(__APPLE__)
	#define PLATFORM_APPLE
	#define CRASH() __builtin_trap()
	#include <time.h>
#elif (defined(__linux__) || defined(__unix__) || defined(__POSIX__))
	#define PLATFORM_LINUX
	#define CRASH() __builtin_trap()
	#include <time.h>
#else
	#error "Unknown Platform???"
#endif

#if defined(_MSC_VER)
	#define UNUSED_FUNCTION
#elif defined(__clang__)
	#define UNUSED_FUNCTION __attribute__((used))
#elif defined(__GNUC__) || defined(__GNUG__)
	#define UNUSED_FUNCTION __attribute__((used))
#endif

