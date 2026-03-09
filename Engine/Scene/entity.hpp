
#pragma once

#include "../Core/sfe_core.hpp"

namespace SFE {
    struct Scene;

    struct Entity {
        friend Scene;

        virtual ~Entity() = default;
        virtual void Update(float dt);

        const char* name;
        Entity* parent = nullptr;
        DS::Vector<Entity*> children;
        bool alive = true;

    protected:
        Entity() = default;
        Entity(const char* name);
    };
}