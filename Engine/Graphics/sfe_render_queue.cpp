#include "sfe_render_queue.hpp"
#include "sfe_material.hpp"

namespace SFE {
	void RenderQueue::Submit(RenderCommand& command) {
		this->commands.push(command);
	}

	void RenderQueue::Draw(GraphicsAPI& graphics) {
		for (const RenderCommand& command : this->commands) {
			graphics.BindMaterial(command.material);
			command.material->shader->setMat4("uModel", command.model);
			graphics.DrawMesh(command.mesh);
		}

		this->commands.clear();
	}
}