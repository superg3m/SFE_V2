#include <basic.hpp>

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