#pragma once

#include "../common.hpp"
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

Mat4 convert_assimp_matrix_to_glm(const aiMatrix4x4& from);
void bind_vertex_attribute(int location, u32 stride, VertexAttribute attribute);
AABB calculate_aabb(Vector<Vertex>& vertices, int base_vertex, int vertex_count);

struct OpenGL {
    struct Texture {
        u32 texture_unit = 505;
        u32 id = 0;
        u32 width = 0;
        u32 height = 0;
        u8* data = nullptr; // if you use should_free = false then you must free this yourself with free_data()
        TextureSamplerType type = TextureSamplerType::SAMPLER_2D;

        static void flip_vertically_in_place(u8* data, int width, int height);
        static Texture load_from_file(u32 texture_unit, const char* path, TextureDescription desc);
        static Texture load_from_memory(u32 texture_unit, const u8* data, int width, int height, int channels, TextureDescription desc);
        static Texture load_cube_map(u32 texture_unit, Vector<const char*> cube_map_texture_paths);
    };

    struct RenderState;
    struct UniformDesc {
        GLenum type;
        int location;
    };

    struct Material;
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
        void set_material(OpenGL::Material* material);
    private:
        Vector<const char*> shader_paths;
        Hashmap<const char*, UniformDesc> uniforms;

