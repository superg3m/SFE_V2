#include "sfe_mesh.hpp"
#include <glad/glad.h>

void Mesh::Bind() {
	glBindVertexArray(this->VAO);
}

void Mesh::Draw() {
	if (this->vertex_count) {
		glDrawArrays(GL_TRIANGLES, 0, this->vertex_count);
	} else if (this->index_count) {
		glDrawElements(GL_TRIANGLES, this->index_count, GL_UNSIGNED_INT, 0);
	}
}