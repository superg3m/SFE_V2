#include "game.hpp"
#include "player_entity.hpp"
#include <iostream>

bool Game::Initalize() {
	this->scene = new SFE::Scene();

	SFE::Entity* camera = this->scene->CreateEntity("camera");
	camera->AddComponent(new SFE::CameraComponent());
	camera->position.z = 2.0f;

	this->scene->CreateEntity<PlayerEntity>("player");

	this->scene->main_camera = camera;

	SFE::Engine::GetInstance().SetScene(this->scene);

	return true;
}

void Game::Update(float dt) {
	SFE::Engine& engine = SFE::Engine::GetInstance();
	SFE::InputManager& input = engine.GetInputManager();

	if (input.GetKeyPressed(SFE::KEY_ESCAPE)) {
		this->SetShouldClose(true);
	}

	this->scene->Update(dt);
}

void Game::Shutdown() {

}