#pragma once

// This is gonna be like ckg (bro I can't fuckign believe it took this long to get to this point like fuck me)
// literally the thing that I learned a year ago or two at this point i'm refucking learning.
// fuck c++ fuck your devilish temptations

#define CORE_TYPES
#define CORE_DEFINES
#define CORE_LOGGING
#define CORE_ASSERT
#define CORE_MEMORY
#define CORE_MEMORY_ARENA
#define CORE_MATH
#define CORE_HANDLE_REGISTRY
#define CORE_STRING
#define CORE_HASHING
#define CORE_RANDOM
#define CORE_DATA_STRUCTURES
#define CORE_PLATFORM
#define CORE_FRONTEND

#if defined(CORE_IMPL)
    #define CORE_TYPES_IMPL
    #define CORE_DEFINES_IMPL
    #define CORE_LOGGING_IMPL
    #define CORE_ASSERT_IMPL
    #define CORE_MATH_IMPL
    #define CORE_STRING_IMPL
    #define CORE_MEMORY_IMPL
    #define CORE_MEMORY_ALLOCATOR_IMPL
    #define CORE_MEMORY_ARENA_IMPL
    #define CORE_HASHING_IMPL
    #define CORE_RANDOM_IMPL
    #define CORE_COLLECTION_IMPL
    #define CORE_CONTAINER_IMPL
    #define CORE_PLATFORM_IMPL
    #define CORE_FRONTEND_IMPL
#endif

#if defined(CORE_TYPES)
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

    // --

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

    typedef void* (AllocateFunction)(void* ctx, size_t allocation_size);
    typedef void* (ReallocFunction)(void* ctx, void* data, size_t old_allocation_size, size_t new_allocation_size);
    typedef void  (FreeFunction)(void* ctx, void* data);

    struct Allocator {
        void* ctx;
        AllocateFunction* malloc;
        ReallocFunction* realloc;
        FreeFunction* free;

        bool operator==(Allocator other) const;
        bool operator!=(Allocator other) const;
    };

    Allocator allocator_invalid();
    Allocator allocator_general();
    
    // --

    template <typename K, typename V>
    struct KeyValuePair {
        K key;
        V value;
    };

    // --

    struct Timer { 
        bool should_tick;
        float elapsed; 
        float duration; 
    };

    Timer timer_create();
    void timer_start(Timer* timer, float duration);
    bool timer_tick(Timer* timer, float dt);
    void timer_stop(Timer* timer);

    // --

    bool memory_equal(const void* buffer_one, size_t b1_size, const void* buffer_two, size_t b2_size);

    template <typename T>
    struct View {
        const T* data = nullptr;
        u64 length = 0;

        bool operator==(const View& other) const;
        bool operator!=(const View& other) const;
    };

    template<typename T>
    bool View<T>::operator==(const View<T>& other) const {
        return memory_equal(this->data, this->length, other.data, other.length);
    }

    template<typename T>
    bool View<T>::operator!=(const View<T>& other) const {
        return !(*this == other);
    }

    template<typename T>
    View<T> view_create(const T* data, u64 length) {
        View<T> ret;
        ret.data = data;
        ret.length = length;

        return ret;
    }

    // --

    template<typename T>
    struct CompileTime {
        static constexpr bool TYPE_IS_CSTRING = std::is_same_v<T, char*> || std::is_same_v<T, const char*>;
        static constexpr bool TYPE_IS_STRING_VIEW = std::is_same_v<T, View<char>> || std::is_same_v<T, View<const char>>;
        static constexpr bool TYPE_IS_TRIVIAL = std::is_trivially_copyable_v<T>;
        static constexpr bool TYPE_IS_POINTER = std::is_pointer_v<T>;
    };
#endif

#if defined(CORE_DEFINES)
    #undef STRINGIFY
    #undef GLUE
    #undef KB
    #undef MB
    #undef GB
    #undef MIN
    #undef MAX
    #undef CLAMP
    #undef FIRST_DIGIT
    #undef GET_BIT
    #undef SET_BIT
    #undef UNSET_BIT
    #undef ARRAY_COUNT
    #undef PLATFORM_WINDOWS
    #undef PLATFORM_APPLE
    #undef PLATFORM_LINUX
    #undef OS_DELIMITER
    #undef CRASH
    #undef UNUSED
    #undef UNUSED_FUNCTION
    #undef CHAR_IS_DIGIT
    #undef CHAR_IS_UPPER
    #undef CHAR_IS_LOWER
    #undef CHAR_IS_ALPHA
    #undef CHAR_IS_ALPHA_NUMERIC
    #undef DEFAULT_CAPACITY
    #undef DEFAULT_LOAD_FACTOR

    #define STRINGIFY(entry) #entry
    #define GLUE(a, b) a##b

    #define KB(value) ((size_t)(value) * 1024L)
    #define MB(value) ((size_t)KB(value) * 1024L)
    #define GB(value) ((size_t)MB(value) * 1024L)
    #define OFFSET_OF(type, member) (size_t)(&(((type*)0)->member))
    #define FIRST_DIGIT(number) ((int)number % 10);
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

    #define INTERNAL_FUNCTION static
    #define LOCAL_PERSIST static

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
#endif

#if defined(CORE_LOGGING)
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
#endif

#if defined(CORE_ASSERT)
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
    #define INVARIENT_TRIVAL_TYPE(...) STATIC_ASSERT(std::is_trivially_copyable_v<__VA_ARGS__>)
#endif

#if defined(CORE_MEMORY)
    void memory_zero(void* data, size_t data_size_in_bytes);
    void memory_copy(void* destination, size_t destination_size, const void* source, size_t source_size);
    bool memory_equal(const void* buffer_one, size_t b1_size, const void* buffer_two, size_t b2_size);

    template <typename T>
    void memory_swap(T &a, T &b) {
        T temp = a;
        a = b;
        b = temp;
    }
#endif

#if defined(CORE_STRING)
    #include <stdarg.h>
    #include <string.h>
    #include <stdio.h>

    char* str_allocate(Allocator a, const char* s1, u64 length);
    char* str_sprintf(Allocator a, u64* out_buffer_length, const char* fmt, ...);
    char* str_sprintf(Allocator a, u64* out_buffer_length, const char* fmt, va_list args);
    void str_sprintf(char* buffer, size_t buffer_capacity, u64* out_buffer_length, const char* fmt, ...);
    void str_sprintf(char* buffer, size_t buffer_capacity, u64* out_buffer_length, const char* fmt, va_list args);

    u64 str_length(const char* c_string);
    bool str_equal(const char* s1, u64 s1_length, const char* s2, u64 s2_length);
    bool str_equal(const char* s1, const char* s2);
    bool str_contains(const char* s1, u64 s1_length, const char* contains, u64 contains_length);

    s64 str_index_of(const char* str, u64 str_length, const char* substring, u64 substring_length);
    s64 str_last_index_of(const char* str, u64 str_length, const char* substring, u64 substring_length);
    bool str_starts_with(const char* str, u64 str_length, const char* starts_with, u64 starts_with_length);
    bool str_ends_with(const char* str, u64 str_length, const char* ends_with, u64 ends_with_length);

    void str_copy(char* s1, size_t s1_capacity, const char* s2, u64 s2_length);
    void str_append(char* str, u64 &out_str_length, size_t str_capacity, const char* to_append, u64 to_append_length);
    void str_append(char* str, u64 &out_str_length, size_t str_capacity, View<char> to_append);
    void str_append(char* str, u64 &out_str_length, size_t str_capacity, View<const char> to_append);
    void str_append(char* str, u64 &out_str_length, size_t str_capacity, char to_append);
    void str_insert(char* str, u64 &out_str_length, size_t str_capacity, const char* to_insert, u64 to_insert_length, u64 index);
    void str_insert(char* str, u64 &out_str_length, size_t str_capacity, char to_insert, u64 index);
#endif

#if defined(CORE_HASHING)
    namespace Hashing {
        u64 zero_hash(const void* source, size_t source_size);
        u64 siphash(const void* source, size_t source_size);
        u64 cstring_hash(const void* str, size_t str_length);
        bool cstring_equality(const void* c1, size_t c1_size, const void* c2, size_t c2_size);
        u64 string_view_hash(const void* view, size_t str_length);
        bool string_view_equality(const void* c1, size_t c1_size, const void* c2, size_t c2_size);
    }
#endif

#if defined(CORE_MATH)
    #include <math.h>

    constexpr float PI = 3.14159265359f;
    constexpr float TAU = PI * 2;
    constexpr float G = 0.0000000000667430f;
    constexpr float EPSILON = 0.0001f;

    #undef SQAURED
    #undef DEGREES_TO_RAD
    #undef RAD_TO_DEGREES
    #undef NEAR_ZERO
    #undef NEAR_EQUAL

    #define SQUARED(a) ((a) * (a))
    #define DEGREES_TO_RAD(degrees) ((degrees)*(PI/180.0f))
    #define RAD_TO_DEGREES(rad) ((rad)*(180.0f/PI))
    #define NEAR_ZERO(x) (fabs(x) <= EPSILON)
    #define NEAR_EQUAL(a, b) (fabs((a) - (b)) <= EPSILON)

    // Date: May 18, 2025
    // NOTE(Jovanni): Visualize these at: https://easinfgs.net/
    /*
    float ease_in_sinfe(float t);
    float ease_out_sinfe(float t);
    float ease_in_out_sinfe(float t);
    float ease_in_quad(float t);
    float ease_out_quad(float t);
    float ease_in_cubic(float t);
    float ease_out_cubic(float t);
    float ease_in_out_cubic(float t);
    float ease_in_quart(float t);
    float ease_out_quart(float t);
    float ease_in_out_quart(float t);
    float ease_in_quint(float t);
    float ease_out_quint(float t);
    float ease_in_out_quint(float t);
    float ease_in_expo(float t);
    float ease_out_expo(float t);
    float ease_in_out_expo(float t);
    float ease_in_circ(float t);
    float ease_out_circ(float t);
    float ease_in_out_circ(float t);
    float ease_in_back(float t);
    float ease_out_back(float t);
    float ease_in_out_back(float t);
    float ease_in_elastic(float t);
    float ease_out_elastic(float t);
    float ease_in_out_elastic(float t);
    float ease_in_bounce(float t);
    float ease_out_bounce(float t);
    float ease_in_out_bounce(float t);
    */

    template <typename T>
    inline T average(T* array, int count) {
        T accumlator = 0;
        for (int i = 0; i < count; i++) {
            accumlator += array[i];
        }

        T average = (T)accumlator / (T)count;

        return average;
    }

    float lerp(float a, float b, float t);
    float inverse_lerp(float a, float b, float value);
    float remap(float x, float s_min, float s_max, float e_min, float e_max);
    float move_toward(float current, float target, float delta);
    int mod(int a, int b);

    typedef struct Vec4 Vec4;
    struct Vec2 {
        float x;
        float y;

        float magnitude() const;
        float magnitude_squared() const;
        Vec2 normalize() const;
        Vec2 scale(float scale) const;
        Vec2 scale(float scale_x, float scale_y) const;
        Vec2 scale(Vec2 s) const;

        Vec2 operator+(const Vec2 &right) const;
        Vec2& operator+=(const Vec2 &right);
        Vec2 operator-(const Vec2 &right) const;
        Vec2& operator-=(const Vec2 &right);

        Vec2 operator*(const Vec2 &right) const;
        Vec2& operator*=(const Vec2 &right);

        Vec2 operator/(const Vec2 &right) const;
        Vec2& operator/=(const Vec2 &right);

        bool operator==(const Vec2 &right) const;
        bool operator!=(const Vec2 &right) const;
    };

    typedef struct Vec4 Vec4;
    struct Vec3 {
        union {
            struct {
                float x;
                float y;
                float z;
            };

            struct {
                float r;
                float g;
                float b;
            };
        };

        float magnitude() const;
        float magnitude_squared() const;
        Vec3 normalize() const;
        Vec3 scale(float scale) const;
        Vec3 scale(float scale_x, float scale_y, float scale_z) const;
        Vec3 scale(Vec3 s) const;

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
            struct {
                float x;
                float y;
                float z;
                float w;
            };

            struct {
                float r;
                float g;
                float b;
                float a;
            };
        };

        float magnitude() const;
        float magnitude_squared() const;
        Vec4 normalize() const;
        Vec4 scale(float scale) const;
        Vec4 scale(float scale_x, float scale_y, float scale_z, float scale_w) const;
        Vec4 scale(Vec4 s) const;

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
            struct {
                int x;
                int y;
                int z;
                int w;
            };
        };

        float magnitude() const;
        float magnitude_squared() const;
        IVec4 normalize() const;
        IVec4 scale(int scale) const;
        IVec4 scale(int scale_x, int scale_y, int scale_z, int scale_w) const;
        IVec4 scale(IVec4 s) const;

        IVec4 operator+(const IVec4 &right) const;
        IVec4& operator+=(const IVec4 &right);
        IVec4 operator-(const IVec4 &right) const;
        IVec4& operator-=(const IVec4 &right);

        IVec4 operator*(const IVec4 &right) const;
        IVec4& operator*=(const IVec4 &right);

        IVec4 operator/(const IVec4 &right) const;
        IVec4& operator/=(const IVec4 &right);

        bool operator==(const IVec4 &right) const;
        bool operator!=(const IVec4 &right) const;
    };

    Vec2 vec2_create(float fill);
    Vec3 vec3_create(float fill);
    Vec4 vec4_create(float fill);
    IVec4 ivec4_create(int fill);

    Vec2 vec2_create(float x, float y);
    Vec3 vec3_create(float x, float y, float z);
    Vec4 vec4_create(float x, float y, float z, float w);
    IVec4 ivec4_create(int x, int y, int z, int w);

    Vec3 vec3_create(Vec4 v);
    Vec3 vec3_create(Vec2 v, float z);
    Vec4 vec4_create(Vec3 v, float w);

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
    float vec2_dot(Vec2 a, Vec2 b);
    float vec3_dot(Vec3 a, Vec3 b);
    float vec4_dot(Vec4 a, Vec4 b);
    float ivec4_dot(IVec4 a, IVec4 b);

    Vec2 vec2_lerp(Vec2 a, Vec2 b, float t);
    Vec3 vec3_lerp(Vec3 a, Vec3 b, float t);
    Vec4 vec4_lerp(Vec4 a, Vec4 b, float t);
    IVec4 ivec4_lerp(IVec4 a, IVec4 b, float t);

    float vec2_distance(Vec2 a, Vec2 b);
    float vec3_distance(Vec3 a, Vec3 b);
    float vec4_distance(Vec4 a, Vec4 b);
    float ivec4_distance(IVec4 a, IVec4 b);

    float vec2_distance_squared(Vec2 a, Vec2 b);
    float vec3_distance_squared(Vec3 a, Vec3 b);
    float vec4_distance_squared(Vec4 a, Vec4 b);
    float ivec4_distance_squared(IVec4 a, IVec4 b);

    Vec2 vec2_move_toward(Vec2 a, Vec2 b, float dt);
    Vec3 vec3_move_toward(Vec3 a, Vec3 b, float dt);
    Vec4 vec4_move_toward(Vec4 a, Vec4 b, float dt);
    IVec4 ivec4_move_toward(IVec4 a, IVec4 b, float dt);

    Vec2 vec2_closest(Vec2 a, Vec2 b, Vec2 target = vec2_create(0));
    Vec3 vec3_closest(Vec3 a, Vec3 b, Vec3 target = vec3_create(0));
    Vec4 vec4_closest(Vec4 a, Vec4 b, Vec4 target = vec4_create(0));
    IVec4 ivec4_closest(IVec4 a, IVec4 b, IVec4 target = ivec4_create(0));

    Vec2 vec2_euler(float yaw, float pitch);
    Vec3 vec3_euler(float yaw, float pitch);

    Vec3 vec3_cross(Vec3 A, Vec3 B);

    typedef struct Quat Quat;
    struct Mat4 { // Matrices are ROW-MAJOR
        Vec4 v[4];

        Mat4 transpose() const;
        Mat4 inverse(bool &success) const;

        Mat4 operator*(const Mat4 &right) const;
        Vec4 operator*(const Vec4 &right) const;
        Mat4& operator*=(const Mat4 &right);
        bool operator==(const Mat4 &right) const;
        bool operator!=(const Mat4 &right) const;

        void print() const;
    };

    Mat4 mat4_create(Vec4 r0, Vec4 r1, Vec4 r2, Vec4 r3);
    Mat4 mat4_create(const float row_major[16]);
    Mat4 mat4_create(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    );
    Mat4 mat4_from_column_major(const float mat[16]);
    Mat4 mat4_identity();
    Mat4 mat4_scale(Mat4 mat, float scale);
    Mat4 mat4_scale(Mat4 mat, Vec3 s);
    Mat4 mat4_scale(Mat4 mat, float scale_x, float scale_y, float scale_z);
    Mat4 mat4_rotate(Mat4 mat, float theta, Vec3 axis);
    Mat4 mat4_rotate(Mat4 mat, float theta, float rot_x, float rot_y, float rot_z);
    Mat4 mat4_translate(Mat4 mat, Vec3 t);
    Mat4 mat4_translate(Mat4 mat, float x, float y, float z);
    Mat4 mat4_transform(Vec3 s, float theta, Vec3 axis, Vec3 t);
    Mat4 mat4_inverse_transform(Vec3 s, float theta, Vec3 axis, Vec3 t);
    void mat4_decompose(Mat4 mat, Vec3* out_position, Quat* out_orientation, Vec3* out_scale);
    Mat4 mat4_perspective(float fov_degrees, float aspect, float near_plane, float far_plane);

    Mat4 mat4_orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane);
    Mat4 mat4_lookat(Vec3 position, Vec3 target, Vec3 world_up);

    // -- 

    struct AABB {
        Vec3 min;
        Vec3 max;

        Vec3 center() const;
        Vec3 extents() const;
    };

    AABB aabb_create(Vec3 min, Vec3 max);
    AABB aabb_create(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);
    AABB aabb_from_center_extents(Vec3 center, Vec3 extents);
    bool aabb_check_line_intersection(AABB aabb, Vec3 p0, Vec3 p1);
    // bool aabb_check_intersection(AABB a, AABB b);

    // --

    struct Quat {
        float w;
        Vec3 v;

        Quat inverse() const;
        Quat scale(float scale) const;
        Quat normalize() const;
        Mat4 to_rotation_matrix() const;
        void angle_axis(float &theta, Vec3 &vec) const;

        Quat operator+(const Quat &right) const;
        Quat& operator+=(const Quat &right);
        Quat operator-(const Quat &right) const;
        Quat& operator-=(const Quat &right);
        Quat operator*(const Quat &right) const;
        Quat& operator*=(const Quat &right);
        Vec3 operator*(const Vec3 &right) const;
        bool operator==(const Quat &right) const;
        bool operator!=(const Quat &right) const;
    };

    Quat quat_create(float theta, Vec3 axis);
    Quat quat_create(float theta, float x, float y, float z);
    Quat quat_identity();
    Quat quat_from_euler(float theta_x, float theta_y, float theta_z);
    Quat quat_from_euler(Vec3 euler_angles_degrees);
    Quat quat_from_angle_axis(float angle, Vec3 axis);
    Quat quat_from_rotation_matrix(const float m[16]);
    Quat quat_from_rotation_matrix(Mat4 mat);
    Quat quat_slerp(Quat q, Quat r, float t);
    float quat_dot(Quat a, Quat b);

    Mat4 mat4_rotate(Mat4 mat, Quat quat);
    Mat4 mat4_transform(Vec3 s, Quat r, Vec3 t);
