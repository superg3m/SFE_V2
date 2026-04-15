#pragma once


#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8  = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

// --
constexpr float PI = 3.14159265359f;
constexpr float TAU = PI * 2;
constexpr float G = 0.0000000000667430f;
constexpr float EPSILON = 0.0001f;

#define STRINGIFY(entry) #entry
#define KB(value) ((size_t)(value) * 1024L)
#define MB(value) ((size_t)KB(value) * 1024L)
#define GB(value) ((size_t)MB(value) * 1024L)
#define OFFSET_OF(type, member) (size_t)(&(((type*)0)->member))
#define UNUSED(a) (void)a
#define INTERNAL_FUNCTION static
#define LOCAL_PERSIST static

#define NEAR_ZERO(x) (fabs(x) <= EPSILON)
#define NEAR_EQUAL(a, b) (fabs((a) - (b)) <= EPSILON)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define CLAMP(value, min_value, max_value) (MIN(MAX(value, min_value), max_value))
#define SQUARED(a) ((a) * (a))
#define DEGREES_TO_RAD(degrees) ((degrees)*(PI/180))
#define RAD_TO_DEGREES(rad) ((rad)*(180/PI))

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

// --

struct Timer { 
    bool should_tick;
    float elapsed; 
    float duration; 

    static Timer create();
    void start(float duration);
    bool tick(float dt);
    void stop();
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    static AABB from_center_extents(glm::vec3 center, glm::vec3 extents) {
        return AABB(center - extents, center + extents);
    }

    static bool intersection(AABB aabb, glm::vec3 p0, glm::vec3 p1) {
        float tmin = -10000;
        float tmax = 10000;
    
        // X coordinate
        if (fabsf(p1.x) > EPSILON) {
            float t1 = (aabb.min.x - p0.x) / p1.x;
            float t2 = (aabb.max.x - p0.x) / p1.x;
    
            tmin = MAX(tmin, MIN(t1, t2));
            tmax = MIN(tmax, MAX(t1, t2));
        }
    
        // Y coordinate
        if (fabs(p1.y) > EPSILON) {
            float t1 = (aabb.min.y - p0.y) / p1.y;
            float t2 = (aabb.max.y - p0.y) / p1.y;
    
            tmin = MAX(tmin, MIN(t1, t2));
            tmax = MIN(tmax, MAX(t1, t2));
        }
    
        // Z coordinate
        if (fabs(p1.z) > EPSILON) {
            float t1 = (aabb.min.z - p0.z) / p1.z;
            float t2 = (aabb.max.z - p0.z) / p1.z;
    
            tmin = MAX(tmin, MIN(t1, t2));
            tmax = MIN(tmax, MAX(t1, t2));
        }
    
        if (tmax > tmin && tmax > 0.0) {
            return true;
        } else {
            return false;
        }
    }

    static AABB create(glm::vec3 min, glm::vec3 max) {
        AABB ret = {};
        ret.min = min;
        ret.max = max;

        return ret;
    }

    static AABB create(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) {
        AABB ret = {};
        ret.min.x = min_x;
        ret.min.y = min_y;
        ret.min.z = min_z;

        ret.max.x = max_x;
        ret.max.y = max_y;
        ret.max.z = max_z;

        return ret;
    }

    glm::vec3 get_center() {
        glm::vec3 extents = this->get_extents();
        return glm::vec3(min.x + extents.x, min.y + extents.y, min.z + extents.z);
    }

    glm::vec3 get_extents() {
        return (max - min) * 0.5f;
    }
};

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

// -- 

#define COLOR_RED "\033[0;31m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_YELLOW "\033[0;33m"
#define COLOR_BLUE "\033[0;34m"
#define COLOR_PURPLE "\033[0;35m"

#define RED_BACKGROUND "\033[41m"
#define COLOR_RESET "\033[0m"

#define LOG_WARNING_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

#if RELEASE == 1
    #define LOG_DEBUG_ENABLED 0
    #define LOG_TRACE_ENABLED 0
#endif

typedef enum LogLevel {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_INFO  = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5,
    LOG_LEVEL_COUNT = 6
} LogLevel;

void LOG_OUTPUT(LogLevel log_level, const char* message, ...);
#define LOG_FATAL(message, ...) LOG_OUTPUT(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) LOG_OUTPUT(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)

#if LOG_WARNING_ENABLED == 1
    #define LOG_WARN(message, ...) LOG_OUTPUT(LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#else
    #define LOG_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
    #define LOG_INFO(message, ...) LOG_OUTPUT(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
    #define LOG_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
    #define LOG_DEBUG(message, ...) LOG_OUTPUT(LOG_LEVEL_DEBUG , message, ##__VA_ARGS__)
#else
    #define LOG_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
    #define LOG_TRACE(message, ...) LOG_OUTPUT(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#else
    #define LOG_TRACE(message, ...)
#endif

// -- 

#if defined(RUNTIME_ASSERTS_DISABLED)
    #define RUNTIME_ASSERT(expression)
    #define RUNTIME_ASSERT_MSG(expression)
#else
    void MACRO_RUNTIME_ASSERT(bool expression, const char* function, const char* file, int line);
    void MACRO_RUNTIME_ASSERT_MSG(bool expression, const char* function, const char* file, int line, const char* fmt, ...);
    #define RUNTIME_ASSERT(expression) MACRO_RUNTIME_ASSERT((expression), __func__, __FILE__, __LINE__)
    #define RUNTIME_ASSERT_MSG(expression, message, ...) MACRO_RUNTIME_ASSERT_MSG((expression), __func__, __FILE__, __LINE__, message, ##__VA_ARGS__)
#endif

#define STATIC_ASSERT static_assert

// -- 

u8* platform_read_entire_file(const char* file_name, size_t& out_file_size, Error& error);
bool platform_file_exists(const char* path);