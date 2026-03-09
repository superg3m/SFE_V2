
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

        Math::Mat4 GetLocalTrasform() {
            return Math::Mat4::Transform(this->scale, this->orientation, this->position);
        }

        Math::Mat4 GetWorldTrasform() {
            if (this->parent) {
                return this->parent->GetWorldTrasform() * this->GetLocalTrasform();
            }

            return this->GetLocalTrasform();
        }

        Math::Vec3 position = Math::Vec3(0);
        Math::Quat orientation = Math::Quat::Identity();
        Math::Vec3 rotation = Math::Vec3(0);
        Math::Vec3 scale = Math::Vec3(1);

    protected:
        Entity() = default;

    };
}