#endif

#if defined(CORE_RANDOM)
    constexpr u32 STATE_VECTOR_LENGTH = 624;
    constexpr u32 STATE_VECTOR_M      = 397;

    struct Seed {
        u32 mt[STATE_VECTOR_LENGTH];
        u32 index = 0;
    };

    Seed random_generate_seed(u32 seed); // non-zero seeds work
    u32 random_generate_u32(Seed* seed);
    float random_generate_f32(Seed* seed); // 0 to 1 0.12190845 for example
    float random_generate_range(Seed* seed, float min, float max); // min to max for example
#endif

#if defined(CORE_DATA_STRUCTURES)
    #include <initializer_list>

    template <typename T>
    struct Vector {
        u64 count;
        u64 capacity;
        T* data;
        Allocator allocator;

        T* begin() { return this->data; }
        T* end() { return this->data + this->count; }
        const T* begin() const { return this->data; }
        const T* end() const { return this->data + this->count; }
        
        T operator[](int i) const {
            RUNTIME_ASSERT_MSG((i >= 0) && (this->count > i), "index: %d is outside of bounds! [0, %lld]\n", i, this->count);

            return this->data[i];
        }

        T& operator[](int i) {
            RUNTIME_ASSERT_MSG((i >= 0) && (this->count > i), "index: %d is outside of bounds! [0, %lld]\n", i, this->count);

            return this->data[i];
        }

        bool operator==(const Vector<T> &right) const {
            if (this->count != right.count) {
                return false;
            }

            for (int i = 0; i < this->count; i++) {
                if (this->data[i] != right.data[i]) {
                    return false;
                }
            }

            return true;
        }
    }; INVARIENT_TRIVAL_TYPE(Vector<int>);

    template<typename T>
    Vector<T> vector_create(Allocator allocator, u64 capacity = DEFAULT_CAPACITY) {
        Vector<T> v = {};
        v.count = 0;
        v.capacity = capacity;
        v.data = nullptr;
        v.allocator = allocator;

        return v;
    }

    template<typename T>
    Vector<T> vector_create(Allocator a, std::initializer_list<T> list) {
        Vector<T> v = {};
        v.count = list.size();
        v.capacity = (v.count * 2) ? (v.count * 2) : DEFAULT_CAPACITY;
        v.allocator = a;

        if (list.size() == 0) return v;

        v.data = (T*)a.malloc(a.ctx, v.capacity * sizeof(T));
        memory_copy(v.data, v.capacity * sizeof(T), list.begin(), list.size() * sizeof(T));
    }

    template<typename T>
    void vector_grow(Vector<T>* v) {
        if (!v->data) { 
            v->data = (T*)v->allocator.malloc(v->allocator.ctx, v->capacity * sizeof(T));
        }

        // TODO(Jovanni): Make this use an allocator
        if (v->count == v->capacity) {
            size_t old_allocation_size = (v->capacity * sizeof(T));
            v->capacity *= 2;
            size_t new_allocation_size = (v->capacity * sizeof(T));
            v->data = (T*)v->allocator.realloc(v->allocator.ctx, v->data, old_allocation_size, new_allocation_size);
        }
    }

    template<typename T>
    void vector_append(Vector<T>* v, T value) {
        vector_grow(v);

        v->data[v->count] = value;
        v->count += 1;
    }

    /**
     * If this returns -1 then it couldn not find the element
     * If there are multiple occurences of this element return the first one's index
     */
    template<typename T>
    int vector_find(Vector<T>* v, T element) {
        for (int i = 0; i < v->count; i++) {
            if (v->data[i] == element) {
                return i;
            }
        }

        return -1;
    }

    template<typename T>
    void vector_stable_remove(Vector<T>* v, int index) {
        RUNTIME_ASSERT_MSG(v->count > 0, "You may not remove if the vector is empty!\n");
        RUNTIME_ASSERT_MSG((index >= 0) && (v->count > index), "index is outside of bounds!\n");

        for (int i = index; i < v->count - 1; i++) {
            v->data[i] = v->data[i + 1];
        }

        v->count -= 1;
    }

    template<typename T>
    void vector_unstable_remove(Vector<T>* v, int index) {
        RUNTIME_ASSERT_MSG(v->count > 0, "You may not remove if the vector is empty!\n");
        RUNTIME_ASSERT_MSG((index >= 0) && (v->count > index), "index is outside of bounds!\n");

        v->data[index] = v->data[--v->count];
    }

    template<typename T>
    void vector_clear(Vector<T>* v) {
        v->count = 0;
        size_t allocation_size = v->capacity * sizeof(T);
        memory_zero(v->data, allocation_size);
    }

    template<typename T>
    void vector_resize(Vector<T>* v, u64 count) {
        const size_t total_allocation_size = sizeof(T) * v->capacity;

        // const size_t old_allocation_size = sizeof(T) * v->count;
        const size_t new_allocation_size = sizeof(T) * count;
        if ((v->data == nullptr) || new_allocation_size > total_allocation_size) {
            v->data = (T*)v->allocator.realloc(v->allocator.ctx, v->data, new_allocation_size);
        }

        v->count = count;
        v->capacity = count;
    }

    template<typename T>
    void vector_reserve(Vector<T>* v, u64 count) {
        const size_t total_allocation_size = sizeof(T) * v->capacity;

        // const size_t old_allocation_size = sizeof(T) * v->count;
        const size_t new_allocation_size = sizeof(T) * count;

        if (new_allocation_size > total_allocation_size) {
            v->data = (T*)v->allocator.malloc(v->allocator.ctx, v->data, new_allocation_size);
            v->capacity = count;
        }
    }

    template<typename T>
    void vector_copy(Vector<T>* destination, Vector<T>* source) {
        RUNTIME_ASSERT(destination->data == nullptr);

        destination->capacity = source->capacity;
        destination->count = source->count;

        if (source->data == nullptr) return;

        size_t allocation_size = destination->capacity * sizeof(T);
        destination->data = (T*)destination->allocator.malloc(destination->allocator.ctx, allocation_size);
        memory_copy(destination->data, allocation_size, source->data, allocation_size);
    }

    // -- 

    typedef u64(HashFunction)(const void*, size_t);
    typedef bool(EqualFunction)(const void*, size_t, const void*, size_t);

    template <typename K, typename V>
    struct HashmapEntry {
        K key;
        V value;
        bool filled = false;
        bool dead = false;

        bool IsValid() {
            return this->filled && !this->dead;
        }
    };

    template <typename K, typename V>
    struct Hashmap {
        STATIC_ASSERT(
            CompileTime<K>::TYPE_IS_TRIVIAL || 
            CompileTime<K>::TYPE_IS_POINTER || 
            CompileTime<K>::TYPE_IS_CSTRING ||
            CompileTime<K>::TYPE_IS_STRING_VIEW
        );

        u64 count;
        u64 dead_count;
        u64 capacity;
        HashmapEntry<K, V>* entries;
        HashFunction* hash_func;
        Allocator allocator;

        struct Iterator {
            HashmapEntry<K, V>* ptr;
            HashmapEntry<K, V>* end;

            Iterator(HashmapEntry<K, V>* p, HashmapEntry<K, V>* e) : ptr(p), end(e) {
                skip();
            }

            void skip() {
                while ((ptr != end) && !ptr->IsValid()) {
                    ptr++;
                }
            }

            HashmapEntry<K, V>& operator*() const {
                return *ptr;
            }

            Iterator& operator++() {
                ++ptr;
                skip();
                return *this;
            }

            bool operator!=(const Iterator& other) const {
                return ptr != other.ptr;
            }
        };

        Iterator begin() {
            return Iterator(this->entries, this->entries + this->capacity);
        }

        Iterator end() {
            return Iterator(this->entries + this->capacity, this->entries + this->capacity);
        }

        float load_factor() const {
            return (float)(this->dead_count + this->count) / (float)this->capacity;
        }
    };

    template <typename K>
    constexpr HashFunction* compile_time_get_hash_function() {
        if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
            return Hashing::cstring_hash;
        } else if (CompileTime<K>::TYPE_IS_STRING_VIEW) {
            return Hashing::string_view_hash;
        } else if (CompileTime<K>::TYPE_IS_TRIVIAL || CompileTime<K>::TYPE_IS_POINTER) {
            return Hashing::siphash;
        } else {
            RUNTIME_ASSERT(false);
            return nullptr;
        }
    }

    /*
    template <typename K>
    constexpr EqualFunction* compile_time_get_equality_function() {
        if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
            return Hashing::cstring_equality;
        } else if (CompileTime<K>::TYPE_IS_STRING_VIEW) {
            return Hashing::string_view_equality;
        } else if (CompileTime<K>::TYPE_IS_TRIVIAL || CompileTime<K>::TYPE_IS_POINTER) {
            return memory_equal;
        } else {
            RUNTIME_ASSERT(false);
            return nullptr;
        }
    }
    */

    template<typename K>
    u64 hashmap_safe_hash(HashFunction* hash_func, K key) {
        if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
            return hash_func((void*)key, 0);
        } else if constexpr (CompileTime<K>::TYPE_IS_STRING_VIEW) {
            return hash_func(&key, 0);
        } else if constexpr (CompileTime<K>::TYPE_IS_TRIVIAL && !CompileTime<K>::TYPE_IS_POINTER) {
            return hash_func(&key, sizeof(K));
        } else if constexpr (CompileTime<K>::TYPE_IS_POINTER) {
            return hash_func((void*)&key, sizeof(K));
        } else {
            RUNTIME_ASSERT_MSG(false, "WE ARE IN HELL?\n");
            return 0;
        }
    }

    /*
    template<typename K>
    bool hashmap_safe_equality(EqualFunction equal_func, K k1, K k2) {
        if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
            return equal_func(k1, 0, k2, 0);
        } else if constexpr (CompileTime<K>::TYPE_IS_STRING_VIEW) {
            return equal_func(&k1, 0, &k2, 0);
        } else if constexpr (CompileTime<K>::TYPE_IS_TRIVIAL && !CompileTime<K>::TYPE_IS_POINTER) {
            return equal_func(&k1, sizeof(K), &k2, sizeof(K));
        } else if constexpr (CompileTime<K>::TYPE_IS_POINTER) {
            return equal_func((void*)&k1, sizeof(K), (void*)&k2, sizeof(K));
        } else {
            RUNTIME_ASSERT_MSG(false, "WE ARE IN HELL?\n");
            return false;
        }
    }
    */

    template<typename K, typename V>
    s64 hashmap_resolve_collision(Hashmap<K, V>* h,  K key, u64 inital_hash_index) {
        s64 cannonical_hash_index = inital_hash_index;

        u64 visited_count = 0;
        while (true) {
            HashmapEntry<K, V>* entry = &h->entries[cannonical_hash_index];
            if (!entry->filled) {
                break;
            }

            bool equality_match = false;
            if constexpr (CompileTime<K>::TYPE_IS_CSTRING) {
                equality_match = str_equal(key, entry->key);
            } else {
                equality_match = (key == entry->key);
            }
            
            if (equality_match) {
                break;
            }

            if (visited_count > h->capacity) {
                return -1;
            }

            visited_count += 1;
            cannonical_hash_index += 1;
            cannonical_hash_index = cannonical_hash_index % h->capacity;
        }

        return cannonical_hash_index;
    }

    template<typename K, typename V>
    void hashmap_grow_rehash(Hashmap<K, V>* h) {
        if (!h->entries) {
            RUNTIME_ASSERT(h->capacity);
            h->entries = (HashmapEntry<K, V>*)h->allocator.malloc(h->allocator.ctx, h->capacity * sizeof(HashmapEntry<K, V>));
            memory_zero(h->entries, h->capacity * sizeof(HashmapEntry<K, V>));
            return;
        }

        u64 old_capacity = h->capacity;
        HashmapEntry<K, V>* old_entries = h->entries;

        h->capacity *= 2;
        size_t new_allocation_size = (h->capacity * sizeof(HashmapEntry<K, V>));
        h->entries = (HashmapEntry<K, V>*)h->allocator.malloc(h->allocator.ctx, new_allocation_size);
        memory_zero(h->entries, new_allocation_size);

        // rehash
        for (u64 i = 0; i < old_capacity; i++) {
            HashmapEntry<K, V> old_entry = old_entries[i];
            if (!old_entry.filled || old_entry.dead) {
                continue;
            }

            u64 hash = hashmap_safe_hash<K>(h->hash_func, old_entry.key);
            s64 index = hashmap_resolve_collision<K, V>(h, old_entry.key, hash % h->capacity);
            RUNTIME_ASSERT(index != -1);

            h->entries[index] = old_entry;
        }

        h->allocator.free(h->allocator.ctx, old_entries);
    }

    template<typename K, typename V>
    Hashmap<K, V> hashmap_create(
        Allocator allocator,
        u64 capacity = DEFAULT_CAPACITY,
        HashFunction* hash_func = compile_time_get_hash_function<K>()
    ) {
        Hashmap<K, V> h = {};
        h.count = 0;
        h.capacity = capacity;
        h.hash_func = hash_func;
        h.entries = nullptr;
        h.allocator = allocator;

        return h;
    }

    template<typename K, typename V>
    Hashmap<K, V> hashmap_create(
        Allocator allocator,
        std::initializer_list<KeyValuePair<K, V>> list,
        HashFunction* hash_func = compile_time_get_hash_function<K>()
    ) {
        Hashmap<K, V> h = {};
        h.count = 0;
        h.capacity = DEFAULT_CAPACITY;
        h.hash_func = hash_func;
        h.entries = nullptr;
        h.allocator = allocator;

        if (list.size() == 0) return h;

        h.capacity = list.size() * 2;
        for (KeyValuePair<K, V> pair : list) {
            hashmap_put(&h, pair.key, pair.value);
        }

        return h;
    }
    
    template<typename K, typename V>
    bool hashmap_has(Hashmap<K, V>* h, K key) {
        if (h->entries == nullptr) return false;

        u64 hash = hashmap_safe_hash(h->hash_func, key);
        s64 index = hashmap_resolve_collision(h, key, hash % h->capacity);
        if (index == -1) {
            return false;
        }

        HashmapEntry<K, V>* entry = &h->entries[index];
        return entry->IsValid();
    }

    template<typename K, typename V>
    void hashmap_put(Hashmap<K, V>* h, K key, V value) {
        if ((h->entries == nullptr) || (h->load_factor() >= DEFAULT_LOAD_FACTOR)) {
            hashmap_grow_rehash(h);
        }

        u64 hash = hashmap_safe_hash<K>(h->hash_func, key);
        s64 index = hashmap_resolve_collision<K>(h, key, hash % h->capacity);
        RUNTIME_ASSERT(index != -1);

        HashmapEntry<K, V>* entry = &h->entries[index];
        if (!entry->filled || entry->dead) {
            h->count += 1;
        }

        entry->key = key;
        entry->value = value;
        entry->filled = true;
        entry->dead = false;
    }

    template<typename K, typename V>
    V hashmap_get(Hashmap<K, V>* h, K key) {
        RUNTIME_ASSERT_MSG(hashmap_has(h, key), "Key doesn't exist\n");

        u64 hash = hashmap_safe_hash(h->hash_func, key);
        s64 index = hashmap_resolve_collision(h, key, hash % h->capacity);
        RUNTIME_ASSERT(index != -1);
        
        HashmapEntry<K, V>* entry = &h->entries[index];
        return entry->value;
    }

    template<typename K, typename V>
    V hashmap_remove(Hashmap<K, V>* h, K key) {
        RUNTIME_ASSERT_MSG(hashmap_has(h, key), "Key doesn't exist\n");

        u64 hash = hashmap_safe_hash(h->hash_func, key);
        s64 index = hashmap_resolve_collision(h, key, hash % h->capacity);
        RUNTIME_ASSERT(index != -1);
        
        HashmapEntry<K, V>* entry = &h->entries[index];

        h->count -= 1;
        h->dead_count += 1;
        entry->dead = true;

        return entry->value;
    }
    
    template<typename K, typename V>
    void hashmap_clear(Hashmap<K, V>* h) {
        h->count = 0;
        h->dead_count = 0;
        memory_zero(h->entries, sizeof(HashmapEntry<K, V>) * h->capacity);
    }
    
    // --

    template <typename T>
    struct Stack {
        T* data;
        u64 count;
        u64 capacity;
        Allocator allocator;
    }; INVARIENT_TRIVAL_TYPE(Stack<int>);

    template<typename T>
    void stack_grow(Stack<T>* s) {
        if (!s->data) { 
            s->data = (T*)s->allocator.malloc(s->allocator.ctx, s->capacity * sizeof(T));
        }

        // TODO(Jovanni): Make this use an allocator
        if (s->count == s->capacity) {
            size_t old_allocation_size = (s->capacity * sizeof(T));
            s->capacity *= 2;
            size_t new_allocation_size = (s->capacity * sizeof(T));
            s->data = (T*)s->allocator.realloc(s->allocator.ctx, s->data, old_allocation_size, new_allocation_size);
        }
    }

    template<typename T>
    T stack_peek(Stack<T>* s) {
        RUNTIME_ASSERT_MSG(!stack_empty(s), "You may not peek if the stack is empty!\n");
        return s->data[s->count - 1];
    }

    template<typename T>
    bool stack_empty(Stack<T>* s) {
        return s->count == 0;
    }

    template<typename T>
    Stack<T> stack_create(Allocator allocator) {
        Stack<T> s = {};
        s.allocator = allocator;
        s.capacity = DEFAULT_CAPACITY;

        return s;
    }

    template<typename T>
    void stack_push(Stack<T>* s, T value) {
        if (!s->data || (s->count == s->capacity)) {
            stack_grow(s);
        }

        s->data[s->count++] = value;
    }

    template<typename T>
    T stack_pop(Stack<T>* s) {
        RUNTIME_ASSERT_MSG(!stack_empty(s), "You may not pop if the stack is empty!\n");

        return s->data[--s->count];
    }

    template<typename T>
    void stack_copy(Stack<T>* destination, Stack<T>* source) {
        destination->capacity = source->capacity;
        destination->count = source->count;

        size_t allocation_size = destination->capacity * sizeof(T);
        destination->data = (T*)destination->allocator.malloc(destination->allocator.ctx, allocation_size);
        memory_copy(destination->data, allocation_size, source->data, allocation_size);
    }

    // --

    template <typename T, int N>
    struct RingBuffer {
        T data[N] = {};
        u64 count = 0;
        const u64 capacity = N;

        size_t read = 0;
        size_t write = 0;
    }; INVARIENT_TRIVAL_TYPE(RingBuffer<int, 5>);

    template <typename T, int N>
    void ring_buffer_enqueue(RingBuffer<T, N>* b, T value) {
        RUNTIME_ASSERT_MSG(!b->Full(), "You may not enqueue if the ring queue is full!\n");

        b->data[b->write] = value;
        b->count += 1;
        b->write = (b->write + 1) % b->capacity;
    }

    template <typename T, int N>
    T ring_buffer_dequeue(RingBuffer<T, N>* b) {
        RUNTIME_ASSERT_MSG(!b->Empty(), "You may not dequeue if the ring queue is empty!\n");

        T ret = b->data[b->read];
        b->count -= 1;
        b->read = (b->read + 1) % b->capacity;

        return ret;
    }

    template <typename T, int N>
    bool ring_buffer_empty(RingBuffer<T, N>* r) {
        return r->count == 0;
    }

    template <typename T, int N>
    bool ring_buffer_full(RingBuffer<T, N>* r)  {
        return r->count == r->capacity;
    }

    template <typename T, int N>
    void ring_buffer_copy(RingBuffer<T, N>* destination, RingBuffer<T, N>* source) {
        destination->count = source->count;
        destination->read = source->read;
        destination->write = source->write;

        size_t allocation_size = destination->capacity * sizeof(T);
        memory_copy(destination->data, allocation_size, source->data, allocation_size);
    }
