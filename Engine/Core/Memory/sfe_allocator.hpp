#pragma once

#include "../Common/sfe_common.hpp"

namespace DS {
    template<typename T>
    struct Stack;
}

namespace Memory {
    void* Malloc(size_t allocation_size);
    void Free(void* data);
    void* Realloc(void* data, size_t old_allocation_size, size_t new_allocation_size);

    struct BaseAllocator {
        virtual ~BaseAllocator() = default;

        virtual void* malloc(size_t allocation_size) = 0;
        virtual void free(void* data) = 0;
        virtual void* realloc(void* data, size_t old_allocation_size, size_t new_allocation_size) = 0;

        bool is_valid() const {
            return this->valid;
        }
    protected:
        bool valid = false;
    };

    void bindAllocator(BaseAllocator* allocator);

    struct GeneralAllocator : public BaseAllocator {
        GeneralAllocator();
        ~GeneralAllocator();

        void* malloc(size_t allocation_size) override;
        void free(void* data) override;
        void* realloc(void* data, size_t old_allocation_size, size_t new_allocation_size) override;
    };

    // add linked list memory pages
    enum ArenaFlag {
        ARENA_FLAG_FIXED       = 0x1,
        ARENA_FLAG_CIRCULAR    = 0x2,
        ARENA_FLAG_STACK_MEMORY= 0x4,
        ARENA_FLAG_COUNT       = 4,
    };

    struct ArenaAllocator : public BaseAllocator {
        ArenaAllocator();
        ~ArenaAllocator();

        ArenaAllocator(void* memory, size_t allocation_size, int flags, u8 alignment);

        static ArenaAllocator Fixed(void* memory, size_t capacity, bool is_stack_memory);
        static ArenaAllocator Circular(void* memory, size_t memory_capacity, bool is_stack_memory);

        void* malloc(size_t allocation_size) override;
        void free(void* data) override;
        void free(size_t size_to_free);
        void* realloc(void* data, size_t old_allocation_size, size_t new_allocation_size) override;

    private:
        int flags = 0;
        size_t used = 0;
        size_t used_save_point = 0;
        size_t capacity = 0;
        u8 alignment = 0;
        u8* base_address = nullptr;
        DS::Stack<size_t>* size_stack = nullptr;

        bool isDataPoppable(void* data);
    };
}
