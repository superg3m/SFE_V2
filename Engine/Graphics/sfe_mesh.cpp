

#include "sfe_mesh.hpp"
#include "sfe_graphics_api.hpp"


#include "../engine.hpp"

#include <glad/glad.h>

namespace SFE {
	Mesh::Mesh(VertexLayout layout, DS::Vector<float> &vertices) {
		this->layout = layout;
		this->vertex_count = (vertices.count() * sizeof(float)) / layout.stride;

		GraphicsAPI &graphics = Engine::GetInstance().GetGraphicsAPI();
		this->VBO = graphics.CreateVertexBuffer(vertices);

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		for (const VertexElement element : this->layout.elements) {
			glVertexAttribPointer(element.index, element.count, element.type, GL_FALSE, layout.stride, (void*)element.offset);
			glEnableVertexAttribArray(element.index);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	Mesh::Mesh(VertexLayout layout, DS::Vector<float> &vertices, DS::Vector<unsigned int> &indices) {
		this->layout = layout;
		this->vertex_count = (vertices.count() * sizeof(float)) / layout.stride;
		this->index_count = indices.count();

		GraphicsAPI &graphics = Engine::GetInstance().GetGraphicsAPI();
		this->VBO = graphics.CreateVertexBuffer(vertices);
		this->EBO = graphics.CreateIndexBuffer(indices);

		glGenVertexArrays(1, &this->VAO);
		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		for (const VertexElement element : this->layout.elements) {
			glVertexAttribPointer(element.index, element.count, element.type, GL_FALSE, layout.stride, (void*)element.offset);
			glEnableVertexAttribArray(element.index);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void Mesh::Bind() {
		glBindVertexArray(this->VAO);
	}

	void Mesh::Draw() {
		if (this->index_count) {
			glDrawElements(GL_TRIANGLES, this->index_count, GL_UNSIGNED_INT, 0);
		} else {
			glDrawArrays(GL_TRIANGLES, 0, this->vertex_count);
		}
	}
}