#endif

#if defined(CORE_MEMORY_ARENA)
    // TODO(Jovanni): add linked list memory pages flag
    enum ArenaFlag {
        ARENA_FLAG_FIXED        = 0x1,
        ARENA_FLAG_CIRCULAR     = 0x2,
        ARENA_FLAG_STACK_MEMORY = 0x4
    };

    struct Arena {
        int flags = 0;
        size_t used = 0;
        size_t capacity = 0;
        u8 alignment = 0;
        u8* base_address = nullptr;

        Stack<size_t> stack_sizes;
        Stack<size_t>  temp_used_stack;
        Allocator allocator;
    };

    Arena arena_create(void* memory, size_t allocation_size, int flags, u8 alignment);
    void* arena_push(void* arena_ctx, size_t allocation_size);
    void arena_pop(Arena* arena, size_t to_pop);
    void arena_free(void* arena_ctx, void* data);
    void* arena_realloc(void* arena_ctx, void* data, size_t old_allocation_size, size_t new_allocation_size);
    Allocator arena_to_allocator(Arena* arena);

    void arena_begin_temp(Arena* arena);
    void arena_end_temp(Arena* arena);

    #define ArenaCreateFixed(memory, allocation_size, is_stack_memory) arena_create(memory, allocation_size, ARENA_FLAG_FIXED|(is_stack_memory ? ARENA_FLAG_STACK_MEMORY : 0), 8)
    #define ArenaCreateCircular(memory, allocation_size, is_stack_memory) arena_create(memory, allocation_size, ARENA_FLAG_CIRCULAR|(is_stack_memory ? ARENA_FLAG_STACK_MEMORY : 0), 8)
#endif

#if defined(CORE_HANDLE_REGISTRY)
    const s32 INVALID_HANDLE_INDEX = -1;

    template<typename T>
    struct Handle {
        s32 index = INVALID_HANDLE_INDEX;
        u32 generation = 0;

        bool operator==(Handle<T> other) const {
            return this->index == other.index && this->generation == other.generation;
        }

        bool operator!=(Handle<T> other) const {
            return !(*this == other);
        }
    };

    template<typename T>
    Handle<T> handle_create(u32 index, u32 generation) {
        Handle<T> ret = {};
        return ret;
    }

    template<typename T>
    Handle<T> handle_invalid() {
        Handle<T> ret = {};
        ret.index = -1;
        
        return ret;
    }

    // -- 

    template<typename T>
    struct Slot {
        bool allocated = false;
        u32 generation = 0;
        T data;
    };

    template<typename T>
    struct Registry {
        // I need LinkedList because the addresses won't be the same otherwise when this resizes...
        // I want like a freelist
        Vector<Slot<T>> slots;
        Allocator allocator;
    };

    template<typename T>
    Handle<T> registery_aquire_handle(Registry<T>* registry) {
        Handle<T> handle = handle_invalid<T>();
        for (int i = 0; i < registry->slots.count; i++) {
            Slot<T>& slot = registry->slots[i];
            if (slot.allocated == false) {
                handle = handle_create<T>(i, slot.generation);
                slot.allocated = true;
                break;
            }
        }

        RUNTIME_ASSERT(handle != handle_invalid<T>());

        /*
        if (handle == handle_invalid<T>()) {
            registry->slots.push(Slot<T>{});
            handle.index = registry->slots.count() - 1;
            handle.generation = registry->slots[handle.index].generation;
        }
        */

        memory_zero(&registry->slots[handle.index].data, sizeof(T));

        return handle;
    }

    template<typename T>
    bool registry_is_handle_valid(Registry<T>* registry, Handle<T> handle) {
        return handle.index != INVALID_HANDLE_INDEX && registry->slots[handle.index].generation == handle.generation;
    }

    template<typename T>
    Handle<T> registery_release_handle(Registry<T>* registry, Handle<T> handle) {
        RUNTIME_ASSERT(registry);
        RUNTIME_ASSERT(registry_is_handle_valid(registry, handle));

        registry->slots[handle.index].allocated = false;
        registry->slots[handle.index].generation++;
    }

    template<typename T>
    Registry<T> registry_create(Allocator allocator) {
        Registry<T> ret = {};

        ret.slots = vector_create<Slot<T>>(allocator, 100);
        ret.allocator = allocator;
        vector_resize(&ret.slots, 100);

        return ret;
    }

    template<typename T>
    T* registry_get(Registry<T>* registry, Handle<T> handle) {
        RUNTIME_ASSERT(registry);
        RUNTIME_ASSERT(registery_is_valid_handle(registry, handle));

        return &registry->slots[handle.index].data;
    }

    template<typename T>
    Handle<T> registry_find(Registry<T>* registry, T* element) {
        Handle<T> handle = handle_invalid<T>();
        for (int i = 0; i < registry->slots.count; i++) {
            Slot<T>& slot = registry->slots[i];
            if (element == &slot.data && slot.allocated) {
                handle = handle_create<T>(i, slot.generation);
                break;
            }
        }

        return handle;
    }
#endif

#if defined(CORE_PLATFORM)
    typedef void DLL;

    struct FileTime {
        #if PLATFORM_WINDOWS
            FILETIME time;
        #else
            time_t time;
        #endif
    };

    bool platform_initialize();
    void platform_shutdown();
    void platform_sleep(u32 ms);
    double platform_get_seconds_elapsed();
    bool platform_file_exists(const char* path);
    /**
     * @brief returns true if copy succeeded
     * 
     * @param source_path 
     * @param dest_path 
     * @param block_until_success
     */
    bool platform_copy_file(const char* source_path, const char* dest_path, bool block_until_success = true);
    u8* platform_read_entire_file(Allocator a, const char* file_path, size_t& out_file_size, Error& error);
    DLL* platform_load_dll(const char* dll_path, Error& error);
    DLL* platform_free_dll(DLL* dll, Error& error);
    void* platform_get_function_address(DLL* dll, const char* proc_name, Error& error);

    char* platform_get_executable_directory();
    char* platform_get_asset_directory();

    FileTime platform_get_file_modification_time(const char* file_path);
    
    /**
     * @brief
     *  if returns false: the file times are false
     *  if returns true: The file times are equal
     * 
     * @param a 
     * @param b
     * @return bool 
     */
    bool platform_compare_file_modification_time(FileTime a, FileTime b);
    Allocator platform_allocator();
#endif

