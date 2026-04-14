#include <gpu_buffers.hpp>
#include <gl_error_check.hpp>

namespace OpenGL {
    void bind_vertex_attribute(int &location, bool instanced, stride_t stride, VertexElement desc) {
        bool is_integer = (desc.type == BufferStrideTypeInfo::INT) || (desc.type == BufferStrideTypeInfo::IVEC4);
        bool is_matrix = desc.type == BufferStrideTypeInfo::MAT4;
        GLenum gl_type  = is_integer ? GL_INT : GL_FLOAT;

        int max_attributes = 0;
        gl_check_error(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attributes));
        RUNTIME_ASSERT_MSG(max_attributes > location, "Location outside range");

        if (is_matrix) {
            for (int i = 0; i < 4; i++) {
                RUNTIME_ASSERT_MSG(max_attributes > location + i,
                    "You are trying to use a location that is outside of the max vertex attributes: %d",
                    max_attributes
                );

                gl_check_error(glEnableVertexAttribArray(location + i));
                gl_check_error(glVertexAttribPointer(location + i, (GLint)BufferStrideTypeInfo::VEC4, GL_FLOAT, false, stride, (void*)(desc.offset + (sizeof(glm::vec4) * i))));
                gl_check_error(glVertexAttribDivisor(location + i, instanced));
            }

            location += 4;
        } else {
            gl_check_error(glEnableVertexAttribArray(location));
            if (is_integer) {
                gl_check_error(glVertexAttribIPointer(location, (GLint)desc.type, gl_type, stride, (void*)desc.offset));
            } else {
                gl_check_error(glVertexAttribPointer(location, (GLint)desc.type, gl_type, false, stride, (void*)desc.offset));
            }

            gl_check_error(glVertexAttribDivisor(location, instanced));
            location += 1;
        }
    }

    static stride_t compute_stride_from_elements(std::vector<VertexElement>& layout) {
        stride_t stride = 0;
        for (VertexElement desc : layout) {
            stride += (stride_t)desc.type * sizeof(float);
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

    void VertexBufferObject::bind() {
        gl_check_error(glBindBuffer(GL_ARRAY_BUFFER, this->id));
    }

    void ElementBufferObject::bind() {
        gl_check_error(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
    }

    VertexArrayObject VertexArrayObject::create() {
        VertexArrayObject ret = {};
        gl_check_error(glGenVertexArrays(1, &ret.id));
        ret.bind();

        return ret;
    }

    // pass like a renderer in here to control the currently bound vao, reduce call overhead
    void VertexArrayObject::bind() {
        gl_check_error(glBindVertexArray(this->id)); // want to check if already bound
    }
}