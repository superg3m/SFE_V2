#pragma once

#include <sfe.hpp>

struct PlayerEntity : public SFE::Entity {
    PlayerEntity();
    void Update(float dt) override;

	SFE::Material material;
	SFE::Mesh* mesh;
    float offset_x = 0.0f;
    float offset_y = 0.0f;
};