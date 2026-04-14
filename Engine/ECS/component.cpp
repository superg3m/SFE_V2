#include <component.hpp>
#include <entity.hpp>

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