#include "scene.hpp"
#include "entity.hpp"

namespace SFE {
    void Scene::Update(float dt) {
        for (int i = 0; i < this->root.count(); i++) {
            Entity* child = this->root[i];
            if (child->alive) {
                child->Update(dt);
            } else {
                this->root.unstableSwapbackRemove(i--);
            } 
        }
    }

    void Scene::Clear() {
        this->root.clear();
    }

    bool Scene::SetParent(Entity* entity, Entity* parent) {
        if (!parent) {
            if (entity->parent) {
                for (int i = 0; i < entity->parent->children.count(); i++) {
                    Entity* child = entity->parent->children[i];
                    if (entity == child) {
                        this->root.push(entity);
                        entity->parent->children.unstableSwapbackRemove(i);

                        break;
                    }
                }
            } else {
                bool found = false;
                for (int i = 0; i < this->root.count(); i++) {
                    Entity* child = this->root[i];
                    if (entity == child) {
                        found = true;
                    
                        break;
                    }
                }

                if (!found) {
                    this->root.push(entity);
                }
            }
        }


        entity->parent = parent;

        if (parent) {
            parent->children.push(entity);
        }

        return true;
    }

    Entity* Scene::CreateEntity(const char* name, Entity* parent = nullptr) {
        Entity* entity = new Entity(name);
        this->SetParent(entity, parent);

        this->entities.push(entity);
    }
}
