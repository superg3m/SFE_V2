#include "game.hpp"

EXPORT_FN void application_init(Engine* engine, Hashmap<String, String>* string_intern_map) {
	engine->application_state = engine->memory.permanent_allocator.malloc(sizeof(AppState), alignof(AppState));
	AppState* app = (AppState*)engine->application_state;
	*app = {};

	EntityHandle cube = engine->scene.create_entity(STR_INTERN("cube"), engine->scene.root);
	Entity& cube_slot = engine->scene.entities.get(cube.handle);

	// app->cube_shader = engine->renderer.create_shader({STR_INTERN("../../../Game/Assets/Shaders/cube.vert"), STR_INTERN("../../../Game/Assets/Shaders/cube.frag")});
	// app->material = engine->renderer.create_material(app->cube_shader);
	// MeshHandle mesh = engine->renderer.create_mesh_cube(app->material);
	// cube_slot.add_component<MeshComponent>(mesh);

	app->opaque_pipeline = Pipeline{
		.rasterizer = {
			.fill = true
		},
		.depth = {
			.depth_testing = true,
			.depth_write = true
		},
		.blend = {
			.enabled = false
		}
	};

	app->opaque_wireframe_pipeline = Pipeline{
		.rasterizer = {
			.fill = false
		},
		.depth = {
			.depth_testing = true,
			.depth_write = true
		},
		.blend = {
			.enabled = false
		}
	};

	TextureDescription desc = {};
	app->container_texture = engine->renderer.create_texture(0, STR_INTERN("../../../Game/Assets/Textures/container.jpg"), desc);
	app->face_texture = engine->renderer.create_texture(1, STR_INTERN("../../../Game/Assets/Textures/awesomeface.png"), desc);

	app->cube_shader = engine->renderer.create_shader({STR_INTERN("../../../Game/Assets/Shaders/cube.vert"), STR_INTERN("../../../Game/Assets/Shaders/cube.frag")});
	app->material = engine->renderer.create_material(app->cube_shader);
	app->cube_mesh = engine->renderer.create_mesh_cube(app->material);

	app->backpack_shader = engine->renderer.create_shader({STR_INTERN("../../../Game/Assets/Shaders/model.vert"), STR_INTERN("../../../Game/Assets/Shaders/model.frag")});
	app->backpack_mesh = engine->renderer.create_mesh(app->backpack_shader, STR_INTERN("../../../Game/Assets/Models/Backpack/backpack.obj"));

	app->cube_translations = Vector<Mat4>(engine->memory.permanent_allocator);
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2) {
		for (int x = -10; x < 10; x += 2) {
			Vec3 translation;
			translation.x = (float)x * 2.0f + offset;
			translation.y = (float)y * 2.0f + offset;
			translation.z = sin(x + y) * 10;
			app->cube_translations.append(Mat4::translate(Mat4::identity(), translation).transpose());
		}
	}
	
	VertexLayout layout = VertexLayout({{
		VertexAttribute{4, 0, BufferStrideTypeInfo::MAT4, true},
	}, engine->memory.frame_allocator});

	app->instance_cube_vbo = engine->renderer.create_vbo(app->cube_mesh, layout, app->cube_translations, true);
	app->timer.start(5.0f);
}

EXPORT_FN void application_update(Engine* engine, Hashmap<String, String>* string_intern_map, float dt) {
	AppState* app = (AppState*)engine->application_state;

	app->accumulator += dt * 100;

	// active_scene.update(dt);
	if (engine->input.get_key_pressed(KEY_ESCAPE)) {
		engine->window.close = true;
	}

	if (engine->input.get_key_pressed(KEY_C)) {
		engine->window.capture_mouse = !engine->window.capture_mouse;
	}

	if (engine->input.get_key_pressed(KEY_R)) {
		engine->renderer.shader_recompile(app->cube_shader);
		engine->renderer.shader_recompile(app->backpack_shader);
	}

	if (engine->input.get_key(KEY_SPACE, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::UP, dt);
	}

	if (engine->input.get_key(KEY_CTRL, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::DOWN, dt);
	}

	if (engine->input.get_key(KEY_W, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::FORWARD, dt); 
	}

	if (engine->input.get_key(KEY_A, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::LEFT, dt); 
	}

	if (engine->input.get_key(KEY_S, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::BACKWARD, dt);
	}

	if (engine->input.get_key(KEY_D, PRESSED|DOWN)) {
		engine->camera.process_keyboard(CameraDirection::RIGHT, dt); 
	}
}

