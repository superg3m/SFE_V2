#pragma once

#include <sfe.hpp>

struct PlayerEntity : public SFE::Entity {
    PlayerEntity();
    void Update(float dt) override;

	SFE::Material material;
	SFE::Mesh* mesh;
};