        u32 create_shader_program(Vector<const char*> shader_paths);
        GLenum type_from_path(const char* path);
        void check_compile_error(unsigned int source_id, const char* path);
        unsigned int shader_source_compile(const char* path);
        unsigned int get_uniform_location(const char* name, GLenum type);
    };

    struct BindingValue {
        BindingValueType type;
        union {
            bool boolean_binding;
            int integer_binding;
            float float_binding;
            Texture texture_binding; // can be sampler to cubemap
            Handle<Texture> texture_handle;
            Vec2 vector2_binding;
            Vec3 vector3_binding;
            Vec4 vector4_binding;
            Mat4 mat4_binding;
        };

        template<typename T>
        BindingValue(T value) {
            if constexpr (std::is_same_v<T, bool>) {
                this->type = BindingValueType::BOOL;
                this->boolean_binding = value;
            } else if constexpr (std::is_same_v<T, int>) {
                this->type = BindingValueType::INTEGER;
                this->integer_binding = value;
            } else if constexpr (std::is_same_v<T, float>) {
                this->type = BindingValueType::FLOAT;
                this->float_binding = value;
            } else if constexpr (std::is_same_v<T, Handle<Texture>>) {
                this->type = BindingValueType::TEXTURE_HANDLE;
                this->texture_handle = value;
            } else if constexpr (std::is_same_v<T, Texture>) {
                if (value.type == TextureSamplerType::SAMPLER_2D) {
                    this->type = BindingValueType::SAMPLER_2D;
                    this->texture_binding = value;
                } else if (value.type == TextureSamplerType::CUBEMAP_3D) {
                    this->type = BindingValueType::CUBEMAP;
                    this->texture_binding = value;
                }
            } else if constexpr (std::is_same_v<T, Vec2>) {
                this->type = BindingValueType::VECTOR2;
                this->vector2_binding = value;
            } else if constexpr (std::is_same_v<T, Vec3>) {
                this->type = BindingValueType::VECTOR3;
                this->vector3_binding = value;
            } else if constexpr (std::is_same_v<T, Vec4>) {
                this->type = BindingValueType::VECTOR4;
                this->vector4_binding = value;
            } else if constexpr (std::is_same_v<T, Mat4>) {
                this->type = BindingValueType::MAT4;
                this->mat4_binding = value;
            } else {
                STATIC_ASSERT(false, "Unsupported BindingValue type");
            }
        }
    };

    struct Material {
        static constexpr const char* DIFFUSE_TEXTURE = "uDiffuseTexture"; // texture unit = aiTextureType_DIFFUSE

        Handle<OpenGL::Shader> shader_handle = Handle<OpenGL::Shader>::invalid();
        Hashmap<const char*, BindingValue> bindings;

        Material() = default;
        Material(Handle<OpenGL::Shader> shader_handle) {
            this->shader_handle = shader_handle;
        }

        void set_uniform(const char* name, BindingValue value) {
            switch (value.type) {
                case BindingValueType::BOOL: {
                    this->set_bool(name, value.boolean_binding);
                } break;

                case BindingValueType::INTEGER: {
                    this->set_int(name, value.integer_binding);
                } break;

                case BindingValueType::FLOAT: {
                    this->set_float(name, value.float_binding);
                } break;

                case BindingValueType::TEXTURE_HANDLE: {
                    RUNTIME_ASSERT(false);
                } break;

                case BindingValueType::SAMPLER_2D:
                case BindingValueType::CUBEMAP: {
                    this->set_texture(name, value.texture_binding);
                } break;

                case BindingValueType::VECTOR2: {
                    this->set_vec2(name, value.vector2_binding);
                } break;

                case BindingValueType::VECTOR3: {
                    this->set_vec3(name, value.vector3_binding);
                } break;

                case BindingValueType::VECTOR4: {
                    this->set_vec4(name, value.vector4_binding);
                } break;

                case BindingValueType::MAT4: {
                    this->set_mat4(name, value.mat4_binding);
                } break;
            }
        }
        void set_bool(const char* name, bool value);
        void set_int(const char* name, int value);
        void set_float(const char* name, float value);
        void set_texture(const char* name, Texture texture);
        void set_vec2(const char* name, const Vec2& value);
        void set_vec2(const char* name, float x, float y);
        void set_vec3(const char* name, const Vec3& value);
        void set_vec3(const char* name, float x, float y, float z);
        void set_vec4(const char* name, const Vec4& value);
        void set_vec4(const char* name, float x, float y, float z, float w);
        void set_mat4(const char* name, const Mat4& mat);
    };

    struct Pipeline {
		RasterizerState rasterizer;
		DepthState depth;
		BlendState blend;
		
		static Pipeline create(PipelineDescriptor desc);
	};

    struct VertexLayout {
        u32 vao;
        u32 stride;
        u32 stride_in_floats; // stride / sizeof(float)
        Vector<VertexAttribute> attributes;

        static VertexLayout& PNT() {
            static VertexLayout layout = VertexLayout({
                VertexAttribute{0, OFFSET_OF(Vertex, aPosition), BufferStrideTypeInfo::VEC3, false},
                VertexAttribute{1, OFFSET_OF(Vertex, aNormal), BufferStrideTypeInfo::VEC3, false},
                VertexAttribute{2, OFFSET_OF(Vertex, aTexCoord), BufferStrideTypeInfo::VEC2, false},
            });

            return layout;
        }

        VertexLayout() = default;
        VertexLayout(Vector<VertexAttribute> attributes) {
            this->stride = 0;
            for (VertexAttribute desc : attributes) {
                this->stride += (u32)desc.type * sizeof(float);
            }

            this->stride_in_floats = this->stride / sizeof(float);
            this->attributes = attributes;
        
            glGenVertexArrays(1, &this->vao);
            glBindVertexArray(this->vao);

            glBindVertexArray(0);
        }
    };

	struct VertexBuffer {
        u32 id;
        GLenum gl_usage;

        /*
            Because of mac opengl I actually need to create the pipeline first
        */
        template<typename T>
        static VertexBuffer create(OpenGL::VertexLayout layout, Vector<T>& buffer, bool dynamic = true) {
            VertexBuffer ret = {};
            ret.gl_usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

            gl_error_check(glBindVertexArray(layout.vao));
            gl_error_check(glGenBuffers(1, &ret.id));
            gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, ret.id));
            gl_error_check(glBufferData(GL_ARRAY_BUFFER, buffer.count * sizeof(T), buffer.data, ret.gl_usage));

            for (VertexAttribute attribute : layout.attributes) {
                bind_vertex_attribute(attribute.location, layout.stride, attribute);
            }

            gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, 0));

            return ret;
        }

        template<typename T>
        void update_buffer(OpenGL::VertexLayout layout, Vector<T>& buffer, s64 offset = 0) {
            RUNTIME_ASSERT(this->gl_usage == GL_DYNAMIC_DRAW);

            gl_error_check(glBindVertexArray(layout.vao)); // might want to cache this
            gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, this->id));
