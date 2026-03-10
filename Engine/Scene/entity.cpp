#include "entity.hpp"
#include "component.hpp"

namespace SFE {
    void Entity::AddComponent(Component* component) {
        this->components.push(component);
        component->owner = this;
    }

    void Entity::Update(float dt) {
        for (Component* component : this->components) {
            component->Update(dt);
        }

        for (int i = 0; i < this->children.count(); i++) {
            Entity* child = this->children[i];
            if (child->alive) {
                child->Update(dt);
            } else {
                this->children.unstableSwapbackRemove(i--);
            } 
        }
    }
}
