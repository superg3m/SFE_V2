#include "player_controller_component.hpp"
#include "../entity.hpp"

#include "../../Input/sfe_input_manager.hpp"
#include "../../engine.hpp"

namespace SFE {
    void PlayerControllerComponent::Update(float dt) {
        InputManager& input = Engine::GetInstance().GetInputManager();

        if (input.GetKeyDown(MOUSE_BUTTON_LEFT)) {
            Math::Vec2 current = input.GetCurrentMousePosition();
            Math::Vec2 previous = input.GetPreviousMousePosition();

            Math::Vec2 delta = current - previous;

            this->owner->rotation.x -= delta.y * this->sensitivity * dt;
            this->owner->rotation.y -= delta.x * this->sensitivity * dt;
            this->owner->orientation = Math::Quat::FromEuler(this->owner->rotation);
        }

        Math::Mat4 rot = Math::Mat4::Rotate(Math::Mat4::Identity(), Math::Quat::FromEuler(this->owner->rotation));
        Math::Vec3 front = (rot * Math::Vec4(0, 0, -1, 0)).normalize();
        Math::Vec3 right = (rot * Math::Vec4(1, 0, 0, 0)).normalize();

        if (input.GetKeyDown(SFE::KEY_W)) {
			this->owner->position += front * this->move_speed * dt;
		}

		if (input.GetKeyDown(SFE::KEY_A)) {
			this->owner->position -= right * this->move_speed * dt;
		}

		if (input.GetKeyDown(SFE::KEY_S)) {
			this->owner->position -= front * this->move_speed * dt;
		}

		if (input.GetKeyDown(SFE::KEY_D)) {
			this->owner->position += right * this->move_speed * dt;
		}
    }
}