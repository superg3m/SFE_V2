#pragma once

#include "../../../Core/core.hpp"

struct CommandBufferHandle {
	Handle handle = Handle::invalid();
	CommandBufferHandle(Handle handle) {
		this->handle = handle;
	}

	static CommandBufferHandle invalid() {
		return {Handle::invalid()};
	}
};

struct TextureHandle {
	Handle handle = Handle::invalid();
	TextureHandle(Handle handle) {
		this->handle = handle;
	}

	static TextureHandle invalid() {
		return {Handle::invalid()};
	}
};

struct MaterialHandle {
	Handle handle = Handle::invalid();
	MaterialHandle(Handle handle) {
		this->handle = handle;
	}

	static MaterialHandle invalid() {
		return {Handle::invalid()};
	}
};

struct ShaderHandle {
	Handle handle = Handle::invalid();
	ShaderHandle(Handle handle) {
		this->handle = handle;
	}

	static ShaderHandle invalid() {
		return {Handle::invalid()};
	}
};

struct MeshHandle {
	Handle handle = Handle::invalid();
	MeshHandle(Handle handle) {
		this->handle = handle;
	}
	
	static MeshHandle invalid() {
		return {Handle::invalid()};
	}
};

struct VertexBufferHandle {
	Handle handle = Handle::invalid();
	VertexBufferHandle(Handle handle) {
		this->handle = handle;
	}

	static VertexBufferHandle invalid() {
		return {Handle::invalid()};
	}
};

struct IndexBufferHandle {
	Handle handle = Handle::invalid();
	IndexBufferHandle(Handle handle) {
		this->handle = handle;
	}

	static IndexBufferHandle invalid() {
		return {Handle::invalid()};
	}
};