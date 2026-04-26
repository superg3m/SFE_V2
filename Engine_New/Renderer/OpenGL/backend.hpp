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

        Material() = default;
        Material(OpenGL::Shader* shader) {
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

    struct Pipeline {
		u32 vao;
		u32 shader_program;
		RasterizerState raster;
		DepthState depth;
		BlendState blend;
		
		static Pipeline create(PipelineDescriptor desc);
	};

	struct VertexBuffer {
        u32 id;
        GLenum gl_usage;

        /*
            Because of mac opengl I actually need to create the pipeline first
        */
        template<typename T>
        static VertexBuffer create(OpenGL::Pipeline pipeline, Vector<T>& buffer, bool dynamic = true) {
            VertexBuffer ret = {};
            ret.gl_usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

            gl_error_check(glBindVertexArray(pipeline.vao));
            gl_error_check(glGenBuffers(1, &ret.id));
            gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, ret.id));
            gl_error_check(glBufferData(GL_ARRAY_BUFFER, buffer.count * sizeof(T), buffer.data, ret.gl_usage));

            return ret;
        }

        template<typename T>
        void update_buffer(OpenGL::Pipeline pipeline, Vector<T>& buffer, s64 offset = 0) {
            RUNTIME_ASSERT(this->gl_usage == GL_DYNAMIC_DRAW);

            gl_error_check(glBindVertexArray(pipeline.vao)); // might want to cache this
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
        static IndexBuffer create(OpenGL::Pipeline pipeline, Vector<u32>& indices, bool dynamic = false) {
            IndexBuffer ret = {};
            ret.gl_usage = dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

            if (indices.count) {
                gl_error_check(glBindVertexArray(pipeline.vao));
                gl_error_check(glGenBuffers(1, &ret.id));
                gl_error_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret.id));
                gl_error_check(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.count * sizeof(u32), indices.data, ret.gl_usage));
            }

            return ret;
        }

        void bind() {
            gl_error_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
        }
	};

	struct CommandBuffer {
		void bind_pipeline(OpenGL::Pipeline pipeline, OpenGL::Shader shader);
		void bind_vertex_buffer(VertexBuffer vbo);
		void bind_index_buffer(IndexBuffer ebo);
		void draw_vertices(u32 vertex_base, u32 vertex_count);
		void draw_indexed(u32 index_base, u32 index_count);

		void begin_frame(u32 framebuffer = 0);
		void end_frame();
	};

    // TODO(Jovanni): For now just allow triangles, but later parameterize this?
    void draw_vertices(u32 vertex_base, u32 vertex_count, u32 instance_count = 0) {
        if (instance_count) {
            gl_error_check(glDrawArraysInstanced(
                GL_TRIANGLES,
                vertex_base,
                vertex_count,
                instance_count
            ));
        } else {
            gl_error_check(glDrawArrays(
                GL_TRIANGLES,
                vertex_base,
                vertex_count
            )); 
        }
    }

    // TODO(Jovanni): For now just allow triangles, but later parameterize this?
    void draw_indices(u32 vertex_base, u32 index_base, u32 index_count, u32 instance_count = 0) {
        if (instance_count) {
            gl_error_check(glDrawElementsInstancedBaseVertex(
                GL_TRIANGLES, index_count,
                GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * index_base),
                instance_count, vertex_base
            ));
        } else {
            gl_error_check(glDrawElementsBaseVertex(
                GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 
                (void*)(sizeof(u32) * index_base), 
                vertex_base
            ));
        }
    }

    s32 BOUND_VAO_ID = -1;
    s32 BOUND_PROGRAM_ID = -1;
    bool DEPTH_TEST = false;
    bool STENCIL = false;
    bool BLENDING = false;
    bool WIREFRAME = false;
    int DRAW_CALL_COUNT = 0;

    Registry<OpenGL::Texture, 256> textures;
	Registry<OpenGL::Pipeline, 256> pipelines;
	Registry<OpenGL::Shader, 256> shaders;
	Registry<OpenGL::VertexBuffer, 256> vbos;
	Registry<OpenGL::IndexBuffer, 256> ebos;
	Registry<OpenGL::CommandBuffer, 256> command_buffers;
	Registry<OpenGL::Material, 256> materials;
};