EXPORT_FN void application_render(Engine* engine, Hashmap<String, String>* string_intern_map, float dt) {
	AppState* app = (AppState*)engine->application_state;

	if(app->timer.tick(dt)) {
		app->use_opaque_pipeline = !app->use_opaque_pipeline;
		app->timer.reset();
	}

	Mat4 model = Mat4::rotate(Mat4::identity(), Quat::from_euler(app->accumulator, app->accumulator, 0));
	Mat4 view = engine->get_view_matrix();
	Mat4 projection = engine->get_projection_matrix();

	if (engine->reloaded_dll) {
		LOG_DEBUG("Before material_set\n");
	}

	engine->renderer.material_set_uniform(app->material, STR_INTERN("uContainer"), app->container_texture); 
	engine->renderer.material_set_uniform(app->material, STR_INTERN("uFace"), app->face_texture); 

	if (engine->reloaded_dll) {
		LOG_DEBUG("After material_set\n");
	}

	Pipeline pipeline = app->use_opaque_pipeline ? app->opaque_pipeline : app->opaque_wireframe_pipeline;
	engine->renderer.bind_vbo(app->instance_cube_vbo, app->cube_mesh);

	if (engine->reloaded_dll) {
		LOG_DEBUG("Before draw 1\n");
	}
	engine->renderer.draw_mesh(pipeline, app->cube_mesh, model, view, projection, app->cube_translations.count);
	
	if (engine->reloaded_dll) {
		LOG_DEBUG("After draw 1\n");
	}

	model = Mat4::translate(model, 0, 5, 0);
	pipeline = !app->use_opaque_pipeline ? app->opaque_pipeline : app->opaque_wireframe_pipeline;
	engine->renderer.draw_mesh(pipeline, app->backpack_mesh, model, view, projection);

	if (engine->reloaded_dll) {
		LOG_DEBUG("After draw 2\n");
	}
}

// https://www.youtube.com/watch?v=9R2rRLbBkHU
// https://www.youtube.com/watch?v=QAeRxfeFAo0

