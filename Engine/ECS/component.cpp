#include <component.hpp>
#include <entity.hpp>
#include <mesh.hpp>
#include <render_queue.hpp>
#include <gpu_buffers.hpp>

void PlayerControllerComponent::update(float dt) {
    StatusComponent* status = this->owner->GetComponent<StatusComponent>();
    HealthComponent* health = this->owner->GetComponent<HealthComponent>();
    MeshComponent* mesh = this->owner->GetComponent<MeshComponent>();
    if (status && status->stunned)  {
        return;
    }

    /*
    if (Input::GetKeyDown(KEY_A)) {
        this->owner.position.x -= 1.0f;
    }
    */
}

void MeshComponent::update(float dt) {
    glm::mat4 view          = glm::mat4(1.0f);
    glm::mat4 projection    = glm::mat4(1.0f);

    OpenGL::RenderCommandOpaque command = {};
    command.vao = this->vao;
    command.entry = this->entry;
    command.material = this->material;
    command.model = this->owner->get_world_transform();
    command.view = view;
    command.projection = projection;

    if (this->should_render_mesh) {
        this->queue->submit(command);
    }

    /*
    if (this->should_render_aabb_mesh) {
        OpenGL::RenderCommandOpaque aabb_command = {};
        aabb_command.shader = this->shader;
        aabb_command.mesh = this->aabb_mesh;
        aabb_command.model = this->owner->get_world_transform();
        aabb_command.view = view; // TODO(Jovanni): this shouldn't get supplied it should use active camera
        aabb_command.projection = projection; // TODO(Jovanni): this shouldn't get supplied it should use active camera

        this->queue->submit(aabb_command);
    }
    */
}