#include "../Basic/basic.hpp"
#include "../Assert/assert.hpp"

const s32 INVALID_HANDLE_INDEX = -1;

template<typename T>
struct Handle {
	s32 index = INVALID_HANDLE_INDEX;
	u32 generation = 0;
	
	static Handle<T> create(u32 index, u32 generation) {
		Handle<T> ret = {};
		ret.index = index;
		ret.generation = generation;

		return ret;
	}

	static Handle<T> invalid() {
		Handle<T> ret = {};
		ret.index = INVALID_HANDLE_INDEX;
		
		return ret;
	}

	bool operator==(Handle<T> other) const {
		return this->index == other.index && this->generation == other.generation;
	}

	bool operator!=(Handle<T> other) const {
		return !(*this == other);
	}
};

// -- 

template<typename T>
struct Slot {
	bool allocated = false;
	u32 generation = 0;
	T data;
};

template<typename T, int N>
struct Registry {
	// I need LinkedList because the addresses won't be the same otherwise when this resizes...
	// I want like a freelist
	Slot<T> slots[N] = {};

	static Registry<T, N> create() {
		Registry<T, N> ret = {};
		return ret;
	}

	Handle<T> acquire() {
		Handle<T> handle = Handle<T>::invalid();
		for (int i = 0; i < N; i++) {
			Slot<T>& slot = this->slots[i];
			if (slot.allocated == false) {
				handle = Handle<T>::create(i, slot.generation);
				slot.allocated = true;
				break;
			}
		}

		RUNTIME_ASSERT(handle != Handle<T>::invalid());

		/*
		if (handle == handle_invalid<T>()) {
			registry->slots.push(Slot<T>{});
			handle.index = registry->slots.count() - 1;
			handle.generation = registry->slots[handle.index].generation;
		}
		*/

		Memory::zero(&this->slots[handle.index].data, sizeof(T));
		return handle;
	}

	void release(Handle<T> handle) {
		RUNTIME_ASSERT(this->is_handle_valid(handle));

		this->slots[handle.index].allocated = false;
		this->slots[handle.index].generation++;
	}

	bool is_handle_valid(Handle<T> handle) {
		return handle.index != INVALID_HANDLE_INDEX && this->slots[handle.index].generation == handle.generation;
	}

	T& get(Handle<T> handle) {
		RUNTIME_ASSERT(this->is_handle_valid(handle));

		return this->slots[handle.index].data;
	}
};

/*
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
*/