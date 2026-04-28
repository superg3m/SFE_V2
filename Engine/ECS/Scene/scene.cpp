#include <scene.hpp>
#include <entity.hpp>
#include <entity_manager.hpp>

void Scene::update(float dt) {
    for (int i = 0; i < this->root.size(); i++) {
        Entity* child = this->root[i];
        if (child->alive) {
            child->update(dt);
        } else {
            this->root.erase(this->root.begin() + i);
        } 
    }
}

void Scene::clear() {
    this->root.clear();
}

bool Scene::set_parent(Entity* entity, Entity* new_parent) {
    // NOTE(Jovanni):
    // 1. You may not reparent to yourself
    // 2. You may not reparent to to your current parent
    // 3. A parent my not have a parent of a child or decendent

    Entity* current_parent = entity->parent;
    if (entity == new_parent || (current_parent && (current_parent == new_parent))) return false;

    if ((current_parent == nullptr) && (new_parent == nullptr)) {
        const auto it = std::find(this->root.begin(), this->root.end(), entity);
        bool found = it != this->root.end();
        if (!found) {
            this->root.push_back(entity);
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

            const auto it = std::find(current_parent->children.begin(), current_parent->children.end(), entity);
            bool found = it != this->root.end();
            RUNTIME_ASSERT_MSG(found, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
            current_parent->children.erase(it);
        } else {
            Entity* current_entity = new_parent;
            while (current_entity) {
                if (current_entity == entity) {
                    // NOTE(Jovanni): new_parent is a decendent of current_parent;
                    return false;
                }

                current_entity = current_entity->parent;
            }

            const auto it = std::find(this->root.begin(), this->root.begin(), entity);
            bool found = it != this->root.end();
            RUNTIME_ASSERT_MSG(found, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
            this->root.erase(it);
        }

        new_parent->children.push_back(entity);
    } else {    
        RUNTIME_ASSERT_MSG(current_parent, "The case where its just create or already root parented should already be handled\n");

        const auto it = std::find(current_parent->children.begin(), current_parent->children.begin(), entity);
        bool found = it != this->root.end();
        RUNTIME_ASSERT_MSG(found, "SHOULD BE GUARANTEED TO BE IN THE CHILDREN\n");
        current_parent->children.erase(it);
        this->root.push_back(entity);
    }

    entity->parent = new_parent;

    return true;
}

Entity* Scene::create_entity(EntityManager* manager, const char* name, Entity* parent) {
    Entity* entity = manager->create_entity();
    entity->name = name;
    this->set_parent(entity, parent);

    return entity;
}