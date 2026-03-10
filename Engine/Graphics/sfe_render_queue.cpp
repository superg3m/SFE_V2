#include "sfe_render_queue.hpp"
#include "sfe_material.hpp"

namespace SFE {
	void RenderQueue::Submit(RenderCommand& command) {
		this->commands.push(command);
	}

	void RenderQueue::Draw(GraphicsAPI& graphics, CameraData& camera_data) {
		for (const RenderCommand& command : this->commands) {
			graphics.BindMaterial(command.material);
			
			Shader* shader = command.material->shader;
			shader->setMat4("uModel", command.model);
			shader->setMat4("uView", camera_data.view);
			shader->setMat4("uProjection", camera_data.projection);

			graphics.DrawMesh(command.mesh);
		}

		this->commands.clear();
	}
}