#include "scene.hpp"
#include "entity.hpp"

namespace SFE {
    void Scene::Update(float dt) {
        for (int i = 0; i < this->entities.count(); i++) {
            Entity* child = this->entities[i];
            if (child->alive) {
                child->Update(dt);
            } else {
                this->entities.unstableSwapbackRemove(i--);
            } 
        }
    }

    void Scene::Clear() {
        this->entities.clear();
    }

    void Scene::SetParent(Entity* entity, Entity* parent) {

    }

    Entity* Scene::CreateEntity(const char* name, Entity* parent = nullptr) {
        Entity* entity = new Entity(name);
        this->SetParent(entity, parent);

        this->entities.push(entity);
    }
}
