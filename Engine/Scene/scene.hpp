#pragma once

#include "../Core/sfe_core.hpp"

namespace SFE {

    struct Entity; 
    struct Scene {
        void Update(float dt);
        void Clear();

        Entity* CreateEntity(const char* name, Entity* parent = nullptr);
        void SetParent(Entity* entity, Entity* parent);

        template<typename T, typename = typename std::enable_if<std::is_base_of_v<Entity, T>>>
        T* CreateEntity(const char* name, Entity* parent = nullptr) {
            T* entity = new T(name);
            this->SetParent(entity, parent);

            this->entities.push(entity);
        }
        
        DS::Vector<Entity*> entities;
    };
}
