#include "game.hpp"
#include "player_entity.hpp"
#include <iostream>

bool Game::Initalize() {
	this->scene = new SFE::Scene();

	SFE::Entity* camera = this->scene->CreateEntity("camera");
	camera->AddComponent(new SFE::CameraComponent());
	camera->position.z = 2.0f;

	camera->AddComponent(new SFE::PlayerControllerComponent());

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
		//   Position               Color
		+0.5f, +0.5f, +0.5f,    1.0f, 0.0f, 0.0f,
		-0.5f, +0.5f, +0.5f,    0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, +0.5f,    0.0f, 0.0f, 1.0f,
		+0.5f, -0.5f, +0.5f,    1.0f, 1.0f, 0.0f,

		+0.5f, +0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
		-0.5f, +0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,
		+0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f
	}; 

	DS::Vector<unsigned int> indices = {
		// Front Face
		0, 1, 2,
		0, 2, 3,

		// Top Face
		4, 5, 1,
		4, 1, 0,

		// Right Face
		4, 0, 3,
		4, 3, 7,

		// Left Face
		1, 5, 6,
		1, 6, 2,

		// Bottom Face
		3, 2, 6,
		3, 6, 7,

		// Back Face
		4, 7, 6,
		4, 6, 5
	}; 

	SFE::GraphicsAPI& graphics = SFE::Engine::GetInstance().GetGraphicsAPI();

	SFE::Shader* shader = graphics.CreateShader({"../../Shaders/text.vert", "../../Shaders/text.frag"});

	SFE::Material* material = new SFE::Material();
	material->setShaderProgram(shader);

	SFE::Mesh* mesh = new SFE::Mesh(layout, vertices, indices);
	SFE::Entity* A = this->scene->CreateEntity("ObjectA");
	A->AddComponent(new SFE::MeshComponent(material, mesh));
	A->position = Math::Vec3(0, 2, 0);

	SFE::Entity* B = this->scene->CreateEntity("ObjectB");
	B->AddComponent(new SFE::MeshComponent(material, mesh));
	B->position = Math::Vec3(0, 2, 2);
	B->rotation = Math::Vec3(0, RAD_TO_DEGREES(1), 0);
	B->orientation = Math::Quat::FromEuler(B->rotation);

	SFE::Entity* C = this->scene->CreateEntity("ObjectC");
	C->AddComponent(new SFE::MeshComponent(material, mesh));
	C->position = Math::Vec3(-2, 0, 0);
	C->rotation = Math::Vec3(RAD_TO_DEGREES(1), 0, RAD_TO_DEGREES(1));
	C->orientation = Math::Quat::FromEuler(C->rotation);
	C->scale = Math::Vec3(1.5f, 1.5f, 1.5f);

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