#pragma once

#include "../Common/sfe_common.hpp"
#include "sfe_allocator.hpp"

namespace Memory {
    void Zero(void* data, size_t data_size_in_bytes);
    void Copy(void* destination, size_t destination_size, const void* source, size_t source_size);
    bool Equal(const void* buffer_one, size_t b1_size, const void* buffer_two, size_t b2_size);

    template <typename T>
    void Swap(T &a, T &b) {
        T temp = a;
        a = b;
        b = temp;
    }
}