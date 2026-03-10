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

    Math::Mat4 GetProjectionMatrix3D(unsigned int width, unsigned int height, float fov) {
        float aspect = width / height;
        float near_plane = 0.1f;
        float far_plane = 3000.0f;

        return Math::Mat4::Perspective(fov, aspect, near_plane, far_plane);
    }
    
    Math::Mat4 CameraComponent::GetProjectionMatrix() {

        return Math::Mat4::Identity();
    }
}