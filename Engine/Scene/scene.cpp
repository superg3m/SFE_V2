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

    bool Scene::SetParent(Entity* entity, Entity* new_parent) {
        // NOTE(Jovanni):
        // 1. You may not reparent to yourself
        // 2. You may not reparent to to your current parent
        // 3. A parent my not have a parent of a child or decendent

        Entity* current_parent = entity->parent;
        if (entity == new_parent || (current_parent && (current_parent == new_parent))) return false;

        if ((current_parent == nullptr) && (new_parent == nullptr)) {
            bool found = this->root.find(entity) != -1;
            if (!found) {
                this->root.push(entity);

                return true;
            }

            return false;
        }

        if (new_parent) {
            if (current_parent) {
                Entity* current_entity = new_parent;
                while (current_entity) {
                    if (current_entity == entity) {
                        // NOTE(Jovanni): new_parent is a decendent of current_parent;
                        return false;
                    }

                    current_entity = current_entity->parent;
                }

                int index = current_parent->children.find(entity);
                RUNTIME_ASSERT_MSG(index != -1, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
                current_parent->children.unstableSwapbackRemove(index);
            } else {
                Entity* current_entity = new_parent;
                while (current_entity) {
                    if (current_entity == entity) {
                        // NOTE(Jovanni): new_parent is a decendent of current_parent;
                        return false;
                    }

                    current_entity = current_entity->parent;
                }

                int index = this->root.find(entity);
                RUNTIME_ASSERT_MSG(index != -1, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
                this->root.unstableSwapbackRemove(index);
            }

            new_parent->children.push(entity);
        } else {    
            RUNTIME_ASSERT_MSG(current_parent, "The case where its just create or already root parented should already be handled\n");

            int index = current_parent->children.find(entity);
            RUNTIME_ASSERT_MSG(index != -1, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
            current_parent->children.unstableSwapbackRemove(index);
            this->root.push(entity);
        }

        entity->parent = new_parent;

        return true;
    }

    Entity* Scene::CreateEntity(const char* name, Entity* parent) {
        Entity* entity = new Entity();
        entity->name = name;
        this->SetParent(entity, parent);

        return entity;
    }
}
