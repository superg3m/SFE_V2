#pragma once

#include "../../Core/core.hpp"
#include "../../../Public/api.hpp"
#include <concepts>
#include <vendor.hpp>

void _GL_ERROR_CHECK(const char* file, int line);
#define ENABLE_GL_ERROR_CHECK
#if defined(ENABLE_GL_ERROR_CHECK)
	#define gl_error_check(glCall) glCall; _GL_ERROR_CHECK(__FILE__, __LINE__) 
#else
	#define gl_error_check(glCall) glCall
#endif

void bind_vertex_attribute(int location, u32 stride, VertexAttribute attribute);

struct OpenGL {
	struct Texture {
		u32 id = 0;
		u32 width = 0;
		u32 height = 0;
		u8* data = nullptr;
		TextureSamplerType type = TextureSamplerType::SAMPLER_2D;

		static void flip_vertically_in_place(u8* data, int width, int height);
		static Texture load_from_file(String path, TextureDescription desc);
		static Texture load_from_memory(const u8* data, int width, int height, int channels, TextureDescription desc);
		static Texture load_cube_map(Vector<String> cube_map_texture_paths);
	};

	struct UniformDesc {
		GLenum type;
		int location;
	};

	struct Shader {
		static Vector<String> fallback_paths;

		unsigned int program_id = 0;
		bool using_fallback = false;
		Vector<String> shader_paths;
		Hashmap<String, UniformDesc> uniforms;
	
		static Shader create(Vector<String> shader_paths) {
			Shader ret = {};
			ret.uniforms = Hashmap<String, UniformDesc>(shader_paths.allocator);
			ret.shader_paths = shader_paths;
			ret.compile();

			return ret;
		}

		void compile();

		void use() const;
		void set_model(Mat4 &model);
		void set_view(Mat4 &view);
		void set_projection(Mat4 &projection);

		void set_bool(String name, bool value);
		void set_int(String name, int value);
		void set_float(String name, float value);
		void set_texture(String name, int texture_unit, Texture texture);
		void set_texture_cube(String name, int texture_unit, Texture texture);
		void set_vec2(String name, const Vec2& value);
		void set_vec2(String name, float x, float y);
		void set_vec3(String name, const Vec3& value);
		void set_vec3(String name, float x, float y, float z);
		void set_vec4(String name, const Vec4& value);
		void set_vec4(String name, float x, float y, float z, float w);
		void set_mat4(String name, const Mat4& mat);
		void set_material(OpenGL* backend, Material* material);
	private:
		u32 create_shader_program(Vector<String> shader_paths);
		GLenum type_from_path(String path);
		void check_compile_error(unsigned int source_id, String path);
		unsigned int shader_source_compile(String path);
		unsigned int get_uniform_location(String name, GLenum type);
	};

	struct VertexArrayObject {
		u32 id;

		static VertexArrayObject create() {
			VertexArrayObject ret = {};
			gl_error_check(glGenVertexArrays(1, &ret.id));
			gl_error_check(glBindVertexArray(ret.id));
			gl_error_check(glBindVertexArray(0));

			return ret;
		}

		// TODO(Jovanni): Check if the correct vao is already bound, reduce the driver overhead
		void bind() {
			gl_error_check(glBindVertexArray(this->id));
		}
	};

	struct VertexBuffer {
		u32 id;
		GLenum gl_usage;

		static VertexBuffer create(VertexArrayObject vao, VertexLayout layout, void* data, int count, size_t element_size, bool dynamic = true) {
			VertexBuffer ret = {};
			ret.gl_usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

			gl_error_check(glBindVertexArray(vao.id));
			gl_error_check(glGenBuffers(1, &ret.id));
			gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, ret.id));
			gl_error_check(glBufferData(GL_ARRAY_BUFFER, count * element_size, data, ret.gl_usage));

			for (VertexAttribute attribute : layout.attributes) {
				bind_vertex_attribute(attribute.location, layout.stride, attribute);
			}

			gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, 0));

			return ret;
		}

		template<typename T>
		void update_buffer(VertexArrayObject vao, Vector<T>& buffer, s64 offset = 0) {
			RUNTIME_ASSERT(this->gl_usage == GL_DYNAMIC_DRAW);

			gl_error_check(glBindVertexArray(vao.id)); // might want to cache this
			gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, this->id));

			#if 0
				void *ptr = gl_error_check(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
				size_t buffer_size = sizeof(T) * buffer.count();
				Memory::Copy(ptr, buffer_size, buffer.data(), buffer_size);
				gl_error_check(glUnmapBuffer(GL_ARRAY_BUFFER));
			#else
				gl_error_check(glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(T) * buffer.count, buffer.data));
			#endif
		}

		void bind() {
			gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, this->id));
		}
	};

	struct IndexBuffer {
		u32 id;
		GLenum gl_usage;
		static IndexBuffer create(VertexArrayObject vao, Vector<u32>& indices, bool dynamic = false) {
			IndexBuffer ret = {};
			ret.gl_usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

			if (indices.count) {
				gl_error_check(glBindVertexArray(vao.id));
				gl_error_check(glGenBuffers(1, &ret.id));
				gl_error_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret.id));
				gl_error_check(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.count * sizeof(u32), indices.data, ret.gl_usage));

				gl_error_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
			}

			return ret;
		}
		
	
		void bind() {
			gl_error_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
		}
	};

	struct CommandBuffer {
		// void bind_pipeline(Pipeline pipeline);
		void bind_rasterizer_description(RasterizerDescription desc);
		void bind_vertex_buffer(VertexBuffer vbo);
		void bind_index_buffer(IndexBuffer ebo);
		void draw_vertices(u32 vertex_base, u32 vertex_count);
		void draw_indexed(u32 index_base, u32 index_count);

		void begin_frame(u32 framebuffer = 0);
		void end_frame();
	};

	// Ok so I can get away with not having Submeshes because assimp has the preprocess geometry flag, I think.
	struct Mesh {
		MeshHandle self = MeshHandle::invalid();
		VertexArrayObject vao = {};
		VertexBuffer vbo = {};
		IndexBuffer ebo = {};

		GLenum draw_type = GL_TRIANGLES;
		u32 vertex_count  = 0;
		u32 index_count   = 0;
		MaterialHandle material = MaterialHandle::invalid();
		String name;
		AABB aabb;

		static Mesh create(MaterialHandle material, Vector<Vertex>& vertices, Vector<u32> indices = {}, GLenum draw_type = GL_TRIANGLES, u32 vertex_base = 0, u32 index_base = 0);
		static Mesh cube(MaterialHandle material);
		static Mesh skybox_cube(MaterialHandle material);
		static Mesh axis_aligned_bounding_box(MaterialHandle material, AABB aabb);
		static Mesh axis_aligned_bounding_box(MaterialHandle material);
		void setup(Vector<Vertex>& vertices, Vector<u32>& indices);
	};

	struct Model {
		Vector<Mesh> meshes; // these have the MeshComponents

		// TODO(Jovanni):
		// Vector<MaterialHandle>

		static Model load_from_file(OpenGL* backend, String path, TextureDescription desc);
	private:
		void process_node(OpenGL* backend, Hashmap<int, MaterialHandle>& map, aiNode* node, const aiScene* scene, Mat4 parent_transform);
		Mesh process_mesh(OpenGL* backend, Hashmap<int, MaterialHandle>& map, aiMesh* ai_mesh, const aiScene* scene, Mat4 parent_transform);
	};

	struct RenderGroup {
		Mesh mesh;
		Mat4 model;
		int instance_count;
		RasterizerDescription desc;
	};

	// TODO(Jovanni): I should pass in the camera and lights
	void execute_defered_request(EngineAPI* engine) {
		Vector<RenderGroup> opaque_draw_calls = Vector<RenderGroup>(engine->memory.frame_allocator);
		Vector<RenderGroup> translucent_draw_calls = Vector<RenderGroup>(engine->memory.frame_allocator);
		Vector<DrawSkyboxRequest> skyboxes = Vector<DrawSkyboxRequest>(engine->memory.frame_allocator);
		Vector<DrawCallRequest> aabbs = Vector<DrawCallRequest>(engine->memory.frame_allocator);

		Mat4 view = engine->get_view_matrix();
		Mat4 projection = engine->get_projection_matrix();

		for (RenderRequest& request : engine->renderer.deferred_requests) {
			switch (request.type) {
				case RequestType::TEXTURE2D_LOAD: {
					Texture& texture = this->textures.get(request.texture.user.handle);
					texture = Texture::load_from_file(request.texture.path, request.texture.description);
				} break;

				case RequestType::TEXTURE3D_LOAD: {
					Texture& texture = this->textures.get(request.texture.user.handle);
					texture = Texture::load_cube_map(request.texture.cubemap_paths);
				} break;

				case RequestType::VBO_BIND: {
					VertexBuffer& vbo_slot = this->vbos.get(request.vbo.user.handle);
					Mesh& mesh_slot = this->meshes.get(request.vbo.mesh.handle);
					vbo_slot = VertexBuffer::create(mesh_slot.vao, request.vbo.layout, request.vbo.data, request.vbo.count, request.vbo.element_size);
				} break;

				case RequestType::VBO_CREATE: {
					VertexBuffer& vbo_slot = this->vbos.get(request.vbo.user.handle);
					Mesh& mesh_slot = this->meshes.get(request.vbo.mesh.handle);
					vbo_slot = VertexBuffer::create(mesh_slot.vao, request.vbo.layout, request.vbo.data, request.vbo.count, request.vbo.element_size);
				} break;

				case RequestType::VBO_UPDATE: {
					RUNTIME_ASSERT(false);
					// Shader& shader = this->shaders.get(request.vbo.user.handle);
					// shader = Shader::create(request.shader.shader_paths);
				} break;

				case RequestType::EBO_UPDATE: {
					RUNTIME_ASSERT(false);
					// Shader& shader = this->shaders.get(request.ebo.handle);
					// shader = Shader(request.shader.shader_paths);
				} break;

				// TODO(Jovanni): make a funciton thats just recompile_dirty_shaders()
				/*
				case RequestType::SHADER_RECOMPILE: {
					Shader& shader = this->shaders.get(request.shader.user.handle);
					shader.compile();
				} break;
				*/

				case RequestType::MODEL_LOAD: {
					Model& mesh = this->models.get(request.model.user.handle);
					mesh = Model::load_from_file(this, request.model.path, request.model.texture_description);
				} break;

				case RequestType::MESH_CUBE_CREATE: {
					Mesh& mesh = this->meshes.get(request.mesh.user.handle);
					mesh = Mesh::cube(request.mesh.material);
				} break;
	
				case RequestType::MATERIAL_SET_UNIFORM: {
					Material& material_slot = this->materials.get(request.material.user.handle);
					material_slot.set_uniform(request.material.name, request.material.value);
				} break;

				case RequestType::DRAW_CALL: {
					Mesh& mesh_slot = this->meshes.get(request.draw_call.mesh.handle);
					Material& material = this->materials.get(mesh_slot.material.handle);

					RenderGroup group = {};
					group.model = request.draw_call.model;
					group.mesh = mesh_slot;
					group.instance_count = request.draw_call.instance_count;
					group.desc = request.draw_call.rasterizer_description;

					bool translucent = material.opacity < 1.0f;
					if (translucent) {
						translucent_draw_calls.append(group);
					} else {
						opaque_draw_calls.append(group);
					}
				} break;

				case RequestType::DRAW_AABB: {
					aabbs.append(request.draw_call);
				} break;

				case RequestType::DRAW_SKYBOX: {
					skyboxes.append(request.draw_skybox);
				} break;
				
				// TODO(Jovanni):
				// DRAW_SKYBOX (just supply a texture 3d everything else I should be able to handle I think?)

				default: {
					RUNTIME_ASSERT(false);
				} break;
			}
		}

		LOCAL_PERSIST Shader pbr_shader = Shader::create({STR("../../../Game/Assets/Shaders/pbr.vert"), STR("../../../Game/Assets/Shaders/pbr.frag")});
		LOCAL_PERSIST Shader pbr_instanced_shader = Shader::create({STR("../../../Game/Assets/Shaders/pbr_instanced.vert"), STR("../../../Game/Assets/Shaders/pbr.frag")});
		
		LOCAL_PERSIST Shader skybox_shader = Shader::create({STR("../../../Game/Assets/Shaders/skybox.vert"), STR("../../../Game/Assets/Shaders/skybox.frag")});
		LOCAL_PERSIST Material& skybox_material = this->materials.get(this->materials.acquire());
		LOCAL_PERSIST Mesh skybox_mesh = Mesh::skybox_cube(skybox_material.self);

		LOCAL_PERSIST Shader aabb_shader = Shader::create({STR("../../../Game/Assets/Shaders/fallback.vert"), STR("../../../Game/Assets/Shaders/fallback.frag")});
		LOCAL_PERSIST Mesh aabb_mesh = Mesh::axis_aligned_bounding_box(MaterialHandle::invalid());

		// eventually do every framebuffers as well...
		CommandBuffer cmd = {}; 
		cmd.begin_frame();
			// NOTE(Jovanni): Draw opaques
			{
				for (RenderGroup& group : opaque_draw_calls) {
					bool instanced = group.instance_count > 1;
					Material& material = this->materials.get(group.mesh.material.handle);

					group.mesh.vao.bind();
					group.mesh.vbo.bind();
					group.mesh.ebo.bind();
					cmd.bind_rasterizer_description(group.desc);
					if (instanced) {
						pbr_instanced_shader.use();
						pbr_instanced_shader.set_material(this, &material);
						pbr_instanced_shader.set_model(group.model);
						pbr_instanced_shader.set_view(view);
						pbr_instanced_shader.set_projection(projection);
					} else {
						pbr_shader.use();
						pbr_shader.set_material(this, &material);
						pbr_shader.set_model(group.model);
						pbr_shader.set_view(view);
						pbr_shader.set_projection(projection);
					}

					this->draw_mesh(group.mesh, group.instance_count);
					for (int i = 0; i < 4; i++) {
						gl_error_check(glActiveTexture(GL_TEXTURE0 + i));
						glBindTexture(GL_TEXTURE_2D, 0);
					}
				}
			}
			
			{
				// TODO(Jovanni): Draw skyboxes
				glDepthFunc(GL_LEQUAL);
				Mat4 skybox_view_matrix = view;
				skybox_view_matrix.v[0].w = 0.0f;
				skybox_view_matrix.v[1].w = 0.0f;
				skybox_view_matrix.v[2].w = 0.0f;
				
				skybox_mesh.vao.bind();
				skybox_mesh.vbo.bind();
				skybox_mesh.ebo.bind();
				cmd.bind_rasterizer_description({
					.cull_enabled = false
				});

				for (DrawSkyboxRequest request : skyboxes) {
					Material& material = this->materials.get(request.material.handle);
					skybox_material = material;

					skybox_shader.use();
					skybox_shader.set_material(this, &skybox_material);
					skybox_shader.set_view(skybox_view_matrix);
					skybox_shader.set_projection(projection);

					this->draw_mesh(skybox_mesh, 1);
				}
				glDepthFunc(GL_LESS);
			}

			// Draw AABBS
			if (aabbs.count) {
				Mesh& mesh_slot = this->meshes.get(aabbs[0].mesh.handle);

				aabb_mesh.vao.bind();
				aabb_mesh.vbo.bind();
				aabb_mesh.ebo.bind();
				aabb_shader.use();
				for (DrawCallRequest request : aabbs) {
					Mat4 model = request.model * mesh_slot.aabb.to_transform_matrix4();
					aabb_shader.set_model(model);
					aabb_shader.set_view(view);
					aabb_shader.set_projection(projection);
					this->draw_mesh(aabb_mesh, 1);
				}
			}
			
			{
				// NOTE(Jovanni): Draw translucents
				if (translucent_draw_calls.count) {
					gl_error_check(glEnable(GL_BLEND));
					gl_error_check(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

					// sort_translucent_meshs_by_camera
					for (int i = 0; i < translucent_draw_calls.count; i++) {
						RenderGroup group_a = translucent_draw_calls[i];
						Vec3 a_position = Vec3(group_a.model.v[0].w, group_a.model.v[1].w, group_a.model.v[2].w); 
						float a_distance = Vec3::distance(a_position, engine->scene.active_camera.position);
						for (int j = i + 1; j < translucent_draw_calls.count; j++) {
							RenderGroup group_b = translucent_draw_calls[j];
							Vec3 b_position = Vec3(group_b.model.v[0].w, group_b.model.v[1].w, group_b.model.v[2].w); 
							float b_distance = Vec3::distance(b_position,  engine->scene.active_camera.position);

							if (a_distance > b_distance) {
								Memory::swap(translucent_draw_calls[i], translucent_draw_calls[j]);
							}
						}
					}

					for (RenderGroup& group : translucent_draw_calls) {
						bool instanced = group.instance_count > 1;
						Material& material = this->materials.get(group.mesh.material.handle);

						group.mesh.vao.bind();
						group.mesh.vbo.bind();
						group.mesh.ebo.bind();
						cmd.bind_rasterizer_description(group.desc);
						if (instanced) {
							pbr_instanced_shader.use();
							pbr_instanced_shader.set_material(this, &material);
							pbr_instanced_shader.set_model(group.model);
							pbr_instanced_shader.set_view(view);
							pbr_instanced_shader.set_projection(projection);
						} else {
							pbr_shader.use();
							pbr_shader.set_material(this, &material);
							pbr_shader.set_model(group.model);
							pbr_shader.set_view(view);
							pbr_shader.set_projection(projection);
						}

						this->draw_mesh(group.mesh, group.instance_count);
						for (int i = 0; i < 4; i++) {
							gl_error_check(glActiveTexture(GL_TEXTURE0 + i));
							glBindTexture(GL_TEXTURE_2D, 0);
						}
					}
				
					gl_error_check(glDisable(GL_BLEND));
				}
			}
		cmd.end_frame();
	}

	void draw_vertices(u32 vertex_base, u32 vertex_count, u32 instance_count = 1, u32 draw_type = GL_TRIANGLES) {
		gl_error_check(glDrawArraysInstanced(
			draw_type,
			vertex_base,
			vertex_count,
			instance_count
		));
	}

	void draw_indices(u32 vertex_base, u32 index_base, u32 index_count, u32 instance_count = 1, u32 draw_type = GL_TRIANGLES) {
		gl_error_check(glDrawElementsInstancedBaseVertex(
			draw_type, index_count,
			GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * index_base),
			instance_count, vertex_base
		));
	}

	void draw_mesh(Mesh& mesh, int instance_count) {
		if (mesh.index_count) {
			this->draw_indices(0, 0, mesh.index_count, instance_count, mesh.draw_type);
		} else {
			this->draw_vertices(0, mesh.vertex_count, instance_count, mesh.draw_type);
		}
	}

	/*
	void create_picking_frameBuffer(int WIDTH, int HEIGHT) {
        glGenFramebuffers(1, &this->picking_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, this->picking_fbo);

        glGenTextures(1, &this->picking_texture);
        glBindTexture(GL_TEXTURE_2D, this->picking_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->picking_texture, 0);

        u32 rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        
        RUNTIME_ASSERT_MSG(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "FBO setup failed\n");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
	*/

	static OpenGL create() {
		OpenGL ret = {};
		gl_error_check(glEnable(GL_DEPTH_TEST));

		return ret;
	}

	s32 BOUND_VAO_ID = -1;
	s32 BOUND_PROGRAM_ID = -1;
	bool DEPTH_TEST = false;
	bool STENCIL = false;
	bool BLENDING = false;
	bool WIREFRAME = false;
	u32  DRAW_CALL_COUNT = 0;

	u32 picking_fbo = INT_MAX; 
	u32 picking_texture = INT_MAX;

	Registry<OpenGL::Texture, 256> textures = {};
	Registry<Material, 256> materials = {};
	Registry<OpenGL::Shader, 256> shaders = {};
	Registry<OpenGL::VertexArrayObject, 256> vaos = {};
	Registry<OpenGL::VertexBuffer, 256> vbos = {};
	Registry<OpenGL::IndexBuffer, 256> ebos = {};
	Registry<OpenGL::CommandBuffer, 256> command_buffers = {};
	Registry<OpenGL::Model, 256> models = {};
	Registry<OpenGL::Mesh, 256> meshes = {};
};