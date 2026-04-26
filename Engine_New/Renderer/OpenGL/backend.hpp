#pragma once

#include "../common.hpp"
#include <glad/glad.h>

void _GL_ERROR_CHECK(const char* file, int line);
#define ENABLE_GL_ERROR_CHECK
#if defined(ENABLE_GL_ERROR_CHECK)
    #define gl_error_check(glCall) glCall; _GL_ERROR_CHECK(__FILE__, __LINE__) 
#else
    #define gl_error_check(glCall) glCall
#endif

struct OpenGL {
    s32 BOUND_VAO_ID = -1;
    s32 BOUND_PROGRAM_ID = -1;
    bool DEPTH_TEST = false;
    bool STENCIL = false;
    bool BLENDING = false;
    bool WIREFRAME = false;
    int DRAW_CALL_COUNT = 0;

    struct Texture {
        u32 id = 0;
        u32 width = 0;
        u32 height = 0;
        u8* data = nullptr; // if you use should_free = false then you must free this yourself with free_data()
        TextureSamplerType type = TextureSamplerType::SAMPLER_2D;

        static void flip_vertically_in_place(u8* data, int width, int height);
        static Texture load_from_file(const char* path, TextureDescription& desc);
        static Texture load_from_memory(const u8* data, int width, int height, int channels, TextureDescription& desc);
        static Texture load_cube_map(Vector<const char*> cube_map_texture_paths);
    };

    struct RenderState;
    struct UniformDesc {
        GLenum type;
        int location;
    };

    struct Material;
    struct Shader {
        unsigned int program_id = 0;
        Allocator allocator = Allocator::invalid();

        Shader(Allocator allocator, Vector<const char*> shader_paths) : allocator(allocator), shader_paths(shader_paths), uniforms(Hashmap<const char *, OpenGL::UniformDesc>(allocator)) {
            this->compile();
        }

        void compile();

        void use(RenderState* render_state) const;
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

    enum class BindingValueType {
        BOOL,
        INTEGER,
        FLOAT,
        SAMPLER_2D,
        CUBEMAP,
        VECTOR2,
        VECTOR3,
        VECTOR4,
        MAT4,
    };

    struct BindingValue {
        BindingValueType type;
        union {
            bool boolean_binding;
            int integer_binding;
            float float_binding;
            Texture sampler2d_binding;
            Texture cubemap_binding;
            Vec2 vector2_binding;
            Vec3 vector3_binding;
            Vec4 vector4_binding;
            Mat4 mat4_binding;
        };
    };

    struct Material {
        static constexpr const char* DIFFUSE_TEXTURE = "uDiffuseTexture"; // texture unit = aiTextureType_DIFFUSE

        Shader* shader = nullptr;
        Hashmap<const char*, BindingValue> bindings;

        Material(Allocator allocator, OpenGL::Shader* shader) : bindings(Hashmap<const char*, BindingValue>(allocator)) {
            this->shader = shader;
        }

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
    };

	struct VertexBuffer {
        u32 id;
        GLenum gl_usage;
        VertexLayout layout;

        /*
            Because of mac opengl I actually need to create the pipeline first
        */
        template<typename T>
        static VertexBuffer create(u32 vao, Vector<T>& buffer, GLenum gl_usage = GL_STATIC_DRAW) {
            VertexBuffer ret = {};
            ret.gl_usage = gl_usage;

            gl_error_check(glBindVertexArray(vao));
            gl_error_check(glGenBuffers(1, &ret.id));
            gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, ret.id));
            gl_error_check(glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(T), buffer.data(), gl_usage));

            return ret;
        }

        template<typename T>
        void update_buffer(u32 vao, Vector<T>& buffer, s64 offset = 0) {
            RUNTIME_ASSERT(gl_usage == GL_DYNAMIC_DRAW);

            gl_error_check(glBindVertexArray(vao)); // might want to cache this
            this->bind();
-
            #if 0
                void *ptr = gl_error_check(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
                size_t buffer_size = sizeof(T) * buffer.count();
                Memory::Copy(ptr, buffer_size, buffer.data(), buffer_size);
                gl_error_check(glUnmapBuffer(GL_ARRAY_BUFFER));
            #else
                gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, this->id));
                gl_error_check(glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(T) * buffer.count(), buffer.data()));
            #endif
        }

        void bind() {
            gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, this->id));
        }
	};

	struct IndexBuffer {
        u32 id;
        GLenum gl_usage;
        static IndexBuffer create(u32 vao, Vector<u32>& indices, GLenum gl_usage = GL_STATIC_DRAW) {
            IndexBuffer ret = {};
            ret.gl_usage = gl_usage;

            if (indices.count) {
                gl_error_check(glBindVertexArray(vao));
                gl_error_check(glGenBuffers(1, &ret.id));
                gl_error_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret.id));
                gl_error_check(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.count * sizeof(u32), indices.data, gl_usage));
            }

            return ret;
        }

        void bind() {
            gl_error_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
        }
	};

	struct Pipeline {
		u32 vao;
		u32 shader_program;
		RasterizerState raster;
		DepthState depth;
		BlendState blend;
		
		static Pipeline create(PipelineDescriptor& desc);
	};

	struct CommandBuffer {
		void bind_pipeline(OpenGL& backend, Handle<OpenGL::Shader> shader, Handle<OpenGL::Pipeline> pipeline_handle);
		void bind_vertex_buffer(Handle<VertexBuffer> vbo);
		void bind_index_buffer(Handle<IndexBuffer> ebo);
		void draw_vertices(u32 vertex_base, u32 vertex_count);
		void draw_indexed(u32 index_base, u32 index_count);

		void begin_frame(u32 framebuffer = 0);
		void end_frame();
	};

	OpenGL::CommandBuffer begin_frame();

	Registry<OpenGL::Texture, 256> textures;
	Registry<OpenGL::Pipeline, 256> pipelines;
};