#if defined(CORE_FRONTEND)
    #define X_SYNTAX_TOKENS          \
        X(TS_PLUS, "+")              \
        X(TS_MINUS, "-")             \
        X(TS_STAR, "*")              \
        X(TS_DIVISION, "/")          \
        X(TS_MODULUS, "%")           \
        X(TS_LEFT_CURLY, "{")        \
        X(TS_RIGHT_CURLY, "}")       \
        X(TS_LEFT_PAREN, "(")        \
        X(TS_RIGHT_PAREN, ")")       \
        X(TS_LEFT_BRACKET, "[")      \
        X(TS_RIGHT_BRACKET, "]")     \
        X(TS_COLON, ":")             \
        X(TS_COMMA, ",")             \
        X(TS_SEMI_COLON, ";")        \
        X(TS_RIGHT_ARROW, "->")      \
                                    \
        X(TS_EQUALS, "=")           \
        X(TS_INFER_EQUALS, ":=")    \
        X(TS_PLUS_EQUALS, "+=")     \
        X(TS_MINUS_EQUALS, "-=")    \
        X(TS_STAR_EQUALS, "*=")     \
        X(TS_DIVISION_EQUALS, "/=") \
                                    \
        X(TS_OR, "||")              \
        X(TS_AND, "&&")             \
                                    \
        X(TS_EQUAL, "==")           \
        X(TS_NOT_EQUAL, "!=")       \
        X(TS_GT, ">")               \
        X(TS_GT_OR_EQUAL, ">=")     \
        X(TS_LT_OR_EQUAL, "<=")     \
        X(TS_EQUALS_EQUALS, "==")   \

    #define X_LITERAL_TOKENS    \
        X(TS_LITERAL_INTEGER)   \
        X(TS_LITERAL_FLOAT)     \
        X(TS_LITERAL_STRING)    \
        X(TS_LITERAL_CHARACTER) \

    #define X_KEYWORD_TOKENS   \
        X(TS_KEYWORD_IF, "if")         \
        X(TS_KEYWORD_ELSE, "else")     \
        X(TS_KEYWORD_FOR, "for")       \
        X(TS_KEYWORD_WHILE, "while")   \
        X(TS_KEYWORD_TRUE, "true")     \
        X(TS_KEYWORD_FALSE, "false")   \
        X(TS_KEYWORD_FUNC, "func")     \
        X(TS_KEYWORD_VAR, "var")       \
        X(TS_KEYWORD_NULL, "null")     \
        X(TS_KEYWORD_RETURN, "return") \
        X(TS_KEYWORD_PRINT, "print")   \

    #define X_PRIMITIVE_TYPES_TOKENS     \
        X(TS_PRIMITIVE_VOID, "void")     \
        X(TS_PRIMITIVE_INT, "int")       \
        X(TS_PRIMITIVE_FLOAT, "float")   \
        X(TS_PRIMITIVE_STRING, "string") \
        
    enum TokenType {
        TOKEN_ILLEGAL_TOKEN,
        TOKEN_EOF,
        
        #define X(name, str) name,
            X_SYNTAX_TOKENS
        #undef X

        #define X(name) name,
            X_LITERAL_TOKENS
        #undef X

        #define X(name, str) name,
            X_KEYWORD_TOKENS
        #undef X

        #define X(name, str) name,
            X_PRIMITIVE_TYPES_TOKENS
        #undef X

        TOKEN_IDENTIFIER,

        TOKEN_COUNT
    };

    struct Token {
        TokenType type;
        u32 line;
        View<char> sv; // used for identifer names, string literals, the source_view

        union {
            int i;
            float f;
            char c;
            bool b;
        };

        void print() const;
    };

    const char* token_to_string(Token token);
    const char* token_to_string(TokenType type);

    Token token_invalid();
    Token token_create(TokenType token_type, View<char> sv, int line);

    Token token_keyword_from_source_view(View<char> sv, int line);
    Token token_syntax_from_source_view(View<char> sv, int line);
    Token token_literal_from_source_view(View<char> sv, int line);
    Token token_primitive_from_source_view(View<char> sv, int line);

    // -- 

    /**
        @Author: Jovanni Djonaj
        @Date: Oct 8th, 2025
        @Description: {
            Token Management and Lexer Extensibility This Lexer module is highly extensible,
            with most token types being data-driven via the `#define X(...)` definitions in 'core_token.h'.
        }
        -----------------------------------------------------------------------------
        HOW TO ADD NEW TOKENS:
        -----------------------------------------------------------------------------
        1. X_KEYWORD_TOKENS and X_LITERAL_TOKENS (Easy): {
            To add new keyword or literal token simply add an entry to the respective macro in 'token.h'. 
            The rest of the lexer/parser handles these tokens automatically. No code changes required.
        }

        2. Syntax Tokens (The Exception): {
            - Single-Character Syntax (e.g., '+', '{'): The system handles these automatically.

            - Multi-Character Syntax (e.g., "->", "==", "+="): You MUST add the entry 
            to `X_SYNTAX_TOKENS` and manually implement the logic to consume this token 
            within the `lexer.consumeSyntax()` function. This is the only part of token 
            addition that requires code outside of 'token.h'.
        }
    */
    struct Lexer {
        View<char> source;
        Vector<Token>* tokens;
        u32 left_pos;
        u32 right_pos;
        u32 line;
        char c;

        bool is_eof() const;
        void report_error(const char* msg) const;
        View<char> scratch_buffer() const;
        char peek_nth_character(u64 n = 0) const;
    };

    void lexer_generate_tokens(u8* data, size_t file_size, Vector<Token>* out_tokens);

    // -- 

    struct Parser {
        Allocator allocator;
        Vector<Token>* tokens;
        int current;

        Token previous_token() const;
        Token peek_nth_token(int n = 0) const;
        void report_error(const char* fmt, ...) const;
    };

    Parser parser_create(Allocator allocator, Vector<Token>* in_tokens);

    Token parser_consume_next_token(Parser* parser);
    Token parser_expect(Parser* parser, TokenType expected_type);
    bool parser_consume_on_match(Parser* parser, TokenType expected_type);

    // --

    enum JsonValueType {
        JSON_VALUE_INT,
        JSON_VALUE_FLOAT,
        JSON_VALUE_STRING,
        JSON_VALUE_OBJECT,
        JSON_VALUE_ARRAY,
        JSON_VALUE_BOOL,
        JSON_VALUE_NULL
    };

    struct JSON;

    struct JsonValueObject {
        JsonValueObject() = default;

        void push(const char* key, JSON* value) {
            RUNTIME_ASSERT_MSG(!hashmap_has(&this->keys, key), "Duplicate key: %s\n", key);

            hashmap_put(&this->keys, key, this->pairs.count);
            vector_append(&this->pairs, {key, value});
        }
        
        // NOTE(Jovanni): int is the index where the key_value pair is stored in teh pairs vector
        Hashmap<const char*, u64> keys;
        Vector<KeyValuePair<const char*, JSON*>> pairs;
    };

    struct JsonValueArray {
        Vector<JSON*> elements;
    };

    template<typename T>
    concept SupportedType = (
        std::is_same_v<T, int> ||
        std::is_same_v<T, float> || 
        std::is_same_v<T, double> || 
        std::is_same_v<T, bool> ||
        std::is_same_v<T, char*> ||
        std::is_same_v<T, const char*> || 
        std::is_same_v<T, View<char>> ||
        std::is_same_v<T, JSON*>
    );

    struct JSON {
        JsonValueType type;
        Allocator allocator;

        union {
            int integer;
            float floating;
            bool boolean;
            View<char> string;
            JsonValueObject object;
            JsonValueArray array;
        };
        
        bool has(const char* key) {
            RUNTIME_ASSERT(this->type == JsonValueType::JSON_VALUE_OBJECT);
            return hashmap_has(&this->object.keys, key);
        }

        template<SupportedType T>
        T get(const char* key) {
            RUNTIME_ASSERT(this->has(key));

            u64 index = hashmap_get(&this->object.keys, key);
            JSON* value = this->object.pairs[index].value;

            if constexpr (std::is_same_v<T, int>) {
                RUNTIME_ASSERT(value->type == JsonValueType::JSON_VALUE_INT);
                return value->integer;
            } else if constexpr (std::is_same_v<T, float>) {
                RUNTIME_ASSERT(value->type == JsonValueType::JSON_VALUE_FLOAT);
                return value->floating;
            } else if constexpr (std::is_same_v<T, double>) {
                RUNTIME_ASSERT(value->type == JsonValueType::JSON_VALUE_FLOAT);
                return value->floating;
            } else if constexpr (std::is_same_v<T, bool>) {
                RUNTIME_ASSERT(value->type == JsonValueType::JSON_VALUE_BOOL);
                return value->boolean;
            } else if constexpr (std::is_same_v<T, char*> || std::is_same_v<T, const char*>) {
                RUNTIME_ASSERT(value->type == JsonValueType::JSON_VALUE_STRING);
                return value->string.data;
            } else if constexpr (std::is_same_v<T, View<char>>) {
                RUNTIME_ASSERT(value->type == JsonValueType::JSON_VALUE_STRING);
                return value->string;
            } else if constexpr (std::is_same_v<T, JSON*>) {
                RUNTIME_ASSERT(
                    value->type == JsonValueType::JSON_VALUE_OBJECT ||
                    value->type == JsonValueType::JSON_VALUE_ARRAY  ||
                    value->type == JsonValueType::JSON_VALUE_NULL
                );
                return value;
            }
        }

        /*
        TODO(Jovanni): Think about this some more because I can't actually do the unstable
        swapback if I want to keep the ordering correct...
        void remove(const char* key) {
            RUNTIME_ASSERT(this->has(key));

            int index = this->object.keys.get(key);

            const char* replacement_key = this->object.pairs[this->object.pairs.count() - 1].key;
            this->object.keys.put(replacement_key, index);

            this->object.pairs.unstable_swapback_remove(index);
        }
        */

        static JSON* Object(Allocator allocator);
        static JSON* Array(Allocator allocator);
        static JSON* Null(Allocator allocator);

        template<SupportedType T>
        constexpr JSON* MAKE_JSON_VALUE(Allocator allocator, T value) {
            if constexpr (std::is_same_v<T, int>) {
                return JSON::Integer(allocator, value);
            } else if constexpr (std::is_same_v<T, float>) {
                return JSON::Floating(allocator, value);
            } else if constexpr (std::is_same_v<T, double>) {
                return JSON::Floating(allocator, value);
            } else if constexpr (std::is_same_v<T, bool>) {
                return JSON::Boolean(allocator, value);
            } else if constexpr (std::is_same_v<T, char*> || std::is_same_v<T, const char*>) {
                return JSON::String(allocator, value);
            } else if constexpr (std::is_same_v<T, View<char>>) {
                return JSON::String(allocator, value);
            } else if constexpr (std::is_same_v<T, JSON*>) {
                return value;
            }

            RUNTIME_ASSERT(false);

            return nullptr;
        }

        template<SupportedType T>
        void push(const char* key, T value) {
            RUNTIME_ASSERT(this->type == JSON_VALUE_OBJECT);

            this->object.push(key, MAKE_JSON_VALUE(this->allocator, value));
        }

        template<SupportedType T>
        void array_push(T value) {
            RUNTIME_ASSERT(this->type == JSON_VALUE_ARRAY);
        
            vector_append(&this->array.elements, MAKE_JSON_VALUE(this->allocator, value));
        }

        static const char* ToString(JSON* root, const char* indent = "    ");
        static JSON* Parse(Allocator allocator, const char* json_string, u64 json_string_length);

        static JSON* Integer(Allocator allocator, int value);
        static JSON* Floating(Allocator allocator, float value);
        static JSON* Boolean(Allocator allocator, bool value);
        static JSON* String(Allocator allocator, View<char> value);
        static JSON* String(Allocator allocator, View<const char> value);
        static JSON* String(Allocator allocator, char* str);
        static JSON* String(Allocator allocator, const char* str);
    };
#endif

//
// =====================================================================================================================
// ====================================================== CORE_IMPL ====================================================
// =====================================================================================================================
//

#if defined(CORE_TYPES_IMPL)
    #include <cstdlib>

    const char* error_get_string(Error error_code) {
        LOCAL_PERSIST const char* ERROR_STRINGS[(int)Error::COUNT] = {
            STRINGIFY(ERROR_SUCCESS),
            STRINGIFY(ERROR_RESOURCE_NOT_FOUND),
            STRINGIFY(ERROR_RESOURCE_TOO_BIG),
            STRINGIFY(ERROR_NULL_PARAMETER),
            STRINGIFY(ERROR_INVALID_PARAMETER),
        };

        RUNTIME_ASSERT(((int)error_code >= 0) && ((int)error_code < (int)Error::COUNT));

        return ERROR_STRINGS[(int)error_code];
    }

    INTERNAL_FUNCTION void* memory_general_allocate(void* ctx, size_t allocation_size) {
        UNUSED(ctx);
        return std::malloc(allocation_size);
    }

    INTERNAL_FUNCTION void* memory_general_realloc(void* ctx, void* data, size_t old_allocation_size, size_t new_allocation_size) {
        UNUSED(ctx);
        return std::realloc(data, new_allocation_size);
    }

    INTERNAL_FUNCTION void memory_general_free(void* ctx, void* data) {
        UNUSED(ctx);
        std::free(data);
    }

    Allocator allocator_invalid() {
        Allocator ret = {};
        return ret;
    }

    Allocator allocator_general() {
        Allocator ret = {};
        ret.malloc = memory_general_allocate;
        ret.realloc = memory_general_realloc;
        ret.free = memory_general_free;

        return ret;
    }

    Timer timer_create() {
        Timer timer = {};
        timer.should_tick = false;
        timer.elapsed = 0.0;
        timer.duration = 0.0;

        return timer;
    }

    void timer_start(Timer* timer, float duration) {
        timer->should_tick = true;
        timer->elapsed = 0.0f;
        timer->duration = duration;
    }

    bool timer_tick(Timer* timer, float dt) {
        if (!timer->should_tick) return false;

        timer->elapsed = timer->elapsed + dt;
        return timer->elapsed <= timer->duration;
    }

    void timer_stop(Timer* timer) {
        timer->should_tick = false;
        timer->elapsed = 0.0f;
        timer->duration = 0.0f;
    }
#endif

#if defined(CORE_LOGGING_IMPL)
    void LOG_OUTPUT(LogLevel log_level, const char* message, ...) {
        #define OUT_MESSAGE_BUFFER_LENGTH KB(4)
        char out_message[OUT_MESSAGE_BUFFER_LENGTH] = {0};

        va_list args;
        va_start(args, message);
            va_list args_copy;
            va_copy(args_copy, args);
                size_t out_message_size = (size_t)vsnprintf(nullptr, 0, message, args_copy) + 1; // +1 for null terminator
                RUNTIME_ASSERT(out_message_size < OUT_MESSAGE_BUFFER_LENGTH);
            va_end(args_copy);

            va_copy(args_copy, args);
                vsnprintf(out_message, out_message_size, message, args_copy);
            va_end(args_copy); 
        u64 out_message_length = out_message_size - 1;
        va_end(args);

        const char* log_level_format[LOG_LEVEL_COUNT] = {
            RED_BACKGROUND,
            COLOR_RED,
            COLOR_YELLOW,
            COLOR_GREEN,
            COLOR_BLUE,
            COLOR_RESET
        };

        bool found = out_message[out_message_length - 1] == '\n';
        printf("%s%.*s%s", log_level_format[log_level], (int)(out_message_length - found), out_message, COLOR_RESET);
        if (found) {
            printf("\n");
        }
    }
#endif

#if defined(CORE_ASSERT_IMPL)
    void MACRO_RUNTIME_ASSERT(bool expression, const char* function, const char* file, int line) {
        if (!expression) {                             
            //stack_trace_dump(function, file, line);                               
            char msg[] = "Func: %s, File: %s:%d\n";          
            LOG_FATAL(msg, function, file, line);
            CRASH();
        }                                                         
    }

    void MACRO_RUNTIME_ASSERT_MSG(bool expression, const char* function, const char* file, int line, const char* msg, ...) {  
        if (!(expression)) {            
            #define BUFFER_LENGTH 256
            char message[BUFFER_LENGTH] = {0};

            va_list args;
            va_start(args, msg);
                va_list args_copy;
                va_copy(args_copy, args);
                    size_t message_size = (size_t)vsnprintf(nullptr, 0, msg, args_copy) + 1; // +1 for null terminator
                    RUNTIME_ASSERT(message_size < BUFFER_LENGTH);
                va_end(args_copy);

                va_copy(args_copy, args);
                    vsnprintf(message, message_size, msg, args_copy);
                va_end(args_copy); 
            va_end(args);   

            u64 message_length = message_size - 1;

            // stack_trace_dump(function, file, line);
            LOG_FATAL("%.*s", message_length, message);                                 
            char func_file_msg[] = "Func: %s, File: %s:%d\n";          
            LOG_FATAL(func_file_msg, function, file, line);

            CRASH();                                                
        }                                                                                    
    }
#endif

#if defined(CORE_HASHING_IMPL)
    // Original location:
    // https://github.com/majek/csiphash/1
    #if defined(_WIN32)
        #define _le64toh(x) ((u64)(x))
    #elif defined(__APPLE__)
        #include <libkern/OSByteOrder.h>
        #define _le64toh(x) OSSwapLittleToHostInt64(x)
    #else
        #if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
            #include <sys/endian.h>
        #else
            #include <endian.h>
        #endif

        #if defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && __BYTE_ORDER == __LITTLE_ENDIAN   
            #define _le64toh(x) ((u64)(x))
        #else
            #define _le64toh(x) le64toh(x)
        #endif
    #endif

    namespace Hashing {
        #define ROTATE(x, b) (u64)( ((x) << (b)) | ( (x) >> (64 - (b))) )

        #define HALF_ROUND(a,b,c,d,s,t)	\
            a += b; c += d;			    \
            b = ROTATE(b, s) ^ a;		\
            d = ROTATE(d, t) ^ c;		\
            a = ROTATE(a, 32);          \

        #define DOUBLE_ROUND(v0,v1,v2,v3)  \
            HALF_ROUND(v0,v1,v2,v3,13,16); \
            HALF_ROUND(v2,v1,v0,v3,17,21); \
            HALF_ROUND(v0,v1,v2,v3,13,16); \
            HALF_ROUND(v2,v1,v0,v3,17,21)

        u64 zero_hash(const void* source, size_t source_size) {
            UNUSED(source);
            UNUSED(source_size);
            return 0;
        }

        u64 siphash(const void* source, size_t source_size) {
            const char key[16] = {
                0x00, 0x01, 0x02, 0x03,
                0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0A, 0x0B,
                0x0C, 0x0D, 0x0E, 0x0F
            };

            const u64 *_key = (const u64*)key;
            u64 k0 = _le64toh(_key[0]);
            u64 k1 = _le64toh(_key[1]);
            u64 b = source_size << 56;
            const u64* in = (const u64*)source;

            u64 v0 = k0 ^ 0x736f6d6570736575ULL;
            u64 v1 = k1 ^ 0x646f72616e646f6dULL;
            u64 v2 = k0 ^ 0x6c7967656e657261ULL;
            u64 v3 = k1 ^ 0x7465646279746573ULL;

            while (source_size >= 8) {
                u64 mi = _le64toh(*in);
                in += 1; source_size -= 8;
                v3 ^= mi;
                DOUBLE_ROUND(v0,v1,v2,v3);
                v0 ^= mi;
            }

            u64 t = 0; 
            u8* pt = (u8*)&t; 
            const u8* m = (const u8*)in;
            switch (source_size) {
                case 7: pt[6] = m[6];
                case 6: pt[5] = m[5];
                case 5: pt[4] = m[4];
                case 4: *((u32*)&pt[0]) = *((u32*)&m[0]); break;
                case 3: pt[2] = m[2];
                case 2: pt[1] = m[1];
                case 1: pt[0] = m[0];
            }
            b |= _le64toh(t);

            v3 ^= b;
            DOUBLE_ROUND(v0,v1,v2,v3);
            v0 ^= b; v2 ^= 0xff;
            DOUBLE_ROUND(v0,v1,v2,v3);
            DOUBLE_ROUND(v0,v1,v2,v3);

            u64 ret = (v0 ^ v1) ^ (v2 ^ v3);
            return ret;
        }

        u64 cstring_hash(const void* str, size_t str_length) {
            UNUSED(str_length);

            u64 hash = 5381;
            u8* str_ptr = (u8*)str;
            int c;

            while ((c = *str_ptr++)) {
                hash = ((hash << 5) + hash) + c;
            }

            return hash;
        }

        bool cstring_equality(const void* c1, size_t c1_size, const void* c2, size_t c2_size) {
            UNUSED(c1_size);
            UNUSED(c2_size);

            u64 c1_length = str_length((const char*)c1);
            u64 c2_length = str_length((const char*)c2);
            
            return str_equal((const char*)c1, c1_length, (const char*)c2, c2_length);
        }

        u64 string_view_hash(const void* view, size_t str_length) {
            UNUSED(str_length);
            View<char>* str_view = (View<char>*)view;
            u64 hash = 5381;
            int c;

            for (u64 i = 0; i < str_view->length; i++) {
                c = str_view->data[i];
                hash = ((hash << 5) + hash) + c;
            }

            return hash;
        }

        bool string_view_equality(const void* c1, size_t c1_size, const void* c2, size_t c2_size) {
            UNUSED(c1_size);
            UNUSED(c2_size);

            View<char>* s1 = (View<char>*)c1;
            View<char>* s2 = (View<char>*)c2;

            return *s1 == *s2;
        }
    }
