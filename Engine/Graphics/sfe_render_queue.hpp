#pragma once

#include "../Core/sfe_core.hpp"
#include "sfe_graphics_api.hpp"

namespace SFE {
	struct Mesh;
	struct Material;

	struct RenderCommand {
		Mesh* mesh = nullptr;
		Material* material = nullptr;
		Math::Mat4 model;
	};

	struct CameraData {
		Math::Mat4 view = Math::Mat4::Identity();
		Math::Mat4 projection = Math::Mat4::Identity();
	};

	struct RenderQueue {
		void Submit(RenderCommand& command);
		void Draw(GraphicsAPI& graphics, CameraData& camera_data);

	private:
		DS::Vector<RenderCommand> commands;
	};
}