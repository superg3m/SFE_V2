#include "game.hpp"
#include <iostream>

bool Game::Initalize() {
	return true;
}

void Game::Update(float dt) {
	SFE::InputManager& input = SFE::Engine::GetInstance().GetInputManager();
	if (input.GetKeyPressed(SFE::KEY_A)) {
		std::cout << "DeltaTime: " << dt << std::endl;
	}
}

void Game::Shutdown() {

}