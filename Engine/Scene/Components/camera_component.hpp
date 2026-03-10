#pragma once

#include "../../Core/sfe_core.hpp"
#include "../component.hpp"

namespace SFE {
    struct CameraComponent : public Component {
        void Update(float dt) override;
        int GetTypeID() override { return this->StaticTypeID<CameraComponent>(); }

        Math::Mat4 GetViewMatrix();
        Math::Mat4 GetProjectionMatrix();
    };
}