-
            #if 0
                void *ptr = gl_error_check(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
                size_t buffer_size = sizeof(T) * buffer.count();
                Memory::Copy(ptr, buffer_size, buffer.data(), buffer_size);
                gl_error_check(glUnmapBuffer(GL_ARRAY_BUFFER));
            #else
                gl_error_check(glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(T) * buffer.count, buffer.data));
            #endif
        }
	};

	struct IndexBuffer {
        u32 id;
        GLenum gl_usage;
        static IndexBuffer create(OpenGL::VertexLayout layout, Vector<u32>& indices, bool dynamic = false) {
            IndexBuffer ret = {};
            ret.gl_usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

            if (indices.count) {
                gl_error_check(glBindVertexArray(layout.vao));
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
		void bind_pipeline(OpenGL::Pipeline pipeline);
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
        Handle<OpenGL::Material> material_handle = Handle<OpenGL::Material>::invalid();
        AABB aabb = {};

        static MeshEntry create(VertexLayout layout, Handle<OpenGL::Material> material_handle, Vector<Vertex>& vertex_data, Vector<u32> indices = {}, u32 vertex_base = 0, u32 index_base = 0, GLenum draw_type = GL_TRIANGLES) {
            MeshEntry ret = {};
            ret.draw_type = draw_type;
            ret.vertex_count = (vertex_data.count * sizeof(Vertex)) / layout.stride;
            ret.index_count = indices.count;
            ret.vertex_base  = vertex_base;
            ret.index_base   = index_base;
            ret.material_handle = material_handle;
            ret.aabb = calculate_aabb(vertex_data, vertex_base, ret.vertex_count);

            return ret;
        }
    };

     // mesh is just geometry the actual material is something you submit with it
    struct Mesh {
        VertexBuffer vbo = {};
        IndexBuffer ebo = {};
        Vector<MeshEntry> entries;
        AABB aabb;

        // static Mesh create(VertexLayout& layout, Vector<Vertex>& vertex_data, Vector<u32> indices = {}, GLenum draw_type = GL_TRIANGLES, u32 base_vertex = 0, u32 base_index = 0);
        static Mesh cube(Handle<Material> material_handle);
        static Mesh axis_aligned_bounding_box(AABB aabb, Handle<Material> material_handle);
        static Mesh axis_aligned_bounding_box(Handle<Material> material_handle);
        static Mesh load_from_file(OpenGL& backend, Handle<Shader> shader_handle, const char* path);

    private:
        Vector<Vertex> vertices;
        Vector<u32> indices;
        void process_node(Hashmap<int, Handle<Material>>& map, aiNode* node, const aiScene* scene, Mat4 parent_transform);
        MeshEntry process_mesh(Hashmap<int, Handle<Material>>& map, aiMesh* ai_mesh, const aiScene* scene, Mat4 parent_transform);
        void setup();
    };

    void bind_layout(OpenGL::VertexLayout layout) {
        gl_error_check(glBindVertexArray(layout.vao));
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

	Registry<OpenGL::VertexLayout, 256> layouts;
    Registry<OpenGL::Texture, 256> textures;
	Registry<OpenGL::Pipeline, 256> pipelines;
	Registry<OpenGL::Shader, 256> shaders;
	Registry<OpenGL::VertexBuffer, 256> vbos;
	Registry<OpenGL::IndexBuffer, 256> ebos;
	Registry<OpenGL::CommandBuffer, 256> command_buffers;
	Registry<OpenGL::Material, 256> materials;
	Registry<OpenGL::MeshEntry, 256> mesh_entries;
};