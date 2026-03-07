#include "game.hpp"
#include <iostream>

bool Game::Initalize() {
	SFE::GraphicsAPI& graphics = SFE::Engine::GetInstance().GetGraphicsAPI();
	SFE::Shader* shader = graphics.CreateShader({"../../Shaders/text.vert", "../../Shaders/text.frag"});

	this->material.setShaderProgram(shader);

	SFE::VertexLayout layout = {
		{
			SFE::VertexElement{
				.index = 0,
				.count = 3,
				.type = GL_FLOAT,
				.offset = 0,
			},

			SFE::VertexElement{
				.index = 1,
				.count = 3,
				.type = GL_FLOAT,
				.offset = sizeof(float) * 3,
			},
		}
	};
	layout.stride = sizeof(float) * 6;

	DS::Vector<float> vertices = {
		//   Position           Position
		+0.5f, +0.5f, +0.0f,    1, 0, 0,
		-0.5f, +0.5f, +0.0f,    0, 1, 0,
		-0.5f, -0.5f, +0.0f,    0, 0, 1,
		+0.5f, -0.5f, +0.0f,    1, 1, 0
	}; 

	DS::Vector<unsigned int> indices = {
		0, 1, 2,
		0, 2, 3
	};

	this->mesh = new SFE::Mesh(layout, vertices, indices);

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