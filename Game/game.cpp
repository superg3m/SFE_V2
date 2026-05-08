#include "game.hpp"

EXPORT_FN void application_init(EngineAPI* engine, Arena* string_arena, Hashmap<String, String>* string_intern_map) {
	engine->app = engine->memory.permanent_allocator.malloc(sizeof(AppState), alignof(AppState));
	AppState* app = (AppState*)engine->app;
	*app = {};

	TextureHandle container_texture = engine->renderer.create_texture(STR_INTERN("../../../Game/Assets/Textures/container.jpg"));
	Material& cube_material = engine->renderer.create_material();
	cube_material.set_texture(STR_INTERN(MATERIAL_ALBEDO_TEXTURE_UNIFORM_NAME), container_texture); 

	MeshHandle cube_mesh = engine->renderer.create_mesh_cube(cube_material.self);
	ModelHandle backback = engine->renderer.create_model(STR_INTERN("../../../Game/Assets/Models/Backpack/backpack.obj"), {.vertical_flip = true});
	ModelHandle glass = engine->renderer.create_model(STR_INTERN("../../../Game/Assets/Models/glass/GlassVaseFlowers.gltf"));
	ModelHandle helmet = engine->renderer.create_model(STR_INTERN("../../../Game/Assets/Models/FlightHelmet/FlightHelmet.gltf"));
	ModelHandle church = engine->renderer.create_model(STR_INTERN("../../../Game/Assets/Models/church.glb"));

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

	app->instance_cube_vbo = engine->renderer.create_vbo(cube_mesh, layout, app->cube_translations, true);

	// {right, left, top, bottom, front, back}
	#define SKYBOX_TEXTURE_PREFIX "../../../Game/Assets/Skyboxes/day_and_night"
	TextureHandle skybox_day = engine->renderer.create_texture({{
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Day/right.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Day/left.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Day/top.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Day/bottom.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Day/front.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Day/back.png"),
	}, engine->memory.frame_allocator});

	TextureHandle skybox_night = engine->renderer.create_texture({{
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Night/right.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Night/left.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Night/top.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Night/bottom.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Night/front.png"),
		STR_INTERN(SKYBOX_TEXTURE_PREFIX "/Night/back.png"),
	}, engine->memory.frame_allocator});

	app->skybox_material = engine->renderer.create_material().self;
	Material& skybox_material = engine->renderer.materials->get(app->skybox_material.handle);
	skybox_material.set_texture(STR_INTERN("uSkyboxDay"), skybox_day);
	skybox_material.set_texture(STR_INTERN("uSkyboxNight"), skybox_night);

	engine->manager.create_entity_from_model(STR_INTERN("backpack"), engine->scene.root, backback);
	engine->manager.create_entity_from_model(STR_INTERN("glass"), engine->scene.root, glass);
	engine->manager.create_entity_from_model(STR_INTERN("helmet"), engine->scene.root, helmet);
	engine->manager.create_entity_from_model(STR_INTERN("church"), engine->scene.root, church);

	Entity& cube = engine->manager.create_entity(STR_INTERN("cube"), engine->scene.root);
	Entity& skybox = engine->manager.create_entity(STR_INTERN("skybox"), engine->scene.root);

	cube.add_component<MeshComponent>(cube_mesh, app->cube_translations.count);
	skybox.add_component<SkyboxComponent>(app->skybox_material);
	app->timer.start(5.0f);
}

EXPORT_FN void application_update(EngineAPI* engine, Arena* string_arena, Hashmap<String, String>* string_intern_map, float dt) {
	AppState* app = (AppState*)engine->app;

	app->accumulator += dt * 5;
	if (app->accumulator >= 720.0f) {
		app->accumulator = 0.0f;
	}

	if (app->accumulator >= 360.0f) {
		app->sky_blend = Math::remap(app->accumulator, 360.0f, 720.0f, 1.0f, 0.0f);
	} else {
		app->sky_blend = Math::remap(app->accumulator, 0.0f, 360.0f, 0.0f, 1.0f);
	}

	if (engine->input.get_key_pressed(KEY_ESCAPE)) {
		engine->window.close = true;
	}

	if (engine->input.get_key_pressed(KEY_C)) {
		engine->window.capture_mouse = !engine->window.capture_mouse;
	}

	if (engine->input.get_key_pressed(KEY_R)) {
		// engine->renderer.recompile_dirty_shaders();
	}

	if (engine->input.get_key(KEY_SPACE, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::UP, dt);
	}

	if (engine->input.get_key(KEY_CTRL, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::DOWN, dt);
	}

	if (engine->input.get_key(KEY_W, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::FORWARD, dt); 
	}

	if (engine->input.get_key(KEY_A, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::LEFT, dt); 
	}

	if (engine->input.get_key(KEY_S, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::BACKWARD, dt);
	}

	if (engine->input.get_key(KEY_D, PRESSED|DOWN)) {
		engine->scene.active_camera.process_keyboard(CameraDirection::RIGHT, dt); 
	}
}

