#pragma once

#include "../Core/sfe_core.hpp"
#include "sfe_graphics_api.hpp"

namespace SFE {
	typedef struct Mesh Mesh;
	typedef struct Material Material;
	struct RenderCommand {
		Mesh* mesh = nullptr;
		Material* material = nullptr;
	};

	struct RenderQueue {
		void Submit(RenderCommand& command);
		void Draw(GraphicsAPI& graphics);

	private:
		DS::Vector<RenderCommand> commands;
	};
}