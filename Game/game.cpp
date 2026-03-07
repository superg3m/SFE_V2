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
		+0.5f, +0.5f, +0.0f,    1.0f, 0.0f, 0.0f,
		-0.5f, +0.5f, +0.0f,    0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, +0.0f,    0.0f, 0.0f, 1.0f,
		+0.5f, -0.5f, +0.0f,    1.0f, 1.0f, 0.0f
	}; 

	DS::Vector<unsigned int> indices = {
		0, 1, 2,
		0, 2, 3
	};

	this->mesh = new SFE::Mesh(layout, vertices, indices);

	return true;
}

void Game::Update(float dt) {
	SFE::Engine& engine = SFE::Engine::GetInstance();
	SFE::InputManager& input = engine.GetInputManager();
	SFE::RenderQueue& queue = engine.GetRenderQueue();

	const bool SHIFT = input.GetKey(SFE::KEY_SHIFT, SFE::PRESSED|SFE::DOWN);
	if (SHIFT && input.GetKeyPressed(SFE::KEY_A)) {
		std::cout << "DeltaTime: " << dt << std::endl;
	}

	if (input.GetKeyPressed(SFE::KEY_ESCAPE)) {
		this->SetShouldClose(true);
	}

	glClearColor(0.2f, 0.8f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SFE::RenderCommand command = {
		.mesh = this->mesh,
		.material = &this->material,
	};

	queue.Submit(command);
}

void Game::Shutdown() {

}