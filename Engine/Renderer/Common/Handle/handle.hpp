#include "../../../Core/core.hpp"

struct CommandBufferHandle {
    Handle handle = Handle::invalid();
	static CommandBufferHandle invalid() {
		return {};
	}
};

struct TextureHandle {
    Handle handle = Handle::invalid();
	static TextureHandle invalid() {
		return {};
	}
};

struct ShaderHandle {
    Handle handle = Handle::invalid();
	static ShaderHandle invalid() {
		return {};
	}
};

struct MaterialHandle {
    Handle handle = Handle::invalid();
	static MaterialHandle invalid() {
		return {};
	}
};

struct MeshHandle {
    Handle handle = Handle::invalid();
	static MeshHandle invalid() {
		return {};
	}
};

struct MeshEntryHandle {
    Handle handle = Handle::invalid();
	static MeshEntryHandle invalid() {
		return {};
	}
};

struct VertexBufferHandle {
    Handle handle = Handle::invalid();
	static VertexBufferHandle invalid() {
		return {};
	}
};

struct IndexBufferHandle {
    Handle handle = Handle::invalid();
	static IndexBufferHandle invalid() {
		return {};
	}
};

struct PipelineHandle {
    Handle handle = Handle::invalid();
	static PipelineHandle invalid() {
		return {};
	}
};