/*
// TODO(Jovanni): I want to reorganize the runtime.

The end goal of this project is the following:
- [] robust rendering system (account for framebuffer objects)
	- [] texture for depth, color, light, normals
		- These shouldn't be in the shader, theres should just be thier own shader and i just swap out the shader
		that way its easy and doesn't clutter up shaders.

- [] Show entity heiarchy
- [] Animations
- [] Physics (collisions, shooting)
- [] Multiple Cameras
- [] pickable entities (probably ray based) (maybe frame buffer) (but also maybe just tree based?)
- [] Hot reloading
- [] Material system thats nice and hotswappable
- [] Scene system with nice lighting abilities (spotlight, sunlight, pointlights)
- [-] nice docking imgui and imguizmo
- [] AS FEW Syscalls during a frame as possible!!!
- [] Profile and code instrumentation in imgui

- [] Remove all constructors except for containers and other places it makes sense, in those places make sure you have a default constructor
- [] Make sure you have as few general allocation as possible I'm very very concerned about allocations across dll boundaries
- [] AABB render (*maybe you can do this by sying draw mesh, but only this range of entries? Little bit hacky but works)
- [] Entity stuff
- [] 3d grid of lines (should be simple?)
- [] inside hte shader directory I should have a manifest that keep track of the last modified time (so I can not have to recompile all shaders
- [] Shader Header is a really interesting idea
	u8* shader_header_data = Platform::read_entire_file("shader_header.h")
	u8* shader_vert_source = Platform::read_entire_file("shader.vert")
	u8* shader_frag_source = Platform::read_entire_file("shader.frag")
	String::append(shader_header_data, length, shader_vert_source);
	String::append(shader_header_data, length, shader_frag_source);

	shader_header.h: {
		vec3 CalcDirLight(const DirLight light, const vec3 normal, const vec3 viewDir, const vec2 texCoord) {
			vec3 lightDir = normalize(-light.direction); // Directional light direction is inverse of uniform value

			// Diffuse calculation
			float diffuse_factor = max(dot(normal, lightDir), 0.0f);

			// Specular calculation
			vec3 reflectDir = reflect(-lightDir, normal); // Light direction, not negative light direction
			float specular_factor = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);

			// Get material colors
			vec3 diffuse_mat_color = getDiffuseColor(texCoord);
			vec3 specular_mat_color = getSpecularColor(texCoord);

			// Combine light and material components
			vec3 ambient = light.ambient * (uMaterial.ambient_color + diffuse_mat_color); // Ambient usually uses diffuse color as well
			vec3 diffuse = light.diffuse * diffuse_factor * diffuse_mat_color;
			vec3 specular = light.specular * specular_factor * specular_mat_color;

			return (ambient + diffuse + specular);
		}

		vec3 CalcPointLight(const PointLight light, const vec3 normal, const vec3 fragPos, const vec3 viewDir, const vec2 texCoord) {
			vec3 lightDir = normalize(light.position - fragPos);

			// Diffuse calculation
			float diffuse_factor = max(dot(normal, lightDir), 0.0f);

			// Specular calculation
			vec3 reflectDir = reflect(-lightDir, normal);
			float specular_factor = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);

			// Attenuation calculation (fixed float suffixes)
			float distance = length(light.position - fragPos);
			float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

			// Get material colors
			vec3 diffuse_mat_color = getDiffuseColor(texCoord);
			vec3 specular_mat_color = getSpecularColor(texCoord);

			// Combine light and material components
			vec3 ambient = light.ambient * (uMaterial.ambient_color + diffuse_mat_color);
			vec3 diffuse = light.diffuse * diffuse_factor * diffuse_mat_color;
			vec3 specular = light.specular * specular_factor * specular_mat_color;

			// Apply attenuation
			ambient *= attenuation;
			diffuse *= attenuation;
			specular *= attenuation;

			return (ambient + diffuse + specular);
		}

		vec3 CalcSpotLight(const SpotLight light, const vec3 normal, const vec3 fragPos, const vec3 viewDir, const vec2 texCoord) {
			vec3 lightDir = normalize(light.position - fragPos);

			// Diffuse calculation
			float diffuse_factor = max(dot(normal, lightDir), 0.0f);

			// Specular calculation
			vec3 reflectDir = reflect(-lightDir, normal);
			float specular_factor = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);

			// Attenuation calculation (fixed float suffixes)
			float distance = length(light.position - fragPos);
			float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

			// Spotlight effect
			float theta = dot(lightDir, normalize(-light.direction)); // Angle between fragment-to-light and spotlight direction
			float epsilon = light.cutOff - light.outerCutOff; // Blending range
			float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f); // Smooth step for falloff

			// Get material colors
			vec3 diffuse_mat_color = getDiffuseColor(texCoord);
			vec3 specular_mat_color = getSpecularColor(texCoord);

			// Combine light and material components
			vec3 ambient = light.ambient * (uMaterial.ambient_color + diffuse_mat_color);
			vec3 diffuse = light.diffuse * diffuse_factor * diffuse_mat_color;
			vec3 specular = light.specular * specular_factor * specular_mat_color;

			// Apply attenuation and spotlight intensity
			ambient *= attenuation * intensity;
			diffuse *= attenuation * intensity;
			specular *= attenuation * intensity;

			return (ambient + diffuse + specular);
		}
	}
*/