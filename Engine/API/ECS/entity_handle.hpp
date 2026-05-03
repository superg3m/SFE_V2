#pragma once

#include "../../Core/core.hpp"

struct EntityHandle {
	Handle handle = Handle::invalid();
	static EntityHandle invalid() {
		return Handle::invalid();
	}

	EntityHandle(Handle handle) {
		this->handle = handle;
	}

	bool operator==(EntityHandle other) const {
		return this->handle == other.handle;
	}
};