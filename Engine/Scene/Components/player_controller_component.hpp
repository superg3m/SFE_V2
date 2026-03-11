#pragma once

#include "../../Core/sfe_core.hpp"
#include "../component.hpp"

namespace SFE {
    struct PlayerControllerComponent : public Component {
        void Update(float dt) override;
        int GetTypeID() override { return this->StaticTypeID<PlayerControllerComponent>(); }

        float sensitivity = 1.0f;
        float move_speed = 1.0f;
    };
}