#pragma once

#include <vector>
#include <glad/glad.h>
#include <gl_error_check.hpp>

namespace OpenGL {
    typedef GLuint stride_t;
    enum class BufferStrideTypeInfo {
        BOOL = 1,
        INT = 1,
        FLOAT = 1, 
        VEC2 = 2,
        VEC3 = 3,
        VEC4 = 4, 
        IVEC4 = 4, 
        MAT4 = 16
    };

    struct VertexLayoutElement {
        size_t offset = 0; // offset in bytes of the actual memeber! OFFSET_OF(Vertex, aPosition)
        BufferStrideTypeInfo type;
    };

    void bind_vertex_attribute(int &location, bool instanced, stride_t stride, VertexLayoutElement desc);

    struct VertexBufferObject {
        GLuint id;
        GLenum gl_usage;
        template<typename T>
        static VertexBufferObject allocate(std::vector<VertexLayoutElement> layout, std::vector<T> buffer, GLenum gl_usage = GL_STATIC_DRAW, bool instanced = false) {
            VertexBufferObject ret = {};
            ret.gl_usage = gl_usage;

            gl_check_error(glGenBuffers(1, &ret.id));
            gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, ret.id));
            gl_check_error(glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(T), buffer.data(), gl_usage));

            int location = 0;
            for (VertexLayoutElement desc : layout) {
                bind_vertex_attribute(location, instanced, sizeof(T), desc);
            }

            return ret;
        }

        template<typename T>
        void update_buffer(std::vector<T> buffer, GLsizeiptr offset = 0) {
            RUNTIME_ASSERT(gl_usage == GL_DYNAMIC_DRAW);

            #if 0
                this->bind();
                void *ptr = gl_check_error(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
                size_t buffer_size = sizeof(T) * buffer.count();
                Memory::Copy(ptr, buffer_size, buffer.data(), buffer_size);
                gl_check_error(glUnmapBuffer(GL_ARRAY_BUFFER));
            #else
                gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, this->id));
                gl_check_error(glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(T) * buffer.count(), buffer.data()));
            #endif
        }

        void bind();
    };

    struct ElementBufferObject {
        GLuint id;
        GLenum gl_usage;
        static ElementBufferObject allocate(std::vector<GLuint> indices, GLenum gl_usage = GL_STATIC_DRAW) {
            ElementBufferObject ret = {};
            ret.gl_usage = gl_usage;

            gl_check_error(glGenBuffers(1, &ret.id));
            gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, ret.id));
            gl_check_error(glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), gl_usage));

            return ret;
        }
        void bind();
    };

    struct VertexArrayObject {
        GLuint id;
        static VertexArrayObject create(std::vector<VertexBufferObject> vbos, std::vector<ElementBufferObject> ebos = {});
        void bind_buffers(std::vector<VertexBufferObject> vbos, std::vector<ElementBufferObject> ebos = {});
        void bind();
    };
}