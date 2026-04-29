#include "../Handle/handle.hpp"
#include "../Texture/texture.hpp"

enum class RequestType {
	MESH_LOAD,
	MESH_CUBE,
	MESH_AABB,

	TEXTURE_LOAD,

	DRAW_CALL
};

struct TextureRequest {
	const char* path;
	TextureDescription description;
};

struct DrawCallRequest {
	PipelineHandle pipeline;
	MeshHandle mesh; // mesh_entries, vao, vbo, ebo
	Mat4 Model;
	int instance_count;
};

struct Request {
	RequestType type;
	union {
		TextureRequest texture;
		DrawCallRequest  draw_call;
	};

	template<typename T>
    Request(T value) {
        if constexpr (std::is_same_v<T, TextureRequest>) {
            this->type = RequestType::TEXTURE_LOAD;
			this->texture = texture;
        } else if constexpr (std::is_same_v<T, DrawCallRequest>) {
            this->type = RequestType::TEXTURE_LOAD;
			this->texture = texture;
        } else {
            STATIC_ASSERT(false, "Unsupported BindingValue type");
        }
    }

	Handle handle;
};