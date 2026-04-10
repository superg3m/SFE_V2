#include "../Common/sfe_common.hpp"

namespace Hashing {
    u64 zeroHash(const void* source, std::size_t source_size);
    u64 siphash24(const void* source, std::size_t source_size);
    u64 cstringHash(const void* str, std::size_t str_length);
    bool cstringEquality(const void* c1, std::size_t c1_size, const void* c2, std::size_t c2_size);
    u64 stringViewHash(const void* view, u64 str_length);
    bool stringViewEquality(const void* c1, std::size_t c1_size, const void* c2, std::size_t c2_size);
}