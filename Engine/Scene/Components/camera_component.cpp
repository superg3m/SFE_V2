#include "camera_component.hpp"
#include "../entity.hpp"

namespace SFE {
    void CameraComponent::Update(float dt) {

    }

    Math::Mat4 CameraComponent::GetViewMatrix() {
        bool success = false;
        Math::Mat4 view = this->owner->GetWorldTrasform().inverse(success);
        if (!success) {
            LOG_ERROR("Failed to inverse camera world trasform\n");
        }

        return view;
    }

    Math::Mat4 CameraComponent::GetProjectionMatrix(float aspect) {
        return Math::Mat4::Perspective(fov, aspect, this->near_plane, this->far_plane);
    }
}