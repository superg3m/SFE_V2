#include "game.hpp"
#include "player_entity.hpp"
#include <iostream>

bool Game::Initalize() {
	this->scene.CreateEntity<PlayerEntity>("player");

	return true;
}

void Game::Update(float dt) {
	SFE::Engine& engine = SFE::Engine::GetInstance();
	SFE::InputManager& input = engine.GetInputManager();

	if (input.GetKeyPressed(SFE::KEY_ESCAPE)) {
		this->SetShouldClose(true);
	}

	this->scene.Update(dt);
}

void Game::Shutdown() {

}