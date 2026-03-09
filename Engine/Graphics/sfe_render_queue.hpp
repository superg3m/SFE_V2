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

	struct RenderQueue {
		void Submit(RenderCommand& command);
		void Draw(GraphicsAPI& graphics);

	private:
		DS::Vector<RenderCommand> commands;
	};
}