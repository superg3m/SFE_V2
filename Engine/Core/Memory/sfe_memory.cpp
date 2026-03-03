#include <stdlib.h>

#include "sfe_memory.hpp"

namespace Memory {
    void Zero(void* data, size_t data_size_in_bytes) {
        RUNTIME_ASSERT(data);
        
        for (size_t i = 0; i < data_size_in_bytes; i++) {
            ((u8*)data)[i] = 0;
        }
    }

    void Copy(void* destination, size_t destination_size, const void* source, size_t source_size) {
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

    bool Equal(const void* buffer_one, size_t b1_size, const void* buffer_two, size_t b2_size) {
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
}