#endif

#if defined(CORE_MEMORY_IMPL)
    void memory_zero(void* data, size_t data_size_in_bytes) {
        RUNTIME_ASSERT(data);
        
        for (size_t i = 0; i < data_size_in_bytes; i++) {
            ((u8*)data)[i] = 0;
        }
    }

    void memory_copy(void* destination, size_t destination_size, const void* source, size_t source_size) {
        RUNTIME_ASSERT(source);
        RUNTIME_ASSERT(destination);
        RUNTIME_ASSERT(source_size <= destination_size);
        if (source_size == 0) {
            return;
        }

        u8* src = (u8*)source;
        u8* dst = (u8*)destination;

        bool overlap = dst < src || dst >= src + source_size;
        if (overlap) {
            for (size_t i = 0; i < source_size; i++) {
                dst[i] = src[i];
            }
        } else {
            for (size_t i = source_size; i-- > 0;) {
                dst[i] = src[i];
            }
        }
    }

    bool memory_equal(const void* buffer_one, size_t b1_size, const void* buffer_two, size_t b2_size) {
        RUNTIME_ASSERT(buffer_one);
        RUNTIME_ASSERT(buffer_two);

        if (b1_size != b2_size) {
            return false;
        }

        u8* buffer_one_data = (u8*)buffer_one;
        u8* buffer_two_data = (u8*)buffer_two;
        for (size_t i = 0; i < b1_size; i++) {
            if (buffer_one_data[i] != buffer_two_data[i]) {
                return false;
            }
        }

        return true;
    }
#endif

#if defined(CORE_STRING_IMPL)
    char* str_allocate(Allocator a, const char* s1, u64 length) {
        char* ret = (char*)a.malloc(a.ctx, length + 1);
        memory_copy(ret, length, s1, length);

        return ret;
    }

    char* str_sprintf(Allocator a, u64* out_buffer_length, const char* fmt, ...) {
        va_list args, copy_args;
        va_start(args, fmt);
            va_copy(copy_args, args);
            u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, copy_args) + 1; // +1 for null terminator
            va_end(copy_args);

            char* buffer = (char*)a.malloc(a.ctx, allocation_ret);

            va_copy(copy_args, args);
            vsnprintf(buffer, allocation_ret, fmt, copy_args);
            va_end(copy_args);
        va_end(args);

        if (out_buffer_length) {
            *out_buffer_length = allocation_ret - 1;
        }
       
        return buffer;
    }
    
    char* str_sprintf(Allocator a, u64* out_buffer_length, const char* fmt, va_list args) {
        va_list args_copy;
        va_copy(args_copy, args);
        u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for null terminator
        va_end(args_copy);

        char* buffer = (char*)a.malloc(a.ctx, allocation_ret);

        va_copy(args_copy, args);
        vsnprintf(buffer, allocation_ret, fmt, args_copy);
        va_end(args_copy);

        if (out_buffer_length) {
            *out_buffer_length = allocation_ret - 1;
        }
        
        return buffer;
    }

    void str_sprintf(char* buffer, size_t buffer_capacity, u64* out_buffer_length, const char* fmt, ...) {
        va_list args, copy_args;
        va_start(args, fmt);
            va_copy(copy_args, args);
                u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, copy_args) + 1; // +1 for null terminator
                RUNTIME_ASSERT(allocation_ret < buffer_capacity);
            va_end(copy_args);

            va_copy(copy_args, args);
                vsnprintf(buffer, allocation_ret, fmt, copy_args);
            va_end(copy_args);
        va_end(args);

        if (out_buffer_length) {
            *out_buffer_length = allocation_ret - 1;
        }
    }

    void str_sprintf(char* buffer, size_t buffer_capacity, u64* out_buffer_length, const char* fmt, va_list args) {
        va_list args_copy;
        va_copy(args_copy, args);
        u64 allocation_ret = (u64)vsnprintf(nullptr, 0, fmt, args_copy) + 1; // +1 for null terminator
        RUNTIME_ASSERT(allocation_ret < buffer_capacity);
        va_end(args_copy);

        va_copy(args_copy, args);
        vsnprintf((char*)buffer, allocation_ret, fmt, args_copy);
        va_end(args_copy);

        if (out_buffer_length) {
            *out_buffer_length = allocation_ret - 1;
        }
    }

    u64 str_length(const char* c_string) {
        u64 ret = 0;
        while (*c_string++ != '\0') {
            ret += 1;
        } 

        return ret;
    }

    bool str_equal(const char* s1, u64 s1_length, const char* s2, u64 s2_length) {
        return memory_equal(s1, s1_length, s2, s2_length);
    }

    bool str_equal(const char* s1, const char* s2) {
        return memory_equal(s1, str_length(s1), s2, str_length(s2));
    }

    s64 str_index_of(const char* str, u64 str_length, const char* substring, u64 substring_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(substring);

        if (str_length == 0 && substring_length == 0) {
            return 0;
        } else if (substring_length == 0) {
            return -1;
        } else if (str_length == 0) {
            return -1;
        }

        if (substring_length > str_length) {
            return -1;
        }
        
        s64 ret_index = -1;
        for (u64 i = 0; i <= str_length - substring_length; i++) {
            if (ret_index != -1) {
                break;
            }
            
            if (str[i] != substring[0]) {
                continue;
            }

            View<char> current_view = view_create(str + i, substring_length);
            if (str_equal(substring, substring_length, current_view.data, current_view.length)) {
                ret_index = (s64)i;
                break;
            }
        }

        return ret_index; // returns -1 if not found
    }

    bool str_contains(const char* str, u64 str_length, const char* contains, u64 contains_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(contains);

        return str_index_of(str, str_length, contains, contains_length) != -1;  
    }

    s64 str_last_index_of(const char* str, u64 str_length, const char* substring, u64 substring_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(substring);

        if (str_length == 0 && substring_length == 0) {
            return 0;
        } else if (substring_length == 0) {
            return -1;
        } else if (str_length == 0) {
            return -1;
        }

        if (substring_length > str_length) {
            return -1;
        }
        
        s64 ret_index = -1;
        for (u64 i = 0; i <= (str_length - substring_length); i++) {
            if (str[i] != substring[0]) {
                continue;
            }

            View<char> current_view = view_create(str + i, substring_length);
            if (str_equal(current_view.data, current_view.length, substring, substring_length)) {
                ret_index = (s64)i;
            }
        }

        return ret_index; // returns -1 if not found
    }

    bool str_starts_with(const char* str, u64 str_length, const char* starts_with, u64 starts_with_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(starts_with);

        if (str_length == 0 && starts_with_length == 0) {
            return true;
        } else if (str[0] != starts_with[0] || str_length < starts_with_length) {
            return false;
        }
        
        if (str_equal(str, starts_with_length, starts_with, starts_with_length)) {
            return true;
        }

        return false;
    }

    bool str_ends_with(const char* str, u64 str_length, const char* ends_with, u64 ends_with_length) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(ends_with);

        if ((str_length == 0) || (ends_with_length == 0)) {
            return true;
        } else if (str_length < ends_with_length) {
            return false;
        }

        u64 start_index = MAX(str_length - 1, 0) - MAX(ends_with_length - 1, 0);
        if (str[start_index] != ends_with[0]) {
            return false;
        }

        if (str_equal(str + start_index, ends_with_length, ends_with, ends_with_length)) {
            return true;
        }

        return false;
    }

    void str_copy(char* s1, size_t s1_capacity, const char* s2, u64 s2_length) {
        memory_zero(s1, s1_capacity);
        memory_copy(s1, s1_capacity, s2, s2_length);
    }

    void str_insert(char* str, u64 &str_length_out, size_t str_capacity, const char* to_insert, u64 to_insert_length, u64 index) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(to_insert);

        u64 new_length = str_length_out + to_insert_length;
        RUNTIME_ASSERT_MSG(new_length < str_capacity, "str_insert: str_capacity is %lld but new valid cstring capacity is %d + %d + 1(null_term) = %d\n", str_capacity, str_length_out, to_insert_length, new_length + 1);
        
        u8* move_source_ptr = (u8*)(str + index);
        u8* move_dest_ptr = (u8*)(move_source_ptr + to_insert_length);

        memory_copy(move_dest_ptr, str_capacity - (index + to_insert_length), move_source_ptr, str_length_out - index);
        u8* copy_dest_ptr = (u8*)(str + index);
        memory_copy(copy_dest_ptr, str_capacity, to_insert, to_insert_length);

        str_length_out += to_insert_length;
    }

    void str_insert(char* str, u64& str_length_out, size_t str_capacity, char to_insert, u64 index) {
        RUNTIME_ASSERT(str);
        RUNTIME_ASSERT(to_insert);

        u64 to_insert_length = 1;
        bool expression = (str_length_out + to_insert_length) < str_capacity;
        RUNTIME_ASSERT_MSG(expression, "str_insert: str overflow new_capacity_required: %d >= current_capacity: %lld\n", str_length_out + to_insert_length, str_capacity);

        char* source_ptr = str + index;
        memory_copy(source_ptr + 1, str_capacity - (index + 1), source_ptr, str_length_out - index);
        str[index] = to_insert;

        str_length_out += 1;
    }

    void str_append(char* str, u64 &out_str_length, size_t str_capacity, const char* to_append, u64 to_append_length) {
        str_insert(str, out_str_length, str_capacity, to_append, to_append_length, out_str_length);
    }

    void str_append(char* str, u64 &out_str_length, size_t str_capacity, View<char> to_append) {
        str_insert(str, out_str_length, str_capacity, to_append.data, to_append.length, out_str_length);
    }

    void str_append(char* str, u64 &out_str_length, size_t str_capacity, View<const char> to_append) {
        str_insert(str, out_str_length, str_capacity, to_append.data, to_append.length, out_str_length);
    }

    void str_append(char* str, u64 &out_str_length, size_t str_capacity, char to_append) {
        str_insert(str, out_str_length, str_capacity, to_append, out_str_length);
    }
#endif

