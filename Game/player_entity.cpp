#include "player_entity.hpp"

PlayerEntity::PlayerEntity() {
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

	this->mesh = new SFE::Mesh(layout, vertices, indices);
}

void PlayerEntity::Update(float dt) {
    SFE::Entity::Update(dt);

    SFE::Engine& engine = SFE::Engine::GetInstance();
	SFE::InputManager& input = engine.GetInputManager();
	SFE::RenderQueue& queue = engine.GetRenderQueue();

	const bool SHIFT = input.GetKey(SFE::KEY_SHIFT, SFE::PRESSED|SFE::DOWN);
	if (SHIFT && input.GetKeyPressed(SFE::KEY_A)) {
		LOG_DEBUG("DeltaTime: %d", dt);
	}

	glClearColor(0.2f, 0.8f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SFE::RenderCommand command = {
		.mesh = this->mesh,
		.material = &this->material,
	};

    if (input.GetKeyDown(SFE::KEY_W)) {
		this->offset_y += 0.001f;
	}

    if (input.GetKeyDown(SFE::KEY_A)) {
		this->offset_x -= 0.001f;
	}

    if (input.GetKeyDown(SFE::KEY_S)) {
		this->offset_y -= 0.001f;
	}

    if (input.GetKeyDown(SFE::KEY_D)) {
		this->offset_x += 0.001f;
	}

    this->material.setParam("offset_x", this->offset_x);
    this->material.setParam("offset_y", this->offset_y);

	queue.Submit(command);
}