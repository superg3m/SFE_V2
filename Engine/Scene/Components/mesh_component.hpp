#pragma once

#include "../component.hpp"

namespace SFE {
    struct Mesh;
    struct Material;

    struct MeshComponent : public Component {
        MeshComponent(Material* material, Mesh* mesh);
        void Update(float dt) override;
        int GetTypeID() override { return this->StaticTypeID<MeshComponent>(); }

        Mesh* mesh;
        Material* material;
    };
}
