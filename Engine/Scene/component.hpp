#pragma once

namespace SFE {
    struct Entity;

    struct Component {
        friend struct Entity;

        ~Component() = default;
        virtual void Update(float dt) = 0;
        virtual int GetTypeID() = 0;

        template<typename T>
        static int StaticTypeID() {
            static int id = Component::next_id++;
            return id;
        }

        Entity* owner = nullptr;
        static int next_id;
    };
}