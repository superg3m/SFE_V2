#include "player_entity.hpp"

PlayerEntity::PlayerEntity() {
    SFE::GraphicsAPI& graphics = SFE::Engine::GetInstance().GetGraphicsAPI();
	SFE::Shader* shader = graphics.CreateShader({"../../Shaders/text.vert", "../../Shaders/text.frag"});

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
		+0.5f, +0.5f, +0.0f,    1.0f, 0.0f, 0.0f,
		-0.5f, +0.5f, +0.0f,    0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, +0.0f,    0.0f, 0.0f, 1.0f,
		+0.5f, -0.5f, +0.0f,    1.0f, 1.0f, 0.0f
	}; 

	DS::Vector<unsigned int> indices = {
		0, 1, 2,
		0, 2, 3
	}; 

	SFE::Material* material = new SFE::Material();
	material->setShaderProgram(shader);

	SFE::Mesh* mesh = new SFE::Mesh(layout, vertices, indices);
	SFE::MeshComponent* mesh_component = new SFE::MeshComponent(material, mesh);

	this->AddComponent(mesh_component);
}

void PlayerEntity::Update(float dt) {
    SFE::Entity::Update(dt);

    SFE::Engine& engine = SFE::Engine::GetInstance();
	SFE::InputManager& input = engine.GetInputManager();

	#if 0
		if (input.GetKeyDown(SFE::KEY_W)) {
			this->position.y += 0.01f;
		}

		if (input.GetKeyDown(SFE::KEY_A)) {
			this->position.x -= 0.01f;
		}

		if (input.GetKeyDown(SFE::KEY_S)) {
			this->position.y -= 0.01f;
		}

		if (input.GetKeyDown(SFE::KEY_D)) {
			this->position.x += 0.01f;
		}

		if (input.GetKeyDown(SFE::KEY_LEFT)) {
			this->rotation.z += 1;
			this->orientation = Math::Quat::FromEuler(this->rotation);
		}

		if (input.GetKeyDown(SFE::KEY_RIGHT)) {
			this->rotation.z -= 1;
			this->orientation = Math::Quat::FromEuler(this->rotation);
		}
	#endif
}