#include "game.hpp"
#include <iostream>

bool Game::Initalize() {
	SFE::GraphicsAPI& graphics = SFE::Engine::GetInstance().GetGraphicsAPI();
	SFE::Shader* shader = graphics.CreateShader({"../../Shaders/text.vert", "../../Shaders/text.frag"});
	(void)shader;

	return true;
}

void Game::Update(float dt) {
	SFE::InputManager& input = SFE::Engine::GetInstance().GetInputManager();
	const bool SHIFT = input.GetKey(SFE::KEY_SHIFT, SFE::PRESSED|SFE::DOWN);

	if (SHIFT && input.GetKeyPressed(SFE::KEY_A)) {
		std::cout << "DeltaTime: " << dt << std::endl;
	}
}

void Game::Shutdown() {

}