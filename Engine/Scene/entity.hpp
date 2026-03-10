
#pragma once

#include "../Core/sfe_core.hpp"
#include "component.hpp"

namespace SFE {
    struct Scene;

    struct Entity {
        friend Scene;

        virtual ~Entity() = default;
        virtual void Update(float dt);

        void AddComponent(Component* component);

        template <typename T, typename = typename std::enable_if_t<std::is_base_of_v<Component, T>>>
        T* GetComponent() {
            int id = Component::StaticTypeID<T>();
            for (Component* component : this->components) {
                if (component->GetTypeID() == id) {
                    return static_cast<T*>(component);
                }
            }

            return nullptr;
        }

        const char* name;
        Entity* parent = nullptr;
        DS::Vector<Entity*> children;
        DS::Vector<Component*> components;
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