#if defined(CORE_RANDOM_IMPL)
    const int UPPER_MASK       = 0x80000000;
    const int LOWER_MASK       = 0x7fffffff;
    const int TEMPERING_MASK_B = 0x9d2c5680;
    const int TEMPERING_MASK_C = 0xefc60000;
    u32 MAX_MASK = (u32)0xffffffff;

    inline INTERNAL_FUNCTION void _seeding(Seed* r, u32 seed) {
        r->mt[0] = seed & MAX_MASK;
        while (r->index++ < STATE_VECTOR_LENGTH) {
            r->mt[r->index] = (6069 * r->mt[r->index-1]) & MAX_MASK;
        }
    }

    Seed random_generate_seed(u32 seed) {
        Seed rand;
        _seeding(&rand, seed);
        return rand;
    }

    u32 random_generate_u32(Seed* r) {
        u32 y;
        LOCAL_PERSIST u32 mag[2] = {0x0, 0x9908b0df};
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

    float random_generate_f32(Seed* r) {
        u32 value = random_generate_u32(r);
        return (float)value / (float)MAX_MASK;
    }

    float random_generate_range(Seed* r, float min, float max) {
        float value = random_generate_u32(r);
        return remap(value, 0, 1, min, max);
    }
#endif

#if defined(CORE_MATH_IMPL)
    float lerp(float a, float b, float t) {
        return a + ((b - a) * t);
    }

    float inverse_lerp(float a, float b, float value) {
        if (NEAR_ZERO(a - b)) {
            return 0.0f; // Avoid division by zero
        }

        return (value - a) / (b - a);
    }

    float remap(float x, float s_min, float s_max, float e_min, float e_max) {
        x = CLAMP(x, s_min, s_max);
        float s_ratio = (x - s_min) / (s_max - s_min);
        
        return e_min + (s_ratio * (e_max - e_min));
    }

    float move_toward(float current, float target, float delta) {
        float diff = target - current;

        if (fabsf(diff) <= delta) {
            return target;
        }

        return current + (diff > 0 ? delta : -delta);
    }

    int mod(int a, int b) {
        int remainder = a % b;
        return (remainder < 0) ? (remainder + b) : remainder;
    }

    Vec2 vec2_create(float fill) {
        Vec2 ret = {};
        ret.x = fill;
        ret.y = fill;
        
        return ret;
    }

    Vec2 vec2_create(float x, float y) {
        Vec2 ret = {};
        ret.x = x;
        ret.y = y;
        
        return ret;
    }

    float Vec2::magnitude() const {
        return sqrtf((this->x * this->x) + (this->y * this->y));
    }

    float Vec2::magnitude_squared() const {
        return (this->x * this->x) + (this->y * this->y);
    }

    Vec2 Vec2::normalize() const {
        Vec2 ret(0, 0);
        const float magnitude = this->magnitude();
        if (magnitude == 0) {
            return vec2_create(0,0);
        }

        ret.x = this->x / magnitude;
        ret.y = this->y / magnitude;

        return ret;
    }

    Vec2 Vec2::scale(float scale) const {
        return vec2_create(this->x * scale, this->y * scale);
    }

    Vec2 Vec2::scale(Vec2 s) const {
        return vec2_create(this->x * s.x, this->y * s.y);
    }

    Vec2 Vec2::scale(float scale_x, float scale_y) const {
        return vec2_create(this->x * scale_x, this->y * scale_y);
    }

    Vec2 Vec2::operator+(const Vec2 &right) const {
        return vec2_create(this->x + right.x, this->y + right.y);
    }
    Vec2& Vec2::operator+=(const Vec2 &right) {
        this->x += right.x;
        this->y += right.y;

        return *this;
    }

    Vec2 Vec2::operator-(const Vec2 &right) const {
        return vec2_create(this->x - right.x, this->y - right.y);
    }
    Vec2& Vec2::operator-=(const Vec2 &right) {
        this->x -= right.x;
        this->y -= right.y;

        return *this;
    }

    Vec2 Vec2::operator*(const Vec2 &right) const {
        return vec2_create(this->x * right.x, this->y * right.y);
    }

    Vec2& Vec2::operator*=(const Vec2 &right) {
        this->x *= right.x;
        this->y *= right.y;

        return *this;
    }

    Vec2 Vec2::operator/(const Vec2 &right) const {
        return vec2_create(this->x / right.x, this->y / right.y);
    }
    Vec2& Vec2::operator/=(const Vec2 &right) {
        this->x /= right.x;
        this->y /= right.y;

        return *this;
    }

    bool Vec2::operator==(const Vec2 &right) const {
        return NEAR_ZERO(this->x - right.x) && NEAR_ZERO(this->y - right.y);
    }

    bool Vec2::operator!=(const Vec2 &right) const {
        return !(*this == right);
    }

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
    float vec2_dot(Vec2 a, Vec2 b) {
        return (a.x * b.x) + (a.y * b.y);
    }

    Vec2 vec2_lerp(Vec2 a, Vec2 b, float t) {
        Vec2 ab = (b - a);
        return a + (ab.scale(t));
    }

    float vec2_distance(Vec2 a, Vec2 b) {
        return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y));
    }

    float vec2_distance_squared(Vec2 a, Vec2 b) {
        return SQUARED(b.x - a.x) + SQUARED(b.y - a.y);
    }

    Vec2 vec2_move_toward(Vec2 a, Vec2 b, float dt) {
        Vec2 ret = vec2_create(0);
        ret.x = move_toward(a.x, b.x, dt);
        ret.y = move_toward(a.y, b.y, dt);
        
        return ret;
    }

    Vec2 vec2_closest(Vec2 a, Vec2 b, Vec2 target) {
        float a_distance = vec2_distance_squared(a, target);
        float b_distance = vec2_distance_squared(b, target);

        if (a_distance <= b_distance) {
            return a;
        }

        return b;
    }

    Vec2 vec2_euler(float yaw, float pitch) {
        Vec2 ret = vec2_create(0, 0);
        ret.x = cosf(DEGREES_TO_RAD(yaw)) * cosf(DEGREES_TO_RAD(pitch));
        ret.y = sinf(DEGREES_TO_RAD(pitch));

        return ret;
    }

    // ----

    Vec3 vec3_create(float fill) {
        Vec3 ret = {};
        ret.x = fill;
        ret.y = fill;
        ret.z = fill;

        return ret;
    }

    Vec3 vec3_create(float x, float y, float z) {
        Vec3 ret = {};
        ret.x = x;
        ret.y = y;
        ret.z = z;

        return ret;
    }

    Vec3 vec3_create(Vec2 v, float z) {
        Vec3 ret = {};
        ret.x = v.x;
        ret.y = v.y;
        ret.z = z;

        return ret;
    }

    Vec3 vec3_create(Vec4 v) {
        Vec3 ret = {};
        ret.x = v.x;
        ret.y = v.y;
        ret.z = v.z;

        return ret;
    }

    float Vec3::magnitude() const {
        return sqrtf(SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z));
    }

    float Vec3::magnitude_squared() const {
        return SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z);
    }

    Vec3 Vec3::normalize() const {
        const float magnitude = this->magnitude();
        if (magnitude == 0) {
            return vec3_create(0);
        }

        Vec3 ret = vec3_create(0);
        ret.x = this->x / magnitude;
        ret.y = this->y / magnitude;
        ret.z = this->z / magnitude;

        return ret;
    }

    Vec3 Vec3::scale(float scale) const {
        return vec3_create(this->x * scale, this->y * scale, this->z * scale);
    }

    Vec3 Vec3::scale(Vec3 s) const {
        return vec3_create(this->x * s.x, this->y * s.y, this->z * s.z);
    }

    Vec3 Vec3::scale(float scale_x, float scale_y, float scale_z) const {
        return vec3_create(this->x * scale_x, this->y * scale_y, this->z * scale_z);
    }

    Vec3 Vec3::operator+(const Vec3 &right) const {
        return vec3_create(this->x + right.x, this->y + right.y, this->z + right.z);
    }
    Vec3& Vec3::operator+=(const Vec3 &right) {
        this->x += right.x;
        this->y += right.y;
        this->z += right.z;

        return *this;
    }

    Vec3 Vec3::operator-(const Vec3 &right) const {
        return vec3_create(this->x - right.x, this->y - right.y, this->z - right.z);
    }
    Vec3& Vec3::operator-=(const Vec3 &right) {
        this->x -= right.x;
        this->y -= right.y;
        this->z -= right.z;

        return *this;
    }


    Vec3 Vec3::operator*(const Vec3 &right) const {
        return vec3_create(this->x * right.x, this->y * right.y, this->z * right.z);
    }
    Vec3& Vec3::operator*=(const Vec3 &right) {
        this->x *= right.x;
        this->y *= right.y;
        this->z *= right.z;

        return *this;
    }


    Vec3 Vec3::operator/(const Vec3 &right) const {
        return vec3_create(this->x / right.x, this->y / right.y, this->z / right.z);
    }
    Vec3& Vec3::operator/=(const Vec3 &right) {
        this->x /= right.x;
        this->y /= right.y;
        this->z /= right.z;

        return *this;
    }

    bool Vec3::operator==(const Vec3 &right) const {
        return NEAR_ZERO(this->x - right.x) && NEAR_ZERO(this->y - right.y) && NEAR_ZERO(this->z - right.z);
    }
    bool Vec3::operator!=(const Vec3 &right) const {
        return !(*this == right);
    }

    float vec3_dot(Vec3 a, Vec3 b) {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
    }

    Vec3 vec3_lerp(Vec3 a, Vec3 b, float t) {
        Vec3 ab = (b - a);
        return a + (ab.scale(t));
    }

    float vec3_distance(Vec3 a, Vec3 b) {
        return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y) - SQUARED(b.z - a.z));
    }

    float vec3_distance_squared(Vec3 a, Vec3 b) {
        return SQUARED(b.x - a.x) + SQUARED(b.y - a.y) - SQUARED(b.z - a.z);
    }

    Vec3 vec3_closest(Vec3 a, Vec3 b, Vec3 target) {
        float a_distance = vec3_distance_squared(a, target);
        float b_distance = vec3_distance_squared(b, target);

        if (a_distance <= b_distance) {
            return a;
        }

        return b;
    }

    Vec3 vec3_euler(float yaw, float pitch) {
        Vec3 ret = vec3_create(0, 0, 0);
        ret.x = cosf(DEGREES_TO_RAD(yaw)) * cosf(DEGREES_TO_RAD(pitch));
        ret.y = sinf(DEGREES_TO_RAD(pitch));
        ret.z = sinf(DEGREES_TO_RAD(yaw)) * cosf(DEGREES_TO_RAD(pitch));

        return ret;
    }

    Vec3 vec3_cross(Vec3 A, Vec3 B) {
        Vec3 ret = vec3_create(0);
        ret.x = A.y * B.z - A.z * B.y;
        ret.y = A.z * B.x - A.x * B.z;
        ret.z = A.x * B.y - A.y * B.x;

        return ret;
    }

    Vec3 vec3_move_toward(Vec3 a, Vec3 b, float dt) {
        Vec3 ret = vec3_create(0);
        ret.x = move_toward(a.x, b.x, dt);
        ret.y = move_toward(a.y, b.y, dt);
        ret.z = move_toward(a.z, b.z, dt);
        
        return ret;
    }

    // ---

    Vec4 vec4_create(float fill) {
        Vec4 ret = {};
        ret.x = fill;
        ret.y = fill;
        ret.z = fill;
        ret.w = fill;

        return ret;
    }

    Vec4 vec4_create(float x, float y, float z, float w) {
        Vec4 ret = {};
        ret.x = x;
        ret.y = y;
        ret.z = z;
        ret.w = w;

        return ret;
    }

    Vec4 vec4_create(Vec3 v, float w) {
        Vec4 ret = {};
        ret.x = v.x;
        ret.y = v.y;
        ret.z = v.z;
        ret.w = w;

        return ret;
    }

    float Vec4::magnitude() const {
        return sqrtf(SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w));
    }

    float Vec4::magnitude_squared() const {
        return SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w);
    }

    Vec4 Vec4::normalize() const {
        const float magnitude = this->magnitude();
        if (magnitude == 0) {
            return vec4_create(0);
        }

        Vec4 ret = vec4_create(0);
        ret.x = this->x / magnitude;
        ret.y = this->y / magnitude;
        ret.z = this->z / magnitude;
        ret.w = this->w / magnitude;

        return ret;
    }

    Vec4 Vec4::scale(float scale) const {
        return vec4_create(this->x * scale, this->y * scale, this->z * scale, this->w * scale);
    }

    Vec4 Vec4::scale(Vec4 s) const {
        return vec4_create(this->x * s.x, this->y * s.y, this->z * s.z, this->w * s.w);
    }

    Vec4 Vec4::scale(float scale_x, float scale_y, float scale_z, float scale_w) const {
        return vec4_create(this->x * scale_x, this->y * scale_y, this->z * scale_z, this->w * scale_w);
    }

    Vec4 Vec4::operator+(const Vec4 &right) const {
        return vec4_create(this->x + right.x, this->y + right.y, this->z + right.z, this->w + right.w);
    }
    Vec4& Vec4::operator+=(const Vec4 &right) {
        this->x += right.x;
        this->y += right.y;
        this->z += right.z;
        this->w += right.w;

        return *this;
    }

    Vec4 Vec4::operator-(const Vec4 &right) const {
        return vec4_create(this->x - right.x, this->y - right.y, this->z - right.z, this->w - right.w);
    }
    Vec4& Vec4::operator-=(const Vec4 &right) {
        this->x -= right.x;
        this->y -= right.y;
        this->z -= right.z;
        this->w -= right.w;

        return *this;
    }

    Vec4 Vec4::operator*(const Vec4 &right) const {
        return vec4_create(this->x * right.x, this->y * right.y, this->z * right.z, this->w * right.w);
    }
    Vec4& Vec4::operator*=(const Vec4 &right) {
        this->x *= right.x;
        this->y *= right.y;
        this->z *= right.z;
        this->w *= right.w;

        return *this;
    }

    Vec4 Vec4::operator/(const Vec4 &right) const {
        return vec4_create(this->x / right.x, this->y / right.y, this->z / right.z, this->w / right.w);
    }
    Vec4& Vec4::operator/=(const Vec4 &right) {
        this->x /= right.x;
        this->y /= right.y;
        this->z /= right.z;
        this->w /= right.w;

        return *this;
    }

    bool Vec4::operator==(const Vec4 &right) const {
        return NEAR_ZERO(this->x - right.x) && NEAR_ZERO(this->y - right.y) && NEAR_ZERO(this->z - right.z) && NEAR_ZERO(this->w - right.w);
    }
    bool Vec4::operator!=(const Vec4 &right) const {
        return !(*this == right);
    }

    float vec4_dot(Vec4 a, Vec4 b) {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    }

    Vec4 vec4_lerp(Vec4 a, Vec4 b, float t) {
        Vec4 ab = (b - a);
        return a + (ab.scale(t));
    }

    float vec4_distance(Vec4 a, Vec4 b) {
        return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y) - SQUARED(b.z - a.z) - SQUARED(b.w - a.w));
    }

    float vec4_distance_squared(Vec4 a, Vec4 b) {
        return SQUARED(b.x - a.x) + SQUARED(b.y - a.y) - SQUARED(b.z - a.z) - SQUARED(b.w - a.w);
    }

    Vec4 vec4_closest(Vec4 a, Vec4 b, Vec4 target) {
        float a_distance = vec4_distance_squared(a, target);
        float b_distance = vec4_distance_squared(b, target);

        if (a_distance <= b_distance) {
            return a;
        }

        return b;
    }

    Vec4 vec4_move_toward(Vec4 a, Vec4 b, float dt) {
        Vec4 ret = vec4_create(0);
        ret.x = move_toward(a.x, b.x, dt);
        ret.y = move_toward(a.y, b.y, dt);
        ret.z = move_toward(a.z, b.z, dt);
        ret.w = move_toward(a.w, b.w, dt);
        
        return ret;
    }

    // ---

    IVec4 ivec4_create(int fill) {
        IVec4 ret = {};
        ret.x = fill;
        ret.y = fill;
        ret.z = fill;
        ret.w = fill;

        return ret;
    }

    IVec4 ivec4_create(int x, int y, int z, int w) {
        IVec4 ret = {};
        ret.x = x;
        ret.y = y;
        ret.z = z;
        ret.w = w;

        return ret;
    }

    float IVec4::magnitude() const {
        return sqrtf(SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w));
    }

    float IVec4::magnitude_squared() const {
        return SQUARED(this->x) + SQUARED(this->y) + SQUARED(this->z) + SQUARED(this->w);
    }

    IVec4 IVec4::normalize() const {
        const float magnitude = this->magnitude();
        if (magnitude == 0) {
            return ivec4_create(0);
        }

        IVec4 ret = ivec4_create(0);
        ret.x = this->x / magnitude;
        ret.y = this->y / magnitude;
        ret.z = this->z / magnitude;
        ret.w = this->w / magnitude;

        return ret;
    }

    IVec4 IVec4::scale(int scale) const {
        return ivec4_create(this->x * scale, this->y * scale, this->z * scale, this->w * scale);
    }

    IVec4 IVec4::scale(IVec4 s) const {
        return ivec4_create(this->x * s.x, this->y * s.y, this->z * s.z, this->w * s.w);
    }

    IVec4 IVec4::scale(int scale_x, int scale_y, int scale_z, int scale_w) const {
        return ivec4_create(this->x * scale_x, this->y * scale_y, this->z * scale_z, this->w * scale_w);
    }

    IVec4 IVec4::operator+(const IVec4 &right) const {
        return ivec4_create(this->x + right.x, this->y + right.y, this->z + right.z, this->w + right.w);
    }
    IVec4& IVec4::operator+=(const IVec4 &right) {
        this->x += right.x;
        this->y += right.y;
        this->z += right.z;
        this->w += right.w;

        return *this;
    }

    IVec4 IVec4::operator-(const IVec4 &right) const {
        return ivec4_create(this->x - right.x, this->y - right.y, this->z - right.z, this->w - right.w);
    }
    IVec4& IVec4::operator-=(const IVec4 &right) {
        this->x -= right.x;
        this->y -= right.y;
        this->z -= right.z;
        this->w -= right.w;

        return *this;
    }


    IVec4 IVec4::operator*(const IVec4 &right) const {
        return ivec4_create(this->x * right.x, this->y * right.y, this->z * right.z, this->w * right.w);
    }
    IVec4& IVec4::operator*=(const IVec4 &right) {
        this->x *= right.x;
        this->y *= right.y;
        this->z *= right.z;
        this->w *= right.w;

        return *this;
    }

    IVec4 IVec4::operator/(const IVec4 &right) const {
        return ivec4_create(this->x / right.x, this->y / right.y, this->z / right.z, this->w / right.w);
    }
    IVec4& IVec4::operator/=(const IVec4 &right) {
        this->x /= right.x;
        this->y /= right.y;
        this->z /= right.z;
        this->w /= right.w;

        return *this;
    }

    bool IVec4::operator==(const IVec4 &right) const {
        return NEAR_ZERO(this->x - right.x) && NEAR_ZERO(this->y - right.y) && NEAR_ZERO(this->z - right.z) && NEAR_ZERO(this->w - right.w);
    }
    bool IVec4::operator!=(const IVec4 &right) const {
        return !(*this == right);
    }

    float ivec4_dot(IVec4 a, IVec4 b) {
        return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    }

    IVec4 ivec4_lerp(IVec4 a, IVec4 b, float t) {
        IVec4 ab = (b - a);
        return a + (ab.scale(t));
    }

    float ivec4_distance(IVec4 a, IVec4 b) {
        return sqrtf(SQUARED(b.x - a.x) + SQUARED(b.y - a.y) - SQUARED(b.z - a.z) - SQUARED(b.w - a.w));
    }

    float ivec4_distance_squared(IVec4 a, IVec4 b) {
        return SQUARED(b.x - a.x) + SQUARED(b.y - a.y) - SQUARED(b.z - a.z) - SQUARED(b.w - a.w);
    }

    IVec4 ivec4_closest(IVec4 a, IVec4 b, IVec4 target) {
        float a_distance = ivec4_distance_squared(a, target);
        float b_distance = ivec4_distance_squared(b, target);

        if (a_distance <= b_distance) {
            return a;
        }

        return b;
    }

    // --

    Mat4 mat4_create(Vec4 r0, Vec4 r1, Vec4 r2, Vec4 r3) {
        Mat4 ret = {};
        ret.v[0] = r0;
        ret.v[1] = r1;
        ret.v[2] = r2;
        ret.v[3] = r3;
        
        return ret;
    }

    Mat4 mat4_create(const float row_major[16]) {
        Mat4 ret = {};
        ret.v[0] = vec4_create(row_major[0],  row_major[1],  row_major[2],  row_major[3]);
        ret.v[1] = vec4_create(row_major[4],  row_major[5],  row_major[6],  row_major[7]);
        ret.v[2] = vec4_create(row_major[8],  row_major[9],  row_major[10], row_major[11]);
        ret.v[3] = vec4_create(row_major[12], row_major[13], row_major[14], row_major[15]);

        return ret;
    }

    Mat4 mat4_create(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ) {
        Mat4 ret = {};
        ret.v[0] = vec4_create(m00, m01, m02, m03);
        ret.v[1] = vec4_create(m10, m11, m12, m13);
        ret.v[2] = vec4_create(m20, m21, m22, m23);
        ret.v[3] = vec4_create(m30, m31, m32, m33);

        return ret;
    }

    Mat4 mat4_from_column_major(const float mat[16]) {
        Mat4 ret = mat4_create(
            vec4_create(mat[0], mat[4], mat[8], mat[12]),
            vec4_create(mat[1], mat[5], mat[9], mat[13]),
            vec4_create(mat[2], mat[6], mat[10], mat[14]),
            vec4_create(mat[3], mat[7], mat[11], mat[15])
        );

        return ret;
    }

    Mat4 mat4_identity() {
        Mat4 ret;
        ret.v[0] = vec4_create(1, 0, 0, 0);
        ret.v[1] = vec4_create(0, 1, 0, 0);
        ret.v[2] = vec4_create(0, 0, 1, 0);
        ret.v[3] = vec4_create(0, 0, 0, 1);

        return ret;
    }

    Mat4 mat4_scale(Mat4 mat, float scale) {
        return mat4_scale(mat, vec3_create(scale, scale, scale));
    }

    Mat4 mat4_scale(Mat4 mat, Vec3 s) {
        Mat4 scale_matrix = mat4_create(
            vec4_create(s.x,  0.0f, 0.0f, 0.0f),
            vec4_create(0.0f, s.y,  0.0f, 0.0f),
            vec4_create(0.0f, 0.0f, s.z,  0.0f),
            vec4_create(0.0f, 0.0f, 0.0f, 1.0f) 
        );

        return scale_matrix * mat;
    }

    Mat4 mat4_scale(Mat4 mat, float scale_x, float scale_y, float scale_z) {
        return mat4_scale(mat, vec3_create(scale_x, scale_y, scale_z));
    }

    Mat4 mat4_rotate(Mat4 mat, float theta, Vec3 axis) {
        float rad = DEGREES_TO_RAD(theta);
        float c = cosf(rad);
        float s = sinf(rad);
        float t = 1.0f - c;

        axis = axis.normalize();
        float x = axis.x;
        float y = axis.y;
        float z = axis.z;

        Mat4 rot = mat4_create(
            vec4_create(t * x * x + c,     t * x * y - z * s, t * x * z + y * s, 0.0f),
            vec4_create(t * x * y + z * s, t * y * y + c,     t * y * z - x * s, 0.0f),
            vec4_create(t * x * z - y * s, t * y * z + x * s, t * z * z + c,     0.0f),
            vec4_create(0.0f,              0.0f,              0.0f,              1.0f)
        );

        return rot * mat;
    }

    Mat4 mat4_rotate(Mat4 mat, float theta, float rot_x, float rot_y, float rot_z) {
        return mat4_rotate(mat, theta, vec3_create(rot_x, rot_y, rot_z));
    }

    Mat4 mat4_rotate(Mat4 mat, Quat quat) {
        float theta;
        Vec3 axis;
        quat.angle_axis(theta, axis);
        
        return mat4_rotate(mat, theta, axis);
    }

    Mat4 mat4_translate(Mat4 mat, Vec3 t) {
        Mat4 translate_matrix = mat4_create(
            vec4_create(1.0f, 0.0f, 0.0f, t.x),
            vec4_create(0.0f, 1.0f, 0.0f, t.y),
            vec4_create(0.0f, 0.0f, 1.0f, t.z),
            vec4_create(0.0f, 0.0f, 0.0f, 1.0f)
        );

        return translate_matrix * mat;
    }

    Mat4 mat4_translate(Mat4 mat, float x, float y, float z) {
        return mat4_translate(mat, vec3_create(x, y, z));
    }

    Mat4 mat4_transform(Vec3 s, float theta, Vec3 axis, Vec3 t) {
        Mat4 scale_matrix = mat4_scale(mat4_identity(), s);
        Mat4 rotation_matrix = mat4_rotate(mat4_identity(), theta, axis);
        Mat4 translation_matrix = mat4_translate(mat4_identity(), t);

        return translation_matrix * rotation_matrix * scale_matrix;
    }

    Mat4 mat4_translate(Vec3 s, Quat r, Vec3 t) {
        Mat4 scale_matrix = mat4_scale(mat4_identity(), s);
        Mat4 rotation_matrix = mat4_rotate(mat4_identity(), r);
        Mat4 translation_matrix = mat4_translate(mat4_identity(), t);

        return translation_matrix * rotation_matrix * scale_matrix;
    }

    Mat4 mat4_inverse_transform(Vec3 s, float theta, Vec3 axis, Vec3 t) {
        Mat4 inverse_scale_matrix = mat4_scale(mat4_identity(), s.scale(1 / s.x, 1 / s.y, 1 / s.z));
        Mat4 inverse_rotation_matrix = mat4_rotate(mat4_identity(), theta, axis).transpose();
        Mat4 inverse_translation_matrix = mat4_translate(mat4_identity(), t.scale(-1));

        return inverse_scale_matrix * inverse_rotation_matrix * inverse_translation_matrix;
    }

    void mat4_decompose(Mat4 mat, Vec3* out_position, Quat* out_orientation, Vec3* out_scale) {
        Vec3 translation = vec3_create(mat.v[0].w, mat.v[1].w, mat.v[2].w);
        Vec3 scale = vec3_create(0);
        {
            Vec3 column1 = vec3_create(mat.v[0].x, mat.v[1].x, mat.v[2].x);
            Vec3 column2 = vec3_create(mat.v[0].y, mat.v[1].y, mat.v[2].y);
            Vec3 column3 = vec3_create(mat.v[0].z, mat.v[1].z, mat.v[2].z);
        
            float scale_x = column1.magnitude();
            float scale_y = column2.magnitude();
            float scale_z = column3.magnitude();
            scale = vec3_create(scale_x, scale_y, scale_z);
        }

        Quat orientation = quat_identity();
        {
            Vec3 column1 = vec3_create(mat.v[0].x, mat.v[1].x, mat.v[2].x);
            Vec3 column2 = vec3_create(mat.v[0].y, mat.v[1].y, mat.v[2].y);
            Vec3 column3 = vec3_create(mat.v[0].z, mat.v[1].z, mat.v[2].z);
            
            column1 = column1.scale(1.0f / scale.x);
            column2 = column2.scale(1.0f / scale.y);
            column3 = column3.scale(1.0f / scale.z);
            
            Mat4 rotation_matrix = mat4_create(
                vec4_create(column1.x, column2.x, column3.x, 0),
                vec4_create(column1.y, column2.y, column3.y, 0),
                vec4_create(column1.z, column2.z, column3.z, 0),
                vec4_create(0,         0,         0,         0)
            );
            orientation = quat_from_rotation_matrix(rotation_matrix);

            /*
            // TRS = M
            // R = T⁻¹ * M * S⁻¹
            // R = T⁻¹ * (TRS) * S⁻¹
            // R = T⁻¹ * TR * I
            // R = I * R * I
            // R = R
            Vec3 inverse_scale = vec3_create(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);
            Mat4 inverse_scale_matrix = mat4_scale(mat4_identity(), inverse_scale);
            Mat4 inverse_translation_matrix = mat4_translate(mat4_identity(), translation.scale(-1));
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

    Mat4 mat4_perspective(float fov_degrees, float aspect, float near_plane, float far_plane) {
        float fov_radians = DEGREES_TO_RAD(fov_degrees);

        const float t = tanf(fov_radians / 2) * near_plane;
        const float b = -t;
        const float r = t * aspect;
        const float l = -t * aspect;

        const float p = (2.0f * near_plane);
        const float A = p / (r - l);
        const float B = p / (t - b);
        const float C = -((far_plane + near_plane) / (far_plane - near_plane));
        const float D = -((p * far_plane) / (far_plane - near_plane));

        Mat4 ret = mat4_create(
            vec4_create(A,  0,  0,  0),
            vec4_create(0,  B,  0,  0),
            vec4_create(0,  0,  C,  D),
            vec4_create(0,  0, -1,  0)
        );

        return ret;
    }

    // Found at: https://en.wikipedia.org/wiki/Orthographic_projection
    Mat4 mat4_orthographic(float left, float right, float bottom, float top, float near_plane, float far_plane) {
        const float A = 2.0f / (right - left);
        const float B = 2.0f / (top - bottom);
        const float C = -2.0f / (far_plane - near_plane);
        const float D = -(right + left) / (right - left);
        const float E = -(top + bottom) / (top - bottom);
        const float F = -(far_plane + near_plane) / (far_plane - near_plane);

        Mat4 ret = mat4_create(
            A,  0,  0,  D,
            0,  B,  0,  E,
            0,  0,  C,  F,
            0,  0,  0,  1
        );

        return ret;
    }

    // Found at: https://www.khronos.org/opengl/wiki/GluLookAt_code
    Mat4 mat4_lookat(Vec3 position, Vec3 target, Vec3 world_up) {
        Vec3 forward = (position - target).normalize();
        Vec3 right   = vec3_cross(world_up, forward).normalize();
        Vec3 up      = vec3_cross(forward, right).normalize();

        Mat4 rotation;
        rotation.v[0] = vec4_create(right, 0);
        rotation.v[1] = vec4_create(up, 0);
        rotation.v[2] = vec4_create(forward, 0);
        rotation.v[3] = vec4_create(0, 0, 0, 1);

        Mat4 translation = mat4_translate(mat4_identity(), position.scale(-1));
        return rotation * translation;
    }

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

    #define DETERMINATE_HELPER(a, b, c, d, e, f, g, h, i) (a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g))
    Mat4 Mat4::inverse(bool &success) const {
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
            return mat4_identity();
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

    Mat4 Mat4::operator*(const Mat4 &right) const {
        Mat4 C = {};

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

    Vec4 Mat4::operator*(const Vec4 &right) const  {
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

    Mat4& Mat4::operator*=(const Mat4 &right) {
        *this = *this * right;
        return *this;
    }

    bool Mat4::operator==(const Mat4 &right) const {
        return (this->v[0] == right.v[0]) && (this->v[1] == right.v[1]) && (this->v[2] == right.v[2]) && (this->v[3] == right.v[3]);
    }

    bool Mat4::operator!=(const Mat4 &right) const {
        return !(*this == right);
    }

    void Mat4::print() const {
        LOG_TRACE(
            "\n"
            "-----------------------------\n"
            "[ %.2f | %.2f | %.2f | %.2f ]\n"
            "[ %.2f | %.2f | %.2f | %.2f ]\n"
            "[ %.2f | %.2f | %.2f | %.2f ]\n"
            "[ %.2f | %.2f | %.2f | %.2f ]\n"
            "-----------------------------\n",
            this->v[0].x, this->v[0].y, this->v[0].z, this->v[0].w,
            this->v[1].x, this->v[1].y, this->v[1].z, this->v[1].w,
            this->v[2].x, this->v[2].y, this->v[2].z, this->v[2].w,
            this->v[3].x, this->v[3].y, this->v[3].z, this->v[3].w
        );
    }

    // --

    AABB aabb_create(Vec3 min, Vec3 max) {
        AABB ret;
        ret.min = min;
        ret.max = max;

        return ret;
    }

    AABB aabb_create(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) {
        AABB ret;
        ret.min.x = min_x;
        ret.min.y = min_y;
        ret.min.z = min_z;

        ret.max.x = max_x;
        ret.max.y = max_y;
        ret.max.z = max_z;

        return ret;
    }

    AABB aabb_from_center_extents(Vec3 center, Vec3 extents) {
        return aabb_create(center - extents, center + extents);
    }

    bool aabb_check_line_intersection(AABB aabb, Vec3 p0, Vec3 p1) {
        float tmin = -10000;
        float tmax = 10000;
    
        if (fabsf(p1.x) > EPSILON) {
            float t1 = (aabb.min.x - p0.x) / p1.x;
            float t2 = (aabb.max.x - p0.x) / p1.x;
    
            tmin = MAX(tmin, MIN(t1, t2));
            tmax = MIN(tmax, MAX(t1, t2));
        }
    
        if (fabs(p1.y) > EPSILON) {
            float t1 = (aabb.min.y - p0.y) / p1.y;
            float t2 = (aabb.max.y - p0.y) / p1.y;
    
            tmin = MAX(tmin, MIN(t1, t2));
            tmax = MIN(tmax, MAX(t1, t2));
        }
    
        if (fabs(p1.z) > EPSILON) {
            float t1 = (aabb.min.z - p0.z) / p1.z;
            float t2 = (aabb.max.z - p0.z) / p1.z;
    
            tmin = MAX(tmin, MIN(t1, t2));
            tmax = MIN(tmax, MAX(t1, t2));
        }
    
        if (tmax > tmin && tmax > 0.0) {
            return true;
        }
            
        return false;
    }

    Vec3 AABB::center() const {
        Vec3 extents = this->extents();
        return vec3_create(min.x + extents.x, min.y + extents.y, min.z + extents.z);
    }

    Vec3 AABB::extents() const {
        return (this->max - this->min).scale(0.5f);
    }

    // --

    Quat quat_create(float theta, Vec3 axis) {
        Quat ret;

        float radians = DEGREES_TO_RAD(theta);
        ret.w = cosf(radians / 2.0f);
        if (NEAR_ZERO(ret.w)) {
            ret.w = 0.0f;
        }

        ret.v = axis.normalize().scale(sinf(radians / 2.0f));
        return ret;
    }

    Quat quat_create(float theta, float x, float y, float z) {
        return quat_create(theta, vec3_create(x, y, z));
    }

    Quat Quat::inverse() const {
        Quat ret = quat_create(1, 0, 0, 0);

        float magnitude_squared = SQUARED(this->w) + vec3_dot(this->v, this->v);
        if (magnitude_squared == 0.0f) { 
            return quat_identity();
        }

        ret.w = this->w / magnitude_squared;
        ret.v = this->v.scale(-1.0f / magnitude_squared);

        return ret;
    }

    Quat Quat::scale(float scale) const {
        Quat ret;

        ret.w   = this->w   * scale;
        ret.v.x = this->v.x * scale;
        ret.v.y = this->v.y * scale;
        ret.v.z = this->v.z * scale;

        return ret;
    }

    Quat Quat::normalize() const {
        Vec4 temp = vec4_create(this->w, this->v.x, this->v.y, this->v.z).normalize();
        
        Quat ret;
        ret.w = temp.x;
        ret.v.x = temp.y;
        ret.v.y = temp.z;
        ret.v.z = temp.w;

        return ret;
    }

    Mat4 Quat::to_rotation_matrix() const {
        Mat4 result = mat4_identity();

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

    void Quat::angle_axis(float &theta, Vec3 &vec) const {
        Quat quat = this->normalize();
        float sinf_half_theta = quat.v.magnitude();

        if (sinf_half_theta < EPSILON) {
            vec = vec3_create(1, 0, 0);
        } else {
            vec = quat.v.scale(1.0f / sinf_half_theta);
        }

        // Clamp w to [-1, 1] to avoid NaNs due to precision issues
        float w = CLAMP(quat.w, -1.0f, 1.0f);
        theta = 2.0f * acosf(w);
        theta = RAD_TO_DEGREES(theta);
    }

    Quat Quat::operator+(const Quat &right) const {
        Quat ret;
        ret.w = this->w + right.w;
        ret.v = this->v + right.v;

        return ret;
    }
    Quat& Quat::operator+=(const Quat &right) {
        *this = *this + right;
        return *this;
    }

    Quat Quat::operator-(const Quat &right) const {
        Quat ret;
        ret.w = this->w - right.w;
        ret.v = this->v - right.v;

        return ret;
    }
    Quat& Quat::operator-=(const Quat &right) {
        *this = *this - right;
        return *this;
    }

    Quat Quat::operator*(const Quat &right) const {
        Quat ret;
        ret.w = (this->w * right.w) - vec3_dot(this->v, right.v);
        ret.v = (this->v.scale(right.w) + right.v.scale(this->w)) + vec3_cross(this->v, right.v);
        
        return ret;
    }
    Quat& Quat::operator*=(const Quat &right) {
        *this = *this * right;
        return *this;
    }

    Vec3 Quat::operator*(const Vec3 &right) const {
        Quat q = *this;
        Quat p = {0, right};

        return ((q * p) * q.inverse()).v;
    }

    bool Quat::operator==(const Quat &right) const {
        return NEAR_ZERO(this->w - right.w) && (this->v == right.v);
    }
    bool Quat::operator!=(const Quat &right) const {
        return !(*this == right);
    }

    Quat quat_identity() {
        Quat ret;
        ret.w = 1;
        ret.v = vec3_create(0);

        return ret;
    }

    Quat quat_from_euler(float theta_x, float theta_y, float theta_z) {
        return quat_from_euler(vec3_create(theta_x, theta_y, theta_z));
    }

    Quat quat_from_euler(Vec3 euler_angles_degrees) {
        float roll_rad_half = DEGREES_TO_RAD(euler_angles_degrees.x) * 0.5f;
        float pitch_rad_half = DEGREES_TO_RAD(euler_angles_degrees.y) * 0.5f;
        float yaw_rad_half = DEGREES_TO_RAD(euler_angles_degrees.z) * 0.5f;

        float cx = cosf(roll_rad_half);
        float sx = sinf(roll_rad_half);
        float cy = cosf(pitch_rad_half);
        float sy = sinf(pitch_rad_half);
        float cz = cosf(yaw_rad_half);
        float sz = sinf(yaw_rad_half);

        Quat q = quat_identity();
        q.w = cx * cy * cz + sx * sy * sz;
        q.v.x = sx * cy * cz - cx * sy * sz;
        q.v.y = cx * sy * cz + sx * cy * sz;
        q.v.z = cx * cy * sz - sx * sy * cz;

        return q;
    }

    Quat quat_from_angle_axis(float angle, Vec3 axis) {
        float half_angle = DEGREES_TO_RAD(angle) * 0.5f;
        float sinf_half = sinf(half_angle);
        float cosf_half = cosf(half_angle);

        Quat q = quat_identity();
        axis = axis.normalize();

        q.w     = cosf_half;
        q.v.x   = axis.x * sinf_half;
        q.v.y   = axis.y * sinf_half;
        q.v.z   = axis.z * sinf_half;

        return q;
    }

    Quat quat_from_rotation_matrix(const float m[16]) {
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

    Quat quat_from_rotation_matrix(Mat4 mat) {
        return quat_from_rotation_matrix(&mat.v[0].x);
    }

    Quat quat_slerp(Quat q, Quat r, float t) {
        q = q.normalize();
        r = r.normalize();

        float dot = quat_dot(q, r);
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

    float quat_dot(Quat a, Quat b) {
        float dot = (
            a.w   * b.w   +
            a.v.x * b.v.x +
            a.v.y * b.v.y +
            a.v.z * b.v.z
        );

        return dot;
    }
#endif

#if defined(CORE_MEMORY_ARENA_IMPL)
    Arena arena_create(void* memory, size_t allocation_size, int flags, u8 alignment) {
        Arena ret = {};

        RUNTIME_ASSERT_MSG(memory, "Memory can't be a null pointer!\n");
        RUNTIME_ASSERT_MSG(allocation_size != 0, "Can't have a zero allocation size!\n");
        int mask = ARENA_FLAG_FIXED | ARENA_FLAG_CIRCULAR;
        RUNTIME_ASSERT_MSG((flags & mask) != mask, "Can't have both a fixed and circular arena!\n");

        ret.flags = flags;
        ret.used = 0;
        ret.capacity = allocation_size;
        ret.alignment = alignment;
        ret.base_address = (u8*)memory;

        Allocator general_allocator = allocator_general();
        ret.stack_sizes = stack_create<size_t>(general_allocator);
        ret.temp_used_stack = stack_create<size_t>(general_allocator);

        return ret;
    }

    void* arena_push(void* arena_ctx, size_t allocation_size) {
        Arena* arena = (Arena*)arena_ctx;
        RUNTIME_ASSERT_MSG(arena->base_address, "Allocator is invalid!\n");
        RUNTIME_ASSERT_MSG(allocation_size != 0, "Element size can't be zero!\n");

        if (arena->flags & ARENA_FLAG_FIXED) {
            RUNTIME_ASSERT_MSG(arena->used + allocation_size <= arena->capacity, "Ran out of arena memory!\n");
        } else if (arena->flags & ARENA_FLAG_CIRCULAR) {
            if ((arena->used + allocation_size > arena->capacity)) {
                arena->used = 0;
                RUNTIME_ASSERT_MSG(arena->used + allocation_size <= arena->capacity, "Element size exceeds circular arena allocation capacity!\n");
            }
        }

        u8* ret = arena->base_address + arena->used;
        size_t previous_used = arena->used;
        arena->used += allocation_size;
        if ((arena->used & (arena->alignment - 1)) != 0) {
            arena->used += (arena->alignment - (arena->used & (arena->alignment - 1)));
        }

        stack_push(&arena->stack_sizes, arena->used - previous_used);

        return ret;
    }

    void arena_pop(Arena* arena, size_t to_pop) {
        arena->used -= to_pop;
    }

    static bool arena_is_data_poppable(Arena* arena, void* data) {
        size_t bytes_to_pop = stack_peek(&arena->stack_sizes);
        return arena->base_address + (arena->used - bytes_to_pop) == data;
    }


    void arena_free(void* arena_ctx, void* data) {
        Arena* arena = (Arena*)arena_ctx;
        RUNTIME_ASSERT(data);

        if (arena_is_data_poppable(arena, data)) {
            arena->used -= stack_pop(&arena->stack_sizes);
        }
    }

    void* arena_realloc(void* arena_ctx, void* data, size_t old_allocation_size, size_t new_allocation_size) {
        Arena* arena = (Arena*)arena_ctx;
        RUNTIME_ASSERT(arena);
        RUNTIME_ASSERT(old_allocation_size != 0);
        RUNTIME_ASSERT(new_allocation_size != 0);

        if (arena_is_data_poppable(arena, data)) {
            arena_pop(arena, old_allocation_size);
            return arena_push(arena, new_allocation_size);
        }

        void* ret = arena_push(arena, new_allocation_size);
        memory_copy(ret, new_allocation_size, data, old_allocation_size);

        return ret;
    }

    void arena_begin_temp(Arena* arena) {
        stack_push(&arena->temp_used_stack, arena->used);
    }

    void arena_end_temp(Arena* arena) {
        arena->used = stack_pop(&arena->temp_used_stack);
    }

    Allocator arena_to_allocator(Arena* arena) {
        Allocator allocator = {};
        allocator.ctx = arena;
        allocator.malloc = arena_push;
        allocator.realloc = arena_realloc;
        allocator.free = arena_free;

        return allocator;
    }
#endif

#if defined(CORE_PLATFORM_IMPL)
    #if defined(PLATFORM_WINDOWS)
        #include <windowsx.h>
        #include <timeapi.h>

        double g_frequency = {0};
        double g_start_time = {0};
        TIMECAPS g_device_time_caps;

        bool platform_initialize() {
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency(&frequency);
            g_frequency = (double)frequency.QuadPart;

            LARGE_INTEGER start_time;
            QueryPerformanceCounter(&start_time);
            g_start_time = (double)start_time.QuadPart;
            
            if (timeGetDevCaps(&g_device_time_caps, sizeof(TIMECAPS)) != MMSYSERR_NOERROR) {
                MessageBoxA(0, "Failed to get timecaps!", "Error!", MB_ICONEXCLAMATION | MB_OK);

                return false;
            }

            LOG_DEBUG("Platform initalization!\n");
            return true;
        }

        void platform_shutdown() {
            timeEndPeriod(g_device_time_caps.wPeriodMin);
            LOG_DEBUG("Platform Shutdown!\n");
        }

        bool platform_file_exists(const char* path) {
            return (GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES);
        }

        bool platform_copy_file(const char* source_path, const char* dest_path, bool block_until_success) {
            if (block_until_success) {
                while (!CopyFileA(source_path, dest_path, FALSE)) {
                    platform_sleep(10);
                }

                return true;
            } 

            return CopyFileA(source_path, dest_path, FALSE) == 0;
        }

        void platform_sleep(u32 ms) {
            Sleep((DWORD)ms);
        }

        double platform_get_seconds_elapsed() {
            LARGE_INTEGER counter = {0};
            QueryPerformanceCounter(&counter);
            double time_in_seconds = (double)(counter.QuadPart - g_start_time) / (double)g_frequency;
        
            return time_in_seconds;
        }

        u8* platform_read_entire_file(Allocator a, const char* file_path, size_t& out_file_size, Error& error) {
            HANDLE file_handle = CreateFileA(
                file_path, 
                GENERIC_READ, 
                FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 
                nullptr, 
                OPEN_EXISTING, 
                FILE_ATTRIBUTE_NORMAL, 
                nullptr
            );
            if (file_handle == INVALID_HANDLE_VALUE) {
                LOG_ERROR("CreateFileA() returned an INVALID_HANDLE_VALUE, the file_path/path is likely wrong: platform_read_entire_file(%s)\n", file_path);
                error = Error::RESOURCE_NOT_FOUND;
                
                return nullptr;
            }

            LARGE_INTEGER large_int;
            memory_zero(&large_int, sizeof(LARGE_INTEGER));
            BOOL success = GetFileSizeEx(file_handle, &large_int);
            if (!success) {
                LOG_ERROR("GetFileSizeEx() Failed to get size from file_handle: platform_read_entire_file(%s)\n", file_path);
                error = Error::RESOURCE_NOT_FOUND;
                CloseHandle(file_handle);
                return nullptr;
            }

            size_t file_size = (size_t)large_int.QuadPart;
            if (file_size > SIZE_MAX) {
                LOG_ERROR("File size is bigger than max size: platform_read_entire_file(%s)\n", file_path);
                error = Error::RESOURCE_TOO_BIG;
                CloseHandle(file_handle);

                return nullptr;
            }

            u8* file_data = (u8*)a.malloc(a.ctx, file_size + 1); // +1 for null-terminator

            DWORD bytes_read = 0;
            success = ReadFile(file_handle, file_data, (DWORD)file_size, &bytes_read, nullptr);
            CloseHandle(file_handle);
            if (!success && bytes_read == file_size) {
                LOG_ERROR("ReadFile() Failed to get the file data or bytes read doesn't match file_size: platform_read_entire_file(%s)\n", file_path);
                error = Error::RESOURCE_NOT_FOUND;
                a.free(a.ctx, file_data);
                return nullptr;
            }

            out_file_size = (size_t)file_size;

            return file_data;
        }

        DLL* platform_load_dll(const char* dll_path, Error& error)  {
            HMODULE library = LoadLibraryA(dll_path);
            if (!library) {
                LOG_ERROR("LoadLibraryA() failed: platform_load_dll(%s)\n", dll_path);
                error = Error::RESOURCE_NOT_FOUND;

                return nullptr;
            }

            return library;
        }

        DLL* platform_free_dll(DLL* dll, Error& error)  {
            RUNTIME_ASSERT(dll);
            FreeLibrary((HMODULE)dll);

            return nullptr;
        }

        void* platform_get_function_address(DLL* dll, const char* proc_name, Error& error) {
            RUNTIME_ASSERT(dll);

            void* proc = (void*)GetProcAddress((HMODULE)dll, proc_name);
            if (!proc) {
                LOG_ERROR("GetProcAddress() failed: platform_get_function_address(%s)\n", proc_name);
                error = Error::RESOURCE_NOT_FOUND;
                return nullptr;
            }

            return proc;
        }

        FileTime platform_get_file_modification_time(const char* file_path, Error& error) {
            HANDLE file_handle = CreateFileA(
                file_path, 
                GENERIC_READ, 
                FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 
                nullptr, 
                OPEN_EXISTING, 
                FILE_ATTRIBUTE_NORMAL, 
                nullptr
            );

            FileTime ret = {0};
            if (file_handle == INVALID_HANDLE_VALUE) {
                LOG_ERROR("CreateFileA() returned an INVALID_HANDLE_VALUE, the file_path/path is likely wrong: platform_get_file_modification_time(%s)\n", file_path);
                error = Error::RESOURCE_NOT_FOUND;
                
                return ret;
            }
            
            if(!GetFileTime(file_handle, nullptr, nullptr, &ret.time)) { 
                CloseHandle(file_handle);
                LOG_ERROR("Failed to get file modification time\n");
                error = Error::RESOURCE_NOT_FOUND;

                return ret;
            }

            CloseHandle(file_handle);
            return ret;
        }

        bool platform_compare_file_modification_time(FileTime a, FileTime b) {
            return CompareFileTime(&a.time, &b.time);
        }

        INTERNAL_FUNCTION void* win32_malloc(const void* ctx, size_t allocation_size) {
            UNUSED(ctx);
            return VirtualAlloc(nullptr, allocation_size, MEM_COMMIT, PAGE_READWRITE);
        }

        INTERNAL_FUNCTION void win32_free(const void* ctx, void* data) {
            UNUSED(ctx);
            VirtualFree(data, 0, MEM_RELEASE);
        }

        INTERNAL_FUNCTION void* win32_realloc(const void* ctx, void* data, size_t old_allocation_size, size_t new_allocation_size) {
            UNUSED(ctx);
            void* ret = VirtualAlloc(nullptr, new_allocation_size, MEM_COMMIT, PAGE_READWRITE);
            memory_copy(ret, new_allocation_size, data, old_allocation_size);
            win32_free(data);

            return ret;
        }

        Allocator platform_allocator() {
            Allocator allocator = {};
            allocator.malloc = win32_malloc;
            allocator.realloc = win32_realloc;
            allocator.free = win32_free;

            return allocator;
        }
    #elif defined(PLATFORM_LINUX) || defined(PLATFORM_APPLE)
        #include <unistd.h>
        #include <dlfcn.h>
        #include <stdio.h>
        #include <sys/stat.h>
        
        bool platform_initialize() { return true; }
        void platform_shutdown() {}
        double platform_get_seconds_elapsed() { return 0.0; }

        void platform_sleep(u32 ms) {
            usleep(ms * 1000);
        }

        bool platform_file_exists(const char* path) {
            FILE *fptr = fopen(path, "r");

            if (fptr == nullptr) {
                return false;
            }

            fclose(fptr);

            return true;
        }

        bool platform_copy_file(const char* source_path, const char* dest_path, int block_until_success) {
            const size_t BUFFER_SIZE = 4096;

            while (true) {
                FILE* in = fopen(source_path, "rb");
                if (!in) {
                    if (!block_until_success) return false;
                    platform_sleep(10);
                    continue;
                }

                FILE* out = fopen(dest_path, "wb");
                if (!out) {
                    fclose(in);
                    if (!block_until_success) return false;
                    platform_sleep(10);
                    continue;
                }

                char buffer[BUFFER_SIZE];
                size_t bytes;
                int success = 1;

                while ((bytes = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
                    if (fwrite(buffer, 1, bytes, out) != bytes) {
                        success = 0;
                        break;
                    }
                }

                fclose(in);
                fclose(out);

                if (success) return true;
                if (!block_until_success) return false;
                platform_sleep(10);
            }
        }

        u8* platform_read_entire_file(Allocator a, const char* file_name, size_t& out_file_size, Error& error) {
            FILE* file_handle = fopen(file_name, "r");
            if (file_handle == nullptr) {
                LOG_ERROR("Invalid file_handle, the file_name/path is likely wrong: platform_read_entire_file(%s)\n", file_name);
                error =Error::RESOURCE_NOT_FOUND;

                return nullptr;
            }

            if (fseek(file_handle, 0L, SEEK_END) != 0) {
                LOG_ERROR("fseek failed: platform_read_entire_file(%s)\n", file_name);
                error = Error::RESOURCE_NOT_FOUND;
                fclose(file_handle);
                return nullptr;
            }

            out_file_size = ftell(file_handle);
            if (out_file_size == -1L) {
                LOG_ERROR("ftell failed: platform_read_entire_file(%s)\n", file_name);
                error = Error::RESOURCE_NOT_FOUND;
                fclose(file_handle);
                return nullptr;
            }

            rewind(file_handle);
            if (ferror(file_handle)) {
                LOG_ERROR("rewind() failed: platform_read_entire_file(%s)\n", file_name);
                error = Error::RESOURCE_NOT_FOUND;
                fclose(file_handle);
                
                return nullptr;
            }

            u8* file_data = (u8*)a.malloc(a.ctx, (size_t)out_file_size + 1); // +1 for null terminator
            if (fread(file_data, out_file_size, 1, file_handle) != 1) {
                LOG_ERROR("fread() failed: platform_read_entire_file(%s)\n", file_name);
                error = Error::RESOURCE_NOT_FOUND;
                a.free(a.ctx, file_data);
                fclose(file_handle);

                return nullptr;
            }

            fclose(file_handle);

            out_file_size = out_file_size + 1;

            return file_data;
        }

        DLL* platform_load_dll(const char* dll_path, Error& error)  {
            DLL* library = dlopen(dll_path, RTLD_LAZY);
            if (!library) {
                LOG_ERROR("dlopen() failed: platform_load_dll(%s)\n", dll_path);
                error = Error::RESOURCE_NOT_FOUND;

                return nullptr;
            }

            return library;
        }

        DLL* platform_free_dll(DLL* dll, Error& error)  {
            RUNTIME_ASSERT(dll);
            dlclose(dll);

            return nullptr;
        }

        void* platform_get_function_address(DLL* dll, const char* proc_name, Error& error) {
            RUNTIME_ASSERT(dll);

            void* proc = dlsym(dll, proc_name);
            if (!proc) {
                LOG_ERROR("dlsym() failed: platform_get_function_address(%s)\n", proc_name);
                error = Error::RESOURCE_NOT_FOUND;

                return nullptr;
            }

            return proc;
        }

        FileTime platform_get_file_modification_time(const char* file_path, Error& error) {
            FileTime file_time = {}; 

            if (platform_file_exists(file_path)) {
                LOG_ERROR("file_name/path is likely wrong: platform_get_file_modification_time(%s)\n", file_path);
                error = Error::RESOURCE_NOT_FOUND;
                return file_time;
            }

            struct stat attr;
            if (stat(file_path, &attr) == -1) {
                LOG_ERROR("Error getting file stats\n");
                error = Error::RESOURCE_NOT_FOUND;
                return file_time;
            }

            // NOTE(Jovanni): attr.st_mtime holds the last modification time
            time_t file_timestamp = attr.st_mtime;
            file_time.time = file_timestamp;
            return file_time;
        }

        bool platform_compare_file_modification_time(FileTime a, FileTime b) {
            return a.time == b.time;
        }

        Allocator platform_allocator() {
            return allocator_general();
        }
    #endif
#endif