EXPORT_FN void application_render(EngineAPI* engine, Arena* string_arena, Hashmap<String, String>* string_intern_map, float dt) {
	AppState* app = (AppState*)engine->app;
	Mat4 model = Mat4::rotate(Mat4::identity(), Quat::from_euler(0, app->accumulator, 0));

	Material& skybox_material = engine->renderer.materials->get(app->skybox_material.handle);
	skybox_material.set_float(STR_INTERN("uBlend"), app->sky_blend);
	skybox_material.set_mat4(STR_INTERN("uModel"), model);
	
	// clear_color()
}

// https://www.youtube.com/watch?v=9R2rRLbBkHU
// https://www.youtube.com/watch?v=QAeRxfeFAo0

/*
// TODO(Jovanni): I want to reorganize the runtime.

The end goal of this project is the following:
- [] Remove mesh entries[0] hack in the editor and look at the renderer see if you can remove the render group shit?
	remove the entry_index stuff.
- [] CameraComponent
- [] maybe still consider have Texture* or Mesh* and then an OpenGL::Mesh inherits from it. SO you have most fields accessable through just the handle without going back
	to the executable. This is very nice and clean it wwould work. The only reason i'm opposed is the the new OpenGL::Mesh will be be slow I think because of 
	lack of cache locality and pointer chasing, but maybe it doesn't matter since these operations by their very natrue are very vew and far between I think?
- [] Clean up the code


- [] Render AABBS
- [x] wireframe
- [] display second camera "minimap" (should just be a screen textured-quad anchored to the top-right, small with render texture from the framebuffer)
	this is why making it manditory to pass in a frame buffer is nice! FrameBuffer {FrameBufferHandle fb, TextureHandle textrue}
- [] remove general allocator calls if possible
- [] Control the framebuffer stuff gets rendered on (make sure you can easily make framebuffers, and get their textures)
- [] Multiple Cameras (camera's as entities)
- [] Lights (should be easy?)
- [] Approaching Zero Driver Overhead in OpenGL (Check if VAO is already bound for example)
- [] transparent mesh (should be easy?)
- [] semi-transparent mesh (should just work, but maybe not?)
- [] robust rendering system (account for framebuffer objects)
	- [] texture for depth, color, light, normals
		- These shouldn't be in the shader, theres should just be thier own shader and i just swap out the shader
		that way its easy and doesn't clutter up shaders.

- [] Animations
- [] Physics (collisions, shooting)
- [] Accomplish this for the editor https://x.com/JKoukourakis/status/2001955668715282521/photo/2
- [] Replace imgui with my own GUI System
- [] See if you can make the API system a bit nicer (I don't like how error prone it is)
	basically because the api objects have data (deferred requests) you can just construct them how you would like to anywehere
- [] skybox component and draw a skybox (should be easy?)
- [-] pickable entities (probably ray based) (maybe frame buffer) (but also maybe just tree based?)
- [] Material system thats nice and hotswappable
	- [] most of this is actully a editor thing
- [] Scene system with nice lighting abilities (spotlight, sunlight, pointlights)
- [-] nice docking imgui and imguizmo
	- [] Make it stylized
- [] AS FEW Syscalls during a frame as possible!!!
- [] Profile and code instrumentation in imgui

- [] Remove all constructors except for containers and other places it makes sense, in those places make sure you have a default constructor
- [] Make sure you have as few general allocation as possible I'm very very concerned about allocations across dll boundaries
- [] AABB render (*maybe you can do this by sying draw mesh, but only this range of entries? Little bit hacky but works)
- [] 3d grid of lines (should be simple?)
- [] inside the shader directory I should have a manifest that keep track of the last modified time (so I can not have to recompile all shaders) (just the dirty ones)
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