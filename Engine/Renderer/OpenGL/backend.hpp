#pragma once

#include "../Common/common.hpp"
#include <glad/glad.h>
#include <concepts>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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
        u32 texture_unit = INT_MAX;
        u32 id = 0;
        u32 width = 0;
        u32 height = 0;
        u8* data = nullptr;
        TextureSamplerType type = TextureSamplerType::SAMPLER_2D;

        static void flip_vertically_in_place(u8* data, int width, int height);
        static Texture load_from_file(u32 texture_unit, const char* path, TextureDescription desc);
        static Texture load_from_memory(u32 texture_unit, const u8* data, int width, int height, int channels, TextureDescription desc);
        static Texture load_cube_map(u32 texture_unit, Vector<const char*> cube_map_texture_paths);
    };

    struct UniformDesc {
        GLenum type;
        int location;
    };

    struct Shader {
        unsigned int program_id = 0;
        Shader() = default;
        Shader(Vector<const char*> shader_paths) {
            this->shader_paths = shader_paths;
            this->compile();
        }

        void compile();

        void use() const;
        void set_model(Mat4 &model);
        void set_view(Mat4 &view);
        void set_projection(Mat4 &projection);

        void set_bool(const char* name, bool value);
        void set_int(const char* name, int value);
        void set_float(const char* name, float value);
        void set_texture(const char* name, Texture texture);
        void set_texture_cube(const char* name, Texture texture);
        void set_vec2(const char* name, const Vec2& value);
        void set_vec2(const char* name, float x, float y);
        void set_vec3(const char* name, const Vec3& value);
        void set_vec3(const char* name, float x, float y, float z);
        void set_vec4(const char* name, const Vec4& value);
        void set_vec4(const char* name, float x, float y, float z, float w);
        void set_mat4(const char* name, const Mat4& mat);
        void set_material(OpenGL* backend, Material* material);
    private:
        Vector<const char*> shader_paths;
        Hashmap<const char*, UniformDesc> uniforms;

        u32 create_shader_program(Vector<const char*> shader_paths);
        GLenum type_from_path(const char* path);
        void check_compile_error(unsigned int source_id, const char* path);
        unsigned int shader_source_compile(const char* path);
        unsigned int get_uniform_location(const char* name, GLenum type);
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

        void bind() {
            gl_error_check(glBindVertexArray(this->id));
        }
    };

	struct VertexBuffer {
        u32 id;
        GLenum gl_usage;
        VertexLayout layout;

        static VertexBuffer create(VertexArrayObject vao, VertexLayout layout, void* data, int count, size_t element_size, bool dynamic = true) {
            VertexBuffer ret = {};
            ret.layout = layout;
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
		void bind_pipeline(Pipeline pipeline);
		void bind_vertex_buffer(VertexBuffer vbo);
		void bind_index_buffer(IndexBuffer ebo);
		void draw_vertices(u32 vertex_base, u32 vertex_count);
		void draw_indexed(u32 index_base, u32 index_count);

		void begin_frame(u32 framebuffer = 0);
		void end_frame();
	};

    struct MeshEntry {
        GLenum draw_type = GL_TRIANGLES;
        u32 vertex_count = 0;
        u32 index_count  = 0;
        u32 vertex_base  = 0; // starting offset to next vertex in the vertex buffer
        u32 index_base   = 0; // offset to next index in the index buffer
        MaterialHandle material = MaterialHandle::invalid();
        AABB aabb = {};

        static MeshEntry create(VertexLayout layout, MaterialHandle material, Vector<Vertex>& vertex_data, Vector<u32> indices = {}, u32 vertex_base = 0, u32 index_base = 0, GLenum draw_type = GL_TRIANGLES);
    };

    // mesh is just geometry the actual material is something you submit with it
    struct Mesh {
        VertexArrayObject vao = {};
        VertexBuffer vbo = {};
        IndexBuffer ebo = {};
        Vector<MeshEntry> entries;
        AABB aabb;

        static Mesh create(MaterialHandle material, Vector<Vertex>& vertices, Vector<u32> indices = {}, GLenum draw_type = GL_TRIANGLES, u32 vertex_base = 0, u32 index_base = 0);
        static Mesh cube(MaterialHandle material);
        static Mesh axis_aligned_bounding_box(MaterialHandle material, AABB aabb);
        static Mesh axis_aligned_bounding_box(MaterialHandle material);
        static Mesh load_from_file(OpenGL* backend, ShaderHandle shader_handle, const char* path);

    private:
        Vector<Vertex> vertices;
        Vector<u32> indices;
        void process_node(Hashmap<int, MaterialHandle>& map, aiNode* node, const aiScene* scene, Mat4 parent_transform);
        MeshEntry process_mesh(Hashmap<int, MaterialHandle>& map, aiMesh* ai_mesh, const aiScene* scene, Mat4 parent_transform);
        void setup();
    };

    void resolve_requests(Vector<Request>& requests, Allocator frame_allocator) {
        Hashmap<Pipeline, Vector<DrawCallRequest>> draw_calls_map = Hashmap<Pipeline, Vector<DrawCallRequest>>(frame_allocator);
        for (Request& request : requests) {
            switch (request.type) {
                case RequestType::TEXTURE2D_LOAD: {
                    Texture& texture = this->textures.get(request.texture.user.handle);
                    texture = Texture::load_from_file(request.texture.texture_unit, request.texture.path, request.texture.description);
                } break;

                case RequestType::TEXTURE3D_LOAD: {
                    Texture& texture = this->textures.get(request.texture.user.handle);
                    texture = Texture::load_cube_map(request.texture.texture_unit, request.texture.cubemap_paths);
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
                    Shader& shader = this->shaders.get(request.vbo.user.handle);
                    shader = Shader(request.shader.shader_paths);
                } break;

                case RequestType::EBO_UPDATE: {
                    RUNTIME_ASSERT(false);
                    // Shader& shader = this->shaders.get(request.ebo.handle);
                    // shader = Shader(request.shader.shader_paths);
                } break;

                case RequestType::SHADER_CREATE: {
                    Shader& shader = this->shaders.get(request.shader.user.handle);
                    shader = Shader(request.shader.shader_paths);
                } break;

                case RequestType::SHADER_RECOMPILE: {
                    Shader& shader = this->shaders.get(request.shader.user.handle);
                    shader.compile();
                } break;

                case RequestType::MESH_LOAD: {
                    Mesh& mesh = this->meshes.get(request.mesh.user.handle);
                    mesh = Mesh::load_from_file(this, request.mesh.shader, request.mesh.path);
                } break;

                case RequestType::MESH_CUBE_CREATE: {
                    Mesh& mesh = this->meshes.get(request.mesh.user.handle);
                    mesh = Mesh::cube(request.mesh.material);
                } break;

                case RequestType::DRAW_CALL: {
                    if (draw_calls_map.has(request.draw_call.pipeline)) { 
                        draw_calls_map[request.draw_call.pipeline].append(request.draw_call);
                    } else {
                        Vector<DrawCallRequest> vector = Vector<DrawCallRequest>({request.draw_call}, frame_allocator);
                        draw_calls_map.put(request.draw_call.pipeline, vector);
                    }
                } break;

                default: {
                    RUNTIME_ASSERT(false);
                } break;
            }
        }

        // eventually do every frame_buffer as well...
        // NOTE(Jovanni): this isn't techinally right because I want to be able to order the pipelines
        // for example opaques should draw before transparent ones. ANd also transparent ones hould be sorted...
        CommandBuffer cmd = {};
        cmd.begin_frame();
            for (auto& entry : draw_calls_map) {
                Pipeline& pipeline = entry.key;
                Vector<DrawCallRequest>& draw_calls = entry.value;
                cmd.bind_pipeline(pipeline);
                for (DrawCallRequest& draw_call : draw_calls) {
                    Mesh& mesh = this->meshes.get(draw_call.mesh.handle);
                    mesh.vao.bind();
                    mesh.vbo.bind();
                    mesh.ebo.bind();
                    for (MeshEntry& mesh_entry : mesh.entries) {
                        Material& material = this->materials.get(mesh_entry.material.handle);
                        Shader& shader = this->shaders.get(material.shader.handle);
                        shader.use();
                        shader.set_material(this, &material);
                        shader.set_model(draw_call.model);
                        shader.set_view(draw_call.view);
                        shader.set_projection(draw_call.projection);

                        if (mesh_entry.index_count) {
                            this->draw_indices(mesh_entry.vertex_base, mesh_entry.index_base, mesh_entry.index_count, draw_call.instance_count);
                        } else {
                            this->draw_vertices(mesh_entry.vertex_base, mesh_entry.vertex_count, draw_call.instance_count);
                        }
                    }
                }
            }
        cmd.end_frame();
    }

    // TODO(Jovanni): For now just allow triangles, but later parameterize this? TRY INSTANCE STUF AGQAIN WIHT THE COUNT
    void draw_vertices(u32 vertex_base, u32 vertex_count, u32 instance_count = 1) {
        gl_error_check(glDrawArraysInstanced(
            GL_TRIANGLES,
            vertex_base,
            vertex_count,
            instance_count
        ));
    }

    // TODO(Jovanni): For now just allow triangles, but later parameterize this?
    void draw_indices(u32 vertex_base, u32 index_base, u32 index_count, u32 instance_count = 1) {
        gl_error_check(glDrawElementsInstancedBaseVertex(
            GL_TRIANGLES, index_count,
            GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * index_base),
            instance_count, vertex_base
        ));
    }

    s32 BOUND_VAO_ID = -1;
    s32 BOUND_PROGRAM_ID = -1;
    bool DEPTH_TEST = false;
    bool STENCIL = false;
    bool BLENDING = false;
    bool WIREFRAME = false;
    int DRAW_CALL_COUNT = 0;

    Registry<OpenGL::Texture, 256> textures = {};
    Registry<Material, 256> materials = {};
	Registry<OpenGL::Shader, 256> shaders = {};
	Registry<OpenGL::VertexArrayObject, 256> vaos = {};
	Registry<OpenGL::VertexBuffer, 256> vbos = {};
	Registry<OpenGL::IndexBuffer, 256> ebos = {};
	Registry<OpenGL::CommandBuffer, 256> command_buffers = {};
	Registry<OpenGL::MeshEntry, 256> mesh_entries = {};
	Registry<OpenGL::Mesh, 256> meshes = {};
};