#include "mesh_component.hpp"
#include "../entity.hpp"
#include "../../engine.hpp"
#include "../../Graphics/sfe_render_queue.hpp"

namespace SFE {
    MeshComponent::MeshComponent(Material* material, Mesh* mesh) {
        this->material = material;
        this->mesh = mesh;
    }

    void MeshComponent::Update(float dt) {
        SFE::Engine& engine = SFE::Engine::GetInstance();
        SFE::RenderQueue& queue = engine.GetRenderQueue();

        SFE::RenderCommand command = {
            .mesh = this->mesh,
            .material = this->material,
            .model = owner->GetWorldTrasform()
        };

        queue.Submit(command);
    }
}