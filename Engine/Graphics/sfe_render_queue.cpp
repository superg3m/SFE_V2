#include "sfe_render_queue.hpp"

namespace SFE {
	void RenderQueue::Submit(RenderCommand& command) {
		this->commands.push(command);
	}

	void RenderQueue::Draw(GraphicsAPI& graphics) {
		for (const RenderCommand& command : this->commands) {
			graphics.BindMaterial(command.material);
			graphics.BindMesh(command.mesh);
			graphics.DrawMesh(command.mesh);
		}

		this->commands.clear();
	}
}