#include "entity.hpp"
       
namespace SFE {
    void Entity::Update(float dt) {
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
