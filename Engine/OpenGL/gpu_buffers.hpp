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

    struct VertexElement {
        size_t offset = 0; // offset in bytes of the actual memeber! OFFSET_OF(Vertex, aPosition)
        BufferStrideTypeInfo type;
    };

    struct VertexLayout {
        stride_t stride;
        u32 stride_in_floats; // stride / sizeof(float)
        std::vector<VertexElement> elements;

        static VertexLayout create(std::vector<VertexElement> elements);
    };

    void bind_vertex_attribute(int &location, bool instanced, stride_t stride, VertexElement desc);

    struct VertexArrayObject {
        GLuint id;
        static VertexArrayObject create();
        void bind();
    };

    struct VertexBufferObject {
        GLuint id;
        GLenum gl_usage;
        VertexLayout layout;
        template<typename T>
        static VertexBufferObject allocate(VertexArrayObject& vao, VertexLayout layout, std::vector<T>& buffer, GLenum gl_usage = GL_STATIC_DRAW, bool instanced = false) {
            VertexBufferObject ret = {};
            ret.gl_usage = gl_usage;


            vao.bind();
            gl_check_error(glGenBuffers(1, &ret.id));
            gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, ret.id));
            gl_check_error(glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(T), buffer.data(), gl_usage));

            int location = 0;
            for (VertexElement desc : layout.elements) {
                bind_vertex_attribute(location, instanced, layout.stride, desc);
            }

            return ret;
        }

        template<typename T>
        void update_buffer(VertexArrayObject& vao, std::vector<T>& buffer, GLsizeiptr offset = 0) {
            RUNTIME_ASSERT(gl_usage == GL_DYNAMIC_DRAW);

            vao.bind();
            this->bind();

            #if 0
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
        static ElementBufferObject allocate(VertexArrayObject& vao, std::vector<GLuint>& indices, GLenum gl_usage = GL_STATIC_DRAW) {
            ElementBufferObject ret = {};
            ret.gl_usage = gl_usage;

            if (indices.size()) {
                vao.bind();
                gl_check_error(glGenBuffers(1, &ret.id));
                gl_check_error(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret.id));
                gl_check_error(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), gl_usage));
            }

            return ret;
        }

        void bind();
    };
}