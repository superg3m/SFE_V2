#include <gpu_buffers.hpp>
#include <gl_error_check.hpp>
#include <mesh.hpp>

namespace OpenGL {
    void bind_vertex_attribute(int &location, bool instanced, u32 stride, VertexElement desc) {
        bool is_integer = (desc.type == BufferStrideTypeInfo::INT) || (desc.type == BufferStrideTypeInfo::IVEC4);
        bool is_matrix = desc.type == BufferStrideTypeInfo::MAT4;
        GLenum gl_type  = is_integer ? GL_INT : GL_FLOAT;

        int max_attributes = 0;
        gl_error_check(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes));
        RUNTIME_ASSERT_MSG(max_attributes > location, "Location outside range");

        if (is_matrix) {
            for (int i = 0; i < 4; i++) {
                RUNTIME_ASSERT_MSG(max_attributes > location + i,
                    "You are trying to use a location that is outside of the max vertex attributes: %d",
                    max_attributes
                );

                gl_error_check(glEnableVertexAttribArray(location + i));
                gl_error_check(glVertexAttribPointer(location + i, (int)BufferStrideTypeInfo::VEC4, GL_FLOAT, false, stride, (void*)(desc.offset + (sizeof(glm::vec4) * i))));
                gl_error_check(glVertexAttribDivisor(location + i, instanced));
            }

            location += 4;
        } else {
            gl_error_check(glEnableVertexAttribArray(location));
            if (is_integer) {
                gl_error_check(glVertexAttribIPointer(location, (int)desc.type, gl_type, stride, (void*)desc.offset));
            } else {
                gl_error_check(glVertexAttribPointer(location, (int)desc.type, gl_type, false, stride, (void*)desc.offset));
            }

            gl_error_check(glVertexAttribDivisor(location, instanced));
            location += 1;
        }
    }

    static u32 compute_stride_from_elements(std::vector<VertexElement>& layout) {
        u32 stride = 0;
        for (VertexElement desc : layout) {
            stride += (u32)desc.type * sizeof(float);
        }

        return stride;
    }

    VertexLayout VertexLayout::create(std::vector<VertexElement> elements) {
        VertexLayout ret = {};
        ret.stride = compute_stride_from_elements(elements);
        ret.stride_in_floats = ret.stride / sizeof(float);
        ret.elements = elements;

        return ret;
    }

    VertexLayout& VertexLayout::PNT() {
        static VertexLayout layout = VertexLayout::create({
            {OFFSET_OF(Vertex, aPosition), BufferStrideTypeInfo::VEC3},
            {OFFSET_OF(Vertex, aNormal), BufferStrideTypeInfo::VEC3},
            {OFFSET_OF(Vertex, aTexCoord), BufferStrideTypeInfo::VEC2},
        });

        return layout;
    }

    void VBO::bind() {
        gl_error_check(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    }

    void EBO::bind() {
        gl_error_check(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
    }

    VAO VAO::create() {
        VAO ret = {};
        gl_error_check(glGenVertexArrays(1, &ret.id));
        ret.bind();

        return ret;
    }

    // pass like a renderer in here to control the currently bound vao, reduce call overhead
    void VAO::bind() {
        gl_error_check(glBindVertexArray(this->id)); // want to check if already bound
    }
}